
/*===================================================================
	File: Enemy.cpp
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
#include "Enemy/Enemy.h"

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
Enemy::Enemy( EnemyManager* manager )
	: PhysicsIdentifier( PHYSICSBASICID_ENEMY, PHYSICSCASTID_ENEMY)
{
	m_Manager = manager;

	m_pScriptData = GetScriptDataHolder();
	m_GameData = m_pScriptData->GetGameData();
	m_DevData = m_pScriptData->GetDevData();

	m_LastDelta = 0.0f;
	m_pModelData = 0;
	m_SubMesh = 0;

	m_Body = 0;
	m_StartPosition.setZero();

	m_IsEnabled = false;
	m_HasRebound = false;

	m_ColourIndex = -1;
	m_Colour = math::Vec3( 1.0f, 1.0f, 1.0f );

	m_Flags = 0;

	m_Type = ENEMYTYPE_BITS_GENERIC;
	m_Damage = 1;
	m_Points = 1;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
Enemy::~Enemy()
{
	m_pModelData = 0;
}

/////////////////////////////////////////////////////
/// Method: Setup
/// Params: None
///
/////////////////////////////////////////////////////
void Enemy::Setup()
{
	// create the box2d shape for the shield
	b2BodyDef bd;
	bd.type = b2_dynamicBody;
	bd.gravityScale = 0.0f;
	bd.awake = false;
	bd.fixedRotation = false;
	bd.position = m_BodyPos = b2Vec2( m_StartPosition.X, m_StartPosition.Y );
	
	m_CurrentPosition = m_StartPosition;

	// setup physics
	m_Body = physics::PhysicsWorldB2D::GetWorld()->CreateBody(&bd);
	DBG_ASSERT_MSG( (m_Body != 0), "Could not create enemy physics body" );
	
	b2FixtureDef fd;
	fd.friction = 0.0f;
	fd.restitution = 1.0f;
	fd.density = 0.1f;

	fd.filter.groupIndex = ENEMY_GROUP;
	fd.filter.categoryBits = ENEMY_CATEGORY;
	fd.filter.maskBits = NOTHING_CATEGORY;	

	b2CircleShape circleShape;
	circleShape.m_p = b2Vec2( 0.0f, 0.0f );
	circleShape.m_radius = m_TypeData.radius;

	fd.shape = &circleShape;

	b2Fixture* pFixture = 0;
	pFixture = m_Body->CreateFixture( &fd );
	DBG_ASSERT_MSG( (pFixture != 0), "Could not create enemy physics fixture" );

	pFixture->SetUserData( reinterpret_cast<void *>(this) );

	b2MassData md;
	md.center.SetZero();
	md.mass = m_GameData.ENEMY_MASS;
	md.I = m_GameData.ENEMY_INERTIA;
	m_Body->SetMassData(&md);

	m_Body->SetUserData( reinterpret_cast<void *>(this) );

	m_Body->SetAwake(false);
	m_Body->SetSleepingAllowed(true);
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void Enemy::Draw()
{
	renderer::OpenGL::GetInstance()->SetColour4f( m_Colour.R, m_Colour.G, m_Colour.B, 1.0f );

	if( m_Body != 0 )
	{
		const b2Transform xfm = m_Body->GetTransform();
		
		m[0] = xfm.q.GetXAxis().x;	m[4] = xfm.q.GetYAxis().x;
		m[1] = xfm.q.GetXAxis().y;	m[5] = xfm.q.GetYAxis().y;
	
		m_CurrentPosition.X = xfm.p.x;
		m_CurrentPosition.Y = xfm.p.y;

		m[12] = m_CurrentPosition.X;
		m[13] = m_CurrentPosition.Y;
		m[14] = -1.0f;

		glPushMatrix();

			glMultMatrixf( m );

			if( m_pModelData != 0 )
			{
				m_pModelData->SetMeshDrawState( -1, false );
				m_pModelData->SetMeshDrawState( m_SubMesh, true );

				m_pModelData->Draw();
			}

		glPopMatrix();
	}
}

/////////////////////////////////////////////////////
/// Method: DrawDebug
/// Params: None
///
/////////////////////////////////////////////////////
void Enemy::DrawDebug()
{

}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void Enemy::Update( float deltaTime )
{
	m_LastDelta = deltaTime;

	if( m_Flags != 0 )
	{
		if( m_Flags & ENEMYFLAG_DESTROY )
		{
			Disable();
		}
		else if( m_Flags & ENEMYFLAG_ABSORB )
		{
			Disable();
		}
		else if( m_Flags & ENEMYFLAG_REBOUND_GOOD /*||
			m_Flags & ENEMYFLAG_REBOUND_BAD*/ )
		{
			if( m_Body != 0 )
			{
				b2Fixture* pFixture = 0;
				pFixture = m_Body->GetFixtureList();

				// change the fixture to collide with nothing
				while( pFixture != 0 )
				{
					b2Filter fd;
					fd.groupIndex = ENEMY_GROUP;
					fd.categoryBits = ENEMY_CATEGORY;
					fd.maskBits = NOTHING_CATEGORY;	

					pFixture->SetFilterData(fd);

					pFixture = pFixture->GetNext();
				}

				m_IsEnabled = true;
				m_HasRebound = true;

				m_ColourIndex = -1;

				if( m_Flags & ENEMYFLAG_REBOUND_GOOD )
				{
					GameSystems::GetInstance()->SpawnExplosion( m_CurrentPosition, m_Colour );
					m_Colour = math::Vec3( 1.0f, 1.0f, 1.0f );
				}
				else
				{
					GameSystems::GetInstance()->SpawnExplosion( m_CurrentPosition, m_Colour );
					m_Colour = math::Vec3( 0.5f, 0.5f, 0.5f );
				}

				SetNumeric1( m_ColourIndex );

				// flick rotation
				if( m_Type == ENEMYTYPE_BULLET )
				{
					b2Vec2 linVel = m_Body->GetLinearVelocity();
					math::Vec3 dir( linVel.x, linVel.y, 0.0f );

					float angle = math::AngleBetweenXY( zeroVec, dir );
					m_Body->SetTransform( m_BodyPos, -angle + math::DegToRad(180.0f) );
				}
			}
		}
		else if( m_Flags & ENEMYFLAG_BAD_MATCH )
		{
			if( m_Body != 0 )
			{
				b2Fixture* pFixture = 0;
				pFixture = m_Body->GetFixtureList();

				// fixture should continue
				while( pFixture != 0 )
				{
					b2Filter fd;
					fd.groupIndex = SHIELD_GROUP;
					fd.categoryBits = ENEMY_CATEGORY;
					fd.maskBits = CORE_CATEGORY;	

					pFixture->SetFilterData(fd);

					pFixture = pFixture->GetNext();
				}

				m_IsEnabled = true;
				m_HasRebound = false;

				m_ColourIndex = -1;

				/*if( m_Flags & ENEMYFLAG_REBOUND_GOOD )
				{
					GameSystems::GetInstance()->SpawnExplosion( m_CurrentPosition, m_Colour );
					m_Colour = math::Vec3( 1.0f, 1.0f, 1.0f );
				}
				else
				{
					GameSystems::GetInstance()->SpawnExplosion( m_CurrentPosition, m_Colour );
					m_Colour = math::Vec3( 0.5f, 0.5f, 0.5f );
				}*/

				m_Colour = math::Vec3( 0.5f, 0.5f, 0.5f );

				SetNumeric1( m_ColourIndex );

				// flick rotation
				/*if( m_Type == ENEMYTYPE_BULLET )
				{
					b2Vec2 linVel = m_Body->GetLinearVelocity();
					math::Vec3 dir( linVel.x, linVel.y, 0.0f );

					float angle = math::AngleBetweenXY( zeroVec, dir );
					m_Body->SetTransform( m_BodyPos, -angle + math::DegToRad(180.0f) );
				}*/
			}
		}

		m_Flags = 0;
	}

	if( m_IsEnabled &&
		!m_Manager->IsPaused() )
	{
		// trojan colour swap
		if( m_Type == ENEMYTYPE_TROJAN &&
			!m_HasRebound )
		{
			if( !m_TrojanStopColourChange )
			{
				m_TrojanColourSwapTime -= deltaTime;
				if( m_TrojanColourSwapTime < 0.0f )
				{
					m_TrojanColourSwapTime = m_GameData.ENEMY_TROJAN_COLOUR_SWAP_TIME;

					std::vector<int> validColours = GameSystems::GetInstance()->GetValidColoursList();

					int idx = math::RandomNumber(0, static_cast<int>(validColours.size()-1));

					// swap colour
					m_ColourIndex = validColours[idx];
					m_Colour = GameSystems::GetInstance()->GetColourForIndex( validColours[idx] );

					SetNumeric1( m_ColourIndex );

					/*GLfloat *mdl = renderer::OpenGL::GetInstance()->GetModelViewMatrix();
					GLfloat *proj = renderer::OpenGL::GetInstance()->GetProjectionMatrix();
					GLint *viewport = renderer::OpenGL::GetInstance()->GetViewport();

					gluUtil::gluProjectf( m_CurrentPosition.X,  m_CurrentPosition.Y,  m_CurrentPosition.Z,
											mdl, proj, viewport,
											&m_TrojanUnprojectedPoint.X, &m_TrojanUnprojectedPoint.Y, &m_TrojanUnprojectedPoint.Z );*/
				}

				// check if trojan is close enough to core to automatically stop colour change
				float distanceToCore = std::abs( m_CurrentPosition.length() );
				if( distanceToCore <= m_GameData.ENEMY_TROJAN_COLOUR_SWAP_STOP_RADIUS )
				{
					// close enough, stop changing colour
					m_TrojanStopColourChange = true;

					GameSystems::GetInstance()->AddTextToConsole( "Trojan identification complete......" );
					GameSystems::GetInstance()->AddTextToConsole( "........." );
				}
			}
		}
		else
		if( m_Type == ENEMYTYPE_VIRUS )
		{
			switch( m_VirusState )
			{
				case EnemyVirusState_MoveToPosition:
				{
					// check if virus is close enough to core to automatically stop colour change
					float circleDistance = std::abs( m_CurrentPosition.length() );
					if( circleDistance <= m_VirusCircleDistance )
					{
						m_VirusState = EnemyVirusState_CircleCore;
						if( m_Body != 0 )
						{
							m_Body->SetLinearVelocity( b2ZeroVec );
							m_Body->SetAngularVelocity( 0.0f );
						}

						// generate the positional target
						m_VirusAttackAngle = math::DegToRad( math::RandomNumber( 0.0f, 359.0f ) );	
					}
				}break;
				case EnemyVirusState_CircleCore:
				{
					// move the angle
					if( m_VirusAttackAngle > m_VirusAngleAroundCore )
					{
						m_VirusAngleAroundCore += math::DegToRad(m_GameData.ENEMY_VIRUS_CIRCLE_SPEED)*deltaTime;
					}
					else
					{
						m_VirusAngleAroundCore -= math::DegToRad(m_GameData.ENEMY_VIRUS_CIRCLE_SPEED)*deltaTime;
					}

					// rotate the body to always face the core
					m_CurrentPosition = math::Vec3(std::cos(m_VirusAngleAroundCore), std::sin(m_VirusAngleAroundCore), 0.0f ) * m_VirusCircleDistance;
					m_BodyPos = b2Vec2( m_CurrentPosition.X, m_CurrentPosition.Y );
					if( m_Body != 0 )
					{
						m_VirusBodyRotation = math::AngleBetweenXY( zeroVec, m_CurrentPosition );
						m_Body->SetTransform( m_BodyPos, -m_VirusBodyRotation );
					}

					// arrived at angle?
					if( math::WithinTolerance(m_VirusAngleAroundCore, m_VirusAttackAngle, 0.5f ) )
					{
						m_VirusState = EnemyVirusState_AttackCore;
						m_BulletFireTime = m_GameData.ENEMY_VIRUS_BULLET_FIRE_TIME;
					}
				}break;
				case EnemyVirusState_AttackCore:
				{
					// spawn bullet or die
					m_BulletFireTime -= deltaTime;

					if( m_BulletFireTime <= 0.0f )
					{
						// generate the positional target
						m_VirusAttackAngle += math::DegToRad( math::RandomNumber( 90.0f, 180.0f ) );	
						m_VirusState = EnemyVirusState_CircleCore;

						// colour should be random
						std::vector<int> validColours = GameSystems::GetInstance()->GetValidColoursList();

						//int idx = math::RandomNumber(0,validColours.size()-1);

						if( m_Manager != 0 )
						{
							// random bullet colour
							//m_Manager->SpawnBullet( m_CurrentPosition, m_VirusBodyRotation, validColours[idx], GameSystems::GetInstance()->GetColourForIndex( validColours[idx] ) );

							// matching colour
							m_Manager->SpawnBullet( m_CurrentPosition, m_VirusBodyRotation, m_ColourIndex, GameSystems::GetInstance()->GetColourForIndex( m_ColourIndex ) );

							GameSystems::GetInstance()->AddTextToConsole( "..Virus attack vector detected...." );
							GameSystems::GetInstance()->AddTextToConsole( "...Standby......" );
							GameSystems::GetInstance()->AddTextToConsole( "............." );
						}

						m_BulletFireTime = m_GameData.ENEMY_VIRUS_BULLET_FIRE_TIME;
						m_VirusBulletsFired++;

						if( m_VirusBulletsFired >= m_GameData.ENEMY_VIRUS_BULLETS )
							m_VirusState = EnemyVirusState_End;
					}
				}break;
				case EnemyVirusState_End:
				{
					// explode and give points
					GameSystems::GetInstance()->SpawnExplosion( m_CurrentPosition, m_Colour );

					GameSystems::GetInstance()->AddToScore( GetPoints() );

					GameSystems::GetInstance()->AddTextToConsole( "..Virus quaranteed...." );
					GameSystems::GetInstance()->AddTextToConsole( "......" );

					// exit
					Disable();
				}break;
				default:
					DBG_ASSERT(0);
				break;
			}
		}
	}

	if( m_Body != 0 )
		m_BodyPos = m_Body->GetPosition();

	// get out of bounds
	if( m_IsEnabled )
	{
		if( m_BodyPos.x >= m_GameData.ENEMY_RESET_BOUND_X ||
			m_BodyPos.x <= -m_GameData.ENEMY_RESET_BOUND_X )
			Disable();
		if( m_BodyPos.y >= m_GameData.ENEMY_RESET_BOUND_Y ||
			m_BodyPos.y <= -m_GameData.ENEMY_RESET_BOUND_Y )
			Disable();
	}
}

