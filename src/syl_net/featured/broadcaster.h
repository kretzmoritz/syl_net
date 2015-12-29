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

#ifndef SYLNET_BROADCASTER_H
#define SYLNET_BROADCASTER_H

#include <string>

#include "../protocol/udp.h"
#include "../net/connection.h"

namespace SylNet
{
	class Broadcaster
	{
		friend void serialize(
			Broadcaster& _object, SerializationContext const& _ctx,
			SylUtil::InterpolationFunction<Broadcaster> const& _interpolationFunction);

	public:
		Broadcaster(std::string _serverName, unsigned short _port, unsigned short _broadcastPort, float _sendDelaySeconds);

		/**
		* Name under which the server is known throughout the subnet.
		*/
		void setServerName(std::string _serverName);
		/**
		* Port under which the server is operating.
		*/
		void setPort(unsigned short _port);
		/**
		* Broadcast our information on this port.
		*/
		void setBroadcastPort(unsigned short _broadcastPort);
		/**
		* Limits broadcasting over network to one packet per timeframe specified.
		*/
		void setSendDelay(float _seconds);

		/**
		* Call this function each frame to broadcast our information.
		*/
		void send();

	private:
		Udp m_udp;

		std::string m_serverName;
		Connection m_connection;
	};

	void serialize(
		Broadcaster& _object, SerializationContext const& _ctx,
		SylUtil::InterpolationFunction<Broadcaster> const& _interpolationFunction = nullptr);
};

#endif