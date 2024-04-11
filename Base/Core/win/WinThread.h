
#ifndef __WINTHREAD_H__
#define __WINTHREAD_H__

#ifdef BASE_PLATFORM_WINDOWS

#ifndef __WININCLUDES_H__
	#include "WinIncludes.h"
#endif // __WININCLUDES_H__

namespace core
{
	namespace win
	{
		class WinThread
		{
			public:
				/// default constructor
				WinThread();
				/// default destructor
				~WinThread();

				/// Initialise - initialise member data of a thread
				/// \param ThreadFunc - pointer to user callback function for thread
				/// \param pThreadParam - pointer to user thread params
				void Initialise( void *(*ThreadFunc)( void *pParam ), void *pThreadParam );
				/// Release - cleanup any member data of a thread
				void Release( void );

			private:
				/// thread handle
				HANDLE m_hThread;
				/// thread ID
				DWORD m_dwThreadID;
				/// initialisation flag
				bool m_Initialised;
		};

	} // namespace win

} // namespace core

#endif // BASE_PLATFORM_WINDOWS

#endif // __WINTHREAD_H_


