#include "connection.h"

namespace SylNet
{
	Connection::Connection()
	{
	}

	Connection::Connection(sf::IpAddress _address, unsigned short _port)
		: address(_address), port(_port)
	{
	}

	bool Connection::operator<(Connection const& _other) const
	{
		if (port == _other.port)
		{
			return address < _other.address;
		}

		return port < _other.port;
	}
};