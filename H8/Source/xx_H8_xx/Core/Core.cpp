
/*===================================================================
	File: Core.cpp
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
#include "Resources/StringResources.h"

#include "Core/Core.h"

namespace
{

	math::Vec3 zeroVec(0.0f,0.0f,0.0f);
	b2Vec2 b2ZeroVec( 0.0f, 0.0f );

	const float BLINK_TIME_MIN = 2.5f;
	const float BLINK_TIME_MAX = 5.0f;

	const float LOOK_TIME_MIN = 1.0f;
	const float LOOK_TIME_MAX = 2.0f;
	const float LOOK_RANDOM_MIN_ANGLE = -180.0f;
	const float LOOK_RANDOM_MAX_ANGLE = 180.0f;

	const float BLINK_ROTATE = 720.0f;
	const float LOOK_ROTATE = 360.0f;

}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
Core::Core()
	: PhysicsIdentifier( PHYSICSBASICID_CORE, PHYSICSCASTID_CORE)
{
	int i=0, j=0;

	m_pScriptData = GetScriptDataHolder();
	m_GameData = m_pScriptData->GetGameData();
	m_DevData = m_pScriptData->GetDevData();

	m_LastDelta = 0.0f;
	m_pBaseModel = 0;
	m_pPetalsModel = 0;
	m_pPupilsModel = 0;

	m_ActivePupil = 0;

	// create model and anims
	m_pBaseModel = res::LoadModel( 10 );
	DBG_ASSERT( m_pBaseModel != 0 );

	m_pPetalsModel = res::LoadModel( 11 );
	DBG_ASSERT( m_pPetalsModel != 0 );

	m_pPupilsModel = res::LoadModel( 12 );
	DBG_ASSERT( m_pPupilsModel != 0 );

	m_pVeinsModel = res::LoadModel( 14 );
	DBG_ASSERT( m_pPetalsModel != 0 );
	m_pVeinsModel->SetTextureAnimation( 0 );
	
	for( i=0; i < 1000; ++i )
	{
		for( j=0; j < (int)m_pVeinsModel->GetNumOpaqueMeshes(); ++j )
		{
			m_pVeinsModel->Update( math::RandomNumber( 0.016f, 0.04f ) );
			m_pVeinsModel->SetMeshDrawState( j, ( (rand()%2) != 1) );
			m_pVeinsModel->Draw();
		}
	}
	// turn back on
	for( i=0; i < (int)m_pVeinsModel->GetNumOpaqueMeshes(); ++i )
	{
		m_pVeinsModel->SetMeshDrawState( i, true );
	}

	m_Body = 0;
	m_StartPosition.setZero();

	m_IsEnabled = false;
	m_ColourIndex = -1;
	m_PetalCount = m_GameData.CORE_PETAL_COUNT;

	m_PupilState = PupilState_Normal;
	m_NextState = PupilState_Normal;
	SetActivePupil( m_PupilState );

	m_BlinkTime = math::RandomNumber( BLINK_TIME_MIN, BLINK_TIME_MAX );
	m_CoreRotation.setZero();
	m_BlinkState = -1;

	m_DoLook = false;
	m_RandomLookTime = math::RandomNumber( LOOK_TIME_MIN, LOOK_TIME_MAX );
	m_RandomLookTarget = math::RandomNumber( LOOK_RANDOM_MIN_ANGLE, LOOK_RANDOM_MAX_ANGLE );

	m_BlinkAudio = AudioSystem::GetInstance()->AddAudioFile( 49 );
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
Core::~Core()
{
	if( m_pBaseModel != 0 )
	{
		res::RemoveModel( m_pBaseModel );
		m_pBaseModel = 0;
	}

	if( m_pPetalsModel != 0 )
	{
		res::RemoveModel( m_pPetalsModel );
		m_pPetalsModel = 0;
	}

	if( m_pPupilsModel != 0 )
	{
		res::RemoveModel( m_pPupilsModel );
		m_pPupilsModel = 0;
	}

	if( m_pVeinsModel != 0 )
	{
		res::RemoveModel( m_pVeinsModel );
		m_pVeinsModel = 0;
	}
}

/////////////////////////////////////////////////////
/// Method: Setup
/// Params: None
///
/////////////////////////////////////////////////////
void Core::Setup( float radius )
{
	// create the box2d shape for the core
	b2BodyDef bd;
	bd.type = b2_staticBody;
	bd.gravityScale = 0.0f;
	bd.awake = true;
	bd.fixedRotation = true;
	
	// setup physics
	m_Body = physics::PhysicsWorldB2D::GetWorld()->CreateBody(&bd);
	DBG_ASSERT_MSG( (m_Body != 0), "Could not create player physics body" );
	
	b2FixtureDef fd;
	fd.friction = 0.0f;
	fd.restitution = 0.0f;
	fd.density = 0.0f;

	fd.filter.groupIndex = CORE_GROUP;
	fd.filter.categoryBits = CORE_CATEGORY;
	fd.filter.maskBits = ENEMY_CATEGORY;	

	b2CircleShape circleShape;
	circleShape.m_p = b2Vec2( 0.0f, 0.0f );
	circleShape.m_radius = radius;

	fd.shape = &circleShape;

	b2Fixture* pFixture = 0;
	pFixture = m_Body->CreateFixture( &fd );
	DBG_ASSERT_MSG( (pFixture != 0), "Could not create player physics fixture" );

	pFixture->SetUserData( reinterpret_cast<void *>(this) );

	// mass setup
	m_Body->SetUserData( reinterpret_cast<void *>(this) );

	m_Body->SetAwake(true);
	m_Body->SetSleepingAllowed( true );

	SetNumeric1( 0 );
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void Core::Draw()
{
	renderer::OpenGL::GetInstance()->SetColour4ub( 255,255,255,255 );

	if( m_pBaseModel != 0 )
		m_pBaseModel->Draw();

	if( m_pPetalsModel != 0 )
		m_pPetalsModel->Draw();

	if( m_pVeinsModel != 0 )
		m_pVeinsModel->Draw();

	if( m_pPupilsModel != 0 )
		m_pPupilsModel->Draw();

}

/////////////////////////////////////////////////////
/// Method: DrawDebug
/// Params: None
///
/////////////////////////////////////////////////////
void Core::DrawDebug()
{

}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void Core::Update( float deltaTime )
{
	int i=0;

	m_LastDelta = deltaTime;

	if( m_IsEnabled )
	{

	}

	if( m_BlinkState == -1 ) // idle
	{
		m_BlinkTime -= deltaTime;
		if( m_BlinkTime <= 0.0f )
		{
			m_BlinkState = 0;
			m_CoreRotation.X = 0.0f;

			if( m_BlinkAudio != snd::INVALID_SOUNDBUFFER )
			{
				AudioSystem::GetInstance()->PlayAudio( m_BlinkAudio, math::Vec3(0.0f,0.0f,0.0f), AL_TRUE, AL_FALSE, 1.0f, 1.0f );
			}	
		}
	}
	else if( m_BlinkState == 0 ) // started
	{
		if( m_CoreRotation.X < 90.0f )
		{
			m_CoreRotation.X += BLINK_ROTATE*deltaTime;
		}

		if( m_CoreRotation.X >= 90.0f )
		{
			m_CoreRotation.X = 90.0f;
			m_BlinkState = 1;

			// check what state
			if( m_PupilState != m_NextState )
			{
				m_PupilState = m_NextState;
				
				SetPupilState( m_PupilState );
				SetActivePupil( m_PupilState );
			}
		}
	}
	else if( m_BlinkState == 1 ) // end
	{
		if( m_CoreRotation.X > 0.0f )
		{
			m_CoreRotation.X -= BLINK_ROTATE*deltaTime;
		}

		if( m_CoreRotation.X <= 0.0f )
		{
			m_CoreRotation.X = 0.0f;

			if( m_PetalCount == 1 )
			{
				if( (m_PupilState != PupilState_Sad_LastPetal) )
				{
					SetPupilState( PupilState_Sad_LastPetal, 2.0f );
				}
				else
				{
					m_BlinkTime = math::RandomNumber( BLINK_TIME_MIN, BLINK_TIME_MAX );
				}
			}
			else
			{
				if( (m_PupilState != PupilState_Normal) )
				{
					SetPupilState( PupilState_Normal, 2.0f );
				}
				else
				{
					m_BlinkTime = math::RandomNumber( BLINK_TIME_MIN, BLINK_TIME_MAX );
				}
			}

			m_BlinkState = -1;
		}
	}

	if( m_PupilState == PupilState_Normal )
	{
		if( m_DoLook )
		{
			if( m_RandomLookTarget > m_CoreRotation.Z )
				m_CoreRotation.Z += LOOK_ROTATE*deltaTime;
			else if( m_RandomLookTarget < m_CoreRotation.Z )
				m_CoreRotation.Z -= LOOK_ROTATE*deltaTime;

			if( math::WithinTolerance( m_CoreRotation.Z, m_RandomLookTarget, 5.0f ) )
			{
				m_CoreRotation.Z = m_RandomLookTarget;

				m_RandomLookTarget = math::RandomNumber( LOOK_RANDOM_MIN_ANGLE, LOOK_RANDOM_MAX_ANGLE );
				m_RandomLookTime = math::RandomNumber( LOOK_TIME_MIN, LOOK_TIME_MAX );

				m_DoLook = false;
			}
		}
		else
		{
			m_RandomLookTime -= deltaTime;
			if( m_RandomLookTime <= 0.0f )
			{
				m_RandomLookTime = 0.0f;
				m_DoLook = true;
			}
		}
	}

	if( m_pPupilsModel != 0 )
	{
		int meshCount = m_pPupilsModel->GetNumOpaqueMeshes();

		for( i=0; i < meshCount; ++i )
		{
			m_pPupilsModel->SetMeshRotation( i, m_CoreRotation );
		}
	}

	if( m_pVeinsModel != 0 )
		m_pVeinsModel->Update( deltaTime );
}

/////////////////////////////////////////////////////
/// Method: Enable
/// Params: None
///
/////////////////////////////////////////////////////
void Core::Enable()
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
}

/////////////////////////////////////////////////////
/// Method: Disable
/// Params: None
///
/////////////////////////////////////////////////////
void Core::Disable()
{
	b2Fixture* pFixture = 0;
	pFixture = m_Body->GetFixtureList();

	b2Vec2 pos( m_StartPosition.X, m_StartPosition.Y );
	m_Body->SetTransform( pos, 0.0f );

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
	m_ColourIndex = -1;

	SetNumeric1( m_ColourIndex );
}

/////////////////////////////////////////////////////
/// Method: TakeDamage
/// Params: None
///
////////////////////////////////////////////////////
void Core::Reset()
{
	m_PetalCount = m_GameData.CORE_PETAL_COUNT;
		
	int i=0;
	for( i=0; i < m_PetalCount; ++i  )
	{
		m_pPetalsModel->SetMeshDrawState( i, true );
		m_pVeinsModel->SetMeshDrawState( i, true );
	}
}

/////////////////////////////////////////////////////
/// Method: TakeDamage
/// Params: None
///
////////////////////////////////////////////////////
void Core::TakeDamage()
{
	m_PetalCount -= 1;

	if( m_PetalCount <= 0 )
	{
		m_PetalCount = 0;
		m_pPetalsModel->SetMeshDrawState( m_PetalCount, false );

		m_pVeinsModel->SetMeshDrawState( m_PetalCount, false );
	}
	else
	{
		m_pPetalsModel->SetMeshDrawState( m_PetalCount, false );
		m_pVeinsModel->SetMeshDrawState( m_PetalCount, false );
	}

	int inserts = math::RandomNumber(1, 3);

	for( int i=0; i < inserts; i++ )
		GameSystems::GetInstance()->AddTextToConsole( res::GetScriptString( math::RandomNumber(1000, 1030) ) );
}

/////////////////////////////////////////////////////
/// Method: SetColour
/// Params: None
///
////////////////////////////////////////////////////
void Core::SetColour( int colourIndex, const math::Vec3& colour )
{
/*	int i=0;
	m_ColourIndex = colourIndex;
	m_Colour = colour;

	SetNumeric1( m_ColourIndex );

	math::Vec4Lite col( static_cast<int>(255.0f*m_Colour.R), static_cast<int>(255.0f*m_Colour.G), static_cast<int>(255.0f*m_Colour.B), 255 );

	if( m_pPupilsModel != 0 )
	{
		int meshCount = m_pPupilsModel->GetNumOpaqueMeshes();

		for( i=0; i < meshCount; ++i )
		{
			m_pPupilsModel->EnableMeshDefaultColour( i );
			m_pPupilsModel->SetMeshDefaultColour( i, col );
		}
	}
*/
}


