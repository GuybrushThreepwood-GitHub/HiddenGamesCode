
#ifndef __MACPURCHASE_H__
#define __MACPURCHASE_H__

#ifdef BASE_PLATFORM_MAC

#include <list>
#include "Support/PurchaseCommon.h"

namespace support
{
    namespace mac
    { 
        class MacPurchase
        {	
            public:
                /// default constructor
                MacPurchase();
                /// default destructor
                ~MacPurchase();
			
				void Initialise( std::list<support::TPurchase> itemList );
				void PurchaseItem( const char* purchaseId );
				void RestorePurchases();
				bool IsContentAlreadyPurchased( const char* purchaseId );
			
				void SetCallback( support::PurchaseCallback* pCallback );
			
			private:
				support::PurchaseCallback* m_Callback;				
        };
        
    } // namespace mac
    
} // namespace support
    
#endif // BASE_PLATFORM_MAC


#endif // __MACPURCHASE_H__

