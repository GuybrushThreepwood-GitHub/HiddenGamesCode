
#ifndef __TCPCLIENT_H__
#define __TCPCLIENT_H__

#ifdef BASE_SUPPORT_NETWORKING

namespace network
{
	class TCPClient
	{
		public:
			/// default constructor
			TCPClient();
			/// default destructor
			~TCPClient();

			/// Initialise - Initialise any client data
			void Initialise();
			/// Release - Release any client data
			void Release();

			/// ConnectToServer - Attempts to connect to a server
			/// \param port - port to connect to the server on
			/// \param szServerIP - IP number of the server
			/// \param FuncCallback - client function callback
			/// \return integer - ( SUCCESS: 0 or FAIL: 1 )
			int ConnectToServer( int port, const char *szServerIP, void (*FuncCallback)( void *pData, int size, int packetID ) );
			/// SendTo - Sends data to a client or server
			/// \param packetID - network ID to send to
			/// \param buffer - data to send
			/// \param size - size of the data to send
			/// \return integer - size of data that was actually sent
			int Send( unsigned short packetID, void *buffer, unsigned int size );
			/// Update - updates the client
			/// \param deltaTime - time since last update
			void Update( float deltaTime );
			/// DisconnectFromServer - Disconnects from a server
			/// \return integer - ( SUCCESS: 0 or FAIL: 1 )
			int DisconnectFromServer();

			/// IsConnected - checks to see if this client is connected
			/// \return integer - ( SUCCESS: true or FAIL: false )
			int IsConnected()								{ return m_IsConnected; }
			/// IsConnecting - checks to see if this client is connecting
			/// \return integer - ( SUCCESS: true or FAIL: false )
			int IsConnecting()								{ return m_IsConnecting; }

			/// SetConnected - Sets the client connection flag
			/// \param isConnected - connection flag
			void SetConnected( int isConnected )			{ m_IsConnected = isConnected; }

			/// SetNetworkID - Sets the network id for the client
			/// \param nNetworkID - clients network id
			void SetNetworkID( unsigned short networkID )	{ m_NetworkID = networkID; }

			/// SetNetworkID - Sets the network id for the client
			/// \return unsigned short - clients network id
			unsigned short GetNetworkID( void )				{ return m_NetworkID; }

			/// GetSocketClass - Gets the socket class for this client
			/// \return CSocket - ( SUCCESS: valid socket class or FAIL: SOCKET_ERROR )
			BaseSocket &GetSocketClass() { return m_ConnectedSocket; }

			/// GetAddressClass - Gets the socket address class for this client
			/// \return CSocket - ( SUCCESS: valid socket address class or FAIL: SOCKET_ERROR )
			SockAddr &GetAddressClass() { return m_ClientAddress; }

			/// GetConnectSocket - Gets the connection socket for this client
			/// \return SOCKET - ( SUCCESS: valid socket id or FAIL: SOCKET_ERROR )
			SOCKET GetConnectSocket() { return m_ConnectedSocket.GetSocket(); }

			/// GetConnectionTime - Gets how long the connection timer has been running
			/// \return float - time in seconds since the timer was started
			//float GetConnectionTime() { return m_ConnectionTimer.GetTimeInSeconds(); }

			/// last time the server was ping'd
			//CTimer lastServerTime;
			/// current ping timer
			//CTimer pingTimer;
			/// how long it took to ping the server
			double pingTimeMillisecs;
			/// current incoming sequence
			unsigned long nIncomingSequence;
			/// current outgoing sequence
			unsigned long nOutgoingSequence;

		private:
			/// Client callback function
			void (*ClientCallback)( void *pData, int size, int packetID );

			/// connected flag
			int m_IsConnected;
			/// connecting flag
			int m_IsConnecting;
			/// connected flag
			int m_ClientConnected;
			/// clients network id
			unsigned short m_NetworkID;

			/// client socket address
			SockAddr m_ClientAddress;
			/// server socket address
			SockAddr m_ServerAddress;
			/// client connected sockets
			BaseSocket m_ConnectedSocket;
			/// timer used while connecting
			float m_ConnectionTimer;

	};
}

#endif // BASE_SUPPORT_NETWORKING

#endif // __TCPCLIENT_H__


