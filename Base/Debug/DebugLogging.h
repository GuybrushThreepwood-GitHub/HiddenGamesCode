
#ifndef __DEBUGLOGGING_H__
#define __DEBUGLOGGING_H__

#ifndef __COREDEFINES_H__
	#include "Core/CoreDefines.h"
#endif // __COREDEFINES_H__

#ifndef __CORECONSTS_H__
	#include "Core/CoreConsts.h"
#endif // __CORECONSTS_H__

#ifndef _CSTDARG_
	#include <cstdarg>
#endif // _CSTDARG_

#ifndef _IOSTREAM_
	#include <iostream>
#endif // _IOSTREAM_

#ifndef _FSTREAM_
	#include <fstream>
#endif // _FSTREAM_

#ifndef __FILEIO__
	#include "Core/FileIO.h"
#endif // __FILEIO__

#if defined(_DEBUG) || defined(BASE_USE_DEBUG_FONT)
	#if defined(BASE_PLATFORM_WINDOWS) || defined(BASE_PLATFORM_ANDROID)
		#define DBGLOG dbg::LogString
	#else 
		#define DBGLOG std::printf
	#endif // BASE_PLATFORM_WINDOWS || BASE_PLATFORM_ANDROID

	#define DBGPRINT dbg::DebugPrint
#else
	#define DBGLOG
	#define DBGPRINT
#endif // defined(_DEBUG) || defined(BASE_USE_DEBUG_FONT)

namespace dbg
{
	const int MAX_LOG_LENGTH = 8192;

	enum EDebugMessageType
	{
		/// message is normal informative output
		DEBUG_NORMAL=0,
		/// message is a warning
		DEBUG_WARNING,
		/// message is an error
		DEBUG_ERROR,
		
	};

	void LogString( const char* str, ... );

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	class Debug
	{
		public:
			/// default constructor
			Debug();
			/// default destructor
			~Debug();

			/// CreateLogFile - Creates the log file
			/// \return integer - (SUCCESS:OK or FAIL:FAIL)
			int CreateLogFile( void );
			/// LogString - Logs a debug string
			/// \param szString - string to output
			/// \param ... - optional variable arguments within string
			void LogString( const char *szString, va_list argPtr );

			static void Initialise( void );
			static void Shutdown( void );

			static Debug *GetInstance( void ) 
			{
				DBG_ASSERT_MSG( (ms_Instance != 0), "Debug instance has not been initialised" );

				return( ms_Instance );
			}
			static bool IsInitialised( void ) 
			{
				return( ms_Instance != 0 );
			}

		private:
			static Debug* ms_Instance;

			/// log file, file pointer
			//std::ofstream m_OutStream;
			file::TFileHandle m_FileHandle;

			/// log buffer
			char m_szBuffer[MAX_LOG_LENGTH];
			/// logfile stored path
			char m_szLogPath[core::MAX_PATH+core::MAX_PATH];
	};

} // namespace dbg

#endif // __DEBUGLOGGING_H__

