
/*===================================================================
	File: Player.cpp
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
#include "Resources/SoundResources.h"

#include "Player/Player.h"

namespace
{
	GLfloat m[16] = 
	{ 1.0f, 0.0f, 0.0f, 0.0f,
	  0.0f, 1.0f, 0.0f, 0.0f,
	  0.0f, 0.0f, 1.0f, 0.0f,
	  0.0f, 0.0f, 0.0f, 1.0f };

	math::Vec3 zeroVec(0.0f,0.0f,0.0f);
	b2Vec2 b2ZeroVec( 0.0f, 0.0f );
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
Player::Player( InputSystem& inputSystem )
	: PhysicsIdentifier( PHYSICSBASICID_SHIELD, PHYSICSCASTID_SHIELD)
	, m_InputSystem(inputSystem)
{
	m_InputSystem.ResetAllInput();

	m_pScriptData = GetScriptDataHolder();
	m_GameData = m_pScriptData->GetGameData();
	m_DevData = m_pScriptData->GetDevData();

	m_AnimState = AnimState_Initial;
	m_AnimComplete = true;
	m_AnimTimer = m_GameData.SHIELD_PIECE_ANIM_TIME;


	m_LastDelta = 0.0f;
	m_pModelData = 0;
	m_NumSubMeshes = 0;
	m_TotalEnabledSubMeshes = 0;

	m_Body = 0;
	m_Joint = 0;

	m_RotationForce = 20.0f;
	m_Reset = false;

	m_FixturesHit.clear();
	m_ValidColours.clear();

	m_NumProtections = 0;
	m_NumShieldPieces = 0;
	m_NumConsecutiveProtections = 0;
	m_ConsecutiveMulti = 1;

	m_ShieldPieceData.clear();

	m_CurrentLevel = 0;
	m_LevelSetupList.clear();

	m_ShieldInOutIndex =0;

	m_LevelUpState = false;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
Player::~Player()
{
	if( m_Body != 0 )
	{
		physics::PhysicsIdentifier* pPhysId = 0;

		// cleanup the user data
		b2Fixture* pElement = m_Body->GetFixtureList();
		while( pElement != 0 )
		{
			pPhysId = reinterpret_cast<physics::PhysicsIdentifier* >( pElement->GetUserData() );
			if( pPhysId != 0 &&
				pPhysId != this )
			{
				delete pPhysId;
				pPhysId = 0;
			}
			pElement = pElement->GetNext();
		}

		pPhysId = reinterpret_cast<physics::PhysicsIdentifier* >( m_Body->GetUserData() );
		if( pPhysId != 0 &&  
			pPhysId != this )
		{
			delete pPhysId;
			pPhysId = 0;
		}
	}

	if( m_pModelData != 0 )
	{
		res::RemoveModel( m_pModelData );
		m_pModelData = 0;
	}
}

/////////////////////////////////////////////////////
/// Method: Initialise
/// Params: None
///
/////////////////////////////////////////////////////
void Player::Initialise()
{
	int i=0;

	LoadShields();

	m_NextChime = 0;

	for( i=0; i < AUDIO_CHIME_SUCCESS_POOL; ++i )
	{
		m_CurrentSuccessList[i] = AudioSystem::GetInstance()->AddAudioFile( 201+i );
	}

	m_ShieldInOutIndex = 0;
	for( i=0; i < AUDIO_INOUT_COUNT; ++i )
		m_ShieldInOutIds[i] = AudioSystem::GetInstance()->AddAudioFile( 70+i );
}

/////////////////////////////////////////////////////
/// Method: Setup
/// Params: None
///
/////////////////////////////////////////////////////
void Player::Setup( const char* shieldSetup  )
{
	int i=0;

	ClearCurrentShield();

	m_ShieldPieceData.clear();
	m_ValidColours.clear();
	m_FixturesHit.clear();

	math::Vec3* colourIndexList = GameSystems::GetInstance()->GetColourLookupPtr();

	// create the box2d shape for the shield
#ifdef _DEBUG
	if( script::LuaFunctionCheck( shieldSetup ) == 0 )
#endif // _DEBUG
	{
		int result = 0;

		int errorFuncIndex;
		errorFuncIndex = script::GetErrorFuncIndex();

		// get the function
		lua_getglobal( script::LuaScripting::GetState(), shieldSetup );

		lua_pushnumber( script::LuaScripting::GetState(), 0.0f ); // posX
		lua_pushnumber( script::LuaScripting::GetState(), 0.0f ); // posY

		result = lua_pcall( script::LuaScripting::GetState(), 2, 2, errorFuncIndex );

		// LUA_ERRRUN --- a runtime error. 
		// LUA_ERRMEM --- memory allocation error. For such errors, Lua does not call the error handler function. 
		// LUA_ERRERR --- error while running the error handler function. 

		if( result == LUA_ERRRUN || result == LUA_ERRMEM || result == LUA_ERRERR )
		{
			DBGLOG( "PLAYER: *ERROR* Calling function '%s' failed\n", shieldSetup );
			DBGLOG( "\tLUA_TRACEBACK: %s\n", lua_tostring( script::LuaScripting::GetState(), -1 ) );
			DBG_ASSERT(0);

			return;
		}

		// grab the return
		m_Joint = reinterpret_cast<b2RevoluteJoint *>( lua_touserdata(script::LuaScripting::GetState(), -1) );
		lua_pop( script::LuaScripting::GetState(), 1 );
		DBG_ASSERT( m_Joint != 0 );

		m_Body = reinterpret_cast<b2Body *>( lua_touserdata(script::LuaScripting::GetState(), -1) );
		lua_pop( script::LuaScripting::GetState(), 1 );
		DBG_ASSERT( m_Body != 0 );

		// remove user data if it exists
		if( m_Body->GetUserData() != 0 )
		{
			physics::PhysicsIdentifier* pPhysId = reinterpret_cast<physics::PhysicsIdentifier*>( m_Body->GetUserData() );
			if( pPhysId != 0 )
			{
				SetNumeric1( pPhysId->GetNumeric1() );
				SetNumeric2( pPhysId->GetNumeric2() );
				SetNumeric3( pPhysId->GetNumeric3() );
				SetNumeric4( pPhysId->GetNumeric4() );
				SetNumeric5( pPhysId->GetNumeric5() );

				if( pPhysId->GetNumeric1() != -9999 )
				{
					// create model and anims
					m_pModelData = res::LoadModel( pPhysId->GetNumeric1() );
					DBG_ASSERT( m_pModelData != 0 );
				}

				// piece count
				if( pPhysId->GetNumeric2() != -9999 )
					m_NumShieldPieces = pPhysId->GetNumeric2();

				// rotation
				//if( pPhysId->GetNumeric3() != -9999 )
				//	m_RotationForce = static_cast<float>( pPhysId->GetNumeric3() );

				if( pPhysId->GetNumeric4() != -9999 )
					SetShieldShape( pPhysId->GetNumeric4() );

										
				// turn off all meshes
				//if( m_AnimState == AnimState_Initial )
				{
					m_pModelData->SetMeshDrawState( -1, false );
					m_NumSubMeshes = m_pModelData->GetNumOpaqueMeshes();
					m_TotalEnabledSubMeshes = 0;
				}

				delete pPhysId;
			}
		}
		else
			DBG_ASSERT(0); // need the data set

		m_Body->SetUserData( reinterpret_cast<void *>(this) );

		b2Filter fd;

		fd.groupIndex = SHIELD_GROUP;
		fd.categoryBits = SHIELD_CATEGORY;
		fd.maskBits = ENEMY_CATEGORY;

		b2Fixture* fixtureList = m_Body->GetFixtureList();
		physics::PhysicsIdentifier* physId = 0;

		while( fixtureList != 0 )
		{
			fixtureList->SetFilterData(fd);
			
			// add valid colours
			physId = reinterpret_cast<physics::PhysicsIdentifier*>( fixtureList->GetUserData() );

			if( physId != 0 &&
				physId->GetNumeric2() != -9999 )
			{
				// get the colour
				AddColourToValidList( physId->GetNumeric2() );
				AddShieldPieceData( physId->GetNumeric2() );

				// not set the colour to the mesh
				if( physId != 0 &&
					physId->GetNumeric1() != -9999 )
				{
					if( m_pModelData != 0 )
					{
						math::Vec3 lookupColour = colourIndexList[physId->GetNumeric2()];
						math::Vec4Lite col( static_cast<int>(255.0f*lookupColour.R), static_cast<int>(255.0f*lookupColour.G), static_cast<int>(255.0f*lookupColour.B), 255 );

						m_pModelData->EnableMeshDefaultColour( physId->GetNumeric1() );
						m_pModelData->SetMeshDefaultColour( physId->GetNumeric1(), col );
					}
				}
			}

			fixtureList = fixtureList->GetNext();
		}

		b2MassData md;
		md.center.SetZero();
		md.mass = m_GameData.SHIELD_MASS;
		md.I = m_GameData.SHIELD_MASS_INERTIA;
		m_Body->SetMassData(&md);

		m_Body->SetAwake(true);
		m_Body->SetSleepingAllowed( false );
		m_Body->SetAngularDamping( m_GameData.SHIELD_ANGULAR_DAMPING );

		GameSystems::GetInstance()->SetValidColoursList( m_ValidColours );
	}

	// generate success pool
	m_NextChime = 0;
	m_ChimeUp = true;
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void Player::Draw()
{
	if( m_Body != 0 )
	{
		const b2Transform xfm = m_Body->GetTransform();
		
		m[0] = xfm.q.GetXAxis().x;	m[4] = xfm.q.GetYAxis().x;
		m[1] = xfm.q.GetXAxis().y;	m[5] = xfm.q.GetYAxis().y;
	
		glPushMatrix();

			glMultMatrixf( m );

			if( m_pModelData != 0 )
				m_pModelData->Draw();

		glPopMatrix();
	}
}

/////////////////////////////////////////////////////
/// Method: DrawDebug
/// Params: None
///
/////////////////////////////////////////////////////
void Player::DrawDebug()
{

}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void Player::Update( float deltaTime )
{
	m_LastDelta = deltaTime;

	switch( m_AnimState )
	{
		case AnimState_Initial:
		{

		}break;
		case AnimState_In:
		{
			if( !m_AnimComplete )
			{
				m_AnimTimer -= deltaTime;
				if( m_AnimTimer <= 0.0f )
				{
					m_pModelData->SetMeshDrawState( m_TotalEnabledSubMeshes, true );
					math::Vec4Lite col = m_pModelData->GetMeshDefaultColour( m_TotalEnabledSubMeshes );
					col.A = 255;
					m_pModelData->SetMeshDefaultColour( m_TotalEnabledSubMeshes, col );

						if( m_ShieldInOutIds[m_TotalEnabledSubMeshes] != snd::INVALID_SOUNDBUFFER )
							AudioSystem::GetInstance()->PlayAudio( m_ShieldInOutIds[m_TotalEnabledSubMeshes], math::Vec3(0.0f,0.0f,0.0f), AL_TRUE, AL_FALSE, 1.0f, 1.0f );

					if( m_TotalEnabledSubMeshes < m_NumSubMeshes-1 )
					{
						m_TotalEnabledSubMeshes++;
						m_AnimTimer = m_GameData.SHIELD_PIECE_ANIM_TIME;
						m_AnimComplete = false;
					}
					else
					{
						m_AnimTimer = 0.0f;
						m_AnimState = AnimState_InGame;
						m_AnimComplete = true;
					}
				}
			}
		}break;
		case AnimState_Out:
		{
			if( !m_AnimComplete )
			{
				m_AnimTimer -= deltaTime;
				if( m_AnimTimer <= 0.0f )
				{
					m_pModelData->SetMeshDrawState( m_TotalEnabledSubMeshes, false );
					math::Vec4Lite col = m_pModelData->GetMeshDefaultColour( m_TotalEnabledSubMeshes );
					col.A = 255;
					m_pModelData->SetMeshDefaultColour( m_TotalEnabledSubMeshes, col );

					if( m_ShieldInOutIds[m_TotalEnabledSubMeshes] != snd::INVALID_SOUNDBUFFER )
						AudioSystem::GetInstance()->PlayAudio( m_ShieldInOutIds[m_TotalEnabledSubMeshes], math::Vec3(0.0f,0.0f,0.0f), AL_TRUE, AL_FALSE, 1.0f, 1.0f );

					if( m_TotalEnabledSubMeshes > 0 )
					{					
						m_TotalEnabledSubMeshes--;
						m_AnimTimer = m_GameData.SHIELD_PIECE_ANIM_TIME;
						m_AnimComplete = false;
					}
					else
					{
						m_AnimTimer = 0.0f;
						m_TotalEnabledSubMeshes = 0;
						m_AnimComplete = true;

						m_Body->SetLinearVelocity(b2ZeroVec);
						m_Body->SetTransform(b2ZeroVec, 0.0f);

						if( m_Reset )
						{
							Setup( m_LevelSetupList[m_CurrentLevel] );

							SetAnimState( AnimState_In );
							m_Reset = false;
						}
						else
						{
							if( !m_LevelUpState )
							{
								SetAnimState( AnimState_Initial );
							}
							m_LevelUpState = false;
						}
					}
				}
			}
		}break;
		case AnimState_InGame:
		{
			if( m_FixturesHit.size() != 0 )
			{
				std::vector<EnemyToShieldCollision>::iterator it = m_FixturesHit.begin();
				while( it != m_FixturesHit.end() )
				{
					b2Filter fd;

					fd = (*it).whichFixture->GetFilterData();
					if( fd.maskBits & NOTHING_CATEGORY )
					{

						/*
						// time to turn a shield back on
						(*it).timeDisabled -= deltaTime;
						if( (*it).timeDisabled <= 0.0f )
						{
							// turn shield back on
							fd.groupIndex = SHIELD_GROUP;
							fd.categoryBits = SHIELD_CATEGORY;
							fd.maskBits = ENEMY_CATEGORY;

							(*it).whichFixture->SetFilterData(fd);

							// enable mesh again
							if( (*it).meshIndex != -9999 &&
								m_pModelData != 0 )
							{
								math::Vec4Lite col = m_pModelData->GetMeshDefaultColour( (*it).meshIndex );
								col.A = 255;
								m_pModelData->SetMeshDefaultColour( (*it).meshIndex, col );
								//m_pModelData->SetMeshDrawState( (*it).meshIndex, true );
							}

							// remove
							it = m_FixturesHit.erase( it );
							continue;
						}*/
					}
					else
					{
						// turn shield off
						fd.groupIndex = ENEMY_GROUP;
						fd.categoryBits = ENEMY_CATEGORY;
						fd.maskBits = NOTHING_CATEGORY;	

						(*it).whichFixture->SetFilterData(fd);
					}

					// next
					it++;
				}
			}

			//bool upPress = m_InputSystem.GetUp();
			//bool downPress = m_InputSystem.GetDown();
			//bool leftPress = m_InputSystem.GetLeft();
			//bool rightPress = m_InputSystem.GetRight();

			math::Vec2 srcDims( static_cast<float>( core::app::GetBaseAssetsWidth() ), static_cast<float>( core::app::GetBaseAssetsHeight() ) );
			math::Vec2 scale = renderer::GetAssetScale( static_cast<int>(srcDims.X), static_cast<int>(srcDims.Y) );

			// the bigger the screen the smaller the movement 
			m_RotationForce = m_GameData.SHIELD_ROTATIONAL_FORCE / scale.X;
			

			if( input::gInputState.TouchesData[input::FIRST_TOUCH].bPress ||
				input::gInputState.TouchesData[input::FIRST_TOUCH].bHeld ||
				input::gInputState.TouchesData[input::FIRST_TOUCH].bRelease )
			{
				float moveAmount = static_cast<float>(input::gInputState.TouchesData[input::FIRST_TOUCH].nXDelta);
				// update with movement
				if( std::abs(moveAmount) > m_GameData.SHIELD_MINIMAL_PIXEL_MOVEMENT )
					m_Body->ApplyAngularImpulse( m_RotationForce*moveAmount, true );
			}
			
			if (core::app::IstvOS())
			{
				bool leftPress = m_InputSystem.GetLeft();
				bool rightPress = m_InputSystem.GetRight();

                const float MOVE_MULTIPLY = 12.0f;
				if ( leftPress )
                {
                    float moveAmount = static_cast<float>(input::gInputState.leftValue)*MOVE_MULTIPLY;
                    
                    m_Body->ApplyAngularImpulse(m_RotationForce*moveAmount, true);
                }
				else
				if ( rightPress )
                {
                    float moveAmount = static_cast<float>(input::gInputState.rightValue)*MOVE_MULTIPLY;

                    m_Body->ApplyAngularImpulse(-m_RotationForce*moveAmount, true);
                }
			}

			/*if( upPress || leftPress )
			{
				m_Body->ApplyAngularImpulse( m_RotationForce );
			}
			else if( downPress || rightPress )
			{
				m_Body->ApplyAngularImpulse( -m_RotationForce );
			}*/

			m_Body->SetLinearVelocity(b2ZeroVec);
		}break;
		default:
			DBG_ASSERT(0);
		break;
	}
}

