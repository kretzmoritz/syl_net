#include "client.h"

namespace SylNet
{
	ClientSocket::ClientSocket(
		std::unique_ptr<sf::TcpSocket>& _socket, 
		SylUtil::time_ms _lastAckReceiveTime, SylUtil::time_ms _lastLatency)
		: socket(std::move(_socket)), lastAckReceiveTime(_lastAckReceiveTime), lastLatency(_lastLatency)
	{
	}

	ClientSocket::ClientSocket(ClientSocket& _other)
	{
		*this = _other;
	}

	ClientSocket& ClientSocket::operator=(ClientSocket& _other)
	{
		socket = std::move(_other.socket);
		lastAckReceiveTime = _other.lastAckReceiveTime;
		lastLatency = _other.lastLatency;

		return *this;
	}
};