
/*===================================================================
 File: NetworkService.mm
 Library: Network
 
 (C)Hidden Games
 =====================================================================*/

#ifdef BASE_PLATFORM_iOS

#ifdef BASE_SUPPORT_BONJOUR

#import "CoreBase.h"

#import <Foundation/Foundation.h>
#import <arpa/inet.h>

#import "Network/NetworkService.h"

/// BonjourNetworkService - simple object for handling the publishing and browsing of bonjour services
@interface BonjourNetworkService: NSObject <NSNetServiceDelegate, NSNetServiceBrowserDelegate>
{
    //id theDelegate;

    BOOL hasPublishedService;
    BOOL isBrowsingForServices;
    BOOL isBrowsingForDomains;

    int32_t serviceFlags;

    NSString* storedDomain;
    NSString* storedType;
    NSString* storedName;
    int storedPort;

    id storedDelegate;

    NetworkService* pNetworkService;
}

// publishing
@property (nonatomic, readwrite, strong) NSNetService* netService;
// browsing
@property (nonatomic, readwrite, strong) NSNetServiceBrowser* netServiceBrowser;

@property (nonatomic, readonly, strong) NSString* storedDomain;
@property (nonatomic, readonly, strong) NSString* storedType;
@property (nonatomic, readonly, strong) NSString* storedName;
@property (nonatomic, readonly) int storedPort;

@property (nonatomic, readonly, strong) id storedDelegate;

// service browsing
@property (nonatomic, readonly, strong) NSMutableSet* availableServices;
@property (nonatomic, readonly, strong) NSMutableSet* pendingServicesToAdd;
@property (nonatomic, readonly, strong) NSMutableSet* pendingServicesToRemove;

// domain browsing
@property (nonatomic, readonly, strong) NSMutableSet* availableDomains;
@property (nonatomic, readonly, strong) NSMutableSet* pendingDomainsToAdd;
@property (nonatomic, readonly, strong) NSMutableSet* pendingDomainsToRemove;

- (id)init:(NetworkService *)servicePtr;

//- (id)delegate;
//- (void)setDelegate:(id)delegate;
- (void) setFlags: (int32_t)flags;

-(NSMutableSet *) getAvailableDomains;
-(NSMutableSet *) getAvailableServices;
-(int32_t) getFlags;
-(bool)hasPublishedNSService;
-(bool)isSearchingForServices;
-(bool)isSearchingForDomains;

// publishing
-(void) publishService:(NSString *)serviceDomain ofType:(NSString *)serviceType withName:(NSString *)serviceName onPort:(int)whichPort;

// browsing
-(void)searchForServices:(NSString *)serviceDomain ofType:(NSString *)serviceType;
-(void)searchForDomains;

-(void) stop;

@end

/////////////////////////////////////////////////////

@implementation BonjourNetworkService

@synthesize storedDomain;
@synthesize storedType;
@synthesize storedName;
@synthesize storedPort;
@synthesize storedDelegate;

-(id) init:(NetworkService *)servicePtr;
{
    if((self = [super init]))
    {
        //theDelegate = nil;
        pNetworkService = servicePtr;

        hasPublishedService = false;
        isBrowsingForServices = false;
        isBrowsingForDomains = false;

        self.netServiceBrowser = [[NSNetServiceBrowser alloc] init];
        [self.netServiceBrowser setDelegate:self];

        _availableServices = [[NSMutableSet alloc] init];
        _pendingServicesToAdd = [[NSMutableSet alloc] init];
        _pendingServicesToRemove = [[NSMutableSet alloc] init];

        _availableDomains = [[NSMutableSet alloc] init];
        _pendingDomainsToAdd = [[NSMutableSet alloc] init];
        _pendingDomainsToRemove = [[NSMutableSet alloc] init];
    }
    return self;
}

/*- (id)initWithDelegate:(id)delegate
{

}

/// delegate - gets the delegate
- (id)delegate
{
	return theDelegate;
}

/// setDelegate - sets the delegate
- (void)setDelegate:(id)delegate
{
	theDelegate = delegate;
}*/

/// setFlags - sets the flags for the service
- (void) setFlags: (int32_t)flags
{
	serviceFlags = flags;
}

/// getAvailableDomains - gives the current list of available domains as NSStrings
-(NSMutableSet *) getAvailableDomains
{
	return _availableDomains;
}

