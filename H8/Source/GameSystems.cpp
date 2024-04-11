
/*===================================================================
	File: GameSystems.cpp
	Game: H8

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "CollisionBase.h"
#include "ModelBase.h"
#include "SoundBase.h"
#include "PhysicsBase.h"
#include "SupportBase.h"

#include "H8.h"
#include "H8Consts.h"

#include "Camera/Camera.h"
#include "Camera/DebugCamera.h"
#include "Effects/Emitter.h"

#include "ScriptAccess/ScriptAccess.h"

#include "Resources/ModelResources.h"
#include "Resources/StringResources.h"
#include "Resources/AchievementList.h"

#include "Profiles/ProfileManager.h"

#include "GameStates/UI/AchievementUI.h"
#include "GameStates/UI/GameUI.h"

#include "GameSystems.h"

GameSystems* GameSystems::ms_Instance = 0;

namespace
{
	math::Vec3 zeroVec(0.0f,0.0f,0.0f);
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
GameSystems::GameSystems( InputSystem& inputSystem )
	: m_InputSystem(inputSystem)
{
	DBG_ASSERT_MSG( (ms_Instance == 0), "GameSystems instance already created" );

	ms_Instance = this;

	m_pScriptData = 0;

	m_pScriptData = GetScriptDataHolder();
	m_DevData = m_pScriptData->GetDevData();
	//m_CameraData = m_pScriptData->GetCameraData();
	m_GameData = m_pScriptData->GetGameData();

	// update some values before use
	math::Vec3 scaleFactor( core::app::GetWidthScale(), core::app::GetHeightScale(), 1.0f );

	m_GameData.ACHIEVEMENT_ICON_MAXX	*= scaleFactor.X;
	m_GameData.ACHIEVEMENT_ICON_SPEED	*= scaleFactor.X;

	//m_GameData.ANALOGUE_DEAD_ZONE		*= scaleFactor.X;
	
	// adbar assumes landscape
/*	m_GameData.ADBAR_PHONE_POSX		*= scaleFactor.Y;
	m_GameData.ADBAR_PHONE_POSY		*= scaleFactor.X;
	m_GameData.ADBAR_PHONEHD_POSX	*= scaleFactor.Y;
	m_GameData.ADBAR_PHONEHD_POSY	*= scaleFactor.X;
	m_GameData.ADBAR_TABLET_POSX	*= scaleFactor.Y;
	m_GameData.ADBAR_TABLET_POSY	*= scaleFactor.X;
	m_GameData.ADBAR_TABLETHD_POSX	*= scaleFactor.Y;
	m_GameData.ADBAR_TABLETHD_POSY	*= scaleFactor.X;
*/
	m_GameData.DOWNLOADBAR_MAX_WIDTH *= scaleFactor.X;
	m_GameData.DOWNLOADBAR_MAX_HEIGHT *= scaleFactor.Y;

	m_pScriptData->SetGameData(m_GameData);

	m_Player = 0;
	m_Core = 0;

	//m_GameCamera = 0;
	m_AchievementUI = 0;
	m_GameUI = 0;
	m_EffectPool = 0;

	m_CurrentScore = 0;
	m_LevelUpgradeCounter = 0;
	m_WrapAroundCounter = 0;
	m_Level = 0;

	m_ValidGameColourList.clear();

	m_CurrentScore = 0;
	m_PersonalBest = 0;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
GameSystems::~GameSystems()
{
	ms_Instance = 0;
}

/////////////////////////////////////////////////////
/// Method: LoggedIn
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::LoggedIn()
{
	// get latest achievements
	if( support::Scores::IsInitialised() )
		support::Scores::GetInstance()->SyncAchievements();
}

/////////////////////////////////////////////////////
/// Method: GetAchievementId
/// Params: [in]achievementString
///
/////////////////////////////////////////////////////
int GameSystems::GetAchievementId( const char* achievementId )
{
	return res::GetAchievementIndex( achievementId );
}