/////////////////////////////////////////////////////
/// Method: LoadShields
/// Params: 
///
/////////////////////////////////////////////////////
void Player::LoadShields()
{
	m_LevelSetupList.clear();
	m_TotalNumLevels = 0;

	// create the box2d shape for the shield
#ifdef _DEBUG
	if( script::LuaFunctionCheck( "LoadShields" ) == 0 )
#endif // _DEBUG
	{
		int result = 0;

		int errorFuncIndex;
		errorFuncIndex = script::GetErrorFuncIndex();

		// get the function
		lua_getglobal( script::LuaScripting::GetState(), "LoadShields" );

		result = lua_pcall( script::LuaScripting::GetState(), 0, 1, errorFuncIndex );

		// LUA_ERRRUN --- a runtime error. 
		// LUA_ERRMEM --- memory allocation error. For such errors, Lua does not call the error handler function. 
		// LUA_ERRERR --- error while running the error handler function. 

		if( result == LUA_ERRRUN || result == LUA_ERRMEM || result == LUA_ERRERR )
		{
			DBGLOG( "PLAYER: *ERROR* Calling function '%s' failed\n", "LoadShields" );
			DBGLOG( "\tLUA_TRACEBACK: %s\n", lua_tostring( script::LuaScripting::GetState(), -1 ) );
			DBG_ASSERT(0);

			return;
		}

		// should be a table of tables
		if( lua_istable( script::LuaScripting::GetState(), -1 ) )
		{
			int i=0;
			int n = luaL_len( script::LuaScripting::GetState(), -1 );

			m_TotalNumLevels = n;

			for( i = 1; i <= n; ++i )
			{
				//int paramIndex = 1;

				// names
				lua_rawgeti( script::LuaScripting::GetState(), -1, i );
				const char* shieldSetup = lua_tostring( script::LuaScripting::GetState(), -1 );

				m_LevelSetupList.push_back( shieldSetup );

				lua_pop( script::LuaScripting::GetState(), 1 );
			}

			lua_pop( script::LuaScripting::GetState(), 1 );
		}
	}
}

