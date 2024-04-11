
#ifdef BASE_SUPPORT_BONJOUR

#ifndef __NETWORKSERVICE_H__
#define __NETWORKSERVICE_H__

#if !defined(BASE_PLATFORM_iOS)
    #include "dns_sd.h"
#endif

#include <string>
#include <vector>
#include <map>

class NetworkService
{
    public:
        struct DiscoveryData
        {
            std::string domain;
            std::string type;

            std::string name;
            std::string hostTarget;

            std::string addr;
            std::string addr6;

			int interfaceId;

            int port;
        };

    public:
        NetworkService();
        ~NetworkService();

        void StartAdvertising( const char* szDomain, const char* szType, const char* szName, int port );
        void StartBrowsing( const char* szDomain, const char* szType );

        void StopAdvertising();
        void StopBrowsing();

        void Stop();

        bool DoesDiscoveryExist( const DiscoveryData* data );
        void RemoveDiscovery( const DiscoveryData* data );

		void SetBluetoothSupport( bool state )				{ m_BluetoothSupport = state; }
        bool SupportBluetooth()                             { return m_BluetoothSupport; }
        void SetInterface( uint32_t supportedInterfaces )	{ m_SupportedInterfaces = supportedInterfaces; }

	public:
#if !defined(BASE_PLATFORM_iOS)
		std::map<DNSServiceRef,int> browseFdMap;
		std::map<DNSServiceRef,int> serviceFdMap; 
#endif // BASE_PLATFORM_WINDOWS

		std::vector<DiscoveryData> discoveryList;

    private:
		bool m_BluetoothSupport;
		uint32_t m_SupportedInterfaces;

#if !defined(BASE_PLATFORM_iOS)
        DNSServiceRef m_NetServiceAdvertise;
        DNSServiceRef m_NetServiceBrowser;

        static void DNSSD_API RegisterReply(DNSServiceRef sdRef, const DNSServiceFlags flags, DNSServiceErrorType errorCode,
                                        const char *name, const char *regtype, const char *domain, void *context);

        static void DNSSD_API BrowseReply(DNSServiceRef sdRef, const DNSServiceFlags flags, uint32_t interfaceIndex, DNSServiceErrorType errorCode,
                                        const char *replyName, const char *replyType, const char *replyDomain, void *context);

        static void DNSSD_API ResolveReply(DNSServiceRef sdRef, DNSServiceFlags flags, uint32_t interfaceIndex, DNSServiceErrorType errorCode,
                                                const char *fullname, const char *hosttarget, uint16_t port, uint16_t txtLen, const unsigned char *txtRecord,
                                                void *context );

        static void DNSSD_API GetAddress( DNSServiceRef sdRef, DNSServiceFlags flags, uint32_t interfaceIndex, DNSServiceErrorType errorCode,
                                              const char *hostname, const struct sockaddr *address, uint32_t ttl, void *context );

		core::app::Thread m_NetBrowseThread; 
		core::app::Thread m_NetServiceThread;

		static void* NetBrowseThread(void* threadData);
		static void* NetServiceThread(void* threadData);
		static bool NetBrowseProcessingThread;

		void ServiceEvents(DNSServiceRef sdRef);
		void SafeRemoveService(DNSServiceRef sdRef);
#else
        void* m_NetServiceAdvertise;
        void* m_NetServiceBrowser;
#endif

};

#endif // __NETWORKSERVICE_H__

#endif // BASE_SUPPORT_BONJOUR
