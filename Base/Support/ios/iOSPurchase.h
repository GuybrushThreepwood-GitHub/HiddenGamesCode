
#ifndef __IOSPURCHASE_H__
#define __IOSPURCHASE_H__

#ifdef BASE_PLATFORM_iOS

#include <list>
#include "AppStorePurchases.h"

namespace support
{
    namespace ios 
    { 
        
        class iOSPurchase
        {	
            public:
                /// default constructor
                iOSPurchase();
                /// default destructor
                ~iOSPurchase();       
			
				void Initialise( std::list<support::TPurchase> itemList );
				void PurchaseItem( const char* purchaseId );
				void RestorePurchases();
				bool IsContentAlreadyPurchased( const char* purchaseId );
			
				void SetCallback( support::PurchaseCallback* pCallback );
			
			private:
				bool m_AppStoreSupport;
				support::PurchaseCallback* m_Callback;
				AppStorePurchases* m_AppPurchases;
        };
        
    } // namespace ios
    
} // namespace support

#endif // BASE_PLATFORM_iOS


#endif // __IOSPURCHASE_H__

