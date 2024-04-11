
#ifndef __TCPSERVER_H__
#define __TCPSERVER_H__

#ifdef BASE_SUPPORT_NETWORKING

#include <vector>

namespace network 
{
	// forward declare
	class TCPClient;

	class TCPServer
	{
		public:
			/// default constructor
			TCPServer();
			/// default destructor
			~TCPServer();

			/// Initialise - Initialises any server data
			void Initialise();
			/// Release - Release any server data
			void Release();

			/// StartServer - Starts a server
			/// \param nPort - port to start the server on
			/// \param nMaxNumClients - maximum number of clients the server can connect with
			/// \param FuncCallback - server function callback
			/// \return integer - ( SUCCESS: OK or FAIL: FAIL )
			int StartServer( int nPort, unsigned int maxNumClients, void (*FuncCallback)( void *pData, int size, int packetID ) );
			/// SendTo - Sends data to all clients
			/// \param packetID - unique packet id
			/// \param buffer - data to send
			/// \param size - size of the data to send
			/// \return integer - size of data that was actually sent
			int SendTo( unsigned short packetID, void *buffer, unsigned int size );
			/// SendTo - Sends data to a specific client
			/// \param packetID - unique packet id
			/// \param toID - client network id
			/// \param buffer - data to send
			/// \param size - size of the data to send
			/// \return integer - size of data that was actually sent
			int SendTo( unsigned short packetID, unsigned short toID, void *buffer, unsigned int size );
			/// Update - update the server per frame
			/// \param deltaTime - time since last frame
			void Update( float deltaTime );
			/// CloseServer - Closes the server down
			/// \return integer - ( SUCCESS: 0 or FAIL: 1 )
			int CloseServer();
			/// DisconnectClient - Disconnects a connect client from the server
			/// \param clientID - id of client to disconnect
			/// \return integer - ( SUCCESS: 0 or FAIL: 1 )
			int DisconnectClient( int clientID );
			/// GetClient - Gets a client class from its ID
			/// \param clientID - id to find
			/// \return TCPClient - ( SUCCESS: Valid client class or FAIL: 0 )
			TCPClient *GetClient( unsigned short clientID );
			/// GetClientPing - Gets a clients ping from its ID
			/// \param nClientID - id to find
			/// \return double - time taken for a pingpong packet to reach the client
			double GetClientPing( unsigned short clientID );
			/// IsServer - Returns whether a server is started
			/// return integer - returns if a server is started
			int IsServerStarted()						{ return m_ServerStarted; }

		private:
			/// Server callback function
			void (*ServerCallback)( void *pData, int nSize, int nPacketID );

			/// server started flag
			bool m_ServerStarted;
			/// max number of clients connect to the server
			unsigned int m_MaxNumClients;
			/// current number of active clients
			unsigned int m_NumActiveClients;

			/// list of clients
			std::vector<BaseSocket *> m_ClientList;

			/// server listen socket
			BaseSocket	m_ListenSocket;

	};
}
#endif // BASE_SUPPORT_NETWORKING

#endif // __TCPSERVER_H__


