
#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "CollisionBase.h"
#include "ModelBase.h"
#include "ScriptBase.h"

#include "Level/LevelTypes.h"
#include "GameObjects/BaseObject.h"

#include "GameSystems.h"

#include "Camera/GameCamera.h"
#include "GameObjects/Door.h"
#include "Effects/Emitter.h"

#include "ScriptBaseObject.h"

struct LuaIntGlobal
{
	const char *szConstantName;
	int nConstantValue;
	int nLuaType;
};

LuaIntGlobal objectGlobals[] = 
{
	// base object
	{ "DRAWSTATE",				static_cast<int>(BaseObject::DRAWSTATE),			LUA_TNUMBER },
	{ "COLLISIONSTATE",			static_cast<int>(BaseObject::COLLISIONSTATE),		LUA_TNUMBER },
	{ "ACTIVESTATE",			static_cast<int>(BaseObject::ACTIVESTATE),			LUA_TNUMBER },
	{ "NUMERIC1",				static_cast<int>(BaseObject::NUMERIC1),				LUA_TNUMBER },
	{ "NUMERIC2",				static_cast<int>(BaseObject::NUMERIC2),				LUA_TNUMBER },
	{ "NUMERIC3",				static_cast<int>(BaseObject::NUMERIC3),				LUA_TNUMBER },
	{ "NUMERIC4",				static_cast<int>(BaseObject::NUMERIC4),				LUA_TNUMBER },
	{ "NUMERIC5",				static_cast<int>(BaseObject::NUMERIC5),				LUA_TNUMBER },

	// player
	{ "PLAYERPOS",				static_cast<int>(Player::PLAYERPOS),				LUA_TNUMBER },
	{ "PLAYERROT",				static_cast<int>(Player::PLAYERROT),				LUA_TNUMBER },

	// camera properties
	{ "FOLLOW_PLAYER",			static_cast<int>(GameCamera::FOLLOW_PLAYER),		LUA_TNUMBER },
	{ "FOLLOW_OBJECT",			static_cast<int>(GameCamera::FOLLOW_OBJECT),		LUA_TNUMBER },
	{ "GOTO_POINT",				static_cast<int>(GameCamera::GOTO_POINT),			LUA_TNUMBER },
	{ "GOTO_OBJECT",			static_cast<int>(GameCamera::GOTO_OBJECT),			LUA_TNUMBER },
	{ "CHANGE_ZOOM",			static_cast<int>(GameCamera::CHANGE_ZOOM),			LUA_TNUMBER },
	{ "RESET_ZOOM",				static_cast<int>(GameCamera::RESET_ZOOM),			LUA_TNUMBER },

	// door properties
	{ "DOORTOGGLE",				static_cast<int>(Door::DOORTOGGLE),					LUA_TNUMBER },

	// emitter properties
	{ "EMITTER_STATE",			static_cast<int>(efx::Emitter::EMITTER_STATE),		LUA_TNUMBER },
};

/////////////////////////////////////////////////////
/// Function: RegisterBaseObjectFunctions
/// Params: None
///
/////////////////////////////////////////////////////
void RegisterBaseObjectFunctions()
{
	int i=0;

	for( i=0; i < sizeof(objectGlobals)/sizeof(LuaIntGlobal); ++i )
	{
		lua_pushinteger( script::LuaScripting::GetState(), objectGlobals[i].nConstantValue );
		lua_setglobal( script::LuaScripting::GetState(), objectGlobals[i].szConstantName );
	}

	script::LuaScripting::GetInstance()->RegisterFunction( "SetPlayerProp",	ScriptSetPlayerProp );

	script::LuaScripting::GetInstance()->RegisterFunction( "SetObjProp",		ScriptSetObjectProp );
	script::LuaScripting::GetInstance()->RegisterFunction( "GetObjProp",		ScriptGetObjectProp );

	script::LuaScripting::GetInstance()->RegisterFunction( "SetCamProp",		ScriptSetCameraProp );
}

/////////////////////////////////////////////////////
/// Function: ScriptSetPlayerProp
/// Params: None
///
/////////////////////////////////////////////////////
int ScriptSetPlayerProp( lua_State* pState )
{
	int objectProperty = static_cast<int>( lua_tointeger( pState, 1 ) );
	int objectIndex = static_cast<int>( lua_tointeger( pState, 2 ) );
	
	Player& player = GameSystems::GetInstance()->GetPlayer();

	switch(objectProperty)
	{
		case Player::PLAYERPOS:
		{
			BaseObject* pObj = GameSystems::GetInstance()->FindObject( objectIndex );
			if( pObj )
			{
				player.SetPosition( pObj->GetPos() );
			}
			else
			{
				DBGLOG( "SetPlayerProp: id - %d - not found", objectIndex );

				DBG_ASSERT_MSG( 0, "SetPlayerProp: id - %d - not found", objectIndex );
			}

		}break;
		case Player::PLAYERROT:
		{
			BaseObject* pObj = GameSystems::GetInstance()->FindObject( objectIndex );
			if( pObj )
			{
				player.SetAngle( pObj->GetRot() );
			}
			else
			{
				DBGLOG( "SetPlayerProp: id - %d - not found", objectIndex );
				
				DBG_ASSERT_MSG( 0, "SetPlayerProp: id - %d - not found", objectIndex );
			}

		}break;

		default:
		{
			DBG_ASSERT_MSG( 0, "Invalid property for SetPlayerProp" );
		}break;
	}

	return(1);
}

