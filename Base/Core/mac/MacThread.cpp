
/*===================================================================
	File: MacThread.cpp
	Library: CoreLib

	(C)Hidden Games
=====================================================================*/

#ifdef BASE_PLATFORM_MAC

#include "Core/CoreDefines.h"

#include "Debug/Assertion.h"
#include "Debug/DebugLogging.h"

#include "Core/mac/MacThread.h"

using core::mac::MacThread;

namespace core
{
	namespace mac
	{
		/////////////////////////////////////////////////////
		/// Default constructor
		/// 
		///
		/////////////////////////////////////////////////////
		MacThread::MacThread()
		{
			m_Initialised = false;
		}

		/////////////////////////////////////////////////////
		/// Default destructor
		/// 
		///
		/////////////////////////////////////////////////////
		MacThread::~MacThread()
		{

		}

		/////////////////////////////////////////////////////
		/// Method: Initialise
		/// Params: [in]ThreadFunc, [in]pThreadParam
		///
		/////////////////////////////////////////////////////
		void MacThread::Initialise( void *(*ThreadFunc)( void *pParam ), void *pThreadParam )
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
		void MacThread::Release( void )
		{
			// delete it
			if( m_Initialised )
			{
				//pthread_kill( m_hThread, 0 );
				//pthread_exit( 0 );
				//pthread_cancel(m_hThread);
			}

			// reset flags
			m_Initialised = false;
		}

	} // namespace mac

} // namespace core

#endif // BASE_PLATFORM_MAC


