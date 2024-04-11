
/*===================================================================
	File: ProfileManager.h
	Game: H8

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
			unsigned long long int personalBest;
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


	private:
		void CreateProfiles();
		
	private:
		static ProfileManager* ms_Instance;

		char m_ProfileName[core::MAX_PATH+core::MAX_PATH];

		file::TFileHandle m_FileHandle;
		Profile m_ProfileData;
		
		GameData m_GameData;

};

#endif // __PROFILEMANAGER_H__

