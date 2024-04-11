
/*===================================================================
	File: MainStateUITablet.cpp
	Game: H4

	(C)Hidden Games
=====================================================================*/

#include "AppConsts.h"

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "SoundBase.h"
#include "CollisionBase.h"
#include "Input/Input.h"
#include "ModelBase.h"
#include "ScriptBase.h"

#include "AppConsts.h"
#include "H4.h"

#include "StateManage/IState.h"

#include "Resources/ModelResources.h"
#include "Resources/StringResources.h"

#include "GameStates/UI/UIBaseState.h"
#include "GameStates/UI/MainStateUITablet.h"
#include "GameStates/UI/UIFileLoader.h"

#include "GameEffects/FullscreenEffects.h"
#include "GameEffects/InsectAttack.h"

#include "GameSystems.h"
#include "MicroGame/MicroGameID.h"

#include "GameStates/UI/GeneratedFiles/hud_l.hgm.h"
#include "GameStates/UI/GeneratedFiles/hud_l.hui.h"

#include "GameStates/UI/GeneratedFiles/hud_r.hgm.h"
#include "GameStates/UI/GeneratedFiles/hud_r.hui.h"

#include "GameStates/UI/GeneratedFiles/optionsgame.hgm.h"
#include "GameStates/UI/GeneratedFiles/optionsgame.hui.h"

namespace
{
	input::Input keyboardInput;

	bool hudToggle = true;
	const int DEFAULT_NOISE_ALPHA = 12;

