
/*===================================================================
	File: ScriptDataHolder.cpp
	Game: Cabby

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

	std::memset( &m_PlayerData, 0, sizeof(PlayerScriptData) );

	std::memset( &m_AirSettings, 0, sizeof(AtmosphereScriptData) );
	std::memset( &m_VacuumSettings, 0, sizeof(AtmosphereScriptData) );
	std::memset( &m_WaterSettings, 0, sizeof(AtmosphereScriptData) );

	m_LevelPackIndex = 0;
	m_DefaultVehicle = 0;

	m_MusicTrackData.clear();

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

	for( i=0; i < m_MusicTrackData.size(); ++i )
	{
		delete m_MusicTrackData[i];
	}

	for( i=0; i < m_VehiclePacks.size(); ++i )
	{
		if( m_VehiclePacks[i]->pPackVehicleInfo != 0 )
		{
			delete [] m_VehiclePacks[i]->pPackVehicleInfo;
		}

		delete m_VehiclePacks[i];
	}

	for( i=0; i < m_Vehicles.size(); ++i )
	{
		if( m_Vehicles[i]->pLandingGearDef != 0 )
			delete m_Vehicles[i]->pLandingGearDef;

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
		if( m_Vehicles[i]->pLandingGearDef != 0 )
			delete m_Vehicles[i]->pLandingGearDef;

		delete m_Vehicles[i];
	}

	m_Vehicles.clear();
}
