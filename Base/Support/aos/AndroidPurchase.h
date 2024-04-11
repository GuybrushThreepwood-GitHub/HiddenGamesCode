
#ifndef __ANDROIDPURCHASE_H__
#define __ANDROIDPURCHASE_H__

#ifdef BASE_PLATFORM_ANDROID

#include <list>
#include "Support/PurchaseCommon.h"

namespace support
{
    namespace aos 
    { 
        
        class AndroidPurchase
        {	
            public:
                /// default constructor
                AndroidPurchase();
                /// default destructor
                ~AndroidPurchase();   
			
				void Initialise( std::list<support::TPurchase> itemList );
				void PurchaseItem( const char* purchaseId );
				void RestorePurchases();
				bool IsContentAlreadyPurchased( const char* purchaseId );
			
				void SetCallback( support::PurchaseCallback* pCallback );
			
			private:
				support::PurchaseCallback* m_Callback;		
        };
        
    } // namespace aos
    
} // namespace support
    
#endif // BASE_PLATFORM_ANDROID


#endif // __ANDROIDPURCHASE_H__

