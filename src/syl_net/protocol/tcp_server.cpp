#include <iostream>

#include "tcp_server.h"

namespace SylNet
{
	TcpServer::TcpServer()
		: m_blockingAccept(false), m_maxClients(0), m_lowestUnusedClientId(0), m_syncAll(true)
	{
		m_listener.setBlocking(false);

		m_socket = std::make_unique<sf::TcpSocket>();
		m_socket->setBlocking(false);
	}

	TcpServer::~TcpServer()
	{
		listen_end();
		close_all();
	}

	bool TcpServer::listen_start(unsigned short _port, unsigned int _minClients, unsigned int _maxClients)
	{
		if (_minClients > MAX_CLIENT_ID)
		{
			std::cout << "TcpError! minimum client count out of bounds." << std::endl;
			return false;
		}

		if (_maxClients > MAX_CLIENT_ID)
		{
			std::cout << "TcpError! maximum client count out of bounds." << std::endl;
			return false;
		}

		if (_maxClients > 0 && _minClients > _maxClients)
		{
			std::cout << "TcpError! minimum client count > maximum client count." << std::endl;
			return false;
		}

		if (m_listener.listen(_port) != sf::Socket::Done)
		{
			std::cout << "TcpError! listen_start on port " << _port << " failed." << std::endl;
			return false;
		}

		m_blockingAccept = true;
		m_maxClients = _maxClients;

		while (m_clients.size() < _minClients)
		{
			accept();
		}

		m_blockingAccept = false;

		return true;
	}

	void TcpServer::listen_end()
	{
		m_listener.close();
	}

	void TcpServer::accept()
	{
		if (m_maxClients > 0 && m_clients.size() >= m_maxClients)
		{
			return;
		}

		if (m_blockingAccept)
		{
			do
			{
				receive_packets();
			} while (m_listener.accept(*m_socket) != sf::Socket::Done);
		}
		else if (m_listener.accept(*m_socket) != sf::Socket::Done)
		{
			return;
		}

		ClientId clientId = getLowestUnusedClientId();

		if (clientId > MAX_CLIENT_ID)
		{
			m_socket->disconnect();
			return;
		}

		sf::Packet packet;
		packet << clientId;

		if (m_socket->send(packet) != sf::Socket::Done)
		{
			m_socket->disconnect();
			return;
		}

		sf::SocketSelector selector;
		selector.add(*m_socket);

		if (selector.wait(m_handshakeTimeout))
		{
			if (m_socket->receive(packet) != sf::Socket::Done)
			{
				m_socket->disconnect();
				return;
			}

			ClientId echo;
			packet >> echo;

			if (echo != clientId) //Wrong echo
			{
				m_socket->disconnect();
				return;
			}

			m_usedClientIds.insert(clientId);
			++m_lowestUnusedClientId;

			unsigned short localPort = m_socket->getLocalPort();
			Connection c(m_socket->getRemoteAddress(), m_socket->getRemotePort());

			m_clients.insert(std::make_pair(clientId, ClientSocket(m_socket, SylUtil::getCurrentTimeMs(), 0)));

			m_socket = std::make_unique<sf::TcpSocket>(); //Allow a new client to connect
			m_socket->setBlocking(false);

			std::cout << "New client (" << clientId << ") connected: " << c.address << ":" << c.port << std::endl;

			if (m_connectCallback)
			{
				m_connectCallback(*this, clientId, localPort, c);
			}

			if (m_clients.size() == m_maxClients && m_capacityReachedCallback)
			{
				m_capacityReachedCallback(*this);
			}

			return;
		}

		m_socket->disconnect();
	}

	void TcpServer::close_all()
	{
		for (auto i = m_clients.begin(); i != m_clients.end(); ++i)
		{
			ClientId clientId = i->first;
			ClientSocket& clientSocket = i->second;

			disconnect(clientId, clientSocket.socket.get());
		}

		m_clients.clear();
	}

	void TcpServer::close(unsigned int _clientId)
	{
		auto i = m_clients.find(_clientId);

		if (i != m_clients.end())
		{
			ClientId clientId = i->first;
			ClientSocket& clientSocket = i->second;

			disconnect(clientId, clientSocket.socket.get());
			m_clients.erase(i);
		}
	}

	void TcpServer::sync_all()
	{
		m_syncAll = true;
	}

	void TcpServer::sync(ClientId _clientId)
	{
		m_syncTarget = _clientId;
		m_syncAll = false;
	}

