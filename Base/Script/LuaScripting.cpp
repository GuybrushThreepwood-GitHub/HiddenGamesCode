
/*===================================================================
	File: LuaScripting.cpp
	Library: ScriptLib

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include <cerrno>
#include <fcntl.h>

#include "ScriptBase.h"

#ifdef BASE_SUPPORT_SCRIPTING

using script::LuaScripting;

LuaScripting* LuaScripting::ms_Instance = 0;
lua_State* LuaScripting::ms_LUAState = 0;

const int MAX_BUFFER_SIZE = 1024*500; // 500k

namespace
{
	char maxFileBuffer[MAX_BUFFER_SIZE];
	//static char szConvertedFilename[core::MAX_PATH+core::MAX_PATH];
}

/////////////////////////////////////////////////////
/// Function: l_baseLuaAlloc
/// Params:
///
/////////////////////////////////////////////////////
static void *l_baseLuaAlloc (void *ud, void *ptr, size_t osize, size_t nsize) 
{
	(void)ud;
	(void)osize;
	if (nsize == 0) 
	{
		free(ptr);
		return 0;
	}
	else
	{
		if( nsize > 50000 )
			DBG_ASSERT(0);

		return realloc(ptr, nsize);
	}

	return 0;
}

/////////////////////////////////////////////////////
/// Method: Initialise
/// Params: None
///
/////////////////////////////////////////////////////
void LuaScripting::Initialise( void )
{
	DBG_ASSERT( (ms_Instance == 0) );
	DBG_ASSERT( (ms_LUAState == 0) );

	DBG_MEMTRY
		ms_Instance = new LuaScripting;
	DBG_MEMCATCH

	DBG_MEMTRY
		ms_LUAState = luaL_newstate();//lua_newstate( l_baseLuaAlloc, 0 );
	DBG_MEMCATCH

	ms_Instance->Init();

	// add default functionality
	ms_Instance->RegisterFunction( "LoadScript", ScriptLoadScript );
	ms_Instance->RegisterFunction( "RandomNumber", ScriptRandomNumber );
}

/////////////////////////////////////////////////////
/// Method: Shutdown
/// Params: None
///
/////////////////////////////////////////////////////
void LuaScripting::Shutdown( void )
{
	if( ms_LUAState )
	{
		lua_close( ms_LUAState );
		ms_LUAState = 0;
	}

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
LuaScripting::LuaScripting()
{

}

/////////////////////////////////////////////////////
/// Default destructor
/// 
///
/////////////////////////////////////////////////////
LuaScripting::~LuaScripting()
{

}

/////////////////////////////////////////////////////
/// Method: Init
/// Params: [in]nInitFlags
///
/////////////////////////////////////////////////////
void LuaScripting::Init()
{

#ifdef _DEBUG
	CHECK_LUASTATE()
#endif // _DEBUG

	lua_gc( LuaScripting::GetState(), LUA_GCSTOP, 0 );
	lua_gc( LuaScripting::GetState(), LUA_GCRESTART, 0 );

	luaL_openlibs( LuaScripting::GetState() );

//Function        | Name
//----------------+-----------------
//luaopen_base    | ""
//luaopen_table   | LUA_TABLIBNAME
//luaopen_io      | LUA_IOLIBNAME
//luaopen_os      | LUA_OSLIBNAME
//luaopen_string  | LUA_STRLIBNAME
//luaopen_math    | LUA_MATHLIBNAME
//luaopen_debug   | LUA_DBLIBNAME
//luaopen_package | LUA_LOADLIBNAME

//	lua_pushcfunction(LuaScripting::GetState(), luaopen_io);
//	lua_pushliteral(LuaScripting::GetState(), LUA_IOLIBNAME);
//	lua_call(LuaScripting::GetState(), 1, 0);

//	lua_pushcfunction(LuaScripting::GetState(), luaopen_string);
//	lua_pushliteral(LuaScripting::GetState(), LUA_STRLIBNAME);
//	lua_call(LuaScripting::GetState(), 1, 0);


}

/////////////////////////////////////////////////////
/// Method: RegisterFunction
/// Params: [in]szFuncName, [in]func
///
/////////////////////////////////////////////////////
int LuaScripting::RegisterFunction( const char *szFuncName, lua_CFunction func )
{

#ifdef _DEBUG
	CHECK_LUASTATE()
#endif // _DEBUG

	lua_register( LuaScripting::GetState(), szFuncName, func);

	return(0);
}

/////////////////////////////////////////////////////
/// Method: LoadScript
/// Params: [in]szFilename
///
/////////////////////////////////////////////////////
int LuaScripting::LoadScript( const char *szFilename )
{
	int err = 0;

#ifdef _DEBUG
	CHECK_LUASTATE()
#endif // _DEBUG

	int errorFuncIndex;
	errorFuncIndex = script::GetErrorFuncIndex();

	if( core::app::GetLoadFilesFromZip() )
	{
		int o_flags = O_RDONLY;	// rb
		int o_modes = 0664;

		ZZIP_FILE* file = zzip_fopen (szFilename, "rb");
		if( file == 0 )
		{
			char newFileName[core::MAX_PATH+core::MAX_PATH];
			snprintf( newFileName, core::MAX_PATH+core::MAX_PATH, "%s/%s", core::app::GetRootZipFile(), szFilename );
			
			file = zzip_open_shared_io( 0, newFileName, o_flags, o_modes, file::GetZipExtensions(), 0 );//zzip_fopen (newFileName, "rt");
			if( file == 0 )
			{
				DBG_ASSERT_MSG( 0, "LUASCRIPTING: *ERROR* could not load script '%s'", szFilename );
				return( 1 );	
			}				
		}
		
		zzip_seek( file, 0, SEEK_END );
		zzip_off_t fileSize = zzip_tell( file );
		zzip_rewind( file );

		zzip_fread( maxFileBuffer, fileSize, sizeof(char), file );

		zzip_fclose( file );

		err = luaL_loadbuffer( LuaScripting::GetState(), maxFileBuffer, fileSize, szFilename );
	}
	else
	{
		// load the script file
		err = luaL_loadfile( LuaScripting::GetState(), szFilename );
	}

	// 0 --- no errors; 
	// LUA_ERRSYNTAX --- syntax error during pre-compilation. 
	// LUA_ERRMEM --- memory allocation error. 
	// LUA_ERRFILE --- cannot open/read the file

	if( err == LUA_ERRSYNTAX || err == LUA_ERRMEM || err == LUA_ERRFILE )
	{
		DBGLOG( "LUASCRIPTING: *ERROR* could not load script, luaL_loadfile failed\n" );
		DBGLOG( "\tLUA_TRACEBACK: %s\n", lua_tostring( LuaScripting::GetState(), -1 ) );

		script::StackDump(LuaScripting::GetState());

		DBG_ASSERT_MSG( 0, "LUASCRIPTING: *ERROR* could not load script '%s'", szFilename );
		return( 1 );
	}

	err = lua_pcall( LuaScripting::GetState(), 0, 0, errorFuncIndex );
		
	// LUA_ERRRUN --- a runtime error. 
	// LUA_ERRMEM --- memory allocation error. For such errors, Lua does not call the error handler function. 
	// LUA_ERRERR --- error while running the error handler function. 

	if( err == LUA_ERRRUN || err == LUA_ERRMEM || err == LUA_ERRERR )
	{
		DBGLOG( "LUASCRIPTING: *ERROR* Calling script '%s' failed\n", szFilename );
		DBGLOG( "\tLUA_TRACEBACK: %s\n", lua_tostring( LuaScripting::GetState(), -1 ) );

		script::StackDump(LuaScripting::GetState());

		DBG_ASSERT_MSG( 0, "LUASCRIPTING: *ERROR* Calling script '%s' failed", szFilename );
		return( 1 );
	}

	//DBGLOG( "LUASCRIPTING: Loaded Script %s\n", szFilename );

	return(0);
}

/////////////////////////////////////////////////////
/// Method: LoadScriptFromString
/// Params: [in]szString
///
/////////////////////////////////////////////////////
int LuaScripting::LoadScriptFromString( const char *szString )
{
	int err = 0;

#ifdef _DEBUG
	CHECK_LUASTATE()
#endif // _DEBUG

	int errorFuncIndex;
	errorFuncIndex = script::GetErrorFuncIndex();

	// load the script string
	err = luaL_loadstring( LuaScripting::GetState(), szString );

	// 0 --- no errors; 
	// LUA_ERRSYNTAX --- syntax error during pre-compilation. 
	// LUA_ERRMEM --- memory allocation error. 
	// LUA_ERRFILE --- cannot open/read the file

	if( err == LUA_ERRSYNTAX || err == LUA_ERRMEM || err == LUA_ERRFILE )
	{
		DBGLOG( "LUASCRIPTING: *ERROR* could not load script, luaL_dostring failed\n" );
		DBGLOG( "\tLUA_TRACEBACK: %s\n", lua_tostring( LuaScripting::GetState(), -1 ) );

		script::StackDump(LuaScripting::GetState());

		DBG_ASSERT_MSG( 0, "LUASCRIPTING: *ERROR* could not load script '%s'", szString );

		return( 1 );
	}

	err = lua_pcall( LuaScripting::GetState(), 0, 0, errorFuncIndex );

	// LUA_ERRRUN --- a runtime error. 
	// LUA_ERRMEM --- memory allocation error. For such errors, Lua does not call the error handler function. 
	// LUA_ERRERR --- error while running the error handler function. 

	if( err == LUA_ERRRUN || err == LUA_ERRMEM || err == LUA_ERRERR )
	{
		DBGLOG( "LUASCRIPTING: *ERROR* Calling script failed\n" );
		DBGLOG( "\tLUA_TRACEBACK: %s\n", lua_tostring( LuaScripting::GetState(), -1 ) );

		script::StackDump(LuaScripting::GetState());

		DBG_ASSERT_MSG( 0, "LUASCRIPTING: *ERROR* Calling script failed '%s'", szString );

		return( 1 );
	}

	//DBGLOG( "LUASCRIPTING: Loaded String Script\n" );

	return(0);
}

/////////////////////////////////////////////////////
/// Method: SaveScript
/// Params: [in]szFilename, [in]pScript
///
/////////////////////////////////////////////////////
int LuaScripting::SaveScript( const char *szFilename, script::TScriptHolder *pScript )
{
	if( pScript )
	{
		int i = 0;
		FILE *fp = 0;

		if( core::IsEmptyString( szFilename ) )
			return(1);

		fp = fopen( szFilename, "wt" );

		if( fp )
		{
			if( pScript->szScriptString )
			{
				for( i = 0; i < (int)pScript->nScriptLength; i++ )
				{
					fprintf( fp, "%c", pScript->szScriptString[i] );
				}
				fclose( fp );
				return(0);
			}
			else
			{
				fclose( fp );
				return(1);
			}
		}
	}

	return(1);
}

/////////////////////////////////////////////////////
/// Method: SaveScriptFromString
/// Params: [in]szFilename, [in]szString
///
/////////////////////////////////////////////////////
int LuaScripting::SaveScriptFromString( const char *szFilename, const char *szString )
{
	if( !szFilename || !szString )
		return(1);

	FILE *fp = 0;

	if( core::IsEmptyString( szFilename ) )
		return(1);

	fp = fopen( szFilename, "wt" );

	if( fp )
	{
		fprintf( fp, "%s", szString );

		fclose( fp );
		return(0);
	}

	return(1);
}

/////////////////////////////////////////////////////
/// Function: GetErrorFuncIndex
/// Params: None
///
/////////////////////////////////////////////////////
int script::GetErrorFuncIndex()
{
	int errorFuncIndex = 0;

	//lua_getglobal(script::LuaScripting::GetState(), "debug");			// push debug-table on the stack
	//lua_getfield(script::LuaScripting::GetState(), -1, "traceback");	// push debug.traceback

	lua_getglobal( script::LuaScripting::GetState(), "_TRACEBACK" );
	errorFuncIndex = lua_gettop( script::LuaScripting::GetState() );

	lua_pop( script::LuaScripting::GetState(), 1 );
	//lua_pop( script::LuaScripting::GetState(), 2 );

	return errorFuncIndex;
}

/////////////////////////////////////////////////////
/// Function: LoadScript
/// Params: [in]szFilename
///
/////////////////////////////////////////////////////
int script::LoadScript( const char *szFilename )
{
	int err = 0;

#ifdef _DEBUG
	CHECK_LUASTATE()
#endif // _DEBUG

	int errorFuncIndex;
	errorFuncIndex = script::GetErrorFuncIndex();

	if( core::app::GetLoadFilesFromZip() )
	{
		int o_flags = O_RDONLY;	// rb
		int o_modes = 0664;

		ZZIP_FILE* file = zzip_fopen (szFilename, "rb");
		if( file == 0 )
		{
			char newFileName[core::MAX_PATH+core::MAX_PATH];
			snprintf( newFileName, core::MAX_PATH+core::MAX_PATH, "%s/%s", core::app::GetRootZipFile(), szFilename );
			
			file = zzip_open_shared_io( 0, newFileName, o_flags, o_modes, file::GetZipExtensions(), 0 );//zzip_fopen (newFileName, "rt");
			if( file == 0 )
			{
				DBG_ASSERT_MSG( 0, "LUASCRIPTING: *ERROR* could not load script '%s'", szFilename );
				return( 1 );	
			}
		}
		
		zzip_seek( file, 0, SEEK_END );
		zzip_off_t fileSize = zzip_tell( file );
		zzip_rewind( file );

		zzip_fread( maxFileBuffer, fileSize, sizeof(char), file );

		zzip_fclose( file );

		err = luaL_loadbuffer( LuaScripting::GetState(), maxFileBuffer, fileSize, szFilename );
	}
	else
	{
		// load the script file
		err = luaL_loadfile( LuaScripting::GetState(), szFilename );
	}

	// 0 --- no errors; 
	// LUA_ERRSYNTAX --- syntax error during pre-compilation. 
	// LUA_ERRMEM --- memory allocation error. 
	// LUA_ERRFILE --- cannot open/read the file

	if( err == LUA_ERRSYNTAX || err == LUA_ERRMEM || err == LUA_ERRFILE )
	{
		DBGLOG( "LUASCRIPTING: *ERROR* could not load script, luaL_loadfile failed\n" );
		DBGLOG( "\tLUA_TRACEBACK: %s\n", lua_tostring( LuaScripting::GetState(), -1 ) );

		script::StackDump(LuaScripting::GetState());

		DBG_ASSERT_MSG( 0, "LUASCRIPTING: *ERROR* could not load script '%s'", szFilename );

		return( 1 );
	}
 
	err = lua_pcall( LuaScripting::GetState(), 0, 0, errorFuncIndex );
		
	// LUA_ERRRUN --- a runtime error. 
	// LUA_ERRMEM --- memory allocation error. For such errors, Lua does not call the error handler function. 
	// LUA_ERRERR --- error while running the error handler function. 

	if( err == LUA_ERRRUN || err == LUA_ERRMEM || err == LUA_ERRERR )
	{
		DBGLOG( "LUASCRIPTING: *ERROR* Calling script '%s' failed\n", szFilename );
		DBGLOG( "\tLUA_TRACEBACK: %s\n", lua_tostring( LuaScripting::GetState(), -1 ) );

		script::StackDump(LuaScripting::GetState());

		DBG_ASSERT_MSG( 0, "LUASCRIPTING: *ERROR* Calling script '%s'", szFilename );

		return( 1 );
	}

	//DBGLOG( "LUASCRIPTING: Loaded Script %s\n", szFilename );
	return(0);
}

/////////////////////////////////////////////////////
/// Function: LuaFunctionCheck
/// Params: [in]szFunctionName
///
/////////////////////////////////////////////////////
int script::LuaFunctionCheck( const char *szFunctionName )
{
	int result = 0;

	lua_getglobal( LuaScripting::GetState(), szFunctionName );

	// Returns 1 if the value at the given acceptable index is a function (either C or Lua), and 0 otherwise. 
	result = lua_isfunction( LuaScripting::GetState(), -1 );

	lua_pop( LuaScripting::GetState(), 1 );

	if( result )
		return( 0 );

	DBGLOG( "LUASCRIPTING: Function '%s' does not exist\n", szFunctionName );
	return( 1 );
}

/////////////////////////////////////////////////////
/// Function: LuaCallFunction
/// Params: [in]szFunctionName, [in]nParams, [in]nResults
///
/////////////////////////////////////////////////////
void script::LuaCallFunction( const char *szFunctionName, int nParams, int nResults )
{
//#ifdef _DEBUG
	if( LuaFunctionCheck( szFunctionName ) == 0 )
//#endif // _DEBUG
	{
		int result = 0;

		int errorFuncIndex;
		errorFuncIndex = script::GetErrorFuncIndex();

		lua_getglobal( LuaScripting::GetState(), szFunctionName );
		// lua_call( gLUAState, nParams, nResults );
		result = lua_pcall( LuaScripting::GetState(), nParams, nResults, errorFuncIndex );

		// LUA_ERRRUN --- a runtime error. 
		// LUA_ERRMEM --- memory allocation error. For such errors, Lua does not call the error handler function. 
		// LUA_ERRERR --- error while running the error handler function. 

		if( result == LUA_ERRRUN || result == LUA_ERRMEM || result == LUA_ERRERR )
		{
			DBGLOG( "LUASCRIPTING: *ERROR* Calling function '%s' failed\n", szFunctionName );
			DBGLOG( "\tLUA_TRACEBACK: %s\n", lua_tostring( LuaScripting::GetState(), -1 ) );
			DBG_ASSERT_MSG( 0, "LUASCRIPTING: *ERROR* Calling function '%s' failed", szFunctionName );

			return;
		}
	}
}

/////////////////////////////////////////////////////
/// Function: LuaAddGlobal
/// Params: [in]szGlobalName, [in]pValue, [in]nLuaType
///
/////////////////////////////////////////////////////
void script::LuaAddGlobal( const char *szGlobalName, void *pValue, int nLuaType )
{
	switch( nLuaType )
	{
		case LUA_TNIL:
		{
			lua_pushnil( LuaScripting::GetState() );
			lua_setglobal( LuaScripting::GetState(), szGlobalName );
		}break;
		case LUA_TBOOLEAN:
		{
			lua_pushboolean( LuaScripting::GetState(), *reinterpret_cast<int *>(pValue) );
			lua_setglobal( LuaScripting::GetState(), szGlobalName );
		}break;
		case LUA_TLIGHTUSERDATA:
		{
			lua_pushlightuserdata( LuaScripting::GetState(), pValue );
			lua_setglobal( LuaScripting::GetState(), szGlobalName );
		}break;
		case LUA_TNUMBER:
		{
			lua_pushnumber( LuaScripting::GetState(), *reinterpret_cast<double *>(pValue) );
			lua_setglobal( LuaScripting::GetState(), szGlobalName );
		}break;
		case LUA_TSTRING:
		{
			lua_pushstring( LuaScripting::GetState(), reinterpret_cast<char *>(pValue) );
			lua_setglobal( LuaScripting::GetState(), szGlobalName );
		}break;
		case LUA_TTABLE:
		{

		}break;
		case LUA_TFUNCTION:
		{

		}break;
		case LUA_TUSERDATA:
		{

		}break;
		case LUA_TTHREAD:
		{

		}break;
	}
}

/////////////////////////////////////////////////////
/// Function: DoesTableItemExist
/// Params: [in]szTableName, [in]szItemName, [in]nLuaType
///
/////////////////////////////////////////////////////
bool script::DoesTableItemExist( const char *szTableName, const char *szItemName, int nLuaType )
{

	if( core::IsEmptyString( szTableName ) || core::IsEmptyString( szItemName ) )
		return( 0 );

	//lua_pushstring( LuaScripting::GetState(), szTableName );
	//lua_gettable( LuaScripting::GetState(), LUA_GLOBALSINDEX );
	lua_getglobal( script::LuaScripting::GetState(), szTableName );

	if( core::IsEmptyString( szItemName ) )
		return(false);

	if( lua_istable( LuaScripting::GetState(), -1 ) )
	{
		// grab number
		lua_pushstring( LuaScripting::GetState(), szItemName );
		lua_gettable( LuaScripting::GetState(), -2 ); // key

		bool exists = false;
		switch( nLuaType )
		{
			case LUA_TNIL:
			{
				if( lua_isnil( LuaScripting::GetState(), -1 ) )
					exists = true;
			}break;
			case LUA_TBOOLEAN:
			{
				if( lua_isboolean( LuaScripting::GetState(), -1 ) )
					exists = true;
				
			}break;
			case LUA_TLIGHTUSERDATA:
			{
				if( lua_islightuserdata( LuaScripting::GetState(), -1 ) )
					exists = true;
				
			}break;
			case LUA_TNUMBER:
			{
				if( lua_isnumber( LuaScripting::GetState(), -1 ) )
					exists = true;
				
			}break;
			case LUA_TSTRING:
			{
				if( lua_isstring( LuaScripting::GetState(), -1 ) )
					exists = true;
				
			}break;
			case LUA_TTABLE:
			{

			}break;
			case LUA_TFUNCTION:
			{

			}break;
			case LUA_TUSERDATA:
			{

			}break;
			case LUA_TTHREAD:
			{

			}break;

			default:
				break;
		}

		lua_pop( LuaScripting::GetState(), 2 );

		return(exists);
	}

	return( false );
}

/////////////////////////////////////////////////////
/// Function: DoesTableItemExist
/// Params: [in]szItemName, [in]parameterIndex, [in]nLuaType
///
/////////////////////////////////////////////////////
bool script::DoesTableItemExist( const char *szItemName, int parameterIndex, int nLuaType )
{
	if( core::IsEmptyString( szItemName ) )
		return(false);

	// LUA_TNIL 
	// LUA_TNUMBER
	// LUA_TBOOLEAN
	// LUA_TSTRING
	// LUA_TTABLE
	// LUA_TFUNCTION
	// LUA_TUSERDATA
	// LUA_TTHREAD
	// LUA_TLIGHTUSERDATA

	if( lua_istable( LuaScripting::GetState(), parameterIndex ) )
	{
		// grab number
		lua_pushstring( LuaScripting::GetState(), szItemName );
		lua_gettable( LuaScripting::GetState(), -2 ); // key

		bool exists = false;
		switch( nLuaType )
		{
			case LUA_TNIL:
			{
				//if( lua_isnil( LuaScripting::GetState(), -1 ) )
				if( lua_type( LuaScripting::GetState(), -1 ) == LUA_TNIL )
					exists = true;
			}break;
			case LUA_TBOOLEAN:
			{
				//if( lua_isboolean( LuaScripting::GetState(), -1 ) )
				if( lua_type( LuaScripting::GetState(), -1 ) == LUA_TBOOLEAN )
					exists = true;
				
			}break;
			case LUA_TLIGHTUSERDATA:
			{
				//if( lua_islightuserdata( LuaScripting::GetState(), -1 ) )
				if( lua_type( LuaScripting::GetState(), -1 ) == LUA_TLIGHTUSERDATA )
					exists = true;
				
			}break;
			case LUA_TNUMBER:
			{
				//if( lua_isnumber( LuaScripting::GetState(), -1 ) )
				if( lua_type( LuaScripting::GetState(), -1 ) == LUA_TNUMBER )
					exists = true;
				
			}break;
			case LUA_TSTRING:
			{
				//if( lua_isstring( LuaScripting::GetState(), -1 ) )
				if( lua_type( LuaScripting::GetState(), -1 ) == LUA_TSTRING )
					exists = true;
				
			}break;
			case LUA_TTABLE:
			{

			}break;
			case LUA_TFUNCTION:
			{

			}break;
			case LUA_TUSERDATA:
			{

			}break;
			case LUA_TTHREAD:
			{

			}break;

			default:
				break;
		}

		lua_pop( LuaScripting::GetState(), 1 );

		return(exists);
	}

	return( false );
}

/////////////////////////////////////////////////////
/// Function: LuaGetStringFromTableItem
/// Params: [in]szTableName, [in]szItemName
///
/////////////////////////////////////////////////////
const char *script::LuaGetStringFromTableItem( const char *szTableName, const char *szItemName )
{
	const char *szString = 0;

	if( core::IsEmptyString( szTableName ) || core::IsEmptyString( szItemName ) )
		return( 0 );

	if( DoesTableItemExist( szTableName, szItemName, LUA_TSTRING ) )
	{
		lua_getglobal( script::LuaScripting::GetState(), szTableName );

		if( lua_istable( LuaScripting::GetState(), -1 ) )
		{
			// grab string
			lua_pushstring( LuaScripting::GetState(), szItemName );
			lua_gettable( LuaScripting::GetState(), -2 ); // key

			szString = lua_tostring( LuaScripting::GetState(), -1 );

			lua_pop( LuaScripting::GetState(), 1 );
		}

		lua_pop( LuaScripting::GetState(), 1 );
	}

	return( szString );
}

/////////////////////////////////////////////////////
/// Function: LuaGetStringFromTableItem
/// Params: [in]szItemName, [in]parameterIndex
///
/////////////////////////////////////////////////////
const char *script::LuaGetStringFromTableItem( const char *szItemName, int parameterIndex )
{
	const char *szString = 0;

	if( core::IsEmptyString( szItemName ) )
		return( 0 );

	if( DoesTableItemExist( szItemName, parameterIndex, LUA_TSTRING ) )
	{
		if( lua_istable( LuaScripting::GetState(), parameterIndex ) )
		{
			// grab string
			lua_pushstring( LuaScripting::GetState(), szItemName );
			lua_gettable( LuaScripting::GetState(), parameterIndex ); // key

			szString = lua_tostring( LuaScripting::GetState(), -1 );

			lua_pop( LuaScripting::GetState(), 1 );

			return( szString );
		}
	}

	return( 0 );
}

/////////////////////////////////////////////////////
/// Function: LuaGetBoolFromTableItem
/// Params: [in]szTableName, [in]szItemName
///
/////////////////////////////////////////////////////
bool script::LuaGetBoolFromTableItem( const char *szTableName, const char *szItemName, bool defaultValue )
{
	bool boolValue = defaultValue;

	if( core::IsEmptyString( szTableName ) || core::IsEmptyString( szItemName ) )
		return(boolValue);

	if( DoesTableItemExist( szTableName, szItemName, LUA_TBOOLEAN ) )
	{
		lua_getglobal( script::LuaScripting::GetState(), szTableName );

		if( lua_istable( LuaScripting::GetState(), -1 ) )
		{
			// grab number
			lua_pushstring( LuaScripting::GetState(), szItemName );
			lua_gettable( LuaScripting::GetState(), -2 ); // key

			boolValue = static_cast<bool>( (lua_toboolean( LuaScripting::GetState(), -1 )!=0) );

			lua_pop( LuaScripting::GetState(), 1 );	
		}

		lua_pop( LuaScripting::GetState(), 1 );
	}

	return( boolValue );
}

/////////////////////////////////////////////////////
/// Function: LuaGetNumberFromTableItem
/// Params: [in]szItemName, [in]parameterIndex
///
/////////////////////////////////////////////////////
bool script::LuaGetBoolFromTableItem( const char *szItemName, int parameterIndex, bool defaultValue )
{
	bool boolValue = defaultValue;

	if( core::IsEmptyString( szItemName ) )
		return(boolValue);

	if( DoesTableItemExist( szItemName, parameterIndex, LUA_TBOOLEAN ) )
	{
		if( lua_istable( LuaScripting::GetState(), -1 ) )
		{
			// grab number
			lua_pushstring( LuaScripting::GetState(), szItemName );
			lua_gettable( LuaScripting::GetState(), parameterIndex ); // key

			boolValue = static_cast<bool>( (lua_toboolean( LuaScripting::GetState(), -1 )!=0) );

			lua_pop( LuaScripting::GetState(), 1 );	
		}
	}
	return( boolValue );
}

/////////////////////////////////////////////////////
/// Function: LuaGetNumberFromTableItem
/// Params: [in]szTableName, [in]szItemName
///
/////////////////////////////////////////////////////
double script::LuaGetNumberFromTableItem( const char *szTableName, const char *szItemName, double defaultValue )
{
	double dValue = defaultValue;

	if( core::IsEmptyString( szTableName ) || core::IsEmptyString( szItemName ) )
		return(dValue);

	if( DoesTableItemExist( szTableName, szItemName, LUA_TNUMBER ) )
	{
		lua_getglobal( script::LuaScripting::GetState(), szTableName );

		if( lua_istable( LuaScripting::GetState(), -1 ) )
		{
			// grab number
			lua_pushstring( LuaScripting::GetState(), szItemName );
			lua_gettable( LuaScripting::GetState(), -2 ); // key

			dValue = lua_tonumber( LuaScripting::GetState(), -1 );

			lua_pop( LuaScripting::GetState(), 1 );
		}
	
		lua_pop( LuaScripting::GetState(), 1 );
	}

	return( dValue );
}

/////////////////////////////////////////////////////
/// Function: LuaGetNumberFromTableItem
/// Params: [in]szItemName, [in]parameterIndex
///
/////////////////////////////////////////////////////
double script::LuaGetNumberFromTableItem( const char *szItemName, int parameterIndex, double defaultValue )
{
	double dValue = defaultValue;

	if( core::IsEmptyString( szItemName ) )
		return(dValue);

	if( DoesTableItemExist( szItemName, parameterIndex, LUA_TNUMBER ) )
	{
		if( lua_istable( LuaScripting::GetState(), -1 ) )
		{
			// grab number
			lua_pushstring( LuaScripting::GetState(), szItemName );
			lua_gettable( LuaScripting::GetState(), parameterIndex ); // key

			dValue = lua_tonumber( LuaScripting::GetState(), -1 );

			lua_pop( LuaScripting::GetState(), 1 );
		}
	}
	return( dValue );
}

/////////////////////////////////////////////////////
/// Function: LuaGetUserDataFromTableItem
/// Params: [in]szTableName, [in]szItemName
///
/////////////////////////////////////////////////////
void* script::LuaGetUserDataFromTableItem( const char *szTableName, const char *szItemName, void* defaultValue )
{
	void* pValue = 0;

	if( core::IsEmptyString( szTableName ) || core::IsEmptyString( szItemName ) )
		return(pValue);

	lua_getglobal( script::LuaScripting::GetState(), szTableName );

	if( lua_istable( LuaScripting::GetState(), -1 ) )
	{
		// grab number
		lua_pushstring( LuaScripting::GetState(), szItemName );
		lua_gettable( LuaScripting::GetState(), -2 ); // key

		pValue = lua_touserdata( LuaScripting::GetState(), -1 );
		lua_pop( LuaScripting::GetState(), 1 );
	}
	
	lua_pop( LuaScripting::GetState(), 1 );

	return( pValue );
}

/////////////////////////////////////////////////////
/// Function: LuaGetUserDataFromTableItem
/// Params: [in]szItemName, [in]parameterIndex
///
/////////////////////////////////////////////////////
void* script::LuaGetUserDataFromTableItem( const char *szItemName, int parameterIndex, void* defaultValue )
{
	void* pValue = 0;

	if( core::IsEmptyString( szItemName ) )
		return(pValue);

	if( lua_istable( LuaScripting::GetState(), -1 ) )
	{
		// grab number
		lua_pushstring( LuaScripting::GetState(), szItemName );
		lua_gettable( LuaScripting::GetState(), parameterIndex ); // key

		pValue = lua_touserdata( LuaScripting::GetState(), -1 );

		lua_pop( LuaScripting::GetState(), 1 );
	}

	return( pValue );
}

/////////////////////////////////////////////////////
/// Function: StackDump
/// Params: [in]l
///
/////////////////////////////////////////////////////
void script::StackDump(lua_State* L)
{
    int i;
    int top = lua_gettop(L);
 
    DBGLOG("Total in stack %d\n", top);
 
    for (i = 1; i <= top; i++)
    {  /* repeat for each level */
        int t = lua_type(L, i);
        switch (t) {
            case LUA_TSTRING:  /* strings */
                DBGLOG("string: '%s'\n", lua_tostring(L, i));
                break;
            case LUA_TBOOLEAN:  /* booleans */
                DBGLOG("boolean %s\n",lua_toboolean(L, i) ? "true" : "false");
                break;
            case LUA_TNUMBER:  /* numbers */
                DBGLOG("number: %g\n", lua_tonumber(L, i));
                break;
            default:  /* other values */
                DBGLOG("%s\n", lua_typename(L, t));
                break;
        }
        DBGLOG("  ");  /* put a separator */
    }
    DBGLOG("\n");  /* end the listing */
}

/////////////////////////////////////////////////////
/// Function: PrintTable
/// Params: [in]szFuncName, [in]func
///
/////////////////////////////////////////////////////
void script::PrintTable(lua_State *L)
{
	lua_pushnil(L);

	while(lua_next(L, -2) != 0)
	{
		if(lua_isstring(L, -1))
			DBGLOG("LUASCRIPTING: %s = %s\n", lua_tostring(L, -2), lua_tostring(L, -1));
		else if(lua_isnumber(L, -1))
			DBGLOG("LUASCRIPTING: %s = %f\n", lua_tostring(L, -2), static_cast<float>( lua_tonumber(L, -1) ) );
		else if(lua_istable(L, -1))
			PrintTable(L);

		lua_pop(L, 1);
	}
}

#endif // BASE_SUPPORT_SCRIPTING