/////////////////////////////////////////////////////
/// Function: ScriptSetObjectProp
/// Params: None
///
/////////////////////////////////////////////////////
int ScriptSetObjectProp( lua_State* pState )
{
	int objectProperty = static_cast<int>( lua_tointeger( pState, 1 ) );
	int objectIndex = static_cast<int>( lua_tointeger( pState, 2 ) );
	
	// find the object
	BaseObject* pObj = GameSystems::GetInstance()->FindObject( objectIndex );
	if( pObj )
	{
		// grab the property
		switch(objectProperty)
		{
			case BaseObject::DRAWSTATE:
			{
				bool objectState = static_cast<bool>( lua_toboolean( pState, 3 ) != 0 );
				pObj->SetDrawState( objectState );
			}break;
			case BaseObject::COLLISIONSTATE:
			{
				bool objectState = static_cast<bool>( lua_toboolean( pState, 3 ) != 0 );
				pObj->SetCollisionState( objectState );
			}break;
			case BaseObject::ACTIVESTATE:
			{
				bool objectState = static_cast<bool>( lua_toboolean( pState, 3 ) != 0 );
				pObj->SetActiveState( objectState );
			}break;
			case BaseObject::NUMERIC1:
			{
				int numeric = static_cast<int>( lua_tointeger( pState, 3 ) );
				pObj->SetNumeric1( numeric );
			}break;
			case BaseObject::NUMERIC2:
			{
				int numeric = static_cast<int>( lua_tointeger( pState, 3 ) );
				pObj->SetNumeric2( numeric );
			}break;
			case BaseObject::NUMERIC3:
			{
				int numeric = static_cast<int>( lua_tointeger( pState, 3 ) );
				pObj->SetNumeric3( numeric );
			}break;
			case BaseObject::NUMERIC4:
			{
				int numeric = static_cast<int>( lua_tointeger( pState, 3 ) );
				pObj->SetNumeric4( numeric );
			}break;
			case BaseObject::NUMERIC5:
			{
				int numeric = static_cast<int>( lua_tointeger( pState, 3 ) );
				pObj->SetNumeric5( numeric );
			}break;
			default:
				SetProperty( pState, pObj, objectProperty );
				break;
		}
	}
#ifdef _DEBUG
	else
	{
		DBGLOG( "SetObjProp: id - %d - not found", objectIndex );

		DBG_ASSERT_MSG( 0, "SetObjProp: id - %d - not found", objectIndex );
	}
#endif // _DEBUG

	return(0);
}

/////////////////////////////////////////////////////
/// Function: ScriptGetObjectProp
/// Params: None
///
/////////////////////////////////////////////////////
int ScriptGetObjectProp( lua_State* pState )
{
	//int objectProperty = static_cast<int>( lua_tointeger( pState, 1 ) );
	int objectIndex = static_cast<int>( lua_tointeger( pState, 2 ) );
	
	// find the object
	BaseObject* pObj = GameSystems::GetInstance()->FindObject( objectIndex );
	if( pObj )
	{

	}
#ifdef _DEBUG
	else
	{
		DBGLOG( "GetObjProp: id - %d - not found", objectIndex );

		DBG_ASSERT_MSG( 0, "GetObjProp: id - %d - not found", objectIndex );
	}
#endif // _DEBUG

	return(1);
}

