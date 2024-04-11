
/*===================================================================
	File: Bullet.cpp
	Game: H4

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "PhysicsBase.h"
#include "CollisionBase.h"
#include "ModelBase.h"
#include "SoundBase.h"

#include "Physics/PhysicsIds.h"
#include "Physics/PhysicsContact.h"

#include "Resources/SoundResources.h"

#include "GameSystems.h"

#include "Player/Bullet.h"

namespace
{
	const float BULLET_HEIGHT_LOW = 1.45f;
	const float BULLET_HEIGHT_HEIGHT = 1.55f;

	const float BULLET_COLLISION_RADIUS = 0.08f;

	math::Vec3 vaPoints[8];

}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
Bullet::Bullet()
	: PhysicsIdentifier( PHYSICSBASICID_BULLET, PHYSICSCASTID_BULLET )
{
	m_Active = false;
	m_Body = 0;

	m_YPos = 0.0f;
	m_YImpulse = 0.0f;
	m_Rot = 0.0f;

	m_Collision.fRadius = BULLET_COLLISION_RADIUS;

	// create overlapping quads
	float depth = 0.0625f;
	float height = 0.0625f;

	vaPoints[0] = math::Vec3( 0.0f, -height, -depth );
	vaPoints[1] = math::Vec3( 0.0f, -height, depth );
	vaPoints[2] = math::Vec3( 0.0f, height, -depth );
	vaPoints[3] = math::Vec3( 0.0f, height, depth );

	vaPoints[4] = math::Vec3( -height, 0.0f, -depth );
	vaPoints[5] = math::Vec3( -height, 0.0f, depth );
	vaPoints[6] = math::Vec3( height, 0.0f, -depth );
	vaPoints[7] = math::Vec3( height, 0.0f, depth );
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
Bullet::~Bullet()
{
	if( m_Body != 0 )
	{
		if( physics::PhysicsWorldB2D::GetWorld() )
			physics::PhysicsWorldB2D::GetWorld()->DestroyBody(m_Body);

		m_Body = 0;
	}
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void Bullet::Update( float deltaTime )
{
	if( !m_Active )
	{
		if( m_Body != 0 )
		{
			physics::PhysicsWorldB2D::GetWorld()->DestroyBody(m_Body);
			m_Body = 0;
		}
	}
	else
	{
		b2Vec2 pos = m_Body->GetPosition();

		m_YPos += m_YImpulse*deltaTime;

		m_Pos.X = pos.x;
		m_Pos.Y = m_YPos;
		m_Pos.Z = pos.y;

		m_Collision.vCenterPoint = m_Pos;

		if( m_Pos.Y >= 1.8f )
		{
			// bullet is considered high
			b2Filter fd;

			// hits only high walls
			fd.groupIndex = PLAYER_AND_BULLET_GROUP;
			fd.categoryBits = BULLETHIGH_CATEGORY;
			fd.maskBits = WORLDHIGH_CATEGORY | ENEMY_CATEGORY;

			m_Fixture->SetFilterData( fd );
		}

		if( m_Pos.Y <= 1.05f )
		{
			// bullet is considered low
			b2Filter fd;

			// hits high/low and the enemy walls
			fd.groupIndex = PLAYER_AND_BULLET_GROUP;
			fd.categoryBits = BULLETLOW_CATEGORY;
			fd.maskBits = WORLDHIGH_CATEGORY | WORLDLOW_CATEGORY | ENEMY_CATEGORY;

			m_Fixture->SetFilterData( fd );
		}

		if( m_Pos.Y <= 0.0f )
			m_Active = false;
	}
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: 
///
/////////////////////////////////////////////////////
void Bullet::Draw()
{
	if( m_Active )
	{
		glPushMatrix();
			glTranslatef( m_Collision.vCenterPoint.X, m_Collision.vCenterPoint.Y, m_Collision.vCenterPoint.Z );
			glRotatef( m_Rot, 0.0f, 1.0f, 0.0f );

			glVertexPointer( 3, GL_FLOAT, sizeof(math::Vec3), vaPoints );

			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4 );
			glDrawArrays(GL_TRIANGLE_STRIP, 4, 4 );
		glPopMatrix();
	}
}

/////////////////////////////////////////////////////
/// Method: DebugDraw
/// Params: 
///
/////////////////////////////////////////////////////
void Bullet::DebugDraw()
{
	glPushMatrix();
		glTranslatef( m_Collision.vCenterPoint.X, m_Collision.vCenterPoint.Y, m_Collision.vCenterPoint.Z );
		renderer::DrawSphere( BULLET_COLLISION_RADIUS );
	glPopMatrix();
}

/////////////////////////////////////////////////////
/// Method: HandleContact
/// Params: 
///
/////////////////////////////////////////////////////
void Bullet::HandleContact( int idNum, int castId, const ContactPoint* contact, const b2Fixture* pOtherFixture, const b2Body* pOtherBody )
{
	switch( idNum )
	{
		case PHYSICSBASICID_WORLD:
		case PHYSICSBASICID_ENEMY:
		{
			m_Active = false;
		}break;

		default:
			// colliding with something that doesn't have an id
			DBG_ASSERT_MSG( 0, "Bullet colliding with object that has no physics identifier" );
		break;
	}
}

/////////////////////////////////////////////////////
/// Method: Spawn
/// Params: 
///
/////////////////////////////////////////////////////
void Bullet::Spawn( BulletSource src, const math::Vec3& spawnPos, float rot, const math::Vec3& impulse )
{
	b2Vec2 zeroVec(0.0f, 0.0f);
	b2BodyDef bd;
	bd.type = b2_dynamicBody;
	bd.gravityScale = 0.0f;
	bd.awake = true;
	bd.fixedRotation = true;
	bd.bullet = true;
	//bd.linearDamping = 0.1f;

	m_Source = src;
	m_Rot = rot;

	// setup physics
	m_Body = physics::PhysicsWorldB2D::GetWorld()->CreateBody(&bd);
	DBG_ASSERT_MSG( (m_Body != 0), "Could not create bullet physics body" );
	
	b2FixtureDef fd;
	fd.friction = 1.0f;
	fd.restitution = 0.0f;
	fd.density = 0.0001f;

	fd.filter.groupIndex = PLAYER_AND_BULLET_GROUP;
	fd.filter.categoryBits = BULLETLOW_CATEGORY;
	fd.filter.maskBits = WORLDHIGH_CATEGORY | ENEMY_CATEGORY;

	b2CircleShape circleShape;
	circleShape.m_p = b2Vec2( 0.0f, 0.0f );
	circleShape.m_radius = BULLET_RADIUS;

	fd.shape = &circleShape;

	m_Fixture = 0;
	m_Fixture = m_Body->CreateFixture( &fd );
	m_Fixture->SetUserData( reinterpret_cast<void *>(this) );

	// mass setup
	b2MassData massData; 
	massData.mass = 0.1f;
	massData.center = b2Vec2( 0.0f, 0.0f );
	massData.I = 1.0f;
	
	m_Body->SetMassData( &massData );
	m_Body->SetUserData( reinterpret_cast<void *>(this) );		

	if( m_Body != 0 )
	{
		m_Body->SetTransform( b2Vec2(spawnPos.X, spawnPos.Z), 0.0f );	
		m_Body->SetLinearVelocity( zeroVec );
		m_Body->SetAngularVelocity( 0.0f );
	}

	m_YStartPos = spawnPos.Y;
	m_YPos = m_YStartPos;
	m_YImpulse = impulse.Y;

	b2Vec2 impulseVec( impulse.X, impulse.Z );
	m_Body->ApplyLinearImpulse( impulseVec, zeroVec, true );

	m_Active = true;
}

/////////////////////////////////////////////////////
/// Method: Clear
/// Params: 
///
/////////////////////////////////////////////////////
void Bullet::Clear()
{
	m_Active = false;

	if( m_Body != 0 )
	{
		physics::PhysicsWorldB2D::GetWorld()->DestroyBody(m_Body);
		m_Body = 0;
	}
}
