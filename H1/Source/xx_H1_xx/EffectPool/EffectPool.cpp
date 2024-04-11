
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
EffectPool::EffectPool( int maxExplosions, int maxFires )
{
	int i=0;
	const res::EmitterResourceStore* er = 0;

	m_UsedExplosionSlots = 0;
	m_UsedFireSlots = 0;

	// explosions
	m_ExplosionIndex = 0;
	m_TotalExplosions = maxExplosions;
	m_ExplosionList = new efx::Emitter[m_TotalExplosions];
	m_ExplosionSmokeList = new efx::Emitter[m_TotalExplosions];
	DBG_ASSERT( m_ExplosionList != 0 );
	
	for( i=0; i < m_TotalExplosions; ++i )
	{
		er = res::GetEmitterResource( 50 );
		m_ExplosionList[i].Create( *er->block );
		res::SetupTexturesOnEmitter( &m_ExplosionList[i] );

		er = res::GetEmitterResource( 51 );
		DBG_ASSERT( er != 0 );
		m_ExplosionSmokeList[i].Create( *er->block );
		res::SetupTexturesOnEmitter( &m_ExplosionSmokeList[i] );

	}

	// fires
	m_TotalFires = maxFires;
	m_FireList = new efx::Emitter[m_TotalFires];
	DBG_ASSERT( m_FireList != 0 );

	m_FireSmokeList = new efx::Emitter[m_TotalFires];
	DBG_ASSERT( m_FireSmokeList != 0 );
	
	for( i=0; i < m_TotalFires; ++i )
	{
		er = res::GetEmitterResource( 52 );
		DBG_ASSERT( er != 0 );
		m_FireList[i].Create( *er->block );
		res::SetupTexturesOnEmitter( &m_FireList[i] );

		if( i % 2 )
			er = res::GetEmitterResource( 53 );
		else 
			er = res::GetEmitterResource( 54 );
		DBG_ASSERT( er != 0 );
		m_FireSmokeList[i].Create( *er->block );

		res::SetupTexturesOnEmitter( &m_FireSmokeList[i] );
	}

	// stars
	m_Stars = new efx::Emitter;
	DBG_ASSERT( m_Stars != 0 );

	er = res::GetEmitterResource( 20 );
	DBG_ASSERT( er != 0 );
	m_Stars->Create( *er->block );
	res::SetupTexturesOnEmitter( m_Stars );
	
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

	if( m_ExplosionSmokeList != 0 )
	{
		delete[] m_ExplosionSmokeList;
		m_ExplosionSmokeList = 0;
	}

	if( m_FireList != 0 )
	{
		delete[] m_FireList;
		m_FireList = 0;
	}

	if( m_FireSmokeList != 0 )
	{
		delete[] m_FireSmokeList;
		m_FireSmokeList = 0;
	}

	if( m_Stars != 0 )
	{
		delete m_Stars;
		m_Stars = 0;
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

	for( i=0; i < m_TotalExplosions; ++i )
	{
		m_ExplosionList[i].Draw();
		m_ExplosionSmokeList[i].Draw();
	}

	for( i=0; i < m_TotalFires; ++i )
	{
		m_FireList[i].Draw();
		m_FireSmokeList[i].Draw();
	}
	
	if( m_Stars != 0 )
		m_Stars->Draw();
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

	// explosions
	for( i=0; i < m_TotalExplosions; ++i )
	{
		/*if( m_ExplosionList[i].IsEnabled() )
		{
			m_ExplosionList[i].Disable();
			m_ExplosionSmokeList[i].Disable();
		}*/

		m_ExplosionSmokeList[i].Update(deltaTime);
		m_ExplosionList[i].Update(deltaTime);
	}

	// fire
	for( i=0; i < m_TotalFires; ++i )
	{
		/*if( m_FireList[i].IsEnabled() )
		{
			m_FireList[i].Disable();
			m_FireSmokeList[i].Disable();
		}*/

		m_FireList[i].Update(deltaTime);
		m_FireSmokeList[i].Update(deltaTime);
	}

	if( m_Stars != 0 )
		m_Stars->Update(deltaTime);
}

/////////////////////////////////////////////////////
/// Method: Reset
/// Params: None
///
/////////////////////////////////////////////////////
void EffectPool::Reset()
{
	int i=0;

	m_ExplosionIndex = 0;
	for( i=0; i < m_TotalExplosions; ++i )
	{
		m_ExplosionList[i].Disable();
		m_ExplosionList[i].SetPos( PosOut );
		m_ExplosionList[i].Stop();

		m_ExplosionSmokeList[i].Disable();
		m_ExplosionSmokeList[i].SetPos( PosOut );
		m_ExplosionSmokeList[i].Stop();
	}

	for( i=0; i < m_TotalFires; ++i )
	{
		m_FireList[i].Disable();
		m_FireList[i].SetPos( PosOut );
		m_FireList[i].Stop();
		
		m_FireSmokeList[i].Disable();
		m_FireSmokeList[i].SetPos( PosOut );
		m_FireSmokeList[i].Stop();
		
	}

	if( m_Stars != 0 )
	{
		m_Stars->Disable();
		m_Stars->SetPos( PosOut );
		m_Stars->Stop();
	}

	m_UsedExplosionSlots = 0;
	m_UsedFireSlots = 0;
}

/////////////////////////////////////////////////////
/// Method: SpawnExplosion
/// Params: [in]pos
///
/////////////////////////////////////////////////////
void EffectPool::SpawnExplosion( const math::Vec3& pos )
{
	//if( m_UsedExplosionSlots < m_TotalExplosions )
	{
		m_ExplosionList[m_ExplosionIndex].SetPos( pos );
		m_ExplosionList[m_ExplosionIndex].Enable();
		m_ExplosionList[m_ExplosionIndex].Disable();

		m_ExplosionSmokeList[m_ExplosionIndex].SetPos( pos );
		m_ExplosionSmokeList[m_ExplosionIndex].Enable();
		m_ExplosionSmokeList[m_ExplosionIndex].Disable();

		if( m_UsedExplosionSlots < m_TotalExplosions )
			m_UsedExplosionSlots++;

		m_ExplosionIndex++;

		if( m_ExplosionIndex >= m_TotalExplosions )
		{
			m_ExplosionIndex = 0;

			m_ExplosionList[m_ExplosionIndex].Stop();
			m_ExplosionSmokeList[m_ExplosionIndex].Stop();
		}
		
		if( m_UsedExplosionSlots >= m_TotalExplosions )
			m_UsedExplosionSlots = m_TotalExplosions;
	}
}

/////////////////////////////////////////////////////
/// Method: SpawnFire
/// Params: [in]pos
///
/////////////////////////////////////////////////////
void EffectPool::SpawnFire( const math::Vec3& pos )
{
	//if( m_UsedFireSlots < m_TotalFires )
	{
		m_FireList[m_UsedFireSlots].SetPos( pos );
		m_FireSmokeList[m_UsedFireSlots].SetPos( pos );
		
		m_FireList[m_UsedFireSlots].Enable();
		m_FireSmokeList[m_UsedFireSlots].Enable();
		m_FireList[m_UsedFireSlots].Disable();
		m_FireSmokeList[m_UsedFireSlots].Disable();	

		m_UsedFireSlots++;

		if( m_UsedFireSlots >= m_TotalFires )
		{
			m_UsedFireSlots = 0;

			m_FireList[m_UsedFireSlots].Stop();
			m_FireSmokeList[m_UsedFireSlots].Stop();	
		}
	}
}

/////////////////////////////////////////////////////
/// Method: SpawnStars
/// Params: [in]pos
///
/////////////////////////////////////////////////////
void EffectPool::SpawnStars( const math::Vec3& pos )
{
	if( m_Stars != 0 )
	{
		m_Stars->SetPos( pos );
		m_Stars->Enable();
		m_Stars->Disable();
	}
}