/////////////////////////////////////////////////////
/// Function: ScriptSetCameraProp
/// Params: None
///
/////////////////////////////////////////////////////
int ScriptSetCameraProp( lua_State* pState )
{
	int objectProperty = static_cast<int>( lua_tointeger( pState, 1 ) );
	
	GameCamera* cam = GameSystems::GetInstance()->GetGameCamera();
	DBG_ASSERT_MSG( (cam != 0), "SetCamProp GameCamera class is invalid" );

	switch( objectProperty )
	{
		case GameCamera::FOLLOW_PLAYER:
		{
			const char* callFunc = lua_tostring( pState, 2 );

			cam->FollowPlayer( callFunc );
		}break;
		case GameCamera::FOLLOW_OBJECT:
		{
			int objectIndex = static_cast<int>( lua_tointeger( pState, 2 ) );

			// find the object
			BaseObject* pObj = GameSystems::GetInstance()->FindObject( objectIndex );

			if( pObj )
			{
				float followTime = static_cast<float>( lua_tonumber( pState, 3 ) );

				cam->FollowObject( pObj, followTime );
			}
			else
			{
				DBGLOG( "SetCamProp: id - %d - not found", objectIndex );
				DBG_ASSERT_MSG( 0, "SetCamProp: id - %d - not found", objectIndex );
			}
		}break;
		/*case GameCamera::GOTO_POINT:
		{
			int objectIndex = static_cast<int>( lua_tointeger( pState, 2 ) );

			// find the object
			BaseObject* pObj = GameSystems::GetInstance()->FindObject( objectIndex );

			if( pObj )
			{
				const char* callFunc = lua_tostring( pState, 3 );
				
				CameraPoint* pCamPoint = reinterpret_cast<CameraPoint*>(pObj);

				cam->GoToPoint( pCamPoint, callFunc );
			}
			else
			{
				DBGLOG( "SetCamProp: id - %d - not found", objectIndex );
				DBG_ASSERT_MSG( 0, "SetCamProp: id - %d - not found", objectIndex );
			}

		}break;*/
		case GameCamera::GOTO_OBJECT:
		{
			int objectIndex = static_cast<int>( lua_tointeger( pState, 2 ) );

			// find the object
			BaseObject* pObj = GameSystems::GetInstance()->FindObject( objectIndex );

			if( pObj )
			{
				const char* callFunc = lua_tostring( pState, 3 );
				
				cam->GoToObject( pObj, callFunc );
			}
			else
			{
				DBGLOG( "SetCamProp: id - %d - not found", objectIndex );
				DBG_ASSERT_MSG( 0, "SetCamProp: id - %d - not found", objectIndex );
			}
		}break;

		case GameCamera::CHANGE_ZOOM:
		{
			float zoomVal = static_cast<float>(lua_tonumber( pState, 2 ) );
			cam->SetZoom(zoomVal);
		}break;
		//case GameCamera::RESET_ZOOM:
		//{
		//	cam->ResetZoom();
		//}break;

		default:
			DBGLOG( "SetCamProp: property - %d - not found", objectProperty );
			DBG_ASSERT_MSG( 0, "SetCamProp: property - %d - not found", objectProperty );
			break;
	}


	return(1);
}

/////////////////////////////////////////////////////
/// Function: SetProperty
/// Params: None
///
/////////////////////////////////////////////////////
void SetProperty( lua_State* pState, BaseObject* pObj, int objectProperty )
{
	switch( pObj->GetType() )
	{
		case DOOR:
		{
			SetDoorProperty( pState, pObj, objectProperty );
		}break;
		case EMITTER:
		{
			SetEmitterProperty( pState, pObj, objectProperty );
		}break;
		default:
		{
			DBGLOG( "SetObjProp: object type - %d - not found", pObj->GetId() );
			DBG_ASSERT_MSG( 0, "SetObjProp: object type - %d - not found", pObj->GetId() );
		}break;
	}
}

/////////////////////////////////////////////////////
/// Function: SetDoorProperty
/// Params: None
///
/////////////////////////////////////////////////////
void SetDoorProperty( lua_State* pState, BaseObject* pObj, int objectProperty )
{
	Door* pDoor = 0;

	pDoor = reinterpret_cast<Door *>(pObj);
	DBG_ASSERT_MSG( (pDoor != 0), "SetDoorProperty: door object is invalid" );

	switch(objectProperty)
	{
		case Door::DOORTOGGLE:
		{
			pDoor->Toggle();
		}break;
		default:
		{
			DBGLOG( "SetObjProp (Door): property id - %d - not found", objectProperty );
			DBG_ASSERT_MSG( 0, "SetObjProp (Door): property id - %d - not found", objectProperty );
		}break;
	}
}

/////////////////////////////////////////////////////
/// Function: SetEmitterProperty
/// Params: None
///
/////////////////////////////////////////////////////
void SetEmitterProperty( lua_State* pState, BaseObject* pObj, int objectProperty )
{
	efx::Emitter* pEmitter = 0;

	pEmitter = reinterpret_cast<efx::Emitter *>(pObj->GetUserData());
	DBG_ASSERT_MSG( (pEmitter != 0), "SetEmitterProperty: emitter object is invalid" );

	switch(objectProperty)
	{
		case efx::Emitter::EMITTER_STATE:
		{
			bool state = static_cast<bool>( lua_toboolean( pState, 3 ) != 0 );

			if( state )
				pEmitter->Enable();
			else
				pEmitter->Disable();

		}break;
		default:
		{
			DBGLOG( "SetObjProp (Emitter): property id - %d - not found", objectProperty );
			DBG_ASSERT_MSG( 0, "SetObjProp (Emitter): property id - %d - not found", objectProperty );
		}break;
	}
}
