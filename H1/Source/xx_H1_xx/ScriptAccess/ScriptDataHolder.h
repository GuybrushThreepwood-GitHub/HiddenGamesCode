
/*===================================================================
	File: ScriptDataHolder.h
	Game: AirCadets

	(C)Hidden Games
=====================================================================*/

#ifndef __SCRIPTDATAHOLDER_H__
#define __SCRIPTDATAHOLDER_H__

#include <vector>

#include "Level/LevelTypes.h"
#include "ScriptAccess/H1/H1Access.h"

const int MAX_SUBMESH = 10;

class ScriptDataHolder
{
	public:
		struct DevScriptData
		{
			DevScriptData()
			{
				enablePhysicsDraw = false;
				enableDebugDraw = false;
				enableDebugUIDraw = false;

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

				useVertexArrays = false;
				useAABBCull = false;
				alphaBlendBatchSprites = false;
				alphaBlendValue = 0.5f;
				batchSpriteDrawMode = 0;

				bootState = 0;
				developerSaveFileRoot = 0;
				userSaveFileRoot = 0;
				language = core::LANGUAGE_ENGLISH;

				allowAdvertBarScaling = false;
				appAdFilterId = 0;
				localAdvertXML = 0;
				externalAdvertXML = 0;

				allowDebugCam = false;
				levelTest = false;
				levelTestMode = 0;
				levelTestId = 0;
				levelTestVehicle = 0;
				levelTestName = 0;

				selectionBoxThickness = 1.0f;
				selectionBoxExpand = 0.0f;
				selectionBoxR = 0;
				selectionBoxG = 255;
				selectionBoxB = 0;
			}

			bool enablePhysicsDraw;
			bool enableDebugDraw;
			bool enableDebugUIDraw;
			bool isTablet;
			bool isRetina;
			bool isPCOnly;
			bool istvOS;

			bool frameLock30;
			bool frameLock60;

			int assetBaseWidth;
			int assetBaseHeight;

			int screenWidth;
			int screenHeight;

			bool useVertexArrays;
			bool useAABBCull;
			bool alphaBlendBatchSprites;
			float alphaBlendValue;
			int batchSpriteDrawMode;

			int bootState;
			const char* developerSaveFileRoot;
			const char* userSaveFileRoot;
			int language;
			bool showPCAdBar;

			bool allowAdvertBarScaling;
			const char* appAdFilterId;
			const char* localAdvertXML;
			const char* externalAdvertXML;

			bool allowDebugCam;
			bool levelTest;
			int levelTestMode;
			int levelTestId;
			int levelTestVehicle;
			const char* levelTestName;

			float selectionBoxThickness;
			float selectionBoxExpand;
			int selectionBoxR;
			int selectionBoxG;
			int selectionBoxB;
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

		struct VehicleScriptData
		{
			struct Part
			{
				Part()
				{
					partResId = -1;
					initialPos.setZero();
					currentPos.setZero();
					currentRot.setZero();
					maxRot.setZero();
					rotSpeed.setZero();
				}

				int partResId;
				math::Vec3 initialPos;
				math::Vec3 currentPos;
				math::Vec3 currentRot;
				math::Vec3 maxRot;
				math::Vec3 rotSpeed;
			};

			struct Effect
			{
				Effect()
				{
					useEffect = false;
					effectResId = -1;
					initialPos.setZero();
					currentPos.setZero();
				}

				bool useEffect;
				int effectResId;

				math::Vec3 initialPos;
				math::Vec3 currentPos;
			};

			VehicleScriptData()
			{
				aircraftName = 0;
				physicsData = 0;
				modelIndex = -1;
				modelIndexUI = -1;
				modelFileShadow = -1;

				minSpeed = 65.0f;
				maxSpeed = 300.0f;
				accelerate = 50.0f;
				decelerate = 20.0f;

				rotSpeedX = 10.0;
				rotSpeedY = 10.0;
				rotSpeedZ = 10.0;	
	
				maxRotXAngle = 25.0f;
				maxRotZAngle = 25.0f;

				bulletSpeed = 750.0f;
				maxBulletDistance = 2000.0f;
				bulletRate = 0.15f;
				bulletLife = 2.0f;
			}

			const char* physicsData;

			int aircraftName;
			int modelIndex;
			int modelIndexUI;
			int modelFileShadow;
			int modelMuzzleFlash;

