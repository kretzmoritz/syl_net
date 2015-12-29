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

#ifndef SYLNET_TCP_H
#define SYLNET_TCP_H

#include "protocol.h"
#include "../net/connection.h"

namespace SylNet
{
	class Tcp : public Protocol
	{
	public:
		Tcp();
		virtual ~Tcp();

		/**
		* Limits heartbeating over network to one packet per timeframe specified.
		*/
		void setHeartbeatDelay(float _seconds);
		/**
		* Handshake will timeout once the supplied seconds have passed without success.
		*/
		void setHandshakeTimeout(float _seconds);
		/**
		* Time until acknowledgement packet has to be received (otherwise connection will close).
		* 0.0f = disable
		*/
		void setConnectionTimeout(float _seconds);

		/**
		* Callback that will fire once a connection is properly established.
		*/
		void setConnectCallback(ConnectionCallback _cb);
		/**
		* Callback that will fire once a disconnect has occured.
		*/
		void setDisconnectCallback(ConnectionCallback _cb);

	protected:
		virtual void heartbeat();
		bool processHeartbeat(sf::Packet& _packet);
		bool processAck(sf::Packet& _packet, SylUtil::time_ms& _lastAckReceiveTime, SylUtil::time_ms& _lastLatency);
		bool isConnectionAlive(SylUtil::time_ms _lastAckReceiveTime);

		sf::Time m_handshakeTimeout;

		ConnectionCallback m_connectCallback;
		ConnectionCallback m_disconnectCallback;

	private:
		SylUtil::time_ms m_heartbeatDelay;
		SylUtil::time_ms m_nextHeartbeatTime;

		SylUtil::time_ms m_connectionTimeout;
	};
};

#endif