/// getAvailableServices - gives the current list of available NSNetServices
-(NSMutableSet *) getAvailableServices
{
	return _availableServices;
}

/// getFlags - gets the current flags
-(int32_t) getFlags
{
	return serviceFlags;
}

/// hasPublishedNSService - has this object published a sevice
-(bool)hasPublishedNSService
{
	return hasPublishedService;
}

/// isSearchingForServices - is this object searching for services
-(bool)isSearchingForServices
{
	return isBrowsingForServices;
}

/// isSearchingForDomains - is this object searching a services
-(bool)isSearchingForDomains
{
	return isBrowsingForDomains;
}

/// publishService - will attempt to publish a new service (if object is not already publishing)
-(void)publishService:(NSString *)serviceDomain ofType:(NSString *)serviceType withName:(NSString *)serviceName onPort:(int)whichPort
{
	if( !hasPublishedService )
	{
		// bonjour publish
		if( self.netService == nil )
			self.netService = [[NSNetService alloc] initWithDomain:serviceDomain
														type:serviceType
														name:serviceName
														port:whichPort];
		
        [self.netService setIncludesPeerToPeer:YES];
		[self.netService setDelegate:self];
		
		// publish the service
		//[self.netService publish];
		[self.netService publishWithOptions:0/*NSNetServiceNoAutoRename*/];
		
		storedDomain = serviceDomain;
		storedType = serviceType;
		storedName = serviceName;
		storedPort = whichPort;
		storedDelegate = self;
	}
}

/// searchForServices - looks for specific services based on the domain and service type
-(void)searchForServices:(NSString *)serviceDomain ofType:(NSString *)serviceType
{
	if( !isBrowsingForServices )
	{
		isBrowsingForServices = true;
        
        [self.netServiceBrowser setIncludesPeerToPeer:YES];
		[self.netServiceBrowser searchForServicesOfType:serviceType inDomain:serviceDomain];
	}
}

/// searchForDomains - looks for browsable domains 
-(void)searchForDomains
{
	if( !isBrowsingForDomains )
	{
		isBrowsingForDomains = true;
		[_netServiceBrowser searchForBrowsableDomains];
	}
}


