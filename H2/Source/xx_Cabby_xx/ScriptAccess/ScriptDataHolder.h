
/*===================================================================
	File: ScriptDataHolder.h
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#ifndef __SCRIPTDATAHOLDER_H__
#define __SCRIPTDATAHOLDER_H__

#include "Box2D.h"
#include "Level/Level.h"

#include "ScriptAccess/Cabby/CabbyAccess.h"

const int MAX_SUBMESH = 10;


class ScriptDataHolder
{
	public:
		struct DevScriptData
		{
			DevScriptData()
			{
				enableDeveloperMenu = false;
				enablePhysicsDraw = false;
				enableDebugDraw = false;
				enableDebugUIDraw = false;
				enableSave = true;
				isTablet = false;
				isRetina = false;
				isPCOnly = false;
				istvOS = false;
				frameLock30 = false;
				frameLock60 = true;

				assetBaseWidth = 320;
				assetBaseHeight = 480;

				screenWidth = 480;
				screenHeight = 320;

				selectionBoxThickness = 1.0f;
				selectionBoxExpand = 0.0f;
				selectionBoxR = 0;
				selectionBoxG = 255;
				selectionBoxB = 0;

				levelMipmap = true;
				smoothFiltering = false;

				useVertexArrays = false;
				bootState = 0;
				developerSaveFileRoot = 0;
				userSaveFileRoot = 0;
				language = core::LANGUAGE_ENGLISH;
				showPCAdBar = false;

				allowAdvertBarScaling = false;
				appAdFilterId = 0;
				localAdvertXML = 0;
				externalAdvertXML = 0;

				disableMeshDraw = false;
				allowDebugCam = false;
				singleHitKill = false;
				levelTest = false;
				levelTestVehicle = 0;
				levelTestName = 0;
				hiresCustomers = false;
			}

			bool enableDeveloperMenu;
			bool enablePhysicsDraw;
			bool enableDebugDraw;
			bool enableDebugUIDraw;
			bool enableSave;
			bool isTablet;
			bool isRetina;
			bool isPCOnly;
			bool istvOS;
			bool frameLock30;
			bool frameLock60;

			float selectionBoxThickness;
			float selectionBoxExpand;
			int selectionBoxR;
			int selectionBoxG;
			int selectionBoxB;

			bool levelMipmap;
			bool smoothFiltering;

			int assetBaseWidth;
			int assetBaseHeight;

			int screenWidth;
			int screenHeight;

			bool useVertexArrays;
			int bootState;
			const char* developerSaveFileRoot;
			const char* userSaveFileRoot;
			int language;
			bool showPCAdBar;

			bool allowAdvertBarScaling;
			const char* appAdFilterId;
			const char* localAdvertXML;
			const char* externalAdvertXML;

			bool disableMeshDraw;
			bool allowDebugCam;
			bool singleHitKill;
			bool levelTest;
			int levelTestVehicle;
			const char* levelTestName;
			bool hiresCustomers;
		};

		struct MusicTrackData
		{
			const char* musicFile;
			const char* musicName;
			float musicVolume;
		};

		struct PlayerScriptData
		{
			bool smoothRotation;
			bool conformToNormal;
		};

		struct VehicleInfo
		{
			const char* vehicleScriptName;
			const char* vehicleName;
			const char* setupFunction;
			bool liteVersionPurchase;
			int purchaseId;			
		};

		struct VehiclePackData
		{
			VehiclePackData()
			{
				packId = -1;
				packIndex = -1;
				packVehicleCount = -1;

				pPackVehicleInfo = 0;
			}

			int packId;
			int packIndex;
			int packVehicleCount;

			VehicleInfo* pPackVehicleInfo;
		};

		struct VehicleScriptData
		{
			VehicleScriptData()
			{
				customerCalls = 0;
				pBody = 0;
				modelIndex = -1;

				audioFileVac = 0;
				audioFileAir = 0;
				audioFileSub = 0;

				audioFileVacBrake = 0;
				audioFileAirBrake = 0;
				audioFileSubBrake = 0;

				maxXVelAir = 12.0f;
				maxYVelAir = 6.0f;
				maxFallVelAir = -8.0f;

				maxXVelVac = 10.0f;
				maxYVelVac = 10.0f;
				maxFallVelVac = -15.0f;
	
				maxXVelWater = 5.0f;
				maxYVelWater = 5.0f;
				maxFallVelWater = -15.0f;

				upwardsForceAir = 50.0f;
				movementForceAir = 30.0f;
				upwardsForceVac = 50.0f;
				movementForceVac = 30.0f;
				upwardsForceWater = 60.0f;
				movementForceWater = 40.0f;

				smallDamage = 10.0f;
				largeDamage = 20.0f;

				mainBodyMesh = -1;
	
				landingGearMeshUp = -1;
				landingGearMeshDown = -1;

				airBrakeMesh = -1;
				burnersVerticalMesh = -1;	
				burnersVerticalEmitterId = 0;
				burnersVerticalFrontOffset = math::Vec2(0.0f,0.0f);
				burnersVerticalBackOffset = math::Vec2(0.0f,0.0f);

				burnersHorizontalMesh = -1;
				burnersHorizontalEmitterId = 0;
				brunersHorizontalOffset = math::Vec2(0.0f,0.0f);

				parcelMesh = -1;
				bombMesh = -1;
				keyMesh = -1;

				permanentSubMeshCount = 0;
				nightSubMeshCount = 0;
				astroSubMeshCount = 0;
				aeroSubMeshCount = 0;
				aquaSubMeshCount = 0;

				for( int i=0; i < MAX_SUBMESH; ++i )
				{
					permanentSubMeshList[i] = -1;
					nightSubMeshList[i] = -1;
					astroSubMeshList[i] = -1;
					aeroSubMeshList[i] = -1;
					aquaSubMeshList[i] = -1;
				}
			}

			int customerCalls;

			b2Body* pBody;
			const char* landingGearShapeTable;
			b2PolygonDef* pLandingGearDef;

			int modelIndex;

			int audioFileVac;
			int audioFileAir;
			int audioFileSub;

			int audioFileVacBrake;
			int audioFileAirBrake;
			int audioFileSubBrake;

			float maxXVelAir;
			float maxYVelAir;
			float maxFallVelAir;

			float maxXVelVac;
			float maxYVelVac;
			float maxFallVelVac;
	
			float maxXVelWater;
			float maxYVelWater;
			float maxFallVelWater;

			float upwardsForceAir;
			float movementForceAir;
			float upwardsForceVac;
			float movementForceVac;
			float upwardsForceWater;
			float movementForceWater;

			float maxXVel;
			float maxYVel;

			float smallDamage;
			float largeDamage;

			int mainBodyMesh;
	
			int landingGearMeshUp;
			int landingGearMeshDown;

			int airBrakeMesh;

			int burnersVerticalMesh;	
			unsigned int burnersVerticalEmitterId;
			math::Vec2 burnersVerticalFrontOffset;
			math::Vec2 burnersVerticalBackOffset;

			int burnersHorizontalMesh;
			unsigned int burnersHorizontalEmitterId;
			math::Vec2 brunersHorizontalOffset;

			int parcelMesh;
			int bombMesh;
			int keyMesh;

			bool hasPermanentMeshes;
			int permanentSubMeshCount;
			int permanentSubMeshList[MAX_SUBMESH];

			bool hasNightChanges;
			int nightSubMeshCount;
			int nightSubMeshList[MAX_SUBMESH];

			bool hasSubMeshChanges;
			int astroSubMeshCount;
			int astroSubMeshList[MAX_SUBMESH];

			int aeroSubMeshCount;
			int aeroSubMeshList[MAX_SUBMESH];

			int aquaSubMeshCount;
			int aquaSubMeshList[MAX_SUBMESH];
		};

		struct LevelLightBlock
		{
			bool useLight;
			bool updateLight;
			bool showVehicleLight;
			math::Vec4 lightPos;
			math::Vec4 lightDiffuse;
			math::Vec4 lightAmbient;
			math::Vec4 lightSpecular;
			math::Vec3 lightAttenuation;
		};

		struct LevelScriptData
		{
			const char* levelName;

			int levelModel;
			int levelBackground;
			int levelForeground;
			const char* levelPhysicsComplex;
			const char* levelData;

			const char* levelDayLight;
			const char* levelAfternoonLight;
			const char* levelNightLight;

			const char* levelFogDay;
			const char* levelFogAfternoon;
			const char* levelFogNight;

			math::Vec3 levelBGPos;
			math::Vec2 levelBGDims;

			bool levelBGDayUseFog;
			int levelBGTextureDay;

			bool levelBGAfternoonUseFog;
			int levelBGTextureAfternoon;
			
			bool levelBGNightUseFog;
			int levelBGTextureNight;

			float levelNearClip;
			float levelFarClip;

			int levelCustomerTotal;
			int levelCustomerMaxSpawn;

			float levelArcadeTime;
			int levelArcadeCustomers;
			int levelArcadeFuel;
			int levelArcadeMoney;

			float levelCameraOffsetX;
			float levelCameraOffsetY;
			float levelCameraZ;

			float levelCamCenterY;
			float levelCamCenterZ;

			float levelCloseZoom;
			float levelCloseZoomSpeed;

			int detailMap;
			float detailMapScaleX;
			float detailMapScaleY;
		};

		struct LevelInfo
		{
			const char* levelScriptName;
			const char* levelLoadFuncName;
			const char* levelName;
			bool liteVersionPurchase;
			int liteVersionPurchaseId;
		};

		struct LevelPackData
		{
			int packId;
			int packIndex;
			int packLevelCount;

			LevelInfo* pPackLevelInfo;
		};

		struct AtmosphereScriptData
		{
			b2Vec2 gravity;
		};

	public:
		ScriptDataHolder();
		~ScriptDataHolder();
		
		void Release();
		void ReleaseVehicleList();

		// development
		void SetDevData( DevScriptData& data );
		DevScriptData& GetDevData();

		// game data
		void SetGameData( GameData& data );
		GameData& GetGameData();

		// player
		void SetPlayerData( PlayerScriptData& data );
		PlayerScriptData& GetPlayerData();

		// level
		void SetLevelData( LevelScriptData& data );
		LevelScriptData& GetLevelData();

		// settings
		void SetAirSettingsData( AtmosphereScriptData& data );
		AtmosphereScriptData& GetAirSettingsData();

		void SetVacuumSettingsData( AtmosphereScriptData& data );
		AtmosphereScriptData& GetVacuumSettingsData();

		void SetWaterSettingsData( AtmosphereScriptData& data );
		AtmosphereScriptData& GetWaterSettingsData();

		unsigned int GetNextLevelPackIndex();

		void SetDefaultVehicle( int Id );
		int GetDefaultVehicle();

		std::vector<MusicTrackData*>& GetMusicTrackList();

		std::vector<VehiclePackData*>& GetVehiclePackList();
		std::vector<VehicleScriptData*>& GetVehicleList();
		std::vector<LevelPackData* >& GetLevelPackList();

	private:
		DevScriptData m_DevData;
		GameData m_GameData;

		LevelScriptData m_LevelData;
		PlayerScriptData m_PlayerData;

		AtmosphereScriptData m_AirSettings;
		AtmosphereScriptData m_VacuumSettings;
		AtmosphereScriptData m_WaterSettings;

		// 
		unsigned int m_LevelPackIndex;

		int m_DefaultVehicle;

		// music tracks
		std::vector<MusicTrackData*> m_MusicTrackData;

		// vehicle base list
		std::vector<VehiclePackData*> m_VehiclePacks;

		// vehicle list
		std::vector<VehicleScriptData*> m_Vehicles;

		// level packs
		std::vector<LevelPackData*> m_LevelPacks;
};

inline void ScriptDataHolder::SetDevData( ScriptDataHolder::DevScriptData& data )
{
	m_DevData = data;
}

inline ScriptDataHolder::DevScriptData& ScriptDataHolder::GetDevData( )
{
	return(m_DevData);
}

inline void ScriptDataHolder::SetGameData( GameData& data )
{
	m_GameData = data;
}

inline GameData& ScriptDataHolder::GetGameData( )
{
	return(m_GameData);
}

inline void ScriptDataHolder::SetLevelData( ScriptDataHolder::LevelScriptData& data )
{
	m_LevelData = data;
}

inline ScriptDataHolder::LevelScriptData& ScriptDataHolder::GetLevelData( )
{
	return(m_LevelData);
}

inline void ScriptDataHolder::SetPlayerData( ScriptDataHolder::PlayerScriptData& data )
{
	m_PlayerData = data;
}

inline ScriptDataHolder::PlayerScriptData& ScriptDataHolder::GetPlayerData( )
{
	return(m_PlayerData);
}

inline void ScriptDataHolder::SetAirSettingsData( ScriptDataHolder::AtmosphereScriptData& data )
{
	m_AirSettings = data;
}

inline ScriptDataHolder::AtmosphereScriptData& ScriptDataHolder::GetAirSettingsData( )
{
	return(m_AirSettings);
}

inline void ScriptDataHolder::SetVacuumSettingsData( ScriptDataHolder::AtmosphereScriptData& data )
{
	m_VacuumSettings = data;
}

inline ScriptDataHolder::AtmosphereScriptData& ScriptDataHolder::GetVacuumSettingsData( )
{
	return(m_VacuumSettings);
}

inline void ScriptDataHolder::SetWaterSettingsData( ScriptDataHolder::AtmosphereScriptData& data )
{
	m_WaterSettings = data;
}

inline ScriptDataHolder::AtmosphereScriptData& ScriptDataHolder::GetWaterSettingsData( )
{
	return(m_WaterSettings);
}

inline std::vector<ScriptDataHolder::MusicTrackData*>& ScriptDataHolder::GetMusicTrackList()
{
	return(m_MusicTrackData);
}

inline std::vector<ScriptDataHolder::VehiclePackData*>& ScriptDataHolder::GetVehiclePackList()
{
	return(m_VehiclePacks);
}

inline std::vector<ScriptDataHolder::VehicleScriptData*>& ScriptDataHolder::GetVehicleList()
{
	return(m_Vehicles);
}

inline void ScriptDataHolder::SetDefaultVehicle( int Id )
{
	m_DefaultVehicle = Id;
}
		
inline int ScriptDataHolder::GetDefaultVehicle()
{
	return(m_DefaultVehicle);
}

inline std::vector<ScriptDataHolder::LevelPackData*>& ScriptDataHolder::GetLevelPackList()
{
	return(m_LevelPacks);
}

#endif // __SCRIPTDATAHOLDER_H__
