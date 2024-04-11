

/*===================================================================
	File: WinMutex.cpp
	Library: CoreLib

	(C)Hidden Games
=====================================================================*/

#ifdef BASE_PLATFORM_WINDOWS

#include "Core/CoreDefines.h"

#include "Debug/Assertion.h"
#include "Debug/DebugLogging.h"

#include "Core/win/WinMutex.h"

using core::win::WinMutex;

/////////////////////////////////////////////////////
/// Default constructor
/// 
///
/////////////////////////////////////////////////////
WinMutex::WinMutex()
{
	m_Initialised = false;
	m_Locked = false;
}

/////////////////////////////////////////////////////
/// Default destructor
/// 
///
/////////////////////////////////////////////////////
WinMutex::~WinMutex()
{

}

/////////////////////////////////////////////////////
/// Method: Initialise
/// Params: None
///
/////////////////////////////////////////////////////
void WinMutex::Initialise( void )
{
	// create it
	InitializeCriticalSection( &m_CS );

	m_Initialised = true;
	m_Locked = false;
}

/////////////////////////////////////////////////////
/// Method: Release
/// Params: None
///
/////////////////////////////////////////////////////
void WinMutex::Release( void )
{
	// delete it
	DeleteCriticalSection( &m_CS );

	// reset flags
	m_Initialised = false;
	m_Locked = false;
}

/////////////////////////////////////////////////////
/// Method: Lock
/// Params: None
///
/////////////////////////////////////////////////////
int WinMutex::Lock( void )
{
	// check the CS was created if not create on
	if( !m_Initialised )
		Initialise();

	EnterCriticalSection( &m_CS );
	m_Locked = true;
	return(0);
}
		
/////////////////////////////////////////////////////
/// Method: UnLock
/// Params: None
///
/////////////////////////////////////////////////////	
int WinMutex::UnLock( void )
{
	// if the CS was not created and somehow unlock gets called before a lock 
	if( !m_Initialised )
	{
		DBGLOG( "WIN32MUTEX: Error calling unlock somehow on an uninitialised Critical Section Object\n" );
		return(1);
	}
	else
	{
		LeaveCriticalSection( &m_CS );
		m_Locked = false;
		return 0;
	}

	return(1);
}

#endif // BASE_PLATFORM_WINDOWS