/// stop - stops a service if published, or a browser if browsing
-(void) stop
{
	if( hasPublishedService )
	{
		[self.netService setDelegate:nil];
		[self.netService stop];
		self.netService = nil;
		
		hasPublishedService = false;
	}
	
	if( self.netServiceBrowser != nil )
	{
		[self.netServiceBrowser setDelegate:nil];
		[self.netServiceBrowser stop];
		
		[self.pendingServicesToAdd removeAllObjects];
		[self.pendingServicesToRemove removeAllObjects];
		[self.availableServices removeAllObjects];
		
		[self.pendingDomainsToAdd removeAllObjects];
		[self.pendingDomainsToRemove removeAllObjects];
		[self.availableDomains removeAllObjects];
	}
	
	[_availableDomains removeAllObjects];
	[_availableServices removeAllObjects];
	
	isBrowsingForServices = false;
	isBrowsingForDomains = false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Sent to the NSNetService instance's delegate prior to advertising the service on the network. If for some reason the service cannot be published, the delegate will not receive this message, and an error will be delivered to the delegate via the delegate's -netService:didNotPublish: method.
- (void)netServiceWillPublish:(NSNetService *)sender
{
	NSLog( @"NSNETSERVICEDELEGATE: netServiceWillPublish" );
}

// Sent to the NSNetService instance's delegate when the publication of the instance is complete and successful.
- (void)netServiceDidPublish:(NSNetService *)sender
{
	NSLog( @"NSNETSERVICEDELEGATE: netServiceDidPublish" );
	
	hasPublishedService = true;
	
	// call CommServiceListener delegate
    //if ([[self delegate] respondsToSelector:@selector(servicePublished)])
    //	[[self delegate] servicePublished];
}

// Sent to the NSNetService instance's delegate when an error in publishing the instance occurs. The error dictionary will contain two key/value pairs representing the error domain and code (see the NSNetServicesError enumeration above for error code constants). It is possible for an error to occur after a successful publication.
- (void)netService:(NSNetService *)sender didNotPublish:(NSDictionary *)errorDict
{
	NSLog( @"NSNETSERVICEDELEGATE: didNotPublish - %@", errorDict );
	
	hasPublishedService = false;
}

// Sent to the NSNetService instance's delegate prior to resolving a service on the network. If for some reason the resolution cannot occur, the delegate will not receive this message, and an error will be delivered to the delegate via the delegate's -netService:didNotResolve: method.
- (void)netServiceWillResolve:(NSNetService *)sender
{
	NSLog( @"NSNETSERVICEDELEGATE: netServiceWillResolve" );
}

// Sent to the NSNetService instance's delegate when one or more addresses have been resolved for an NSNetService instance. Some NSNetService methods will return different results before and after a successful resolution. An NSNetService instance may get resolved more than once; truly robust clients may wish to resolve again after an error, or to resolve more than once.
- (void)netServiceDidResolveAddress:(NSNetService *)sender
{
	NSLog(@"NSNETSERVICEDELEGATE: netServiceDidResolveAddress" );
	
	NSLog(@"name: %@", sender.name);
	NSLog(@"hostName: %@", sender.hostName);
	NSLog(@"port: %d", sender.port);
	 
	// go through all addresses attached to this service (both IP4 and IP6 if required)
    for( NSData* data in sender.addresses)
	{
		char addressBuffer[INET6_ADDRSTRLEN];
		struct sockaddr_in* socketAddress = (struct sockaddr_in *)[data bytes];
		int sockFamily = socketAddress->sin_family;
        if( sockFamily == AF_INET )
		{
            const char* addressStr = 0;
            addressStr = inet_ntop( sockFamily, &(socketAddress->sin_addr), addressBuffer, sizeof(addressBuffer) );
            int port = 0;
            port = ntohs(socketAddress->sin_port);

            if( addressStr != 0 &&
                port )
            {
				NSLog( @"address %s : %d", addressStr, port );

                if( pNetworkService )
                {
                    NetworkService::DiscoveryData newData;

                    newData.domain = std::string([sender.domain UTF8String]);
                    newData.type = std::string([sender.type UTF8String]);
                    newData.name = std::string([sender.name UTF8String]);
                    newData.hostTarget = std::string([sender.hostName UTF8String]);
                    newData.addr = std::string(addressStr);
                    //newData.addr6;
                    newData.port = port;

                    if( !pNetworkService->DoesDiscoveryExist( &newData ) )
                        pNetworkService->discoveryList.push_back(newData);
                }
            }
		}
    }
	
	// once we have resolution, we have the ability to connect via address/port
    //if ([[self delegate] respondsToSelector:@selector(browsingAvailabilityChanges:whichService:)])
    //	[[self delegate] browsingAvailabilityChanges:CommServiceChange_ResolvedService whichService:sender];
}

// Sent to the NSNetService instance's delegate when an error in resolving the instance occurs. The error dictionary will contain two key/value pairs representing the error domain and code (see the NSNetServicesError enumeration above for error code constants).
- (void)netService:(NSNetService *)sender didNotResolve:(NSDictionary *)errorDict
{
	NSLog( @"NSNETSERVICEDELEGATE: didNotResolve - %@", errorDict );
}

// Sent to the NSNetService instance's delegate when the instance's previously running publication or resolution request has stopped.
- (void)netServiceDidStop:(NSNetService *)sender
{
	NSLog( @"NSNETSERVICEDELEGATE: netServiceDidStop" );
		
	// call CommServiceListener delegate
    //if ([[self delegate] respondsToSelector:@selector(serviceStopped:)])
    //	[[self delegate] serviceStopped:sender];
}

// Sent to the NSNetService instance's delegate when the instance is being monitored and the instance's TXT record has been updated. The new record is contained in the data parameter.
- (void)netService:(NSNetService *)sender didUpdateTXTRecordData:(NSData *)data
{
	NSLog( @"NSNETSERVICEDELEGATE: didUpdateTXTRecordData" );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////// NET SERVICE BROWSER DELEGATE ///////////

- (void)netServiceBrowserWillSearch:(NSNetServiceBrowser *)aNetServiceBrowser
{
	NSLog( @"NSNETSERVICEBROWSERDELEGATE: netServiceBrowserWillSearch" );
}

// Sent to the NSNetServiceBrowser instance's delegate when the instance's previous running search request has stopped.
- (void)netServiceBrowserDidStopSearch:(NSNetServiceBrowser *)aNetServiceBrowser
{
	NSLog( @"NSNETSERVICEBROWSERDELEGATE: netServiceBrowserDidStopSearch" );
	
	isBrowsingForServices = false;
	isBrowsingForDomains = false;
}

// Sent to the NSNetServiceBrowser instance's delegate when an error in searching for domains or services has occurred. The error dictionary will contain two key/value pairs representing the error domain and code (see the NSNetServicesError enumeration above for error code constants). It is possible for an error to occur after a search has been started successfully.
- (void)netServiceBrowser:(NSNetServiceBrowser *)aNetServiceBrowser didNotSearch:(NSDictionary *)errorDict
{
	NSLog( @"NSNETSERVICEBROWSERDELEGATE: didNotSearch - %@", errorDict );
	
	isBrowsingForServices = false;
	isBrowsingForDomains = false;
	
	// clear all lists
	[self.pendingServicesToAdd removeAllObjects];
	[self.pendingServicesToRemove removeAllObjects];
	[self.availableServices removeAllObjects];
	
	[self.pendingDomainsToAdd removeAllObjects];
	[self.pendingDomainsToRemove removeAllObjects];
	[self.availableDomains removeAllObjects];
}

// Sent to the NSNetServiceBrowser instance's delegate for each domain discovered. If there are more domains, moreComing will be YES. If for some reason handling discovered domains requires significant processing, accumulating domains until moreComing is NO and then doing the processing in bulk fashion may be desirable.
- (void)netServiceBrowser:(NSNetServiceBrowser *)aNetServiceBrowser didFindDomain:(NSString *)domainString moreComing:(BOOL)moreComing
{
	NSLog( @"NSNETSERVICEBROWSERDELEGATE: didFindDomain" );
	
	[self.pendingDomainsToAdd addObject:domainString];
	
	if( !moreComing )
	{
		NSSet* setToAdd;
		
		setToAdd = [self.pendingDomainsToAdd copy];
		[self.pendingDomainsToAdd removeAllObjects];
		
		[self.availableDomains unionSet:setToAdd];
		
		// call CommServiceListener delegate
        //if ([[self delegate] respondsToSelector:@selector(browsingAvailabilityChanges:whichService:)])
        //	[[self delegate] browsingAvailabilityChanges:CommServiceChange_FoundDomain whichService:nil];
	}
}

// Sent to the NSNetServiceBrowser instance's delegate for each service discovered. If there are more services, moreComing will be YES. If for some reason handling discovered services requires significant processing, accumulating services until moreComing is NO and then doing the processing in bulk fashion may be desirable.
- (void)netServiceBrowser:(NSNetServiceBrowser *)aNetServiceBrowser didFindService:(NSNetService *)aNetService moreComing:(BOOL)moreComing
{
	NSLog( @"NSNETSERVICEBROWSERDELEGATE: didFindService" );
	
	[self.pendingServicesToAdd addObject:aNetService];
	
	if( !moreComing )
	{
		NSSet* setToAdd;
		
		setToAdd = [self.pendingServicesToAdd copy];
		[self.pendingServicesToAdd removeAllObjects];
		
		[self.availableServices unionSet:setToAdd];
	
		NSArray* addressesToResolve = [self.availableServices allObjects];
		
		for( NSNetService* service in addressesToResolve )
		{
			// try and resolve the actual IP address
			[service setDelegate:self];
			[service resolveWithTimeout:30];
		}
		
		// call CommServiceListener delegate
		//if ([[self delegate] respondsToSelector:@selector(browsingAvailabilityChanges:whichService:)])
		//	[[self delegate] browsingAvailabilityChanges];
	}
}

// Sent to the NSNetServiceBrowser instance's delegate when a previously discovered domain is no longer available.
- (void)netServiceBrowser:(NSNetServiceBrowser *)aNetServiceBrowser didRemoveDomain:(NSString *)domainString moreComing:(BOOL)moreComing
{
	NSLog( @"NSNETSERVICEBROWSERDELEGATE: didRemoveDomain" );
	
	[self.pendingDomainsToRemove addObject:domainString];
	
	if( !moreComing )
	{
		NSSet* setToRemove;
		
		setToRemove = [self.pendingDomainsToRemove copy];
		[self.pendingDomainsToRemove removeAllObjects];
		
		[self.availableDomains minusSet:setToRemove];
		
		// call CommServiceListener delegate
        //if ([[self delegate] respondsToSelector:@selector(browsingAvailabilityChanges:whichService:)])
        //	[[self delegate] browsingAvailabilityChanges:CommServiceChange_BrowserRemoveDomain whichService:nil];
	}
}

// Sent to the NSNetServiceBrowser instance's delegate when a previously discovered service is no longer published.
- (void)netServiceBrowser:(NSNetServiceBrowser *)aNetServiceBrowser didRemoveService:(NSNetService *)aNetService moreComing:(BOOL)moreComing
{
	NSLog( @"NSNETSERVICEBROWSERDELEGATE: didRemoveService" );
	
	[self.pendingServicesToRemove addObject:aNetService];
	
	if( !moreComing )
	{
		NSSet* setToRemove;
		
		setToRemove = [self.pendingServicesToRemove copy];
		[self.pendingServicesToRemove removeAllObjects];
		
        if( pNetworkService )
        {
            NetworkService::DiscoveryData removeData;

            NSArray *array = [setToRemove allObjects];
            for( NSNetService* obj in array )
            {
                removeData.domain = std::string([obj.domain UTF8String]);
                removeData.type = std::string([obj.type UTF8String]);
                removeData.name = std::string([obj.name UTF8String]);

                pNetworkService->RemoveDiscovery(&removeData);
            }
        }

		[self.availableServices minusSet:setToRemove];
		
		// call CommServiceListener delegate
        //if ([[self delegate] respondsToSelector:@selector(browsingAvailabilityChanges:whichService:)])
        //	[[self delegate] browsingAvailabilityChanges:CommServiceChange_BrowserRemoveService whichService:aNetService];
	}
}

@end

/////////////////////////////

/////////////////////////////////////////////////////
/// Default Constructor
///
///
/////////////////////////////////////////////////////
NetworkService::NetworkService()
{
    // can cast void* to id(name)
    m_NetServiceAdvertise = 0;
    m_NetServiceBrowser = 0;
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
/// Params: None
///
/////////////////////////////////////////////////////
void NetworkService::StartAdvertising( const char* szDomain, const char* szType, const char* szName, int port )
{
    StopAdvertising();

    // advertise
    NSString* domainStr = [[NSString alloc] initWithFormat:@"%s", szDomain];
    NSString* typeStr = [[NSString alloc] initWithFormat:@"%s", szType];
    NSString* nameStr = [[NSString alloc] initWithFormat:@"%s", szName];

    BonjourNetworkService* service = [[BonjourNetworkService alloc] init:this];

    m_NetServiceAdvertise = (__bridge_retained void*)service;
    BonjourNetworkService* objCService = (__bridge BonjourNetworkService*)m_NetServiceAdvertise;

    [objCService publishService:domainStr ofType:typeStr withName:nameStr onPort:port];
}

/////////////////////////////////////////////////////
/// Method: StartBrowsing
/// Params: None
///
/////////////////////////////////////////////////////
void NetworkService::StartBrowsing( const char* szDomain, const char* szType )
{
    StopBrowsing();

    // find
    NSString* domainStr = [[NSString alloc] initWithFormat:@"%s", szDomain];
    NSString* typeStr = [[NSString alloc] initWithFormat:@"%s", szType];

    BonjourNetworkService* service = [[BonjourNetworkService alloc] init:this];

    m_NetServiceBrowser = (__bridge_retained void*)service;
    BonjourNetworkService* objCService = (__bridge BonjourNetworkService*)m_NetServiceBrowser;

    [objCService searchForServices:domainStr ofType:typeStr];
}

/////////////////////////////////////////////////////
/// Method: StopAdvertising
/// Params: None
///
/////////////////////////////////////////////////////
void NetworkService::StopAdvertising()
{
    if( m_NetServiceAdvertise != 0 )
    {
        BonjourNetworkService* objCService = (__bridge BonjourNetworkService*)m_NetServiceAdvertise;

        [objCService stop];
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
    if( m_NetServiceBrowser != 0 )
    {
        BonjourNetworkService* objCService = (__bridge BonjourNetworkService*)m_NetServiceBrowser;

        [objCService stop];
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

#endif // BASE_SUPPORT_BONJOUR

#endif // BASE_PLATFORM_iOS