/////////////////////////////////////////////////////
/// Method: ClearCurrentShield
/// Params: 
///
/////////////////////////////////////////////////////
void Player::ClearCurrentShield()
{
	if( m_Body != 0 )
	{
		physics::PhysicsIdentifier* pPhysId = 0;

		// cleanup the user data
		b2Fixture* pElement = m_Body->GetFixtureList();
		while( pElement != 0 )
		{
			pPhysId = reinterpret_cast<physics::PhysicsIdentifier* >( pElement->GetUserData() );
			if( pPhysId != 0 &&
				pPhysId != this )
			{
				delete pPhysId;
				pPhysId = 0;
			}
			pElement = pElement->GetNext();
		}

		pPhysId = reinterpret_cast<physics::PhysicsIdentifier* >( m_Body->GetUserData() );
		if( pPhysId != 0 &&  
			pPhysId != this )
		{
			delete pPhysId;
			pPhysId = 0;
		}

		physics::PhysicsWorldB2D::GetWorld()->DestroyJoint( m_Joint );
		m_Joint = 0;

		physics::PhysicsWorldB2D::GetWorld()->DestroyBody( m_Body );
		m_Body = 0;
	}

	if( m_pModelData != 0 )
	{
		res::RemoveModel( m_pModelData );
		m_pModelData = 0;
	}
}

