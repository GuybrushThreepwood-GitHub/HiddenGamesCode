
#ifndef __ROOMACCESS_H__
#define __ROOMACCESS_H__

void RegisterRoomFunctions();

int ScriptSetupLimitedInstanceRooms( lua_State* pState );

int ScriptAddLimitedInstanceRoom( lua_State* pState );

int ScriptSetupExtendedInstanceRooms( lua_State* pState );

int ScriptAddExtendedInstanceRoom( lua_State* pState );

#endif // __ROOMACCESS_H__

