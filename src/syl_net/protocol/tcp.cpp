#include "tcp.h"

namespace SylNet
{
	Tcp::Tcp()
		: m_handshakeTimeout(sf::seconds(1.0f)), m_heartbeatDelay(1000), m_nextHeartbeatTime(0), m_connectionTimeout(5000)
	{
	}

	Tcp::~Tcp()
	{
	}

	void Tcp::setHeartbeatDelay(float _seconds)
	{
		m_heartbeatDelay = SylUtil::convertSecondsToMs(_seconds);
		SylUtil::initializeNextActivityTime(m_nextHeartbeatTime, m_heartbeatDelay);
	}

	void Tcp::setHandshakeTimeout(float _seconds)
	{
		m_handshakeTimeout = sf::seconds(_seconds);
	}

	void Tcp::setConnectionTimeout(float _seconds)
	{
		m_connectionTimeout = SylUtil::convertSecondsToMs(_seconds);
	}

	void Tcp::setConnectCallback(ConnectionCallback _cb)
	{
		m_connectCallback = _cb;
	}

	void Tcp::setDisconnectCallback(ConnectionCallback _cb)
	{
		m_disconnectCallback = _cb;
	}

	void Tcp::heartbeat()
	{
		if (SylUtil::getCurrentTimeMs() < m_nextHeartbeatTime)
		{
			return;
		}

		sf::Packet packet;
		packet << PacketType::HEARTBEAT;
		packet << SylUtil::getCurrentTimeMs();

		send_packet(packet);

		m_nextHeartbeatTime = SylUtil::getCurrentTimeMs() + m_heartbeatDelay; //Waits at least delay (ms) until next heartbeat
	}

	bool Tcp::processHeartbeat(sf::Packet& _packet)
	{
		PacketType pt;
		_packet >> pt;

		if (pt != PacketType::HEARTBEAT)
		{
			sf::setReadPos(_packet, 0);
			return false;
		}

		SylUtil::time_ms time;
		_packet >> time;

		sf::Packet packet;
		packet << PacketType::ACK;
		packet << time;

		send_packet(packet);

		return true;
	}

	bool Tcp::processAck(sf::Packet& _packet, SylUtil::time_ms& _lastAckReceiveTime, SylUtil::time_ms& _lastLatency)
	{
		PacketType pt;
		_packet >> pt;

		if (pt != PacketType::ACK)
		{
			sf::setReadPos(_packet, 0);
			return false;
		}

		SylUtil::time_ms time;
		_packet >> time;

		_lastAckReceiveTime = SylUtil::getCurrentTimeMs();
		_lastLatency = (_lastAckReceiveTime - time) / 2;

		return true;
	}

	bool Tcp::isConnectionAlive(SylUtil::time_ms _lastAckReceiveTime)
	{
		if (m_connectionTimeout == 0 || SylUtil::getCurrentTimeMs() - _lastAckReceiveTime < m_connectionTimeout)
		{
			return true;
		}

		return false;
	}
};