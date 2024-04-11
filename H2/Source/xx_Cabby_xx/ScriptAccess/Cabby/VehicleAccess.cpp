
/*===================================================================
	File: VehicleAccess.cpp
	Game: Cabby

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

#include "Physics/PhysicsWorld.h"
#include "Level/Level.h"
#include "Player/Player.h"

#include "ScriptAccess/ScriptDataHolder.h"
#include "ScriptAccess/Cabby/VehicleAccess.h"

namespace cabbyScript
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
	cabbyScript::pScriptVehicleData = &dataHolder;

	script::LuaScripting::GetInstance()->RegisterFunction( "AddVehiclePack", ScriptAddVehiclePack );
	script::LuaScripting::GetInstance()->RegisterFunction( "AddVehicle", ScriptAddVehicle );
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

		cabbyScript::pScriptVehicleData->GetVehiclePackList().push_back(pVehiclePack);
	}

	return(0);
}

/////////////////////////////////////////////////////
/// Function: ScriptAddVehicle
/// Params: [in]pState
///
/////////////////////////////////////////////////////
int ScriptAddVehicle( lua_State* pState )
{
	if( lua_istable( pState, 1 ) )
	{
		int i=0;

		ScriptDataHolder::VehicleScriptData* pVehicles = 0;
		pVehicles = new ScriptDataHolder::VehicleScriptData;
		DBG_ASSERT( pVehicles != 0 );

		std::memset( pVehicles, 0, sizeof(ScriptDataHolder::VehicleScriptData) );

		pVehicles->customerCalls = static_cast<int>( script::LuaGetNumberFromTableItem( "customerCalls", 1 ) );

		pVehicles->pBody = reinterpret_cast<b2Body*>( script::LuaGetUserDataFromTableItem( "bodyPtr", 1 ) );
		DBG_ASSERT( pVehicles->pBody != 0 );

		pVehicles->landingGearShapeTable = script::LuaGetStringFromTableItem( "landingGearShapeTable", 1 );
		DBG_ASSERT( pVehicles->landingGearShapeTable != 0 );

		// look up the landing gear shape data
		{
			pVehicles->pLandingGearDef = new b2PolygonDef;
			DBG_ASSERT( pVehicles->pLandingGearDef != 0 );

			if( script::DoesTableItemExist( pVehicles->landingGearShapeTable, "friction", LUA_TNUMBER ) )
				pVehicles->pLandingGearDef->friction	= static_cast<float32>( script::LuaGetNumberFromTableItem( pVehicles->landingGearShapeTable, "friction", 0.2 ) );
			if( script::DoesTableItemExist( pVehicles->landingGearShapeTable, "restitution", LUA_TNUMBER ) )
				pVehicles->pLandingGearDef->restitution	= static_cast<float32>( script::LuaGetNumberFromTableItem( pVehicles->landingGearShapeTable, "restitution", 0.0 ) );
			if( script::DoesTableItemExist( pVehicles->landingGearShapeTable, "density", LUA_TNUMBER ) )
				pVehicles->pLandingGearDef->density		= static_cast<float32>( script::LuaGetNumberFromTableItem( pVehicles->landingGearShapeTable, "density", 0.0 ) );
					
			b2Vec2 center( 0.0f, 0.0f );
			if( script::DoesTableItemExist( pVehicles->landingGearShapeTable, "centerX", LUA_TNUMBER ) )
				center.x			= static_cast<float32>( script::LuaGetNumberFromTableItem( pVehicles->landingGearShapeTable, "centerX" ) );
			if( script::DoesTableItemExist( pVehicles->landingGearShapeTable, "centerY", LUA_TNUMBER ) )
				center.y			= static_cast<float32>( script::LuaGetNumberFromTableItem( pVehicles->landingGearShapeTable, "centerY" ) );

			float32 rotation		= 0.0f;
			if( script::DoesTableItemExist( pVehicles->landingGearShapeTable, "rotation", LUA_TNUMBER ) )
				rotation			= static_cast<float32>( script::LuaGetNumberFromTableItem( pVehicles->landingGearShapeTable, "rotation" ) );
			float32 hx				= 1.0f;
			if( script::DoesTableItemExist( pVehicles->landingGearShapeTable, "hx", LUA_TNUMBER ) )
				hx = static_cast<float32>( script::LuaGetNumberFromTableItem( pVehicles->landingGearShapeTable, "hx" ) );
			float32 hy				= 1.0f;
			if( script::DoesTableItemExist( pVehicles->landingGearShapeTable, "hy", LUA_TNUMBER ) )
				hy = static_cast<float32>( script::LuaGetNumberFromTableItem( pVehicles->landingGearShapeTable, "hy" ) );

			pVehicles->pLandingGearDef->SetAsBox( hx, hy, center, rotation );
		}

		pVehicles->modelIndex = static_cast<int>( script::LuaGetNumberFromTableItem( "modelFile", 1 ) );
		DBG_ASSERT( pVehicles->modelIndex != -1 );

		pVehicles->audioFileVac = static_cast<int>( script::LuaGetNumberFromTableItem( "audioFileVac", 1 ) );
		pVehicles->audioFileAir = static_cast<int>( script::LuaGetNumberFromTableItem( "audioFileAir", 1 ) );
		pVehicles->audioFileSub = static_cast<int>( script::LuaGetNumberFromTableItem( "audioFileSub", 1 ) );

		pVehicles->audioFileVacBrake = static_cast<int>( script::LuaGetNumberFromTableItem( "audioFileVacBrake", 1 ) );
		pVehicles->audioFileAirBrake = static_cast<int>( script::LuaGetNumberFromTableItem( "audioFileAirBrake", 1 ) );
		pVehicles->audioFileSubBrake = static_cast<int>( script::LuaGetNumberFromTableItem( "audioFileSubBrake", 1 ) );

		pVehicles->maxXVelAir = static_cast<float>( script::LuaGetNumberFromTableItem( "maxXVelAir", 1 ) ); 
		pVehicles->maxYVelAir = static_cast<float>( script::LuaGetNumberFromTableItem( "maxYVelAir", 1 ) ); 
		pVehicles->maxFallVelAir = static_cast<float>( script::LuaGetNumberFromTableItem( "maxFallVelAir", 1 ) ); 
		pVehicles->maxXVelVac = static_cast<float>( script::LuaGetNumberFromTableItem( "maxXVelVac", 1 ) ); 
		pVehicles->maxYVelVac = static_cast<float>( script::LuaGetNumberFromTableItem( "maxYVelVac", 1 ) ); 
		pVehicles->maxFallVelVac = static_cast<float>( script::LuaGetNumberFromTableItem( "maxFallVelVac", 1 ) ); 
		pVehicles->maxXVelWater = static_cast<float>( script::LuaGetNumberFromTableItem( "maxXVelWater", 1 ) ); 
		pVehicles->maxYVelWater = static_cast<float>( script::LuaGetNumberFromTableItem( "maxYVelWater", 1 ) ); 
		pVehicles->maxFallVelWater = static_cast<float>( script::LuaGetNumberFromTableItem( "maxFallVelWater", 1 ) ); 

		pVehicles->upwardsForceAir = static_cast<float>( script::LuaGetNumberFromTableItem( "upwardsForceAir", 1 ) ); 
		pVehicles->movementForceAir = static_cast<float>( script::LuaGetNumberFromTableItem( "movementForceAir", 1 ) ); 
		pVehicles->upwardsForceVac = static_cast<float>( script::LuaGetNumberFromTableItem( "upwardsForceVac", 1 ) ); 
		pVehicles->movementForceVac = static_cast<float>( script::LuaGetNumberFromTableItem( "movementForceVac", 1 ) ); 
		pVehicles->upwardsForceWater = static_cast<float>( script::LuaGetNumberFromTableItem( "upwardsForceWater", 1 ) ); 
		pVehicles->movementForceWater = static_cast<float>( script::LuaGetNumberFromTableItem( "movementForceWater", 1 ) ); 

		pVehicles->smallDamage = static_cast<float>( script::LuaGetNumberFromTableItem( "smallDamage", 1 ) ); 
		pVehicles->largeDamage = static_cast<float>( script::LuaGetNumberFromTableItem( "largeDamage", 1 ) ); 

		pVehicles->mainBodyMesh = static_cast<int>(script::LuaGetNumberFromTableItem( "mainBodyMesh", 1 ) );
		pVehicles->landingGearMeshUp = static_cast<int>( script::LuaGetNumberFromTableItem( "landingGearMeshUp", 1 ) );
		pVehicles->landingGearMeshDown = static_cast<int>( script::LuaGetNumberFromTableItem( "landingGearMeshDown", 1 ) );

		pVehicles->airBrakeMesh = static_cast<int>( script::LuaGetNumberFromTableItem( "airBrakeMesh", 1 ) );

		pVehicles->burnersVerticalMesh = static_cast<int>( script::LuaGetNumberFromTableItem( "burnersVerticalMesh", 1 ) );
		pVehicles->burnersVerticalEmitterId = static_cast<unsigned int>( script::LuaGetNumberFromTableItem( "burnersVerticalEmitterId", 1 ) );
		pVehicles->burnersVerticalFrontOffset.X = static_cast<float>( script::LuaGetNumberFromTableItem( "burnersVerticalFrontOffsetX", 1 ) );
		pVehicles->burnersVerticalFrontOffset.Y = static_cast<float>( script::LuaGetNumberFromTableItem( "burnersVerticalFrontOffsetY", 1 ) );
		pVehicles->burnersVerticalBackOffset.X = static_cast<float>( script::LuaGetNumberFromTableItem( "burnersVerticalBackOffsetX", 1 ) );
		pVehicles->burnersVerticalBackOffset.Y = static_cast<float>( script::LuaGetNumberFromTableItem( "burnersVerticalBackOffsetY", 1 ) );

		pVehicles->burnersHorizontalMesh = static_cast<int>( script::LuaGetNumberFromTableItem( "burnersHorizontalMesh", 1 ) );
		pVehicles->burnersHorizontalEmitterId = static_cast<unsigned int>( script::LuaGetNumberFromTableItem( "burnersHorizontalEmitterId", 1 ) );
		pVehicles->brunersHorizontalOffset.X = static_cast<float>( script::LuaGetNumberFromTableItem( "brunersHorizontalOffsetX", 1 ) );
		pVehicles->brunersHorizontalOffset.Y = static_cast<float>( script::LuaGetNumberFromTableItem( "brunersHorizontalOffsetY", 1 ) );

		pVehicles->parcelMesh = static_cast<int>( script::LuaGetNumberFromTableItem( "parcelMesh", 1 ) );
		pVehicles->bombMesh = static_cast<int>( script::LuaGetNumberFromTableItem( "bombMesh", 1 ) );
		pVehicles->keyMesh = static_cast<int>( script::LuaGetNumberFromTableItem( "keyMesh", 1 ) );

		pVehicles->permanentSubMeshCount = 0;
		pVehicles->nightSubMeshCount = 0;
		pVehicles->astroSubMeshCount = 0;
		pVehicles->aeroSubMeshCount = 0;
		pVehicles->aquaSubMeshCount = 0;

		for( i=0; i < MAX_SUBMESH; ++i )
		{
			pVehicles->permanentSubMeshList[i] = -1;
			pVehicles->nightSubMeshList[i] = -1;
			pVehicles->astroSubMeshList[i] = -1;
			pVehicles->aeroSubMeshList[i] = -1;
			pVehicles->aquaSubMeshList[i] = -1;
		}

		pVehicles->hasPermanentMeshes = static_cast<bool>( script::LuaGetBoolFromTableItem( "hasPermanentMeshes", 1 ) );
		if( pVehicles->hasPermanentMeshes )
		{
			{
				// grab number
				lua_pushstring( pState, "permanentSubMeshList" );
				lua_gettable( pState, 1 ); // key

				// new table is top
				if( lua_istable( pState, -1 ) )
				{
					pVehicles->permanentSubMeshCount = luaL_len( pState, -1 );
					DBG_ASSERT( pVehicles->permanentSubMeshCount < MAX_SUBMESH );

					for( i=1; i <= pVehicles->permanentSubMeshCount; ++i )
					{
						lua_rawgeti( pState, -1, i );

						pVehicles->permanentSubMeshList[i-1] = static_cast<int>( lua_tonumber( pState, -1 ) );

						lua_pop( pState, 1 );
					}
				}
				lua_pop( pState, 1 );
			}
		}

		pVehicles->hasNightChanges = static_cast<bool>( script::LuaGetBoolFromTableItem( "hasNightChanges", 1 ) );
		if( pVehicles->hasNightChanges )
		{
			{
				// grab number
				lua_pushstring( pState, "nightSubMeshList" );
				lua_gettable( pState, 1 ); // key

				// new table is top
				if( lua_istable( pState, -1 ) )
				{
					pVehicles->nightSubMeshCount = luaL_len( pState, -1 );
					DBG_ASSERT( pVehicles->nightSubMeshCount < MAX_SUBMESH );

					for( i=1; i <= pVehicles->nightSubMeshCount; ++i )
					{
						lua_rawgeti( pState, -1, i );

						pVehicles->nightSubMeshList[i-1] = static_cast<int>( lua_tonumber( pState, -1 ) );

						lua_pop( pState, 1 );
					}
				}
				lua_pop( pState, 1 );
			}
		}

		pVehicles->hasSubMeshChanges = static_cast<bool>( script::LuaGetBoolFromTableItem( "hasSubMeshChanges", 1 ) );

		if( pVehicles->hasSubMeshChanges )
		{		
			{
				// grab number
				lua_pushstring( pState, "astroSubMeshList" );
				lua_gettable( pState, 1 ); // key

				// new table is top
				if( lua_istable( pState, -1 ) )
				{
					pVehicles->astroSubMeshCount = luaL_len( pState, -1 );
					DBG_ASSERT( pVehicles->astroSubMeshCount < MAX_SUBMESH );

					for( i=1; i <= pVehicles->astroSubMeshCount; ++i )
					{
						lua_rawgeti( pState, -1, i );

						pVehicles->astroSubMeshList[i-1] = static_cast<int>( lua_tonumber( pState, -1 ) );

						lua_pop( pState, 1 );
					}
				}
				lua_pop( pState, 1 );
			}
			{
				// grab number
				lua_pushstring( pState, "aeroSubMeshList" );
				lua_gettable( pState, 1 ); // key

				// new table is top
				if( lua_istable( pState, -1 ) )
				{
					pVehicles->aeroSubMeshCount = luaL_len( pState, -1 );
					DBG_ASSERT( pVehicles->aeroSubMeshCount < MAX_SUBMESH );

					for( i=1; i <= pVehicles->aeroSubMeshCount; ++i )
					{
						lua_rawgeti( pState, -1, i );

						pVehicles->aeroSubMeshList[i-1] = static_cast<int>( lua_tonumber( pState, -1 ) );

						lua_pop( pState, 1 );
					}
				}
				lua_pop( pState, 1 );
			}
			{
				// grab number
				lua_pushstring( pState, "aquaSubMeshList" );
				lua_gettable( pState, 1 ); // key

				// new table is top
				if( lua_istable( pState, -1 ) )
				{
					pVehicles->aquaSubMeshCount = luaL_len( pState, -1 );
					DBG_ASSERT( pVehicles->aquaSubMeshCount < MAX_SUBMESH );

					for( i=1; i <= pVehicles->aquaSubMeshCount; ++i )
					{
						lua_rawgeti( pState, -1, i );

						pVehicles->aquaSubMeshList[i-1] = static_cast<int>( lua_tonumber( pState, -1 ) );

						lua_pop( pState, 1 );
					}
				}
				lua_pop( pState, 1 );
			}
		}

		cabbyScript::pScriptVehicleData->GetVehicleList().push_back(pVehicles);
	}

	return(0);
}
