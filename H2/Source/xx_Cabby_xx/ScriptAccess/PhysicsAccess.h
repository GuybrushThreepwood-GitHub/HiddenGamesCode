
/*===================================================================
	File: PhysicsAccess.h
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#ifndef __PHYSICSACCESS_H__
#define __PHYSICSACCESS_H__

void RegisterPhysicsFunctions( ScriptDataHolder& dataHolder );

int ScriptCreateBody( lua_State* pState );
int ScriptGetBodyWorldCenter( lua_State* pState );

int ScriptCreateBoxShape( lua_State* pState );
int ScriptCreateCircleShape( lua_State* pState );
int ScriptCreatePolygonShape( lua_State* pState );
int ScriptDestroyShape( lua_State* pState );

int ScriptCreateDistanceJoint( lua_State* pState );
int ScriptCreateGearJoint( lua_State* pState );
int ScriptCreateMouseJoint( lua_State* pState );
int ScriptCreatePrismaticJoint( lua_State* pState );
int ScriptCreatePulleyJoint( lua_State* pState );
int ScriptCreateRevoluteJoint( lua_State* pState );;
int ScriptDestroyJoint( lua_State* pState );

int ScriptSetMassFromShapes( lua_State* pState );
int ScriptSetMeshToBody( lua_State* pState );

int ScriptSetBodyXForm( lua_State* pState );
int ScriptSetBodyMass( lua_State* pState );

#endif // __PHYSICSACCESS_H__
