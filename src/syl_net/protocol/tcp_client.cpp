#include <iostream>

#include "tcp_client.h"

namespace SylNet
{
	TcpClient::TcpClient()
		: m_connected(false), m_openTimeout(10000), m_lastLatency(0)
	{
		m_socket.setBlocking(false);
	}

	TcpClient::~TcpClient()
	{
		close();
	}

	bool TcpClient::open(sf::IpAddress _address, unsigned short _port)
	{
		close();

		SylUtil::time_ms openStart = SylUtil::getCurrentTimeMs();

		do
		{
			if (m_socket.connect(_address, _port) == sf::Socket::Done)
			{
				sf::SocketSelector selector;
				selector.add(m_socket);

				if (selector.wait(m_handshakeTimeout))
				{
					sf::Packet packet;

					if (m_socket.receive(packet) == sf::Socket::Done)
					{
						if (m_socket.send(packet) == sf::Socket::Done) //Echo client id to server
						{
							packet >> m_clientId;
							m_lastAckReceiveTime = SylUtil::getCurrentTimeMs();

							m_connected = true;

							if (m_connectCallback)
							{
								m_connectCallback(*this, m_clientId, m_socket.getLocalPort(), Connection(m_socket.getRemoteAddress(), m_socket.getRemotePort()));
							}

							return true;
						}
					}
				}

				m_socket.disconnect();
				std::cout << "TcpError! handshake on address " << _address << " and port " << _port << " failed." << std::endl;

				return false;
			}
		} while ((SylUtil::getCurrentTimeMs() - openStart) < m_openTimeout);

		std::cout << "TcpError! open on address " << _address << " and port " << _port << " failed." << std::endl;

		return false;
	}

	void TcpClient::close()
	{
		if (m_connected)
		{
			disconnect();
		}
	}

	unsigned int TcpClient::getConnectionCount() const
	{
		return m_connected ? 1 : 0;
	}

	void TcpClient::setOpenTimeout(float _seconds)
	{
		m_openTimeout = SylUtil::convertSecondsToMs(_seconds);
	}

	SylUtil::time_ms TcpClient::getLastLatency() const
	{
		return m_lastLatency;
	}

	bool TcpClient::send_packet(sf::Packet& _packet)
	{
		sf::Socket::Status state;

		if (m_connected && (state = m_socket.send(_packet)) == sf::Socket::Disconnected)
		{
			disconnect();
		}

		return state == sf::Socket::Done;
	}

	void TcpClient::receive_packets()
	{
		if (m_connected)
		{
			sf::Packet packet;
			sf::Socket::Status state;
			unsigned int packetCount = 0;

			while ((state = m_socket.receive(packet)) == sf::Socket::Done)
			{
				if (!(processHeartbeat(packet) || processAck(packet, m_lastAckReceiveTime, m_lastLatency)))
				{
					m_packets.insert(std::make_pair(m_clientId, packet));
				}

				if (m_packetLimit > 0 && ++packetCount >= m_packetLimit)
				{
					break;
				}
			}

			if (state == sf::Socket::Disconnected || !isConnectionAlive(m_lastAckReceiveTime))
			{
				disconnect();
			}
			else
			{
				heartbeat();
			}
		}
	}

	void TcpClient::disconnect()
	{
		if (m_disconnectCallback)
		{
			m_disconnectCallback(*this, m_clientId, m_socket.getLocalPort(), Connection(m_socket.getRemoteAddress(), m_socket.getRemotePort()));
		}

		m_socket.disconnect();

		m_packets.clear();

		for (auto i = m_networkables.begin(); i != m_networkables.end(); ++i)
		{
			Networkable* networkable = i->second;
			networkable->accessors.clear();
		}

		m_lastLatency = 0;
		m_connected = false;
	}
};