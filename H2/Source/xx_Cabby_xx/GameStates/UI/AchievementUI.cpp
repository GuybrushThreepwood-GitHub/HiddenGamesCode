
/*===================================================================
	File: AchievementUI.cpp
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "SoundBase.h"
#include "CollisionBase.h"
#include "Input/Input.h"
#include "ModelBase.h"
#include "ScriptBase.h"
#include "SupportBase.h"

#include "CabbyConsts.h"
#include "Cabby.h"

#include "StateManage/IState.h"

#include "Resources/ModelResources.h"
#include "Resources/StringResources.h"
#include "Resources/SoundResources.h"

#include "GameEffects/FullscreenEffects.h"

#include "GameStates/UI/UIBaseState.h"
#include "GameStates/UI/AchievementUI.h"
#include "GameStates/UI/UIFileLoader.h"

#include "GameStates/UI/GeneratedFiles/achievement_popup.hgm.h"
#include "GameStates/UI/GeneratedFiles/achievement_popup.hui.h"

namespace
{

}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
AchievementUI::AchievementUI()
{
	m_UIMesh = 0;

	m_GameData = GetScriptDataHolder()->GetGameData();

	m_IsIdle = true;

	math::Vec2 srcDims( static_cast<float>( core::app::GetBaseAssetsWidth() ), static_cast<float>( core::app::GetBaseAssetsHeight() ) );
	//m_UIMesh = res::LoadModel(1014);
	//DBG_ASSERT( m_UIMesh != 0 );

	m_Elements.Load( "assets/ui/achievement_popup.hui", srcDims, m_UIMesh );

	m_IconCurPos = m_IconStartPos = m_Elements.GetElementPosition( HUI_GLYPH_ACHIEVEMENT );
	
	if( renderer::OpenGL::GetInstance()->GetIsRotated() )
		m_MaxXPos = core::app::GetOrientationHeight() - m_GameData.ACHIEVEMENT_ICON_MAXX;
	else 
		m_MaxXPos = core::app::GetOrientationWidth() - m_GameData.ACHIEVEMENT_ICON_MAXX;

	m_ShowTimer = m_GameData.ACHIEVEMENT_ICON_SHOWTIME;
	m_MovementSpeed = -m_GameData.ACHIEVEMENT_ICON_SPEED;
	m_CurrentShowTimer = 0.0f;

	m_MoveOut = false;
	m_MoveIn = false;

	AudioSystem::GetInstance()->AddAudioFile( 132 );
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
AchievementUI::~AchievementUI()
{
	if( m_UIMesh != 0 )
	{
		res::RemoveModel( m_UIMesh );
		m_UIMesh = 0;
	}
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void AchievementUI::Update( float deltaTime )
{
	m_LastDeltaTime = deltaTime;

	m_Elements.Update( TOUCH_SIZE_MENU, 2, deltaTime );

	// update the show timer if we have to
	if( m_CurrentShowTimer > 0.0f )
	{
		m_CurrentShowTimer -= deltaTime;
		if( m_CurrentShowTimer <= 0.0f )
		{
			m_CurrentShowTimer = 0.0f;
			m_MoveIn = true;
		}
	}

	// check idle state
	if( m_IsIdle )
	{
		// any new achievements in the queue
		if( !m_AchievementsToShow.empty() )
		{
			// start the move out
			if( core::app::IsSupportingAchievements() )
			{
				m_IsIdle = false;
				m_MoveOut = true;

				// play success
				ALuint sourceId = snd::INVALID_SOUNDSOURCE;
				ALuint bufferId = snd::INVALID_SOUNDBUFFER;
				sourceId = snd::SoundManager::GetInstance()->GetFreeSoundSource();
				bufferId = AudioSystem::GetInstance()->FindSound( 132 );

				if( sourceId != snd::INVALID_SOUNDSOURCE &&
					bufferId != snd::INVALID_SOUNDBUFFER )
				{
					AudioSystem::GetInstance()->PlayAudio( sourceId, bufferId, math::Vec3(0.0f,0.0f,0.0f), AL_TRUE );
				}
			}

			// remove it
			m_AchievementsToShow.pop();
		}
	}
	else
	{
		// update the icon until it hits the far point
		if( m_MoveOut )
		{
			if( m_IconCurPos.X > m_MaxXPos )
			{
				m_IconCurPos.X += m_MovementSpeed*deltaTime;
				if( m_IconCurPos.X <= m_MaxXPos )
				{
					m_IconCurPos.X = m_MaxXPos;
					m_MoveOut = false;

					// start the timer
					m_CurrentShowTimer = m_ShowTimer;
				}
					
				m_Elements.ChangeElementPosition(HUI_GLYPH_ACHIEVEMENT, m_IconCurPos);
			}
		}
		else if( m_MoveIn )
		{
			// update icon to move back in
			if( m_IconCurPos.X < m_IconStartPos.X )
			{
				m_IconCurPos.X -= m_MovementSpeed*deltaTime;
				if( m_IconCurPos.X >= m_IconStartPos.X )
				{
					m_IconCurPos.X = m_IconStartPos.X;
					m_MoveIn = false;
					m_IsIdle = true;

					m_CurrentShowTimer = 0.0f;
				}
					
				m_Elements.ChangeElementPosition(HUI_GLYPH_ACHIEVEMENT, m_IconCurPos);
			}
		}
	}
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void AchievementUI::Draw()
{
	renderer::OpenGL::GetInstance()->SetNearFarClip( NEAR_CLIP_ORTHO, FAR_CLIP_ORTHO );
	renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );
	renderer::OpenGL::GetInstance()->DisableLighting();

	renderer::OpenGL::GetInstance()->SetColour4ub( 255, 255, 255, 255 );

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_ALWAYS );
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	if( m_UIMesh != 0 )
		m_UIMesh->Draw();

	m_Elements.Draw();

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_LESS );
	renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}

/////////////////////////////////////////////////////
/// Method: Show
/// Params: [in]achievementId
///
/////////////////////////////////////////////////////
void AchievementUI::Show( int achievementId )
{
	// add to the queue
	m_AchievementsToShow.push(achievementId);
}