/////////////////////////////////////////////////////
/// Method: AddColourToValidList
/// Params: [in]colourIndex
///
/////////////////////////////////////////////////////
void Player::AddColourToValidList( int colourIndex )
{
	if( m_ValidColours.size() < 0 )
		return;

	std::vector<int>::iterator it = m_ValidColours.begin();

	while( it != m_ValidColours.end() )
	{
		if( (*it) == colourIndex )
			return;

		it++;
	}

	m_ValidColours.push_back(colourIndex);
}

/////////////////////////////////////////////////////
/// Method: RemoveColourFromValidList
/// Params: [in]colourIndex
///
/////////////////////////////////////////////////////
void Player::RemoveColourFromValidList( int colourIndex )
{
	if( m_ValidColours.size() <= 1 )
		return;

	std::vector<int>::iterator it = m_ValidColours.begin();

	while( it != m_ValidColours.end() )
	{
		if( (*it) == colourIndex )
		{
			it = m_ValidColours.erase( it );

			// update 
			GameSystems::GetInstance()->SetValidColoursList( m_ValidColours );
			return;
		}

		it++;
	}

}

/////////////////////////////////////////////////////
/// Method: AddShieldPieceData
/// Params: [in]colourIndex
///
/////////////////////////////////////////////////////
void Player::AddShieldPieceData( int colourIndex )
{
	if( m_ShieldPieceData.size() < 0 )
		return;

	std::vector<ShieldData>::iterator it = m_ShieldPieceData.begin();

	while( it != m_ShieldPieceData.end() )
	{
		if( (*it).colourIndex == colourIndex )
			return;

		it++;
	}

	ShieldData shieldData;
	shieldData.colourIndex = colourIndex;
	shieldData.numAbsorptions = 0;
	shieldData.numDamageHits = 0;

	m_ShieldPieceData.push_back(shieldData);
}

