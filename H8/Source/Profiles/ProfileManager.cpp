
/*===================================================================
	File: ProfileManager.cpp
	Game: H8

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include <sstream>
#include <iomanip>
#include <string>

#include "MathBase.h"
#include "RenderBase.h"
#include "SoundBase.h"
#include "InputBase.h"
#include "ScriptBase.h"
#include "CollisionBase.h"
#include "Model/ModelCommon.h"
#include "ModelBase.h"

// app includes
#include "H8Consts.h"

#include "Profiles/ProfileManager.h"

namespace
{
	const int VERSION_ID = 1; // first release Profile_1_0.sav

	const int MAX_STRING_BUFFER = 8192;

	void FormatToString( std::string& addTo, const char *fmt, ... )
	{
		char textString[MAX_STRING_BUFFER];

		// -- empty the buffer properly to ensure no leaks.
		memset( textString, '\0', sizeof(char)*MAX_STRING_BUFFER );

		va_list args;
		va_start ( args, fmt );
		vsnprintf ( textString, MAX_STRING_BUFFER, fmt, args );
		va_end ( args );
		addTo += textString;
	}
}

ProfileManager* ProfileManager::ms_Instance = 0;

/////////////////////////////////////////////////////
/// Static Method: Initialise
/// Params: None
///
/////////////////////////////////////////////////////
void ProfileManager::Initialise( void )
{
	DBG_ASSERT( (ms_Instance == 0) );

	DBG_MEMTRY
		ms_Instance = new ProfileManager;
	DBG_MEMCATCH
}

/////////////////////////////////////////////////////
/// Static Method: Shutdown
/// Params: None
///
/////////////////////////////////////////////////////
void ProfileManager::Shutdown( void )
{
	if( ms_Instance )
	{
		delete ms_Instance;
		ms_Instance = 0;
	}
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
ProfileManager::ProfileManager()
{
	std::memset( m_ProfileName, 0, sizeof(char)*core::MAX_PATH+core::MAX_PATH );

	std::memset( &m_ProfileData, 0, sizeof(Profile) );

	DBGLOG( "Profile struct size = %lu-bytes\n", sizeof(Profile) );
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
ProfileManager::~ProfileManager()
{

}

/////////////////////////////////////////////////////
/// Method: CreateProfiles
/// Params: None
///
/////////////////////////////////////////////////////
void ProfileManager::CreateProfiles()
{
	DBG_ASSERT( m_ProfileName != 0 );

	ClearProfile(true);

	SaveProfileToScript();
}

/////////////////////////////////////////////////////
/// Method: ClearProfile
/// Params: [in]slot
///
/////////////////////////////////////////////////////
void ProfileManager::ClearProfile( bool completeClear )
{
	if( completeClear )
	{
		std::memset( &m_ProfileData, 0, sizeof(Profile) );

		m_ProfileData.sfxState = true;
	}

	// default data
	m_GameData = GetScriptDataHolder()->GetGameData();
}

/////////////////////////////////////////////////////
/// Method: Setup
/// Params: None
///
/////////////////////////////////////////////////////
void ProfileManager::Setup()
{
	// set up the gamedata
	m_GameData = GetScriptDataHolder()->GetGameData();
}

/////////////////////////////////////////////////////
/// Method: LoadProfile
/// Params: [in]profileData
///
/////////////////////////////////////////////////////
void ProfileManager::LoadProfile( const char* profileData )
{
	DBG_ASSERT( profileData != 0 );

	// copy filename
	snprintf( m_ProfileName, core::MAX_PATH+core::MAX_PATH, "%s", profileData );

	// if the file doesn't exist, create it
	if( !file::FileExists( m_ProfileName ) )
		CreateProfiles();

	LoadProfileFromScript( m_ProfileName, 0 );
}

/////////////////////////////////////////////////////
/// Method: LoadProfileFromScript
/// Params: None
///
/////////////////////////////////////////////////////
void ProfileManager::LoadProfileFromScript( const char* profileData, lua_State* pState )
{
	bool canRead = false;
	int tableIndex = 1;

	if( pState == 0 )
	{
		if( core::IsEmptyString( profileData ) )
			return;

		// reading from proper saved file
		pState = script::LuaScripting::GetState();

		if( !script::LoadScript( profileData ) )
		{
			canRead = true;
			lua_getglobal( pState, "profile" );
			tableIndex = -1;
		}
		else
			return;
	}
	else
	{
		// going to set the data manually
		canRead = true;
		tableIndex = 1;
	}

	if( canRead )
	{
		if( lua_istable( pState, tableIndex ) )
		{
			// options
			m_ProfileData.sfxState		= true;//static_cast<bool>( script::LuaGetNumberFromTableItem( "sfxState", 1, 1.0 ) != 0 );

			m_ProfileData.personalBest	= static_cast<unsigned long long int>( script::LuaGetNumberFromTableItem( "personalBest", 1, 0.0 ) );
		}

		if( tableIndex == -1 )
			lua_pop( pState, 1 );
	}
}

/////////////////////////////////////////////////////
/// Method: SaveProfile
/// Params: None
///
/////////////////////////////////////////////////////
void ProfileManager::SaveProfile()
{
	SaveProfileToScript();
}

/////////////////////////////////////////////////////
/// Method: SaveProfileToScript
/// Params: None
///
/////////////////////////////////////////////////////
void ProfileManager::SaveProfileToScript()
{
	if( !file::FileOpen( m_ProfileName, file::FILETYPE_TEXT_WRITE, &m_FileHandle ) )
	{
		return;
		//DBG_ASSERT(0);
	}
	else
	{
		std::string fileToWrite;

		FormatToString( fileToWrite, "profile = {\n" );
			// options
			//FormatToString( fileToWrite, "sfxState = %d;\n", m_ProfileData.sfxState );

			FormatToString( fileToWrite, "personalBest = %llu;\n", m_ProfileData.personalBest );
		FormatToString( fileToWrite, "}\n" );

		// write string to normal file
		fprintf( m_FileHandle.fp, "%s", fileToWrite.c_str() );

		file::FileClose( &m_FileHandle );

		// now call luac
	#ifdef _DEBUG
		if( script::LuaFunctionCheck( "SaveToBinary" ) == 0 )
	#endif // _DEBUG
		{
			int result = 0;

			int errorFuncIndex;
			errorFuncIndex = script::GetErrorFuncIndex();

			// get the function
			lua_getglobal( script::LuaScripting::GetState(), "SaveToBinary" );

			lua_pushstring( script::LuaScripting::GetState(), &m_ProfileName[0] ); // inputFile
			lua_pushstring( script::LuaScripting::GetState(), &m_ProfileName[0] ); // outputFile

			result = lua_pcall( script::LuaScripting::GetState(), 2, 0, errorFuncIndex );

			// LUA_ERRRUN --- a runtime error. 
			// LUA_ERRMEM --- memory allocation error. For such errors, Lua does not call the error handler function. 
			// LUA_ERRERR --- error while running the error handler function. 

			if( result == LUA_ERRRUN || result == LUA_ERRMEM || result == LUA_ERRERR )
			{
				DBGLOG( "PROFILEMANAGER: *ERROR* Calling function '%s' failed\n", "SaveToBinary" );
				DBGLOG( "\tLUA_TRACEBACK: %s\n", lua_tostring( script::LuaScripting::GetState(), -1 ) );

				script::StackDump( script::LuaScripting::GetState() );

				DBG_ASSERT(0);
			}
		}
	}
}

/////////////////////////////////////////////////////
/// Method: GetProfile
/// Params: [in]slot
///
/////////////////////////////////////////////////////
ProfileManager::Profile* ProfileManager::GetProfile()
{
	return( &m_ProfileData );
}
