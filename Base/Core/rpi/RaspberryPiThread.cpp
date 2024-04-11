
/*===================================================================
	File: RaspberryPiThread.cpp
	Library: CoreLib

	(C)Hidden Games
=====================================================================*/

#ifdef BASE_PLATFORM_RASPBERRYPI

#include "Core/CoreDefines.h"

#include "Debug/Assertion.h"
#include "Debug/DebugLogging.h"

#include "Core/rpi/RaspberryPiThread.h"

using core::rpi::RaspberryPiThread;

namespace core
{
	namespace rpi
	{
		/////////////////////////////////////////////////////
		/// Default constructor
		/// 
		///
		/////////////////////////////////////////////////////
		RaspberryPiThread::RaspberryPiThread()
		{
			m_Initialised = false;
		}

		/////////////////////////////////////////////////////
		/// Default destructor
		/// 
		///
		/////////////////////////////////////////////////////
		RaspberryPiThread::~RaspberryPiThread()
		{

		}

		/////////////////////////////////////////////////////
		/// Method: Initialise
		/// Params: [in]ThreadFunc, [in]pThreadParam
		///
		/////////////////////////////////////////////////////
		void RaspberryPiThread::Initialise( void *(*ThreadFunc)( void *pParam ), void *pThreadParam )
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
		void RaspberryPiThread::Release( void )
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

	} // namespace rpi

} // namespace core

#endif // BASE_PLATFORM_RASPBERRYPI


