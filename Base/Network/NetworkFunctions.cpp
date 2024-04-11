
/*===================================================================
	File: NetworkFunctions.cpp
	Library: Network

	(C)Hidden Games
=====================================================================*/

#ifdef BASE_SUPPORT_NETWORKING

#include "CoreBase.h"

#include "PlatformNetworkBase.h"
#include "NetworkFunctions.h"

namespace
{
	const int MAX_IPS = 16;
	const int MAX_HOSTNAME_LEN = 256;

	static unsigned char localIP[MAX_IPS][4];

#ifdef BASE_PLATFORM_WINDOWS
	// winsock data structure
	static WSADATA wsaData;
#endif // BASE_PLATFORM_WINDOWS
}

/////////////////////////////////////////////////////
/// Function: Initialise
/// Params: None
///
/////////////////////////////////////////////////////
int network::Initialise( void )
{
#ifdef BASE_PLATFORM_WINDOWS
	DBGLOG( "----- Winsock Setup -----\n" );

	unsigned short version;
	int err = -1;

	version = MAKEWORD( 2, 2 );
	err = WSAStartup( version, &wsaData ); 

	if( err < 0 )
	{
		DBGLOG( "WINSOCK: *ERROR* Winsock startup failed" );
		return 1;
	}

	DBGLOG( "\n" );
	DBGLOG( "Winsock Started\n" );
	DBGLOG( "--------------------------------------\n" );
	DBGLOG( "Winsock Version: %s\n", wsaData.szDescription );
	DBGLOG( "--------------------------------------\n" );
	DBGLOG( "Winsock Status: %s\n", wsaData.szSystemStatus );
	DBGLOG( "--------------------------------------\n" );
	DBGLOG( "\n" );

	DBGLOG( "----- Winsock Setup Complete -----\n" );
#endif // BASE_PLATFORM_WINDOWS

	return 0;
}

/////////////////////////////////////////////////////
/// Method: Shutdown
/// Params: None
///
/////////////////////////////////////////////////////
void network::Shutdown( void )
{
#ifdef BASE_PLATFORM_WINDOWS
	DBGLOG( "----- Winsock Shutdown -----\n" );

	int err = -1;
	err = WSACleanup();

	if( err < 0 )
	{
		DBGLOG( "WINSOCK: *ERROR* Winsock shutdown failed" );
		return;
	}

	DBGLOG( "----- Winsock Shutdown Complete -----\n" );
#endif // BASE_PLATFORM_WINDOWS
}

/////////////////////////////////////////////////////
/// Function: TextIPAddressToULong
/// Params: [in]szAddress
///
/////////////////////////////////////////////////////
unsigned long network::TextIPAddressToULong( const char *szAddress )
{
	unsigned long a = INADDR_NONE;

	if( ( a = inet_addr( szAddress ) ) == INADDR_NONE )
	{
		// address lookup failed, try and resolve it as a www address
		hostent *pHostEnt = gethostbyname( szAddress );

		if( pHostEnt == 0 )
		{
			return( INADDR_NONE );
		}
		
		// resolved address
		a = *((unsigned long *)pHostEnt->h_addr_list[0]);
	}

	return(a);
}

/////////////////////////////////////////////////////
/// Function: GetLocalIPAddress
/// Params: None
///
/////////////////////////////////////////////////////
void network::GetLocalIPAddress() 
{
	char hostname[MAX_HOSTNAME_LEN];
	struct hostent* hostInfo;
	char* p;
	int	ip;
	int	n;

	if( gethostname( hostname, MAX_HOSTNAME_LEN ) == SOCKET_ERROR ) 
	{
		return;
	}

	hostInfo = gethostbyname( hostname );
	if( !hostInfo ) 
	{
		return;
	}

	DBGLOG( "NETWORKFUNCTIONS: Hostname: %s\n", hostInfo->h_name );
	n = 0;
	while( ( p = hostInfo->h_aliases[n++] ) != 0 ) 
	{
		DBGLOG( "NETWORKFUNCTIONS: Alias: %s\n", p );
	}

	if ( hostInfo->h_addrtype != AF_INET ) 
	{
		return;
	}

	int numIP = 0;
	while( ( p = hostInfo->h_addr_list[numIP] ) != 0 && numIP < MAX_IPS ) 
	{
		ip = ntohl( *(int *)p );
		localIP[ numIP ][0] = p[0];
		localIP[ numIP ][1] = p[1];
		localIP[ numIP ][2] = p[2];
		localIP[ numIP ][3] = p[3];
		DBGLOG( "NETWORKFUNCTIONS: IP: %i.%i.%i.%i\n", ( ip >> 24 ) & 0xff, ( ip >> 16 ) & 0xff, ( ip >> 8 ) & 0xff, ip & 0xff );
		numIP++;
	}
}

