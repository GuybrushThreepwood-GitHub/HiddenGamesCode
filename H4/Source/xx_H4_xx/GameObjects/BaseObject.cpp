
/*===================================================================
	File: BaseObject.cpp
	Game: H4

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "ScriptBase.h"

#include "BaseObject.h"

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
BaseObject::BaseObject( int physicsIdentifier, int physicsCastingId, int objectId, int objectType, lua_State* scriptState, void* userData)
	: PhysicsIdentifier( physicsIdentifier, physicsCastingId )
	, m_ObjectId(objectId)
	, m_ObjectType(objectType)
	, m_ScriptState(scriptState)
	, m_UserData(userData)
	, m_DrawState(true)
	, m_CollisionSate(false)
	, m_ActiveState(false)
{

}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
BaseObject::~BaseObject()
{

}
