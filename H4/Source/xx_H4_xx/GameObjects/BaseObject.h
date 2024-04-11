
/*===================================================================
	File: BaseObject.h
	Game: H4

	(C)Hidden Games
=====================================================================*/

#ifndef __BASEOBJECT_H__
#define __BASEOBJECT_H__

#include "ScriptBase.h"
#include "PhysicsBase.h"

#include "Physics/PhysicsIds.h"

class BaseObject : public physics::PhysicsIdentifier 
{
	public:
		enum ObjectProperties
		{
			DRAWSTATE=0,
			COLLISIONSTATE,
			ACTIVESTATE,

			NUMERIC1,
			NUMERIC2,
			NUMERIC3,
			NUMERIC4,
			NUMERIC5,

			BASEOBJECT_STATECOUNT,

			EXTRASTATE = BASEOBJECT_STATECOUNT
		};

	public:
		BaseObject( int physicsIdentifier, int physicsCastingId, int objectId, int objectType, lua_State* scriptState, void* userData=0 );
		virtual ~BaseObject();

		virtual int PostConstruction()		{return(0);}
		virtual void OnCreate()				{}
		virtual void OnDrawState()			{}
		virtual void OnCollisionState()		{}
		virtual void OnActiveState()		{}

		// object id
		int GetId();

		// object type
		int GetType();

		// script state
		void SetScriptState( lua_State* pState );
		lua_State* GetScriptState();

		// object position
		void SetPos( const math::Vec3& pos );
		const math::Vec3& GetPos() const;

		// object rotation
		void SetRot( float rot );
		float GetRot();

		// should the object be drawn
		void SetDrawState( bool state );
		bool GetDrawState();

		// should the object have collision/physics enabled
		void SetCollisionState( bool state );
		bool GetCollisionState();

		// should the object be moving/animating
		void SetActiveState( bool state );
		bool GetActiveState();

		// gets user data
		void* GetUserData();

	private:
		int m_ObjectId;
		int m_ObjectType;

		lua_State* m_ScriptState;
		void* m_UserData;

		bool m_DrawState;
		bool m_CollisionSate;
		bool m_ActiveState;

		math::Vec3 m_Pos;
		float m_Rot;
};

/////////////////////////////////////////////////////
/// Method: GetId
/// Params: None
///
/////////////////////////////////////////////////////
inline int BaseObject::GetId()
{
	return(m_ObjectId);
}

/////////////////////////////////////////////////////
/// Method: GetType
/// Params: None
///
/////////////////////////////////////////////////////
inline int BaseObject::GetType()
{
	return(m_ObjectType);
}

/////////////////////////////////////////////////////
/// Method: SetScriptState
/// Params: None
///
/////////////////////////////////////////////////////
inline void BaseObject::SetScriptState( lua_State* state )
{
	m_ScriptState = state;
}

/////////////////////////////////////////////////////
/// Method: GetScriptState
/// Params: None
///
/////////////////////////////////////////////////////		
inline lua_State* BaseObject::GetScriptState()
{
	return(m_ScriptState);
}

/////////////////////////////////////////////////////
/// Method: SetPos
/// Params: [in]pos
///
/////////////////////////////////////////////////////
inline void BaseObject::SetPos( const math::Vec3& pos )
{
	m_Pos = pos;
}

/////////////////////////////////////////////////////
/// Method: GetPos
/// Params: None
///
/////////////////////////////////////////////////////
inline const math::Vec3& BaseObject::GetPos() const
{
	return(m_Pos);
}

/////////////////////////////////////////////////////
/// Method: SetRot
/// Params: [in]rot
///
/////////////////////////////////////////////////////
inline void BaseObject::SetRot( float rot )
{
	m_Rot = rot;
}

/////////////////////////////////////////////////////
/// Method: GetRot
/// Params: None
///
/////////////////////////////////////////////////////
inline float BaseObject::GetRot()
{
	return(m_Rot);
}

/////////////////////////////////////////////////////
/// Method: SetDrawState
/// Params: [in]state
///
/////////////////////////////////////////////////////
inline void BaseObject::SetDrawState( bool state )
{
	m_DrawState = state;
	OnDrawState();
}

/////////////////////////////////////////////////////
/// Method: GetDrawState
/// Params: None
///
/////////////////////////////////////////////////////
inline bool BaseObject::GetDrawState()
{
	return(m_DrawState);
}

/////////////////////////////////////////////////////
/// Method: SetCollisionState
/// Params: None
///
/////////////////////////////////////////////////////
inline void BaseObject::SetCollisionState( bool state )
{
	m_CollisionSate = state;
	OnCollisionState();
}

/////////////////////////////////////////////////////
/// Method: GetCollisionState
/// Params: None
///
/////////////////////////////////////////////////////
inline bool BaseObject::GetCollisionState()
{
	return(m_CollisionSate);
}

/////////////////////////////////////////////////////
/// Method: SetActiveState
/// Params: [in]state
///
/////////////////////////////////////////////////////
inline void BaseObject::SetActiveState( bool state )
{
	m_ActiveState = state;
	OnActiveState();
}

/////////////////////////////////////////////////////
/// Method: GetActiveState
/// Params: None
///
/////////////////////////////////////////////////////
inline bool BaseObject::GetActiveState()
{
	return(m_ActiveState);
}

/////////////////////////////////////////////////////
/// Method: GetUserData
/// Params: None
///
/////////////////////////////////////////////////////
inline void* BaseObject::GetUserData()
{
	return(m_UserData);
}

#endif // __BASEOBJECT_H__

