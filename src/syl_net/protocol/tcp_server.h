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

#ifndef SYLNET_TCP_SERVER_H
#define SYLNET_TCP_SERVER_H

#include <set>

#include "tcp.h"

namespace SylNet
{
	typedef std::function<void(Protocol&)> CapacityReachedCallback;

	class TcpServer : public Tcp
	{
	public:
		TcpServer();
		virtual ~TcpServer();

		/**
		* Starts to listen for incoming connections on the specified port.
		* @param _minClients = blocks until at least this amount is properly connected
		* @param _maxClients = only allows this amount to establish a connection (0 = unlimited)
		*/
		bool listen_start(unsigned short _port, unsigned int _minClients = 0, unsigned int _maxClients = 0);
		/**
		* Stops listening for new connections.
		*/
		void listen_end();

		/**
		* This function does the actual work of accepting the clients.
		* Should you require further connections than the ones already active after listen_start call this every frame.
		*/
		void accept();
		/**
		* Closes all open connections.
		*/
		void close_all();
		/**
		* Closes an open connection with the specified client.
		*/
		void close(ClientId _clientId);

		/**
		* Send will serialize all clients.
		*/
		void sync_all();
		/**
		* Send will serialize only the specified client.
		*/
		void sync(ClientId _clientId);

		/**
		* Returns the current connection count.
		*/
		virtual unsigned int getConnectionCount() const;

		/**
		* Callback that will fire once maxClients have connected.
		*/
		void setCapacityReachedCallback(CapacityReachedCallback _cb);
		/**
		* Returns latency of the client's connection.
		* It's calculated as (RTT / 2).
		*/
		SylUtil::time_ms getLastLatency(ClientId _clientId) const;

	protected:
		virtual bool send_packet(sf::Packet& _packet);
		virtual void receive_packets();

		virtual void heartbeat();
		bool processHeartbeat(sf::Packet& _packet, ClientId _clientId);

	private:
		ClientId getLowestUnusedClientId();
		void disconnect(ClientId _clientId, sf::TcpSocket* _socket);

		sf::TcpListener m_listener;
		std::unique_ptr<sf::TcpSocket> m_socket;
		std::map<ClientId, ClientSocket> m_clients;

		bool m_blockingAccept;
		unsigned int m_maxClients;
		ClientId m_lowestUnusedClientId;
		std::set<ClientId> m_usedClientIds;

		bool m_syncAll;
		ClientId m_syncTarget;

		CapacityReachedCallback m_capacityReachedCallback;
	};
};

#endif