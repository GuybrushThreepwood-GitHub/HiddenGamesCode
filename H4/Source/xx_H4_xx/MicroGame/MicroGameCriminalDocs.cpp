

/*===================================================================
	File: MicroGameCriminalDocs.cpp
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
#include "GameSystems.h"

#include "Resources/SoundResources.h"
#include "Resources/ModelResources.h"
#include "Resources/StringResources.h"
#include "MicroGameCriminalDocs.h"

// generated files
#include "GameStates/UI/GeneratedFiles/hiddendocs.hgm.h" // HGM submesh list
#include "GameStates/UI/GeneratedFiles/hiddendocs.hui.h" // UI element list

namespace
{
	const int MAX_TOUCH_TESTS = 1;

	const char ENDING_TITLE[] = "";

	const int DOC1_PAGES = 2;
	const int DOC2_PAGES = 2;
	const int DOC3_PAGES = 3;
	const int DOC4_PAGES = 2;
	const int DOC5_PAGES = 2;
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
MicroGameCriminalDocs::MicroGameCriminalDocs( int whichDoc )
{
	m_DocsModels = 0;

	m_WhichDoc = whichDoc;
	m_CurrentPage = 1;
	m_NumPages = 0;

	m_PageFlip = snd::INVALID_SOUNDBUFFER;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
MicroGameCriminalDocs::~MicroGameCriminalDocs()
{

}

/////////////////////////////////////////////////////
/// Method: Initialise
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGameCriminalDocs::Initialise()
{

}

/////////////////////////////////////////////////////
/// Method: Release
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGameCriminalDocs::Release()
{

}

/////////////////////////////////////////////////////
/// Method: OnEnter
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGameCriminalDocs::OnEnter()
{
	m_DocsModels = 0;

	m_DocsModels = res::LoadModel( 2009 );
	DBG_ASSERT( (m_DocsModels != 0) );

	math::Vec2 srcAssetDims( static_cast<float>(core::app::GetBaseAssetsWidth()), static_cast<float>(core::app::GetBaseAssetsHeight()) );
	m_DocsUI.Load( "hui/hiddendocs.hui", srcAssetDims, m_DocsModels );
	m_DocsModels->SetMeshDrawState( -1, false );
	m_DocsUI.ChangeElementDrawState( false );

	m_NextEnabled = true;
	m_PreviousEnabled = false;

	m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_NEXT, true );
	m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_PREVIOUS, false );

	m_DocsModels->SetMeshDrawState( HIDDENDOCS_BG, true );
	m_DocsModels->SetMeshDrawState( HIDDENDOCS_DIM_TOP, true );
	m_DocsModels->SetMeshDrawState( HIDDENDOCS_DIM_BOTTOM, true );
	m_DocsModels->SetMeshDrawState( HIDDENDOCS_TITLE_BG, true );
	m_DocsModels->SetMeshDrawState( HIDDENDOCS_BTN_EXIT, true );

	switch(m_WhichDoc)
	{
		case 1:
		{
			m_NumPages = DOC1_PAGES;

			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_TITLE, res::GetScriptString(4000) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE1, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE1, "%s", res::GetScriptString(6040) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE2, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE2, "%s", res::GetScriptString(6041) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE3, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE3, "%s", res::GetScriptString(6042) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE4, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE4, "%s", res::GetScriptString(6043) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE5, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE5, "%s", res::GetScriptString(6044) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE7, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE7, "%s", res::GetScriptString(6045) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE8, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE8, "%s", res::GetScriptString(6046) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE9, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE9, "%s", res::GetScriptString(6047) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE10, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE10, "%s", res::GetScriptString(6048) );

			GameSystems::GetInstance()->AwardAchievement(15);

		}break;
		case 2:
		{
			m_NumPages = DOC2_PAGES;

			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_TITLE, res::GetScriptString(4001) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE1, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE1, "%s", res::GetScriptString(6060) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE2, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE2, "%s", res::GetScriptString(6061) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE3, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE3, "%s", res::GetScriptString(6062) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE5, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE5, "%s", res::GetScriptString(6063) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE6, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE6, "%s", res::GetScriptString(6064) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE7, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE7, "%s", res::GetScriptString(6065) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE8, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE8, "%s", res::GetScriptString(6066) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE9, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE9, "%s", res::GetScriptString(6067) );

			GameSystems::GetInstance()->AwardAchievement(16);

		}break;
		case 3:
		{
			m_NumPages = DOC3_PAGES;

			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_TITLE, res::GetScriptString(4002) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE1, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE1, "%s", res::GetScriptString(6080) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE2, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE2, "%s", res::GetScriptString(6081) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE3, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE3, "%s", res::GetScriptString(6082) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE4, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE4, "%s", res::GetScriptString(6083) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE5, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE5, "%s", res::GetScriptString(6084) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE6, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE6, "%s", res::GetScriptString(6085) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE8, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE8, "%s", res::GetScriptString(6086) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE9, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE9, "%s", res::GetScriptString(6087) );

			GameSystems::GetInstance()->AwardAchievement(17);
		}break;
		case 4:
		{
			m_NumPages = DOC4_PAGES;

			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_TITLE, res::GetScriptString(4003) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE1, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE1, "%s", res::GetScriptString(6100) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE2, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE2, "%s", res::GetScriptString(6101) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE3, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE3, "%s", res::GetScriptString(6102) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE4, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE4, "%s", res::GetScriptString(6103) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE5, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE5, "%s", res::GetScriptString(6104) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE7, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE7, "%s", res::GetScriptString(6105) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE8, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE8, "%s", res::GetScriptString(6106) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE9, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE9, "%s", res::GetScriptString(6107) );

			GameSystems::GetInstance()->AwardAchievement(18);
		}break;
		case 5:
		{
			m_NumPages = DOC5_PAGES;

			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_TITLE, res::GetScriptString(4004) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE1, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE1, "%s", res::GetScriptString(6120) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE2, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE2, "%s", res::GetScriptString(6121) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE3, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE3, "%s", res::GetScriptString(6122) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE4, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE4, "%s", res::GetScriptString(6123) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE6, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE6, "%s", res::GetScriptString(6124) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE7, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE7, "%s", res::GetScriptString(6125) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE8, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE8, "%s", res::GetScriptString(6126) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE9, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE9, "%s", res::GetScriptString(6127) );

			GameSystems::GetInstance()->AwardAchievement(19);
		}break;

		default:
			DBG_ASSERT(0);
			break;
	}

	m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_NEXT, true );
	
	m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_PAGECOUNTER, true );
	m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_PAGECOUNTER, "PAGE %d / %d", m_CurrentPage, m_NumPages );

	m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_TITLE, true );
	m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_EXIT, true );

	// audio
	const char* sr = 0;
	snd::Sound sndLoad;

	sr = res::GetSoundResource( 190 );
	m_PageFlip = snd::SoundLoad( sr, sndLoad );
	DBG_ASSERT( (m_PageFlip != snd::INVALID_SOUNDBUFFER) );
}

/////////////////////////////////////////////////////
/// Method: OnExit
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGameCriminalDocs::OnExit()
{
	res::RemoveModel( m_DocsModels );
	m_DocsModels = 0;

	snd::RemoveSound( m_PageFlip );
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void MicroGameCriminalDocs::Update( float deltaTime )
{
	int i=0;
	math::Vec3 zeroVec( 0.0f, 0.0f, 0.0f );

	// update UI elements
	m_DocsUI.Update( TOUCH_SIZE*0.5f, MAX_TOUCH_TESTS, deltaTime );

	if( m_DocsUI.CheckElementForSingleTouch(HIDDENDOCS_UI_BTN_EXIT) )
	{
		m_Complete = true;
		m_ReturnCode = true;
		return;
	}

	/*if( m_CurrentPage >= m_NumPages )
	{
		if( input::gInputState.TouchesData[input::FIRST_TOUCH].bRelease )
		{
			m_Complete = true;
			m_ReturnCode = true;
			return;
		}
	}*/

	bool clicked = false;

	if( m_NextEnabled )
	{
		if( m_DocsUI.CheckElementForSingleTouch(HIDDENDOCS_UI_BTN_NEXT) )
		{
			m_CurrentPage++;
			if( m_CurrentPage > 1 )
			{
				m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_PREVIOUS, true );
				m_PreviousEnabled = true;
			}
			if( m_CurrentPage == m_NumPages )
			{
				m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_NEXT, false );
				m_NextEnabled = false;
			}

			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_PAGECOUNTER, "PAGE %d / %d", m_CurrentPage, m_NumPages );

			clicked = true;
			AudioSystem::GetInstance()->PlayAudio( m_PageFlip, zeroVec, true );
		}
	}

	if( m_PreviousEnabled )
	{
		if( m_DocsUI.CheckElementForSingleTouch(HIDDENDOCS_UI_BTN_PREV) )
		{
			m_CurrentPage--;
			if( m_CurrentPage == 1 )
			{
				m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_PREVIOUS, false );
				m_PreviousEnabled = false;
			}
			if( m_CurrentPage < m_NumPages )
			{
				m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_NEXT, true );
				m_NextEnabled = true;
			}

			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_PAGECOUNTER, "PAGE %d / %d", m_CurrentPage, m_NumPages );

			clicked = true;
			AudioSystem::GetInstance()->PlayAudio( m_PageFlip, zeroVec, true );
		}
	}

	if(clicked)
	{
		for( i=HIDDENDOCS_UI_LABEL_LINE1; i <= HIDDENDOCS_UI_LABEL_LINE10; ++i )
			m_DocsUI.ChangeElementDrawState( i, false );

		switch( m_WhichDoc )
		{
			case 1:
			{
				if( m_CurrentPage == 1 )
				{
			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE1, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE1, "%s", res::GetScriptString(6040) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE2, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE2, "%s", res::GetScriptString(6041) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE3, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE3, "%s", res::GetScriptString(6042) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE4, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE4, "%s", res::GetScriptString(6043) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE5, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE5, "%s", res::GetScriptString(6044) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE7, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE7, "%s", res::GetScriptString(6045) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE8, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE8, "%s", res::GetScriptString(6046) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE9, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE9, "%s", res::GetScriptString(6047) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE10, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE10, "%s", res::GetScriptString(6048) );
				}
				else if( m_CurrentPage == 2 )
				{
			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE1, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE1, "%s", res::GetScriptString(6049) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE2, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE2, "%s", res::GetScriptString(6050) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE3, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE3, "%s", res::GetScriptString(6051) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE4, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE4, "%s", res::GetScriptString(6052) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE6, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE6, "%s", res::GetScriptString(6053) );
				}

			}break;
			case 2:
			{
				if( m_CurrentPage == 1 )
				{
			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE1, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE1, "%s", res::GetScriptString(6060) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE2, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE2, "%s", res::GetScriptString(6061) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE3, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE3, "%s", res::GetScriptString(6062) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE5, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE5, "%s", res::GetScriptString(6063) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE6, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE6, "%s", res::GetScriptString(6064) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE7, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE7, "%s", res::GetScriptString(6065) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE8, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE8, "%s", res::GetScriptString(6066) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE9, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE9, "%s", res::GetScriptString(6067) );
				}
				else if( m_CurrentPage == 2 )
				{
			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE1, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE1, "%s", res::GetScriptString(6068) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE2, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE2, "%s", res::GetScriptString(6069) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE3, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE3, "%s", res::GetScriptString(6070) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE5, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE5, "%s", res::GetScriptString(6071) );
				}
			}break;
			case 3:
			{
				if( m_CurrentPage == 1 )
				{
			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE1, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE1, "%s", res::GetScriptString(6080) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE2, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE2, "%s", res::GetScriptString(6081) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE3, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE3, "%s", res::GetScriptString(6082) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE4, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE4, "%s", res::GetScriptString(6083) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE5, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE5, "%s", res::GetScriptString(6084) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE6, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE6, "%s", res::GetScriptString(6085) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE8, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE8, "%s", res::GetScriptString(6086) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE9, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE9, "%s", res::GetScriptString(6087) );
				}
				else if( m_CurrentPage == 2 )
				{
			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE1, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE1, "%s", res::GetScriptString(6088) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE2, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE2, "%s", res::GetScriptString(6089) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE4, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE4, "%s", res::GetScriptString(6090) );
				}
				else if( m_CurrentPage == 3 )
				{
			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE1, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE1, "%s", res::GetScriptString(6091) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE2, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE2, "%s", res::GetScriptString(6092) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE3, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE3, "%s", res::GetScriptString(6093) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE4, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE4, "%s", res::GetScriptString(6094) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE5, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE5, "%s", res::GetScriptString(6095) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE7, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE7, "%s", res::GetScriptString(6096) );
				}
			}break;
			case 4:
			{
				if( m_CurrentPage == 1 )
				{
			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE1, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE1, "%s", res::GetScriptString(6100) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE2, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE2, "%s", res::GetScriptString(6101) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE3, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE3, "%s", res::GetScriptString(6102) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE4, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE4, "%s", res::GetScriptString(6103) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE5, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE5, "%s", res::GetScriptString(6104) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE7, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE7, "%s", res::GetScriptString(6105) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE8, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE8, "%s", res::GetScriptString(6106) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE9, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE9, "%s", res::GetScriptString(6107) );
				}
				else if( m_CurrentPage == 2 )
				{
			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE1, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE1, "%s", res::GetScriptString(6108) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE2, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE2, "%s", res::GetScriptString(6109) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE3, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE3, "%s", res::GetScriptString(6110) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE4, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE4, "%s", res::GetScriptString(6111) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE5, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE5, "%s", res::GetScriptString(6112) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE7, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE7, "%s", res::GetScriptString(6113) );
				}
			}break;
			case 5:
			{
				if( m_CurrentPage == 1 )
				{
			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE1, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE1, "%s", res::GetScriptString(6120) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE2, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE2, "%s", res::GetScriptString(6121) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE3, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE3, "%s", res::GetScriptString(6122) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE4, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE4, "%s", res::GetScriptString(6123) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE6, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE6, "%s", res::GetScriptString(6124) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE7, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE7, "%s", res::GetScriptString(6125) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE8, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE8, "%s", res::GetScriptString(6126) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE9, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE9, "%s", res::GetScriptString(6127) );
				}
				else if( m_CurrentPage == 2 )
				{
			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE1, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE1, "%s", res::GetScriptString(6128) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE2, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE2, "%s", res::GetScriptString(6129) );

			m_DocsUI.ChangeElementDrawState( HIDDENDOCS_UI_LABEL_LINE4, true );
			m_DocsUI.ChangeElementText( HIDDENDOCS_UI_LABEL_LINE4, "%s", res::GetScriptString(6130) );
				}
			}break;
			default:
				DBG_ASSERT(0);
			break;
		}
	}
	clicked = false;
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGameCriminalDocs::Draw()
{
	renderer::OpenGL::GetInstance()->SetColour4ub( 255,255,255,255 );

	renderer::OpenGL::GetInstance()->ClearColour( 0.0f, 0.0f, 0.0f, 1.0f );

	renderer::OpenGL::GetInstance()->SetNearFarClip( -500.0f, 100.0f );
	renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_ALWAYS );
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	renderer::OpenGL::GetInstance()->AlphaMode( false, GL_ALWAYS, 0.0f );

	m_DocsModels->Draw();

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_ALWAYS );
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	renderer::OpenGL::GetInstance()->SetNearFarClip( -100.0f, 100.0f );
	renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );

	m_DocsUI.Draw();
}

