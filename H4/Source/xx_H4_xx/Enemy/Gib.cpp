
/*===================================================================
	File: Gib.h
	Game: H4

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"

#include "MathBase.h"
#include "RenderBase.h"
#include "SoundBase.h"
#include "CollisionBase.h"
#include "ModelBase.h"
#include "InputBase.h"
#include "ScriptBase.h"
#include "PhysicsBase.h"

#include "AppConsts.h"

#include "Physics/PhysicsContact.h"
#include "GameSystems.h"

#include "Enemy/Gib.h"

namespace
{
	b2Vec2 zeroVec(0.0f, 0.0f);

	const float FAKE_GRAV = -2.0f;
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
Gib::Gib()
	: PhysicsIdentifier( PHYSICSBASICID_GIB, PHYSICSCASTID_GIB )
{
	m_Body = 0;
	m_IsActive = false;

	m_FirstFloorHit = true;
	m_NextSplatIndex = 0;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
Gib::~Gib()
{
	m_pModel = 0;

	if( m_Body != 0 )
	{
		if( physics::PhysicsWorldB2D::GetWorld() )
			physics::PhysicsWorldB2D::GetWorld()->DestroyBody(m_Body);
		m_Body = 0;
	}
}

/////////////////////////////////////////////////////
/// Method: SpawnGib
/// Params: [in]pos
///
/////////////////////////////////////////////////////
void Gib::SpawnGib( mdl::ModelHGM* pModel, const math::Vec3& pos, float radius )
{
	m_pModel = pModel;

	b2BodyDef bd;
	bd.type = b2_dynamicBody;
	bd.gravityScale = 0.0f;
	bd.awake = true;
	bd.fixedRotation = true;
	bd.bullet = false;
	bd.linearDamping = 1.0f;

	// setup physics
	m_Body = physics::PhysicsWorldB2D::GetWorld()->CreateBody(&bd);
	DBG_ASSERT_MSG( (m_Body != 0), "Could not create physics body for gib" );

	b2CircleShape circleShape;
	b2FixtureDef fd;

	fd.friction = 1.0f;
	fd.restitution = 0.2f;
	fd.density = 100.0f;

	fd.filter.groupIndex = GIB_GROUP;
	fd.filter.categoryBits = GIB_CATEGORY;
	fd.filter.maskBits = WORLDHIGH_CATEGORY | WORLDLOW_CATEGORY;

	circleShape.m_p = zeroVec;
	circleShape.m_radius = radius;

	fd.shape = &circleShape;

	b2Fixture* pFixture = 0;
	pFixture = m_Body->CreateFixture( &fd );
	pFixture->SetUserData( reinterpret_cast<void *>(this) );

	// mass setup
	b2MassData massData; 
	massData.mass = 2.0f;
	massData.center = zeroVec;
	massData.I = 1.0f;
	
	m_Body->SetMassData( &massData );
	m_Body->SetUserData( reinterpret_cast<void *>(this) );		

	if( m_Body != 0 )
	{
		m_Body->SetTransform( b2Vec2(pos.X, pos.Z), 0.0f );	
		m_Body->SetLinearVelocity( zeroVec );
		m_Body->SetAngularVelocity( 0.0f );
	}

	m_Pos.Y = math::RandomNumber( 1.0f, 1.5f );
	m_Rotation.Y = math::RandomNumber( 0.0f, 360.0f );

	m_BoundSphere.vCenterPoint = pos;
	m_BoundSphere.fRadius = radius;
	
	b2Vec2 impulse( math::RandomNumber( -10.0f, 10.0f ), math::RandomNumber( -10.0f, 10.0f ) );
	m_Vel.Y = math::RandomNumber( 0.2f, 0.2f );

	m_FakeGrav = FAKE_GRAV;

	m_Body->ApplyLinearImpulse( impulse, zeroVec, true );

	m_IsActive = true;
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params:
///
/////////////////////////////////////////////////////
void Gib::Draw()
{
	if( m_IsActive )
	{
		glPushMatrix();
			glTranslatef( m_BoundSphere.vCenterPoint.X, m_BoundSphere.vCenterPoint.Y, m_BoundSphere.vCenterPoint.Z );
			glRotatef( m_Rotation.Y, 0.0f, 1.0f, 0.0f );
		//	renderer::DrawSphere( m_BoundSphere.fRadius );
			m_pModel->Draw();
		glPopMatrix();
	}
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void Gib::Update( float deltaTime )
{
	if( m_IsActive )
	{
		b2Vec2 bodyPos = m_Body->GetWorldPoint(zeroVec);
		b2Vec2 linVel = m_Body->GetLinearVelocity();

		//if( m_FakeGrav > FAKE_GRAV )
		//	m_FakeGrav -= 5.0f*deltaTime;

		if( m_Vel.Y > 0.0f )
			m_Vel.Y += (m_FakeGrav*0.75f)*deltaTime;
		else
			m_Vel.Y = m_FakeGrav*deltaTime;
	
		m_Pos.X = bodyPos.x;
		m_Pos.Y += m_Vel.Y;
		m_Pos.Z = bodyPos.y;

		if( m_Pos.Y < m_BoundSphere.fRadius )
		{
			m_Pos.Y = m_BoundSphere.fRadius;

			if( m_FirstFloorHit &&
				m_NextSplatIndex < MAX_ENEMY_BLOOD_SPLATS )
			{
				float floorOffset = 0.005f;
				math::Vec3 pos = math::Vec3( m_Pos.X, floorOffset, m_Pos.Z );
				math::Vec3 normal = math::Vec3( 0.0f, 1.0f, 0.0f );

				GameSystems::GetInstance()->GetDecalSystem().AddBloodSplat( true, pos, normal );

				m_NextSplatIndex++;
				m_FirstFloorHit = false;
			}
		}
		//if( m_Pos.Y <= 0.0f && ((linVel.x != 0.0f) || (linVel.y != 0.0f)) )
		//	m_FakeGrav = 1.0f;
	}

	m_BoundSphere.vCenterPoint = m_Pos;
}

/////////////////////////////////////////////////////
/// Method: HandleContact
/// Params: [in]idNum, [in]castId, [in]point, [in]pOtherShape, [in]pOtherBody
///
/////////////////////////////////////////////////////
void Gib::HandleContact( int idNum, int castId, const ContactPoint* contact, const b2Fixture* pOtherFixture, const b2Body* pOtherBody )
{
	int numeric1 = -9999;
	//int numeric2 = -9999;
	//int numeric3 = -9999;
	//int numeric4 = -9999;
	//int numeric5 = -9999;

	switch( idNum )
	{
		case PHYSICSBASICID_WORLD:
		{
			physics::PhysicsIdentifier* pUserData = 0;
			if( pOtherBody != 0 )
			{
				pUserData = reinterpret_cast<physics::PhysicsIdentifier*>( pOtherFixture->GetUserData() );
				DBG_ASSERT_MSG( (pUserData != 0), "Could not get physics user data on gib world contact" );
				
				numeric1 = pUserData->GetNumeric1();
				//numeric2 = pUserData->GetNumeric2();
				//numeric3 = pUserData->GetNumeric3();
				//numeric4 = pUserData->GetNumeric4();
				//numeric5 = pUserData->GetNumeric5();
			}
			
			// draw splat
			if( (numeric1 == NUMERIC1_WORLD_HIGHWALL || numeric1 == -9999) )
			{
				if( m_NextSplatIndex < MAX_ENEMY_BLOOD_SPLATS )
				{
					math::Vec3 decalPoint = math::Vec3( contact->position.x, m_Pos.Y, contact->position.y );
					math::Vec3 decalNormal = math::Vec3( contact->normal.x, 0.0f, contact->normal.y );

					// move decal away from wall
					decalPoint.X += (BLOOD_DECAL_OFFSET) * decalNormal.X;
					decalPoint.Z += (BLOOD_DECAL_OFFSET) * decalNormal.Z;

					GameSystems::GetInstance()->GetDecalSystem().AddBloodSplat( false, decalPoint, decalNormal );
					m_NextSplatIndex++;
				}
			}
		}break;

		default:
			// colliding with something that doesn't have an id
			DBG_ASSERT_MSG( 0, "Gib colliding with object that has no physics id" );
		break;
	}
}