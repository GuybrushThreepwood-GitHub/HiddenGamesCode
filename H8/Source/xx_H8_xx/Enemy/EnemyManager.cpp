
/*===================================================================
	File: EnemyManager.cpp
	Game: H8

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "SoundBase.h"
#include "CollisionBase.h"
#include "Model/ModelCommon.h"
#include "ModelBase.h"
#include "InputBase.h"
#include "ScriptBase.h"

#include "Camera/Camera.h"
#include "Camera/DebugCamera.h"

#include "H8Consts.h"
#include "H8.h"

#include "Profiles/ProfileManager.h"

#include "Resources/EmitterResources.h"
#include "Resources/ModelResources.h"
#include "Resources/TextureResources.h"

#include "Enemy/EnemyManager.h"

namespace
{
	math::Vec3 zeroVec(0.0f,0.0f,0.0f);
	b2Vec2 b2ZeroVec( 0.0f, 0.0f );

	const float MAX_LINE_SIZE_PUMP	= 3.0f;
	const float LINE_SIZE_INC		= 5.0f;
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
EnemyManager::EnemyManager()
{
	m_pScriptData = GetScriptDataHolder();
	m_GameData = m_pScriptData->GetGameData();
	m_DevData = m_pScriptData->GetDevData();

	m_LastDelta = 0.0f;

	m_NumEnemyTypes = 0;
	m_EnemyTypes = 0;

	m_Enemies = 0;
	m_Bullets = 0;

	m_LinePulseUp = true;
	m_LineSize = 1.0f;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
EnemyManager::~EnemyManager()
{
	int i=0;

	if( m_EnemyTypes != 0 )
	{
		for( i=0; i < m_NumEnemyTypes; ++i )
		{
			if( m_EnemyTypes[i] != 0 )
			{
				if( m_EnemyTypes[i]->modelData != 0 )
				{
					res::RemoveModel( m_EnemyTypes[i]->modelData );
					m_EnemyTypes[i]->modelData = 0;
				}

				delete m_EnemyTypes[i];
				m_EnemyTypes[i] = 0;
			}
		}
		
		delete[] m_EnemyTypes;
		m_EnemyTypes = 0;
	}

	if( m_Enemies != 0 )
	{
		for( i=0; i < MAX_ENEMIES; ++i )
		{
			delete m_Enemies[i];
			m_Enemies[i] = 0;
		}

		delete[] m_Enemies;
		m_Enemies = 0;
	}

	if( m_Bullets != 0 )
	{
		for( i=0; i < MAX_ENEMIES; ++i )
		{
			delete m_Bullets[i];
			m_Bullets[i] = 0;
		}

		delete[] m_Bullets;
		m_Bullets = 0;
	}
}

/////////////////////////////////////////////////////
/// Method: LoadEnemyTypes
/// Params: None
///
/////////////////////////////////////////////////////
void EnemyManager::LoadEnemyTypes()
{
#ifdef _DEBUG
	if( script::LuaFunctionCheck( "LoadEnemySetups" ) == 0 )
#endif // _DEBUG
	{
		int result = 0;

		int errorFuncIndex;
		errorFuncIndex = script::GetErrorFuncIndex();

		// get the function
		lua_getglobal( script::LuaScripting::GetState(), "LoadEnemySetups" );

		result = lua_pcall( script::LuaScripting::GetState(), 0, 1, errorFuncIndex );

		// LUA_ERRRUN --- a runtime error. 
		// LUA_ERRMEM --- memory allocation error. For such errors, Lua does not call the error handler function. 
		// LUA_ERRERR --- error while running the error handler function. 

		if( result == LUA_ERRRUN || result == LUA_ERRMEM || result == LUA_ERRERR )
		{
			DBGLOG( "RESOURCES: *ERROR* Calling function '%s' failed\n", "LoadEnemySetups" );
			DBGLOG( "\tLUA_TRACEBACK: %s\n", lua_tostring( script::LuaScripting::GetState(), -1 ) );
			
			DBG_ASSERT_MSG( 0, "RESOURCES: *ERROR* Calling function '%s' failed", "LoadEnemySetups" );
			return;
		}

		// should be a table of tables
		if( lua_istable( script::LuaScripting::GetState(), -1 ) )
		{
			int i=0;
			int n = luaL_len( script::LuaScripting::GetState(), -1 );

			m_NumEnemyTypes = n;

			m_EnemyTypes = new EnemyTypeData*[m_NumEnemyTypes];
			DBG_ASSERT( m_EnemyTypes != 0 );

			// go through all the tables in this table
			for( i = 1; i <= n; ++i )
			{
				m_EnemyTypes[i-1] = new EnemyTypeData;
				DBG_ASSERT( m_EnemyTypes[i-1] != 0 );

				lua_rawgeti( script::LuaScripting::GetState(), -1, i );
				if( lua_istable( script::LuaScripting::GetState(), -1 ) )
				{
					int paramIndex = 2;

					int type = ENEMYTYPE_VIRUS;
					int hgmModelIndex = -1;								
					float radius = 0.5f;
					int coreDamage = 1;
					int matchPoints = 1;
					int shieldDamage = 1;
					float force = 0.3f;		

					type			= static_cast<int>( script::LuaGetNumberFromTableItem( "type", paramIndex, 0.0 ) );
					hgmModelIndex	= static_cast<int>( script::LuaGetNumberFromTableItem( "hgmModelIndex", paramIndex, -1.0 ) );
					radius			= static_cast<float>( script::LuaGetNumberFromTableItem( "radius", paramIndex, 0.5 ) );
					coreDamage		= static_cast<int>( script::LuaGetNumberFromTableItem( "coreDamage", paramIndex, 1.0 ) );
					matchPoints		= static_cast<int>( script::LuaGetNumberFromTableItem( "matchPoints", paramIndex, 1.0 ) );
					shieldDamage	= static_cast<int>( script::LuaGetNumberFromTableItem( "shieldDamage", paramIndex, 1.0 ) );
					force			= static_cast<float>( script::LuaGetNumberFromTableItem( "force", paramIndex, 0.3 ) );


					m_EnemyTypes[i-1]->typeId			= type;
					m_EnemyTypes[i-1]->hgmModelIndex	= hgmModelIndex;
					m_EnemyTypes[i-1]->radius			= radius;
					m_EnemyTypes[i-1]->coreDamage		= coreDamage;
					m_EnemyTypes[i-1]->matchPoints		= matchPoints;
					m_EnemyTypes[i-1]->shieldDamage		= shieldDamage;
					m_EnemyTypes[i-1]->force			= force;

					if( m_EnemyTypes[i-1]->hgmModelIndex != -1 )
					{
						m_EnemyTypes[i-1]->modelData = res::LoadModel( m_EnemyTypes[i-1]->hgmModelIndex );
						DBG_ASSERT( m_EnemyTypes[i-1]->modelData != 0 );

						if( m_EnemyTypes[i-1]->modelData != 0 )
						{
							m_EnemyTypes[i-1]->modelData->SetModelGeneralFlags(0);
						}
					}
				}
				lua_pop( script::LuaScripting::GetState(), 1 );	
			}
			lua_pop( script::LuaScripting::GetState(), 1 );
		}
	}
}

/////////////////////////////////////////////////////
/// Method: Setup
/// Params: None
///
/////////////////////////////////////////////////////
void EnemyManager::Initialise()
{
	int i=0;

	m_pScriptData = GetScriptDataHolder();
	m_DevData = m_pScriptData->GetDevData();
	m_GameData = m_pScriptData->GetGameData();

	m_CountdownToAttack = math::RandomNumber( m_GameData.ENEMY_ATTACK_TIME_MIN, m_GameData.ENEMY_ATTACK_TIME_MAX );

	LoadEnemyTypes();

	// enemies
	math::Vec3 enemyPos( zeroVec );
	m_Enemies = new Enemy*[MAX_ENEMIES];
	DBG_ASSERT( m_Enemies != 0 );

	m_Bullets = new Enemy*[MAX_ENEMIES];
	DBG_ASSERT( m_Bullets != 0 );

	const float pointInc = 2.0f * math::PI / static_cast<float>(MAX_ENEMIES);
	float theta = 0.0f;	

	for( i=0; i < MAX_ENEMIES; ++i )
	{
		m_Enemies[i] = new Enemy( this );
		DBG_ASSERT( m_Enemies != 0 );

		enemyPos = math::Vec3(std::cos(theta), std::sin(theta), 0.0f ) * SPAWN_RADIUS;
		m_Enemies[i]->SetStartPoint( enemyPos, theta );
		theta += pointInc;

		m_Enemies[i]->Disable();
	}

	for( i=0; i < MAX_ENEMIES; ++i )
	{
		m_Bullets[i] = new Enemy( this );
		DBG_ASSERT( m_Bullets != 0 );

		m_Bullets[i]->Disable();
	}

	m_TrojanAppearAudio = AudioSystem::GetInstance()->AddAudioFile( 50 );
	m_VirusAppearAudio = AudioSystem::GetInstance()->AddAudioFile( 51 );


	for( i=0; i < EXPLOSION_AUDIO; ++i )
		m_ExplosionAudio[i] = AudioSystem::GetInstance()->AddAudioFile( 30+i );
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void EnemyManager::Draw()
{
	int i=0;	
	
	// draw lines
	math::Vec3 vaPoints[2];

	if( m_Enemies != 0 )
	{
		for( i=0; i < MAX_ENEMIES; ++i )
		{
			if( m_Enemies[i]->IsEnabled() )
				m_Enemies[i]->Draw( );
		}

		for( i=0; i < MAX_ENEMIES; ++i )
		{
			if( m_Bullets[i]->IsEnabled() )
				m_Bullets[i]->Draw( );
		}

		if( m_DevData.showEnemyLines )
		{
			bool textureState = renderer::OpenGL::GetInstance()->GetTextureState();

			if( textureState )
				renderer::OpenGL::GetInstance()->DisableTexturing();

			renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
			glLineWidth( m_LineSize );
			for( i=0; i < MAX_ENEMIES; ++i )
			{
				// draw lines
				if( m_Enemies[i]->IsEnabled( ) && 
					!m_Enemies[i]->HasRebound() )
				{
					math::Vec3 lineColour = m_Enemies[i]->GetColour();

					vaPoints[1].setZero();
					vaPoints[1].Z = 1.0f;
					vaPoints[0] = m_Enemies[i]->GetCurrentPosition();

					renderer::OpenGL::GetInstance()->SetColour4f( lineColour.R, lineColour.G, lineColour.B, 0.5f );

					glVertexPointer( 3, GL_FLOAT, sizeof(math::Vec3), vaPoints );
					glDrawArrays(GL_LINES, 0, 2);
				}
			}
			glLineWidth( 1.0f );
			renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

			if( textureState )
				renderer::OpenGL::GetInstance()->EnableTexturing();
		}
	}
}

/////////////////////////////////////////////////////
/// Method: DrawDebug
/// Params: None
///
/////////////////////////////////////////////////////
void EnemyManager::DrawDebug()
{

}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void EnemyManager::Update( float deltaTime )
{
	int i=0;
	m_LastDelta = deltaTime;

	if( !m_AttackPaused )
	{
		m_CountdownToAttack -= deltaTime;
		if( m_CountdownToAttack <= 0.0f )
		{
			m_CountdownToAttack = math::RandomNumber( m_GameData.ENEMY_ATTACK_TIME_MIN, m_GameData.ENEMY_ATTACK_TIME_MAX );

			bool attack = false;
			while( !attack )
			{
				int index = math::RandomNumber( 0, MAX_ENEMIES-1 );
				if( !m_Enemies[index]->IsEnabled() )
				{
					std::vector<int> validColours = GameSystems::GetInstance()->GetValidColoursList();

					int idx = math::RandomNumber(0, static_cast<int>(validColours.size() - 1));

					int shieldShape = GameSystems::GetInstance()->GetPlayer().GetShieldShape();

					bool spawnedSpecialType = false;
					
					// trojan
					if( GameSystems::GetInstance()->GetLevelCounter() > m_GameData.TROJAN_MIN_APPEAR_LEVEL )
					{
						int randomProbability = math::RandomNumber( 1, 10 );
						if(randomProbability <= m_GameData.TROJAN_APPEAR_PROBABILITY )
						{
							m_Enemies[index]->Attack( *m_EnemyTypes[ENEMYTYPE_TROJAN], validColours[idx], GameSystems::GetInstance()->GetColourForIndex( validColours[idx] ) );
							spawnedSpecialType = true;

							GameSystems::GetInstance()->AddTextToConsole( "***************************" );
							GameSystems::GetInstance()->AddTextToConsole( "Trojan detected......." );
							GameSystems::GetInstance()->AddTextToConsole( "origin unknown......" );
							GameSystems::GetInstance()->AddTextToConsole( "...........scanning.............." );

							// trojan appear
							if( m_TrojanAppearAudio != snd::INVALID_SOUNDBUFFER )
							{
								AudioSystem::GetInstance()->PlayAudio( m_TrojanAppearAudio, math::Vec3(0.0f,0.0f,0.0f), AL_TRUE );
							}
						}
					}

					// virus
					if( !spawnedSpecialType &&
						GameSystems::GetInstance()->GetLevelCounter() > m_GameData.VIRUS_MIN_APPEAR_LEVEL )
					{
						int randomProbability = math::RandomNumber( 1, 10 );
						if(randomProbability <= m_GameData.VIRUS_APPEAR_PROBABILITY )
						{
							m_Enemies[index]->Attack( *m_EnemyTypes[ENEMYTYPE_VIRUS], validColours[idx], GameSystems::GetInstance()->GetColourForIndex( validColours[idx] ) );
							spawnedSpecialType = true;

							GameSystems::GetInstance()->AddTextToConsole( "***************************" );
							GameSystems::GetInstance()->AddTextToConsole( "Virus detected" );
							GameSystems::GetInstance()->AddTextToConsole( "....Prepare for attack........" );

							// trojan appear

							if(m_VirusAppearAudio != snd::INVALID_SOUNDBUFFER )
							{
								AudioSystem::GetInstance()->PlayAudio( m_VirusAppearAudio, math::Vec3(0.0f,0.0f,0.0f), AL_TRUE );
							}
						}
					}

					// normal spawn
					if( !spawnedSpecialType )
					{

						int randomProbability = math::RandomNumber( 1, 10 );
						if(randomProbability <= 1 )
							m_Enemies[index]->Attack( *m_EnemyTypes[ENEMYTYPE_BITS_GENERIC], validColours[idx], GameSystems::GetInstance()->GetColourForIndex( validColours[idx] ) );
						else
							m_Enemies[index]->Attack( *m_EnemyTypes[shieldShape], validColours[idx], GameSystems::GetInstance()->GetColourForIndex( validColours[idx] ) );
					}
					attack = true;
				}
				
			}
		}
	}

	if( m_Enemies != 0 )
	{
		for( i=0; i < MAX_ENEMIES; ++i )
		{
			m_Enemies[i]->Update( deltaTime );
		}
	}

	if( m_Bullets != 0 )
	{
		for( i=0; i < MAX_ENEMIES; ++i )
		{
			m_Bullets[i]->Update( deltaTime );
		}
	}

	if( m_LinePulseUp )
	{
		if( m_LineSize <= MAX_LINE_SIZE_PUMP )
			m_LineSize += LINE_SIZE_INC*deltaTime;
		else
		{
			m_LineSize = MAX_LINE_SIZE_PUMP;
			m_LinePulseUp = false;
		}
	}
	else
	{
		if( m_LineSize > 0.1f )
			m_LineSize -= LINE_SIZE_INC*deltaTime;
		else
		{
			m_LineSize = 0.1f;
			m_LinePulseUp = true;
		}
	}
}

/////////////////////////////////////////////////////
/// Method: PauseAttack
/// Params: None
///
/////////////////////////////////////////////////////
void EnemyManager::PauseAttack()
{
	m_AttackPaused = true;
}
		
/////////////////////////////////////////////////////
/// Method: UnPauseAttack
/// Params: None
///
/////////////////////////////////////////////////////
void EnemyManager::UnPauseAttack()
{
	m_AttackPaused = false;
}

/////////////////////////////////////////////////////
/// Method: SpawnBullet
/// Params: [in]pos, [in]angle
///
/////////////////////////////////////////////////////
void EnemyManager::SpawnBullet( math::Vec3 pos, float angle, int colourIndex, const math::Vec3& colour )
{
	bool attack = false;
	while( !attack )
	{
		int index = math::RandomNumber( 0, MAX_ENEMIES-1 );
		if( !m_Bullets[index]->IsEnabled() )
		{
			m_Bullets[index]->SetStartPoint( pos, angle );
			m_Bullets[index]->Attack( *m_EnemyTypes[ENEMYTYPE_BULLET], colourIndex, colour );
					
		}
		attack = true;
	}
}

/////////////////////////////////////////////////////
/// Method: DestroyActiveEnemies
/// Params: None
///
/////////////////////////////////////////////////////
void EnemyManager::DestroyActiveEnemies(bool awardPoints)
{
	int i=0;

	if( m_Enemies != 0 )
	{
		for( i=0; i < MAX_ENEMIES; ++i )
		{
			if( m_Enemies[i]->IsEnabled() )
			{
				if( !m_Enemies[i]->HasRebound() )
				{
					// enemy is fresh, add a bonus to the points
					if( awardPoints )
						GameSystems::GetInstance()->AddToScore( 10*m_Enemies[i]->GetPoints() );	
				}

				int randomSfx = math::RandomNumber( 0, EXPLOSION_AUDIO-1 );

				if( m_ExplosionAudio[randomSfx] != snd::INVALID_SOUNDBUFFER )
				{
					float randomPitch = math::RandomNumber( -0.5f, 0.5f );
					AudioSystem::GetInstance()->PlayAudio( m_ExplosionAudio[randomSfx], math::Vec3(0.0f,0.0f,0.0f), AL_TRUE, AL_FALSE, 1.0f+randomPitch, 0.25f );
				}

				GameSystems::GetInstance()->SpawnExplosion( m_Enemies[i]->GetCurrentPosition(), m_Enemies[i]->GetColour() );
				m_Enemies[i]->Destroy();
			}
		}
	}

	if( m_Bullets != 0 )
	{
		for( i=0; i < MAX_ENEMIES; ++i )
		{
			if( m_Bullets[i]->IsEnabled() )
			{
				if( !m_Bullets[i]->HasRebound() )
				{
					if( awardPoints)
						GameSystems::GetInstance()->AddToScore( 10*m_Bullets[i]->GetPoints() );	// bullet is fresh, add a bonus to the points
				}

				int randomSfx = math::RandomNumber( 0, EXPLOSION_AUDIO-1 );

				if( m_ExplosionAudio[randomSfx] != snd::INVALID_SOUNDBUFFER )
				{
					float randomPitch = math::RandomNumber( -0.5f, 0.5f );
					AudioSystem::GetInstance()->PlayAudio( m_ExplosionAudio[randomSfx], math::Vec3(0.0f,0.0f,0.0f), AL_TRUE, AL_FALSE, 1.0f+randomPitch, 0.25f );
				}

				GameSystems::GetInstance()->SpawnExplosion( m_Bullets[i]->GetCurrentPosition(), m_Bullets[i]->GetColour() );
				m_Bullets[i]->Destroy();
			}
		}
	}
}

