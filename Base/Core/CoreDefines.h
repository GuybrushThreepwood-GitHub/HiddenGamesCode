
#ifndef __COREDEFINES_H__
#define __COREDEFINES_H__

#ifdef BASE_PLATFORM_WINDOWS
	#define _CRT_SECURE_NO_DEPRECATE // don't care about secure c functions
	#define _CRT_NONSTDC_NO_DEPRECATE // don't complain about non std c
	#define _CRT_SECURE_NO_WARNINGS // no warnings

#ifdef _DEBUG
	#define _CRTDBG_MAP_ALLOC // debug calls
	//#define _CRTDBG_MAP_ALLOC_NEW
#endif // _DEBUG

//	#define	_CRT_NON_CONFORMING_SWPRINTFS // stops complaint about non conforming swprintf without the size param

	#define WIN32_LEAN_AND_MEAN // Windows specific includes and defines
	#define NOMINMAX // disable macro min/max

	#undef MAX_PATH

	//#define snprintf _snprintf

	#define GLEW_STATIC
	#define GLEW_NO_GLU
	#define CURL_STATICLIB
#endif // BASE_PLATFORM_WINDOWS

#ifdef BASE_PLATFORM_iOS
    #ifndef BASE_DUMMY_APP
        #import <Foundation/Foundation.h>

        #define iOS_VERSION_EQUAL_TO(v)                  ([[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] == NSOrderedSame)
        #define iOS_VERSION_GREATER_THAN(v)              ([[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] == NSOrderedDescending)
        #define iOS_VERSION_GREATER_THAN_OR_EQUAL_TO(v)  ([[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] != NSOrderedAscending)
        #define iOS_VERSION_LESS_THAN(v)                 ([[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] == NSOrderedAscending)
        #define iOS_VERSION_LESS_THAN_OR_EQUAL_TO(v)     ([[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] != NSOrderedDescending)
    #endif // BASE_DUMMY_APP
#endif // BASE_PLATFORM_iOS

//////////
// debug try/catch memory allocations
#ifdef _DEBUG
#define DBG_MEMTRY		try \
						{

#define DBG_MEMCATCH	} \
						catch( std::bad_alloc &ba ) \
						{ \
							DBGLOG( "*ERROR* allocation failed with %s (line:%d)\n", ba.what(), __LINE__ ); \
						} 
	#ifdef BASE_PLATFORM_ANDROID
		#undef DBG_MEMTRY
		#undef DBG_MEMCATCH
		#define DBG_MEMTRY
		#define DBG_MEMCATCH
	#endif // BASE_PLATFORM_ANDROID
#else // not _DEBUG
	#define DBG_MEMTRY
	#define DBG_MEMCATCH
#endif //_DEBUG
/////////////

/////////////
#define CLEAN_DELETE(x)	if( x != 0 ) \
						{ \
							delete x; \
							x = 0; \
						}

#define CLEAN_DELETE_ARRAY(x)	if( x != 0 ) \
								{ \
									delete[] x; \
									x = 0; \
								}
/////////////

#endif // __COREDEFINES_H__

