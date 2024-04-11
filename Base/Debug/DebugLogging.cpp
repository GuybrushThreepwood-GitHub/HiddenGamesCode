
/*===================================================================
	File: DebugLogging.cpp
	Library: Debug

	(C)Hidden Games
=====================================================================*/

#include "Core/CoreDefines.h"

#ifdef BASE_PLATFORM_WINDOWS
	#ifndef _INC_WINDOWS	
		#include <windows.h>
	#endif // _INC_WINDOWS
#endif // BASE_PLATFORM_WINDOWS

#ifdef BASE_PLATFORM_ANDROID
	#include <jni.h>
	#include <android/log.h>

	#define  LOG_TAG    "BASE"
	//#define  LOGI(...)  __android_log_print( ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__ )
	//#define  LOGE(...)  __android_log_print( ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__ )
	#define  LOGDEBUG(...)  __android_log_print( ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__ )
#endif // BASE_PLATFORM_ANDROID

#include <cstring>

#include "Core/CoreConsts.h"
#include "Core/CoreFunctions.h"

#include "DebugBase.h"

using dbg::Debug;

Debug* Debug::ms_Instance = 0;

/////////////////////////////////////////////////////
/// Function: LogString
/// Params: None
///
/////////////////////////////////////////////////////
void dbg::LogString( const char* str, ... )
{
	va_list argPtr;
	va_start( argPtr, str );
		if( dbg::Debug::IsInitialised() )
		{
			dbg::Debug::GetInstance()->LogString( str, argPtr );
		}
		else
		{
			std::printf( str, argPtr );

#ifdef BASE_PLATFORM_ANDROID
			char strBuffer[MAX_LOG_LENGTH];
			std::memset( strBuffer, 0, sizeof(char)*MAX_LOG_LENGTH );

			if( core::IsEmptyString( str ) )
				return;

			size_t len = strlen( str );

			if( len >= MAX_LOG_LENGTH-1 )
				return;

			vsnprintf( strBuffer, MAX_LOG_LENGTH-1, str, argPtr );

			__android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, (const char *)strBuffer);
#endif // BASE_PLATFORM_ANDROID

		}
	va_end( argPtr );
}

/////////////////////////////////////////////////////
/// Static Method: Initialise
/// Params: None
///
/////////////////////////////////////////////////////
void Debug::Initialise( void )
{
	DBG_ASSERT_MSG( (ms_Instance == 0), "Debug instance already initialised" );

	DBG_MEMTRY
		ms_Instance = new Debug;
	DBG_MEMCATCH
}

/////////////////////////////////////////////////////
/// Static Method: Shutdown
/// Params: None 
///
/////////////////////////////////////////////////////
void Debug::Shutdown( void )
{
	if( ms_Instance )
	{
		delete ms_Instance;
		ms_Instance = 0;
	}
}

/////////////////////////////////////////////////////
/// Default constructor
/// 
///
/////////////////////////////////////////////////////
Debug::Debug()
{
	std::memset( m_szLogPath, 0, sizeof(char)*core::MAX_PATH+core::MAX_PATH );
	// default log name
	sprintf( m_szLogPath, "DebugLog.txt" );
}

/////////////////////////////////////////////////////
/// Default destructor
/// 
///
/////////////////////////////////////////////////////
Debug::~Debug()
{
	file::FileClose( &m_FileHandle );
}

/////////////////////////////////////////////////////
/// Method: CreateLogFile
/// Params: None
///
/////////////////////////////////////////////////////
int Debug::CreateLogFile( void )
{
#ifdef BASE_PLATFORM_WINDOWS
	const int PATH_SIZE = 256;

	char szPath[PATH_SIZE];
	std::memset( szPath, 0, sizeof(char)*PATH_SIZE );
	GetCurrentDirectoryA( PATH_SIZE, szPath );

	if( !core::IsEmptyString( szPath ) )
		snprintf( m_szLogPath, PATH_SIZE, "%s/DebugLog.txt", szPath );
#endif // BASE_PLATFORM_WINDOWS

	//m_OutStream.open( m_szLogPath, std::ios::out );

	if( file::FileOpen( m_szLogPath, file::FILETYPE_TEXT_WRITE, &m_FileHandle ) )
	{
		dbg::LogString( "Log file started\n\n" );	
		return(0);
	}

	std::printf( "DEBUG: *ERROR* %s log not created\n", m_szLogPath );
	return(1);
}

/////////////////////////////////////////////////////
/// Method: LogString
/// Params: [in]szString, [in]optional
///
/////////////////////////////////////////////////////
void Debug::LogString( const char *szString, va_list argPtr )
{
	std::memset( m_szBuffer, 0, sizeof(char)*MAX_LOG_LENGTH );

	if( core::IsEmptyString( szString ) )
		return;

	size_t len = std::strlen( szString );

	if( len >= MAX_LOG_LENGTH-1 )
		return;

	vsnprintf( m_szBuffer, MAX_LOG_LENGTH-1, szString, argPtr );

	if( file::FileIsOpen( &m_FileHandle ) )
	{
		fputs( m_szBuffer, m_FileHandle.fp );

#ifdef BASE_PLATFORM_WINDOWS		
		OutputDebugStringA( m_szBuffer );
#endif // BASE_PLATFORM_WINDOWS

#ifdef BASE_PLATFORM_ANDROID
		__android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, (const char *)m_szBuffer);
#endif // BASE_PLATFORM_ANDROID

		std::printf( "%s", m_szBuffer );
	}
	else
	{
#ifdef BASE_PLATFORM_ANDROID
		__android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, (const char *)m_szBuffer);
#endif // BASE_PLATFORM_ANDROID

		std::printf( "%s", m_szBuffer );
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
