
/*===================================================================
	File: ProfileManager.cpp
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include <sstream>
#include <iomanip>
#include <string>

#include "MathBase.h"
#include "RenderBase.h"
#include "SoundBase.h"
#include "InputBase.h"
#include "ScriptBase.h"
#include "CollisionBase.h"
#include "Model/ModelCommon.h"
#include "ModelBase.h"

// app includes
#include "CabbyConsts.h"

#include "Profiles/ProfileManager.h"

namespace
{
	const int VERSION_ID = 2; // first release Profile_1_0.sav

	const int MAX_STRING_BUFFER = 8192;

	void FormatToString( std::string& addTo, const char *fmt, ... )
	{
		char textString[MAX_STRING_BUFFER];

		// -- empty the buffer properly to ensure no leaks.
		memset( textString, '\0', sizeof(char)*MAX_STRING_BUFFER );

		va_list args;
		va_start ( args, fmt );
		vsnprintf ( textString, MAX_STRING_BUFFER, fmt, args );
		va_end ( args );
		addTo += textString;
	}
}

ProfileManager* ProfileManager::ms_Instance = 0;

/////////////////////////////////////////////////////
/// Static Method: Initialise
/// Params: None
///
/////////////////////////////////////////////////////
void ProfileManager::Initialise( void )
{
	DBG_ASSERT( (ms_Instance == 0) );

	DBG_MEMTRY
		ms_Instance = new ProfileManager;
	DBG_MEMCATCH
}

/////////////////////////////////////////////////////
/// Static Method: Shutdown
/// Params: None
///
/////////////////////////////////////////////////////
void ProfileManager::Shutdown( void )
{
	if( ms_Instance )
	{
		delete ms_Instance;
		ms_Instance = 0;
	}
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
ProfileManager::ProfileManager()
{
	m_ArcadeBestData = 0;

	std::memset( m_ProfileName, 0, sizeof(char)*core::MAX_PATH+core::MAX_PATH );

	std::memset( &m_ProfileData, 0, sizeof(Profile) );

	std::memset( &m_ActiveLevelData, 0, sizeof(ActiveLevelData) );

	m_GameMode = CAREER_MODE;
	m_ActiveProfile = -1;

	m_ArcadeModeVehicle = 0;
	m_ArcadeModePackId = 0;
	m_ArcadeModeLevelId = 0;

	m_TotalVehicleUnlocks = 0;
	m_VehicleUnlockIds.clear();

	m_PurchaseList.clear();

	DBGLOG( "Profile struct size = %lu-bytes\n", sizeof(Profile) );
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
ProfileManager::~ProfileManager()
{
	if( m_ArcadeBestData != 0 )
	{
		delete[] m_ArcadeBestData;
		m_ArcadeBestData = 0;
	}
}

/////////////////////////////////////////////////////
/// Method: CreateProfiles
/// Params: None
///
/////////////////////////////////////////////////////
void ProfileManager::CreateProfiles()
{
	DBG_ASSERT( m_ProfileName != 0 );

	ClearProfile(true);

	SaveProfileToScript();
}

/////////////////////////////////////////////////////
/// Method: ClearProfile
/// Params: [in]slot
///
/////////////////////////////////////////////////////
void ProfileManager::ClearProfile( bool completeClear )
{
	if( completeClear )
	{
		int i=0;
		std::memset( &m_ProfileData, 0, sizeof(Profile) );

		for( i=0; i < m_GameData.TOTAL_LEVELS; ++i )
		{
			m_ArcadeBestData[i].highestMoney = 0;
			m_ArcadeBestData[i].maxStars = 0;
		}

		m_ProfileData.yellowCabUnlocked		= true;
		m_ProfileData.blackCabUnlocked		= true;
		m_ProfileData.towTruckUnlocked		= false;
		m_ProfileData.deliveryVanUnlocked	= false;
		m_ProfileData.hotRodUnlocked		= false;

		m_ProfileData.bondLotusUnlocked		= false;
		m_ProfileData.aTeamVanUnlocked		= false;
		m_ProfileData.ghoulHunterUnlocked	= false;
		m_ProfileData.spinnerUnlocked		= false;
		m_ProfileData.deloreanUnlocked		= false;

			// vehicle pack 1
		m_ProfileData.Pack1vehicle1		= false;
		m_ProfileData.Pack1vehicle2		= false;
		m_ProfileData.Pack1vehicle3		= false;
		m_ProfileData.Pack1vehicle4		= false;
		m_ProfileData.Pack1vehicle5		= false;

			// vehicle pack 2
		m_ProfileData.Pack2vehicle1		= false;
		m_ProfileData.Pack2vehicle2		= false;
		m_ProfileData.Pack2vehicle3		= false;
		m_ProfileData.Pack2vehicle4		= false;
		m_ProfileData.Pack2vehicle5		= false;

			// vehicle pack 3
		m_ProfileData.Pack3vehicle1		= false;
		m_ProfileData.Pack3vehicle2		= false;
		m_ProfileData.Pack3vehicle3		= false;
		m_ProfileData.Pack3vehicle4		= false;
		m_ProfileData.Pack3vehicle5		= false;

		m_ProfileData.towTruckShouldUnlock	= false;
		
        m_ProfileData.controlType = 0;
		m_ProfileData.tiltControl = false;
		m_ProfileData.sfxState = true;
		m_ProfileData.bgmState = true;
	}

	// default data
	m_GameData = GetScriptDataHolder()->GetGameData();

	m_ProfileData.careerActive		= false;
	m_ProfileData.currentFuel		= 100.0f;
	m_ProfileData.currentLives		= m_GameData.DEFAULT_LIVES_COUNT;
	m_ProfileData.currentMoney		= 0;
	m_ProfileData.currentLevelIndex	= 0;
	m_ProfileData.currentPerfectFares = 0;
	m_ProfileData.currentCustomersLost = 0;
	m_ProfileData.currentLivesLost	= 0;
	m_ProfileData.vehicleId			= 0;

	// vehicle unlocks
	//m_ProfileData.yellowCabUnlocked		= true;
	//m_ProfileData.blackCabUnlocked		= true;
	//m_ProfileData.towTruckUnlocked		= false;
	//m_ProfileData.deliveryVanUnlocked		= false;
	//m_ProfileData.hotRodUnlocked			= false;

	//m_ProfileData.bondLotusUnlocked		= false;
	//m_ProfileData.aTeamVanUnlocked		= false;
	//m_ProfileData.ghoulHunterUnlocked		= false;
	//m_ProfileData.spinnerUnlocked			= false;
	//m_ProfileData.deloreanUnlocked		= false;
	
	//m_ProfileData.Pack1vehicle1		= false;
	//m_ProfileData.Pack1vehicle2		= false;
	//m_ProfileData.Pack1vehicle3		= false;
	//m_ProfileData.Pack1vehicle4		= false;
	//m_ProfileData.Pack1vehicle5		= false;
	
	//m_ProfileData.towTruckShouldUnlock	= false;

	// all levels are always unlocked
	/*m_ProfileData.pack1Level1 = true;
	m_ProfileData.pack1Level2 = true;
	m_ProfileData.pack1Level3 = true;
	m_ProfileData.pack1Level4 = true;
	m_ProfileData.pack1Level5 = true;

	m_ProfileData.pack2Level1 = true;
	m_ProfileData.pack2Level2 = true;
	m_ProfileData.pack2Level3 = true;
	m_ProfileData.pack2Level4 = true;
	m_ProfileData.pack2Level5 = true;

	m_ProfileData.pack3Level1 = true;
	m_ProfileData.pack3Level2 = true;
	m_ProfileData.pack3Level3 = true;
	m_ProfileData.pack3Level4 = true;
	m_ProfileData.pack3Level5 = true;

	m_ProfileData.pack4Level1 = true;
	m_ProfileData.pack4Level2 = true;
	m_ProfileData.pack4Level3 = true;
	m_ProfileData.pack4Level4 = true;
	m_ProfileData.pack4Level5 = true;

	m_ProfileData.pack5Level1 = true;
	m_ProfileData.pack5Level2 = true;
	m_ProfileData.pack5Level3 = true;
	m_ProfileData.pack5Level4 = true;
	m_ProfileData.pack5Level5 = true;*/

	// career reset
	m_ProfileData.newComplete				= false;
	m_ProfileData.careerGameOver			= false;
	//m_ProfileData.numberOfCompletes		= 0;
	m_ProfileData.extraLifeTotal			= 0;

	//m_ProfileData.totalCustomersTransported = 0;
	//m_ProfileData.totalCustomersLost		= 0;
	//m_ProfileData.totalParcelsTransported	= 0;
	//m_ProfileData.totalParcelsLost		= 0;
	//m_ProfileData.taxisLost				= 0;		
	//m_ProfileData.perfectFares			= 0;
	//m_ProfileData.totalMoneyEarned		= 0;
	//m_ProfileData.fuelPurchased			= 0;
}

