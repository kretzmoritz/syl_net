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

#ifndef SYLNET_UDP_H
#define SYLNET_UDP_H

#include <map>

#include "protocol.h"
#include "../net/client.h"
#include "../net/connection.h"

namespace SylNet
{
	class Udp : public Protocol
	{
	public:
		Udp();
		virtual ~Udp();

		/**
		* Binds the specified port.
		* This allows us to receive packets through this channel.
		*/
		bool bind(unsigned short _port);
		/**
		* Unbinds the currently used port.
		* This returns access to the OS.
		*/
		void unbind();
		/**
		* Get the port to which the socket is bound locally.
		*/
		unsigned short getLocalPort() const;

		/**
		* Adds connection to which we can send data.
		*/
		bool addConnection(ClientId _clientId, Connection _connection);
		/**
		* Removes active connection.
		*/
		void removeConnection(ClientId _clientId);
		/**
		* Clears all active connections.
		*/
		void clearConnections();
		/**
		* Returns the current connection count.
		*/
		virtual unsigned int getConnectionCount() const;

		/**
		* Allows receiving of packets from unknown (and thus not added) connections.
		*/
		void acceptPacketsFromUnknownConnections(bool _value = true);

	private:
		virtual bool send_packet(sf::Packet& _packet);
		virtual void receive_packets();

		sf::UdpSocket m_socket;
		
		std::map<ClientId, Connection> m_connections;
		std::map<Connection, ClientId> m_ids;

		bool m_acceptPacketsFromUnknownConnections;
	};
};

#endif