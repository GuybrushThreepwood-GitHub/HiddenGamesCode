
/*===================================================================
	File: InventoryAccess.cpp
	Game: H4

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "ScriptBase.h"

#include "AppConsts.h"

#include "GameSystems.h"
#include "ScriptAccess/H4/Inventory/InventoryAccess.h"


/////////////////////////////////////////////////////
/// Function: RegisterInventoryFunctions
/// Params: 
///
/////////////////////////////////////////////////////
void RegisterInventoryFunctions( )
{
	script::LuaScripting::GetInstance()->RegisterFunction( "SetDataValue",					ScriptSetDataValue );
	script::LuaScripting::GetInstance()->RegisterFunction( "GetDataValue",					ScriptGetDataValue );

	script::LuaScripting::GetInstance()->RegisterFunction( "AddItem",						ScriptAddItem );
	script::LuaScripting::GetInstance()->RegisterFunction( "HasItem",						ScriptHasItem );

	script::LuaScripting::GetInstance()->RegisterFunction( "Say",							ScriptSay );

	script::LuaScripting::GetInstance()->RegisterFunction( "GivePlayerHealth",				ScriptGivePlayerHealth );
	script::LuaScripting::GetInstance()->RegisterFunction( "GivePlayerAmmo",				ScriptGivePlayerAmmo );

#ifdef DEVELOPER_SETUP
	script::LuaScripting::GetInstance()->RegisterFunction( "SetPlayerHealth",				ScriptSetPlayerHealth );
	script::LuaScripting::GetInstance()->RegisterFunction( "SetPlayerAmmo",				ScriptSetPlayerAmmo );
#endif // DEVELOPER_SETUP
}

/////////////////////////////////////////////////////
/// Function: ScriptSetDataValue
/// Params: 
///
/////////////////////////////////////////////////////
int ScriptSetDataValue( lua_State* pState )
{
	const char* varName = lua_tostring( pState, 1 );
	int varValue = static_cast<int>( lua_tonumber( pState, 2 ) );

	GameSystems::GetInstance()->SetDataValue( varName, varValue );

	return(0);
}

/////////////////////////////////////////////////////
/// Function: ScriptGetDataValue
/// Params: 
///
/////////////////////////////////////////////////////
int ScriptGetDataValue( lua_State* pState )
{
	const char* varName = lua_tostring( pState, 1 );

	int val = GameSystems::GetInstance()->GetDataValue( varName );

	lua_pushnumber( pState, val );
	return(1);
}

/////////////////////////////////////////////////////
/// Function: ScriptAddItem
/// Params: 
///
/////////////////////////////////////////////////////
int ScriptAddItem( lua_State* pState )
{
	int index = static_cast<int>( lua_tonumber( pState, 1 ) );

	GameSystems::GetInstance()->AddItem( index );

	return(0);
}

/////////////////////////////////////////////////////
/// Function: ScriptHasItem
/// Params: 
///
/////////////////////////////////////////////////////
int ScriptHasItem( lua_State* pState )
{
	int index = static_cast<int>( lua_tonumber( pState, 1 ) );

	bool hasItem = GameSystems::GetInstance()->HasItem( index );

	lua_pushboolean( pState, (hasItem != 0) );

	return(1);
}

/////////////////////////////////////////////////////
/// Function: ScriptSay
/// Params: 
///
/////////////////////////////////////////////////////
int ScriptSay( lua_State* pState )
{
	int index1 = static_cast<int>( lua_tonumber( pState, 1 ) );
	int index2 = static_cast<int>( lua_tonumber( pState, 2 ) );

	const char* callFunc = lua_tostring( pState, 3 );

	GameSystems::GetInstance()->Say( index1, index2, callFunc );

	return(0);
}

/////////////////////////////////////////////////////
/// Function: ScriptGivePlayerHealth
/// Params: 
///
/////////////////////////////////////////////////////
int ScriptGivePlayerHealth( lua_State* pState )
{
	int healthAmount = static_cast<int>( lua_tonumber( pState, 1 ) );

	GameSystems::GetInstance()->GetPlayer().GiveHealth( healthAmount );

	return(0);
}

/////////////////////////////////////////////////////
/// Function: ScriptGivePlayerAmmo
/// Params: 
///
/////////////////////////////////////////////////////
int ScriptGivePlayerAmmo( lua_State* pState )
{
	int pistolAmount = static_cast<int>( lua_tonumber( pState, 1 ) );
	int shotgunAmount = static_cast<int>( lua_tonumber( pState, 2 ) );

	GameSystems::GetInstance()->GetPlayer().GiveAmmo( pistolAmount, shotgunAmount );

	return(0);
}

#ifdef DEVELOPER_SETUP
	/////////////////////////////////////////////////////
	/// Function: ScriptSetPlayerHealth
	/// Params: 
	///
	/////////////////////////////////////////////////////
	int ScriptSetPlayerHealth( lua_State* pState )
	{
		int healthAmount = static_cast<int>( lua_tonumber( pState, 1 ) );

		GameSystems::GameSaveData& saveData = GameSystems::GetInstance()->GetSaveData();
		
		saveData.playerHealth = healthAmount;

		GameSystems::GetInstance()->SetSaveData( saveData );

		return(0);
	}

	/////////////////////////////////////////////////////
	/// Function: ScriptSetPlayerAmmo
	/// Params: 
	///
	/////////////////////////////////////////////////////
	int ScriptSetPlayerAmmo( lua_State* pState )
	{
		int pistolMag = static_cast<int>( lua_tonumber( pState, 1 ) );
		int pistolTotal = static_cast<int>( lua_tonumber( pState, 2 ) );

		int shotgunMag = static_cast<int>( lua_tonumber( pState, 3 ) );
		int shotgunTotal = static_cast<int>( lua_tonumber( pState, 4 ) );

		GameSystems::GameSaveData& saveData = GameSystems::GetInstance()->GetSaveData();
		
		saveData.pistolAmmo			= pistolMag;
		saveData.pistolAmmoTotal	= pistolTotal;
		saveData.shotgunAmmo		= shotgunMag;
		saveData.shotgunAmmoTotal	= shotgunTotal;

		GameSystems::GetInstance()->SetSaveData( saveData );

		return(0);
	}
#endif // DEVELOPER_SETUP