	const float ANALOGUE_RADIUS = 65.0f;
	const float ROTATE_MULT = 0.25f;
	const float LOOK_MULT = 0.25f;
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
MainStateUITablet::MainStateUITablet( StateManager& stateManager, InputSystem& inputSystem )
: MainStateUI( stateManager, inputSystem )
{

}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
MainStateUITablet::~MainStateUITablet()
{

}

/////////////////////////////////////////////////////
/// Method: Enter
/// Params: None
///
/////////////////////////////////////////////////////
void MainStateUITablet::Enter()
{
	m_Paused = false;
	m_Quit = false;
	m_ContinueFromDeath = false;

	m_CurrentOptions = GameSystems::GetInstance()->GetOptions();
	m_GeneralData = GameSystems::GetInstance()->GetGeneralData();

	math::Vec4Lite defaultColour( 255,255,255,255 );

	m_LeftHanded = m_CurrentOptions.leftHanded;

	m_InputSystem.SetAnalogueVal(ANALOGUE_RANGE_LARGE);
	
	if( m_LeftHanded )
	{
		m_UIMesh = res::LoadModel( 1010 );
		DBG_ASSERT( m_UIMesh != 0 );

		math::Vec3 scaleFactor = math::Vec3( core::app::GetWidthScale(), core::app::GetHeightScale(), 1.0f );

		math::Vec2 srcAssetDims( 768.0f, 1024.0f );
		m_Elements.Load( "hui/hud_l_tablet.hui", srcAssetDims, m_UIMesh );

		m_UIMesh->SetMeshDrawState( LEFTHAND_BTN_INTERACT, false );
		m_UIMesh->SetMeshDrawState( LEFTHAND_BTN_SHOOT, false );
		m_UIMesh->SetMeshDrawState( LEFTHAND_TORCH_BG, false );
		m_UIMesh->SetMeshDrawState( LEFTHAND_TORCH_GLARE, false );
		m_UIMesh->SetMeshDrawState( LEFTHAND_DAMAGE_BG, false );
		m_UIMesh->SetMeshDrawState( LEFTHAND_SHAKE_ALERT, false );

		m_UIMesh->SetMeshDrawState( LEFTHAND_BTN_RELOAD, false );
		m_UIMesh->SetMeshDrawState( LEFTHAND_BTN_RELOAD_ALT, false );

		//m_LeftDeadZone.vCenterPoint = m_Elements.GetElementPosition( LEFTHAND_UI_LEFTANALOGUE );
		//m_LeftDeadZone.fRadius = 20.0f;

		//m_RightDeadZone.vCenterPoint = m_Elements.GetElementPosition( LEFTHAND_UI_RIGHTANALOGUE );
		//m_RightDeadZone.fRadius = 20.0f;

		m_Elements.ChangeElementText( LEFTHAND_UI_LABEL_SPEECH_LINE1, "" );
		m_Elements.ChangeElementText( LEFTHAND_UI_LABEL_SPEECH_LINE2, "" );

		m_Elements.ChangeElementText( LEFTHAND_UI_LABEL_LEVELNAME, "" );
	}
	else
	{
		m_UIMesh = res::LoadModel( 1011 );
		DBG_ASSERT( m_UIMesh != 0 );

		math::Vec3 scaleFactor = math::Vec3( core::app::GetWidthScale(), core::app::GetHeightScale(), 1.0f );
		/*for( i=RIGHTHAND_TORCH_BG; i <= RIGHTHAND_SHAKE_ALERT; ++i )
		{		
			m_UIMesh->SetMeshScale(i, scaleFactor);
		}*/

		math::Vec2 srcAssetDims( 768.0f, 1024.0f );
		m_Elements.Load( "hui/hud_r_tablet.hui", srcAssetDims, m_UIMesh );

		m_UIMesh->SetMeshDrawState( RIGHTHAND_BTN_INTERACT, false );
		m_UIMesh->SetMeshDrawState( RIGHTHAND_BTN_SHOOT, false );
		m_UIMesh->SetMeshDrawState( RIGHTHAND_TORCH_BG, false );
		m_UIMesh->SetMeshDrawState( RIGHTHAND_TORCH_GLARE, false );
		m_UIMesh->SetMeshDrawState( RIGHTHAND_DAMAGE_BG, false );
		m_UIMesh->SetMeshDrawState( RIGHTHAND_SHAKE_ALERT, false );

		m_UIMesh->SetMeshDrawState( RIGHTHAND_BTN_RELOAD, false );
		m_UIMesh->SetMeshDrawState( RIGHTHAND_BTN_RELOAD_ALT, false );

		m_Elements.ChangeElementText( RIGHTHAND_UI_LABEL_SPEECH_LINE1, "" );
		m_Elements.ChangeElementText( RIGHTHAND_UI_LABEL_SPEECH_LINE2, "" );

		m_Elements.ChangeElementText( RIGHTHAND_UI_LABEL_LEVELNAME, "" );
	}

	m_OptionsMesh = res::LoadModel( 1004 );
	DBG_ASSERT( m_OptionsMesh != 0 );

	math::Vec3 scaleFactor = math::Vec3( core::app::GetWidthScale(), core::app::GetHeightScale(), 1.0f );

	m_OptionsMesh->SetDefaultMeshColour(defaultColour);
	m_OptionsMesh->EnableDefaultMeshColour();

	math::Vec2 srcAssetDims( static_cast<float>(core::app::GetBaseAssetsWidth()), static_cast<float>(core::app::GetBaseAssetsHeight()) );
	m_OptionsElements.Load( "hui/optionsgame.hui", srcAssetDims, m_OptionsMesh );

	/*if( !core::app::IsGameCenterAvailable() ||
	   !core::app::IsNetworkAvailable() )
	{
		m_OptionsElements.ChangeElementDrawState( OPTIONSGAME_UI_GLYPH_ACHIEVEMENT, false );
	}*/

	m_ControlTypeIndex = 1;//m_CurrentOptions.controlType;

	m_NoiseHeld = false;
	
	m_NoiseBarStartPos = m_OptionsElements.GetElementPosition(OPTIONSGAME_UI_BTN_SCRUBBER); 
	m_NoiseBarMinRange = m_OptionsElements.GetElementPosition(OPTIONSGAME_UI_NOISE_RANGE_MIN);
	m_NoiseBarMaxRange = m_OptionsElements.GetElementPosition(OPTIONSGAME_UI_NOISE_RANGE_MAX);

	m_NoiseBarRange = static_cast<int>( (m_NoiseBarMaxRange.X - m_NoiseBarMinRange.X) / 10.0f );
	m_NoiseBarXPos = (m_CurrentOptions.noiseFilter * m_NoiseBarRange);

	math::Vec3 barPos = math::Vec3( m_NoiseBarStartPos.X+m_NoiseBarXPos, m_NoiseBarStartPos.Y, m_NoiseBarStartPos.Z );
	m_OptionsElements.ChangeElementPosition(OPTIONSGAME_UI_BTN_SCRUBBER, barPos);

	m_CurrentOptions.noiseFilter = (m_NoiseBarXPos / m_NoiseBarRange);

	int defaultIncrease = 2*(m_CurrentOptions.noiseFilter-5);
	if( defaultIncrease < -DEFAULT_NOISE_ALPHA )
		defaultIncrease = -DEFAULT_NOISE_ALPHA;

	SetNoiseAlpha( DEFAULT_NOISE_ALPHA+defaultIncrease );

	math::Vec3 barMeshPos = math::Vec3( static_cast<float>(m_NoiseBarXPos), 0.0f, 0.0f );
	//m_OptionsMesh->SetMeshTranslation( SCRUBBER_INACTIVE, barMeshPos);
	//m_OptionsMesh->SetMeshTranslation( SCRUBBER_ACTIVE, barMeshPos);

	m_TorchSway = math::Vec3( 0.0f, 0.0f, 0.0f );

	m_Say1Index = -1;
	m_Say2Index = -1;
	m_SayCallFunc = 0;

	m_SayLine1 = 0;
	m_SayLine2 = 0;
	
	if( m_GeneralData.fpsModeUnlocked )
	{
		Player& player = GameSystems::GetInstance()->GetPlayer();
		if( m_CurrentOptions.fpsMode )
		{
			player.EnableFPSMode();
			GameSystems::GetInstance()->GetGameCamera()->SetLerp(false);	
		}
		else
		{
			player.DisableFPSMode();
			GameSystems::GetInstance()->GetGameCamera()->SetLerp(m_CurrentOptions.camLerp);		
		}
	}
}

/////////////////////////////////////////////////////
/// Method: Exit
/// Params: None
///
/////////////////////////////////////////////////////
void MainStateUITablet::Exit()
{
	res::RemoveModel( m_UIMesh );
	m_UIMesh = 0;

	res::RemoveModel( m_OptionsMesh );
	m_UIMesh = 0;
}

/////////////////////////////////////////////////////
/// Method: TransitionIn
/// Params: None
///
/////////////////////////////////////////////////////
int MainStateUITablet::TransitionIn()
{
	//core::app::SetLandscape();

	return(0);
}

/////////////////////////////////////////////////////
/// Method: TransitionOut
/// Params: None
///
/////////////////////////////////////////////////////
int MainStateUITablet::TransitionOut()
{

	return(0);
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void MainStateUITablet::Update( float deltaTime )
{
	Player& player = GameSystems::GetInstance()->GetPlayer();

	if( GameSystems::GetInstance()->GetChangeStageFlag() )
		return;

	// micro game takes complete control
	if( GameSystems::GetInstance()->IsMicroGameActive() )
		return;

	if( m_Paused )
		UpdatePauseUI( deltaTime );
	else
	{
		if( m_LeftHanded )
			UpdateLeftGameUI( deltaTime );
		else
			UpdateRightGameUI( deltaTime );
	}

	if( player.IsDead() )
	{
		if( m_LeftHanded )
		{
			//for( i=LEFTHAND_TORCH_BG; i <= LEFTHAND_SHAKE_ALERT; ++i )
				m_UIMesh->SetMeshDrawState( -1, false );

			m_UIMesh->SetMeshDrawState( LEFTHAND_DAMAGE_BG, true );
		}
		else
		{
			//for( i=RIGHTHAND_TORCH_BG; i <= RIGHTHAND_SHAKE_ALERT; ++i )
				m_UIMesh->SetMeshDrawState( -1, false );

			m_UIMesh->SetMeshDrawState( RIGHTHAND_DAMAGE_BG, true );
		}
	}
}

/////////////////////////////////////////////////////
/// Method: UpdateLeftGameUI
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void MainStateUITablet::UpdateLeftGameUI( float deltaTime )
{
	int i=0, j=0;
	Player& player = GameSystems::GetInstance()->GetPlayer();

	if( keyboardInput.IsKeyPressed( input::KEY_3, true ) )
	{
		hudToggle = !hudToggle;
	}

	m_LastDeltaTime = deltaTime;
	m_Elements.Update( TOUCH_SIZE*0.5f, input::MAX_TOUCHES, deltaTime );

	if( IsInsectAttackActive() )
		m_UIMesh->SetMeshDrawState( LEFTHAND_SHAKE_ALERT, true );
	else
		m_UIMesh->SetMeshDrawState( LEFTHAND_SHAKE_ALERT, false );

	if( player.DamageFlag() )
		m_UIMesh->SetMeshDrawState( LEFTHAND_DAMAGE_BG, true );
	else
		m_UIMesh->SetMeshDrawState( LEFTHAND_DAMAGE_BG, false );

	m_UIMesh->SetMeshDrawState( LEFTHAND_BTN_PAUSE, true );
	m_UIMesh->SetMeshDrawState( LEFTHAND_BTN_INVENTORY, true );

	bool useTorch = player.TorchEnabled();
	m_UIMesh->SetMeshDrawState( LEFTHAND_TORCH_BG, useTorch );
	m_UIMesh->SetMeshDrawState( LEFTHAND_TORCH_GLARE, useTorch );

	if( !GameSystems::GetInstance()->IsTalking() )
	{
		if( m_Elements.CheckElementForSingleTouch( LEFTHAND_UI_BTN_SHOOT ) )
			m_InputSystem.SetAction(true);
		else
			m_InputSystem.SetAction(false);

		if( m_Elements.CheckElementForSingleTouch( LEFTHAND_UI_BTN_INTERACT ) )
			m_InputSystem.SetUse(true);
		else
			m_InputSystem.SetUse(false);

		/*if( m_Elements.CheckElementForSingleTouch( LEFTHAND_UI_WEAPON ) )
			m_InputSystem.SetWeaponSwap(true);
		else
			m_InputSystem.SetWeaponSwap(false);*/

		if( m_Elements.CheckElementForSingleTouch( LEFTHAND_UI_BTN_RELOAD ) )
			m_InputSystem.SetReload(true);
		else
			m_InputSystem.SetReload(false);

		if( m_Elements.CheckElementForSingleTouch( LEFTHAND_UI_BTN_RELOAD_ALT ) )
			m_InputSystem.SetReload(true);
		else
			m_InputSystem.SetReload(false);

		if( m_Elements.CheckElementForSingleTouch( LEFTHAND_UI_BTN_PAUSE ) )
		{
			if( support::Scores::IsInitialised() )
				support::Scores::GetInstance()->SyncAchievements();
			
			m_Paused = true;
			AudioSystem::GetInstance()->Pause();
			UpdateOptions();
		}	

		if( m_Elements.CheckElementForSingleTouch( LEFTHAND_UI_BTN_INVENTORY ) )
		{
			GameSystems::GetInstance()->LaunchMicroGame( MICROGAME_INVENTORY, 0 );
			return;
		}
	}

	m_InputSystem.SetLeftVal(0.0f);
	m_InputSystem.SetRightVal(0.0f);
	m_InputSystem.SetUpVal(0.0f);
	m_InputSystem.SetDownVal(0.0f);
	
	m_InputSystem.SetLookUpVal(0.0f);
	m_InputSystem.SetLookDownVal(0.0f);
	m_InputSystem.SetRotateLeftVal(0.0f);
	m_InputSystem.SetRotateRightVal(0.0f);

	m_InputSystem.SetUp(false);
	m_InputSystem.SetDown(false);
	m_InputSystem.SetLeft(false);
	m_InputSystem.SetRight(false);

	m_InputSystem.SetLookUp(false);
	m_InputSystem.SetLookDown(false);
	m_InputSystem.SetRotateLeft(false);
	m_InputSystem.SetRotateRight(false);

	// say
	if( m_Say1Index == -1 && m_Say2Index == -1 )
	{
		GameSystems::GetInstance()->GetSay( &m_Say1Index, &m_Say2Index, &m_SayCallFunc );

		if( m_Say1Index != -1 )
			m_SayLine1 = res::GetScriptString( m_Say1Index );
		else
			m_SayLine1 = 0;

		if( m_Say2Index != -1 )
			m_SayLine2 = res::GetScriptString( m_Say2Index );
		else
			m_SayLine2 = 0;

		if( m_Say1Index != -1 )
		{
			// got the strings, clear out the script side
			GameSystems::GetInstance()->ClearText();

			// lock
			GameSystems::GetInstance()->SetTalkState( true );
		}
	}

	if( m_Say1Index != -1 )
		m_Elements.ChangeElementText( LEFTHAND_UI_LABEL_SPEECH_LINE1, m_SayLine1 );
	else
		m_Elements.ChangeElementText( LEFTHAND_UI_LABEL_SPEECH_LINE1, "" );

	if( m_Say2Index != -1 )
		m_Elements.ChangeElementText( LEFTHAND_UI_LABEL_SPEECH_LINE2, m_SayLine2 );
	else
		m_Elements.ChangeElementText( LEFTHAND_UI_LABEL_SPEECH_LINE2, "" );

	// update the movement and camera based on the type
	if( m_ControlTypeIndex == 1 )
		UpdateType4( deltaTime );
	//if( m_ControlTypeIndex == 2 )
	//	UpdateType6( deltaTime );
	
	// handle ipod run via double tap
#if defined (BASE_PLATFORM_iOS) || defined (BASE_PLATFORM_ANDROID)
	if( m_InputSystem.GetUpVal() != 0.0f )
	{
		int touchIndex = m_InputSystem.GetUpTouchIndex();
		if( touchIndex != -1 )
		{
			const input::TInputState::TouchData* pData = 0;
			pData = input::GetTouch(touchIndex);	
					
			if( pData->nTaps == 2 || 
				pData->Ticks >= 4.0f)
				m_InputSystem.SetRun(true);
			else
				m_InputSystem.SetRun(false);
		}
	}
#endif // (BASE_PLATFORM_iOS) || (BASE_PLATFORM_ANDROID)

	// torch
	const float TORCH_RANGE = 70.0f;
	float TORCH_MOVE_SPEED = 50.0f;

	m_TorchPos = m_Elements.GetElementPosition( LEFTHAND_UI_CENTER_POS2 );

	if( m_InputSystem.GetRotateLeft() || 
		m_InputSystem.GetLeft() )
	{
		if( m_TorchSway.X > -TORCH_RANGE )
			m_TorchSway.X -= TORCH_MOVE_SPEED*deltaTime;
	}
	else if( m_InputSystem.GetRotateRight() || 
			m_InputSystem.GetRight() )
	{
		if( m_TorchSway.X < TORCH_RANGE )
			m_TorchSway.X += TORCH_MOVE_SPEED*deltaTime;
	}
	else
	{
		float TORCH_SWAY_SPEED = 25.0f;
		if( m_TorchSway.X < 0.0f )
		{
			if( std::abs( m_TorchSway.X ) < 2.0f )
				m_TorchSway.X = 0.0f;
			else
				m_TorchSway.X += TORCH_SWAY_SPEED*deltaTime;
		}
		else if( m_TorchSway.X > 0.0f )
		{
			if( std::abs( m_TorchSway.X ) < 2.0f )
				m_TorchSway.X = 0.0f;
			else
				m_TorchSway.X -= TORCH_SWAY_SPEED*deltaTime;
		}
	}

	static bool swap = true;
	static float val = 0.0f;

	if( swap )
	{
		val += 2.0f*deltaTime;
		if( val > 1.0f )
		{
			val = 1.0f;
			swap = false;
		}
	}
	else
	{
		val -= 2.0f*deltaTime;
		if( val < -1.0f )
		{
			val = -1.0f;
			swap = true;
		}
	}
	m_TorchSway.Y = 3.0f*std::sin( val );

	m_UIMesh->SetMeshTranslation( LEFTHAND_TORCH_BG, m_TorchPos+m_TorchSway );
	m_UIMesh->SetMeshTranslation( LEFTHAND_TORCH_GLARE, m_TorchPos+m_TorchSway );

	// display various meshes
	if( player.InUseArea() )
	{
		m_UIMesh->SetMeshDrawState( LEFTHAND_BTN_INTERACT, true );
	}
	else
	{
		m_UIMesh->SetMeshDrawState( LEFTHAND_BTN_INTERACT, false );
	}

	if( player.GetWeaponIndex() == WEAPONINDEX_PISTOL )
	{
		m_UIMesh->SetMeshDrawState( LEFTHAND_BTN_SHOOT, true );
		m_UIMesh->SetMeshDrawState( LEFTHAND_PANEL1, true );
		m_UIMesh->SetMeshDrawState( LEFTHAND_PISTOL, true );
		m_UIMesh->SetMeshDrawState( LEFTHAND_SHOTGUN, false );

		m_UIMesh->SetMeshDrawState( LEFTHAND_PISTOL_SHELLS_BG, true );
		m_UIMesh->SetMeshDrawState( LEFTHAND_SHOTGUN_SHELLS_BG, false );

		char text[UI_MAXSTATICTEXTBUFFER_SIZE];
		snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "%d/%d", player.GetPistolBulletMagCount(), player.GetPistolBulletTotal() );
		m_Elements.ChangeElementText( LEFTHAND_UI_LABEL_AMMO, text );

		if( player.GetPistolBulletMagCount() <= 0 && !player.IsReloading() )
			m_UIMesh->SetMeshDrawState( LEFTHAND_BTN_RELOAD, true );
		else
			m_UIMesh->SetMeshDrawState( LEFTHAND_BTN_RELOAD, false );

		if( player.GetPistolBulletMagCount() <= 0 && !player.IsReloading() )
			m_UIMesh->SetMeshDrawState( LEFTHAND_BTN_RELOAD_ALT, true );
		else
			m_UIMesh->SetMeshDrawState( LEFTHAND_BTN_RELOAD_ALT, false );

		for( i=LEFTHAND_SHELL1, j=0; i <= LEFTHAND_SHELL12; ++i, ++j )
		{
			if( player.GetPistolBulletMagCount() >= j+1 )
			{
				m_UIMesh->SetMeshDrawState( i, true );
			}
			else
				m_UIMesh->SetMeshDrawState( i, false );
		}
	}
	else if( player.GetWeaponIndex() == WEAPONINDEX_SHOTGUN )
	{
		m_UIMesh->SetMeshDrawState( LEFTHAND_BTN_SHOOT, true );
		m_UIMesh->SetMeshDrawState( LEFTHAND_PANEL1, true );
		m_UIMesh->SetMeshDrawState( LEFTHAND_PISTOL, false );
		m_UIMesh->SetMeshDrawState( LEFTHAND_SHOTGUN, true );
		
		m_UIMesh->SetMeshDrawState( LEFTHAND_PISTOL_SHELLS_BG, false );
		m_UIMesh->SetMeshDrawState( LEFTHAND_SHOTGUN_SHELLS_BG, true );

		char text[UI_MAXSTATICTEXTBUFFER_SIZE];
		snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "%d/%d", player.GetShotgunBulletMagCount(), player.GetShotgunBulletTotal() );
		m_Elements.ChangeElementText( LEFTHAND_UI_LABEL_AMMO, text );

		if( player.GetShotgunBulletMagCount() <= 0 )
			m_UIMesh->SetMeshDrawState( LEFTHAND_BTN_RELOAD, true );
		else
			m_UIMesh->SetMeshDrawState( LEFTHAND_BTN_RELOAD, false );

		if( player.GetShotgunBulletMagCount() <= 0 )
			m_UIMesh->SetMeshDrawState( LEFTHAND_BTN_RELOAD_ALT, true );
		else
			m_UIMesh->SetMeshDrawState( LEFTHAND_BTN_RELOAD_ALT, false );

		for( i=LEFTHAND_SHELL1, j=0; i <= LEFTHAND_SHELL12; ++i, ++j )
		{
			if( player.GetShotgunBulletMagCount() >= j+1 )
			{
				m_UIMesh->SetMeshDrawState( i, true );
			}
			else
				m_UIMesh->SetMeshDrawState( i, false );
		}
	}
	else
	{
		m_UIMesh->SetMeshDrawState( LEFTHAND_BTN_SHOOT, false );
		m_UIMesh->SetMeshDrawState( LEFTHAND_PANEL1, false );
		m_UIMesh->SetMeshDrawState( LEFTHAND_PISTOL, false );
		m_UIMesh->SetMeshDrawState( LEFTHAND_SHOTGUN, false );

		m_UIMesh->SetMeshDrawState( LEFTHAND_BTN_RELOAD, false );
		m_UIMesh->SetMeshDrawState( LEFTHAND_BTN_RELOAD_ALT, false );

		char text[UI_MAXSTATICTEXTBUFFER_SIZE];
		snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "" );
		m_Elements.ChangeElementText( LEFTHAND_UI_LABEL_AMMO, text );

