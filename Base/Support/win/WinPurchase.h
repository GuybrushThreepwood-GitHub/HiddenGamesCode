
#ifndef __WINPURCHASE_H__
#define __WINPURCHASE_H__

#ifdef BASE_PLATFORM_WINDOWS

#include <list>
#include "Support/PurchaseCommon.h"

namespace support
{
    namespace win 
    { 
        class WinPurchase
        {	
            public:
                /// default constructor
                WinPurchase();
                /// default destructor
                ~WinPurchase(); 
			
				void Initialise( std::list<support::TPurchase> itemList );
				void PurchaseItem( const char* purchaseId );
				void RestorePurchases();
				bool IsContentAlreadyPurchased( const char* purchaseId );
			
				void SetCallback( support::PurchaseCallback* pCallback );
			
			private:
				support::PurchaseCallback* m_Callback;				
        };
        
    } // namespace win
    
} // namespace support
    
#endif // BASE_PLATFORM_WINDOWS


#endif // __WINPURCHASE_H__

