
/*===================================================================
	File: ScriptedPlayer.cpp
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

#include "AppConsts.h"
#include "H4.h"

#include "Player/Scripted/ScriptedPlayerAnimationLookup.h"
#include "Player/Scripted/ScriptedPlayer.h"

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
ScriptedPlayer::ScriptedPlayer( )
{
	m_pModelData = 0;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
ScriptedPlayer::~ScriptedPlayer()
{
	if( m_pModelData != 0 )
	{
		delete m_pModelData;
		m_pModelData = 0;
	}
}

/////////////////////////////////////////////////////
/// Method: Setup
/// Params: None
///
/////////////////////////////////////////////////////
void ScriptedPlayer::Setup()
{
	int i;

	// setup model and anims
	m_pModelData = 0;
	m_pModelData = new mdl::ModelHGA;
	DBG_ASSERT_MSG( (m_pModelData != 0), "Could not allocate scripted player HGM model" );

	m_pModelData->LoadBindPose( "hga/pl_scene_bind.hga", GL_LINEAR, GL_LINEAR, core::app::GetLowPowerMode() );

	for( i=0; i < ScriptedPlayerAnim::NUM_ANIMS; ++i )
	{
		ScriptedPlayerAnim::AnimData animData = GetScriptedPlayerAnimationData(i);
		m_pModelData->LoadAnimation( animData.szFile, animData.animId, animData.loopFlag );
	}

	// no culling as they are moving models
	m_pModelData->SetModelGeneralFlags( 0 );
	m_pModelData->SetupAndFree();

	m_pModelData->SetAnimation( ScriptedPlayerAnim::IDLE, -1, true );
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void ScriptedPlayer::Update( float deltaTime )
{
	m_pModelData->Update(deltaTime);
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void ScriptedPlayer::Draw()
{
	glPushMatrix();
		//glTranslatef( m_Pos.X, m_Pos.Y, m_Pos.Z );
		//glRotatef( m_RotationAngle, 0.0f, 1.0f, 0.0f );

		m_pModelData->Draw();
	glPopMatrix();
}