		m_UIMesh->SetMeshDrawState( LEFTHAND_PISTOL_SHELLS_BG, false );
		m_UIMesh->SetMeshDrawState( LEFTHAND_SHOTGUN_SHELLS_BG, false );

		for( i=LEFTHAND_SHELL1, j=0; i <= LEFTHAND_SHELL12; ++i, ++j )
		{
			m_UIMesh->SetMeshDrawState( i, false );
		}
	}
}

/////////////////////////////////////////////////////
/// Method: UpdateRightGameUI
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void MainStateUITablet::UpdateRightGameUI( float deltaTime )
{
	int i=0, j=0;
	Player& player = GameSystems::GetInstance()->GetPlayer();

	if( keyboardInput.IsKeyPressed( input::KEY_3, true ) )
	{
		hudToggle = !hudToggle;
	}

	m_LastDeltaTime = deltaTime;
	m_Elements.Update( TOUCH_SIZE*0.5f, input::MAX_TOUCHES, deltaTime );

	if( IsInsectAttackActive() )
		m_UIMesh->SetMeshDrawState( RIGHTHAND_SHAKE_ALERT, true );
	else
		m_UIMesh->SetMeshDrawState( RIGHTHAND_SHAKE_ALERT, false );

	if( player.DamageFlag() )
		m_UIMesh->SetMeshDrawState( RIGHTHAND_DAMAGE_BG, true );
	else
		m_UIMesh->SetMeshDrawState( RIGHTHAND_DAMAGE_BG, false );

	m_UIMesh->SetMeshDrawState( RIGHTHAND_BTN_PAUSE, true );
	m_UIMesh->SetMeshDrawState( RIGHTHAND_BTN_INVENTORY, true );

	bool useTorch = player.TorchEnabled();
	m_UIMesh->SetMeshDrawState( RIGHTHAND_TORCH_BG, useTorch );
	m_UIMesh->SetMeshDrawState( RIGHTHAND_TORCH_GLARE, useTorch );

	if( !GameSystems::GetInstance()->IsTalking() )
	{
		if( m_Elements.CheckElementForSingleTouch( RIGHTHAND_UI_BTN_SHOOT ) )
			m_InputSystem.SetAction(true);
		else
			m_InputSystem.SetAction(false);

		if( m_Elements.CheckElementForSingleTouch( RIGHTHAND_UI_BTN_INTERACT ) )
			m_InputSystem.SetUse(true);
		else
			m_InputSystem.SetUse(false);

		/*if( m_Elements.CheckElementForSingleTouch( RIGHTHAND_UI_WEAPON ) )
			m_InputSystem.SetWeaponSwap(true);
		else
			m_InputSystem.SetWeaponSwap(false);*/

		if( m_Elements.CheckElementForSingleTouch( RIGHTHAND_UI_BTN_RELOAD ) )
			m_InputSystem.SetReload(true);
		else
			m_InputSystem.SetReload(false);

		if( m_Elements.CheckElementForSingleTouch( RIGHTHAND_UI_BTN_RELOAD_ALT ) )
			m_InputSystem.SetReload(true);
		else
			m_InputSystem.SetReload(false);

		if( m_Elements.CheckElementForSingleTouch( RIGHTHAND_UI_BTN_PAUSE ) )
		{
			if( support::Scores::IsInitialised() )
				support::Scores::GetInstance()->SyncAchievements();
			
			m_Paused = true;
			AudioSystem::GetInstance()->Pause();
			UpdateOptions();
		}	

		if( m_Elements.CheckElementForSingleTouch( RIGHTHAND_UI_BTN_INVENTORY ) )
		{
			GameSystems::GetInstance()->LaunchMicroGame( MICROGAME_INVENTORY, 0 );
			return;
		}
	}

	m_InputSystem.SetLeftVal(0.0f);
	m_InputSystem.SetRightVal(0.0f);
	m_InputSystem.SetUpVal(0.0f);
	m_InputSystem.SetDownVal(0.0f);
	
	m_InputSystem.SetLookUpVal(0.0f);
	m_InputSystem.SetLookDownVal(0.0f);
	m_InputSystem.SetRotateLeftVal(0.0f);
	m_InputSystem.SetRotateRightVal(0.0f);

	m_InputSystem.SetUp(false);
	m_InputSystem.SetDown(false);
	m_InputSystem.SetLeft(false);
	m_InputSystem.SetRight(false);

	m_InputSystem.SetLookUp(false);
	m_InputSystem.SetLookDown(false);
	m_InputSystem.SetRotateLeft(false);
	m_InputSystem.SetRotateRight(false);

	// say
	if( m_Say1Index == -1 && m_Say2Index == -1 )
	{
		GameSystems::GetInstance()->GetSay( &m_Say1Index, &m_Say2Index, &m_SayCallFunc );

		if( m_Say1Index != -1 )
			m_SayLine1 = res::GetScriptString( m_Say1Index );
		else
			m_SayLine1 = 0;

		if( m_Say2Index != -1 )
			m_SayLine2 = res::GetScriptString( m_Say2Index );
		else
			m_SayLine2 = 0;

		if( m_Say1Index != -1 )
		{
			// got the strings, clear out the script side
			GameSystems::GetInstance()->ClearText();

			// lock
			GameSystems::GetInstance()->SetTalkState( true );
		}
	}

	if( m_Say1Index != -1 )
		m_Elements.ChangeElementText( RIGHTHAND_UI_LABEL_SPEECH_LINE1, m_SayLine1 );
	else
		m_Elements.ChangeElementText( RIGHTHAND_UI_LABEL_SPEECH_LINE1, "" );

	if( m_Say2Index != -1 )
		m_Elements.ChangeElementText( RIGHTHAND_UI_LABEL_SPEECH_LINE2, m_SayLine2 );
	else
		m_Elements.ChangeElementText( RIGHTHAND_UI_LABEL_SPEECH_LINE2, "" );

	// update the movement and camera based on the type
	if( m_ControlTypeIndex == 1 )
		UpdateType1( deltaTime );
	//if( m_ControlTypeIndex == 2 )
	//	UpdateType3( deltaTime );
	
	// handle ipod run via double tap
#if defined (BASE_PLATFORM_iOS) || defined (BASE_PLATFORM_ANDROID)
	if( m_InputSystem.GetUpVal() != 0.0f )
	{
		int touchIndex = m_InputSystem.GetUpTouchIndex();
		if( touchIndex != -1 )
		{
			const input::TInputState::TouchData* pData = 0;
			pData = input::GetTouch(touchIndex);	
					
			if( pData->nTaps == 2 || 
				pData->Ticks >= 4.0f)
				m_InputSystem.SetRun(true);
			else
				m_InputSystem.SetRun(false);
		}
	}
#endif // (BASE_PLATFORM_iOS) || (BASE_PLATFORM_ANDROID)

	// torch
	const float TORCH_RANGE = 70.0f;
	float TORCH_MOVE_SPEED = 50.0f;

	m_TorchPos = m_Elements.GetElementPosition( RIGHTHAND_UI_CENTER_POS2 );

	if( m_InputSystem.GetRotateLeft() || 
		m_InputSystem.GetLeft() )
	{
		if( m_TorchSway.X > -TORCH_RANGE )
			m_TorchSway.X -= TORCH_MOVE_SPEED*deltaTime;
	}
	else if( m_InputSystem.GetRotateRight() || 
			m_InputSystem.GetRight() )
	{
		if( m_TorchSway.X < TORCH_RANGE )
			m_TorchSway.X += TORCH_MOVE_SPEED*deltaTime;
	}
	else
	{
		float TORCH_SWAY_SPEED = 25.0f;
		if( m_TorchSway.X < 0.0f )
		{
			if( std::abs( m_TorchSway.X ) < 2.0f )
				m_TorchSway.X = 0.0f;
			else
				m_TorchSway.X += TORCH_SWAY_SPEED*deltaTime;
		}
		else if( m_TorchSway.X > 0.0f )
		{
			if( std::abs( m_TorchSway.X ) < 2.0f )
				m_TorchSway.X = 0.0f;
			else
				m_TorchSway.X -= TORCH_SWAY_SPEED*deltaTime;
		}
	}

	static bool swap = true;
	static float val = 0.0f;

	if( swap )
	{
		val += 2.0f*deltaTime;
		if( val > 1.0f )
		{
			val = 1.0f;
			swap = false;
		}
	}
	else
	{
		val -= 2.0f*deltaTime;
		if( val < -1.0f )
		{
			val = -1.0f;
			swap = true;
		}
	}
	m_TorchSway.Y = 3.0f*std::sin( val );

	m_UIMesh->SetMeshTranslation( RIGHTHAND_TORCH_BG, m_TorchPos+m_TorchSway );
	m_UIMesh->SetMeshTranslation( RIGHTHAND_TORCH_GLARE, m_TorchPos+m_TorchSway );

	// display various meshes
	if( player.InUseArea() )
	{
		m_UIMesh->SetMeshDrawState( RIGHTHAND_BTN_INTERACT, true );
	}
	else
	{
		m_UIMesh->SetMeshDrawState( RIGHTHAND_BTN_INTERACT, false );
	}

	if( player.GetWeaponIndex() == WEAPONINDEX_PISTOL )
	{
		m_UIMesh->SetMeshDrawState( RIGHTHAND_BTN_SHOOT, true );
		m_UIMesh->SetMeshDrawState( RIGHTHAND_PANEL1, true );
		m_UIMesh->SetMeshDrawState( RIGHTHAND_PISTOL, true );
		m_UIMesh->SetMeshDrawState( RIGHTHAND_SHOTGUN, false );

		m_UIMesh->SetMeshDrawState( RIGHTHAND_PISTOL_SHELLS_BG, true );
		m_UIMesh->SetMeshDrawState( RIGHTHAND_SHOTGUN_SHELLS_BG, false );

		char text[UI_MAXSTATICTEXTBUFFER_SIZE];
		snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "%d/%d", player.GetPistolBulletMagCount(), player.GetPistolBulletTotal() );
		m_Elements.ChangeElementText( RIGHTHAND_UI_LABEL_AMMO, text );

		if( player.GetPistolBulletMagCount() <= 0 && !player.IsReloading() )
			m_UIMesh->SetMeshDrawState( RIGHTHAND_BTN_RELOAD, true );
		else
			m_UIMesh->SetMeshDrawState( RIGHTHAND_BTN_RELOAD, false );

		if( player.GetPistolBulletMagCount() <= 0 && !player.IsReloading() )
			m_UIMesh->SetMeshDrawState( RIGHTHAND_BTN_RELOAD_ALT, true );
		else
			m_UIMesh->SetMeshDrawState( RIGHTHAND_BTN_RELOAD_ALT, false );

		for( i=RIGHTHAND_SHELL1, j=0; i <= RIGHTHAND_SHELL12; ++i, ++j )
		{
			if( player.GetPistolBulletMagCount() >= j+1 )
			{
				m_UIMesh->SetMeshDrawState( i, true );
			}
			else
				m_UIMesh->SetMeshDrawState( i, false );
		}
	}
	else if( player.GetWeaponIndex() == WEAPONINDEX_SHOTGUN )
	{
		m_UIMesh->SetMeshDrawState( RIGHTHAND_BTN_SHOOT, true );
		m_UIMesh->SetMeshDrawState( RIGHTHAND_PANEL1, true );
		m_UIMesh->SetMeshDrawState( RIGHTHAND_PISTOL, false );
		m_UIMesh->SetMeshDrawState( RIGHTHAND_SHOTGUN, true );

		m_UIMesh->SetMeshDrawState( RIGHTHAND_PISTOL_SHELLS_BG, false );
		m_UIMesh->SetMeshDrawState( RIGHTHAND_SHOTGUN_SHELLS_BG, true );

		char text[UI_MAXSTATICTEXTBUFFER_SIZE];
		snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "%d/%d", player.GetShotgunBulletMagCount(), player.GetShotgunBulletTotal() );
		m_Elements.ChangeElementText( RIGHTHAND_UI_LABEL_AMMO, text );

		if( player.GetShotgunBulletMagCount() <= 0 )
			m_UIMesh->SetMeshDrawState( RIGHTHAND_BTN_RELOAD, true );
		else
			m_UIMesh->SetMeshDrawState( RIGHTHAND_BTN_RELOAD, false );

		if( player.GetShotgunBulletMagCount() <= 0 )
			m_UIMesh->SetMeshDrawState( RIGHTHAND_BTN_RELOAD_ALT, true );
		else
			m_UIMesh->SetMeshDrawState( RIGHTHAND_BTN_RELOAD_ALT, false );

		for( i=RIGHTHAND_SHELL1, j=0; i <= RIGHTHAND_SHELL12; ++i, ++j )
		{
			if( player.GetShotgunBulletMagCount() >= j+1 )
			{
				m_UIMesh->SetMeshDrawState( i, true );
			}
			else
				m_UIMesh->SetMeshDrawState( i, false );
		}
	}
	else
	{
		m_UIMesh->SetMeshDrawState( RIGHTHAND_BTN_SHOOT, false );
		m_UIMesh->SetMeshDrawState( RIGHTHAND_PANEL1, false );
		m_UIMesh->SetMeshDrawState( RIGHTHAND_PISTOL, false );
		m_UIMesh->SetMeshDrawState( RIGHTHAND_SHOTGUN, false );

		m_UIMesh->SetMeshDrawState( RIGHTHAND_PISTOL_SHELLS_BG, false );
		m_UIMesh->SetMeshDrawState( RIGHTHAND_SHOTGUN_SHELLS_BG, false );

		m_UIMesh->SetMeshDrawState( RIGHTHAND_BTN_RELOAD, false );
		m_UIMesh->SetMeshDrawState( RIGHTHAND_BTN_RELOAD_ALT, false );

		char text[UI_MAXSTATICTEXTBUFFER_SIZE];
		snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "" );
		m_Elements.ChangeElementText( RIGHTHAND_UI_LABEL_AMMO, text );

		for( i=RIGHTHAND_SHELL1, j=0; i <= RIGHTHAND_SHELL12; ++i, ++j )
		{
			m_UIMesh->SetMeshDrawState( i, false );
		}
	}
}