/////////////////////////////////////////////////////
/// Method: AddShieldCollision
/// Params: [in]collision
///
/////////////////////////////////////////////////////
void Player::AddShieldCollision( EnemyToShieldCollision collision )
{
	// take this collision but add all the fixtures with the same data
	if( m_Body != 0 )
	{
		b2Fixture* list = m_Body->GetFixtureList();

		while( list !=0 )
		{
			b2Filter fd;

			physics::PhysicsIdentifier* physId = reinterpret_cast<physics::PhysicsIdentifier*>( list->GetUserData() );
			if( physId != 0 )
			{
				int num1 = physId->GetNumeric1();
				int num2 = physId->GetNumeric2();
				//int num3 = physId->GetNumeric3();
				//int num4 = physId->GetNumeric4();
				//int num5 = physId->GetNumeric5();

				// find the colour index in the fixtures
				if( num2 != -9999 &&
					num2 == collision.colourIndex )
				{
					// disable the mesh now (put it here so it only happens once)
					if( num1 != -9999 &&
						m_pModelData != 0 )
					{
						math::Vec4Lite col = m_pModelData->GetMeshDefaultColour( num1 );
						col.A = 32;
						m_pModelData->SetMeshDefaultColour( num1, col );
						//m_pModelData->SetMeshDrawState( num1, false );
					}

					EnemyToShieldCollision newCollision;
					newCollision.meshIndex = num1;
					newCollision.colourIndex = num2;
					newCollision.whichFixture = list;
					newCollision.timeDisabled = m_GameData.SHIELD_DISABLE_TIME;

					// remove colour from valid colour list
					RemoveColourFromValidList( num2 );

					m_FixturesHit.push_back(newCollision);
				}
			}

			// next
			list = list->GetNext();
		}
	}
}

