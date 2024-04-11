
#ifndef __INVENTORYACCESS_H__
#define __INVENTORYACCESS_H__

void RegisterInventoryFunctions();

int ScriptSetDataValue( lua_State* pState );
int ScriptGetDataValue( lua_State* pState );

int ScriptAddItem( lua_State* pState );

int ScriptHasItem( lua_State* pState );

//int ScriptRemoveItem( lua_State* pState );

int ScriptSay( lua_State* pState );

int ScriptGivePlayerHealth( lua_State* pState );

int ScriptGivePlayerAmmo( lua_State* pState );

#ifdef DEVELOPER_SETUP
	int ScriptSetPlayerHealth( lua_State* pState );
	int ScriptSetPlayerAmmo( lua_State* pState );
#endif // DEVELOPER_SETUP


#endif // __INVENTORYACCESS_H__
