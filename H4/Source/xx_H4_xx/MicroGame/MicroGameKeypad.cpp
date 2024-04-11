
/*===================================================================
	File: MicroGameKeypad.cpp
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
#include "MicroGameKeypad.h"

// generated files
#include "GameStates/UI/GeneratedFiles/microgame_keypad.h" // HGM submesh list
#include "GameStates/UI/GeneratedFiles/microgame_keypad.hui.h" // UI element list

namespace
{
	const float SHOW_TIME = 1.0f;
	const int MAX_TOUCH_TESTS = 1;
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
MicroGameKeypad::MicroGameKeypad( unsigned int correctCode )
{
	m_KeypadModel = 0;
	m_CorrectCode = correctCode;

	m_KeyAudio = snd::INVALID_SOUNDBUFFER;
	m_KeyEnterAudio = snd::INVALID_SOUNDBUFFER;

	m_WaitForRelease = false;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
MicroGameKeypad::~MicroGameKeypad()
{

}

/////////////////////////////////////////////////////
/// Method: Initialise
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGameKeypad::Initialise()
{

}

/////////////////////////////////////////////////////
/// Method: Release
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGameKeypad::Release()
{

}

/////////////////////////////////////////////////////
/// Method: OnEnter
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGameKeypad::OnEnter()
{
	int i=0;

	m_KeypadModel = res::LoadModel( 2000 );
	DBG_ASSERT( (m_KeypadModel != 0) );

	m_KeypadModel->SetMeshDrawState( KEYPAD_CORRECT, false );
	m_KeypadModel->SetMeshDrawState( KEYPAD_INCORRECT, false );

	math::Vec2 srcAssetDims( static_cast<float>(core::app::GetBaseAssetsWidth()), static_cast<float>(core::app::GetBaseAssetsHeight()) );
	m_KeypadUI.Load( "hui/microgame_keypad.hui", srcAssetDims );

	math::Vec3 scaleFactor = math::Vec3( core::app::GetWidthScale(), core::app::GetHeightScale(), 1.0f );
	for( i=KEYPAD_BACKBTN; i <= KEYPAD_INCORRECT; ++i )
	{		
		m_KeypadModel->SetMeshScale(i, scaleFactor);
	}

	const char* sr = 0;
	sr = res::GetSoundResource( 100 );

	snd::Sound sndLoad;
	m_KeyAudio = snd::SoundLoad( sr, sndLoad );
	DBG_ASSERT( (m_KeyAudio != snd::INVALID_SOUNDBUFFER) );

	sr = res::GetSoundResource( 101 );
	m_KeyEnterAudio = snd::SoundLoad( sr, sndLoad );
	DBG_ASSERT( (m_KeyEnterAudio != snd::INVALID_SOUNDBUFFER) );

	m_TotalDigits = 0;
	std::memset( m_DigitString, 0, sizeof(char)*NUM_KEYPAD_CHARS );

	m_KeypadUI.ChangeElementText( KEYPAD_OUTPUT, m_DigitString );

	m_ClrPressCount = 0;
	m_OnePressCount = 0;
	m_TwoPressCount = 0;
	m_ThreePressCount = 0;
	m_FourPressCount = 0;
	m_FivePressCount = 0;
	m_SixPressCount = 0;
	m_SevenPressCount = 0;
	m_EightPressCount = 0;
	m_NinePressCount = 0;


	m_ShowCorrect = false;
	m_ShowIncorrect = false;
	m_ShowTimer = 0.0f;

	m_WaitForRelease = false;
}

/////////////////////////////////////////////////////
/// Method: OnExit
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGameKeypad::OnExit()
{
	res::RemoveModel( m_KeypadModel );
	m_KeypadModel = 0;

	snd::RemoveSound( m_KeyAudio );
	snd::RemoveSound( m_KeyEnterAudio );
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void MicroGameKeypad::Update( float deltaTime )
{
	math::Vec3 zeroVec( 0.0f, 0.0f, 0.0f );

	// update UI elements
	m_KeypadUI.Update( TOUCH_SIZE*0.5f, MAX_TOUCH_TESTS, deltaTime );

	if(m_KeypadUI.CheckElementForTouch( KEYPAD_EXIT, UIFileLoader::EVENT_PRESS ) ||
	   m_KeypadUI.CheckElementForTouch( KEYPAD_EXIT, UIFileLoader::EVENT_HELD ) )
	{
		m_Complete = true;
		m_ReturnCode = false;
		
		return;
	}
	
	if( m_ShowCorrect )
		m_KeypadModel->SetMeshDrawState( KEYPAD_CORRECT, true );
	else if( m_ShowIncorrect )
		m_KeypadModel->SetMeshDrawState( KEYPAD_INCORRECT, true );

	// show the correct light mesh
	if( m_ShowCorrect || m_ShowIncorrect )
	{
		m_ShowTimer -= deltaTime;

		if( m_ShowTimer <= 0.0f )
		{
			m_ShowTimer = 0.0f;
			m_KeypadModel->SetMeshDrawState( KEYPAD_CORRECT, false );
			m_KeypadModel->SetMeshDrawState( KEYPAD_INCORRECT, false );

			if( m_ReturnCode )
				m_Complete = true;

			m_ShowCorrect = false;
			m_ShowIncorrect = false;
		}

		return;
	}

	// don't allow sliding a finger over the buttons
	if( m_WaitForRelease )
	{
#if defined( BASE_PLATFORM_iOS ) || defined( BASE_PLATFORM_ANDROID )
		int i=0;
		for( i=0; i < MAX_TOUCH_TESTS; ++i )
		{		
			const input::TInputState::TouchData* pData = 0;
			pData = input::GetTouch(i);
			
			if( pData->bActive == false )
				m_WaitForRelease = false;
		}
#else
		if( !input::gInputState.TouchesData[input::FIRST_TOUCH].bRelease )
			m_WaitForRelease = false;
#endif 

		return;
	}

	// button tests
	if( m_KeypadUI.CheckElementForTouch( KEYPAD_OK ) )
	{
		if( atoi( m_DigitString ) == m_CorrectCode )
		{
			m_ReturnCode = true;
			m_ShowCorrect = true;
			m_ShowTimer = SHOW_TIME;
		}
		else
		{
			m_TotalDigits = 0;
			std::memset( m_DigitString, 0, sizeof(char)*NUM_KEYPAD_CHARS );

			m_ShowIncorrect = true;
			m_ShowTimer = SHOW_TIME;

			m_KeypadUI.ChangeElementText( KEYPAD_OUTPUT, m_DigitString );
		}

		AudioSystem::GetInstance()->PlayAudio( m_KeyEnterAudio, zeroVec, true );
		return;
	}
	
	if( m_KeypadUI.CheckElementForTouch( KEYPAD_CLEAR ) )
	{
		m_ClrPressCount++;
		if( m_ClrPressCount <= 1 )
		{
			m_TotalDigits = 0;
			std::memset( m_DigitString, 0, sizeof(char)*NUM_KEYPAD_CHARS );

			m_KeypadUI.ChangeElementText( KEYPAD_OUTPUT, m_DigitString );

			AudioSystem::GetInstance()->PlayAudio( m_KeyAudio, zeroVec, true );
			m_WaitForRelease = true;
			return;
		}
	}
	else
	{
		m_ClrPressCount = 0;
	}
	

	// handle digits
	if( m_TotalDigits < NUM_KEYPAD_DIGITS )
	{
// 0
		if( m_KeypadUI.CheckElementForTouch( KEYPAD_ZERO ) )
		{
			m_ZeroPressCount++;
			if( m_ZeroPressCount <= 1 )
			{
				m_DigitString[m_TotalDigits] = '0';

				m_KeypadUI.ChangeElementText( KEYPAD_OUTPUT, m_DigitString );
				m_TotalDigits++;

				AudioSystem::GetInstance()->PlayAudio( m_KeyAudio, zeroVec, true );
				m_WaitForRelease = true;
				return;
			}
		}
		else
		{
			m_ZeroPressCount = 0;
		}

// 1
		if( m_KeypadUI.CheckElementForTouch( KEYPAD_ONE ) )
		{
			m_OnePressCount++;
			if( m_OnePressCount <= 1 )
			{
				m_DigitString[m_TotalDigits] = '1';

				m_KeypadUI.ChangeElementText( KEYPAD_OUTPUT, m_DigitString );
				m_TotalDigits++;

				AudioSystem::GetInstance()->PlayAudio( m_KeyAudio, zeroVec, true );
				m_WaitForRelease = true;
				return;
			}
		}
		else
		{
			m_OnePressCount = 0;
		}

// 2
		if( m_KeypadUI.CheckElementForTouch( KEYPAD_TWO ) )
		{
			m_TwoPressCount++;
			if( m_TwoPressCount <= 1 )
			{
				m_DigitString[m_TotalDigits] = '2';

				m_KeypadUI.ChangeElementText( KEYPAD_OUTPUT, m_DigitString );
				m_TotalDigits++;

				AudioSystem::GetInstance()->PlayAudio( m_KeyAudio, zeroVec, true );
				m_WaitForRelease = true;
				return;
			}
		}
		else
		{
			m_TwoPressCount = 0;
		}

// 3
		if( m_KeypadUI.CheckElementForTouch( KEYPAD_THREE ) )
		{
			m_ThreePressCount++;
			if( m_ThreePressCount <= 1 )
			{
				m_DigitString[m_TotalDigits] = '3';

				m_KeypadUI.ChangeElementText( KEYPAD_OUTPUT, m_DigitString );
				m_TotalDigits++;

				AudioSystem::GetInstance()->PlayAudio( m_KeyAudio, zeroVec, true );
				m_WaitForRelease = true;
				return;
			}
		}
		else
		{
			m_ThreePressCount = 0;
		}

// 4
		if( m_KeypadUI.CheckElementForTouch( KEYPAD_FOUR ) )
		{
			m_FourPressCount++;
			if( m_FourPressCount <= 1 )
			{
				m_DigitString[m_TotalDigits] = '4';

				m_KeypadUI.ChangeElementText( KEYPAD_OUTPUT, m_DigitString );
				m_TotalDigits++;

				AudioSystem::GetInstance()->PlayAudio( m_KeyAudio, zeroVec, true );
				m_WaitForRelease = true;
				return;
			}
		}
		else
		{
			m_FourPressCount = 0;
		}

// 5
		if( m_KeypadUI.CheckElementForTouch( KEYPAD_FIVE ) )
		{
			m_FivePressCount++;
			if( m_FivePressCount <= 1 )
			{
				m_DigitString[m_TotalDigits] = '5';

				m_KeypadUI.ChangeElementText( KEYPAD_OUTPUT, m_DigitString );
				m_TotalDigits++;

				AudioSystem::GetInstance()->PlayAudio( m_KeyAudio, zeroVec, true );
				m_WaitForRelease = true;
				return;
			}
		}
		else
		{
			m_FivePressCount = 0;
		}

// 6
		if( m_KeypadUI.CheckElementForTouch( KEYPAD_SIX ) )
		{
			m_SixPressCount++;
			if( m_SixPressCount <= 1 )
			{
				m_DigitString[m_TotalDigits] = '6';

				m_KeypadUI.ChangeElementText( KEYPAD_OUTPUT, m_DigitString );
				m_TotalDigits++;

				AudioSystem::GetInstance()->PlayAudio( m_KeyAudio, zeroVec, true );
				m_WaitForRelease = true;
				return;
			}
		}
		else
		{
			m_SixPressCount = 0;
		}

// 7
		if( m_KeypadUI.CheckElementForTouch( KEYPAD_SEVEN ) )
		{
			m_SevenPressCount++;
			if( m_SevenPressCount <= 1 )
			{
				m_DigitString[m_TotalDigits] = '7';

				m_KeypadUI.ChangeElementText( KEYPAD_OUTPUT, m_DigitString );
				m_TotalDigits++;

				AudioSystem::GetInstance()->PlayAudio( m_KeyAudio, zeroVec, true );
				m_WaitForRelease = true;
				return;
			}
		}
		else
		{
			m_SevenPressCount = 0;
		}

// 8
		if( m_KeypadUI.CheckElementForTouch( KEYPAD_EIGHT ) )
		{
			m_EightPressCount++;
			if( m_EightPressCount <= 1 )
			{
				m_DigitString[m_TotalDigits] = '8';

				m_KeypadUI.ChangeElementText( KEYPAD_OUTPUT, m_DigitString );
				m_TotalDigits++;

				AudioSystem::GetInstance()->PlayAudio( m_KeyAudio, zeroVec, true );
				m_WaitForRelease = true;
				return;
			}
		}
		else
		{
			m_EightPressCount = 0;
		}

// 9
		if( m_KeypadUI.CheckElementForTouch( KEYPAD_NINE ) )
		{
			m_NinePressCount++;
			if( m_NinePressCount <= 1 )
			{
				m_DigitString[m_TotalDigits] = '9';

				m_KeypadUI.ChangeElementText( KEYPAD_OUTPUT, m_DigitString );
				m_TotalDigits++;

				AudioSystem::GetInstance()->PlayAudio( m_KeyAudio, zeroVec, true );
				m_WaitForRelease = true;
				return;
			}
		}
		else
		{
			m_NinePressCount = 0;
		}
	}
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGameKeypad::Draw()
{
	renderer::OpenGL::GetInstance()->ClearColour( 0.0f, 0.0f, 0.0f, 1.0f );

	renderer::OpenGL::GetInstance()->SetNearFarClip( -100.0f, 100.0f );
	renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_ALWAYS );
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	renderer::OpenGL::GetInstance()->AlphaMode( false, GL_ALWAYS, 0.0f );

	m_KeypadModel->Draw();

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_ALWAYS );
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	renderer::OpenGL::GetInstance()->SetNearFarClip( -100.0f, 100.0f );
	renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );

	m_KeypadUI.Draw();
}
