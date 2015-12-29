#include "serialization.h"

namespace SylNet
{
	SerializationContext::SerializationContext(Networkable::State _state, sf::Packet& _packet)
		: state(_state), prevPacket(_packet), packet(_packet), interpolationProgress(1.0f)
	{
	}

	SerializationContext::SerializationContext(
		Networkable::State _state, 
		sf::Packet& _prevPacket, sf::Packet& _packet, float _interpolationProgress)
		: state(_state), prevPacket(_prevPacket), packet(_packet), 
		interpolationProgress(_interpolationProgress)
	{
	}

	SerializationContext::SerializationContext(SerializationContext const& _ctx)
		: state(_ctx.state), prevPacket(_ctx.prevPacket), packet(_ctx.packet), 
		interpolationProgress(_ctx.interpolationProgress)
	{
	}
};