
/*===================================================================
	File: SockAddr.cpp
	Library: Network

	(C)Hidden Games
=====================================================================*/

#ifdef BASE_SUPPORT_NETWORKING

#include "CoreBase.h"

#include "PlatformNetworkBase.h"
#include "SockAddr.h"

using network::SockAddr;

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
SockAddr::SockAddr( ) : sockaddr_in()
{
}

/////////////////////////////////////////////////////
/// Constructor
/// Params: [in]sa
///
/////////////////////////////////////////////////////
SockAddr::SockAddr( const sockaddr_in& sa ) 
	: sockaddr_in()
{
	DBG_ASSERT( sa.sin_family == AF_INET );
	sin_family = AF_INET;
	sin_addr = sa.sin_addr;
	sin_port = sa.sin_port;
}

/////////////////////////////////////////////////////
/// Constructor
/// Params: [in]inAddr, [in]wPort
///
/////////////////////////////////////////////////////
SockAddr::SockAddr( unsigned long inAddr, unsigned short wPort ) 
	: sockaddr_in()
{
	sin_family = AF_INET;
	sin_addr.s_addr = inAddr;
	sin_port = htons( wPort );
}

/////////////////////////////////////////////////////
/// Constructor
/// Params: [in]inAddr, [in]wPort
///
/////////////////////////////////////////////////////
SockAddr::SockAddr( const in_addr& inAddr, unsigned short wPort ) 
	: sockaddr_in()
{
	sin_family = AF_INET;
	sin_addr = inAddr;
	sin_port = htons( wPort );
}

/////////////////////////////////////////////////////
/// Method: GetInAddr
/// Params: None
///
/////////////////////////////////////////////////////
in_addr SockAddr::GetInAddr() const
{
	return sin_addr;
}

/////////////////////////////////////////////////////
/// Method: GetPtr
/// Params: None
///
/////////////////////////////////////////////////////
const sockaddr_in* SockAddr::GetPtr() const
{
	return this;
}

/////////////////////////////////////////////////////
/// Method: GetAddr
/// Params: None
///
/////////////////////////////////////////////////////
unsigned long SockAddr::GetAddr() const
{
	return( ntohl( sin_addr.s_addr ) );
}

/////////////////////////////////////////////////////
/// Method: GetPort
/// Params: None
///
/////////////////////////////////////////////////////
unsigned short SockAddr::GetPort() const
{
	return( ntohs( sin_port ) );
}

/////////////////////////////////////////////////////
/// Method: GetStr
/// Params: [in]strAddr, [in]includePort
///
/////////////////////////////////////////////////////
void SockAddr::GetStr( unsigned short* strAddr, bool includePort ) const
{
	DBG_ASSERT( strAddr != 0 );

#ifdef _WIN32    
/*	int iChars = wsprintfW( strAddr, L"%d.%d.%d.%d", 
                            sin_addr.S_un.S_un_b.s_b1,
                            sin_addr.S_un.S_un_b.s_b2,
                            sin_addr.S_un.S_un_b.s_b3,
                            sin_addr.S_un.S_un_b.s_b4 );

	if( bIncludePort )
	{
		unsigned short strPort[8];
		wsprintfW( strPort, L":%d", GetPort() );
		lstrcpyW( strAddr + iChars, strPort );
	}
*/
#endif // _WIN32
}

/////////////////////////////////////////////////////
/// Method: Clear
/// Params: None
///
/////////////////////////////////////////////////////
void SockAddr::Clear()
{
	sin_family = AF_INET;
	memset( &sin_addr, 0, sizeof( in_addr ) );
	memset( &sin_port, 0, sizeof( u_short ) );
}

#endif // BASE_SUPPORT_NETWORKING