/////////////////////////////////////////////////////
/// Function: ResolveNameToIP
/// Params: [in]hostName
///
/////////////////////////////////////////////////////
const char* network::ResolveNameToIP( const char* hostName )
{
	struct hostent *h = 0;
    if( (h=gethostbyname(hostName)) == 0 ) 
	{  
		return 0;
	}
	else
	{
		//DBGLOG("NETWORKFUNCTIONS: Host name  : %s\n", h->h_name);
		//DBGLOG("NETWORKFUNCTIONS: IP Address : %s\n", inet_ntoa(*((struct in_addr *)h->h_addr)));

		return inet_ntoa(*((struct in_addr *)h->h_addr));
	}

	return 0;
}

/////////////////////////////////////////////////////
/// Function: ResolveNameToIP
/// Params: [in]hostName, [in]family, [in]socketType
///
/////////////////////////////////////////////////////
const char* network::ResolveNameToIP( const char* hostName, int family, int socketType )
{
	struct addrinfo hints;
	struct addrinfo *result = 0, *rp = 0;
	int s = 0;

	std::memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_family = family;		// Allow IPv4 or IPv6
    hints.ai_socktype = socketType;	// Datagram socket
    //hints.ai_flags = AI_PASSIVE;		// For wildcard IP address
    hints.ai_protocol = 0;				// Any protocol
    //hints.ai_canonname = 0;
    //hints.ai_addr = 0;
    //hints.ai_next = 0;

	s = getaddrinfo( hostName, 0, &hints, &result );
	if( s != 0 )
	{
        DBGLOG("NETWORKFUNCTIONS: *ERROR* getaddrinfo failed with %d\n", s);
    }

	const char* addressReturn = 0;
	for (rp = result; rp != 0; rp = rp->ai_next) 
	{
		if( rp->ai_socktype == socketType )
		{
			struct sockaddr_in *sockaddr_ipv4 = (struct sockaddr_in *) rp->ai_addr;
			DBGLOG( "NETWORKFUNCTIONS: Resolved address = %s\n", inet_ntoa(sockaddr_ipv4->sin_addr) );

			addressReturn = inet_ntoa(sockaddr_ipv4->sin_addr);
		}
	}

	return addressReturn;
}

/////////////////////////////////////////////////////
/// Function: GetMachineName
/// Params: [in/out]machineName
///
/////////////////////////////////////////////////////
void network::GetMachineName(char* machineName)
{
    char bufferName[MAX_HOSTNAME_LEN];    
	std::memset(bufferName, 0, MAX_HOSTNAME_LEN);

#ifdef BASE_PLATFORM_WINDOWS
    DWORD bufCharCount = MAX_HOSTNAME_LEN;

    if( !GetComputerNameA( bufferName, &bufCharCount ) )
    {
        std::strcpy(bufferName, "Unknown_Host_Name");
    }
#else
    if( gethostname(bufferName, MAX_HOSTNAME_LEN) != 0 )
    {
        std::strcpy(bufferName, "Unknown_Host_Name");
    }
#endif

    strncpy(machineName, bufferName, MAX_HOSTNAME_LEN);
}

#endif // BASE_SUPPORT_NETWORKING