/////////////////////////////////////////////////////
/// Method: AddShieldAbsorb
/// Params: 
///
/////////////////////////////////////////////////////
void Player::AddShieldAbsorb( int colourIndex )
{
	if( m_ShieldPieceData.size() < 0 )
		return;

	std::vector<ShieldData>::iterator it = m_ShieldPieceData.begin();

	while( it != m_ShieldPieceData.end() )
	{
		if( (*it).colourIndex == colourIndex )
		{
			(*it).numAbsorptions++;

			if( GameSystems::GetInstance()->GetLevelCounter() > m_GameData.LEVEL_SPEED_UP_START )
				IncreaseNumProtections();

			return;
		}

		// next
		it++;
	}
}

/////////////////////////////////////////////////////
/// Method: AddShieldDeflect
/// Params: 
///
/////////////////////////////////////////////////////
void Player::AddShieldDeflect( int colourIndex )
{
	if( m_ShieldPieceData.size() < 0 )
		return;

	std::vector<ShieldData>::iterator it = m_ShieldPieceData.begin();

	while( it != m_ShieldPieceData.end() )
	{
		if( (*it).colourIndex == colourIndex )
		{
			(*it).numAbsorptions = 0;
			(*it).numDamageHits++;
			return;
		}

		// next
		it++;
	}
}

