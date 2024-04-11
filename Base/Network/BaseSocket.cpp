
/*===================================================================
	File: BaseSocket.cpp
	Library: Network

	(C)Hidden Games
=====================================================================*/

#ifdef BASE_SUPPORT_NETWORKING

#include "CoreBase.h"
#include "PlatformNetworkBase.h"

#include "BaseSocket.h"

using network::BaseSocket;

/////////////////////////////////////////////////////
/// Constructor
/// Params: [in]sock
///
/////////////////////////////////////////////////////
BaseSocket::BaseSocket( SOCKET sock ) 
	: m_Socket( sock )
{
}

/////////////////////////////////////////////////////
/// Constructor
/// Params: [in]type
///
/////////////////////////////////////////////////////
BaseSocket::BaseSocket( SocketType type ) 
	: m_Socket( INVALID_SOCKET )
{
	int success = Open( type );
	DBG_ASSERT( !success );
}

/////////////////////////////////////////////////////
/// Constructor
/// Params: [in]iType, [in]iProtocol
///
/////////////////////////////////////////////////////
BaseSocket::BaseSocket( int type, int protocol ) 
	: m_Socket( INVALID_SOCKET )
{
	int success = Open( type, protocol );
	DBG_ASSERT( !success );
}

/////////////////////////////////////////////////////
/// Default destructor
/// 
///
/////////////////////////////////////////////////////
BaseSocket::~BaseSocket()
{
	Close();
}

/////////////////////////////////////////////////////
/// Method: Open
/// Params: [in]type
///
/////////////////////////////////////////////////////
int BaseSocket::Open( SocketType type )
{
	switch( type )
	{
		case SocketType_UDP:
			return Open( SOCK_DGRAM, IPPROTO_UDP );

		case SocketType_TCP:
			return Open( SOCK_STREAM, IPPROTO_TCP );

		default:
			DBG_ASSERT( type == SocketType_RAW );
			return Open( SOCK_RAW, IPPROTO_RAW );
	}
}

/////////////////////////////////////////////////////
/// Method: Open
/// Params: [in]type, [in]protocol
///
/////////////////////////////////////////////////////
int BaseSocket::Open( int type, int protocol )
{
	Close();
	m_Socket = socket( AF_INET, type, protocol );

	return( m_Socket != INVALID_SOCKET );
}

/////////////////////////////////////////////////////
/// Method: Open
/// Params: [in]type, [in]protocol
///
/////////////////////////////////////////////////////
int BaseSocket::Open( int family, int type, int protocol )
{
	Close();
	m_Socket = socket( family, type, protocol );

	return( m_Socket != INVALID_SOCKET );
}

/////////////////////////////////////////////////////
/// Method: IsOpen
/// Params: None
///
/////////////////////////////////////////////////////
int BaseSocket::IsOpen() const
{
	return( m_Socket != INVALID_SOCKET );
}

/////////////////////////////////////////////////////
/// Method: Close
/// Params: None
///
/////////////////////////////////////////////////////
int BaseSocket::Close()
{
	int result = 0;
	if( m_Socket != INVALID_SOCKET )
	{
#ifdef BASE_PLATFORM_WINDOWS	
		result = closesocket( m_Socket );
#else
		close( m_Socket );
#endif //
		m_Socket = INVALID_SOCKET;
	}

	return result;
}

/////////////////////////////////////////////////////
/// Method: Accept
/// Params: [in]pSockAddr
///
/////////////////////////////////////////////////////
SOCKET BaseSocket::Accept( sockaddr_in* pSockAddr )
{
	DBG_ASSERT( m_Socket != INVALID_SOCKET );
	int size = sizeof( sockaddr_in );

	SOCKET sockResult = accept( m_Socket, (sockaddr*)(pSockAddr), (socklen_t *)&size );

	if( sockResult != INVALID_SOCKET 
		&& pSockAddr != 0 )
		DBG_ASSERT( size == sizeof( sockaddr_in ) );

	return sockResult;
}

/////////////////////////////////////////////////////
/// Method: Bind
/// Params: [in]pSockAddr
///
/////////////////////////////////////////////////////
int BaseSocket::Bind( const sockaddr_in* pSockAddr )
{
	DBG_ASSERT( m_Socket != INVALID_SOCKET );
	DBG_ASSERT( pSockAddr != 0 );
	DBG_ASSERT( pSockAddr->sin_family == AF_INET );

	int result = bind( m_Socket, (const sockaddr*)(pSockAddr), sizeof( sockaddr_in ) );

	return result;
}

