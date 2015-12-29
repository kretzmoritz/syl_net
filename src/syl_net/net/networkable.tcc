#include "networkable.h"
#include "../util/serialization.h"

namespace SylNet
{
	template<class T>
	NetworkableTarget<T>::NetworkableTarget(T& _object, SylUtil::InterpolationFunction<T> _interpolationFunction, bool _disableDefaultInterpolation)
		: m_object(_object), m_interpolationFunction(_interpolationFunction)
	{
		m_disableDefaultInterpolation = _disableDefaultInterpolation;
	}

	template<class T>
	void NetworkableTarget<T>::read(sf::Packet& _packet)
	{
		void(*serialize_func)(
			T& _object, SerializationContext const& _ctx, 
			decltype(m_interpolationFunction) const& _interpolationFunction) = serialize;
		
		SerializationContext ctx(Networkable::State::READ, _packet);
		serialize_func(m_object, ctx, nullptr);
	}

	template<class T>
	void NetworkableTarget<T>::write(sf::Packet& _packet)
	{
		void(*serialize_func)(
			T& _object, SerializationContext const& _ctx, 
			decltype(m_interpolationFunction) const& _interpolationFunction) = serialize;

		SerializationContext ctx(Networkable::State::WRITE, _packet);
		serialize_func(m_object, ctx, nullptr);
	}

	template<class T>
	void NetworkableTarget<T>::write_local(sf::Packet const& _packet, size_t _dataSize)
	{
		if (m_disableDefaultInterpolation)
		{
			return;
		}

		void(*serialize_func)(
			T& _object, SerializationContext const& _ctx, 
			decltype(m_interpolationFunction) const& _interpolationFunction) = serialize;

		m_prevPacket.clear();
		SerializationContext ctx(Networkable::State::WRITE_LOCAL, m_prevPacket);
		serialize_func(m_object, ctx, nullptr);

		m_packet.clear();
		m_packet.append(static_cast<char const*>(_packet.getData()) + sf::getReadPos(_packet), _dataSize);

		m_interpolationStart = SylUtil::getCurrentTimeMs();
	}

	template<class T>
	void NetworkableTarget<T>::interpolate(SylUtil::time_ms _interpolationDuration)
	{
		void(*serialize_func)(
			T& _object, SerializationContext const& _ctx, 
			decltype(m_interpolationFunction) const& _interpolationFunction) = serialize;

		if (m_disableDefaultInterpolation)
		{
			sf::Packet data;
			SerializationContext ctx(Networkable::State::INTERPOLATE, data);
			serialize_func(m_object, ctx, m_interpolationFunction);
		}
		else
		{
			if (m_prevPacket.getDataSize() == 0 || m_packet.getDataSize() == 0) //Don't interpolate until we receive data
			{
				return;
			}

			sf::setReadPos(m_prevPacket, 0);
			sf::setReadPos(m_packet, 0);

			float interpolationProgress = _interpolationDuration > 0 ?
				(SylUtil::getCurrentTimeMs() - m_interpolationStart) / static_cast<float>(_interpolationDuration) : 1.0f;
			SerializationContext ctx(Networkable::State::INTERPOLATE, m_prevPacket, m_packet, interpolationProgress);
			serialize_func(m_object, ctx, m_interpolationFunction);
		}
	}

	template<class T>
	void NetworkableTarget<T>::sent(bool _success)
	{
		void(*serialize_func)(
			T& _object, SerializationContext const& _ctx,
			decltype(m_interpolationFunction) const& _interpolationFunction) = serialize;

		sf::Packet data;
		SerializationContext ctx(_success ? Networkable::State::SEND_SUCCESS : Networkable::State::SEND_FAILED, data);
		serialize_func(m_object, ctx, nullptr);
	}
};