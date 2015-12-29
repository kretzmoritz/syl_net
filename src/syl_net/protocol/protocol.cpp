#include "protocol.h"

namespace SylNet
{
	Protocol::Protocol()
		: m_packetLimit(0), m_sendDelay(50), m_nextSendTime(0), m_useCustomInterpolationDuration(false), m_packetId(0)
	{
	}

	Protocol::~Protocol()
	{
		clearNetworkables();
	}

	void Protocol::unregisterNetworkable(Networkable::Id _id)
	{
		auto i = m_networkables.find(_id);

		if (i != m_networkables.end())
		{
			delete i->second;
			m_networkables.erase(i);
		}
	}

	void Protocol::clearNetworkables()
	{
		for (auto i = m_networkables.begin(); i != m_networkables.end(); ++i)
		{
			delete i->second;
		}

		m_networkables.clear();
	}

	void Protocol::setSendDelay(float _seconds)
	{
		m_sendDelay = SylUtil::convertSecondsToMs(_seconds);
		SylUtil::initializeNextActivityTime(m_nextSendTime, m_sendDelay);
	}

	SylUtil::time_ms Protocol::getSendDelayMilliSeconds() const
	{
		return m_sendDelay;
	}

	float Protocol::getSendDelaySeconds() const
	{
		return getSendDelayMilliSeconds() / 1000.0f;
	}

	void Protocol::setReceivePacketLimit(unsigned int _packetLimit)
	{
		m_packetLimit = _packetLimit;
	}

	void Protocol::setDefaultInterpolationDuration()
	{
		m_useCustomInterpolationDuration = false;
	}

	void Protocol::setCustomInterpolationDuration(float _seconds)
	{
		m_customInterpolationDuration = SylUtil::convertSecondsToMs(_seconds);
		m_useCustomInterpolationDuration = true;
	}

	SylUtil::time_ms Protocol::getInterpolationDurationMilliSeconds() const
	{
		if (m_useCustomInterpolationDuration)
		{
			return m_customInterpolationDuration;
		}

		return m_sendDelay * 2;
	}

	float Protocol::getInterpolationDurationSeconds() const
	{
		return getInterpolationDurationMilliSeconds() / 1000.0f;
	}

	bool Protocol::send()
	{
		if (SylUtil::getCurrentTimeMs() < m_nextSendTime)
		{
			return false;
		}

		sf::Packet packet;
		packet << PacketType::DEFAULT;
		packet << m_packetId;

		for (auto i = m_networkables.begin(); i != m_networkables.end(); ++i)
		{
			Networkable::Id networkableId = i->first;
			Networkable* networkable = i->second;

			sf::Packet data;
			networkable->write(data);

			size_t dataSize = data.getDataSize();

			if (dataSize > 0)
			{
				packet << networkableId;
				packet << dataSize;

				packet.append(data.getData(), dataSize);
			}
		}

		bool result = false;

		if (packet.getDataSize() > sizeof(PacketType) + sizeof(PacketId) && send_packet(packet)) //We always have a packet type and id; make sure the rest of the packet isn't empty
		{
			++m_packetId;
			result = true;
		}
		
		for (auto i = m_networkables.begin(); i != m_networkables.end(); ++i)
		{
			Networkable* networkable = i->second;
			networkable->sent(result);
		}

		m_nextSendTime = SylUtil::getCurrentTimeMs() + m_sendDelay; //Waits at least delay (ms) until next send

		return result;
	}

	void Protocol::receive()
	{
		receive_packets(); //Populate multimap with valid packets

		for (auto i = m_packets.begin(); i != m_packets.end(); ++i)
		{
			ClientId clientId = i->first;
			sf::Packet& packet = i->second;

			sf::moveReadPos(packet, sizeof(PacketType)); //Ignore packet type; invalid ones have already been filtered out

			PacketId receivedPacketId;
			packet >> receivedPacketId;

			while (!packet.endOfPacket())
			{
				Networkable::Id networkableId;
				packet >> networkableId;

				size_t dataSize;
				packet >> dataSize;

				bool serialize = false;
				auto j = m_networkables.find(networkableId);

				if (j != m_networkables.end())
				{
					Networkable* networkable = j->second;

					if (clientId == BROADCAST_ID) //Allow all broadcast packets period
					{
						serialize = true;
					}
					else
					{
						auto& accessors = networkable->accessors;
						auto k = accessors.find(clientId);

						if (k != accessors.end())
						{
							auto& currentPacketId = k->second;

							if (receivedPacketId > currentPacketId) //Is packet newer than the previous one we received?
							{
								currentPacketId = receivedPacketId;
								serialize = true;
							}
						}
						else
						{
							auto pair = std::make_pair(clientId, receivedPacketId);
							accessors.insert(pair);

							serialize = true;
						}
					}

					if (serialize)
					{
						networkable->write_local(packet, dataSize);
						networkable->read(packet);
					}
				}

				if (!serialize)
				{
					sf::moveReadPos(packet, dataSize);
				}
			}
		}

		m_packets.clear(); //All packets have been processed

		for (auto i = m_networkables.begin(); i != m_networkables.end(); ++i)
		{
			Networkable* networkable = i->second;
			networkable->interpolate(getInterpolationDurationMilliSeconds());
		}
	}
};