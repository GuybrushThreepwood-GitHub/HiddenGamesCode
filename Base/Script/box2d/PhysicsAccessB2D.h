
/*===================================================================
	File: PhysicsAccessB2D.h
	Library: Script

	(C)Hidden Games
=====================================================================*/

#ifdef BASE_SUPPORT_BOX2D

#ifndef __PHYSICSACCESSB2D_H__
#define __PHYSICSACCESSB2D_H__

namespace script
{
	void RegisterPhysicsB2DFunctions();

	int ScriptCreateBody( lua_State* pState );
	int ScriptGetBodyWorldCenter( lua_State* pState );

	int ScriptCreateBoxShape( lua_State* pState );
	int ScriptCreateCircleShape( lua_State* pState );
	int ScriptCreatePolygonShape( lua_State* pState );
	int ScriptDestroyFixture( lua_State* pState );

	int ScriptCreateDistanceJoint( lua_State* pState );
	int ScriptCreateGearJoint( lua_State* pState );
	int ScriptCreateMouseJoint( lua_State* pState );
	int ScriptCreatePrismaticJoint( lua_State* pState );
	int ScriptCreatePulleyJoint( lua_State* pState );
	int ScriptCreateRevoluteJoint( lua_State* pState );
	int ScriptDestroyJoint( lua_State* pState );

	int ScriptSetMassFromShapes( lua_State* pState );
	int ScriptSetMeshToBody( lua_State* pState );

	int ScriptSetBodyXForm( lua_State* pState );
	int ScriptSetBodyMass( lua_State* pState );

} // namespace script

#endif // __PHYSICSACCESSB2D_H__

#endif // BASE_SUPPORT_BOX2D