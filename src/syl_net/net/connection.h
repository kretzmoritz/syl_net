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

#ifndef SYLNET_CONNECTION_H
#define SYLNET_CONNECTION_H

#include <functional>
#include <SFML/Network.hpp>

#include "../protocol/protocol.h"
#include "client.h"

namespace SylNet
{
	struct Connection
	{
	public:
		Connection();
		Connection(sf::IpAddress _address, unsigned short _port);

		bool operator<(Connection const& _other) const;

		sf::IpAddress address;
		unsigned short port;
	};

	typedef std::function<void(Protocol&, ClientId, unsigned short, Connection)> ConnectionCallback; //args: caller, client id, local port, remote adress/port
};

#endif