/////////////////////////////////////////////////////
/// Method: GetAchievementString
/// Params: [in]achievementId
///
/////////////////////////////////////////////////////
const char* GameSystems::GetAchievementString( int achievementId )
{
	return res::GetAchievementString( achievementId );
}

/////////////////////////////////////////////////////
/// Method: AchievementSubmitted
/// Params: [in]achievementId
///
/////////////////////////////////////////////////////
void GameSystems::AchievementSubmitted( int achievementId, int errorId )
{
	if( errorId == 0 )
	{
	
	}
	else
	// called when an achievement was successful/failed
	if( errorId != 0 )
	{
		DBGLOG( "%s failed to be submitted\n", res::GetAchievementString(achievementId) );
	}
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void GameSystems::Update( float deltaTime )
{
	UpdateAchievementUI( deltaTime );
}

/////////////////////////////////////////////////////
/// Method: CreatePlayer
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::CreatePlayer()
{
	//m_Player = new Player( m_InputSystem );
	//DBG_ASSERT_MSG( (m_Player != 0), "Could not create player" );
}

/////////////////////////////////////////////////////
/// Method: DestroyPlayer
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::DestroyPlayer()
{
	if( m_Player != 0 )
	{
		delete m_Player;
		m_Player = 0;
	}
}

/////////////////////////////////////////////////////
/// Method: CreatePhysics
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::CreatePhysics()
{
	b2Vec2 areaMin( -1000.0f, -1000.0f );
	b2Vec2 areaMax(1000.0f, 1000.0f);
	b2Vec2 gravity(0.0f,1.0f);
	
	physics::PhysicsWorldB2D::Create( areaMin, areaMax, gravity, m_DevData.enablePhysicsDraw );

	physics::PhysicsWorldB2D::GetWorld()->SetContactListener(&m_ContactListener);
}

/////////////////////////////////////////////////////
/// Method: DestroyPhysics
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::DestroyPhysics()
{
	physics::PhysicsWorldB2D::Destroy();
}

/////////////////////////////////////////////////////
/// Method: CreateScore
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::CreateScore()
{
    support::Scores::Create(this);

	m_AchievementUI = new AchievementUI;
	DBG_ASSERT( m_AchievementUI != 0 );

}

/////////////////////////////////////////////////////
/// Method: DestroyScore
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::DestroyScore()
{
    support::Scores::Destroy();

	if( m_AchievementUI != 0)
	{
		delete m_AchievementUI;
		m_AchievementUI = 0;
	}
}

/////////////////////////////////////////////////////
/// Method: CreatePurchases
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::CreatePurchase()
{
    support::Purchase::Create();
}

/////////////////////////////////////////////////////
/// Method: DestroyPurchases
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::DestroyPurchase()
{
    support::Purchase::Destroy();
}

/////////////////////////////////////////////////////
/// Method: AwardAchievement
/// Params: [in]achievementId
///
/////////////////////////////////////////////////////
void GameSystems::AwardAchievement( int achievementId, int textId )
{
	if( support::Scores::IsInitialised() )
	{
		if( achievementId != -1 )
		{
			if( support::Scores::GetInstance()->IsNewAchievement( achievementId ) )
				PrepareAchievementUI( achievementId, textId );
			else
			{
				if( textId != -1 )
					PrepareAchievementUI( -1, textId );
			}	
			
			support::Scores::GetInstance()->AwardAchievement( 100.0f, achievementId );
		}
		else
		{
			if( textId != -1 )
			{
				PrepareAchievementUI( achievementId, textId );
			}
		}
	}
}

/////////////////////////////////////////////////////
/// Method: AwardAchievement
/// Params: [in]achievementId
///
/////////////////////////////////////////////////////
void GameSystems::AwardAchievement( int achievementId, const char* text )
{
	if( support::Scores::IsInitialised() )
	{
		if( achievementId != -1 )
		{
			if( support::Scores::GetInstance()->IsNewAchievement( achievementId ) )
				PrepareAchievementUI( achievementId, text );
			else
			{
				if( text != 0 )
					PrepareAchievementUI( -1, text );
			}	

			support::Scores::GetInstance()->AwardAchievement( 100.0f, achievementId );
		}
		else
		{
			if( text != 0 )
			{
				PrepareAchievementUI( achievementId, text );
			}
		}
	}
}

/////////////////////////////////////////////////////
/// Method: PrepareAchievementUI
/// Params: [in]achievementId
///
/////////////////////////////////////////////////////
void GameSystems::PrepareAchievementUI( int achievementId, int textId  )
{
	if( m_AchievementUI != 0)
		m_AchievementUI->Show( achievementId, textId );
}

/////////////////////////////////////////////////////
/// Method: PrepareAchievementUI
/// Params: [in]achievementId
///
/////////////////////////////////////////////////////
void GameSystems::PrepareAchievementUI( int achievementId, const char* text  )
{
	if( m_AchievementUI != 0)
		m_AchievementUI->Show( achievementId, text );
}

/////////////////////////////////////////////////////
/// Method: UpdateAchievementUI
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::UpdateAchievementUI( float deltaTime )
{
	if( m_AchievementUI != 0 )
		m_AchievementUI->Update( deltaTime );
}

/////////////////////////////////////////////////////
/// Method: DrawAchievementUI
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::DrawAchievementUI()
{
	if( m_AchievementUI != 0)
		m_AchievementUI->Draw();
}

/////////////////////////////////////////////////////
/// Method: HasAchievement
/// Params: None
///
/////////////////////////////////////////////////////
bool GameSystems::HasAchievement( int achievementId )
{
	if( support::Scores::IsInitialised() )
	{
		if( achievementId != -1 )
		{
			return !support::Scores::GetInstance()->IsNewAchievement( achievementId );
		}
	}

	return false;
}

/////////////////////////////////////////////////////
/// Method: CreateGameCamera
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::CreateGameCamera()
{
	DBG_ASSERT_MSG( (m_Player != 0), "Player needs to exist before the game camera can be created" );

	m_GameCamera = new GameCamera( m_InputSystem, *m_Player );
	DBG_ASSERT_MSG( (m_GameCamera != 0), "Could not create GameCamera class" );

}

/////////////////////////////////////////////////////
/// Method: CreateGameCameraNoPlayer
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::CreateGameCameraNoPlayer()
{
	m_GameCamera = new GameCamera(m_InputSystem);
	DBG_ASSERT_MSG( (m_GameCamera != 0), "Could not create GameCamera class" );

}

/////////////////////////////////////////////////////
/// Method: DestroyGameCamera
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::DestroyGameCamera()
{
	if(m_GameCamera != 0)
	{
		delete m_GameCamera;
		m_GameCamera = 0;
	}
}

/////////////////////////////////////////////////////
/// Method: PreloadResources
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::PreloadResources()
{

}
		
/////////////////////////////////////////////////////
/// Method: ClearPreloaded
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::ClearPreloaded()
{

}

/////////////////////////////////////////////////////
/// Method: DrawDebug
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::DrawDebug()
{

}

/////////////////////////////////////////////////////
/// Method: CreateEffectPool
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::CreateEffectPool()
{
	m_EffectPool = new EffectPool( );
	DBG_ASSERT_MSG( (m_EffectPool != 0), "Could not create EffectPool class" );
}
		
/////////////////////////////////////////////////////
/// Method: DestroyEffectPool
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::DestroyEffectPool()
{
	if(m_EffectPool != 0)
	{
		delete m_EffectPool;
		m_EffectPool = 0;
	}
}
		
/////////////////////////////////////////////////////
/// Method: DrawEffectPool
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::DrawEffectPool()
{
	if(m_EffectPool != 0)
		m_EffectPool->Draw();
}

/////////////////////////////////////////////////////
/// Method: UpdateEffectPool
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::UpdateEffectPool( float deltaTime )
{
	if(m_EffectPool != 0)
		m_EffectPool->Update(deltaTime);
}

/////////////////////////////////////////////////////
/// Method: ResetEffectPool
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::ResetEffectPool( )
{
	if(m_EffectPool != 0)
		m_EffectPool->Reset();
}

/////////////////////////////////////////////////////
/// Method: SpawnCircle
/// Params: [in]pos, [in]col
///
/////////////////////////////////////////////////////
void GameSystems::SpawnExplosion( const math::Vec3& pos, const math::Vec3& col )
{
	if(m_EffectPool != 0)
		m_EffectPool->SpawnExplosion(pos, col);
}

/////////////////////////////////////////////////////
/// Method: SpawnCircle
/// Params: [in]pos, [in]col
///
/////////////////////////////////////////////////////
void GameSystems::SpawnCircle( const math::Vec3& pos, const math::Vec3& col )
{
	if(m_EffectPool != 0)
		m_EffectPool->SpawnCircle(pos, col);
}

/////////////////////////////////////////////////////
/// Method: SpawnHearts
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::SpawnHearts()
{
	if(m_EffectPool != 0)
		m_EffectPool->SpawnHearts();
}

/////////////////////////////////////////////////////
/// Method: InitAudio
/// Params: 
///
/////////////////////////////////////////////////////
void GameSystems::InitAudio()
{

}

/////////////////////////////////////////////////////
/// Method: AddToScore
/// Params: 
///
/////////////////////////////////////////////////////
void GameSystems::AddToScore( int val )		
{ 
	m_CurrentScore += val; 

	if( m_CurrentScore < 0 )
		m_CurrentScore = 0;

	if( m_GameUI != 0 )
		m_GameUI->SetCurrentScore( m_CurrentScore );

	if( m_CurrentScore > m_PersonalBest )
	{
		if( !m_JustBeatenPB )
		{
			//AddTextToConsole( "PERSONAL BEST BEATEN" );
			//AddTextToConsole( "Well Done" );

			m_JustBeatenPB = true;
		}

		SetPersonalBest( m_CurrentScore );
	}
}
		
/////////////////////////////////////////////////////
/// Method: ResetScore
/// Params: 
///
/////////////////////////////////////////////////////
void GameSystems::ResetScore()				
{ 
	m_CurrentScore = 0; 
	m_JustBeatenPB = false;

	if( m_GameUI != 0 )
		m_GameUI->SetCurrentScore( m_CurrentScore );
}

/////////////////////////////////////////////////////
/// Method: SetPersonalBest
/// Params: 
///
/////////////////////////////////////////////////////
void GameSystems::SetPersonalBest( unsigned long long int personalBest )	
{
	m_PersonalBest = personalBest;

	if( m_GameUI != 0 )
		m_GameUI->SetPersonalBest( personalBest );
}

/////////////////////////////////////////////////////
/// Method: AddTextToConsole
/// Params: 
///
/////////////////////////////////////////////////////
void GameSystems::AddTextToConsole( const char* text, ...)
{
	char szText[UI_MAXSTATICTEXTBUFFER_SIZE];
	std::va_list ap;				

	if( text == 0 ||
		std::strlen(text) <= 0 )		
		return;				

	va_start( ap, text );					
		std::vsprintf( szText, text, ap );			
	va_end( ap );

	if( m_GameUI != 0 )
		m_GameUI->AddTextToConsole( szText );
}

/////////////////////////////////////////////////////
/// Method: LoadColourIndexList
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::LoadColourIndexList()
{
#ifdef _DEBUG
	if( script::LuaFunctionCheck( "LoadValidColours" ) == 0 )
#endif // _DEBUG
	{
		int result = 0;

		int errorFuncIndex;
		errorFuncIndex = script::GetErrorFuncIndex();

		// get the function
		lua_getglobal( script::LuaScripting::GetState(), "LoadValidColours" );

		result = lua_pcall( script::LuaScripting::GetState(), 0, 1, errorFuncIndex );

		// LUA_ERRRUN --- a runtime error. 
		// LUA_ERRMEM --- memory allocation error. For such errors, Lua does not call the error handler function. 
		// LUA_ERRERR --- error while running the error handler function. 

		if( result == LUA_ERRRUN || result == LUA_ERRMEM || result == LUA_ERRERR )
		{
			DBGLOG( "RESOURCES: *ERROR* Calling function '%s' failed\n", "LoadValidColours" );
			DBGLOG( "\tLUA_TRACEBACK: %s\n", lua_tostring( script::LuaScripting::GetState(), -1 ) );
			
			DBG_ASSERT_MSG( 0, "RESOURCES: *ERROR* Calling function '%s' failed", "LoadValidColours" );
			return;
		}

		// should be a table of tables
		if( lua_istable( script::LuaScripting::GetState(), -1 ) )
		{
			int i=0;
			int n = luaL_len( script::LuaScripting::GetState(), -1 );

			DBG_ASSERT( n < MAX_COLOURS+1 );
			m_TotalColours = n;

			// go through all the tables in this table
			for( i = 1; i <= n; ++i )
			{
				lua_rawgeti( script::LuaScripting::GetState(), -1, i );
				if( lua_istable( script::LuaScripting::GetState(), -1 ) )
				{
					int innerCount = luaL_len( script::LuaScripting::GetState(), -1 );
					
					if( innerCount == 4 )
					{
						int paramIndex = 1;

						// index first
						lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
						int index = static_cast<int>( lua_tonumber( script::LuaScripting::GetState(), -1 ) );

						lua_pop( script::LuaScripting::GetState(), 1 );

						// red
						lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
						int red = static_cast<int>( lua_tonumber( script::LuaScripting::GetState(), -1 ) );

						lua_pop( script::LuaScripting::GetState(), 1 );

						// green
						lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
						int green = static_cast<int>( lua_tonumber( script::LuaScripting::GetState(), -1 ) );

						lua_pop( script::LuaScripting::GetState(), 1 );

						// blue
						lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
						int blue = static_cast<int>( lua_tonumber( script::LuaScripting::GetState(), -1 ) );

						lua_pop( script::LuaScripting::GetState(), 1 );

						m_ColourLookup[i-1].R = static_cast<float>(red)/255.0f;
						m_ColourLookup[i-1].G = static_cast<float>(green)/255.0f;
						m_ColourLookup[i-1].B = static_cast<float>(blue)/255.0f;

					}
				}
				lua_pop( script::LuaScripting::GetState(), 1 );	
			}
			lua_pop( script::LuaScripting::GetState(), 1 );
		}
	}
}

/////////////////////////////////////////////////////
/// Method: SetValidColoursList
/// Params: 
///
/////////////////////////////////////////////////////
void GameSystems::SetValidColoursList( std::vector<int> colourList )
{
	std::vector<int>::iterator it = colourList.begin();
	m_ValidGameColourList.clear();

	while( it != colourList.end() )
	{
		m_ValidGameColourList.push_back( (*it) );

		// next
		it++;
	}
}

/////////////////////////////////////////////////////
/// Method: GetColourForIndex
/// Params: [in]colourIndex, [in]multiply
///
/////////////////////////////////////////////////////
b2Color GameSystems::GetColourForIndex( int colourIndex, float multiply )
{
	if( colourIndex < 0 )
		return b2Color( 1.0f, 1.0f, 1.0f );

	DBG_ASSERT( colourIndex < m_TotalColours );

	return b2Color( m_ColourLookup[colourIndex].R*multiply, m_ColourLookup[colourIndex].G*multiply, m_ColourLookup[colourIndex].B*multiply );

}

/////////////////////////////////////////////////////
/// Method: GetColourForIndex
/// Params: [in]colourIndex
///
/////////////////////////////////////////////////////
math::Vec3 GameSystems::GetColourForIndex( int colourIndex )
{
	DBG_ASSERT( (colourIndex >= 0) && (colourIndex < MAX_COLOURS) );

	return m_ColourLookup[colourIndex];
}
