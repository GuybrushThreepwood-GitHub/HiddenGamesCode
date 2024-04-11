
/*===================================================================
	File: ScriptDataHolder.h
	Game: H8

	(C)Hidden Games
=====================================================================*/

#ifndef __SCRIPTDATAHOLDER_H__
#define __SCRIPTDATAHOLDER_H__

#include <vector>

#include "ScriptAccess/H8/H8Access.h"

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

				selectionBoxThickness = 1.0f;
				selectionBoxExpand = 0.0f;
				selectionBoxR = 0;
				selectionBoxG = 255;
				selectionBoxB = 0;

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

				allowDebugCam = false;
				showEnemyLines = false;
				shieldSetup = 0;
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

			float selectionBoxThickness;
			float selectionBoxExpand;
			int selectionBoxR;
			int selectionBoxG;
			int selectionBoxB;

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

			bool allowDebugCam;
			bool showEnemyLines;
			const char* shieldSetup;
		};

		struct MusicTrackData
		{
			const char* musicFile;
			const char* musicName;
			float musicVolume;
		};

	public:
		ScriptDataHolder();
		~ScriptDataHolder();
		
		void Release();

		// development
		void SetDevData( DevScriptData& data );
		DevScriptData& GetDevData();

		// game data
		void SetGameData( GameData& data );
		GameData& GetGameData();

	private:
		DevScriptData m_DevData;
		GameData m_GameData;
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

#endif // __SCRIPTDATAHOLDER_H__
