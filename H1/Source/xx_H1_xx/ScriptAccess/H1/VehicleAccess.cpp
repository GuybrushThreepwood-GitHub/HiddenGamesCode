
/*===================================================================
	File: VehicleAccess.cpp
	Game: AirCadets

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "SoundBase.h"
#include "CollisionBase.h"
#include "Model/ModelCommon.h"
#include "ModelBase.h"
#include "InputBase.h"
#include "ScriptBase.h"

#include "Audio/AudioSystem.h"
#include "Resources/EmitterResources.h"
#include "Resources/StringResources.h"

#include "Level/Level.h"
#include "Player/Player.h"

#include "ScriptAccess/ScriptDataHolder.h"
#include "ScriptAccess/H1/VehicleAccess.h"

namespace h1Script
{
	ScriptDataHolder* pScriptVehicleData = 0;
}

/////////////////////////////////////////////////////
/// Function: RegisterVehicleFunctions
/// Params: [in]dataHolder
///
/////////////////////////////////////////////////////
void RegisterVehicleFunctions( ScriptDataHolder& dataHolder )
{
	h1Script::pScriptVehicleData = &dataHolder;

	script::LuaScripting::GetInstance()->RegisterFunction( "AddVehiclePack", ScriptAddVehiclePack );
	script::LuaScripting::GetInstance()->RegisterFunction( "SetupVehicle", ScriptSetupVehicle );
}

/////////////////////////////////////////////////////
/// Function: ScriptAddVehiclePack
/// Params: [in]pState
///
/////////////////////////////////////////////////////
int ScriptAddVehiclePack( lua_State* pState )
{
	if( lua_istable( pState, 1 ) )
	{
		int i=0;

		ScriptDataHolder::VehiclePackData* pVehiclePack = 0;
		pVehiclePack = new ScriptDataHolder::VehiclePackData;
		DBG_ASSERT( pVehiclePack != 0 );

		std::memset( pVehiclePack, 0, sizeof(ScriptDataHolder::VehiclePackData) );

		pVehiclePack->packId = static_cast<int>( script::LuaGetNumberFromTableItem( "packId", 1 ) );
		pVehiclePack->packVehicleCount = static_cast<unsigned int>( script::LuaGetNumberFromTableItem( "packCount", 1 ) );

		pVehiclePack->pPackVehicleInfo = new ScriptDataHolder::VehicleInfo[pVehiclePack->packVehicleCount];
		DBG_ASSERT( pVehiclePack->pPackVehicleInfo != 0 );

		pVehiclePack->pVehicleScriptData = new ScriptDataHolder::VehicleScriptData[pVehiclePack->packVehicleCount];
		DBG_ASSERT( pVehiclePack->pVehicleScriptData != 0 );

		for( i=0; i < pVehiclePack->packVehicleCount; ++i )
		{
			char vehicleNameId[core::MAX_PATH];
			std::memset( vehicleNameId, core::MAX_PATH, sizeof(char)*core::MAX_PATH );
			snprintf( vehicleNameId, core::MAX_PATH, "vehicleSetupFilename%d", i+1 );

			pVehiclePack->pPackVehicleInfo[i].vehicleScriptName = script::LuaGetStringFromTableItem( vehicleNameId, 1 );
			DBG_ASSERT( pVehiclePack->pPackVehicleInfo[i].vehicleScriptName != 0 );

			// load the script for the pack
			if( !core::IsEmptyString(pVehiclePack->pPackVehicleInfo[i].vehicleScriptName) )
				script::LoadScript( pVehiclePack->pPackVehicleInfo[i].vehicleScriptName );

			std::memset( vehicleNameId, core::MAX_PATH, sizeof(char)*core::MAX_PATH );
			snprintf( vehicleNameId, core::MAX_PATH, "vehicleSetupFunction%d", i+1 );

			pVehiclePack->pPackVehicleInfo[i].setupFunction = script::LuaGetStringFromTableItem( vehicleNameId, 1 );
			DBG_ASSERT( pVehiclePack->pPackVehicleInfo[i].setupFunction != 0 );

			if( pVehiclePack->pPackVehicleInfo[i].setupFunction != 0 )
			{
				int result = 0;

				int errorFuncIndex;
				errorFuncIndex = script::GetErrorFuncIndex();

				lua_getglobal( pState, pVehiclePack->pPackVehicleInfo[i].setupFunction );

				lua_pushlightuserdata( pState, &pVehiclePack->pVehicleScriptData[i] );

				// lua_call( gLUAState, nParams, nResults );
				result = lua_pcall( pState, 1, 0, errorFuncIndex );

				// LUA_ERRRUN --- a runtime error. 
				// LUA_ERRMEM --- memory allocation error. For such errors, Lua does not call the error handler function. 
				// LUA_ERRERR --- error while running the error handler function. 

				if( result == LUA_ERRRUN || result == LUA_ERRMEM || result == LUA_ERRERR )
				{
					DBGLOG( "LUASCRIPTING: *ERROR* Calling function '%s' failed\n", pVehiclePack->pPackVehicleInfo[i].setupFunction );
					DBGLOG( "\tLUA_TRACEBACK: %s\n", lua_tostring( pState, -1 ) );
					DBG_ASSERT_MSG( 0, "LUASCRIPTING: *ERROR* Calling function '%s' failed", pVehiclePack->pPackVehicleInfo[i].setupFunction );
				}
			}

			std::memset( vehicleNameId, core::MAX_PATH, sizeof(char)*core::MAX_PATH );
			snprintf( vehicleNameId, core::MAX_PATH, "vehicleName%d", i+1 );

			pVehiclePack->pPackVehicleInfo[i].vehicleName = res::GetScriptString( static_cast<int>(script::LuaGetNumberFromTableItem( vehicleNameId, 1, 0.0 )) );
			DBG_ASSERT( pVehiclePack->pPackVehicleInfo[i].vehicleName != 0 );

			std::memset( vehicleNameId, core::MAX_PATH, sizeof(char)*core::MAX_PATH );
			snprintf( vehicleNameId, core::MAX_PATH, "liteVersionPurchase%d", i+1 );
			pVehiclePack->pPackVehicleInfo[i].liteVersionPurchase = static_cast<bool>( script::LuaGetBoolFromTableItem( vehicleNameId, 1, false ) != 0 );

			std::memset( vehicleNameId, core::MAX_PATH, sizeof(char)*core::MAX_PATH );
			snprintf( vehicleNameId, core::MAX_PATH, "vehiclePurchaseId%d", i+1 );
			pVehiclePack->pPackVehicleInfo[i].purchaseId = static_cast<int>(script::LuaGetNumberFromTableItem( vehicleNameId, 1, -1.0 ) );
		}

		h1Script::pScriptVehicleData->GetVehiclePackList().push_back(pVehiclePack);
	}

	return(0);
}

/////////////////////////////////////////////////////
/// Function: ScriptSetupVehicle
/// Params: [in]pState
///
/////////////////////////////////////////////////////
int ScriptSetupVehicle( lua_State* pState )
{
	ScriptDataHolder::VehicleScriptData* pVehicles = 0;
	if( lua_islightuserdata( pState, 1 ) )
		pVehicles = reinterpret_cast<ScriptDataHolder::VehicleScriptData*>( lua_touserdata( pState, 1 ) );
	DBG_ASSERT( pVehicles != 0 );

	int tableIndex = 2;
	if( lua_istable( pState, tableIndex ) )
	{		

		std::memset( pVehicles, 0, sizeof(ScriptDataHolder::VehicleScriptData) );

		pVehicles->aircraftName = static_cast<int>( script::LuaGetNumberFromTableItem( "aircraftName", tableIndex ) );

		pVehicles->modelIndex = static_cast<int>( script::LuaGetNumberFromTableItem( "modelFile", tableIndex ) );
		DBG_ASSERT( pVehicles->modelIndex != -1 );

		pVehicles->modelIndexUI = static_cast<int>( script::LuaGetNumberFromTableItem( "modelFileUI", tableIndex ) );
		DBG_ASSERT( pVehicles->modelIndexUI != -1 );

		pVehicles->modelFileShadow = static_cast<int>( script::LuaGetNumberFromTableItem( "modelFileShadow", tableIndex ) );
		DBG_ASSERT( pVehicles->modelFileShadow != -1 );
		
		pVehicles->physicsData =  script::LuaGetStringFromTableItem( "physicsData", tableIndex );
		DBG_ASSERT( pVehicles->modelFileShadow != -1 );

		pVehicles->minSpeed = static_cast<float>( script::LuaGetNumberFromTableItem( "minSpeed", tableIndex ) );
		pVehicles->maxSpeed = static_cast<float>( script::LuaGetNumberFromTableItem( "maxSpeed", tableIndex ) );
		pVehicles->accelerate = static_cast<float>( script::LuaGetNumberFromTableItem( "accelerate", tableIndex ) );
		pVehicles->decelerate = static_cast<float>( script::LuaGetNumberFromTableItem( "decelerate", tableIndex ) );

		pVehicles->rotSpeedX = static_cast<float>( script::LuaGetNumberFromTableItem( "rotSpeedX", tableIndex ) );
		pVehicles->rotSpeedY = static_cast<float>( script::LuaGetNumberFromTableItem( "rotSpeedY", tableIndex ) );
		pVehicles->rotSpeedZ = static_cast<float>( script::LuaGetNumberFromTableItem( "rotSpeedZ", tableIndex ) );

		pVehicles->maxRotXAngle = static_cast<float>( script::LuaGetNumberFromTableItem( "maxRotXAngle", tableIndex ) );
		pVehicles->maxRotZAngle = static_cast<float>( script::LuaGetNumberFromTableItem( "maxRotZAngle", tableIndex ) );

		pVehicles->bulletSpeed = static_cast<float>( script::LuaGetNumberFromTableItem( "bulletSpeed", tableIndex ) );
		pVehicles->maxBulletDistance = static_cast<float>( script::LuaGetNumberFromTableItem( "maxBulletDistance", tableIndex ) );
		pVehicles->bulletRate = static_cast<float>( script::LuaGetNumberFromTableItem( "bulletRate", tableIndex ) );
		pVehicles->bulletLife = static_cast<float>( script::LuaGetNumberFromTableItem( "bulletLife", tableIndex ) );

		// grab table
		lua_pushstring( pState, "prop_model" );
		lua_gettable( pState, -2 ); // key

		// new table is top
		if( lua_istable( pState, -1 ) )
		{
			int paramIndex = 3;

			math::Vec3 pos;
			math::Vec3 maxRot;

			pVehicles->propModel.partResId = static_cast<int>( script::LuaGetNumberFromTableItem( "modelFile", paramIndex, -1.0 ) );

			pVehicles->propModel.initialPos.X = static_cast<float>( script::LuaGetNumberFromTableItem( "posX", paramIndex, 0.0 ) );
			pVehicles->propModel.initialPos.Y = static_cast<float>( script::LuaGetNumberFromTableItem( "posY", paramIndex, 0.0 ) );
			pVehicles->propModel.initialPos.Z = static_cast<float>( script::LuaGetNumberFromTableItem( "posZ", paramIndex, 0.0 ) );
		}

		// remove rudder table
		lua_pop( pState, 1 );

		// grab table
		lua_pushstring( pState, "rudder" );
		lua_gettable( pState, -2 ); // key

		// new table is top
		if( lua_istable( pState, -1 ) )
		{
			int paramIndex = 3;

			math::Vec3 pos;
			math::Vec3 maxRot;

			pVehicles->rudder.partResId = static_cast<int>( script::LuaGetNumberFromTableItem( "modelFile", paramIndex, -1.0 ) );

			pVehicles->rudder.initialPos.X = static_cast<float>( script::LuaGetNumberFromTableItem( "posX", paramIndex, 0.0 ) );
			pVehicles->rudder.initialPos.Y = static_cast<float>( script::LuaGetNumberFromTableItem( "posY", paramIndex, 0.0 ) );
			pVehicles->rudder.initialPos.Z = static_cast<float>( script::LuaGetNumberFromTableItem( "posZ", paramIndex, 0.0 ) );

			pVehicles->rudder.maxRot.X = static_cast<float>( script::LuaGetNumberFromTableItem( "maxRotX", paramIndex, 0.0 ) );
			pVehicles->rudder.maxRot.Y = static_cast<float>( script::LuaGetNumberFromTableItem( "maxRotY", paramIndex, 0.0 ) );
			pVehicles->rudder.maxRot.Z = static_cast<float>( script::LuaGetNumberFromTableItem( "maxRotZ", paramIndex, 0.0 ) );

			pVehicles->rudder.rotSpeed.X = static_cast<float>( script::LuaGetNumberFromTableItem( "rotSpeedX", paramIndex, 0.0 ) );
			pVehicles->rudder.rotSpeed.Y = static_cast<float>( script::LuaGetNumberFromTableItem( "rotSpeedY", paramIndex, 0.0 ) );
			pVehicles->rudder.rotSpeed.Z = static_cast<float>( script::LuaGetNumberFromTableItem( "rotSpeedZ", paramIndex, 0.0 ) );
		}

		// remove rudder table
		lua_pop( pState, 1 );

		// grab table
		lua_pushstring( pState, "tail_left_flap" );
		lua_gettable( pState, -2 ); // key

		// new table is top
		if( lua_istable( pState, -1 ) )
		{
			int paramIndex = 3;

			math::Vec3 pos;
			math::Vec3 maxRot;

			pVehicles->leftTailFlap.partResId = static_cast<int>( script::LuaGetNumberFromTableItem( "modelFile", paramIndex, -1.0 ) );

			pVehicles->leftTailFlap.initialPos.X = static_cast<float>( script::LuaGetNumberFromTableItem( "posX", paramIndex, 0.0 ) );
			pVehicles->leftTailFlap.initialPos.Y = static_cast<float>( script::LuaGetNumberFromTableItem( "posY", paramIndex, 0.0 ) );
			pVehicles->leftTailFlap.initialPos.Z = static_cast<float>( script::LuaGetNumberFromTableItem( "posZ", paramIndex, 0.0 ) );

			pVehicles->leftTailFlap.maxRot.X = static_cast<float>( script::LuaGetNumberFromTableItem( "maxRotX", paramIndex, 0.0 ) );
			pVehicles->leftTailFlap.maxRot.Y = static_cast<float>( script::LuaGetNumberFromTableItem( "maxRotY", paramIndex, 0.0 ) );
			pVehicles->leftTailFlap.maxRot.Z = static_cast<float>( script::LuaGetNumberFromTableItem( "maxRotZ", paramIndex, 0.0 ) );

			pVehicles->leftTailFlap.rotSpeed.X = static_cast<float>( script::LuaGetNumberFromTableItem( "rotSpeedX", paramIndex, 0.0 ) );
			pVehicles->leftTailFlap.rotSpeed.Y = static_cast<float>( script::LuaGetNumberFromTableItem( "rotSpeedY", paramIndex, 0.0 ) );
			pVehicles->leftTailFlap.rotSpeed.Z = static_cast<float>( script::LuaGetNumberFromTableItem( "rotSpeedZ", paramIndex, 0.0 ) );
		}

		// remove tail_left_flap table
		lua_pop( pState, 1 );

		// grab table
		lua_pushstring( pState, "tail_right_flap" );
		lua_gettable( pState, -2 ); // key

		// new table is top
		if( lua_istable( pState, -1 ) )
		{
			int paramIndex = 3;

			math::Vec3 pos;
			math::Vec3 maxRot;

			pVehicles->rightTailFlap.partResId = static_cast<int>( script::LuaGetNumberFromTableItem( "modelFile", paramIndex, -1.0 ) );

			pVehicles->rightTailFlap.initialPos.X = static_cast<float>( script::LuaGetNumberFromTableItem( "posX", paramIndex, 0.0 ) );
			pVehicles->rightTailFlap.initialPos.Y = static_cast<float>( script::LuaGetNumberFromTableItem( "posY", paramIndex, 0.0 ) );
			pVehicles->rightTailFlap.initialPos.Z = static_cast<float>( script::LuaGetNumberFromTableItem( "posZ", paramIndex, 0.0 ) );

			pVehicles->rightTailFlap.maxRot.X = static_cast<float>( script::LuaGetNumberFromTableItem( "maxRotX", paramIndex, 0.0 ) );
			pVehicles->rightTailFlap.maxRot.Y = static_cast<float>( script::LuaGetNumberFromTableItem( "maxRotY", paramIndex, 0.0 ) );
			pVehicles->rightTailFlap.maxRot.Z = static_cast<float>( script::LuaGetNumberFromTableItem( "maxRotZ", paramIndex, 0.0 ) );

			pVehicles->rightTailFlap.rotSpeed.X = static_cast<float>( script::LuaGetNumberFromTableItem( "rotSpeedX", paramIndex, 0.0 ) );
			pVehicles->rightTailFlap.rotSpeed.Y = static_cast<float>( script::LuaGetNumberFromTableItem( "rotSpeedY", paramIndex, 0.0 ) );
			pVehicles->rightTailFlap.rotSpeed.Z = static_cast<float>( script::LuaGetNumberFromTableItem( "rotSpeedZ", paramIndex, 0.0 ) );
		}

		// remove tail_right_flap table
		lua_pop( pState, 1 );

		// grab table
		lua_pushstring( pState, "wing_left_flap" );
		lua_gettable( pState, -2 ); // key

		// new table is top
		if( lua_istable( pState, -1 ) )
		{
			int paramIndex = 3;

			math::Vec3 pos;
			math::Vec3 maxRot;

			pVehicles->leftWingFlap.partResId = static_cast<int>( script::LuaGetNumberFromTableItem( "modelFile", paramIndex, -1.0 ) );

			pVehicles->leftWingFlap.initialPos.X = static_cast<float>( script::LuaGetNumberFromTableItem( "posX", paramIndex, 0.0 ) );
			pVehicles->leftWingFlap.initialPos.Y = static_cast<float>( script::LuaGetNumberFromTableItem( "posY", paramIndex, 0.0 ) );
			pVehicles->leftWingFlap.initialPos.Z = static_cast<float>( script::LuaGetNumberFromTableItem( "posZ", paramIndex, 0.0 ) );

			pVehicles->leftWingFlap.maxRot.X = static_cast<float>( script::LuaGetNumberFromTableItem( "maxRotX", paramIndex, 0.0 ) );
			pVehicles->leftWingFlap.maxRot.Y = static_cast<float>( script::LuaGetNumberFromTableItem( "maxRotY", paramIndex, 0.0 ) );
			pVehicles->leftWingFlap.maxRot.Z = static_cast<float>( script::LuaGetNumberFromTableItem( "maxRotZ", paramIndex, 0.0 ) );

			pVehicles->leftWingFlap.rotSpeed.X = static_cast<float>( script::LuaGetNumberFromTableItem( "rotSpeedX", paramIndex, 0.0 ) );
			pVehicles->leftWingFlap.rotSpeed.Y = static_cast<float>( script::LuaGetNumberFromTableItem( "rotSpeedY", paramIndex, 0.0 ) );
			pVehicles->leftWingFlap.rotSpeed.Z = static_cast<float>( script::LuaGetNumberFromTableItem( "rotSpeedZ", paramIndex, 0.0 ) );
		}

		// remove wing_left_flap table
		lua_pop( pState, 1 );

		// grab table
		lua_pushstring( pState, "wing_right_flap" );
		lua_gettable( pState, -2 ); // key

		// new table is top
		if( lua_istable( pState, -1 ) )
		{
			int paramIndex = 3;

			math::Vec3 pos;
			math::Vec3 maxRot;

			pVehicles->rightWingFlap.partResId = static_cast<int>( script::LuaGetNumberFromTableItem( "modelFile", paramIndex, -1.0 ) );

			pVehicles->rightWingFlap.initialPos.X = static_cast<float>( script::LuaGetNumberFromTableItem( "posX", paramIndex, 0.0 ) );
			pVehicles->rightWingFlap.initialPos.Y = static_cast<float>( script::LuaGetNumberFromTableItem( "posY", paramIndex, 0.0 ) );
			pVehicles->rightWingFlap.initialPos.Z = static_cast<float>( script::LuaGetNumberFromTableItem( "posZ", paramIndex, 0.0 ) );

			pVehicles->rightWingFlap.maxRot.X = static_cast<float>( script::LuaGetNumberFromTableItem( "maxRotX", paramIndex, 0.0 ) );
			pVehicles->rightWingFlap.maxRot.Y = static_cast<float>( script::LuaGetNumberFromTableItem( "maxRotY", paramIndex, 0.0 ) );
			pVehicles->rightWingFlap.maxRot.Z = static_cast<float>( script::LuaGetNumberFromTableItem( "maxRotZ", paramIndex, 0.0 ) );

			pVehicles->rightWingFlap.rotSpeed.X = static_cast<float>( script::LuaGetNumberFromTableItem( "rotSpeedX", paramIndex, 0.0 ) );
			pVehicles->rightWingFlap.rotSpeed.Y = static_cast<float>( script::LuaGetNumberFromTableItem( "rotSpeedY", paramIndex, 0.0 ) );
			pVehicles->rightWingFlap.rotSpeed.Z = static_cast<float>( script::LuaGetNumberFromTableItem( "rotSpeedZ", paramIndex, 0.0 ) );
		}

		// remove wing_right_flap table
		lua_pop( pState, 1 );

		// grab table
		lua_pushstring( pState, "muzzleflash_model" );
		lua_gettable( pState, -2 ); // key

		// new table is top
		if( lua_istable( pState, -1 ) )
		{
			int paramIndex = 3;

			math::Vec3 pos;

			pVehicles->muzzleFlash.partResId = static_cast<int>( script::LuaGetNumberFromTableItem( "modelFile", paramIndex, -1.0 ) );

			pVehicles->muzzleFlash.initialPos.X = static_cast<float>( script::LuaGetNumberFromTableItem( "posX", paramIndex, 0.0 ) );
			pVehicles->muzzleFlash.initialPos.Y = static_cast<float>( script::LuaGetNumberFromTableItem( "posY", paramIndex, 0.0 ) );
			pVehicles->muzzleFlash.initialPos.Z = static_cast<float>( script::LuaGetNumberFromTableItem( "posZ", paramIndex, 0.0 ) );
		}

		// remove muzzleflash_model table
		lua_pop( pState, 1 );


		// grab table
		lua_pushstring( pState, "exhaust1" );
		lua_gettable( pState, -2 ); // key

		// new table is top
		if( lua_istable( pState, -1 ) )
		{
			int paramIndex = 3;

			math::Vec3 pos;

			pVehicles->exhaust1.useEffect = static_cast<bool>( script::LuaGetBoolFromTableItem( "useEffect", paramIndex, false ) );
			pVehicles->exhaust1.effectResId = static_cast<int>( script::LuaGetNumberFromTableItem( "effectResId", paramIndex, -1.0 ) );

			pVehicles->exhaust1.initialPos.X = static_cast<float>( script::LuaGetNumberFromTableItem( "posX", paramIndex, 0.0 ) );
			pVehicles->exhaust1.initialPos.Y = static_cast<float>( script::LuaGetNumberFromTableItem( "posY", paramIndex, 0.0 ) );
			pVehicles->exhaust1.initialPos.Z = static_cast<float>( script::LuaGetNumberFromTableItem( "posZ", paramIndex, 0.0 ) );
		}

		// remove exhaust1 table
		lua_pop( pState, 1 );

		// grab table
		lua_pushstring( pState, "exhaust2" );
		lua_gettable( pState, -2 ); // key

		// new table is top
		if( lua_istable( pState, -1 ) )
		{
			int paramIndex = 3;

			math::Vec3 pos;

			pVehicles->exhaust2.useEffect = static_cast<bool>( script::LuaGetBoolFromTableItem( "useEffect", paramIndex, false ) );
			pVehicles->exhaust2.effectResId = static_cast<int>( script::LuaGetNumberFromTableItem( "effectResId", paramIndex, -1.0 ) );

			pVehicles->exhaust2.initialPos.X = static_cast<float>( script::LuaGetNumberFromTableItem( "posX", paramIndex, 0.0 ) );
			pVehicles->exhaust2.initialPos.Y = static_cast<float>( script::LuaGetNumberFromTableItem( "posY", paramIndex, 0.0 ) );
			pVehicles->exhaust2.initialPos.Z = static_cast<float>( script::LuaGetNumberFromTableItem( "posZ", paramIndex, 0.0 ) );
		}

		// remove exhaust2 table
		lua_pop( pState, 1 );

		// grab table
		lua_pushstring( pState, "wing_air_slice1" );
		lua_gettable( pState, -2 ); // key

		// new table is top
		if( lua_istable( pState, -1 ) )
		{
			int paramIndex = 3;

			math::Vec3 pos;

			pVehicles->wingSlice1.useEffect = static_cast<bool>( script::LuaGetBoolFromTableItem( "useEffect", paramIndex, false ) );
			pVehicles->wingSlice1.effectResId = static_cast<int>( script::LuaGetNumberFromTableItem( "effectResId", paramIndex, -1.0 ) );

			pVehicles->wingSlice1.initialPos.X = static_cast<float>( script::LuaGetNumberFromTableItem( "posX", paramIndex, 0.0 ) );
			pVehicles->wingSlice1.initialPos.Y = static_cast<float>( script::LuaGetNumberFromTableItem( "posY", paramIndex, 0.0 ) );
			pVehicles->wingSlice1.initialPos.Z = static_cast<float>( script::LuaGetNumberFromTableItem( "posZ", paramIndex, 0.0 ) );
		}

		// remove wing_slice1 table
		lua_pop( pState, 1 );

		// grab table
		lua_pushstring( pState, "wing_air_slice2" );
		lua_gettable( pState, -2 ); // key

		// new table is top
		if( lua_istable( pState, -1 ) )
		{
			int paramIndex = 3;

			math::Vec3 pos;

			pVehicles->wingSlice2.useEffect = static_cast<bool>( script::LuaGetBoolFromTableItem( "useEffect", paramIndex, false ) );
			pVehicles->wingSlice2.effectResId = static_cast<int>( script::LuaGetNumberFromTableItem( "effectResId", paramIndex, -1.0 ) );

			pVehicles->wingSlice2.initialPos.X = static_cast<float>( script::LuaGetNumberFromTableItem( "posX", paramIndex, 0.0 ) );
			pVehicles->wingSlice2.initialPos.Y = static_cast<float>( script::LuaGetNumberFromTableItem( "posY", paramIndex, 0.0 ) );
			pVehicles->wingSlice2.initialPos.Z = static_cast<float>( script::LuaGetNumberFromTableItem( "posZ", paramIndex, 0.0 ) );
		}

		// remove wing_slice2 table
		lua_pop( pState, 1 );

	}

	return(0);
}
