
/*===================================================================
	File: EffectPool.cpp
	Game: H5

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "SoundBase.h"
#include "PhysicsBase.h"
#include "ScriptBase.h"

#include "Resources/EmitterResources.h"
#include "Resources/SpriteResources.h"
#include "Resources/TextureResources.h"
#include "Resources/ResourceHelper.h"

#include "EffectPool.h"

namespace
{
	math::Vec3 PosOut( 0.0f, -10000.0f, 0.0f );
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
EffectPool::EffectPool( )
{
	int i=0;
	const res::EmitterResourceStore* er = 0;
	//const res::SpriteResourceStore* sr = 0;
	//const res::TextureResourceStore* tr = 0;

	m_NextCircleIndex = 0;
	for( i=0; i < MAX_CIRCLE_EFFECTS; ++i )
	{
		m_CircleEffect[i].active = false;
		m_CircleEffect[i].colour = math::Vec3( 1.0f, 1.0f, 1.0f );
		m_CircleEffect[i].scale = 0.1f;

		const float offset = 2.0f * math::PI / MAX_CIRCLE_POINTS;
		float theta = 0.0f;
	
		for (int k = 0; k < MAX_CIRCLE_POINTS; ++k)
		{
			math::Vec3 v = math::Vec3( std::cos(theta), std::sin(theta), 0.0f ) * 1.0f;
			m_CircleEffect[i].vaPoints[k] = v;
			theta += offset;
		}
	}	

	m_NextExplosionIndex = 0;
	m_ExplosionList = new efx::Emitter[MAX_PIXEL_EXPLOSIONS];
	DBG_ASSERT( m_ExplosionList != 0 );

	for( i=0; i < MAX_PIXEL_EXPLOSIONS; ++i )
	{
		if( i % 2 )
			er = res::GetEmitterResource( 0 );
		else
			er = res::GetEmitterResource( 1 );

		m_ExplosionList[i].Create( *er->block );
		res::SetupTexturesOnEmitter( &m_ExplosionList[i] );
	}


	// hearts
	er = res::GetEmitterResource( 200 );
	m_Hearts.Create( *er->block );
	res::SetupTexturesOnEmitter( &m_Hearts );
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
EffectPool::~EffectPool()
{
	if( m_ExplosionList != 0 )
	{
		delete[] m_ExplosionList;
		m_ExplosionList = 0;
	}
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void EffectPool::Draw()
{
	int i=0;

	for( i=0; i < MAX_PIXEL_EXPLOSIONS; ++i )
	{
		m_ExplosionList[i].Draw();
	}

	m_Hearts.Draw();

	bool textureState = renderer::OpenGL::GetInstance()->GetTextureState();
	if( textureState )
		renderer::OpenGL::GetInstance()->DisableTexturing();

	for( i=0; i < MAX_CIRCLE_EFFECTS; ++i )
	{
		if(m_CircleEffect[i].active)
		{
			renderer::OpenGL::GetInstance()->SetColour4f( m_CircleEffect[i].colour.R, m_CircleEffect[i].colour.G, m_CircleEffect[i].colour.B, 0.1f );

			glPushMatrix();
				glTranslatef( m_CircleEffect[i].pos.X, m_CircleEffect[i].pos.Y, m_CircleEffect[i].pos.Z );
				glScalef( m_CircleEffect[i].scale, m_CircleEffect[i].scale, m_CircleEffect[i].scale );

				glVertexPointer(3, GL_FLOAT, sizeof(math::Vec3), m_CircleEffect[i].vaPoints);
				glDrawArrays(GL_LINE_LOOP, 0, MAX_CIRCLE_POINTS );
			glPopMatrix();			
		}
	}	

	if( textureState )
		renderer::OpenGL::GetInstance()->EnableTexturing();
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void EffectPool::Update( float deltaTime )
{
	int i=0;
	math::Vec3 posVec;

	m_Hearts.Update( deltaTime );

	if( m_Hearts.IsEnabled() )
		m_Hearts.Disable();

	for( i=0; i < MAX_PIXEL_EXPLOSIONS; ++i )
	{
		m_ExplosionList[i].Update( deltaTime );

		if( m_ExplosionList[i].IsEnabled() )
			m_ExplosionList[i].Disable();
	}

	for( i=0; i < MAX_CIRCLE_EFFECTS; ++i )
	{
		if(m_CircleEffect[i].active)
		{
			m_CircleEffect[i].scale += 10.0f*deltaTime;

			//m_CircleEffect[i].pos.Z += 1.0f*deltaTime;

			if( m_CircleEffect[i].scale > 60.0f )
				m_CircleEffect[i].active = false;
		}
	}	
}

/////////////////////////////////////////////////////
/// Method: Reset
/// Params: None
///
/////////////////////////////////////////////////////
void EffectPool::Reset()
{
	int i=0;

	m_NextCircleIndex = 0;
	for( i=0; i < MAX_CIRCLE_EFFECTS; ++i )
	{
		m_CircleEffect[i].active = false;
		m_CircleEffect[i].colour = math::Vec3( 1.0f, 1.0f, 1.0f );
		m_CircleEffect[i].scale = 1.0f;
	}	

	m_NextExplosionIndex = 0;
	for( i=0; i < MAX_PIXEL_EXPLOSIONS; ++i )
	{
		m_ExplosionList[i].Disable();
		m_ExplosionList[i].SetPos( PosOut );
		m_ExplosionList[i].Stop();
	}

	m_Hearts.Disable();
	m_Hearts.SetPos( PosOut );
	m_Hearts.Stop();
}

/////////////////////////////////////////////////////
/// Method: SpawnExplosion
/// Params: [in]pos, [in]colour
///
/////////////////////////////////////////////////////
void EffectPool::SpawnExplosion( const math::Vec3& pos, const math::Vec3 colour )
{
	//if( m_UsedExplosionSlots < m_TotalExplosions )
	{
		math::Vec4 startCol( colour.R, colour.G, colour.B, 1.0f );
		math::Vec4 endCol( colour.R, colour.G, colour.B, 0.0f );

		m_ExplosionList[m_NextExplosionIndex].SetStartColour1( startCol );
		m_ExplosionList[m_NextExplosionIndex].SetStartColour2( startCol );
		m_ExplosionList[m_NextExplosionIndex].SetEndColour1( endCol );
		m_ExplosionList[m_NextExplosionIndex].SetEndColour2( endCol );

		m_ExplosionList[m_NextExplosionIndex].SetPos( pos );
		m_ExplosionList[m_NextExplosionIndex].Reset();

		m_ExplosionList[m_NextExplosionIndex].Enable();
		m_ExplosionList[m_NextExplosionIndex].Disable();

		m_NextExplosionIndex++;
		if( m_NextExplosionIndex >= MAX_PIXEL_EXPLOSIONS )
			m_NextExplosionIndex = 0;
	}
}

/////////////////////////////////////////////////////
/// Method: SpawnCircle
/// Params: [in]pos
///
/////////////////////////////////////////////////////
void EffectPool::SpawnCircle( const math::Vec3& pos, const math::Vec3& col )
{
	m_CircleEffect[m_NextCircleIndex].pos = pos;
	m_CircleEffect[m_NextCircleIndex].scale = 0.1f;
	m_CircleEffect[m_NextCircleIndex].active = true;
	m_CircleEffect[m_NextCircleIndex].colour = col;

	m_NextCircleIndex++;
	if( m_NextCircleIndex >= MAX_CIRCLE_EFFECTS )
		m_NextCircleIndex = 0;
}

/////////////////////////////////////////////////////
/// Method: SpawnHearts
/// Params: None
///
/////////////////////////////////////////////////////
void EffectPool::SpawnHearts()
{
	math::Vec3 center;
	center.setZero();

	m_Hearts.SetPos( center );
	m_Hearts.Reset();

	m_Hearts.Enable();
	m_Hearts.Disable();
}
