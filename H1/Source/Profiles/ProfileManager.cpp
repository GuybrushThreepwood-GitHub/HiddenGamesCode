
/*===================================================================
	File: ProfileManager.cpp
	Game: AirCadets

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
#include "H1Consts.h"

#include "Profiles/ProfileManager.h"

namespace
{
	const int VERSION_ID = 2; // first release Profile_1_0.sav

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

	std::memset( &m_ActiveLevelData, 0, sizeof(ActiveLevelData) );

	m_ActiveProfile = -1;

	m_ModeVehicle = 0;
	m_ModePackId = 0;
	m_ModeLevelId = 0;

	m_BestData.clear();

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
		m_ProfileData.adsRemoved = true;
		m_ProfileData.flightTime = 0.0f;
	}

	// default data
	m_GameData = GetScriptDataHolder()->GetGameData();

	m_BestData.clear();
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
	int i=0;

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
			m_ProfileData.sfxState		= static_cast<bool>( script::LuaGetNumberFromTableItem( "sfxState", 1, 1.0 ) != 0 );
			m_ProfileData.adsRemoved	= static_cast<bool>( script::LuaGetNumberFromTableItem( "adsRemoved", 1, 0.0 ) != 0 );

			m_ProfileData.flightTime	= static_cast<float>( script::LuaGetNumberFromTableItem( "flightTime", 1, 0.0 ) );
			
			// grab what levels were generated for this career
			lua_pushstring( script::LuaScripting::GetState(), "scores" );
			lua_gettable( script::LuaScripting::GetState(), -2 );

			if( lua_istable( script::LuaScripting::GetState(), -1 ) )
			{
				int n = luaL_len( script::LuaScripting::GetState(), -1 );
				
				// go through all the tables in this table
				for( i = 1; i <= n; ++i )
				{
					lua_rawgeti( script::LuaScripting::GetState(), -1, i );
					if( lua_istable( script::LuaScripting::GetState(), -1 ) )
					{
						int paramIndex = 1;
						
						LevelBest newData;

						lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
						newData.packId = static_cast<int>( lua_tonumber( script::LuaScripting::GetState(), -1 ) != 0 ) ;
						lua_pop( script::LuaScripting::GetState(), 1 );

						lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
						newData.levelId = static_cast<int>( lua_tonumber( script::LuaScripting::GetState(), -1 ) );
						lua_pop( script::LuaScripting::GetState(), 1 );

						lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
						newData.mode = static_cast<int>( lua_tonumber( script::LuaScripting::GetState(), -1 ) );
						lua_pop( script::LuaScripting::GetState(), 1 );

						lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
						newData.layout = static_cast<int>( lua_tonumber( script::LuaScripting::GetState(), -1 ) );
						lua_pop( script::LuaScripting::GetState(), 1 );

						lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
						newData.bestTime = static_cast<float>( lua_tonumber( script::LuaScripting::GetState(), -1 ) );
						lua_pop( script::LuaScripting::GetState(), 1 );

						lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
						newData.targetsDestroyed = static_cast<int>( lua_tonumber( script::LuaScripting::GetState(), -1 ) );
						lua_pop( script::LuaScripting::GetState(), 1 );

						lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
						newData.targetsTotal = static_cast<int>( lua_tonumber( script::LuaScripting::GetState(), -1 ) );
						lua_pop( script::LuaScripting::GetState(), 1 );

						m_BestData.push_back( newData );
					}
					lua_pop( script::LuaScripting::GetState(), 1 );
				}
			}
			lua_pop( script::LuaScripting::GetState(), 1 ); // remove score
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
	std::size_t i=0;
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
			FormatToString( fileToWrite, "sfxState = %d;\n", m_ProfileData.sfxState );
			FormatToString( fileToWrite, "adsRemoved = %d;\n", m_ProfileData.adsRemoved );
			FormatToString( fileToWrite, "flightTime = %f;\n", m_ProfileData.flightTime );
			
			FormatToString( fileToWrite, "scores = {\n" );
			for( i=0; i < m_BestData.size(); i++ )
			{
				if(i==0)
					FormatToString( fileToWrite, "{ %d, %d, %d, %d, %f, %d, %d }\n", m_BestData[i].packId, m_BestData[i].levelId, m_BestData[i].mode, m_BestData[i].layout, m_BestData[i].bestTime, m_BestData[i].targetsDestroyed, m_BestData[i].targetsTotal );
				else
					FormatToString( fileToWrite, ",{ %d, %d, %d, %d, %f, %d, %d }\n", m_BestData[i].packId, m_BestData[i].levelId, m_BestData[i].mode, m_BestData[i].layout, m_BestData[i].bestTime, m_BestData[i].targetsDestroyed, m_BestData[i].targetsTotal );
			}
			FormatToString( fileToWrite, "};\n" );

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

/////////////////////////////////////////////////////
/// Method: ClearActiveLevelData
/// Params: None
///
/////////////////////////////////////////////////////
void ProfileManager::ClearActiveLevelData()
{
	std::memset( &m_ActiveLevelData, 0, sizeof(ActiveLevelData) );
}

/////////////////////////////////////////////////////
/// Method: AddScore
/// Params: None
///
/////////////////////////////////////////////////////
void ProfileManager::AddScore( bool update, int packId, int levelId, int mode, int layout, int targetsDestroyed, int targetsTotal, float time )
{
	std::size_t i=0;

	bool exists = false;

	for( i=0; i < m_BestData.size(); i++ )
	{
		if( m_BestData[i].packId == packId &&
			m_BestData[i].levelId == levelId &&
			m_BestData[i].mode == mode &&
			m_BestData[i].layout == layout )
		{
			if( update )
			{
				m_BestData[i].bestTime = time;
				m_BestData[i].targetsDestroyed = targetsDestroyed;
				m_BestData[i].targetsTotal = targetsTotal;
			}

			exists = true;
		}
	}

	if( !exists )
	{
		LevelBest newData;

		newData.packId = packId;
		newData.levelId = levelId;
		newData.mode = mode;
		newData.layout = layout;

		newData.bestTime = time;
		newData.targetsDestroyed = targetsDestroyed;
		newData.targetsTotal = targetsTotal;

		m_BestData.push_back( newData );
	}
}

/////////////////////////////////////////////////////
/// Method: GetScore
/// Params: None
///
/////////////////////////////////////////////////////
bool ProfileManager::GetScore( int packId, int levelId, int mode, int layout, LevelBest* pScore )
{
	DBG_ASSERT( pScore != 0 );
	
	std::size_t i=0;

	for( i=0; i < m_BestData.size(); i++ )
	{
		if( m_BestData[i].packId == packId &&
			m_BestData[i].levelId == levelId &&
			m_BestData[i].mode == mode &&
			m_BestData[i].layout == layout )
		{
			*pScore = m_BestData[i];
			return true;
		}
	}

	return false;
}
