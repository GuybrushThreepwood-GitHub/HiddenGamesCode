
#ifndef __WINMUTEX_H__
#define __WINMUTEX_H__

#ifdef BASE_PLATFORM_WINDOWS

#ifndef __WININCLUDES_H__
	#include "WinIncludes.h"
#endif // __WININCLUDES_H__

namespace core
{
	namespace win
	{
		class WinMutex
		{
			public:
				/// default constructor
				WinMutex();
				/// default destructor
				~WinMutex();

				/// Initialise - initialise member data of a mutex
				void Initialise( void );
				/// Release - cleanup any member data of a mutex
				void Release( void );
				/// Lock - lock the mutex for the current thread
				/// \return integer ( SUCCESS: OK or FAIL: FAIL )
				int Lock( void );
				/// UnLock - unlock the mutex from the thread
				/// \return integer ( SUCCESS: OK or FAIL: FAIL )
				int UnLock( void );
				/// IsLocked - Check to see if this mutex currently is locked
				/// \return boolean ( SUCCESS: true or FAIL: false )
				bool IsLocked( void )	{ return m_Locked; }

			private:
				/// initialisation flag
				bool m_Initialised;
				/// locked flag
				bool m_Locked;
				/// critical section structure
				CRITICAL_SECTION m_CS;
		};

	} // namespace win

} // namespace core

#endif // BASE_PLATFORM_WINDOWS

#endif // __WINMUTEX_H__

