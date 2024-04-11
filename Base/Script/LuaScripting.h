
#ifndef __LUASCRIPTING_H__
#define __LUASCRIPTING_H__

#ifdef BASE_SUPPORT_SCRIPTING

#define CallLuaFunc( FuncName, Params, Results )		\
	{													\
		lua_getglobal( script::LuaScripting::GetState(), FuncName );			\
		lua_call( script::LuaScripting::GetState(), Params, Results );			\
	}

#define GetIntParam( Index )	\
		( int ) lua_tonumber( script::LuaScripting::GetState(), Index );

#define GetFloatParam( Index )	\
		( float ) lua_tonumber( script::LuaScripting::GetState(), Index );

#define GetStringParam( Index )	\
		reinterpret_cast<char *>(lua_tostring( script::LuaScripting::GetState(), Index ));

#define ReturnNumeric( Numeric )	\
		lua_pushnumber( script::LuaScripting::GetState(), Numeric );

#define lua_unregister( FuncName )						\
		{												\
			lua_pushnil( script::LuaScripting::GetState() );					\
			lua_setglobal( script::LuaScripting::GetState(), FuncName );		\
		}

namespace script
{
	#define CHECK_LUASTATE()	DBG_ASSERT( (script::LuaScripting::GetState() != 0) );

	struct LuaGlobal
	{
		const char *szConstantName;
		double nConstantValue;
		int nLuaType;
	};

	struct TScriptHolder
	{
		/// enable flag
		bool bEnabled;

		/// script saved filename
		char szScriptFilename[core::MAX_PATH+core::MAX_PATH];

		/// script string
		char *szScriptString;
		/// script string length
		unsigned int nScriptLength;
		/// script number of lines
		unsigned int nScriptLines;

		/// name of function to call to enter script
		char szFunctionCallName[script::FUNCTION_NAME_SIZE];

		/// script compiled flag
		bool bCompiled;
	};

	class LuaScripting
	{
		public:
			/// default constructor
			LuaScripting();
			/// default destructor
			~LuaScripting();

			/// Init - initialise the lua library
			/// \param nInitFlags - flags denoting which functions to register
			void Init();

			/// RegisterFunction - Registers a new function with the current lua state
			/// \param szFuncName - script function name
			/// \param func - C function name
			/// \return integer - ( SUCCESS: OK or FAIL: FAIL )
			int RegisterFunction( const char *szFuncName, lua_CFunction func );
			/// LoadScript - Loads a lua script file
			/// \param szFilename - file to load
			/// \return integer - ( SUCCESS: OK or FAIL: FAIL )
			int LoadScript( const char *szFilename );
			/// LoadScriptFromString - Loads a lua script file from a passed string
			/// \param szString - string to load as a script
			/// \return integer - ( SUCCESS: OK or FAIL: FAIL )
			int LoadScriptFromString( const char *szString );
			/// SaveScript - Saves a lua script file from a passed script holder
			/// \param szFilename - file to save
			/// \param pScript - pointer to script holder
			/// \return integer - ( SUCCESS: OK or FAIL: FAIL )
			int SaveScript( const char *szFilename, TScriptHolder *pScript );
			/// SaveScriptFromString - Saves a lua script file from a passed string
			/// \param szFilename - file to save
			/// \param szString - string to save as a script
			/// \return integer - ( SUCCESS: OK or FAIL: FAIL )
			int SaveScriptFromString( const char *szFilename, const char *szString );

			static void Initialise( void );
			static void Shutdown( void );

			static LuaScripting *GetInstance( void ) 
			{
				DBG_ASSERT( (ms_Instance != 0) );

				return( ms_Instance );
			}
			static lua_State *GetState( void ) 
			{
				DBG_ASSERT( (ms_LUAState != 0) );

				return( ms_LUAState );
			}

			static bool IsInitialised( void ) 
			{
				return( (ms_Instance != 0) && (ms_LUAState != 0) );
			}

		private:
			static LuaScripting* ms_Instance;
			static lua_State *ms_LUAState;
	};

	/// GetErrorFuncIndex - push the error function and return its index
	/// \return integer - error function index
	int GetErrorFuncIndex();

