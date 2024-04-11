

/*===================================================================
	File: WinThread.cpp
	Library: CoreLib

	(C)Hidden Games
=====================================================================*/

#ifdef BASE_PLATFORM_WINDOWS

#include "Core/CoreDefines.h"

#include "Debug/Assertion.h"
#include "Debug/DebugLogging.h"

#include "Core/win/WinThread.h"

using core::win::WinThread;

/////////////////////////////////////////////////////
/// Default constructor
/// 
///
/////////////////////////////////////////////////////
WinThread::WinThread()
{
	m_hThread = 0;
	m_Initialised = false;
}

/////////////////////////////////////////////////////
/// Default destructor
/// 
///
/////////////////////////////////////////////////////
WinThread::~WinThread()
{

}

/////////////////////////////////////////////////////
/// Method: Initialise
/// Params: [in]ThreadFunc, [in]pThreadParam
///
/////////////////////////////////////////////////////
void WinThread::Initialise( void *(*ThreadFunc)( void *pParam ), void *pThreadParam )
{
	// create it
	m_dwThreadID = 0;

	m_hThread = CreateThread( 0, 0, (LPTHREAD_START_ROUTINE)ThreadFunc, pThreadParam, 0, &m_dwThreadID );
	//m_hThread = (HANDLE)_beginthreadex( 0, 0, ThreadFunc, pThreadParam, 0, 0 );

	if( m_hThread == 0 )
	{
		// failed thread creation
		DBGLOG( "WIN32THREAD: *ERROR* Could not create thread\n" );
		m_Initialised = false;
	}
	else
	{
		m_Initialised = true;
	}
}

/////////////////////////////////////////////////////
/// Method: Release
/// Params: None
///
/////////////////////////////////////////////////////
void WinThread::Release( void )
{
	// delete it
	if( m_hThread )
	{
		//TerminateThread( m_hThread, 0 );

		CloseHandle( m_hThread );
		//_endthreadex(0);

		m_hThread = 0;
	}
	// reset flags
	m_Initialised = false;
}

#endif // BASE_PLATFORM_WINDOWS