/////////////////////////////////////////////////////
/// Method: Enable
/// Params: None
///
/////////////////////////////////////////////////////
void Enemy::Enable()
{
	if( m_Body != 0 )
	{
		b2Fixture* pFixture = 0;
		pFixture = m_Body->GetFixtureList();

		m_Body->SetLinearVelocity( b2ZeroVec );
		m_Body->SetAngularVelocity( 0.0f );

		while( pFixture != 0 )
		{
			b2Filter fd;
			fd.groupIndex = ENEMY_GROUP;
			fd.categoryBits = ENEMY_CATEGORY;
			fd.maskBits = CORE_CATEGORY | SHIELD_CATEGORY;	

			pFixture->SetFilterData(fd);

			pFixture = pFixture->GetNext();
		}

		m_IsEnabled = true;
		m_HasRebound = false;
	}
}

/////////////////////////////////////////////////////
/// Method: Disable
/// Params: None
///
/////////////////////////////////////////////////////
void Enemy::Disable()
{
	if( m_Body != 0 )
	{
		b2Fixture* pFixture = 0;
		pFixture = m_Body->GetFixtureList();

		m_BodyPos = b2Vec2( -10000.0f, -10000.0f );
		m_CurrentPosition.X = m_BodyPos.x;
		m_CurrentPosition.Y = m_BodyPos.y;
		m_Body->SetTransform( m_BodyPos, 0.0f );

		m_Body->SetLinearVelocity( b2ZeroVec );
		m_Body->SetAngularVelocity( 0.0f );

		while( pFixture != 0 )
		{
			b2Filter fd;
			fd.groupIndex = ENEMY_GROUP;
			fd.categoryBits = ENEMY_CATEGORY;
			fd.maskBits = NOTHING_CATEGORY;	

			pFixture->SetFilterData(fd);

			pFixture = pFixture->GetNext();
		}

		m_IsEnabled = false;
		m_HasRebound = false;

		m_ColourIndex = -1;
		m_Colour = math::Vec3( 1.0f, 1.0f, 1.0f );

		SetNumeric1( m_ColourIndex );
	}
}

