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

#ifndef SYLNET_TCP_CLIENT_H
#define SYLNET_TCP_CLIENT_H

#include "tcp.h"

namespace SylNet
{
	class TcpClient : public Tcp
	{
	public:
		TcpClient();
		virtual ~TcpClient();

		/**
		* Connects to a server on the address and port specified.
		*/
		bool open(sf::IpAddress _address, unsigned short _port);
		/**
		* Closes an open connection.
		*/
		void close();

		/**
		* Returns the current connection count.
		*/
		virtual unsigned int getConnectionCount() const;

		/**
		* Open attempt will timeout once the supplied seconds have passed without success.
		*/
		void setOpenTimeout(float _seconds);
		/**
		* Returns latency of the active connection.
		* It's calculated as (RTT / 2).
		*/
		SylUtil::time_ms getLastLatency() const;

	protected:
		virtual bool send_packet(sf::Packet& _packet);
		virtual void receive_packets();

	private:
		void disconnect();

		sf::TcpSocket m_socket;
		bool m_connected;
		unsigned int m_clientId;

		SylUtil::time_ms m_openTimeout;
		
		SylUtil::time_ms m_lastAckReceiveTime;
		SylUtil::time_ms m_lastLatency;
	};
};

#endif