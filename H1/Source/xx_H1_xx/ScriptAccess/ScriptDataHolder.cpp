
/*===================================================================
	File: ScriptDataHolder.cpp
	Game: AirCadets

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "CollisionBase.h"
#include "Model/ModelCommon.h"
#include "ModelBase.h"

#include "ScriptAccess/ScriptDataHolder.h"

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
ScriptDataHolder::ScriptDataHolder()
{
	std::memset( &m_DevData, 0, sizeof(DevScriptData) );

	std::memset( &m_LevelData, 0, sizeof(LevelScriptData) );

	std::memset( &m_PhysicsSetup, 0, sizeof(PhysicsSetup) );

	std::memset( &m_PlayerData, 0, sizeof(PlayerScriptData) );

	m_LevelPackIndex = 0;
	m_DefaultVehicle = 0;

	m_VehiclePacks.clear();
	m_Vehicles.clear();
	m_LevelPacks.clear();
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
ScriptDataHolder::~ScriptDataHolder()
{
	Release();
}

/////////////////////////////////////////////////////
/// Method: Release
/// Params: None
///
/////////////////////////////////////////////////////
void ScriptDataHolder::Release()
{
	unsigned int i=0;

	for( i=0; i < m_VehiclePacks.size(); ++i )
	{
		if( m_VehiclePacks[i]->pPackVehicleInfo != 0 )
		{
			delete [] m_VehiclePacks[i]->pPackVehicleInfo;
		}

		if( m_VehiclePacks[i]->pVehicleScriptData != 0 )
		{
			delete [] m_VehiclePacks[i]->pVehicleScriptData;
		}

		delete m_VehiclePacks[i];
	}

	for( i=0; i < m_Vehicles.size(); ++i )
	{
		delete m_Vehicles[i];
	}

	for( i=0; i < m_LevelPacks.size(); ++i )
	{
		if( m_LevelPacks[i]->pPackLevelInfo != 0 )
		{
			delete [] m_LevelPacks[i]->pPackLevelInfo;
		}

		delete m_LevelPacks[i];
	}

	m_LevelPackIndex = 0;

	m_VehiclePacks.clear();
	m_Vehicles.clear();
	m_LevelPacks.clear();
}

/////////////////////////////////////////////////////
/// Method: Release
/// Params: None
///
/////////////////////////////////////////////////////
void ScriptDataHolder::ReleaseVehicleList()
{
	unsigned int i=0;

	for( i=0; i < m_Vehicles.size(); ++i )
	{
		delete m_Vehicles[i];
	}

	m_Vehicles.clear();
}
