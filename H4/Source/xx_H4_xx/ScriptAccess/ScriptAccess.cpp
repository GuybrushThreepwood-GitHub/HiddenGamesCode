
/*===================================================================
	File: ScriptAccess.cpp
	Game: H4

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "SoundBase.h"
#include "CollisionBase.h"
#include "ModelBase.h"
#include "InputBase.h"
#include "ScriptBase.h"

#include "H4.h"
#include "AppConsts.h"

#include "Audio/AudioSystem.h"
//#include "Physics/PhysicsWorld.h"

#include "ScriptAccess/ScriptDataHolder.h"
#include "ScriptAccess/Emitter/EmitterAccess.h"
#include "ScriptAccess/WorldAccess.h"
#include "ScriptAccess/ScriptAccess.h"
#include "ScriptAccess/H4/H4Access.h"

#include "MicroGame/MicroGameID.h"

namespace h4Script
{
	ScriptDataHolder* pScriptData = 0;
}

LuaGlobal scriptGlobals[] = 
{
	// fog setup
	{ "GL_LINEAR",				static_cast<double>(GL_LINEAR),				LUA_TNUMBER },
	{ "GL_EXP",					static_cast<double>(GL_EXP),				LUA_TNUMBER },
	{ "GL_EXP2",				static_cast<double>(GL_EXP2),				LUA_TNUMBER },

	// used by emitters

	// depth/alpha
	{ "GL_NEVER",		static_cast<double>(GL_NEVER),		LUA_TNUMBER }, 
	{ "GL_LESS",		static_cast<double>(GL_LESS),		LUA_TNUMBER },
	{ "GL_LEQUAL",		static_cast<double>(GL_LEQUAL),		LUA_TNUMBER },
	{ "GL_EQUAL",		static_cast<double>(GL_EQUAL),		LUA_TNUMBER },
	{ "GL_GREATER",		static_cast<double>(GL_GREATER),	LUA_TNUMBER },
	{ "GL_NOTEQUAL",	static_cast<double>(GL_NOTEQUAL),	LUA_TNUMBER },
	{ "GL_GEQUAL",		static_cast<double>(GL_GEQUAL),		LUA_TNUMBER },
	{ "GL_ALWAYS",		static_cast<double>(GL_ALWAYS),		LUA_TNUMBER },

	// blend
	{ "GL_ZERO",				static_cast<double>(GL_ZERO),				LUA_TNUMBER },
	{ "GL_ONE",					static_cast<double>(GL_ONE),				LUA_TNUMBER },
	{ "GL_SRC_COLOR",			static_cast<double>(GL_SRC_COLOR),			LUA_TNUMBER },
	{ "GL_ONE_MINUS_SRC_COLOR",	static_cast<double>(GL_ONE_MINUS_SRC_COLOR),LUA_TNUMBER },
	{ "GL_DST_COLOR",			static_cast<double>(GL_DST_COLOR),			LUA_TNUMBER },
	{ "GL_ONE_MINUS_DST_COLOR",	static_cast<double>(GL_ONE_MINUS_DST_COLOR),LUA_TNUMBER },
	{ "GL_SRC_ALPHA",			static_cast<double>(GL_SRC_ALPHA),			LUA_TNUMBER },
	{ "GL_ONE_MINUS_SRC_ALPHA",	static_cast<double>(GL_ONE_MINUS_SRC_ALPHA),LUA_TNUMBER },
	{ "GL_DST_ALPHA",			static_cast<double>(GL_DST_ALPHA),			LUA_TNUMBER },
	{ "GL_ONE_MINUS_DST_ALPHA",	static_cast<double>(GL_ONE_MINUS_DST_ALPHA),LUA_TNUMBER },
	{ "GL_SRC_ALPHA_SATURATE",	static_cast<double>(GL_SRC_ALPHA_SATURATE),	LUA_TNUMBER },

	// texture filters
	{ "GL_NEAREST",	static_cast<double>(GL_NEAREST),	LUA_TNUMBER },
	{ "GL_LINEAR",	static_cast<double>(GL_LINEAR),		LUA_TNUMBER },

	{ "GL_NEAREST_MIPMAP_NEAREST",	static_cast<double>(GL_NEAREST_MIPMAP_NEAREST),	LUA_TNUMBER },
	{ "GL_LINEAR_MIPMAP_NEAREST",	static_cast<double>(GL_LINEAR_MIPMAP_NEAREST),	LUA_TNUMBER },
	{ "GL_NEAREST_MIPMAP_LINEAR",	static_cast<double>(GL_NEAREST_MIPMAP_LINEAR),	LUA_TNUMBER },
	{ "GL_LINEAR_MIPMAP_LINEAR",	static_cast<double>(GL_LINEAR_MIPMAP_LINEAR),	LUA_TNUMBER },

	// states
	{ "FRONTEND_STATE",	static_cast<double>(0),	LUA_TNUMBER },
	{ "MAINGAME_STATE",	static_cast<double>(1),	LUA_TNUMBER },

	// micro game ids
	{ "MICROGAME_KEYPAD",			static_cast<double>(MICROGAME_KEYPAD),			LUA_TNUMBER },
	{ "MICROGAME_SLIDERHOLD",		static_cast<double>(MICROGAME_SLIDERHOLD),		LUA_TNUMBER },
	{ "MICROGAME_LIFTBUTTONS",		static_cast<double>(MICROGAME_LIFTBUTTONS),		LUA_TNUMBER },
	{ "MICROGAME_WIRING",			static_cast<double>(MICROGAME_WIRING),			LUA_TNUMBER },
	{ "MICROGAME_DIALS",			static_cast<double>(MICROGAME_DIALS),			LUA_TNUMBER },
	{ "MICROGAME_PICKUPITEM",		static_cast<double>(MICROGAME_PICKUPITEM),		LUA_TNUMBER },
	{ "MICROGAME_CARDSWIPE",		static_cast<double>(MICROGAME_CARDSWIPE),		LUA_TNUMBER },
	{ "MICROGAME_THUMBSCAN",		static_cast<double>(MICROGAME_THUMBSCAN),		LUA_TNUMBER },
	{ "MICROGAME_KEYLOCK",			static_cast<double>(MICROGAME_KEYLOCK),			LUA_TNUMBER },
	{ "MICROGAME_CRIMINALDOCS",		static_cast<double>(MICROGAME_CRIMINALDOCS),	LUA_TNUMBER },
	{ "MICROGAME_WATERTIGHTDOOR",	static_cast<double>(MICROGAME_WATERTIGHTDOOR),	LUA_TNUMBER },
	{ "MICROGAME_MAP",				static_cast<double>(MICROGAME_MAP),				LUA_TNUMBER },
	{ "MICROGAME_INVENTORY",		static_cast<double>(MICROGAME_INVENTORY),		LUA_TNUMBER },
	{ "MICROGAME_USEITEM",			static_cast<double>(MICROGAME_USEITEM),		LUA_TNUMBER },
};

/////////////////////////////////////////////////////
/// Function: GetPositionFromTable
/// Params: [in]pos, [in]paramIndex
///
/////////////////////////////////////////////////////
void GetPositionFromTable( math::Vec3& pos, int paramIndex );
void GetPositionFromTable( math::Vec3& pos, int paramIndex )
{
	pos.X = static_cast<float>( script::LuaGetNumberFromTableItem( "pos_x", paramIndex ) );
	pos.Y = static_cast<float>( script::LuaGetNumberFromTableItem( "pos_y", paramIndex ) );
	pos.Z = static_cast<float>( script::LuaGetNumberFromTableItem( "pos_z", paramIndex ) );
}

/////////////////////////////////////////////////////
/// Function: RegisterScriptFunctions
/// Params: [in]dataHolder
///
/////////////////////////////////////////////////////
void RegisterScriptFunctions( ScriptDataHolder& dataHolder )
{
	int i=0;
	h4Script::pScriptData = &dataHolder;

	// setup globals
	for( i=0; i < sizeof(scriptGlobals)/sizeof(LuaGlobal); ++i )
	{
		lua_pushnumber( script::LuaScripting::GetState(), scriptGlobals[i].nConstantValue );
		lua_setglobal( script::LuaScripting::GetState(), scriptGlobals[i].szConstantName );
	}

	script::LuaScripting::GetInstance()->RegisterFunction( "SetDevData",		ScriptSetDevData );

	script::LuaScripting::GetInstance()->RegisterFunction( "GetHiResMode",		ScriptGetHiResMode );

	RegisterH4Functions( dataHolder );
	script::RegisterPhysicsB2DFunctions();
	RegisterStageFunctions( dataHolder );
	RegisterEmitterFunctions( dataHolder );
}

/////////////////////////////////////////////////////
/// Function: ScriptGetHiResMode
/// Params: 
///
/////////////////////////////////////////////////////
int ScriptGetHiResMode( lua_State* pState )
{
	int val = static_cast<int>(H4::GetHiResMode());

	lua_pushnumber( pState, val );
	return(1);
}

/////////////////////////////////////////////////////
/// Function: ScriptSetDevData
/// Params: [in]pState
///
/////////////////////////////////////////////////////
int ScriptSetDevData( lua_State* pState )
{
	if( lua_istable( pState, 1 ) )
	{
		ScriptDataHolder::DevScriptData devData = h4Script::pScriptData->GetDevData();

		// grab the data
		devData.enablePhysicsDraw	= static_cast<bool>(script::LuaGetBoolFromTableItem( "enablePhysicsDraw", 1 )); 
		devData.enableDebugDraw		= static_cast<bool>(script::LuaGetBoolFromTableItem( "enableDebugDraw", 1 ));
		devData.enableDebugUIDraw	= static_cast<bool>(script::LuaGetBoolFromTableItem( "enableDebugUIDraw", 1 ));
		devData.enableSave			= static_cast<bool>(script::LuaGetBoolFromTableItem( "enableSave", 1, true ));
		devData.isTablet			= static_cast<bool>(script::LuaGetBoolFromTableItem( "isTablet", 1, false ));
		devData.isRetina			= static_cast<bool>(script::LuaGetBoolFromTableItem( "isRetina", 1, false ));
		devData.frameLock30			= static_cast<bool>(script::LuaGetBoolFromTableItem( "frameLock30", 1 )); 
		devData.frameLock60			= static_cast<bool>(script::LuaGetBoolFromTableItem( "frameLock60", 1 )); 
		devData.disableMusic		= static_cast<bool>(script::LuaGetBoolFromTableItem( "disableMusic", 1 ));  
		devData.enableFPSMode		= static_cast<bool>(script::LuaGetBoolFromTableItem( "enableFPSMode", 1 ));
		devData.enableNoise			= static_cast<bool>(script::LuaGetBoolFromTableItem( "enableNoise", 1 )); 
		devData.levelMipmap			= static_cast<bool>(script::LuaGetBoolFromTableItem( "levelMipmap", 1, true )); 
		devData.smoothFiltering		= static_cast<bool>(script::LuaGetBoolFromTableItem( "smoothFiltering", 1 ));
		devData.assetBaseWidth		= static_cast<int>(script::LuaGetNumberFromTableItem( "assetBaseWidth", 1 ));
		devData.assetBaseHeight		= static_cast<int>(script::LuaGetNumberFromTableItem( "assetBaseHeight", 1 ));
		devData.screenWidth			= static_cast<int>(script::LuaGetNumberFromTableItem( "screenWidth", 1 ));
		devData.screenHeight		= static_cast<int>(script::LuaGetNumberFromTableItem( "screenHeight", 1 ));
		devData.bootState			= static_cast<int>(script::LuaGetNumberFromTableItem( "bootState", 1 ));
		devData.hiResMode			= static_cast<bool>(script::LuaGetBoolFromTableItem( "hiResMode", 1 ));
		devData.useVertexArrays		= static_cast<bool>(script::LuaGetBoolFromTableItem( "useVertexArrays", 1 ));

		h4Script::pScriptData->SetDevData( devData );
	}

	return(0);
}

/////////////////////////////////////////////////////
/// Function: GetScriptDataHolder
/// Params: None
///
/////////////////////////////////////////////////////
ScriptDataHolder* GetScriptDataHolder()
{
	return h4Script::pScriptData;
}
