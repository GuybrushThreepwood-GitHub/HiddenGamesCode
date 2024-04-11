
/*===================================================================
 File: NetworkService.cpp
 Library: Network
 
 (C)Hidden Games
 =====================================================================*/

#ifdef BASE_PLATFORM_WINDOWS

#ifdef BASE_SUPPORT_BONJOUR

#include <winsock2.h>

#include "CoreBase.h"
#include "NetworkBase.h"

#include "Network/NetworkService.h"

namespace
{
	//static core::app::Mutex changeLockMutex;

	// Note: the select() implementation on Windows (Winsock2)
	//fails with any timeout much larger than this
	#define LONG_TIME 100000000
	static volatile int stopNow = 0;
	static volatile int timeOut = LONG_TIME;
}

bool NetworkService::NetBrowseProcessingThread = false;

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
NetworkService::NetworkService()
{
    m_NetServiceAdvertise = 0;
    m_NetServiceBrowser = 0;

	m_BluetoothSupport = false;
	m_SupportedInterfaces = kDNSServiceInterfaceIndexAny;

	//changeLockMutex.Initialise();
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
NetworkService::~NetworkService()
{
    Stop();
}

/////////////////////////////////////////////////////
/// Method: StartAdvertising
/// Params: [in]szDomain, [in]szType, [in]szName
///
/////////////////////////////////////////////////////
void NetworkService::StartAdvertising( const char* szDomain, const char* szType, const char* szName, int port )
{
    StopAdvertising();

    // advertise
    DNSServiceErrorType err = kDNSServiceErr_NoError;
    DNSServiceFlags flags = 0;

    if( m_BluetoothSupport )
        flags |= kDNSServiceFlagsIncludeP2P;

    err = DNSServiceRegister(&m_NetServiceAdvertise, 
                             flags,
							 m_SupportedInterfaces,
                             szName,
                             szType,
                             szDomain,
                             0,
                             port,
                             0,
                             0,
                             RegisterReply,
                             this );

    if( err == kDNSServiceErr_NoError )
    {
		ServiceEvents(m_NetServiceAdvertise);
    }
    else
    {
        DBGLOG( "NETWORKSERVICE: *ERROR* DNSServiceRegister failed with %d\n", err );

		if( m_NetServiceAdvertise != 0 )
		{
			DNSServiceRefDeallocate(m_NetServiceAdvertise);
			m_NetServiceAdvertise = 0;
		}
    }
}

/////////////////////////////////////////////////////
/// Method: StartBrowsing
/// Params: [in]szDomain, [in]szType
///
/////////////////////////////////////////////////////
void NetworkService::StartBrowsing( const char* szDomain, const char* szType )
{
    StopBrowsing();

    // find
    DNSServiceErrorType err = kDNSServiceErr_NoError;
	DNSServiceFlags flags = 0;

	if( m_BluetoothSupport )
		flags |= kDNSServiceFlagsIncludeP2P;

    err = DNSServiceBrowse( &m_NetServiceBrowser, 
							flags, 
							m_SupportedInterfaces,
                            szType,
                            szDomain,
                            BrowseReply,
                            this );

    if( err == kDNSServiceErr_NoError )
    {
		// add to the map
		browseFdMap[m_NetServiceBrowser] = DNSServiceRefSockFD(m_NetServiceBrowser);

		// start the thread
		m_NetBrowseThread.Initialise( NetBrowseThread, this );
    }
    else
    {
        DBGLOG( "NETWORKSERVICE: *ERROR* DNSServiceBrowse failed with %d\n", err );

		if( m_NetServiceBrowser != 0 )
		{
			DNSServiceRefDeallocate(m_NetServiceBrowser);
			m_NetServiceBrowser = 0;
		}
    }
}

/////////////////////////////////////////////////////
/// Method: StopAdvertising
/// Params: None
///
/////////////////////////////////////////////////////
void NetworkService::StopAdvertising()
{
    if(m_NetServiceAdvertise != 0)
    {
		SafeRemoveService(m_NetServiceAdvertise);

        DNSServiceRefDeallocate(m_NetServiceAdvertise);
        m_NetServiceAdvertise = 0;
    }
}

/////////////////////////////////////////////////////
/// Method: StopBrowsing
/// Params: None
///
/////////////////////////////////////////////////////
void NetworkService::StopBrowsing()
{
    NetworkService::NetBrowseProcessingThread = false;
	m_NetBrowseThread.Release(); 

    if(m_NetServiceBrowser != 0)
	{
		SafeRemoveService(m_NetServiceBrowser);

		DNSServiceRefDeallocate(m_NetServiceBrowser);
        m_NetServiceBrowser = 0;
	}
}

/////////////////////////////////////////////////////
/// Method: Stop
/// Params: None
///
/////////////////////////////////////////////////////
void NetworkService::Stop()
{
    StopAdvertising();

    StopBrowsing();

	//changeLockMutex.Lock();
	for ( auto it = browseFdMap.begin() ; it != browseFdMap.end() ; it++ )
		DNSServiceRefDeallocate(it->first);

	browseFdMap.clear();
	//changeLockMutex.UnLock();
}

/////////////////////////////////////////////////////
/// Method: DoesDiscoveryExist
/// Params: [in]data
///
/////////////////////////////////////////////////////
bool NetworkService::DoesDiscoveryExist( const NetworkService::DiscoveryData* data )
{
    if( data != 0 )
    {
        std::vector<NetworkService::DiscoveryData>::iterator it = discoveryList.begin();
        while( it != discoveryList.end() )
        {
            if( (*it).domain.compare( data->domain ) == 0 &&
                (*it).type.compare( data->type ) == 0 &&
                (*it).name.compare( data->name ) == 0 &&
                (*it).hostTarget.compare( data->hostTarget ) == 0 &&
                (*it).addr.compare( data->addr ) == 0 &&
                (*it).port == data->port )
                return true;

            it++;
        }
    }
    return false;
}

/////////////////////////////////////////////////////
/// Method: RemoveDiscovery
/// Params: [in]data
///
/////////////////////////////////////////////////////
void NetworkService::RemoveDiscovery( const NetworkService::DiscoveryData* data )
{
    if( data != 0 )
    {
        std::vector<NetworkService::DiscoveryData>::iterator it = discoveryList.begin();
        while( it != discoveryList.end() )
        {
            bool remove = false;
            if( (*it).domain.compare( data->domain ) == 0 &&
                (*it).type.compare( data->type ) == 0 &&
                (*it).name.compare( data->name ) == 0 )
                remove = true;

            if( remove )
                it = discoveryList.erase(it);
            else
                it++;
        }
    }
}

/////////////////////////////////////////////////////
/// Static Method: RegisterReply
/// Params:
///
/////////////////////////////////////////////////////
void DNSSD_API NetworkService::RegisterReply(DNSServiceRef sdRef, const DNSServiceFlags flags, DNSServiceErrorType errorCode,
                                const char *name, const char *regtype, const char *domain, void *context)
{
    NetworkService* pObj = 0;
	if( context != 0 )
        pObj = reinterpret_cast<NetworkService *>(context);

    DBGLOG("NETWORKSERVICE: Registered service %s.%s%s: \n", name, regtype, domain);

    if(errorCode == kDNSServiceErr_NoError)
    {
        if (flags & kDNSServiceFlagsAdd)
        {
            DBGLOG("Name now registered and active\n");
        }
        else
            DBGLOG("Name registration removed\n");
    }
    else if (errorCode == kDNSServiceErr_NameConflict)
    {
        DBGLOG("NETWORKSERVICE: Name in use, please choose another\n");
    }
    else
        DBGLOG("NETWORKSERVICE: *ERROR* RegisterReply Error %d\n", errorCode);

    if (!(flags & kDNSServiceFlagsMoreComing))
    {
		// done
    }
}

/////////////////////////////////////////////////////
/// Static Method: BrowseReply
/// Params:
///
/////////////////////////////////////////////////////
void DNSSD_API NetworkService::BrowseReply(DNSServiceRef sdRef, const DNSServiceFlags flags, uint32_t interfaceIndex, DNSServiceErrorType errorCode,
										const char *replyName, const char *replyType, const char *replyDomain, void *context)
{
    NetworkService* pObj = 0;
	if( context != 0 )
        pObj = reinterpret_cast<NetworkService *>(context);

    bool serviceToAdd = false;

    if( (flags & kDNSServiceFlagsAdd) &&
         (errorCode == kDNSServiceErr_NoError) )
    {
        serviceToAdd = true;

        // resolve
        DNSServiceRef client = 0;
        DNSServiceErrorType err = kDNSServiceErr_NoError;
        DNSServiceFlags resFlags = 0;

        if( pObj &&
            pObj->SupportBluetooth() )
            resFlags |= kDNSServiceFlagsIncludeP2P;

		err = DNSServiceResolve ( &client,
                                  resFlags,
                                  interfaceIndex,
                                  replyName,
                                  replyType,
                                  replyDomain,
                                  ResolveReply,
                                  context );

        if ( err == kDNSServiceErr_NoError )
        {
			if( pObj )
			{
				//changeLockMutex.Lock();
					pObj->browseFdMap[client] = DNSServiceRefSockFD(client);
				//changeLockMutex.UnLock();
			}
        }
        else
        {
			DBGLOG( "NETWORKSERVICE: *ERROR* DNSServiceResolve failed with %d\n", errorCode );
			if( client != 0 )
				DNSServiceRefDeallocate(client);
        }
    }


    if(errorCode == kDNSServiceErr_NoError)
	{
		if( serviceToAdd )
		{
	        DBGLOG("Add - %6X%3d %-25s %-25s %s\n", flags, interfaceIndex, replyDomain, replyType, replyName);

			if( pObj )
			{
                NetworkService::DiscoveryData newData;
                newData.domain = std::string(replyDomain);
                newData.type = std::string(replyType);
                newData.name = std::string(replyName);
				newData.interfaceId = interfaceIndex;

				pObj->discoveryList.push_back(newData);
			}
		}
		else
		{
			// remove
			if( pObj )
			{
                std::vector<NetworkService::DiscoveryData>::iterator it = pObj->discoveryList.begin();

				while( it != pObj->discoveryList.end() )
				{
                    std::string stringDomain(replyDomain);
                    std::string stringType(replyType);
                    std::string stringName(replyName);

					if( (stringDomain.compare( (*it).domain ) == 0) &&
						(stringType.compare( (*it).type ) == 0) &&
						(stringName.compare( (*it).name ) == 0) &&
						interfaceIndex == (*it).interfaceId )
					{
						pObj->discoveryList.erase(it);
						break;
					}

					it++;
				}
			}

			DBGLOG("Rmv - %6X%3d %-25s %-25s %s\n", flags, interfaceIndex, replyDomain, replyType, replyName);
		}
	}
    else
	{
		DBGLOG( "NETWORKSERVICE: *ERROR* BrowseReply failed with %d\n", errorCode );
	}

    if (!(flags & kDNSServiceFlagsMoreComing))
    {
        // done
    }
}

/////////////////////////////////////////////////////
/// Static Method: ResolveReply
/// Params:
///
/////////////////////////////////////////////////////
void DNSSD_API NetworkService::ResolveReply(DNSServiceRef sdRef, DNSServiceFlags flags, uint32_t interfaceIndex, DNSServiceErrorType errorCode,
											const char *fullname, const char *hosttarget, uint16_t port, uint16_t txtLen, const unsigned char *txtRecord, void *context )
{
    NetworkService* pObj = 0;
	if( context != 0 )
        pObj = reinterpret_cast<NetworkService *>(context);

    if( errorCode == kDNSServiceErr_NoError )
    {
        DNSServiceRef client = 0;
        DNSServiceErrorType err = kDNSServiceErr_NoError;

		err = DNSServiceGetAddrInfo( &client,
                                     kDNSServiceFlagsTimeout,
                                     interfaceIndex,
                                     kDNSServiceProtocol_IPv4,
                                     hosttarget,
                                     GetAddress,
                                     context );

        if ( err == kDNSServiceErr_NoError )
        {
			//changeLockMutex.Lock();
				pObj->browseFdMap[client] = DNSServiceRefSockFD(client);
			//changeLockMutex.UnLock();

			if( pObj )
			{
                std::vector<NetworkService::DiscoveryData>::iterator it = pObj->discoveryList.begin();

				bool found = false;

				while( it != pObj->discoveryList.end() )
				{
					char fullnameUTF8[kDNSServiceMaxDomainName];
					err = DNSServiceConstructFullName( &fullnameUTF8[0], (*it).name.c_str(), (*it).type.c_str(), (*it).domain.c_str() );

					if ( err == kDNSServiceErr_NoError )
					{
						std::string stringFullname(fullnameUTF8);

						if( (stringFullname.compare( std::string(fullname) ) == 0) &&
							interfaceIndex == (*it).interfaceId )
						{
							(*it).hostTarget = std::string(hosttarget);
							(*it).port = port;
							found = true;
							break;
						}
					}
					it++;
				}

				if( !found )
				{
					DBGLOG( "NETWORKSERVICE: *WARNING* Discovery data not found for %s - cannot resolve ip\n", fullname );
				}
			}
        }
		else
		{
			DBGLOG( "NETWORKSERVICE: *ERROR* DNSServiceGetAddrInfo failed with %d\n", err );

			if( client != 0 )
				DNSServiceRefDeallocate(client);
		}
    }
	else
	{
		DBGLOG( "NETWORKSERVICE: *ERROR* ResolveReply failed with %d\n", errorCode );

		if( sdRef != 0 )
			DNSServiceRefDeallocate(sdRef);
	}
}

/////////////////////////////////////////////////////
/// Static Method: GetAddress
/// Params:
///
/////////////////////////////////////////////////////
void DNSSD_API NetworkService::GetAddress( DNSServiceRef sdRef, DNSServiceFlags flags, uint32_t interfaceIndex, DNSServiceErrorType errorCode,
										const char *hostname, const struct sockaddr *address, uint32_t ttl, void *context )
{
    NetworkService* pObj = 0;
	if( context != 0 )
        pObj = reinterpret_cast<NetworkService *>(context);

    if ( errorCode == kDNSServiceErr_NoError )
    {
        const sockaddr_in *in = (const sockaddr_in *) address;
        char *ip = inet_ntoa( in->sin_addr );

		if( pObj )
		{
            std::vector<NetworkService::DiscoveryData>::iterator it = pObj->discoveryList.begin();

			while( it != pObj->discoveryList.end() )
			{
                std::string stringHostName( hostname );

				if( (stringHostName.compare( (*it).hostTarget ) == 0) &&
					interfaceIndex == (*it).interfaceId )
				{
                    (*it).addr = std::string(ip);
					DBGLOG( "Resolved ip: %s:%d for %s\n", (*it).addr.c_str(), (*it).port, hostname );
					break;
				}

				it++;
			}
		}
    }
	else
	{
		DBGLOG( "NETWORKSERVICE: *ERROR* GetAddress failed with %d\n", errorCode );
		if( sdRef != 0 )
			DNSServiceRefDeallocate(sdRef);
	}

    if ( !(flags & kDNSServiceFlagsMoreComing ) )
    {
		// done
    }
}

/////////////////////////////////////////////////////
/// Static Method: NetBrowseThread
/// Params: [in]threadData
///
/////////////////////////////////////////////////////
void* NetworkService::NetBrowseThread(void *threadData)
{
	if( threadData != 0 )
	{
        NetworkService* pObj = reinterpret_cast<NetworkService *>(threadData);
        NetworkService::NetBrowseProcessingThread = true;
		struct timeval tv;

        while( NetworkService::NetBrowseProcessingThread )
		{
			//changeLockMutex.Lock();

			fd_set readfds;
            int max_sd = 0;
			FD_ZERO(&readfds);

            int fdIdx = 0;
			for ( auto i = pObj->browseFdMap.begin() ; i != pObj->browseFdMap.end() ; i++ )
            {
                fdIdx = i->second;
				FD_SET(i->second, &readfds);
                
                if( fdIdx > max_sd )
                    max_sd = fdIdx;
            }
		
            tv.tv_sec = timeOut;
            tv.tv_usec = 0;
		
			int result = select(max_sd+1, &readfds, (fd_set*)0, (fd_set*)0, &tv);
		
			if ( result > 0 ) 
			{
				//
				// While iterating through the loop, the callback functions might delete
				// the client pointed to by the current iterator, so I have to increment
				// it BEFORE calling DNSServiceProcessResult
				//
				for ( auto i = pObj->browseFdMap.begin() ; i != pObj->browseFdMap.end() ; ) 
				{
                    if( !NetworkService::NetBrowseProcessingThread )
					{
						//changeLockMutex.UnLock();
						return 0;
					}

					auto j = i++;
					if (FD_ISSET(j->second, &readfds) ) 
					{
						DNSServiceErrorType err = DNSServiceProcessResult(j->first);

						if( err != kDNSServiceErr_NoError )
						{
							DBGLOG( "NETWOKRSERVICE: NetBrowseThread DNSServiceProcessResult error %d\n", err );
						}
					}
				}
			}

			//changeLockMutex.UnLock();

			// let the thread breathe
			Sleep(1000);
		}
	}

	return 0;
}

/////////////////////////////////////////////////////
/// Static Method: NetBrowseThread
/// Params: [in]threadData
///
/////////////////////////////////////////////////////
void* NetworkService::NetServiceThread(void* threadData)
{
	return 0;
}

/////////////////////////////////////////////////////
/// Method: ServiceEvents
/// Params: [in]sdRef
///
/////////////////////////////////////////////////////
void NetworkService::ServiceEvents(DNSServiceRef sdRef)
{
    int socketFD = DNSServiceRefSockFD(sdRef);
    int nfds = socketFD + 1;
    fd_set readfds;
    struct timeval tv;
    int result;

	stopNow = 0;

    while (!stopNow)
    {
        FD_ZERO(&readfds);
        FD_SET(socketFD, &readfds);

        tv.tv_sec = timeOut;
        tv.tv_usec = 0;

        result = select(nfds, &readfds, (fd_set*)0, (fd_set*)0, &tv);
        if (result > 0)
        {
            DNSServiceErrorType err = kDNSServiceErr_NoError;

            if (FD_ISSET(socketFD, &readfds))
            {
                err = DNSServiceProcessResult(sdRef);

                if (err == kDNSServiceErr_NoError)
                    stopNow = 1;
            }
        }
        else
        {
            DBGLOG("select() returned %d errno %d %s\n", result, errno, strerror(errno));
            //if (errno != EINTR)
                stopNow = 1;
        }
    }
}

/////////////////////////////////////////////////////
/// Method: SafeRemoveService
/// Params: [in]sdRef
///
/////////////////////////////////////////////////////
void NetworkService::SafeRemoveService(DNSServiceRef sdRef)
{
	//changeLockMutex.Lock();
		auto it = browseFdMap.find( sdRef );
		if ( it != browseFdMap.end() )
			browseFdMap.erase( it );
	//changeLockMutex.UnLock();
}

#endif // BASE_SUPPORT_BONJOUR

#endif // BASE_PLATFORM_WINDOWS
