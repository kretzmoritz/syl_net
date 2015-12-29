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

#ifndef SYLNET_PACKET_H
#define SYLNET_PACKET_H

#include <SFML/Network.hpp>

namespace SylNet
{
	enum PacketType
	{
		DEFAULT,
		HEARTBEAT,
		ACK
	};

	struct PacketId
	{
	public:
		PacketId();
		PacketId(unsigned int _id);

		PacketId& operator++();
		bool operator>(PacketId const& _other);

	private:
		unsigned int m_id;
	};
};

#endif