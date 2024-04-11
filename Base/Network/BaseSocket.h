
#ifndef __BASESOCKET_H__
#define __BASESOCKET_H__

#ifdef BASE_SUPPORT_NETWORKING

namespace network
{
	class BaseSocket
	{
		public:
			/// enumerated socket types
			enum SocketType
			{
				SocketType_UDP,
				SocketType_TCP,
				SocketType_RAW
			};

			explicit BaseSocket( SOCKET = INVALID_SOCKET );
			explicit BaseSocket( SocketType type );
			/// default constructor
			/// \param type - type of socket
			/// \param protocol - socket protocol
			BaseSocket( int type, int protocol );
			/// default destructor
			~BaseSocket();

			/// Open - Sets up a new socket 
			/// \param type - type of socket to create
			/// \return integer - ( SUCCESS: valid socket id or FAIL: INVALID_SOCKET )
			int Open( SocketType type );
			/// Open - Sets up a new socket 
			/// \param type - type of socket to create
			/// \param protocol - protocol to attach to this socket
			/// \return integer - ( SUCCESS: valid socket id or FAIL: INVALID_SOCKET )
			int Open( int type, int protocol );
			/// Open - Sets up a new socket 
			/// \param family - socket address family
			/// \param type - type of socket to create
			/// \param protocol - protocol to attach to this socket
			/// \return integer - ( SUCCESS: valid socket id or FAIL: INVALID_SOCKET )
			int Open( int family, int type, int protocol );
			/// IsOpen - checks to see if there's a valid socket id
			/// \return integer - ( SUCCESS: valid socket id or FAIL: INVALID_SOCKET )
			int IsOpen() const;
			/// Close - closes the socket
			/// \return integer - 
			int Close();
			/// Accept - Accepts an incoming connect on this socket
			/// \param pSockAddr - accepted address structure
			/// \returns SOCKET - ( SUCCESS: valid socket id or FAIL: INVALID_SOCKET )
			SOCKET Accept( sockaddr_in* pSockAddr = 0 );
			/// Bind - Binds an address to this socket
			/// \param pSockAddr - address structure to bind
			/// \return integer - ( SUCCESS: 0 or FAIL: SOCKET_ERROR )
			int Bind( const sockaddr_in* pSockAddr );
			/// Connect - tries to establish a connection
			/// \param pSockAddr - address to recieve the connection
			/// \return integer - ( SUCCESS: 0 or FAIL: SOCKET_ERROR )
			int Connect( const sockaddr_in* pSockAddr );
			/// GetSocket - gets the socket id attached to this class
			/// \return SOCKET - ( SUCCESS: valid socket id or FAIL: INVALID_SOCKET )
			SOCKET GetSocket() const;
			/// GetSockName - get the address attached to the socket
			/// \param pSockAddr - Pointer to a sockaddr_in structure that receives the address (name) of the socket
			/// \return integer - ( SUCCESS: 0 or FAIL: SOCKET_ERROR )
			int GetSockName( sockaddr_in* pSockAddr ) const;
			/// GetSockOpt - Finds out if a certain state is set on the socket
			/// \param level - Level at which the option is defined
			/// \param name - Socket option for which the value is to be retrieved
			/// \param pValue - Pointer to the buffer in which the value for the requested option is to be returned
			/// \param pSize - Pointer to the size of the optval buffer, in bytes
			/// \return integer - ( SUCCESS: 0 or FAIL: SOCKET_ERROR )
			int GetSockOpt( int level, int name, void* pValue, int* pSize ) const;
			/// IoCtlSocket - Sets a IO options of the socket
			/// \param nCmd - Command to perform on the socket
			/// \param pArg - Pointer to a parameter for cmd
			/// \return integer - ( SUCCESS: 0 or FAIL: SOCKET_ERROR )
			int IoCtlSocket( long cmd, unsigned long* pArg );
            /// Fcntl
            int Fcntl(int cmd, long arg);
			/// Listen - Listens for incoming connects
			/// \param backlog - number of connections to backlog
			/// \return integer - ( SUCCESS: 0 or FAIL: SOCKET_ERROR )
			int Listen( int backlog = SOMAXCONN );
			/// Recv - receives data on the socket
			/// \param pBuffer - data to recieve
			/// \param bytes - number of bytes to receive on the socket
			/// \return integer - bytes received
			int Recv( void* pBuffer, int bytes );
			/// RecvFrom - UDP based receiving
			/// \param pBuffer - data to recieve 
			/// \param bytes - number of bytes to receive on the socket
			/// \param pSockAddr - address data was received on
			/// \return integer - bytes received
			int RecvFrom( void* pBuffer, int bytes, sockaddr_in* pSockAddr = 0 );
			/// Select - checks the state of a socket list
			/// \param pbRead - check for reads
			/// \param pbWrite - check for writes
			/// \param pbError - check for errors
			/// \return integer - ( SUCCESS: number of socket handles or FAIL: 0/SOCKET_ERROR )
			int Select( int* pbRead, int* pbWrite, int* pbError );
			/// Send - sends data out
			/// \param pBuffer - data to send
			/// \param bytes - number of bytes to send
			/// \return integer - bytes sent
			int Send( const void* pBuffer, int bytes );
			/// SendTo - sends data out
			/// \param pBuffer - data to send
			/// \param bytes - number of bytes to send
			/// \param pSockAddr - address to send data on
			/// \return integer - bytes sent
			int SendTo( const void* pBuffer, int bytes, const sockaddr_in* pSockAddr = 0 );
			/// SetSockOpt - sets a socket option
			/// \param level - Level at which the option is defined
			/// \param name - Socket option for which the value is to be set.
			/// \param pValue - Pointer to the buffer in which the value for the requested option is specified
			/// \param bytes - Size of the pValue buffer, in bytes
			/// \return integer - ( SUCCESS: 0 or FAIL: SOCKET_ERROR )
			int SetSockOpt( int level, int name, const void* pValue, int bytes );
			/// Shutdown - waits to send/recv all current data before closing the socket
			/// \param how - Flag that describes what types of operation will no longer be allowed
			/// \return integer - ( SUCCESS: 0 or FAIL: SOCKET_ERROR )
			int Shutdown( int how );

		private:
			BaseSocket( const BaseSocket& );
			BaseSocket& operator=( const BaseSocket& );

			/// main socket id
			SOCKET m_Socket;
	};
}

#endif // BASE_SUPPORT_NETWORKING

#endif // __BASESOCKET_H__


