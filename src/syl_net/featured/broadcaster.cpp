#include "broadcaster.h"

namespace SylNet
{
	Broadcaster::Broadcaster(std::string _serverName, unsigned short _port, unsigned short _broadcastPort, float _sendDelaySeconds)
		: m_serverName(_serverName), m_connection(sf::IpAddress::getLocalAddress(), _port)
	{
		m_udp.addConnection(0, Connection(sf::IpAddress::Broadcast, _broadcastPort));
		m_udp.setSendDelay(_sendDelaySeconds);
		m_udp.registerNetworkable(0, *this);
	}

	void Broadcaster::setServerName(std::string _serverName)
	{
		m_serverName = _serverName;
	}

	void Broadcaster::setPort(unsigned short _port)
	{
		m_connection.port = _port;
	}

	void Broadcaster::setBroadcastPort(unsigned short _broadcastPort)
	{
		m_udp.clearConnections();
		m_udp.addConnection(0, Connection(sf::IpAddress::Broadcast, _broadcastPort));
	}

	void Broadcaster::setSendDelay(float _seconds)
	{
		m_udp.setSendDelay(_seconds);
	}

	void Broadcaster::send()
	{
		m_udp.send();
	}

	void serialize(
		Broadcaster& _object, SerializationContext const& _ctx,
		SylUtil::InterpolationFunction<Broadcaster> const& _interpolationFunction)
	{
		serialize(_object.m_serverName, _ctx);
		serialize(_object.m_connection, _ctx);
	}
};