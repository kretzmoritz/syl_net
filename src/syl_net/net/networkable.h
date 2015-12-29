///////////////////////////////////////
//  (             (                
//  )\ )     (    )\ )             
// (()/((    )\  (()/(    (   )    
//  /(_))\ )((_)  /(_))  ))\ /((   
// (_))(()/( _   (_))_  /((_|_))\  
// / __|)(_)) |   |   \(_)) _)((_) 
// \__ \ || | |   | |) / -_)\ V /  
// |___/\_, |_|   |___/\___| \_/   
//      |__/                       
//
// 2014 Moritz Kretz
///////////////////////////////////////

#ifndef SYLNET_NETWORKABLE_H
#define SYLNET_NETWORKABLE_H

#include <unordered_map>
#include <functional>
#include <SFML/Network.hpp>

#include "client.h"
#include "packet.h"
#include "../../syl_util/time/time.h"
#include "../../syl_util/math/interpolation.h"

namespace SylNet
{
	class Networkable
	{
	public:
		enum State
		{
			READ,
			WRITE,
			WRITE_LOCAL,
			INTERPOLATE,
			SEND_SUCCESS,
			SEND_FAILED,
		};

		typedef unsigned int Id;

		virtual void read(sf::Packet& _packet) = 0;
		virtual void write(sf::Packet& _packet) = 0;
		virtual void write_local(sf::Packet const& _packet, size_t _dataSize) = 0;
		virtual void interpolate(SylUtil::time_ms _interpolationDuration) = 0;
		virtual void sent(bool _success) = 0;

		std::unordered_map<ClientId, PacketId> accessors;

	protected:
		bool m_disableDefaultInterpolation;
		sf::Packet m_prevPacket;
		sf::Packet m_packet;
		SylUtil::time_ms m_interpolationStart;
	};

	template<class T>
	class NetworkableTarget : public Networkable
	{
	public:
		NetworkableTarget(T& _object, SylUtil::InterpolationFunction<T> _interpolationFunction, bool _disableDefaultInterpolation);

		virtual void read(sf::Packet& _packet);
		virtual void write(sf::Packet& _packet);
		virtual void write_local(sf::Packet const& _packet, size_t _dataSize);
		virtual void interpolate(SylUtil::time_ms _interpolationDuration);
		virtual void sent(bool _success);

	private:
		T& m_object;
		SylUtil::InterpolationFunction<T> m_interpolationFunction;
	};
}

#include "networkable.tcc"

#endif