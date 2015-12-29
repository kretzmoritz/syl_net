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

#ifndef SYLNET_BROWSER_H
#define SYLNET_BROWSER_H

#include <vector>
#include <utility>
#include <string>

#include "../protocol/udp.h"
#include "../net/connection.h"
#include "../../syl_util/time/time.h"

namespace SylNet
{
	class Browser
	{
		friend void serialize(
			Browser& _object, SerializationContext const& _ctx,
			SylUtil::InterpolationFunction<Browser> const& _interpolationFunction);

	public:
		Browser(unsigned short _browserPort, float _keepAliveSeconds);

		/**
		* Search on this port for active servers.
		*/
		void setBrowserPort(unsigned short _browserPort);
		/**
		* Keep known servers alive for the specified time.
		* Once a server stops broadcasting this is how long you will still see it show up.
		*/
		void setKeepAliveTime(float _seconds);
		/**
		* Returns a vector of active servers.
		* Each entry consists of a server name and connection pair.
		*/
		void getRemoteServers(std::vector<std::pair<std::string, Connection>>& _remoteServers);

		/**
		* Call this function each frame to update active servers.
		*/
		void receive();

	private:
		Udp m_udp;

		SylUtil::time_ms m_keepAliveTime;
		std::map<Connection, std::pair<std::string, SylUtil::time_ms>> m_remoteServers;
	};

	void serialize(
		Browser& _object, SerializationContext const& _ctx,
		SylUtil::InterpolationFunction<Browser> const& _interpolationFunction = nullptr);
};

#endif