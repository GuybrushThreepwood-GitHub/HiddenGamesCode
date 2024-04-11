
/*===================================================================
	File: AndroidThread.cpp
	Library: CoreLib

	(C)Hidden Games
=====================================================================*/

#ifdef BASE_PLATFORM_ANDROID

#include "Core/CoreDefines.h"

#include "Debug/Assertion.h"
#include "Debug/DebugLogging.h"

#include "Core/aos/AndroidThread.h"

using core::aos::AndroidThread;

namespace core
{
	namespace aos
	{
		/////////////////////////////////////////////////////
		/// Default constructor
		/// 
		///
		/////////////////////////////////////////////////////
		AndroidThread::AndroidThread()
		{
			m_Initialised = false;
		}

		/////////////////////////////////////////////////////
		/// Default destructor
		/// 
		///
		/////////////////////////////////////////////////////
		AndroidThread::~AndroidThread()
		{

		}

		/////////////////////////////////////////////////////
		/// Method: Initialise
		/// Params: [in]ThreadFunc, [in]pThreadParam
		///
		/////////////////////////////////////////////////////
		void AndroidThread::Initialise( void *(*ThreadFunc)( void *pParam ), void *pThreadParam )
		{
			// create it
			pthread_attr_init( &m_hThreadAttribs );

			pthread_create( &m_hThread, 0, ThreadFunc, ( void *)pThreadParam );

			// make sure thread finishes before main thread is closed
			//pthread_join( m_hThread, 0 );

			m_Initialised = true;
		}

		/////////////////////////////////////////////////////
		/// Method: Release
		/// Params: None
		///
		/////////////////////////////////////////////////////
		void AndroidThread::Release( void )
		{
			// delete it
			if( m_Initialised )
			{
				// on android threads should always terminate themselves via signals or conditions
				//pthread_kill( m_hThread, 0 );
				//pthread_exit( 0 );
				//pthread_cancel(m_hThread);
			}
			
			// reset flags
			m_Initialised = false;
		}

	} // namespace aos

} // namespace core

#endif // BASE_PLATFORM_ANDROID


