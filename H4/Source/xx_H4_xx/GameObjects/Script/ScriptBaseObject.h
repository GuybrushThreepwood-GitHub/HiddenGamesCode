
#ifndef __SCRIPTBASEOBJECT_H__
#define __SCRIPTBASEOBJECT_H__

#include "GameObjects/BaseObject.h"

void RegisterBaseObjectFunctions();

int ScriptSetPlayerProp( lua_State* pState );

int ScriptSetObjectProp( lua_State* pState );
int ScriptGetObjectProp( lua_State* pState );

int ScriptSetCameraProp( lua_State* pState );

void SetProperty( lua_State* pState, BaseObject* pObj, int objectProperty );
void SetDoorProperty( lua_State* pState, BaseObject* pObj, int objectProperty );
void SetEmitterProperty( lua_State* pState, BaseObject* pObj, int objectProperty );

#endif // __SCRIPTBASEOBJECT_H__

