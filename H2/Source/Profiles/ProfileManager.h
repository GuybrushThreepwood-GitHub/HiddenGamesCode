
/*===================================================================
	File: ProfileManager.h
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#ifndef __PROFILEMANAGER_H__
#define __PROFILEMANAGER_H__

#include "ScriptAccess/ScriptAccess.h"

const int MAX_PURCHASE_ID_NAME = 256;

class ProfileManager
{
	public:
		struct CareerLevels
		{
			bool complete;
			int packIndex;
			int levelIndex;
		};

		struct Profile
		{
			// active career
			bool careerActive;
			long currentMoney;
			float currentFuel;
			short currentLives;
			short currentLevelIndex;
			short currentPerfectFares;
			short currentCustomersLost;
			short currentLivesLost;
			short vehicleId;
			
			CareerLevels careerLevels[LEVELS_IN_A_CAREER];

			// options
            int controlType;
			bool tiltControl;
			bool sfxState;
			bool bgmState;

			// vehicle pack 0
			bool yellowCabUnlocked;
			bool blackCabUnlocked;
			bool towTruckUnlocked;
			bool deliveryVanUnlocked;
			bool hotRodUnlocked;

			bool bondLotusUnlocked;
			bool aTeamVanUnlocked;
			bool ghoulHunterUnlocked;
			bool spinnerUnlocked;
			bool deloreanUnlocked;

			// vehicle pack 1
			bool Pack1vehicle1;
			bool Pack1vehicle2;
			bool Pack1vehicle3;
			bool Pack1vehicle4;
			bool Pack1vehicle5;

			// vehicle pack 2
			bool Pack2vehicle1;
			bool Pack2vehicle2;
			bool Pack2vehicle3;
			bool Pack2vehicle4;
			bool Pack2vehicle5;

			// vehicle pack 3
			bool Pack3vehicle1;
			bool Pack3vehicle2;
			bool Pack3vehicle3;
			bool Pack3vehicle4;
			bool Pack3vehicle5;

			bool towTruckShouldUnlock;

			// level pack 1
			/*bool pack1Level1;
			bool pack1Level2;
			bool pack1Level3;
			bool pack1Level4;
			bool pack1Level5;

			// level pack 2
			bool pack2Level1;
			bool pack2Level2;
			bool pack2Level3;
			bool pack2Level4;
			bool pack2Level5;

			// level pack 3
			bool pack3Level1;
			bool pack3Level2;
			bool pack3Level3;
			bool pack3Level4;
			bool pack3Level5;

			// level pack 4
			bool pack4Level1;
			bool pack4Level2;
			bool pack4Level3;
			bool pack4Level4;
			bool pack4Level5;

			// level pack 5
			bool pack5Level1;
			bool pack5Level2;
			bool pack5Level3;
			bool pack5Level4;
			bool pack5Level5;*/

			// overall career stats
			bool newComplete;
			bool careerGameOver;
			int numberOfCompletes;

			int extraLifeTotal;

			// running career stats
			short totalCustomersTransported;
			short totalCustomersLost;
			short totalParcelsTransported;
			short totalParcelsLost;
			short perfectFares;
			long totalMoneyEarned;
			long fuelPurchased;
			short taxisLost;
		};

		struct ArcadeBest
		{
			long highestMoney;
			//short vehicleId;
			//float timeToComplete;
			int maxStars;
		};

		struct ActiveLevelData
		{
			int levelArcadeCustomers;
			int levelArcadeFuel;
			int levelArcadeMoney;

			int customersTransported;
			int customersLost;

			int parcelsTransported;
			int parcelsLost;

			int perfectFares;

			int moneyEarned;
			int costOfRepairs;

			int totalMoneyEarned;
			short taxisLost;

			int fuelPurchased;

			float timeToComplete;

			bool starCollected;
			bool customerCountStar;
			bool moneyStar;
		};

		enum eGameMode
		{
			CAREER_MODE=0,
			ARCADE_MODE
		};

		struct PurchaseId
		{
			int purchaseId;
			char purchaseName[MAX_PURCHASE_ID_NAME];
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

		void SetGameMode( eGameMode gameMode )		{ m_GameMode = gameMode; }
		eGameMode GetGameMode()						{ return m_GameMode; }

		ArcadeBest* GetArcadeBestData( int index )	{ DBG_ASSERT( (index >=0 && index <=m_GameData.TOTAL_LEVELS-1) ); return &m_ArcadeBestData[index]; }

		void ClearActiveLevelData();
		ActiveLevelData* GetActiveLevelData()		{ return &m_ActiveLevelData; }

		void SetArcadeModeVehicle( int id )			{ DBG_ASSERT( (id >=0 && id <=m_GameData.TOTAL_VEHICLES-1) ); m_ArcadeModeVehicle = id; }
		int GetArcadeModeVehicle()					{ return m_ArcadeModeVehicle; }

		void SetArcadeModeLevel( int packId, int levelId )			
		{ 
			DBG_ASSERT( (packId >=0 && packId <=m_GameData.TOTAL_LEVEL_PACKS-1) ); 
			DBG_ASSERT( (levelId >=0 && levelId <=m_GameData.MAX_ITEMS_PER_PACK-1) );

			m_ArcadeModePackId = packId;
			m_ArcadeModeLevelId = levelId;
		}
		int GetArcadeModePackId()					{ return m_ArcadeModePackId; }
		int GetArcadeModeLevelId()					{ return m_ArcadeModeLevelId; }

		void AddVehicleToUnlocks( int vehicleId ) 
		{
			if( !m_VehicleUnlockIds.empty() )
			{
				for( unsigned int i=0; i < m_VehicleUnlockIds.size(); ++i )
				{
					// already added
					if( m_VehicleUnlockIds[i] == vehicleId )
					{
						return;
					}
				}
			}
			m_TotalVehicleUnlocks++;
			m_VehicleUnlockIds.push_back(vehicleId);
		}
		bool GetVehicleUnlockState()				{ return (m_TotalVehicleUnlocks>0); }
		int GetVehicleUnlockId()					
		{ 
			if( m_VehicleUnlockIds.size() > 0 )
				return (m_VehicleUnlockIds[m_VehicleUnlockIds.size()-1]); 

			return -1;
		}
		void RemoveVehicleFromUnlocks()				
		{
			m_TotalVehicleUnlocks--;
			m_VehicleUnlockIds.pop_back();
		}

		void AddToPurchases( const char* purchaseName )
		{
			if( !m_PurchaseList.empty() )
			{
				for( unsigned int i=0; i < m_PurchaseList.size(); ++i )
				{
					// already added
					if( strcmp( m_PurchaseList[i].purchaseName, purchaseName ) == 0 )
					{
						return;
					}
				}
			}

			// add it
			PurchaseId newId;
			snprintf( newId.purchaseName, MAX_PURCHASE_ID_NAME, "%s", purchaseName );

			m_PurchaseList.push_back(newId);
		}

		bool HasBeenPurchased( const char* purchaseName )
		{
			if( !m_PurchaseList.empty() )
			{
				for( unsigned int i=0; i < m_PurchaseList.size(); ++i )
				{
					// exists
					if( strcmp( m_PurchaseList[i].purchaseName, purchaseName ) == 0 )
					{
						return true;
					}
				}
			}

			return false;
		}

	private:
		void CreateProfiles();
		
	private:
		static ProfileManager* ms_Instance;

		char m_ProfileName[core::MAX_PATH+core::MAX_PATH];

		file::TFileHandle m_FileHandle;
		Profile m_ProfileData;
		ArcadeBest* m_ArcadeBestData;
		GameData m_GameData;

		ActiveLevelData m_ActiveLevelData;

		eGameMode m_GameMode;
		int m_ActiveProfile;

		int m_ArcadeModeVehicle;
		int m_ArcadeModePackId;
		int m_ArcadeModeLevelId;

		int m_TotalVehicleUnlocks;
		std::vector<int> m_VehicleUnlockIds;

		std::vector<PurchaseId> m_PurchaseList;

		//bool m_VehicleUnlockState;
		//int m_VehicleUnlockId;
};

#endif // __PROFILEMANAGER_H__

