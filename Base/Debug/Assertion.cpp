
/*===================================================================
	File: Assertion.cpp
	Library: Debug

	(C)Hidden Games
=====================================================================*/

#include "Core/CoreDefines.h"

#ifdef BASE_PLATFORM_WINDOWS
	#ifndef _INC_WINDOWS	
		#include <windows.h>
	#endif // _INC_WINDOWS
#endif // BASE_PLATFORM_WINDOWS

#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <cassert>
#include <iostream>

#include "Debug/Assertion.h"

void PrintLine( const char* buffer, bool newLine=true );

/////////////////////////////////////////////////////
/// Method: PrintLine
/// Params: [in]buffer, [in]newLine
///
/////////////////////////////////////////////////////
void PrintLine( const char* buffer, bool newLine )
{
#ifdef BASE_PLATFORM_WINDOWS	
	if( buffer )
		OutputDebugStringA( buffer );

	if( newLine )
		OutputDebugStringA( "\n" );
#endif // BASE_PLATFORM_WINDOWS

	if( buffer )
		std::printf( "%s", buffer );

	if( newLine )
		std::printf( "\n" );
}

/////////////////////////////////////////////////////
/// Method: ReportAssert
/// Params: [in]condition, [in]file, [in]line, [in]description
///
/////////////////////////////////////////////////////
int dbg::ReportAssert( const char* condition, const char* file, int line, const char* description, ... )
{
	const int MAX_LOG_LENGTH = 4096;
	char buffer[MAX_LOG_LENGTH];

	PrintLine( 0 );
	PrintLine( "***ASSERTION***" );

	if( condition )
	{
		std::memset( buffer, 0, sizeof(char)*MAX_LOG_LENGTH );
		snprintf( buffer, MAX_LOG_LENGTH, "\tCondition: '%s'", condition );
		PrintLine( buffer );
	}

	if( file )
	{
		std::memset( buffer, 0, sizeof(char)*MAX_LOG_LENGTH );
		snprintf( buffer, MAX_LOG_LENGTH, "\tFile: '%s'", file );
		PrintLine( buffer );
	}

	// line
	std::memset( buffer, 0, sizeof(char)*MAX_LOG_LENGTH );
	snprintf( buffer, MAX_LOG_LENGTH, "\tLine: '%d'", line );
	PrintLine( buffer );

	std::memset( buffer, 0, sizeof(char)*MAX_LOG_LENGTH );

	if( description )
	{
		size_t len = strlen( description );

		if( len > 0 &&
			len <= MAX_LOG_LENGTH-1 )
		{
			va_list ap;
			va_start( ap, description );
				vsnprintf( buffer, MAX_LOG_LENGTH-1, description, ap );
			va_end( ap );

			PrintLine( "\tDesc: ", false );
			PrintLine( buffer );
		}
	}
	PrintLine( "***************" );
	PrintLine( 0 );

	return(1);
}

#ifdef _DEBUG
/*	bool dbg::CustomAssertFunction( bool exp, const char* description, int line, const char* file, bool* ignore )
	{
		if( exp )
		{
			
		}

		return(false);
	}*/
#endif // _DEBUG