/////////////////////////////////////////////////////
/// Method: Attack
/// Params: None
///
/////////////////////////////////////////////////////
void Enemy::Attack( const EnemyTypeData& typeData, int colourIndex, const math::Vec3& colour )
{
	m_ColourIndex = colourIndex;
	m_Colour = colour;

	m_TypeData = typeData;
	m_Type = m_TypeData.typeId;
	m_pModelData = m_TypeData.modelData;
	m_Points = m_TypeData.matchPoints;

	m_SubMesh = math::RandomNumber( 0, m_pModelData->GetNumOpaqueMeshes()-1 );

	m_IsEnabled = false;
	m_HasRebound = false;
	m_Flags = 0;

	Setup();

	float force = m_GameData.ENEMY_ATTACK_FORCE;

	if( GameSystems::GetInstance()->GetLevelCounter() > m_GameData.LEVEL_SPEED_UP_START )
	{
		if( m_TypeData.typeId <= ENEMYTYPE_TROJAN )
		{
			//int numProtections = GameSystems::GetInstance()->GetPlayer().GetNumProtections();
			float val = static_cast<float>(GameSystems::GetInstance()->GetLevelCounter() - m_GameData.LEVEL_SPEED_UP_START);
			force = m_GameData.ENEMY_ATTACK_FORCE + (val*m_GameData.ENEMY_ATTACK_INC);
		}
	}

	if( force > m_GameData.ENEMY_ATTACK_MAX_FORCE )
		force = m_GameData.ENEMY_ATTACK_MAX_FORCE;

	if( m_Body != 0 )
	{
		SetNumeric1( m_ColourIndex );
		Enable();

		math::Vec3 pos = -m_StartPosition;
		pos.normalise();

		b2Vec2 finalForce( (pos.X*force), (pos.Y*force) );

		m_Body->SetActive(true);
		m_Body->SetAwake(true);

		m_Body->ApplyLinearImpulse( finalForce, b2ZeroVec, true );

		if( m_Type != ENEMYTYPE_VIRUS &&
			m_Type != ENEMYTYPE_BULLET )
			m_Body->ApplyAngularImpulse( math::RandomNumber(m_GameData.ENEMY_RANDOM_ANGULAR_MIN, m_GameData.ENEMY_RANDOM_ANGULAR_MAX), true );
	}

	m_TrojanColourSwapTime = m_GameData.ENEMY_TROJAN_COLOUR_SWAP_TIME;
	m_TrojanStopColourChange = false;
	m_TrojanUnprojectedPoint = math::Vec3( -100000.0f, -100000.0f, -100000.0f );

	m_VirusState = EnemyVirusState_MoveToPosition;
	m_VirusBodyRotation = math::AngleBetweenXY( zeroVec, m_StartPosition );
	m_VirusAngleAroundCore = m_StartAngle;
	m_VirusBulletsFired = 0;
	m_VirusCircleDistance = math::RandomNumber( m_GameData.ENEMY_VIRUS_CIRCLE_DISTANCE_MIN, m_GameData.ENEMY_VIRUS_CIRCLE_DISTANCE_MAX );
	m_VirusAttackAngle = 0.0f;

	if( m_Type == ENEMYTYPE_VIRUS ||
		m_Type == ENEMYTYPE_BULLET )
	{
		if( m_Body != 0 )
		{
			m_Body->SetTransform( m_BodyPos, -m_VirusBodyRotation );
		}
	}
}

/////////////////////////////////////////////////////
/// Method: Destroy
/// Params: None
///
/////////////////////////////////////////////////////
void Enemy::Destroy()
{
	Disable();
}

/////////////////////////////////////////////////////
/// Method: SetFlags
/// Params: None
///
/////////////////////////////////////////////////////
void Enemy::SetFlags( unsigned long flag, bool addFlags )
{
	if( addFlags )
		m_Flags |= flag;
	else
		m_Flags = flag;
}