/////////////////////////////////////////////////////
/// Method: NextLevel
/// Params: 
///
/////////////////////////////////////////////////////
void Player::NextLevel()											
{
	m_CurrentLevel++;

	if( m_CurrentLevel >= m_TotalNumLevels )
	{
		GameSystems::GetInstance()->IncrementWrapAroundCounter();

		m_CurrentLevel = 0;

		GameSystems::GetInstance()->AddTextToConsole( "..." );
		GameSystems::GetInstance()->AddTextToConsole( "......routine complete...." );
		GameSystems::GetInstance()->AddTextToConsole( "...big bonus...." );

		GameSystems::GetInstance()->AddToScore( m_GameData.COMPLETE_ROUTINE );
	}

	Setup( m_LevelSetupList[m_CurrentLevel] );
}

/////////////////////////////////////////////////////
/// Method: PreviousLevel
/// Params: 
///
/////////////////////////////////////////////////////
void Player::PreviousLevel()
{
	m_CurrentLevel--;

	if( m_CurrentLevel < 0 )
		m_CurrentLevel = m_TotalNumLevels-1;

	Setup( m_LevelSetupList[m_CurrentLevel] );
}

/////////////////////////////////////////////////////
/// Method: Reset
/// Params: 
///
/////////////////////////////////////////////////////
void Player::Reset()
{
	m_FixturesHit.clear();

	GameSystems::GetInstance()->ResetWrapAroundCounter();

	m_CurrentLevel = 0;
	m_NumProtections = 0;
	m_NumConsecutiveProtections = 0;
	m_ConsecutiveMulti = 1;

	if( m_AnimState == AnimState_Initial )
	{
		m_Reset = false;
		SetAnimState( AnimState_In );
	}
	else
	{
		m_Reset = true;
		SetAnimState( AnimState_Out );
	}
	m_NextChime = 0;
	m_PitchChange = 0.0f;
}

