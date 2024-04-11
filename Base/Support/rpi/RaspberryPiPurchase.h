
#ifndef __RASPBERRYPIPURCHASE_H__
#define __RASPBERRYPIPURCHASE_H__

#ifdef BASE_PLATFORM_RASPBERRYPI

#include <list>
#include "Support/PurchaseCommon.h"

namespace support
{
    namespace rpi 
    { 
        
        class RaspberryPiPurchase
        {	
            public:
                /// default constructor
                RaspberryPiPurchase();
                /// default destructor
                ~RaspberryPiPurchase();   
			
				void Initialise( std::list<support::TPurchase> itemList );
				void PurchaseItem( const char* purchaseId );
				void RestorePurchases();
				bool IsContentAlreadyPurchased( const char* purchaseId );
			
				void SetCallback( support::PurchaseCallback* pCallback );
			
			private:
				support::PurchaseCallback* m_Callback;		
        };
        
    } // namespace rpi
    
} // namespace support
    
#endif // BASE_PLATFORM_RASPBERRYPI


#endif // __RASPBERRYPIPURCHASE_H__