	/// LoadScript - Loads a lua script file
	/// \param szFilename - file to load
	/// \return integer - ( SUCCESS: OK or FAIL: FAIL )
	int LoadScript( const char *szFilename );
	/// LuaFunctionCheck - Checks a lua function exists
	/// \param szFunctionName - function name
	/// \return integer - ( SUCCESS: OK or FAIL: FAIL )
	int LuaFunctionCheck( const char *szFunctionName );
	/// LuaCallFunction - Calls a lua function
	/// \param szFunctionName - function name
	/// \param nParams - function param count
	/// \param nResults - function return count
	void LuaCallFunction( const char *szFunctionName, int nParams, int nResults );
	/// LuaAddGlobal - Adds a global variable to lua
	/// \param szGlobalName - global name
	/// \param pValue - global value
	/// \param nLuaType - global type
	void LuaAddGlobal( const char *szGlobalName, void *pValue, int nLuaType );

	/// DoesTableItemExist - Tries to find an item in a global table
	/// \param szTableName - global table name
	/// \param szItemName - global tables item to request
	/// \param nLuaType - global type
	/// \return boolean - Does exist: true or Doesn't exist: false
	bool DoesTableItemExist( const char *szTableName, const char *szItemName, int nLuaType );
	/// DoesTableItemExist - Tries to find an item in a table
	/// \param szItemName - global tables item to request
	/// \param parameterIndex - which parameter is the table in the current state
	/// \param nLuaType - global type
	/// \return boolean - Does exist: true or Doesn't exist: false
	bool DoesTableItemExist( const char *szItemName, int parameterIndex, int nLuaType );

	/// LuaGetStringFromTableItem - Gets the value of a string item in a global table
	/// \param szTableName - global table name
	/// \param szItemName - global tables item to request
	/// \return const char pointer - value of items string
	const char *LuaGetStringFromTableItem( const char *szTableName, const char *szItemName );
	/// LuaGetStringFromTableItem - Gets the value of a string item in a table
	/// \param szItemName - tables item to request
	/// \param parameterIndex - which parameter in the state is the table
	/// \return const char pointer - value of items string
	const char *LuaGetStringFromTableItem( const char *szItemName, int parameterIndex );

	/// LuaGetBoolFromTableItem - Gets the value of a bool item in a global table
	/// \param szTableName - global table name
	/// \param szItemName - global tables item to request
	/// \param defaultValue - value to set if the item is not found in the table
	/// \return const char pointer - value of items string
	bool LuaGetBoolFromTableItem( const char *szTableName, const char *szItemName, bool defaultValue=false );
	/// LuaGetBoolFromTableItem - Gets the value of a bool item in a table
	/// \param szItemName - tables item to request
	/// \param parameterIndex - which parameter in the state is the table
	/// \param defaultValue - value to set if the item is not found in the table
	/// \return const char pointer - value of items string
	bool LuaGetBoolFromTableItem( const char *szItemName, int parameterIndex, bool defaultValue=false );

	/// LuaGetNumberFromTableItem - Gets the value of a number item in a global table
	/// \param szTableName - global table name
	/// \param szItemName - global tables item to request
	/// \param defaultValue - value to set if the item is not found in the table
	/// \return double - value of items number
	double LuaGetNumberFromTableItem( const char *szTableName, const char *szItemName, double defaultValue=0.0 );
	/// LuaGetNumberFromTableItem - Gets the value of a number item in a table
	/// \param szItemName - tables item to request
	/// \param parameterIndex - which parameter in the state is the table
	/// \param defaultValue - value to set if the item is not found in the table
	/// \return double - value of items number
	double LuaGetNumberFromTableItem( const char *szItemName, int parameterIndex, double defaultValue=0.0 );

	/// LuaGetUserDataFromTableItem - Gets the pointer to user data in a global table
	/// \param szTableName - global table name
	/// \param szItemName - global tables item to request
	/// \param defaultValue - value to set if the item is not found in the table
	/// \return pointer - pointer to light user data
	void* LuaGetUserDataFromTableItem( const char *szTableName, const char *szItemName, void* defaultValue=0 );
	/// LuaGetUserDataFromTableItem - Gets the pointer to user data in a global table
	/// \param szItemName - tables item to request
	/// \param parameterIndex - which parameter in the state is the table
	/// \param defaultValue - value to set if the item is not found in the table
	/// \return pointer - pointer to light user data
	void* LuaGetUserDataFromTableItem( const char *szItemName, int parameterIndex, void* defaultValue=0 );
	
	/// StackDump - Prints a lua stack dump
	/// \param L - current state
	void StackDump(lua_State* L);

	/// PrintTable - Debug prints the contents of a table
	/// \param L - current state
	void PrintTable(lua_State *L);

} // namespace script 

#endif // BASE_SUPPORT_SCRIPTING

#endif // __LUASCRIPTING_H__


