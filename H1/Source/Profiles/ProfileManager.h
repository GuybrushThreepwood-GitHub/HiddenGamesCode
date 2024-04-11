
/*===================================================================
	File: ProfileManager.h
	Game: AirCadets

	(C)Hidden Games
=====================================================================*/

#ifndef __PROFILEMANAGER_H__
#define __PROFILEMANAGER_H__

#include "ScriptAccess/ScriptAccess.h"

const int MAX_PURCHASE_ID_NAME = 256;

class ProfileManager
{
	public:
		struct Profile
		{
			// options
			bool sfxState;

			bool adsRemoved = true;

			float flightTime;
		};

		struct LevelBest
		{
			int packId;
			int levelId;

			int mode;
			int layout;

			int targetsDestroyed;
			int targetsTotal;

			float bestTime;
		};

		struct ActiveLevelData
		{
			int packId;
			int levelId;

			int mode;
			int layout;

			int targetsDestroyed;
			int targetsTotal;
			float time;
		};

	public:
		ProfileManager();
		~ProfileManager();

		static void Initialise( void );
		static void Shutdown( void );

		static ProfileManager *GetInstance( void ) 
		{
			DBG_ASSERT( (ms_Instance != 0) );

			return( ms_Instance );
		}
		static bool IsInitialised( void ) 
		{
			return( ms_Instance != 0 );
		}	
	
		void Setup();

		void LoadProfile( const char* profileData );
		void LoadProfileFromScript( const char* profileData, lua_State* pState );

		void SaveProfile();
		void SaveProfileToScript();

		void ClearProfile( bool completeClear );

		Profile* GetProfile();

		void ClearActiveLevelData();
		ActiveLevelData* GetActiveLevelData()		{ return &m_ActiveLevelData; }

		void SetModeVehicle( int id )				{ DBG_ASSERT( (id >=0 && id <= m_GameData.TOTAL_VEHICLES) ); m_ModeVehicle = id; }
		int GetModeVehicle()						{ return m_ModeVehicle; }

		void SetModeLevel( int packId, int levelId )			
		{ 
			DBG_ASSERT( (packId >=0 && packId <=m_GameData.TOTAL_LEVEL_PACKS-1) ); 
			DBG_ASSERT( (levelId >=0 && levelId <=m_GameData.MAX_ITEMS_PER_PACK-1) );

			m_ModePackId = packId;
			m_ModeLevelId = levelId;
		}
		int GetModePackId()						{ return m_ModePackId; }
		int GetModeLevelId()					{ return m_ModeLevelId; }

		void AddScore( bool update, int packId, int levelId, int mode, int layout, int targetsDestroyed, int targetsTotal, float time );
		bool GetScore( int packId, int levelId, int mode, int layout, LevelBest* pScore );


	private:
		void CreateProfiles();
		
	private:
		static ProfileManager* ms_Instance;

		char m_ProfileName[core::MAX_PATH+core::MAX_PATH];

		file::TFileHandle m_FileHandle;
		Profile m_ProfileData;
		
		GameData m_GameData;

		ActiveLevelData m_ActiveLevelData;

		int m_ActiveProfile;

		int m_ModeVehicle;
		int m_ModePackId;
		int m_ModeLevelId;

		std::vector<LevelBest> m_BestData;

};

#endif // __PROFILEMANAGER_H__

