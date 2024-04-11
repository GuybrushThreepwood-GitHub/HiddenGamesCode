
/*===================================================================
	File: ScriptDataHolder.h
	Game: H4

	(C)Hidden Games
=====================================================================*/

#ifndef __SCRIPTDATAHOLDER_H__
#define __SCRIPTDATAHOLDER_H__

#include <vector>

#include "ScriptAccess/H4/H4Access.h"

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
				enableSave = true;

				isTablet = false;
				isRetina = false;

				frameLock30 = false;
				frameLock60 = true;

				disableMusic = true;

				enableFPSMode = false;
				enableNoise = false;

				assetBaseWidth = 320;
				assetBaseHeight = 480;

				screenWidth = 320;
				screenHeight = 480;

				levelMipmap = true;
				smoothFiltering = false;

				bootState = 0;
				hiResMode = false;
			}

			bool enablePhysicsDraw;
			bool enableDebugDraw;
			bool enableDebugUIDraw;
			bool enableSave;

			bool isTablet;
			bool isRetina;

			bool frameLock30;
			bool frameLock60;

			bool disableMusic;

			bool enableFPSMode;
			bool enableNoise;

			bool levelMipmap;
			bool smoothFiltering;

			int assetBaseWidth;
			int assetBaseHeight;

			int screenWidth;
			int screenHeight;

			int bootState;
			bool hiResMode;
			bool useVertexArrays;

		};

		struct MusicTrackData
		{
			MusicTrackData()
			{
				musicFile = 0;
				musicName = 0;
				musicVolume = 1.0f;
			}

			const char* musicFile;
			const char* musicName;
			float musicVolume;
		};
		
		struct CameraSetup
		{
			float cam_posx_offset;
			float cam_posy_offset;
			float cam_posz_offset;

			float cam_targetx_offset;
			float cam_targety_offset;
			float cam_targetz_offset;
		};

		struct StageSetup
		{
			StageSetup()
			{
				stageName = 0;
				stageFile = 0;
				stageSetupFunc = 0;
			}

			const char* stageName;
			const char* stageFile;
			const char* stageSetupFunc;
		};

		struct StageDefinition
		{
			StageDefinition()
			{
				stageName = 0;
				stageSetupFunc = 0;

				gameLayer = 0;
				physicsFileB2D = 0;
				navMeshFile = 0;
				typesFile = 0;
				roomSetupFunc = 0;

				skyboxIndex = -1;

				clearColour = math::Vec3(0.0f,0.0f,0.0f);

				magFilter = GL_NEAREST;
				minFilter = GL_NEAREST_MIPMAP_NEAREST;

				fogState = false;
				fogMode = GL_LINEAR;
				fogNearClip = 1000.0f;
				fogFarClip = 1001.0f;
				fogDensity = 0.1f;
				fogColour = math::Vec3(0.0f,0.0f,0.0f);
				fogSetClearToMatch = false;

				nearClip = 1.0f;
				farClip = 100.0f;

				minEnemies = 0;
				maxEnemies = 1;

				minFloatingHeads = 0;
				maxFloatingHeads = 0;

				minHangingSpiders = 0;
				maxHangingSpiders = 0;

				minAntSpawn = 0;
				maxAntSpawn = 0;

				maxSpawnEnemies = 0;

				roomMaxEmpty = 0;
				roomMaxAmmo = 0;
				roomMaxHealth = 0;
				roomMaxZombie = 0;

				dreamState = false;
				allowGibs = true;
				allowNoise = false;
				enableFlashlight = false;
			}

			const char* stageName;
			const char* stageSetupFunc;

			const char* gameLayer;
			const char* physicsFileB2D;
			const char* navMeshFile;
			const char* typesFile;
	
			const char* roomSetupFunc;

			int skyboxIndex;

			math::Vec3 clearColour;

			GLenum magFilter;
			GLenum minFilter;

			bool fogState;
			GLenum fogMode;
			float fogNearClip;
			float fogFarClip;
			float fogDensity;
			math::Vec3 fogColour;
			bool fogSetClearToMatch;
		
			float nearClip;
			float farClip;	

			int minEnemies;
			int maxEnemies;

			int minFloatingHeads;
			int maxFloatingHeads;

			int minHangingSpiders;
			int maxHangingSpiders;

			int minAntSpawn;
			int maxAntSpawn;

			int maxSpawnEnemies;

			unsigned int roomMaxEmpty;
			unsigned int roomMaxAmmo;
			unsigned int roomMaxHealth;
			unsigned int roomMaxZombie;

			bool dreamState;
			bool allowGibs;
			bool allowNoise;
			bool enableFlashlight;
		};

	public:
		ScriptDataHolder();
		~ScriptDataHolder();
		
		void Release();

		// dev data
		void SetDevData( DevScriptData& data );
		DevScriptData& GetDevData();

		// game data
		void SetGameData( GameData& data );
		GameData& GetGameData();

		// camera
		void SetCameraData( CameraSetup& data );
		CameraSetup& GetCameraData();

		// stage
		void SetActiveStageData( StageDefinition& data );
		StageDefinition& GetActiveStageData();

		std::vector<StageSetup* >& GetStageSetupsList();

	private:
		// stage chunks
		std::vector<StageSetup*> m_StageSetups;

		DevScriptData m_DevData;
		GameData m_GameData;

		CameraSetup m_CameraSetup;

		// current active stage
		StageDefinition m_ActiveStage;
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

inline void ScriptDataHolder::SetCameraData( ScriptDataHolder::CameraSetup& data )
{
	m_CameraSetup = data;
}

inline ScriptDataHolder::CameraSetup& ScriptDataHolder::GetCameraData( )
{
	return(m_CameraSetup);
}

inline void ScriptDataHolder::SetActiveStageData( ScriptDataHolder::StageDefinition& data )
{
	m_ActiveStage = data;
}

inline ScriptDataHolder::StageDefinition& ScriptDataHolder::GetActiveStageData( )
{
	return(m_ActiveStage);
}

inline std::vector<ScriptDataHolder::StageSetup*>& ScriptDataHolder::GetStageSetupsList()
{
	return(m_StageSetups);
}

#endif // __SCRIPTDATAHOLDER_H__