/////////////////////////////////////////////////////
/// Method: UpdatePauseUI
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void MainStateUITablet::UpdatePauseUI( float deltaTime )
{
	m_LastDeltaTime = deltaTime;
	Player& player = GameSystems::GetInstance()->GetPlayer();

	m_OptionsElements.Update( TOUCH_SIZE, 1, deltaTime );

	// quit
	if( m_OptionsElements.CheckElementForSingleTouch(OPTIONSGAME_UI_BTN_QUIT) )
	{
		m_Quit = true;
		GameSystems::GetInstance()->SaveOptions();
		return;
	}

	if( core::app::IsGameCenterAvailable() && 
		m_OptionsElements.CheckElementForTouch(OPTIONSGAME_UI_GLYPH_ACHIEVEMENT) )
	{
		/*if( core::app::IsNetworkAvailable() &&
		   support::Scores::GetInstance()->IsLoggedIn())*/
		{		
			support::Scores::GetInstance()->ShowAchievements();
		}
	}

	// crosshair
	/*if( m_OptionsElements.CheckElementForSingleTouch(OPTIONSGAME_TOGGLE_CROSSHAIROFF) )
	{
		m_CurrentOptions.crosshair = false;
		UpdateOptions();
	}
	else if( m_OptionsElements.CheckElementForSingleTouch(OPTIONSGAME_TOGGLE_CROSSHAIRON) )
	{
		m_CurrentOptions.crosshair = true;
		UpdateOptions();
	}*/

	// cam lerp
	if( m_OptionsElements.CheckElementForSingleTouch(OPTIONSGAME_UI_BTN_LERP_OFF) )
	{
		m_CurrentOptions.camLerp = false;
		GameSystems::GetInstance()->GetGameCamera()->SetLerp(m_CurrentOptions.camLerp);
		UpdateOptions();
	}
	else if( m_OptionsElements.CheckElementForSingleTouch(OPTIONSGAME_UI_BTN_LERP_ON) )
	{
		m_CurrentOptions.camLerp = true;
		GameSystems::GetInstance()->GetGameCamera()->SetLerp(m_CurrentOptions.camLerp);
		UpdateOptions();
	}

	// noise
	if(m_OptionsElements.CheckElementForTouch(OPTIONSGAME_UI_BTN_SCRUBBER, UIFileLoader::EVENT_PRESS) || 
	   m_OptionsElements.CheckElementForTouch(OPTIONSGAME_UI_BTN_SCRUBBER, UIFileLoader::EVENT_HELD) )
	{
		m_NoiseHeld = true;
		UpdateOptions();

		math::Vec3 touchPos = m_OptionsElements.GetLastTouchPosition( OPTIONSGAME_UI_BTN_SCRUBBER );

		if( touchPos.X < m_NoiseBarMinRange.X )
			touchPos.X = m_NoiseBarMinRange.X;

		if( touchPos.X > m_NoiseBarMaxRange.X )
			touchPos.X = m_NoiseBarMaxRange.X;

		m_NoiseBarXPos = static_cast<int>(touchPos.X - m_NoiseBarStartPos.X);

		math::Vec3 barPos = math::Vec3( touchPos.X, m_NoiseBarStartPos.Y, m_NoiseBarStartPos.Z );
		m_OptionsElements.ChangeElementPosition(OPTIONSGAME_UI_BTN_SCRUBBER, barPos);

		math::Vec3 barMeshPos = math::Vec3( static_cast<float>(m_NoiseBarXPos), 0.0f, 0.0f );
		//m_OptionsMesh->SetMeshTranslation( SCRUBBER_INACTIVE, barMeshPos);
		//m_OptionsMesh->SetMeshTranslation( SCRUBBER_ACTIVE, barMeshPos);
		
		// update actual value
		m_CurrentOptions.noiseFilter = (m_NoiseBarXPos / m_NoiseBarRange);

		int defaultIncrease = 2*(m_CurrentOptions.noiseFilter-5);
		if( defaultIncrease < -DEFAULT_NOISE_ALPHA )
			defaultIncrease = -DEFAULT_NOISE_ALPHA;

		SetNoiseAlpha( DEFAULT_NOISE_ALPHA+defaultIncrease );
	}
	else
	{
		m_NoiseHeld = false;
		UpdateOptions( );
	}

	// fps
	if( m_GeneralData.fpsModeUnlocked )
	{
		if( m_OptionsElements.CheckElementForSingleTouch(OPTIONSGAME_UI_BTN_FPSMODE_OFF) )
		{
			m_CurrentOptions.fpsMode = false;
			player.DisableFPSMode();
			GameSystems::GetInstance()->GetGameCamera()->SetLerp(m_CurrentOptions.camLerp);
			UpdateOptions();
		}
		else if( m_OptionsElements.CheckElementForSingleTouch(OPTIONSGAME_UI_BTN_FPSMODE_ON) )
		{
			m_CurrentOptions.fpsMode = true;
			player.EnableFPSMode();
			GameSystems::GetInstance()->GetGameCamera()->SetLerp(false);
			UpdateOptions();
		}
	}

	if( m_OptionsElements.CheckElementForSingleTouch(OPTIONSGAME_UI_BTN_RESUME) )
	{
		GameSystems::GetInstance()->SetOptions(m_CurrentOptions);
		m_Paused = false;
		AudioSystem::GetInstance()->UnPause();
	}
}

