
/*===================================================================
	File: HangingSpider.cpp
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
#include "Player/Player.h"

#include "Resources/SoundResources.h"
#include "Resources/SpriteResources.h"
#include "Resources/EmitterResources.h"

#include "ScriptAccess/ScriptDataHolder.h"
#include "ScriptAccess/ScriptAccess.h"

#include "GameEffects/InsectAttack.h"
#include "GameSystems.h"

#include "Enemy/Insects.h"

namespace
{
	const float CULLING_RADIUS = 1.0f;
	const float TIME_IN_AREA = 1.5f;

	b2Vec2 zeroVec2(0.0f,0.0f);
	math::Vec3 zeroVec( 0.0f, 0.0f, 0.0f );
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
Insects::Insects(  )
{
	m_Pos = math::Vec3( 0.0f, 0.0f, 0.0f );
	m_RotationAngle = 0.0f;

	m_BoundSphere.vCenterPoint = math::Vec3( 0.0f, 0.0f, 0.0f );
	m_BoundSphere.fRadius = CULLING_RADIUS;

	m_LastDeltaTime = 0.0f;

	m_TimeInArea = 0.0f;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
Insects::~Insects()
{

}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void Insects::Draw( )
{
/*	renderer::OpenGL::GetInstance()->SetColour4ub( 255,255,255,255 );
	glPushMatrix();
		glTranslatef( m_BoundSphere.vCenterPoint.X, m_BoundSphere.vCenterPoint.Y, m_BoundSphere.vCenterPoint.Z );
		renderer::DrawSphere( m_BoundSphere.fRadius );
	glPopMatrix();
*/
	// DRAWN VIA A SPRITE
}

/////////////////////////////////////////////////////
/// Method: DrawDebug
/// Params: None
///
/////////////////////////////////////////////////////
void Insects::DrawDebug()
{
#ifdef _DEBUG
	renderer::OpenGL::GetInstance()->SetColour4ub( 255,255,255,255 );
	glPushMatrix();
		glTranslatef( m_BoundSphere.vCenterPoint.X, m_BoundSphere.vCenterPoint.Y, m_BoundSphere.vCenterPoint.Z );
		renderer::DrawSphere( m_BoundSphere.fRadius );
	glPopMatrix();
#endif // _DEBUG
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void Insects::Update( float deltaTime )
{
	if( IsInsectAttackActive() )
	{
		m_TimeInArea = 0.0f;
		return;
	}
	
	Player& player = GameSystems::GetInstance()->GetPlayer();
	collision::Sphere playerArea = player.GetBoundingSphere();

	if( playerArea.SphereCollidesWithSphere(m_BoundSphere) )
	{
		m_TimeInArea += deltaTime;

		if( m_TimeInArea >= TIME_IN_AREA )
		{
			StartInsectAttack(INSECTATTACK_ANTS);
			m_TimeInArea = 0.0f;
		}
	}
	else
	{
		m_TimeInArea -= deltaTime;
		if( m_TimeInArea <= 0.0f )
			m_TimeInArea = 0.0f;
	}
}