/////////////////////////////////////////////////////
/// Method: SetActivePupil
/// Params: None
///
////////////////////////////////////////////////////
void Core::SetActivePupil( int meshId )
{
	if( m_pPupilsModel != 0 )
	{
		m_pPupilsModel->SetMeshDrawState( -1, false );

		m_ActivePupil = meshId;
		m_pPupilsModel->SetMeshDrawState( m_ActivePupil, true );
	}
}

/////////////////////////////////////////////////////
/// Method: SetPupilState
/// Params: None
///
////////////////////////////////////////////////////
void Core::SetPupilState( PupilState state, float blinkTime )
{
	m_NextState = state;

	m_BlinkTime = blinkTime;

	m_CoreRotation.Z = 0.0f;
}

/////////////////////////////////////////////////////
/// Method: NextPupil
/// Params: None
///
////////////////////////////////////////////////////
void Core::NextPupil()
{
	if( m_pPupilsModel != 0 )
	{
		m_ActivePupil++;
		if( m_ActivePupil >= (int)m_pPupilsModel->GetNumOpaqueMeshes() )
			m_ActivePupil = 0;

		SetActivePupil(m_ActivePupil);
	}
}

/////////////////////////////////////////////////////
/// Method: PlayCoreHitAudio
/// Params: None
///
////////////////////////////////////////////////////
void Core::PlayCoreHitAudio()
{
	// play hit sound
	/*ALuint sourceId = snd::INVALID_SOUNDSOURCE;
	int randomSound = math::RandomNumber( 0, CORE_HIT_AUDIO_COUNT-1 );

	if( m_CoreHitBufferIds[randomSound] != snd::INVALID_SOUNDBUFFER )
	{
		AudioSystem::GetInstance()->PlayAudio( m_CoreHitBufferIds[randomSound], math::Vec3(0.0f,0.0f,0.0f), AL_TRUE );
	}*/
}