/////////////////////////////////////////////////////
/// Method: UpdateOptions
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void MainStateUITablet::UpdateOptions()
{
	/*if( m_CurrentOptions.crosshair )
	{
		m_OptionsMesh->SetMeshDrawState( OPTIONSGAME_CROSSHAIRON_INACTIVE, false );
		m_OptionsMesh->SetMeshDrawState( OPTIONSGAME_CROSSHAIRON_ACTIVE, true );

		m_OptionsMesh->SetMeshDrawState( OPTIONSGAME_CROSSHAIROFF_INACTIVE, true );
		m_OptionsMesh->SetMeshDrawState( OPTIONSGAME_CROSSHAIROFF_ACTIVE, false );
	}
	else
	{
		m_OptionsMesh->SetMeshDrawState( OPTIONSGAME_CROSSHAIRON_INACTIVE, true );
		m_OptionsMesh->SetMeshDrawState( OPTIONSGAME_CROSSHAIRON_ACTIVE, false );

		m_OptionsMesh->SetMeshDrawState( OPTIONSGAME_CROSSHAIROFF_INACTIVE, false );
		m_OptionsMesh->SetMeshDrawState( OPTIONSGAME_CROSSHAIROFF_ACTIVE, true );
	}*/

	if( m_CurrentOptions.camLerp )
	{
		m_OptionsMesh->SetMeshDrawState( BTN_LERP_ON_INACTIVE, false );
		m_OptionsMesh->SetMeshDrawState( BTN_LERP_ON_ACTIVE, true );

		m_OptionsMesh->SetMeshDrawState( BTN_LERP_OFF_INACTIVE, true );
		m_OptionsMesh->SetMeshDrawState( BTN_LERP_OFF_ACTIVE, false );
	}
	else
	{
		m_OptionsMesh->SetMeshDrawState( BTN_LERP_ON_INACTIVE, true );
		m_OptionsMesh->SetMeshDrawState( BTN_LERP_ON_ACTIVE, false );

		m_OptionsMesh->SetMeshDrawState( BTN_LERP_OFF_INACTIVE, false );
		m_OptionsMesh->SetMeshDrawState( BTN_LERP_OFF_ACTIVE, true );
	}

	// noise
	if( m_NoiseHeld )
	{
		m_OptionsMesh->SetMeshDrawState( SCRUBBER_INACTIVE, false );
		m_OptionsMesh->SetMeshDrawState( SCRUBBER_ACTIVE, true );
	}
	else
	{
		m_OptionsMesh->SetMeshDrawState( SCRUBBER_INACTIVE, true );
		m_OptionsMesh->SetMeshDrawState( SCRUBBER_ACTIVE, false );
	}

	if( m_GeneralData.fpsModeUnlocked )
	{
		m_OptionsMesh->SetMeshDrawState( LOCK_FPSMODE, false );

		m_OptionsElements.ChangeElementDrawState( OPTIONSGAME_UI_LABEL_FPSMODE_OFF, true );
		m_OptionsElements.ChangeElementDrawState( OPTIONSGAME_UI_LABEL_FPSMODE_ON, true );

		if( m_CurrentOptions.fpsMode )
		{
			m_OptionsMesh->SetMeshDrawState( BTN_FPSMODE_ON_INACTIVE, false );
			m_OptionsMesh->SetMeshDrawState( BTN_FPSMODE_ON_ACTIVE, true );

			m_OptionsMesh->SetMeshDrawState( BTN_FPSMODE_OFF_INACTIVE, true );
			m_OptionsMesh->SetMeshDrawState( BTN_FPSMODE_OFF_ACTIVE, false );
		}
		else
		{
			m_OptionsMesh->SetMeshDrawState( BTN_FPSMODE_ON_INACTIVE, true );
			m_OptionsMesh->SetMeshDrawState( BTN_FPSMODE_ON_ACTIVE, false );

			m_OptionsMesh->SetMeshDrawState( BTN_FPSMODE_OFF_INACTIVE, false );
			m_OptionsMesh->SetMeshDrawState( BTN_FPSMODE_OFF_ACTIVE, true );
		}
	}
	else
	{
		m_OptionsMesh->SetMeshDrawState( LOCK_FPSMODE, true );

		m_OptionsElements.ChangeElementDrawState( OPTIONSGAME_UI_LABEL_FPSMODE_OFF, false );
		m_OptionsElements.ChangeElementDrawState( OPTIONSGAME_UI_LABEL_FPSMODE_ON, false );

		m_OptionsMesh->SetMeshDrawState( BTN_FPSMODE_ON_INACTIVE, false );
		m_OptionsMesh->SetMeshDrawState( BTN_FPSMODE_ON_ACTIVE, false );

		m_OptionsMesh->SetMeshDrawState( BTN_FPSMODE_OFF_INACTIVE, false );
		m_OptionsMesh->SetMeshDrawState( BTN_FPSMODE_OFF_ACTIVE, false );
	}
}

