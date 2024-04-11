
#ifndef __PURCHASE_H__
#define __PURCHASE_H__

#ifndef __PURCHASECOMMON_H__
	#include "PurchaseCommon.h"
#endif // __PURCHASECOMMON_H__

#ifdef BASE_PLATFORM_WINDOWS
	#ifndef __WINPURCHASE_H__
		#include "win/WinPurchase.h"
	#endif // __WINPURCHASE_H__
#endif // BASE_PLATFORM_WINDOWS

#ifdef BASE_PLATFORM_iOS
	#ifndef __IOSPURCHASE_H__
		#include "ios/iOSPurchase.h"
	#endif // __IOSPURCHASE_H__
#endif // BASE_PLATFORM_iOS

#ifdef BASE_PLATFORM_ANDROID
	#ifndef __ANDROIDPURCHASE_H__
		#include "aos/AndroidPurchase.h"
	#endif // __ANDROIDPURCHASE_H__
#endif // BASE_PLATFORM_ANDROID

#ifdef BASE_PLATFORM_RASPBERRYPI
	#ifndef __RASPBERRYPIPURCHASE_H__
		#include "rpi/RaspberryPiPurchase.h"
	#endif // __RASPBERRYPIPURCHASE_H__
#endif // BASE_PLATFORM_RASPBERRYPI

#ifdef BASE_PLATFORM_MAC
	#ifndef __MACPURCHASE_H__
		#include "mac/MacPurchase.h"
	#endif // __MACPURCHASE_H__
#endif // BASE_PLATFORM_MAC

namespace support
{
    #ifdef BASE_PLATFORM_WINDOWS
		#define PURCHASE_PLATFORM_CLASS win::WinPurchase
    #endif // BASE_PLATFORM_WINDOWS

    #ifdef BASE_PLATFORM_iOS
		#define PURCHASE_PLATFORM_CLASS ios::iOSPurchase
    #endif // BASE_PLATFORM_iOS
		
    #ifdef BASE_PLATFORM_ANDROID
		#define PURCHASE_PLATFORM_CLASS aos::AndroidPurchase
    #endif // BASE_PLATFORM_ANDROID

    #ifdef BASE_PLATFORM_RASPBERRYPI
		#define PURCHASE_PLATFORM_CLASS rpi::RaspberryPiPurchase
    #endif // BASE_PLATFORM_RASPBERRYPI

	#ifdef BASE_PLATFORM_MAC
		#define PURCHASE_PLATFORM_CLASS mac::MacPurchase
	#endif // BASE_PLATFORM_MAC
	
    class Purchase : public PURCHASE_PLATFORM_CLASS
    {
        public:
            static void Create();
            static void Destroy();
			
			static Purchase *GetInstance( void ) 
			{
				DBG_ASSERT( (ms_Instance != 0) );
					
				return( ms_Instance );
			}
			static bool IsInitialised( void ) 
			{
				return( ms_Instance != 0 );
			}	
            
		private:
			static Purchase* ms_Instance; 			
    };

} // namespace support

#endif // __PURCHASE_H__