	unsigned int TcpServer::getConnectionCount() const
	{
		return m_clients.size();
	}

	void TcpServer::setCapacityReachedCallback(CapacityReachedCallback _cb)
	{
		m_capacityReachedCallback = _cb;
	}

	SylUtil::time_ms TcpServer::getLastLatency(ClientId _clientId) const
	{
		auto i = m_clients.find(_clientId);

		if (i != m_clients.end())
		{
			ClientSocket const& clientSocket = i->second;
			return clientSocket.lastLatency;
		}

		return 0;
	}

	bool TcpServer::send_packet(sf::Packet& _packet)
	{
		bool result = false;

		if (!m_syncAll)
		{
			auto i = m_clients.find(m_syncTarget);

			if (i != m_clients.end())
			{
				ClientId clientId = i->first;
				ClientSocket& clientSocket = i->second;

				sf::Socket::Status state;

				if ((state = clientSocket.socket->send(_packet)) == sf::Socket::Disconnected)
				{
					disconnect(clientId, clientSocket.socket.get());
					m_clients.erase(i);
				}
				else if (state == sf::Socket::Done)
				{
					result = true;
				}
			}
		}
		else
		{
			for (auto i = m_clients.begin(); i != m_clients.end();)
			{
				ClientId clientId = i->first;
				ClientSocket& clientSocket = i->second;

				sf::Socket::Status state;

				if ((state = clientSocket.socket->send(_packet)) == sf::Socket::Disconnected)
				{
					disconnect(clientId, clientSocket.socket.get());
					i = m_clients.erase(i);

					continue;
				}
				else if (state == sf::Socket::Done)
				{
					result = true;
				}

				++i;
			}
		}

		return result;
	}

	void TcpServer::receive_packets()
	{
		sf::Packet packet;
		sf::Socket::Status state;
		unsigned int packetCount;

		for (auto i = m_clients.begin(); i != m_clients.end();)
		{
			ClientId clientId = i->first;
			ClientSocket& clientSocket = i->second;

			packetCount = 0;

			while ((state = clientSocket.socket->receive(packet)) == sf::Socket::Done)
			{
				if (!(processHeartbeat(packet, clientId) || processAck(packet, clientSocket.lastAckReceiveTime, clientSocket.lastLatency)))
				{
					m_packets.insert(std::make_pair(clientId, packet));
				}

				if (m_packetLimit > 0 && ++packetCount >= m_packetLimit)
				{
					break;
				}
			}

			if (state == sf::Socket::Disconnected || !isConnectionAlive(clientSocket.lastAckReceiveTime))
			{
				disconnect(clientId, clientSocket.socket.get());
				i = m_clients.erase(i);

				continue;
			}

			++i;
		}

		heartbeat();
	}

	void TcpServer::heartbeat()
	{
		bool previousSyncAll = m_syncAll;

		sync_all();
		Tcp::heartbeat();

		m_syncAll = previousSyncAll;
	}

	bool TcpServer::processHeartbeat(sf::Packet& _packet, ClientId _clientId)
	{
		bool previousSyncAll = m_syncAll;
		ClientId previousSyncTarget = m_syncTarget;

		sync(_clientId);
		bool result = Tcp::processHeartbeat(_packet);

		m_syncAll = previousSyncAll;
		m_syncTarget = previousSyncTarget;

		return result;
	}

	ClientId TcpServer::getLowestUnusedClientId()
	{
		auto i = m_usedClientIds.lower_bound(m_lowestUnusedClientId);

		while (i != m_usedClientIds.end() && *i == m_lowestUnusedClientId)
		{
			++m_lowestUnusedClientId;
			++i;
		}

		return m_lowestUnusedClientId;
	}

	void TcpServer::disconnect(ClientId _clientId, sf::TcpSocket* _socket)
	{
		Connection c(_socket->getRemoteAddress(), _socket->getRemotePort());

		if (m_disconnectCallback)
		{
			m_disconnectCallback(*this, _clientId, _socket->getLocalPort(), c);
		}

		_socket->disconnect();

		if (_clientId < m_lowestUnusedClientId)
		{
			m_lowestUnusedClientId = _clientId;
		}

		m_usedClientIds.erase(_clientId);
		m_packets.erase(_clientId);

		for (auto i = m_networkables.begin(); i != m_networkables.end(); ++i)
		{
			Networkable* networkable = i->second;
			networkable->accessors.erase(_clientId);
		}

		std::cout << "Client (" << _clientId << ") disconnected: " << c.address << ":" << c.port << std::endl;
	}
};