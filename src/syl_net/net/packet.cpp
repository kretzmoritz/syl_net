#include <limits>

#include "packet.h"

namespace SylNet
{
	PacketId::PacketId()
	{
	}

	PacketId::PacketId(unsigned int _id)
		: m_id(_id)
	{
	}

	PacketId& PacketId::operator++()
	{
		++m_id;
		return *this;
	}

	bool PacketId::operator>(PacketId const& _other)
	{
		if (m_id > _other.m_id)
		{
			return true;
		}

		unsigned int difference = _other.m_id - m_id;
		unsigned int limit = std::numeric_limits<unsigned int>::max();

		if (difference > limit / 2) //Wrap around check
		{
			return true;
		}

		return false;
	}
};