			float minSpeed;
			float maxSpeed;
			float accelerate;
			float decelerate;

			float rotSpeedX;
			float rotSpeedY;
			float rotSpeedZ;	

			float maxRotXAngle;
			float maxRotZAngle;

			float bulletSpeed;
			float maxBulletDistance;
			float bulletRate;
			float bulletLife;
			
			int propellerSpriteMeshId;
			int propellerModelMeshId;

			Part propModel;
			Part rudder;
			Part leftTailFlap;
			Part rightTailFlap;
			Part leftWingFlap;
			Part rightWingFlap;
			Part muzzleFlash;

			Effect exhaust1;
			Effect exhaust2;

			Effect wingSlice1;
			Effect wingSlice2;
		};

		struct VehiclePackData
		{
			VehiclePackData()
			{
				packId = -1;
				packIndex = -1;
				packVehicleCount = -1;

				pPackVehicleInfo = 0;
				pVehicleScriptData = 0;
			}

			int packId;
			int packIndex;
			int packVehicleCount;

			VehicleInfo* pPackVehicleInfo;
			VehicleScriptData* pVehicleScriptData;
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
			const char* levelPhysicsComplex;
			const char* levelData;

			const char* levelDayLight;
			const char* levelAfternoonLight;
			const char* levelNightLight;

			const char* levelFogDay;
			const char* levelFogAfternoon;
			const char* levelFogNight;

			bool allowSnow;
			int snowProbability;

			bool allowRain;
			int rainProbability;

			int detailMap;
			float detailMapScaleX;
			float detailMapScaleY;

			math::Vec3 levelSkyboxOffset;

			bool levelBGDayUseFog;
			int levelBGTextureDay;

			bool levelBGAfternoonUseFog;
			int levelBGTextureAfternoon;
			
			bool levelBGNightUseFog;
			int levelBGTextureNight;

			int objectTargetId1;
			int objectTargetId2;
			int objectTargetId3;

			float levelNearClip;
			float levelFarClip;

			const char* coverage;

			int numUsedSpriteBatches;
			spriteBatchData spriteBatches[MAX_LEVEL_SPRITES];
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

		struct PhysicsSetup
		{
			float gravityX;
			float gravityY;
			float gravityZ;

			float autoDisableLinearThreshold;
			float autoDisableAngularThreshold;
			int autoDisableSteps;
			float autoDisableTime;

			float CFM;
			float linearDamping;
			float angularDamping;
			float contactMaxCorrectingVel;
			float contactSurfaceLayer;
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

		// physics
		void SetPhysicsData( PhysicsSetup& data );
		PhysicsSetup& GetPhysicsData();

		unsigned int GetNextLevelPackIndex();

		void SetDefaultVehicle( int Id );
		int GetDefaultVehicle();

		std::vector<MusicTrackData*>& GetMusicTrackList();

		std::vector<VehiclePackData*>& GetVehiclePackList();
		//std::vector<VehicleScriptData*>& GetVehicleList();
		std::vector<LevelPackData* >& GetLevelPackList();

	private:
		DevScriptData m_DevData;
		GameData m_GameData;

		LevelScriptData m_LevelData;
		PlayerScriptData m_PlayerData;
		PhysicsSetup m_PhysicsSetup;

		// 
		unsigned int m_LevelPackIndex;

		int m_DefaultVehicle;

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

inline void ScriptDataHolder::SetPhysicsData( ScriptDataHolder::PhysicsSetup& data )
{
	m_PhysicsSetup = data;
}

inline ScriptDataHolder::PhysicsSetup& ScriptDataHolder::GetPhysicsData( )
{
	return(m_PhysicsSetup);
}

inline void ScriptDataHolder::SetPlayerData( ScriptDataHolder::PlayerScriptData& data )
{
	m_PlayerData = data;
}

inline ScriptDataHolder::PlayerScriptData& ScriptDataHolder::GetPlayerData( )
{
	return(m_PlayerData);
}

inline std::vector<ScriptDataHolder::VehiclePackData*>& ScriptDataHolder::GetVehiclePackList()
{
	return(m_VehiclePacks);
}

//inline std::vector<ScriptDataHolder::VehicleScriptData*>& ScriptDataHolder::GetVehicleList()
//{
//	return(m_Vehicles);
//}

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