/////////////////////////////////////////////////////
/// Method: UpdateType1
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void MainStateUITablet::UpdateType1( float deltaTime )
{
	// TYPE 1
	// LEFT DIGITAL / SCREEN 

	m_UIMesh->SetMeshDrawState( RIGHTHAND_BTN_UP1, true );
	m_UIMesh->SetMeshDrawState( RIGHTHAND_BTN_DOWN1, true );
	m_UIMesh->SetMeshDrawState( RIGHTHAND_BTN_LEFT1, true );
	m_UIMesh->SetMeshDrawState( RIGHTHAND_BTN_RIGHT1, true );

#if defined( BASE_PLATFORM_iOS ) || defined( BASE_PLATFORM_ANDROID )	

	// left side
	int lastTouch = -1;
	int i=0;
	
	if(m_Elements.CheckElementForTouch( RIGHTHAND_UI_LEFTSIDE_UP, UIFileLoader::EVENT_PRESS ) || 
	   m_Elements.CheckElementForTouch( RIGHTHAND_UI_LEFTSIDE_UP, UIFileLoader::EVENT_HELD ) )
	{
		m_InputSystem.SetUpVal(ANALOGUE_RANGE_LARGE);
		m_InputSystem.SetUp(true);

		lastTouch = m_Elements.GetLastTouchIndex(RIGHTHAND_UI_LEFTSIDE_UP);
		m_InputSystem.SetUpTouchIndex(lastTouch);
	}
	else
	{
		m_InputSystem.SetUpVal(0.0f);
		m_InputSystem.SetUp(false);
		m_InputSystem.SetUpTouchIndex(-1);
	}

	if(m_Elements.CheckElementForTouch( RIGHTHAND_UI_LEFTSIDE_DOWN, UIFileLoader::EVENT_PRESS ) ||  
	   m_Elements.CheckElementForTouch( RIGHTHAND_UI_LEFTSIDE_DOWN, UIFileLoader::EVENT_HELD ) )
	{
		m_InputSystem.SetDownVal(-ANALOGUE_RANGE_LARGE);
		m_InputSystem.SetDown(true);

		lastTouch = m_Elements.GetLastTouchIndex(RIGHTHAND_UI_LEFTSIDE_DOWN);
	}
	else
	{
		m_InputSystem.SetDownVal(0.0f);
		m_InputSystem.SetDown(false);
	}

	if(m_Elements.CheckElementForTouch( RIGHTHAND_UI_LEFTSIDE_LEFT, UIFileLoader::EVENT_PRESS ) ||  
	   m_Elements.CheckElementForTouch( RIGHTHAND_UI_LEFTSIDE_LEFT, UIFileLoader::EVENT_HELD ) )
	{
		m_InputSystem.SetLeftVal(-ANALOGUE_RANGE_LARGE);
		m_InputSystem.SetLeft(true);

		lastTouch = m_Elements.GetLastTouchIndex(RIGHTHAND_UI_LEFTSIDE_LEFT);
	}
	else
	{
		m_InputSystem.SetLeftVal(0.0f);
		m_InputSystem.SetLeft(false);
	}

	if(m_Elements.CheckElementForTouch( RIGHTHAND_UI_LEFTSIDE_RIGHT, UIFileLoader::EVENT_PRESS ) ||  
	   m_Elements.CheckElementForTouch( RIGHTHAND_UI_LEFTSIDE_RIGHT, UIFileLoader::EVENT_HELD ) )
	{
		m_InputSystem.SetRightVal(ANALOGUE_RANGE_LARGE);
		m_InputSystem.SetRight(true);

		lastTouch = m_Elements.GetLastTouchIndex(RIGHTHAND_UI_LEFTSIDE_RIGHT);
	}
	else
	{
		m_InputSystem.SetRightVal(0.0f);
		m_InputSystem.SetRight(false);
	}

	// right side
	for( i=0; i < 2; ++i )
	{		
		const input::TInputState::TouchData* pData = 0;
		pData = input::GetTouch(i);
		
		if( pData->bActive == false || i == lastTouch )
			continue;

		if( pData->nXDelta < 0.0f )
		{
			m_InputSystem.SetRotateLeft(true);
			m_InputSystem.SetRotateRight(false);

			m_InputSystem.SetRotateLeftVal( -((pData->nXDelta*ROTATE_MULT)*ANALOGUE_RANGE_LARGE) );
			m_InputSystem.SetRotateRightVal( 0.0f );
		}
		else if( pData->nXDelta > 0.0f )
		{
			m_InputSystem.SetRotateLeft(false);
			m_InputSystem.SetRotateRight(true);

			m_InputSystem.SetRotateLeftVal( 0.0f );
			m_InputSystem.SetRotateRightVal( ((pData->nXDelta*ROTATE_MULT)*ANALOGUE_RANGE_LARGE) );
		}

		if( pData->nYDelta > 0.0f )
		{
			m_InputSystem.SetLookUp(true);
			m_InputSystem.SetLookDown(false);

			m_InputSystem.SetLookUpVal( ((pData->nYDelta*LOOK_MULT)*ANALOGUE_RANGE_LARGE) );
			m_InputSystem.SetLookDownVal( 0.0f );
		}
		else if( pData->nYDelta < 0.0f )
		{
			m_InputSystem.SetLookUp(false);
			m_InputSystem.SetLookDown(true);

			m_InputSystem.SetLookUpVal( 0.0f );
			m_InputSystem.SetLookDownVal( -((pData->nYDelta*LOOK_MULT)*ANALOGUE_RANGE_LARGE) );
		}
	}


	if( GameSystems::GetInstance()->IsTalking() )
	{
		if( input::gInputState.TouchesData[input::FIRST_TOUCH].bRelease )
		{
			//if( pData->Ticks <= 1.0f )
			{
				m_Say1Index = -1;
				m_Say2Index = -1;

				m_SayLine1 = 0;
				m_SayLine2 = 0;
				
				// any function call
				if( !core::IsEmptyString( m_SayCallFunc ) )
				{
#ifdef _DEBUG
					if( script::LuaFunctionCheck( m_SayCallFunc ) == 0 )
#endif // _DEBUG
					{
						int result = 0;
						
						int errorFuncIndex;
						errorFuncIndex = script::GetErrorFuncIndex();
						
						// get the function
						lua_getglobal( script::LuaScripting::GetState(), m_SayCallFunc );
						
						result = lua_pcall( script::LuaScripting::GetState(), 0, 0, errorFuncIndex );
						
						// LUA_ERRRUN --- a runtime error. 
						// LUA_ERRMEM --- memory allocation error. For such errors, Lua does not call the error handler function. 
						// LUA_ERRERR --- error while running the error handler function. 
						
						if( result == LUA_ERRRUN || result == LUA_ERRMEM || result == LUA_ERRERR )
						{
							DBGLOG( "MAINSTATEUI: *ERROR* Calling function '%s' failed\n", m_SayCallFunc );
							DBGLOG( "\tLUA_TRACEBACK: %s\n", lua_tostring( script::LuaScripting::GetState(), -1 ) );
							
							DBG_ASSERT_MSG( 0, "MAINSTATEUI: *ERROR* Calling function '%s' failed", m_SayCallFunc );
						}
					}
#ifdef _DEBUG
					else
						DBG_ASSERT_MSG( 0, "Script Function '%s' does not exist, cannot create door type", m_SayCallFunc );		
#endif // _DEBUG
					
				}
				
				m_SayCallFunc = 0;				

				int newLine = -1;
				GameSystems::GetInstance()->GetSay( &newLine, 0, 0 );
				if( newLine == -1 )
				{
					// stop the button being pressed more than once
					input::DirtyTouchTicks();
					GameSystems::GetInstance()->SetTalkState( false );
				}
			}
		}
	}

#else // PC

	if( GameSystems::GetInstance()->IsTalking() )
	{
		if( input::gInputState.TouchesData[input::FIRST_TOUCH].bRelease || 
			m_InputSystem.GetAction() ||
			m_InputSystem.GetUse() )
		{
			m_Say1Index = -1;
			m_Say2Index = -1;

			m_SayLine1 = 0;
			m_SayLine2 = 0;

			// any function call
			if( !core::IsEmptyString( m_SayCallFunc ) )
			{
#ifdef _DEBUG
				if( script::LuaFunctionCheck( m_SayCallFunc ) == 0 )
#endif // _DEBUG
				{
					int result = 0;

					int errorFuncIndex;
					errorFuncIndex = script::GetErrorFuncIndex();

					// get the function
					lua_getglobal( script::LuaScripting::GetState(), m_SayCallFunc );

					result = lua_pcall( script::LuaScripting::GetState(), 0, 0, errorFuncIndex );

					// LUA_ERRRUN --- a runtime error. 
					// LUA_ERRMEM --- memory allocation error. For such errors, Lua does not call the error handler function. 
					// LUA_ERRERR --- error while running the error handler function. 

					if( result == LUA_ERRRUN || result == LUA_ERRMEM || result == LUA_ERRERR )
					{
						DBGLOG( "MAINSTATEUI: *ERROR* Calling function '%s' failed\n", m_SayCallFunc );
						DBGLOG( "\tLUA_TRACEBACK: %s\n", lua_tostring( script::LuaScripting::GetState(), -1 ) );

						DBG_ASSERT_MSG( 0, "MAINSTATEUI: *ERROR* Calling function '%s' failed", m_SayCallFunc );
					}
				}
#ifdef _DEBUG
				else
					DBG_ASSERT_MSG( 0, "Script Function '%s' does not exist, cannot create door type", m_SayCallFunc );		
#endif // _DEBUG
	
			}

			m_SayCallFunc = 0;

				int newLine = -1;
				GameSystems::GetInstance()->GetSay( &newLine, 0, 0 );
				if( newLine == -1 )
				{
					GameSystems::GetInstance()->SetTalkState( false );
				}
		}
	}
#endif
}

