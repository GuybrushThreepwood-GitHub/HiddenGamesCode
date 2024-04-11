
/*===================================================================
	File: RaspberryPiMutex.cpp
	Library: CoreLib

	(C)Hidden Games
=====================================================================*/

#ifdef BASE_PLATFORM_RASPBERRYPI

#include "Core/CoreDefines.h"

#include "Debug/Assertion.h"
#include "Debug/DebugLogging.h"

#include "Core/rpi/RaspberryPiMutex.h"

using core::rpi::RaspberryPiMutex;

namespace core
{
	namespace rpi
	{
		/////////////////////////////////////////////////////
		/// Default constructor
		/// 
		///
		/////////////////////////////////////////////////////
		RaspberryPiMutex::RaspberryPiMutex()
		{
			m_Initialised = false;
			m_Locked = false;
		}

		/////////////////////////////////////////////////////
		/// Default destructor
		/// 
		///
		/////////////////////////////////////////////////////
		RaspberryPiMutex::~RaspberryPiMutex()
		{

		}

		/////////////////////////////////////////////////////
		/// Method: Initialise
		/// Params: None
		///
		/////////////////////////////////////////////////////
		void RaspberryPiMutex::Initialise( void )
		{
			// create it
			pthread_mutex_init( &m_hMutex, 0 );

			m_Initialised = true;
			m_Locked = false;
		}

		/////////////////////////////////////////////////////
		/// Method: Release
		/// Params: None
		///
		/////////////////////////////////////////////////////
		void RaspberryPiMutex::Release( void )
		{
			// delete it
			pthread_mutex_destroy( &m_hMutex );

			// reset flags
			m_Initialised = false;
			m_Locked = false;
		}

		/////////////////////////////////////////////////////
		/// Method: Lock
		/// Params: None
		///
		/////////////////////////////////////////////////////
		int RaspberryPiMutex::Lock( void )
		{
			// check the mutex was created if not create one
			if( !m_Initialised )
			{
				Initialise();

				pthread_mutex_lock( &m_hMutex );

				m_Locked = true;
				return(0);
			}
			else
			{
				pthread_mutex_lock( &m_hMutex );

				m_Locked = true;
				return(0);
			}

			return(1);
		}
				
		/////////////////////////////////////////////////////
		/// Method: UnLock
		/// Params: None
		///
		/////////////////////////////////////////////////////	
		int RaspberryPiMutex::UnLock( void )
		{
			// if the mutex was not created and somehow unlock gets called before a lock 
			if( !m_Initialised )
			{
				DBGLOG( "RASPBERRYPIMUTEX: Error calling unlock somehow on an uninitialised pthread\n" );
				return(1);
			}
			else
			{
				pthread_mutex_unlock( &m_hMutex );
				m_Locked = false;
			}

			return(1);
		}
		
	} // namespace rpi
	
} // namespace core

#endif // BASE_PLATFORM_RASPBERRYPI