/////////////////////////////////////////////////////
/// Method: Setup
/// Params: None
///
/////////////////////////////////////////////////////
void ProfileManager::Setup()
{
	int i=0; 
	m_GameData = GetScriptDataHolder()->GetGameData();

	m_ArcadeBestData = new ArcadeBest[m_GameData.TOTAL_LEVELS];
	
	for( i=0; i < m_GameData.TOTAL_LEVELS; ++i )
	{
		m_ArcadeBestData[i].highestMoney = 0;
		m_ArcadeBestData[i].maxStars = 0;
	}
}

/////////////////////////////////////////////////////
/// Method: LoadProfile
/// Params: [in]profileData
///
/////////////////////////////////////////////////////
void ProfileManager::LoadProfile( const char* profileData )
{
	DBG_ASSERT( profileData != 0 );

	// copy filename
	snprintf( m_ProfileName, core::MAX_PATH+core::MAX_PATH, "%s", profileData );

	// if the file doesn't exist, create it
	if( !file::FileExists( m_ProfileName ) )
		CreateProfiles();

	LoadProfileFromScript( m_ProfileName, 0 );
}

/////////////////////////////////////////////////////
/// Method: LoadProfileFromScript
/// Params: None
///
/////////////////////////////////////////////////////
void ProfileManager::LoadProfileFromScript( const char* profileData, lua_State* pState )
{
	bool canRead = false;
	int tableIndex = 1;
	int i=0;

	if( pState == 0 )
	{
		if( core::IsEmptyString( profileData ) )
			return;

		// reading from proper saved file
		pState = script::LuaScripting::GetState();

		if( !script::LoadScript( profileData ) )
		{
			canRead = true;
			lua_getglobal( pState, "profile" );
			tableIndex = -1;
		}
		else
			return;
	}
	else
	{
		// going to set the data manually
		canRead = true;
		tableIndex = 1;
	}

	if( canRead )
	{
		if( lua_istable( pState, tableIndex ) )
		{
			m_ProfileData.careerActive		= static_cast<bool>( script::LuaGetNumberFromTableItem( "careerActive", 1, 0.0 ) != 0 );
			m_ProfileData.currentMoney		= static_cast<long>( script::LuaGetNumberFromTableItem( "currentMoney", 1, 0.0 ) ); 
			m_ProfileData.currentFuel		= static_cast<float>( script::LuaGetNumberFromTableItem( "currentFuel", 1, 100.0 ) ); 
			m_ProfileData.currentLives		= static_cast<short>( script::LuaGetNumberFromTableItem( "currentLives", 1, m_GameData.DEFAULT_LIVES_COUNT ) );
			m_ProfileData.currentLevelIndex	= static_cast<short>( script::LuaGetNumberFromTableItem( "currentLevelIndex", 1, 0.0 ) );

			m_ProfileData.currentPerfectFares	= static_cast<short>( script::LuaGetNumberFromTableItem( "currentPerfectFares", 1, 0.0 ) );
			m_ProfileData.currentCustomersLost	= static_cast<short>( script::LuaGetNumberFromTableItem( "currentCustomersLost", 1, 0.0 ) );
			m_ProfileData.currentLivesLost	= static_cast<short>( script::LuaGetNumberFromTableItem( "currentLivesLost", 1, 0.0 ) );
			m_ProfileData.vehicleId			= static_cast<short>( script::LuaGetNumberFromTableItem( "vehicleId", 1, 0.0 ) );

			if( m_ProfileData.vehicleId < 0 || 
				m_ProfileData.vehicleId >= m_GameData.TOTAL_VEHICLES )
				m_ProfileData.vehicleId = 0;

			// grab what levels were generated for this career
			lua_pushstring( script::LuaScripting::GetState(), "careerLevels" );
			lua_gettable( script::LuaScripting::GetState(), -2 );

			if( lua_istable( script::LuaScripting::GetState(), -1 ) )
			{
				int n = luaL_len( script::LuaScripting::GetState(), -1 );
				
				// go through all the tables in this table
				for( i = 1; i <= n; ++i )
				{
					lua_rawgeti( script::LuaScripting::GetState(), -1, i );
					if( lua_istable( script::LuaScripting::GetState(), -1 ) )
					{
						int paramIndex = 1;
						
						if( i <= LEVELS_IN_A_CAREER ) // lua needs +1 for array index
						{
							lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
							m_ProfileData.careerLevels[i-1].complete = static_cast<bool>( lua_tonumber( script::LuaScripting::GetState(), -1 ) != 0 ) ;
							lua_pop( script::LuaScripting::GetState(), 1 );

							lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
							m_ProfileData.careerLevels[i-1].packIndex = static_cast<int>( lua_tonumber( script::LuaScripting::GetState(), -1 ) );
							lua_pop( script::LuaScripting::GetState(), 1 );

							lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
							m_ProfileData.careerLevels[i-1].levelIndex = static_cast<int>( lua_tonumber( script::LuaScripting::GetState(), -1 ) );
							lua_pop( script::LuaScripting::GetState(), 1 );

							if( m_ProfileData.careerLevels[i-1].packIndex < 0 ||
								m_ProfileData.careerLevels[i-1].packIndex >= m_GameData.TOTAL_LEVEL_PACKS )
								m_ProfileData.careerLevels[i-1].packIndex = 0;

							if( m_ProfileData.careerLevels[i-1].levelIndex < 0 ||
								m_ProfileData.careerLevels[i-1].levelIndex >= m_GameData.MAX_ITEMS_PER_PACK )
								m_ProfileData.careerLevels[i-1].levelIndex = 0;
						}
					}
					lua_pop( script::LuaScripting::GetState(), 1 );
				}
			}
			lua_pop( script::LuaScripting::GetState(), 1 );

			// options
            m_ProfileData.controlType = static_cast<int>( script::LuaGetNumberFromTableItem( "controlType", 1, 0.0 )  );

			m_ProfileData.tiltControl	= static_cast<bool>( script::LuaGetNumberFromTableItem( "tiltControl", 1, 0.0 ) != 0 );
			m_ProfileData.sfxState		= static_cast<bool>( script::LuaGetNumberFromTableItem( "sfxState", 1, 1.0 ) != 0 );
			m_ProfileData.bgmState		= static_cast<bool>( script::LuaGetNumberFromTableItem( "bgmState", 1, 1.0 ) != 0 );	

			// vehicle pack 1
			m_ProfileData.yellowCabUnlocked		= static_cast<bool>( script::LuaGetNumberFromTableItem( "yellowCabUnlocked", 1, 1.0 ) != 0 );	
			m_ProfileData.blackCabUnlocked		= static_cast<bool>( script::LuaGetNumberFromTableItem( "blackCabUnlocked", 1, 1.0 ) != 0 );	
			m_ProfileData.towTruckUnlocked		= static_cast<bool>( script::LuaGetNumberFromTableItem( "towTruckUnlocked", 1, 0.0 ) != 0 );	
			m_ProfileData.deliveryVanUnlocked	= static_cast<bool>( script::LuaGetNumberFromTableItem( "deliveryVanUnlocked", 1, 0.0 ) != 0 );	
			m_ProfileData.hotRodUnlocked		= static_cast<bool>( script::LuaGetNumberFromTableItem( "hotRodUnlocked", 1, 0.0 ) != 0 );	

			// vehicle pack 2
			m_ProfileData.bondLotusUnlocked		= static_cast<bool>( script::LuaGetNumberFromTableItem( "bondLotusUnlocked", 1, 0.0 ) != 0 );
			m_ProfileData.aTeamVanUnlocked		= static_cast<bool>( script::LuaGetNumberFromTableItem( "aTeamVanUnlocked", 1, 0.0 ) != 0 );	
			m_ProfileData.ghoulHunterUnlocked	= static_cast<bool>( script::LuaGetNumberFromTableItem( "ghoulHunterUnlocked", 1, 0.0 ) != 0 );	
			m_ProfileData.spinnerUnlocked		= static_cast<bool>( script::LuaGetNumberFromTableItem( "spinnerUnlocked", 1, 0.0 ) != 0 );	
			m_ProfileData.deloreanUnlocked		= static_cast<bool>( script::LuaGetNumberFromTableItem( "deloreanUnlocked", 1, 0.0 ) != 0 );	

			// vehicle pack 1
			m_ProfileData.Pack1vehicle1		= static_cast<bool>( script::LuaGetNumberFromTableItem( "Pack1vehicle1", 1, 0.0 ) != 0 );
			m_ProfileData.Pack1vehicle2		= static_cast<bool>( script::LuaGetNumberFromTableItem( "Pack1vehicle2", 1, 0.0 ) != 0 );
			m_ProfileData.Pack1vehicle3		= static_cast<bool>( script::LuaGetNumberFromTableItem( "Pack1vehicle3", 1, 0.0 ) != 0 );
			m_ProfileData.Pack1vehicle4		= static_cast<bool>( script::LuaGetNumberFromTableItem( "Pack1vehicle4", 1, 0.0 ) != 0 );
			m_ProfileData.Pack1vehicle5		= static_cast<bool>( script::LuaGetNumberFromTableItem( "Pack1vehicle5", 1, 0.0 ) != 0 );

			// vehicle pack 2
			m_ProfileData.Pack2vehicle1		= static_cast<bool>( script::LuaGetNumberFromTableItem( "Pack2vehicle1", 1, 0.0 ) != 0 );
			m_ProfileData.Pack2vehicle2		= static_cast<bool>( script::LuaGetNumberFromTableItem( "Pack2vehicle2", 1, 0.0 ) != 0 );
			m_ProfileData.Pack2vehicle3		= static_cast<bool>( script::LuaGetNumberFromTableItem( "Pack2vehicle3", 1, 0.0 ) != 0 );
			m_ProfileData.Pack2vehicle4		= static_cast<bool>( script::LuaGetNumberFromTableItem( "Pack2vehicle4", 1, 0.0 ) != 0 );
			m_ProfileData.Pack2vehicle5		= static_cast<bool>( script::LuaGetNumberFromTableItem( "Pack2vehicle5", 1, 0.0 ) != 0 );

			// vehicle pack 3
			m_ProfileData.Pack3vehicle1		= static_cast<bool>( script::LuaGetNumberFromTableItem( "Pack3vehicle1", 1, 0.0 ) != 0 );
			m_ProfileData.Pack3vehicle2		= static_cast<bool>( script::LuaGetNumberFromTableItem( "Pack3vehicle2", 1, 0.0 ) != 0 );
			m_ProfileData.Pack3vehicle3		= static_cast<bool>( script::LuaGetNumberFromTableItem( "Pack3vehicle3", 1, 0.0 ) != 0 );
			m_ProfileData.Pack3vehicle4		= static_cast<bool>( script::LuaGetNumberFromTableItem( "Pack3vehicle4", 1, 0.0 ) != 0 );
			m_ProfileData.Pack3vehicle5		= static_cast<bool>( script::LuaGetNumberFromTableItem( "Pack3vehicle5", 1, 0.0 ) != 0 );

			m_ProfileData.towTruckShouldUnlock	= static_cast<bool>( script::LuaGetNumberFromTableItem( "towTruckShouldUnlock", 1, 0.0 ) != 0 );	

			// level pack 1
			/*m_ProfileData.pack1Level1		= static_cast<bool>( script::LuaGetNumberFromTableItem( "pack1Level1", 1, 1.0 ) != 0 );	
			m_ProfileData.pack1Level2		= static_cast<bool>( script::LuaGetNumberFromTableItem( "pack1Level2", 1, 1.0 ) != 0 );	
			m_ProfileData.pack1Level3		= static_cast<bool>( script::LuaGetNumberFromTableItem( "pack1Level3", 1, 1.0 ) != 0 );	
			m_ProfileData.pack1Level4		= static_cast<bool>( script::LuaGetNumberFromTableItem( "pack1Level4", 1, 1.0 ) != 0 );	
			m_ProfileData.pack1Level5		= static_cast<bool>( script::LuaGetNumberFromTableItem( "pack1Level5", 1, 1.0 ) != 0 );	

			// level pack 2
			m_ProfileData.pack2Level1		= static_cast<bool>( script::LuaGetNumberFromTableItem( "pack2Level1", 1, 1.0 ) != 0 );	
			m_ProfileData.pack2Level2		= static_cast<bool>( script::LuaGetNumberFromTableItem( "pack2Level2", 1, 1.0 ) != 0 );	
			m_ProfileData.pack2Level3		= static_cast<bool>( script::LuaGetNumberFromTableItem( "pack2Level3", 1, 1.0 ) != 0 );	
			m_ProfileData.pack2Level4		= static_cast<bool>( script::LuaGetNumberFromTableItem( "pack2Level4", 1, 1.0 ) != 0 );	
			m_ProfileData.pack2Level5		= static_cast<bool>( script::LuaGetNumberFromTableItem( "pack2Level5", 1, 1.0 ) != 0 );	

			// level pack 3
			m_ProfileData.pack3Level1		= static_cast<bool>( script::LuaGetNumberFromTableItem( "pack3Level1", 1, 1.0 ) != 0 );
			m_ProfileData.pack3Level2		= static_cast<bool>( script::LuaGetNumberFromTableItem( "pack3Level2", 1, 1.0 ) != 0 );
			m_ProfileData.pack3Level3		= static_cast<bool>( script::LuaGetNumberFromTableItem( "pack3Level3", 1, 1.0 ) != 0 );
			m_ProfileData.pack3Level4		= static_cast<bool>( script::LuaGetNumberFromTableItem( "pack3Level4", 1, 1.0 ) != 0 );
			m_ProfileData.pack3Level5		= static_cast<bool>( script::LuaGetNumberFromTableItem( "pack3Level5", 1, 1.0 ) != 0 );

			// level pack 4
			m_ProfileData.pack4Level1		= static_cast<bool>( script::LuaGetNumberFromTableItem( "pack4Level1", 1, 1.0 ) != 0 );
			m_ProfileData.pack4Level2		= static_cast<bool>( script::LuaGetNumberFromTableItem( "pack4Level2", 1, 1.0 ) != 0 );
			m_ProfileData.pack4Level3		= static_cast<bool>( script::LuaGetNumberFromTableItem( "pack4Level3", 1, 1.0 ) != 0 );
			m_ProfileData.pack4Level4		= static_cast<bool>( script::LuaGetNumberFromTableItem( "pack4Level4", 1, 1.0 ) != 0 );
			m_ProfileData.pack4Level5		= static_cast<bool>( script::LuaGetNumberFromTableItem( "pack4Level5", 1, 1.0 ) != 0 );

			// level pack 5
			m_ProfileData.pack5Level1		= static_cast<bool>( script::LuaGetNumberFromTableItem( "pack5Level1", 1, 1.0 ) != 0 );
			m_ProfileData.pack5Level2		= static_cast<bool>( script::LuaGetNumberFromTableItem( "pack5Level2", 1, 1.0 ) != 0 );
			m_ProfileData.pack5Level3		= static_cast<bool>( script::LuaGetNumberFromTableItem( "pack5Level3", 1, 1.0 ) != 0 );
			m_ProfileData.pack5Level4		= static_cast<bool>( script::LuaGetNumberFromTableItem( "pack5Level4", 1, 1.0 ) != 0 );
			m_ProfileData.pack5Level5		= static_cast<bool>( script::LuaGetNumberFromTableItem( "pack5Level5", 1, 1.0 ) != 0 );*/

			// overall career stats
			m_ProfileData.newComplete	= static_cast<bool>( script::LuaGetNumberFromTableItem( "newComplete", 1, 0.0 ) != 0 );
			m_ProfileData.careerGameOver = static_cast<bool>( script::LuaGetNumberFromTableItem( "careerGameOver", 1, 0.0 ) != 0 );
			m_ProfileData.numberOfCompletes	= static_cast<int>( script::LuaGetNumberFromTableItem( "numberOfCompletes", 1, 0.0 ) );
			m_ProfileData.extraLifeTotal = static_cast<int>( script::LuaGetNumberFromTableItem( "extraLifeTotal", 1, 0.0 ) );

			m_ProfileData.totalCustomersTransported	= static_cast<short>( script::LuaGetNumberFromTableItem( "totalCustomersTransported", 1, 0.0 ) );
			m_ProfileData.totalCustomersLost = static_cast<short>( script::LuaGetNumberFromTableItem( "totalCustomersLost", 1, 0.0 ) );
			m_ProfileData.totalParcelsTransported	= static_cast<short>( script::LuaGetNumberFromTableItem( "totalParcelsTransported", 1, 0.0 ) );
			m_ProfileData.totalParcelsLost	= static_cast<short>( script::LuaGetNumberFromTableItem( "totalParcelsLost", 1, 0.0 ) );
			m_ProfileData.perfectFares	= static_cast<short>( script::LuaGetNumberFromTableItem( "perfectFares", 1, 0.0 ) );
			m_ProfileData.totalMoneyEarned	= static_cast<long>( script::LuaGetNumberFromTableItem( "totalMoneyEarned", 1, 0.0 ) );
			m_ProfileData.fuelPurchased	= static_cast<long>( script::LuaGetNumberFromTableItem( "fuelPurchased", 1, 0.0 ) );	
			m_ProfileData.taxisLost	= static_cast<short>( script::LuaGetNumberFromTableItem( "taxisLost", 1, 0.0 ) );	

			// read the best star scores for the levels
			lua_pushstring( script::LuaScripting::GetState(), "best" );
			lua_gettable( script::LuaScripting::GetState(), -2 );

			if( lua_istable( script::LuaScripting::GetState(), -1 ) )
			{
				int n = luaL_len( script::LuaScripting::GetState(), -1 );
				
				// go through all the tables in this table
				for( i = 1; i <= n; ++i )
				{
					lua_rawgeti( script::LuaScripting::GetState(), -1, i );
					if( lua_istable( script::LuaScripting::GetState(), -1 ) )
					{
						int paramIndex = 1;
						
						if( i <= m_GameData.TOTAL_LEVELS ) // lua needs +1 for array index
						{
							lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
							m_ArcadeBestData[i-1].highestMoney = static_cast<int>( lua_tonumber( script::LuaScripting::GetState(), -1 ) != 0 ) ;
							lua_pop( script::LuaScripting::GetState(), 1 );

							lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
							m_ArcadeBestData[i-1].maxStars = static_cast<int>( lua_tonumber( script::LuaScripting::GetState(), -1 ) );
							lua_pop( script::LuaScripting::GetState(), 1 );
						}
					}
					lua_pop( script::LuaScripting::GetState(), 1 );
				}
			}
			lua_pop( script::LuaScripting::GetState(), 1 );

#if defined(CABBY_LITE) || defined(CABBY_FREE)
			// IAP list
			lua_pushstring( script::LuaScripting::GetState(), "iap" );
			lua_gettable( script::LuaScripting::GetState(), -2 );

			if( lua_istable( script::LuaScripting::GetState(), -1 ) )
			{
				int n = luaL_len( script::LuaScripting::GetState(), -1 );
				
				// go through all the tables in this table
				for( i = 1; i <= n; ++i )
				{
					lua_rawgeti( script::LuaScripting::GetState(), -1, i );
					if( lua_istable( script::LuaScripting::GetState(), -1 ) )
					{
						int paramIndex = 1;
						
						lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
						const char* purchaseName = lua_tostring( script::LuaScripting::GetState(), -1 );
						lua_pop( script::LuaScripting::GetState(), 1 );

						if( purchaseName != 0 )
							AddToPurchases( purchaseName );
					}
					lua_pop( script::LuaScripting::GetState(), 1 );
				}
			}
			lua_pop( script::LuaScripting::GetState(), 1 );
#endif // defined(CABBY_LITE) || defined(CABBY_FREE)

		}

		if( tableIndex == -1 )
			lua_pop( pState, 1 );
	}
}

