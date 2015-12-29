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

#ifndef SYLNET_PROTOCOL_H
#define SYLNET_PROTOCOL_H

#include <unordered_map>
#include <functional>
#include <SFML/Network.hpp>

#include "../net/networkable.h"

namespace SylNet
{
	class Protocol
	{
	public:
		Protocol();
		virtual ~Protocol();

		/**
		* Add a networkable you wish to serialize.
		* An interpolation function can be given in order to achieve smooth transition.
		* @param _disableDefaultInterpolation ADVANCED: Should you want to roll your own interpolation implementation set this to true.
		*/
		template<class T> bool registerNetworkable(
			Networkable::Id _id, T& _object, 
			SylUtil::InterpolationFunction<T> _interpolationFunction = nullptr, bool _disableDefaultInterpolation = false);
		/**
		* Remove a registered networkable. It will no longer be serialized.
		*/
		void unregisterNetworkable(Networkable::Id _id);
		/**
		* Removes all registered networkables.
		*/
		void clearNetworkables();

		/**
		* Limits sending over network to one packet per timeframe specified.
		*/
		void setSendDelay(float _seconds);
		/**
		* Returns the send delay used in ms.
		*/
		SylUtil::time_ms getSendDelayMilliSeconds() const;
		/**
		* Returns the send delay used in seconds.
		*/
		float getSendDelaySeconds() const;
		/**
		* Limits receiving over network (per frame) to packet limit specified.
		* 0 = unlimited
		*/
		void setReceivePacketLimit(unsigned int _packetLimit);
		
		/**
		* Default interpolation time will be given as (sendDelay * 2).
		* This is due to Udp sometimes loosing packets in transfer.
		*/
		void setDefaultInterpolationDuration();
		/**
		* USE WITH CARE: This allows to set a custom interpolation time.
		* Effects might vary.
		*/
		void setCustomInterpolationDuration(float _seconds);
		/**
		* Returns the interpolation time used in ms.
		*/
		SylUtil::time_ms getInterpolationDurationMilliSeconds() const;
		/**
		* Returns the interpolation time used in seconds.
		*/
		float getInterpolationDurationSeconds() const;

		/**
		* Returns the current connection count.
		*/
		virtual unsigned int getConnectionCount() const = 0;

		/**
		* Combines all supplied data and if delay permits sends packet.
		* @retval true = succeeded
		* @retval false = an error occured during sending or delay still active (you can circumvent this by calling #setSendDelay with a parameter of 0.0f)
		*/
		bool send();
		/**
		* Receives and processes packets. Also handles interpolation.
		*/
		void receive();

	protected:
		virtual bool send_packet(sf::Packet& _packet) = 0;
		virtual void receive_packets() = 0;

		std::unordered_multimap<ClientId, sf::Packet> m_packets;
		std::unordered_map<Networkable::Id, Networkable*> m_networkables;

		unsigned int m_packetLimit;

	private:
		SylUtil::time_ms m_sendDelay;
		SylUtil::time_ms m_nextSendTime;
		
		bool m_useCustomInterpolationDuration;
		SylUtil::time_ms m_customInterpolationDuration;

		PacketId m_packetId;
	};
};

#include "protocol.tcc"

#endif