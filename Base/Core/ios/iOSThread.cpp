
/*===================================================================
	File: iOSThread.cpp
	Library: CoreLib

	(C)Hidden Games
=====================================================================*/

#ifdef BASE_PLATFORM_iOS

#include "Core/CoreDefines.h"

#include "Debug/Assertion.h"
#include "Debug/DebugLogging.h"

#include "Core/ios/iOSThread.h"

using core::ios::iOSThread;

namespace core
{
	namespace ios
	{
		/////////////////////////////////////////////////////
		/// Default constructor
		/// 
		///
		/////////////////////////////////////////////////////
		iOSThread::iOSThread()
		{
			m_Initialised = false;
		}

		/////////////////////////////////////////////////////
		/// Default destructor
		/// 
		///
		/////////////////////////////////////////////////////
		iOSThread::~iOSThread()
		{

		}

		/////////////////////////////////////////////////////
		/// Method: Initialise
		/// Params: [in]ThreadFunc, [in]pThreadParam
		///
		/////////////////////////////////////////////////////
		void iOSThread::Initialise( void *(*ThreadFunc)( void *pParam ), void *pThreadParam )
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
		void iOSThread::Release( void )
		{
			// delete it
			if( m_Initialised )
			{
				//pthread_kill( m_hThread, 0 );
				//pthread_exit( 0 );
				pthread_cancel(m_hThread);
			}
			
			// reset flags
			m_Initialised = false;
		}

	} // namespace ios

} // namespace core

#endif // BASE_PLATFORM_iOS