/////////////////////////////////////////////////////
/// Method: SaveProfile
/// Params: None
///
/////////////////////////////////////////////////////
void ProfileManager::SaveProfile()
{
	SaveProfileToScript();
}

/////////////////////////////////////////////////////
/// Method: SaveProfileToScript
/// Params: None
///
/////////////////////////////////////////////////////
void ProfileManager::SaveProfileToScript()
{
	if( !file::FileOpen( m_ProfileName, file::FILETYPE_TEXT_WRITE, &m_FileHandle ) )
	{
		DBGLOG( "PROFILEMANAGER: *ERROR* could not open save file %s\n", m_ProfileName );
		return;
		//DBG_ASSERT(0);
	}
	else
	{
		int i=0;
		std::string fileToWrite;

		FormatToString( fileToWrite, "profile = {\n" );
			// current career
			FormatToString( fileToWrite, "careerActive = %d;\n", m_ProfileData.careerActive );
			FormatToString( fileToWrite, "currentMoney = %ld;\n", m_ProfileData.currentMoney );
			FormatToString( fileToWrite, "currentFuel = %.2f;\n", m_ProfileData.currentFuel );
			FormatToString( fileToWrite, "currentLives = %d;\n", m_ProfileData.currentLives );
			FormatToString( fileToWrite, "currentLevelIndex = %d;\n", m_ProfileData.currentLevelIndex );
			FormatToString( fileToWrite, "currentPerfectFares = %d;\n", m_ProfileData.currentPerfectFares );
			FormatToString( fileToWrite, "currentCustomersLost = %d;\n", m_ProfileData.currentCustomersLost );
			FormatToString( fileToWrite, "currentLivesLost = %d;\n", m_ProfileData.currentLivesLost );
			FormatToString( fileToWrite, "vehicleId = %d;\n", m_ProfileData.vehicleId );

			FormatToString( fileToWrite, "careerLevels = {\n" );
			for( i=0; i < LEVELS_IN_A_CAREER; ++i )
			{
				if(i==0)
					FormatToString( fileToWrite, "{ %d, %d, %d }\n", m_ProfileData.careerLevels[i].complete, m_ProfileData.careerLevels[i].packIndex, m_ProfileData.careerLevels[i].levelIndex );
				else
					FormatToString( fileToWrite, ",{ %d, %d, %d }\n", m_ProfileData.careerLevels[i].complete, m_ProfileData.careerLevels[i].packIndex, m_ProfileData.careerLevels[i].levelIndex );
			}
			FormatToString( fileToWrite, "};\n" );

			// options
            FormatToString( fileToWrite, "controlType = %d;\n", m_ProfileData.controlType );
			FormatToString( fileToWrite, "tiltControl = %d;\n", m_ProfileData.tiltControl );
			FormatToString( fileToWrite, "sfxState = %d;\n", m_ProfileData.sfxState );
			FormatToString( fileToWrite, "bgmState = %d;\n", m_ProfileData.bgmState );

			// vehicles
			FormatToString( fileToWrite, "yellowCabUnlocked = %d;\n", m_ProfileData.yellowCabUnlocked );
			FormatToString( fileToWrite, "blackCabUnlocked = %d;\n", m_ProfileData.blackCabUnlocked );
			FormatToString( fileToWrite, "towTruckUnlocked = %d;\n", m_ProfileData.towTruckUnlocked );
			FormatToString( fileToWrite, "deliveryVanUnlocked = %d;\n", m_ProfileData.deliveryVanUnlocked );
			FormatToString( fileToWrite, "hotRodUnlocked = %d;\n", m_ProfileData.hotRodUnlocked );

			FormatToString( fileToWrite, "bondLotusUnlocked = %d;\n", m_ProfileData.bondLotusUnlocked );
			FormatToString( fileToWrite, "aTeamVanUnlocked = %d;\n", m_ProfileData.aTeamVanUnlocked );
			FormatToString( fileToWrite, "ghoulHunterUnlocked = %d;\n", m_ProfileData.ghoulHunterUnlocked );
			FormatToString( fileToWrite, "spinnerUnlocked = %d;\n", m_ProfileData.spinnerUnlocked );
			FormatToString( fileToWrite, "deloreanUnlocked = %d;\n", m_ProfileData.deloreanUnlocked );

			FormatToString( fileToWrite, "Pack1vehicle1 = %d;\n", m_ProfileData.Pack1vehicle1 );
			FormatToString( fileToWrite, "Pack1vehicle2 = %d;\n", m_ProfileData.Pack1vehicle2 );
			FormatToString( fileToWrite, "Pack1vehicle3 = %d;\n", m_ProfileData.Pack1vehicle3 );
			FormatToString( fileToWrite, "Pack1vehicle4 = %d;\n", m_ProfileData.Pack1vehicle4 );
			FormatToString( fileToWrite, "Pack1vehicle5 = %d;\n", m_ProfileData.Pack1vehicle5 );
		
			FormatToString( fileToWrite, "towTruckShouldUnlock = %d;\n", m_ProfileData.towTruckShouldUnlock );

			// levels
			/*FormatToString( fileToWrite, "pack1Level1 = %d;\n", m_ProfileData.pack1Level1 );
			FormatToString( fileToWrite, "pack1Level2 = %d;\n", m_ProfileData.pack1Level2 );
			FormatToString( fileToWrite, "pack1Level3 = %d;\n", m_ProfileData.pack1Level3 );
			FormatToString( fileToWrite, "pack1Level4 = %d;\n", m_ProfileData.pack1Level4 );
			FormatToString( fileToWrite, "pack1Level5 = %d;\n", m_ProfileData.pack1Level5 );

			FormatToString( fileToWrite, "pack2Level1 = %d;\n", m_ProfileData.pack2Level1 );
			FormatToString( fileToWrite, "pack2Level2 = %d;\n", m_ProfileData.pack2Level2 );
			FormatToString( fileToWrite, "pack2Level3 = %d;\n", m_ProfileData.pack2Level3 );
			FormatToString( fileToWrite, "pack2Level4 = %d;\n", m_ProfileData.pack2Level4 );
			FormatToString( fileToWrite, "pack2Level5 = %d;\n", m_ProfileData.pack2Level5 );

			FormatToString( fileToWrite, "pack3Level1 = %d;\n", m_ProfileData.pack3Level1 );
			FormatToString( fileToWrite, "pack3Level2 = %d;\n", m_ProfileData.pack3Level2 );
			FormatToString( fileToWrite, "pack3Level3 = %d;\n", m_ProfileData.pack3Level3 );
			FormatToString( fileToWrite, "pack3Level4 = %d;\n", m_ProfileData.pack3Level4 );
			FormatToString( fileToWrite, "pack3Level5 = %d;\n", m_ProfileData.pack3Level5 );*/

			// overall career stats
			FormatToString( fileToWrite, "newComplete = %d;\n", m_ProfileData.newComplete );
			FormatToString( fileToWrite, "careerGameOver = %d;\n", m_ProfileData.careerGameOver );
			FormatToString( fileToWrite, "numberOfCompletes = %d;\n", m_ProfileData.numberOfCompletes );
		
			FormatToString( fileToWrite, "extraLifeTotal = %d;\n", m_ProfileData.extraLifeTotal );

			FormatToString( fileToWrite, "totalCustomersTransported = %d;\n", m_ProfileData.totalCustomersTransported );
			FormatToString( fileToWrite, "totalCustomersLost = %d;\n", m_ProfileData.totalCustomersLost );
			FormatToString( fileToWrite, "totalParcelsTransported = %d;\n", m_ProfileData.totalParcelsTransported );
			FormatToString( fileToWrite, "totalParcelsLost = %d;\n", m_ProfileData.totalParcelsLost );
			FormatToString( fileToWrite, "perfectFares = %d;\n", m_ProfileData.perfectFares );
			FormatToString( fileToWrite, "totalMoneyEarned = %ld;\n", m_ProfileData.totalMoneyEarned );
			FormatToString( fileToWrite, "fuelPurchased = %ld;\n", m_ProfileData.fuelPurchased );
			FormatToString( fileToWrite, "taxisLost = %d;\n", m_ProfileData.taxisLost );
	
			FormatToString( fileToWrite, "best = {\n" );
			for( i=0; i < m_GameData.TOTAL_LEVELS; ++i )
			{
				if(i==0)
					FormatToString( fileToWrite, "{ %ld, %d }\n", m_ArcadeBestData[i].highestMoney, m_ArcadeBestData[i].maxStars );
				else
					FormatToString( fileToWrite, ",{ %ld, %d }\n", m_ArcadeBestData[i].highestMoney, m_ArcadeBestData[i].maxStars  );
			}
			FormatToString( fileToWrite, "};\n" );

#if defined(CABBY_LITE) || defined(CABBY_FREE)
			FormatToString( fileToWrite, "iap = {\n" );
			for( i=0; i < static_cast<int>(m_PurchaseList.size()); ++i )
			{
				if(i==0)
					FormatToString( fileToWrite, "{ \"%s\" }\n", m_PurchaseList[i].purchaseName );
				else
					FormatToString( fileToWrite, ",{ \"%s\" }\n", m_PurchaseList[i].purchaseName );
			}
			FormatToString( fileToWrite, "};\n" );
#endif // defined(CABBY_LITE) || defined(CABBY_FREE)

		FormatToString( fileToWrite, "}\n" );

		// write string to normal file
		fprintf( m_FileHandle.fp, "%s", fileToWrite.c_str() );

		file::FileClose( &m_FileHandle );

		// now call luac
	#ifdef _DEBUG
		if( script::LuaFunctionCheck( "SaveToBinary" ) == 0 )
	#endif // _DEBUG
		{
			int result = 0;

			int errorFuncIndex;
			errorFuncIndex = script::GetErrorFuncIndex();

			// get the function
			lua_getglobal( script::LuaScripting::GetState(), "SaveToBinary" );

			lua_pushstring( script::LuaScripting::GetState(), &m_ProfileName[0] ); // inputFile
			lua_pushstring( script::LuaScripting::GetState(), &m_ProfileName[0] ); // outputFile

			result = lua_pcall( script::LuaScripting::GetState(), 2, 0, errorFuncIndex );

			// LUA_ERRRUN --- a runtime error. 
			// LUA_ERRMEM --- memory allocation error. For such errors, Lua does not call the error handler function. 
			// LUA_ERRERR --- error while running the error handler function. 

			if( result == LUA_ERRRUN || result == LUA_ERRMEM || result == LUA_ERRERR )
			{
				DBGLOG( "PROFILEMANAGER: *ERROR* Calling function '%s' failed\n", "SaveToBinary" );
				DBGLOG( "\tLUA_TRACEBACK: %s\n", lua_tostring( script::LuaScripting::GetState(), -1 ) );

				script::StackDump( script::LuaScripting::GetState() );

				DBG_ASSERT(0);
			}
		}
	}
}

/////////////////////////////////////////////////////
/// Method: GetProfile
/// Params: [in]slot
///
/////////////////////////////////////////////////////
ProfileManager::Profile* ProfileManager::GetProfile()
{
	return( &m_ProfileData );
}

/////////////////////////////////////////////////////
/// Method: ClearActiveLevelData
/// Params: None
///
/////////////////////////////////////////////////////
void ProfileManager::ClearActiveLevelData()
{
	std::memset( &m_ActiveLevelData, 0, sizeof(ActiveLevelData) );
}