/////////////////////////////////////////////////////
/// Method: Connect
/// Params: [in]pSockAddr
///
/////////////////////////////////////////////////////
int BaseSocket::Connect( const sockaddr_in* pSockAddr )
{
	DBG_ASSERT( m_Socket != INVALID_SOCKET );
	DBG_ASSERT( pSockAddr != 0 );
	DBG_ASSERT( pSockAddr->sin_family == AF_INET );

	int result = connect( m_Socket, (const sockaddr*)(pSockAddr), sizeof( sockaddr_in ) );
	
	return result;
}

/////////////////////////////////////////////////////
/// Method: GetSocket
/// Params: None
///
/////////////////////////////////////////////////////
SOCKET BaseSocket::GetSocket() const
{
	return m_Socket;
}

/////////////////////////////////////////////////////
/// Method: GetSockName
/// Params: [in]pSockAddr
///
/////////////////////////////////////////////////////
int BaseSocket::GetSockName( sockaddr_in* pSockAddr ) const
{
	DBG_ASSERT( m_Socket != INVALID_SOCKET );
	DBG_ASSERT( pSockAddr != 0 );
	int size = sizeof( sockaddr_in );

	int result = getsockname( m_Socket, (sockaddr*)(pSockAddr), (socklen_t *)&size );

	if( result != SOCKET_ERROR )
		DBG_ASSERT( size == sizeof( sockaddr_in ) );

	return result;
}

/////////////////////////////////////////////////////
/// Method: GetSockOpt
/// Params: [in]level, [in]name, [out]pValue, [out]piSize
///
/////////////////////////////////////////////////////
int BaseSocket::GetSockOpt( int level, int name, void* pValue, int* pSize ) const
{
	DBG_ASSERT( m_Socket != INVALID_SOCKET );
	DBG_ASSERT( pValue != 0 );
	DBG_ASSERT( pSize != 0 );

	int result = getsockopt( m_Socket, level, name, (char *)(pValue), (socklen_t *)pSize );

	return result;
}

/////////////////////////////////////////////////////
/// Method: IoCtlSocket
/// Params: [in]nCmd, [in]pArg
///
/////////////////////////////////////////////////////
int BaseSocket::IoCtlSocket( long cmd, unsigned long* pArg )
{
	DBG_ASSERT( m_Socket != INVALID_SOCKET );
	DBG_ASSERT( pArg != 0 );

	int result = 0;

#ifdef BASE_PLATFORM_WINDOWS	
	result = ioctlsocket( m_Socket, cmd, pArg );
#else
	result = ioctl( m_Socket, cmd, pArg );
#endif 

	return result;
}

/////////////////////////////////////////////////////
/// Method: Fcntl
/// Params: [in]cmd, [in]pArg
///
/////////////////////////////////////////////////////
int BaseSocket::Fcntl(int cmd, long arg)
{
    DBG_ASSERT( m_Socket != INVALID_SOCKET );
    
	int result = 0;
   
#ifdef BASE_PLATFORM_WINDOWS	

#else
	result = fcntl( m_Socket, cmd, arg );
#endif 
    
	return result;
}

/////////////////////////////////////////////////////
/// Method: Listen
/// Params: [in]iBacklog
///
/////////////////////////////////////////////////////
int BaseSocket::Listen( int backlog )
{
	DBG_ASSERT( m_Socket != INVALID_SOCKET );

	int result = listen( m_Socket, backlog );

	return result;
}

/////////////////////////////////////////////////////
/// Method: Recv
/// Params: [in]pBuffer, [in]iBytes
///
/////////////////////////////////////////////////////
int BaseSocket::Recv( void* pBuffer, int bytes )
{
	DBG_ASSERT( m_Socket != INVALID_SOCKET );
	DBG_ASSERT( pBuffer != 0 );
	DBG_ASSERT( bytes >= 0 );

	int result = recv( m_Socket, (char *)(pBuffer), bytes, 0 );

	return result;
}

