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

#ifndef SYLNET_SERIALIZATION_H
#define SYLNET_SERIALIZATION_H

#include <functional>
#include <SFML/Network.hpp>

#include "../net/networkable.h"

namespace SylNet
{
	struct SerializationContext
	{
		template<class T> friend class NetworkableTarget;

	public:
		/**
		* The current internal serialization state.
		* Can be used to decide whether send (WRITE) or receive (READ) is happening.
		* The result of send will either be SEND_SUCCESSFUL or SEND_FAILED.
		* WRITE_LOCAL is a special state used to save internal values needed for interpolation.
		*/
		Networkable::State const state;
		sf::Packet& prevPacket;
		sf::Packet& packet;
		float const interpolationProgress;

	private:
		SerializationContext(Networkable::State _state, sf::Packet& _packet);
		SerializationContext(
			Networkable::State _state, 
			sf::Packet& _prevPacket, sf::Packet& _packet, float _interpolationProgress);
		SerializationContext(SerializationContext const& _ctx);
	};

	template<class T> sf::Packet& operator<<(sf::Packet& _packet, T const& _arg);
	template<class T> sf::Packet& operator>>(sf::Packet& _packet, T& _arg);
	template<class T> void skip(sf::Packet& _packet, T const& _arg);
	template<class T, class U> sf::Packet& operator<<(sf::Packet& _packet, std::pair<T, U> const& _arg);
	template<class T, class U> sf::Packet& operator>>(sf::Packet& _packet, std::pair<T, U>& _arg);
	template<class T, class U> void skip(sf::Packet& _packet, std::pair<T, U> const& _arg);

	/**
	* Override this function and add all variables you wish to serialize.
	*/
	template<class T> void serialize(
		T& _object, SerializationContext const& _ctx, 
		SylUtil::InterpolationFunction<T> const& _interpolationFunction = nullptr);
};

#include "serialization.tcc"

#endif