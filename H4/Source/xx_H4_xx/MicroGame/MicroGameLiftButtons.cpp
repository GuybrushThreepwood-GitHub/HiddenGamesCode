
/*===================================================================
	File: MicroGameLiftButtons.cpp
	Game: H4

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "CollisionBase.h"
#include "SoundBase.h"
#include "ModelBase.h"
#include "InputBase.h"

#include "AppConsts.h"
#include "H4.h"

#include "Audio/AudioSystem.h"

#include "Resources/SoundResources.h"
#include "Resources/ModelResources.h"
#include "MicroGameLiftButtons.h"

// generated files
#include "GameStates/UI/GeneratedFiles/microgame_liftbuttons.h" // HGM submesh list
#include "GameStates/UI/GeneratedFiles/microgame_liftbuttons.hui.h" // UI element list

namespace
{
	const int MAX_TOUCH_TESTS = 1;
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
MicroGameLiftButtons::MicroGameLiftButtons( int currentFloor, 
										   bool button1Active, const char* button1Label, const char* button1Call, 
										   bool button2Active, const char* button2Label, const char* button2Call, 
										   bool button3Active, const char* button3Label, const char* button3Call )
{
	m_LiftButtonsModel = 0;

	m_PressInactiveAudio = snd::INVALID_SOUNDBUFFER;
	m_PressActiveAudio = snd::INVALID_SOUNDBUFFER;

	m_CallFunc = false;
	m_CallScriptFunc = 0;
	m_SelectedFloor = -1;

	m_FloorSelected = false;
	m_CurrentFloor = currentFloor;
	m_ExitTimer = 0.0f;

	m_Button1Active = button1Active;
	m_Button1Label = button1Label;
	m_Button1Call = button1Call;

	m_Button2Active = button2Active;
	m_Button2Label = button2Label;
	m_Button2Call = button2Call;

	m_Button3Active = button3Active;
	m_Button3Label = button3Label;
	m_Button3Call = button3Call;

	m_WaitAudioDelay = false;
	m_AudioDelay = 0.0f;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
MicroGameLiftButtons::~MicroGameLiftButtons()
{

}

/////////////////////////////////////////////////////
/// Method: Initialise
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGameLiftButtons::Initialise()
{

}

/////////////////////////////////////////////////////
/// Method: Release
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGameLiftButtons::Release()
{

}

/////////////////////////////////////////////////////
/// Method: OnEnter
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGameLiftButtons::OnEnter()
{
	int i=0;
	m_LiftButtonsModel = res::LoadModel( 2002 );
	DBG_ASSERT( (m_LiftButtonsModel != 0) );

	math::Vec2 srcAssetDims( static_cast<float>(core::app::GetBaseAssetsWidth()), static_cast<float>(core::app::GetBaseAssetsHeight()) );
	m_LiftButtonsUI.Load( "hui/microgame_liftbuttons.hui", srcAssetDims );

	math::Vec3 scaleFactor = math::Vec3( core::app::GetWidthScale(), core::app::GetHeightScale(), 1.0f );
	for( i=LIFTBUTTONS_BGMESH; i <= LIFTBUTTONS_BUTTON_3_CURRENT; ++i )
	{		
		m_LiftButtonsModel->SetMeshScale(i, scaleFactor);
	}

	m_LiftButtonsModel->SetMeshDrawState( LIFTBUTTONS_BUTTON_1_DOWN, false );
	m_LiftButtonsModel->SetMeshDrawState( LIFTBUTTONS_BUTTON_2_DOWN, false );
	m_LiftButtonsModel->SetMeshDrawState( LIFTBUTTONS_BUTTON_3_DOWN, false );

	m_LiftButtonsModel->SetMeshDrawState( LIFTBUTTONS_BUTTON_1_CURRENT, false );
	m_LiftButtonsModel->SetMeshDrawState( LIFTBUTTONS_BUTTON_2_CURRENT, false );
	m_LiftButtonsModel->SetMeshDrawState( LIFTBUTTONS_BUTTON_3_CURRENT, false );

	if( !m_Button1Active )
		m_LiftButtonsModel->SetMeshDrawState( LIFTBUTTONS_BUTTON_1_NORMAL, false );
	if( !m_Button2Active )
		m_LiftButtonsModel->SetMeshDrawState( LIFTBUTTONS_BUTTON_2_NORMAL, false );
	if( !m_Button3Active )
		m_LiftButtonsModel->SetMeshDrawState( LIFTBUTTONS_BUTTON_3_NORMAL, false );

	m_LiftButtonsUI.ChangeElementText( LIFTBUTTONS_LABEL_1, m_Button1Label );
	m_LiftButtonsUI.ChangeElementText( LIFTBUTTONS_LABEL_2, m_Button2Label );
	m_LiftButtonsUI.ChangeElementText( LIFTBUTTONS_LABEL_3, m_Button3Label );

	if( m_CurrentFloor == 1 )
	{
		m_LiftButtonsModel->SetMeshDrawState( LIFTBUTTONS_BUTTON_1_CURRENT, true );
		m_LiftButtonsModel->SetMeshDrawState( LIFTBUTTONS_BUTTON_1_NORMAL, false );
	}
	else
		m_LiftButtonsModel->SetMeshDrawState( LIFTBUTTONS_BUTTON_1_CURRENT, false );

	if( m_CurrentFloor == 2 )
	{
		m_LiftButtonsModel->SetMeshDrawState( LIFTBUTTONS_BUTTON_2_CURRENT, true );
		m_LiftButtonsModel->SetMeshDrawState( LIFTBUTTONS_BUTTON_2_NORMAL, false );
	}
	else
		m_LiftButtonsModel->SetMeshDrawState( LIFTBUTTONS_BUTTON_2_CURRENT, false );

	if( m_CurrentFloor == 3 )
	{
		m_LiftButtonsModel->SetMeshDrawState( LIFTBUTTONS_BUTTON_3_CURRENT, true );
		m_LiftButtonsModel->SetMeshDrawState( LIFTBUTTONS_BUTTON_3_NORMAL, false );
	}
	else
		m_LiftButtonsModel->SetMeshDrawState( LIFTBUTTONS_BUTTON_3_CURRENT, false );

	const char* sr = 0;
	snd::Sound sndLoad;

	sr = res::GetSoundResource( 160 );
	m_PressActiveAudio = snd::SoundLoad( sr, sndLoad );
	DBG_ASSERT( (m_PressActiveAudio != snd::INVALID_SOUNDBUFFER) );

	sr = res::GetSoundResource( 161 );
	m_PressInactiveAudio = snd::SoundLoad( sr, sndLoad );
	DBG_ASSERT( (m_PressInactiveAudio != snd::INVALID_SOUNDBUFFER) );
}

/////////////////////////////////////////////////////
/// Method: OnExit
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGameLiftButtons::OnExit()
{
	res::RemoveModel( m_LiftButtonsModel );
	m_LiftButtonsModel = 0;

	snd::RemoveSound( m_PressActiveAudio );
	snd::RemoveSound( m_PressInactiveAudio );
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void MicroGameLiftButtons::Update( float deltaTime )
{
	math::Vec3 zeroVec( 0.0f, 0.0f, 0.0f );

	// update UI elements
	m_LiftButtonsUI.Update( TOUCH_SIZE*0.5f, MAX_TOUCH_TESTS, deltaTime );

	if( m_FloorSelected )
	{
		m_ExitTimer += deltaTime;

		if( m_ExitTimer > 1.0f )
		{
			if( m_CallFunc && 
				!core::IsEmptyString(m_CallScriptFunc) )
			{
				int result = 0;
				int errorFuncIndex;
				errorFuncIndex = script::GetErrorFuncIndex();

				lua_getglobal( script::LuaScripting::GetState(), m_CallScriptFunc );

				result = lua_pcall( script::LuaScripting::GetState(), 0, 0, errorFuncIndex );

				// LUA_ERRRUN --- a runtime error. 
				// LUA_ERRMEM --- memory allocation error. For such errors, Lua does not call the error handler function. 
				// LUA_ERRERR --- error while running the error handler function. 

				if( result == LUA_ERRRUN || result == LUA_ERRMEM || result == LUA_ERRERR )
				{
					DBGLOG( "*ERROR* Calling function '%s' failed\n", m_CallScriptFunc );
					DBGLOG( "\tLUA_TRACEBACK: %s\n", lua_tostring( script::LuaScripting::GetState(), -1 ) );

					DBG_ASSERT_MSG( 0, "*ERROR* Calling function '%s' failed", m_CallScriptFunc );
				}
			}
			
			m_Complete = true;
			m_ReturnCode = true;
		}

		return;
	}


	if( m_Button1Active && 
		m_LiftButtonsUI.CheckElementForSingleTouch( LIFTBUTTONS_1 ) )
	{
		m_CallFunc = true;
		m_FloorSelected = true;
		m_CallScriptFunc = m_Button1Call;	

		m_SelectedFloor = 1;

		if(m_SelectedFloor != m_CurrentFloor)
		{
			m_LiftButtonsModel->SetMeshDrawState( LIFTBUTTONS_BUTTON_1_NORMAL, false );
			m_LiftButtonsModel->SetMeshDrawState( LIFTBUTTONS_BUTTON_1_DOWN, true );

			AudioSystem::GetInstance()->PlayAudio( m_PressActiveAudio, zeroVec, true );
		}
		else
			AudioSystem::GetInstance()->PlayAudio( m_PressInactiveAudio, zeroVec, true );
	}

	else if( m_Button2Active && 
		m_LiftButtonsUI.CheckElementForSingleTouch( LIFTBUTTONS_2 ) )
	{
		m_CallFunc = true;
		m_FloorSelected = true;
		m_CallScriptFunc = m_Button2Call;	

		m_SelectedFloor = 2;

		if(m_SelectedFloor != m_CurrentFloor)
		{
			m_LiftButtonsModel->SetMeshDrawState( LIFTBUTTONS_BUTTON_2_NORMAL, false );
			m_LiftButtonsModel->SetMeshDrawState( LIFTBUTTONS_BUTTON_2_DOWN, true );

			AudioSystem::GetInstance()->PlayAudio( m_PressActiveAudio, zeroVec, true );
		}
		else
			AudioSystem::GetInstance()->PlayAudio( m_PressInactiveAudio, zeroVec, true );
	}


	else if( m_Button3Active && 
		m_LiftButtonsUI.CheckElementForSingleTouch( LIFTBUTTONS_3 ) )
	{
		m_CallFunc = true;
		m_FloorSelected = true;
		m_CallScriptFunc = m_Button3Call;	

		m_SelectedFloor = 3;

		if(m_SelectedFloor != m_CurrentFloor)
		{
			m_LiftButtonsModel->SetMeshDrawState( LIFTBUTTONS_BUTTON_3_NORMAL, false );
			m_LiftButtonsModel->SetMeshDrawState( LIFTBUTTONS_BUTTON_3_DOWN, true );

			AudioSystem::GetInstance()->PlayAudio( m_PressActiveAudio, zeroVec, true );
		}
		else
			AudioSystem::GetInstance()->PlayAudio( m_PressInactiveAudio, zeroVec, true );
		
	}

	if( m_LiftButtonsUI.CheckElementForSingleTouch( LIFTBUTTONS_EXIT ) )
	{
		m_CallFunc = false;
		m_CallScriptFunc = 0;
		m_FloorSelected = true;
		m_SelectedFloor = -1;

		m_Complete = true;
		m_ReturnCode = true;

		return;
	}

	if(m_SelectedFloor == m_CurrentFloor)
	{
		m_CallFunc = false;
		m_CallScriptFunc = 0;
		m_FloorSelected = false;

		if( !m_WaitAudioDelay )
		{
			m_SelectedFloor = -1;

			m_WaitAudioDelay = true;
			m_AudioDelay = 2.0f;
		}
	}

	if( m_WaitAudioDelay )
	{
		m_AudioDelay -= deltaTime;
		if( m_AudioDelay <= 0.0f )
		{
			m_WaitAudioDelay = false;
			m_AudioDelay = 0.0f;
		}
	}
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGameLiftButtons::Draw()
{
	renderer::OpenGL::GetInstance()->ClearColour( 0.0f, 0.0f, 0.0f, 1.0f );

	renderer::OpenGL::GetInstance()->SetNearFarClip( -100.0f, 100.0f );
	renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_LESS );
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	renderer::OpenGL::GetInstance()->AlphaMode( false, GL_ALWAYS, 0.0f );

	m_LiftButtonsModel->Draw();

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_ALWAYS );
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	renderer::OpenGL::GetInstance()->SetNearFarClip( -100.0f, 100.0f );
	renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );

	m_LiftButtonsUI.Draw();
}