/////////////////////////////////////////////////////
/// Method: SetAnimState
/// Params: [in]state
///
/////////////////////////////////////////////////////
void Player::SetAnimState( AnimState state )
{
	m_AnimState = state;

	m_AnimTimer = m_GameData.SHIELD_PIECE_ANIM_TIME;
	m_AnimComplete = false;

	m_FixturesHit.clear();
}

/////////////////////////////////////////////////////
/// Method: PlayPositiveChime
/// Params: None
///
/////////////////////////////////////////////////////
void Player::PlayPositiveChime()
{
	// play success
	if( m_CurrentSuccessList[m_NextChime] != snd::INVALID_SOUNDBUFFER )
	{
		AudioSystem::GetInstance()->PlayAudio( m_CurrentSuccessList[m_NextChime], math::Vec3(0.0f,0.0f,0.0f), AL_TRUE, AL_FALSE, 1.0f );

		if( m_ChimeUp )
		{
			m_NextChime++;
			if( m_NextChime > AUDIO_CHIME_SUCCESS_POOL-1 )
			{
				m_NextChime = AUDIO_CHIME_SUCCESS_POOL-1;
				m_ChimeUp = false;
			}
		}
		else
		{
			m_NextChime--;
			if( m_NextChime < 0 )
			{
				m_NextChime = 0;
				m_ChimeUp = true;
			}
		}
	}	
}

/////////////////////////////////////////////////////
/// Method: PlayNegativeChime
/// Params: None
///
/////////////////////////////////////////////////////
void Player::PlayNegativeChime()
{
	// play failure
	if( m_CurrentSuccessList[m_NextChime] != snd::INVALID_SOUNDBUFFER )
	{
		float randomPitch = math::RandomNumber( -0.5f, 0.5f );
		AudioSystem::GetInstance()->PlayAudio( m_CurrentSuccessList[m_NextChime], math::Vec3(0.0f,0.0f,0.0f), AL_TRUE, AL_FALSE, 1.0f+randomPitch );
	}
}

/////////////////////////////////////////////////////
/// Method: ResetConsecutiveCount
/// Params: None
///
/////////////////////////////////////////////////////
void Player::ResetConsecutiveCount()
{
	m_NumConsecutiveProtections = 0;
	m_ConsecutiveMulti = 1;
}

/////////////////////////////////////////////////////
/// Method: IncreaseConsecutiveCount
/// Params: None
///
/////////////////////////////////////////////////////
void Player::IncreaseConsecutiveCount()
{
	m_NumConsecutiveProtections++;
	GameSystems::GetInstance()->AddToScore( m_NumConsecutiveProtections );

	if( m_NumConsecutiveProtections % 10 == 0 )
	{
		GameSystems::GetInstance()->AddTextToConsole( "%d packets in a row........", m_NumConsecutiveProtections );
		GameSystems::GetInstance()->AddTextToConsole( "bonus...%d........", m_GameData.CONSECUTIVE_BONUS * m_ConsecutiveMulti );
		GameSystems::GetInstance()->AddTextToConsole( "<3" );

		GameSystems::GetInstance()->AddToScore( m_GameData.CONSECUTIVE_BONUS * m_ConsecutiveMulti );
		m_ConsecutiveMulti++;
	}
}

