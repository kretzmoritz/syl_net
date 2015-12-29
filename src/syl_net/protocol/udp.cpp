#include <iostream>

#include "udp.h"

namespace SylNet
{
	Udp::Udp()
		: m_acceptPacketsFromUnknownConnections(false)
	{
		m_socket.setBlocking(false);
	}

	Udp::~Udp()
	{
	}

	bool Udp::bind(unsigned short _port)
	{
		if (m_socket.bind(_port) != sf::Socket::Done)
		{
			std::cout << "UdpError! bind on port " << _port << " failed." << std::endl;
			return false;
		}

		return true;
	}

	void Udp::unbind()
	{
		m_socket.unbind();
	}

	unsigned short Udp::getLocalPort() const
	{
		return m_socket.getLocalPort();
	}

	bool Udp::addConnection(ClientId _clientId, Connection _connection)
	{
		if (_clientId > MAX_CLIENT_ID)
		{
			std::cout << "UdpError! clientId out of bounds." << std::endl;
			return false;
		}

		if (m_connections.find(_clientId) != m_connections.end())
		{
			std::cout << "UdpError! addConnection failed because clientId " << _clientId 
				<< " is already registered." << std::endl;
			return false;
		}

		if (m_ids.find(_connection) != m_ids.end())
		{
			std::cout << "UdpError! addConnection failed because connection " << _connection.address << ":" << _connection.port 
				<< " is already registered." << std::endl;
			return false;
		}

		auto pair0 = std::make_pair(_clientId, _connection);
		m_connections.insert(pair0);

		auto pair1 = std::make_pair(_connection, _clientId);
		m_ids.insert(pair1);

		return true;
	}

	void Udp::removeConnection(ClientId _clientId)
	{
		auto i = m_connections.find(_clientId);

		if (i != m_connections.end())
		{
			Connection connection = i->second;

			m_ids.erase(connection);
			m_connections.erase(_clientId);
			m_packets.erase(_clientId);

			for (auto j = m_networkables.begin(); j != m_networkables.end(); ++j)
			{
				Networkable* networkable = j->second;
				networkable->accessors.erase(_clientId);
			}
		}
	}

	void Udp::clearConnections()
	{
		for (auto i = m_connections.begin(); i != m_connections.end(); ++i) //Don't erase packets received from unknown connections
		{
			ClientId clientId = i->first;
			m_packets.erase(clientId);
		}

		m_connections.clear();
		m_ids.clear();

		for (auto i = m_networkables.begin(); i != m_networkables.end(); ++i)
		{
			Networkable* networkable = i->second;
			networkable->accessors.clear();
		}
	}

	unsigned int Udp::getConnectionCount() const
	{
		return m_connections.size();
	}

	void Udp::acceptPacketsFromUnknownConnections(bool _value)
	{
		m_acceptPacketsFromUnknownConnections = _value;
	}

	bool Udp::send_packet(sf::Packet& _packet)
	{
		bool result = false;

		for (auto i = m_connections.begin(); i != m_connections.end(); ++i)
		{
			sf::IpAddress address = i->second.address;
			unsigned short port = i->second.port;

			if (m_socket.send(_packet, address, port) == sf::Socket::Done)
			{
				result = true;
			}
		}

		return result;
	}

	void Udp::receive_packets()
	{
		sf::Packet packet;
		sf::IpAddress address;
		unsigned short port;
		unsigned int packetCount = 0;

		while (m_socket.receive(packet, address, port) == sf::Socket::Done)
		{
			Connection connection(address, port);
			auto i = m_ids.find(connection);

			if (i != m_ids.end())
			{
				ClientId clientId = i->second;
				auto pair = std::make_pair(clientId, packet);
				
				m_packets.insert(pair);
			}
			else if (m_acceptPacketsFromUnknownConnections)
			{
				auto pair = std::make_pair(BROADCAST_ID, packet);
				m_packets.insert(pair);
			}

			if (m_packetLimit > 0 && ++packetCount >= m_packetLimit)
			{
				break;
			}
		}
	}
};