/////////////////////////////////////////////////////
/// Method: UpdateType4
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void MainStateUITablet::UpdateType4( float deltaTime )
{
	// TYPE 4
	// SCREEN / RIGHT DIGITAL

	m_UIMesh->SetMeshDrawState( LEFTHAND_BTN_UP2, true );
	m_UIMesh->SetMeshDrawState( LEFTHAND_BTN_DOWN2, true );
	m_UIMesh->SetMeshDrawState( LEFTHAND_BTN_LEFT2, true );
	m_UIMesh->SetMeshDrawState( LEFTHAND_BTN_RIGHT2, true );

#if defined( BASE_PLATFORM_iOS ) || defined( BASE_PLATFORM_ANDROID )	

	// left side
	int lastTouch = -1;
	int i=0;
	
	if(m_Elements.CheckElementForTouch( LEFTHAND_UI_RIGHTSIDE_UP, UIFileLoader::EVENT_PRESS ) ||  
	   m_Elements.CheckElementForTouch( LEFTHAND_UI_RIGHTSIDE_UP, UIFileLoader::EVENT_HELD ) )
	{
		m_InputSystem.SetUpVal(ANALOGUE_RANGE_LARGE);
		m_InputSystem.SetUp(true);

		lastTouch = m_Elements.GetLastTouchIndex(LEFTHAND_UI_RIGHTSIDE_UP);
		m_InputSystem.SetUpTouchIndex(lastTouch);
	}
	else
	{
		m_InputSystem.SetUpVal(0.0f);
		m_InputSystem.SetUp(false);
		m_InputSystem.SetUpTouchIndex(-1);
	}

	if(m_Elements.CheckElementForTouch( LEFTHAND_UI_RIGHTSIDE_DOWN, UIFileLoader::EVENT_PRESS ) ||  
	   m_Elements.CheckElementForTouch( LEFTHAND_UI_RIGHTSIDE_DOWN, UIFileLoader::EVENT_HELD ) )
	{
		m_InputSystem.SetDownVal(-ANALOGUE_RANGE_LARGE);
		m_InputSystem.SetDown(true);

		lastTouch = m_Elements.GetLastTouchIndex(LEFTHAND_UI_RIGHTSIDE_DOWN);
	}
	else
	{
		m_InputSystem.SetDownVal(0.0f);
		m_InputSystem.SetDown(false);
	}

	if(m_Elements.CheckElementForTouch( LEFTHAND_UI_RIGHTSIDE_LEFT, UIFileLoader::EVENT_PRESS ) ||  
	   m_Elements.CheckElementForTouch( LEFTHAND_UI_RIGHTSIDE_LEFT, UIFileLoader::EVENT_HELD ) )
	{
		m_InputSystem.SetLeftVal(-ANALOGUE_RANGE_LARGE);
		m_InputSystem.SetLeft(true);

		lastTouch = m_Elements.GetLastTouchIndex(LEFTHAND_UI_RIGHTSIDE_LEFT);
	}
	else
	{
		m_InputSystem.SetLeftVal(0.0f);
		m_InputSystem.SetLeft(false);
	}

	if(m_Elements.CheckElementForTouch( LEFTHAND_UI_RIGHTSIDE_RIGHT, UIFileLoader::EVENT_PRESS ) ||  
	   m_Elements.CheckElementForTouch( LEFTHAND_UI_RIGHTSIDE_RIGHT, UIFileLoader::EVENT_HELD ) )
	{
		m_InputSystem.SetRightVal(ANALOGUE_RANGE_LARGE);
		m_InputSystem.SetRight(true);

		lastTouch = m_Elements.GetLastTouchIndex(LEFTHAND_UI_RIGHTSIDE_RIGHT);
	}
	else
	{
		m_InputSystem.SetRightVal(0.0f);
		m_InputSystem.SetRight(false);
	}

	// right side
	for( i=0; i < 2; ++i )
	{		
		const input::TInputState::TouchData* pData = 0;
		pData = input::GetTouch(i);
		
		if( pData->bActive == false || i == lastTouch )
			continue;

		if( pData->nXDelta < 0.0f )
		{
			m_InputSystem.SetRotateLeft(true);
			m_InputSystem.SetRotateRight(false);

			m_InputSystem.SetRotateLeftVal( -((pData->nXDelta*ROTATE_MULT)*ANALOGUE_RANGE_LARGE) );
			m_InputSystem.SetRotateRightVal( 0.0f );
		}
		else if( pData->nXDelta > 0.0f )
		{
			m_InputSystem.SetRotateLeft(false);
			m_InputSystem.SetRotateRight(true);

			m_InputSystem.SetRotateLeftVal( 0.0f );
			m_InputSystem.SetRotateRightVal( ((pData->nXDelta*ROTATE_MULT)*ANALOGUE_RANGE_LARGE) );
		}

		if( pData->nYDelta > 0.0f )
		{
			m_InputSystem.SetLookUp(true);
			m_InputSystem.SetLookDown(false);

			m_InputSystem.SetLookUpVal( ((pData->nYDelta*LOOK_MULT)*ANALOGUE_RANGE_LARGE) );
			m_InputSystem.SetLookDownVal( 0.0f );
		}
		else if( pData->nYDelta < 0.0f )
		{
			m_InputSystem.SetLookUp(false);
			m_InputSystem.SetLookDown(true);

			m_InputSystem.SetLookUpVal( 0.0f );
			m_InputSystem.SetLookDownVal( -((pData->nYDelta*LOOK_MULT)*ANALOGUE_RANGE_LARGE) );
		}
	}


	if( GameSystems::GetInstance()->IsTalking() )
	{
		
		if( input::gInputState.TouchesData[input::FIRST_TOUCH].bRelease )
		{
			//if( pData->Ticks <= 1.0f )
			{
				m_Say1Index = -1;
				m_Say2Index = -1;

				m_SayLine1 = 0;
				m_SayLine2 = 0;
				
				// any function call
				if( !core::IsEmptyString( m_SayCallFunc ) )
				{
#ifdef _DEBUG
					if( script::LuaFunctionCheck( m_SayCallFunc ) == 0 )
#endif // _DEBUG
					{
						int result = 0;
						
						int errorFuncIndex;
						errorFuncIndex = script::GetErrorFuncIndex();
						
						// get the function
						lua_getglobal( script::LuaScripting::GetState(), m_SayCallFunc );
						
						result = lua_pcall( script::LuaScripting::GetState(), 0, 0, errorFuncIndex );
						
						// LUA_ERRRUN --- a runtime error. 
						// LUA_ERRMEM --- memory allocation error. For such errors, Lua does not call the error handler function. 
						// LUA_ERRERR --- error while running the error handler function. 
						
						if( result == LUA_ERRRUN || result == LUA_ERRMEM || result == LUA_ERRERR )
						{
							DBGLOG( "MAINSTATEUI: *ERROR* Calling function '%s' failed\n", m_SayCallFunc );
							DBGLOG( "\tLUA_TRACEBACK: %s\n", lua_tostring( script::LuaScripting::GetState(), -1 ) );
							
							DBG_ASSERT_MSG( 0, "MAINSTATEUI: *ERROR* Calling function '%s' failed", m_SayCallFunc );
						}
					}
#ifdef _DEBUG
					else
						DBG_ASSERT_MSG( 0, "Script Function '%s' does not exist, cannot create door type", m_SayCallFunc );		
#endif // _DEBUG
					
				}
				
				m_SayCallFunc = 0;				

				int newLine = -1;
				GameSystems::GetInstance()->GetSay( &newLine, 0, 0 );
				if( newLine == -1 )
				{
					// stop the button being pressed more than once
					input::DirtyTouchTicks();
					GameSystems::GetInstance()->SetTalkState( false );
				}
			}
		}
	}

#else // PC

	if( GameSystems::GetInstance()->IsTalking() )
	{
		if( input::gInputState.TouchesData[input::FIRST_TOUCH].bRelease || 
			m_InputSystem.GetAction() ||
			m_InputSystem.GetUse() )
		{
			m_Say1Index = -1;
			m_Say2Index = -1;

			m_SayLine1 = 0;
			m_SayLine2 = 0;

			// any function call
			if( !core::IsEmptyString( m_SayCallFunc ) )
			{
#ifdef _DEBUG
				if( script::LuaFunctionCheck( m_SayCallFunc ) == 0 )
#endif // _DEBUG
				{
					int result = 0;

					int errorFuncIndex;
					errorFuncIndex = script::GetErrorFuncIndex();

					// get the function
					lua_getglobal( script::LuaScripting::GetState(), m_SayCallFunc );

					result = lua_pcall( script::LuaScripting::GetState(), 0, 0, errorFuncIndex );

					// LUA_ERRRUN --- a runtime error. 
					// LUA_ERRMEM --- memory allocation error. For such errors, Lua does not call the error handler function. 
					// LUA_ERRERR --- error while running the error handler function. 

					if( result == LUA_ERRRUN || result == LUA_ERRMEM || result == LUA_ERRERR )
					{
						DBGLOG( "MAINSTATEUI: *ERROR* Calling function '%s' failed\n", m_SayCallFunc );
						DBGLOG( "\tLUA_TRACEBACK: %s\n", lua_tostring( script::LuaScripting::GetState(), -1 ) );

						DBG_ASSERT_MSG( 0, "MAINSTATEUI: *ERROR* Calling function '%s' failed", m_SayCallFunc );
					}
				}
#ifdef _DEBUG
				else
					DBG_ASSERT_MSG( 0, "Script Function '%s' does not exist, cannot create door type", m_SayCallFunc );		
#endif // _DEBUG
	
			}

			m_SayCallFunc = 0;

				int newLine = -1;
				GameSystems::GetInstance()->GetSay( &newLine, 0, 0 );
				if( newLine == -1 )
				{
					GameSystems::GetInstance()->SetTalkState( false );
				}
		}
	}
#endif
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void MainStateUITablet::Draw()
{
	// game is loading, or continue from death
	if( m_ContinueFromDeath ||
		GameSystems::GetInstance()->GetChangeStageFlag() )
	{
		renderer::OpenGL::GetInstance()->SetNearFarClip( -100.0f, 100.0f );
		renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );
		renderer::OpenGL::GetInstance()->DisableLighting();

		renderer::OpenGL::GetInstance()->DepthMode( true, GL_ALWAYS );
		renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		renderer::OpenGL::GetInstance()->AlphaMode( false, GL_ALWAYS, 0.0f );

		// don't draw anything except level name
		if( m_LeftHanded )
			m_Elements.ChangeElementText( LEFTHAND_UI_LABEL_AMMO, "" );
		else
			m_Elements.ChangeElementText( RIGHTHAND_UI_LABEL_AMMO, "" );

		m_Elements.Draw();

		return;
	}

	// micro game takes complete control
	if( GameSystems::GetInstance()->IsMicroGameActive() )
		return;

	if( !hudToggle )
		return;

	renderer::OpenGL::GetInstance()->SetNearFarClip( -100.0f, 100.0f );
	renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );
	renderer::OpenGL::GetInstance()->DisableLighting();

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_ALWAYS );
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	renderer::OpenGL::GetInstance()->AlphaMode( false, GL_ALWAYS, 0.0f );

	if( m_Paused )
	{
		m_OptionsMesh->Draw();
		
		renderer::OpenGL::GetInstance()->SetNearFarClip( -100.0f, 100.0f );
		renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );

		m_OptionsElements.Draw();
	}
	else
	{
		// disable hud on talk
		if( GameSystems::GetInstance()->IsTalking() )
		{
			Player& player = GameSystems::GetInstance()->GetPlayer();

			if( m_LeftHanded )
			{
				//for( i=LEFTHAND_TORCH_BG; i<=LEFTHAND_SHAKE_ALERT; ++i )
					m_UIMesh->SetMeshDrawState( -1, false );

				bool useTorch = player.TorchEnabled();
				m_UIMesh->SetMeshDrawState( LEFTHAND_TORCH_BG, useTorch );
				m_UIMesh->SetMeshDrawState( LEFTHAND_TORCH_GLARE, useTorch );

				renderer::OpenGL::GetInstance()->SetNearFarClip( -100.0f, 100.0f );
				renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );

				renderer::OpenGL::GetInstance()->SetColour4ub( 255,255,255,255 );
				
				m_UIMesh->Draw();

				renderer::OpenGL::GetInstance()->SetNearFarClip( -100.0f, 100.0f );
				renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );

				m_Elements.ChangeElementText( LEFTHAND_UI_LABEL_AMMO, "" );
				m_Elements.Draw();
			}
			else
			{
				//for( i=RIGHTHAND_TORCH_BG; i<=RIGHTHAND_SHAKE_ALERT; ++i )
					m_UIMesh->SetMeshDrawState( -1, false );

				bool useTorch = player.TorchEnabled();
				m_UIMesh->SetMeshDrawState( RIGHTHAND_TORCH_BG, useTorch );
				m_UIMesh->SetMeshDrawState( RIGHTHAND_TORCH_GLARE, useTorch );

				renderer::OpenGL::GetInstance()->SetColour4ub( 255,255,255,255 );
				
				renderer::OpenGL::GetInstance()->SetNearFarClip( -100.0f, 100.0f );
				renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );

				m_UIMesh->Draw();
				
				renderer::OpenGL::GetInstance()->SetNearFarClip( -100.0f, 100.0f );
				renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );

				m_Elements.ChangeElementText( RIGHTHAND_UI_LABEL_AMMO, "" );
				m_Elements.Draw();
			}
		}
		else
		{
			renderer::OpenGL::GetInstance()->SetColour4ub( 255,255,255,255 );
			
			renderer::OpenGL::GetInstance()->SetNearFarClip( -100.0f, 100.0f );
			renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );

			m_UIMesh->Draw();

			renderer::OpenGL::GetInstance()->SetNearFarClip( -100.0f, 100.0f );
			renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );

			m_Elements.Draw();
		}
	}
}