/////////////////////////////////////////////////////
/// Method: RecvFrom
/// Params: [in]pBuffer, [in]iBytes, [in]pSockAddr
///
/////////////////////////////////////////////////////
int BaseSocket::RecvFrom( void* pBuffer, int bytes, sockaddr_in* pSockAddr )
{
	DBG_ASSERT( m_Socket != INVALID_SOCKET );
	DBG_ASSERT( pBuffer != 0 );
	DBG_ASSERT( bytes >= 0 );

	int size = sizeof( sockaddr_in );

	int result = recvfrom( m_Socket, (char *)(pBuffer), bytes, 0, (sockaddr*)(pSockAddr), (socklen_t *)&size );

	if( result != SOCKET_ERROR && pSockAddr != 0 )
		DBG_ASSERT( size == sizeof( sockaddr_in ) );

	return result;
}

/////////////////////////////////////////////////////
/// Method: Select
/// Params: [in]pbRead, [in]pbWrite, [in]pbError
///
/////////////////////////////////////////////////////
int BaseSocket::Select( int* pbRead, int* pbWrite, int* pbError )
{
	DBG_ASSERT( m_Socket != INVALID_SOCKET );

	int resultTotal = 0;
	int nfds = 0;

	timeval tv;
	tv.tv_sec = 0; 
	tv.tv_usec = 0;

	if( pbRead )
	{
		fd_set fdsRead;

		FD_ZERO( &fdsRead );
		FD_SET( m_Socket, &fdsRead );

		nfds = (int)m_Socket + 1;

		int result = select( nfds, &fdsRead, 0, 0, &tv );
		DBG_ASSERT( result != SOCKET_ERROR );

		*pbRead = ( result == 1 );
		resultTotal += result;
	}

	if( pbWrite )
	{
		fd_set fdsWrite;

		FD_ZERO( &fdsWrite );
		FD_SET( m_Socket, &fdsWrite );

		nfds = (int)m_Socket + 1;

		int result = select( nfds, 0, &fdsWrite, 0, &tv );
		DBG_ASSERT( result != SOCKET_ERROR );

		*pbWrite = ( result == 1 );
		resultTotal += result;
	}

	if( pbError )
	{
		fd_set fdsError;

		FD_ZERO( &fdsError );
		FD_SET( m_Socket, &fdsError );

		nfds = (int)m_Socket + 1;

		int result = select( nfds, 0, 0, &fdsError, &tv );
		DBG_ASSERT( result != SOCKET_ERROR );

		*pbError = ( result == 1 );
		resultTotal += result;
	}
    
	return resultTotal;
}

/////////////////////////////////////////////////////
/// Method: Send
/// Params: [in]pBuffer, [in]bytes
///
/////////////////////////////////////////////////////
int BaseSocket::Send( const void* pBuffer, int bytes )
{
	DBG_ASSERT( m_Socket != INVALID_SOCKET );
	DBG_ASSERT( pBuffer != 0 );
	DBG_ASSERT( bytes >= 0 );

	int result = send( m_Socket, (const char*)(pBuffer), bytes, 0 );

	return result;
}

/////////////////////////////////////////////////////
/// Method: SendTo
/// Params: [in]pBuffer, [in]bytes, [in]pSockAddr
///
/////////////////////////////////////////////////////
int BaseSocket::SendTo( const void* pBuffer, int bytes, const sockaddr_in* pSockAddr )
{
	DBG_ASSERT( m_Socket != INVALID_SOCKET );
	DBG_ASSERT( pBuffer != 0 );
	DBG_ASSERT( bytes >= 0 );

	int result = sendto( m_Socket, (const char*)(pBuffer), bytes, 0, (const sockaddr*)(pSockAddr), sizeof( sockaddr_in ) );
	
	return result;
}

/////////////////////////////////////////////////////
/// Method: SetSockOpt
/// Params: [in]level, [in]name, [out]pValue, [in]bytes
///
/////////////////////////////////////////////////////
int BaseSocket::SetSockOpt( int level, int name, const void* pValue, int bytes )
{
	DBG_ASSERT( m_Socket != INVALID_SOCKET );
	DBG_ASSERT( pValue != 0 );

	int result = setsockopt( m_Socket, level, name, (const char*)(pValue), bytes );
	
	return result;
}

/////////////////////////////////////////////////////
/// Method: Shutdown
/// Params: [in]how
///
/////////////////////////////////////////////////////
int BaseSocket::Shutdown( int how )
{
	DBG_ASSERT( m_Socket != INVALID_SOCKET );

	int result = shutdown( m_Socket, how );

	return result;
}

#endif // BASE_SUPPORT_NETWORKING
