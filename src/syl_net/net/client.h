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

#ifndef SYLNET_CLIENT_H
#define SYLNET_CLIENT_H

#include <memory>
#include <SFML/Network.hpp>
#include <limits>

#include "../../syl_util/time/time.h"

namespace SylNet
{
	typedef unsigned int ClientId;
	static ClientId const BROADCAST_ID = std::numeric_limits<ClientId>::max();
	static ClientId const MAX_CLIENT_ID = std::numeric_limits<ClientId>::max() - 1;

	struct ClientSocket
	{
	public:
		ClientSocket(
			std::unique_ptr<sf::TcpSocket>& _socket, 
			SylUtil::time_ms _lastAckReceiveTime, SylUtil::time_ms _lastLatency);
		ClientSocket(ClientSocket& _other);

		ClientSocket& operator=(ClientSocket& _other);

		std::unique_ptr<sf::TcpSocket> socket;
		SylUtil::time_ms lastAckReceiveTime;
		SylUtil::time_ms lastLatency;
	};
};

#endif