/////////////////////////////////////////////////////
/// Method: SetLevelName
/// Params: [in]name
///
/////////////////////////////////////////////////////
void MainStateUITablet::SetLevelName( const char* name )
{
	if( m_LeftHanded )
		m_Elements.ChangeElementText( LEFTHAND_UI_LABEL_LEVELNAME, name );
	else
		m_Elements.ChangeElementText( RIGHTHAND_UI_LABEL_LEVELNAME, name );
}

/////////////////////////////////////////////////////
/// Method: MainStateUITablet
/// Params: None
///
/////////////////////////////////////////////////////
void MainStateUITablet::ClearSay()	
{ 
	m_Say1Index = -1; 
	m_Say2Index = -1; 
	m_SayLine1 = 0; 
	m_SayLine2 = 0; 
	m_SayCallFunc = 0; 

	if( m_LeftHanded )
	{
		m_Elements.ChangeElementText( LEFTHAND_UI_LABEL_SPEECH_LINE1, "" );
		m_Elements.ChangeElementText( LEFTHAND_UI_LABEL_SPEECH_LINE2, "" );
	}
	else
	{
		m_Elements.ChangeElementText( RIGHTHAND_UI_LABEL_SPEECH_LINE1, "" );
		m_Elements.ChangeElementText( RIGHTHAND_UI_LABEL_SPEECH_LINE2, "" );
	}
}
