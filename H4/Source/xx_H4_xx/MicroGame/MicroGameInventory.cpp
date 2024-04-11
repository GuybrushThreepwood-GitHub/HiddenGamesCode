
/*===================================================================
	File: MicroGameInventory.cpp
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
#include "Resources/StringResources.h"
#include "MicroGameInventory.h"

// generated files
#include "GameStates/UI/GeneratedFiles/inventory.hgm.h" // HGM submesh list
#include "GameStates/UI/GeneratedFiles/inventory.hui.h" // UI element list

namespace
{
	const int MAX_TOUCH_TESTS = 1;
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
MicroGameInventory::MicroGameInventory( )
{
	m_InventoryModel = 0;
	
	m_PageIndex = 0;
	m_SelectedItem = -1;

	m_Clicks = 0;

	m_PistolEquipAudio = snd::INVALID_SOUNDBUFFER;
	m_ShotgunEquipAudio = snd::INVALID_SOUNDBUFFER;
	m_UnEquipAudio = snd::INVALID_SOUNDBUFFER;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
MicroGameInventory::~MicroGameInventory()
{

}

/////////////////////////////////////////////////////
/// Method: Initialise
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGameInventory::Initialise()
{

}

/////////////////////////////////////////////////////
/// Method: Release
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGameInventory::Release()
{

}

/////////////////////////////////////////////////////
/// Method: OnEnter
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGameInventory::OnEnter()
{
	int i=0;

	m_InventoryModel = res::LoadModel( 2012 );
	DBG_ASSERT( (m_InventoryModel != 0) );

	const char* sr = 0;
	snd::Sound sndLoad;

	sr = res::GetSoundResource( 170 );
	m_PistolEquipAudio = snd::SoundLoad( sr, sndLoad );
	DBG_ASSERT( (m_PistolEquipAudio != snd::INVALID_SOUNDBUFFER) );

	sr = res::GetSoundResource( 171 );
	m_ShotgunEquipAudio = snd::SoundLoad( sr, sndLoad );
	DBG_ASSERT( (m_ShotgunEquipAudio != snd::INVALID_SOUNDBUFFER) );

	sr = res::GetSoundResource( 172 );
	m_UnEquipAudio = snd::SoundLoad( sr, sndLoad );
	DBG_ASSERT( (m_UnEquipAudio != snd::INVALID_SOUNDBUFFER) );

	m_InventoryModel->SetMeshDrawState( BTN_INVENTORY_OFF, false );	
	m_InventoryModel->SetMeshDrawState( BTN_INVENTORY_ON, true );

	m_InventoryModel->SetMeshDrawState( BTN_COLLECTABLES_OFF, true );	
	m_InventoryModel->SetMeshDrawState( BTN_COLLECTABLES_ON, false );

	m_InventoryModel->SetMeshDrawState( EQUIPPED_SLOT1, false );
	m_InventoryModel->SetMeshDrawState( EQUIPPED_SLOT2, false );

	// turn off all selections
	for( i=SELECTED_SLOT1; i <= WEAR_SLOT18; ++i )
	{
		m_InventoryModel->SetMeshDrawState( i, false );	
	}

	// all items off
	for( i=TORCH; i <= COSTUME9; ++i )
	{
		m_InventoryModel->SetMeshDrawState( i, false );	
	}


	math::Vec3 scaleFactor = math::Vec3( core::app::GetWidthScale(), core::app::GetHeightScale(), 1.0f );

	math::Vec2 srcAssetDims( static_cast<float>(core::app::GetBaseAssetsWidth()), static_cast<float>(core::app::GetBaseAssetsHeight()) );
	m_InventoryUI.Load( "hui/inventory.hui", srcAssetDims, m_InventoryModel );

	SetupInventory();
}

/////////////////////////////////////////////////////
/// Method: OnExit
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGameInventory::OnExit()
{
	res::RemoveModel( m_InventoryModel );
	m_InventoryModel = 0;

	snd::RemoveSound( m_PistolEquipAudio );
	snd::RemoveSound( m_ShotgunEquipAudio );
	snd::RemoveSound( m_UnEquipAudio );
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void MicroGameInventory::Update( float deltaTime )
{
	int i=0, j=0;
	math::Vec3 zeroVec( 0.0f, 0.0f, 0.0f );

	// update UI elements
	m_InventoryUI.Update( TOUCH_SIZE*0.5f, MAX_TOUCH_TESTS, deltaTime );

	if( m_InventoryUI.CheckElementForSingleTouch( HUI_BTN_EXIT ) )
	{
		m_ReturnCode = false;
		m_Complete = true;
		return;
	}

	if( m_InventoryUI.CheckElementForSingleTouch( BTN_TOGGLE_INVENTORY ) )
	{
		m_InventoryModel->SetMeshDrawState( BTN_INVENTORY_OFF, false );	
		m_InventoryModel->SetMeshDrawState( BTN_INVENTORY_ON, true );

		m_InventoryModel->SetMeshDrawState( BTN_COLLECTABLES_OFF, true );	
		m_InventoryModel->SetMeshDrawState( BTN_COLLECTABLES_ON, false );

		if( m_PageIndex != 0 )
		{
			m_InventoryModel->SetMeshDrawState( (SELECTED_SLOT1+m_SelectedItem), false );	

			m_SelectedItem = -1;
			m_PageIndex = 0;

			//m_InventoryModel->SetMeshDrawState( (SELECTED_SLOT1+m_SelectedItem), true );
		}
	}
	else if( m_InventoryUI.CheckElementForSingleTouch( BTN_TOGGLE_COLLECTABLES ) )
	{
		m_InventoryModel->SetMeshDrawState( BTN_INVENTORY_OFF, true );	
		m_InventoryModel->SetMeshDrawState( BTN_INVENTORY_ON, false );

		m_InventoryModel->SetMeshDrawState( BTN_COLLECTABLES_OFF, false );	
		m_InventoryModel->SetMeshDrawState( BTN_COLLECTABLES_ON, true );

		if( m_PageIndex != 1 )
		{
			m_InventoryModel->SetMeshDrawState( (SELECTED_SLOT1+m_SelectedItem), false );	

			m_SelectedItem = -1;
			m_PageIndex = 1;
			m_Clicks = 0;

			//m_InventoryModel->SetMeshDrawState( (SELECTED_SLOT1+m_SelectedItem), true );
		}
	}

	int selectedItem = m_SelectedItem;
	bool clicked = false;
	for( i=BTN_SLOT1, j=0; i <= BTN_SLOT18; ++i, ++j )
	{
		if( m_InventoryUI.CheckElementForSingleTouch( i ) )
		{
			selectedItem = j;
			clicked = true;
			break;
		}
	}

	if( selectedItem != m_SelectedItem )
	{
		// disable the old selected
		m_InventoryModel->SetMeshDrawState( (SELECTED_SLOT1+m_SelectedItem), false );	

		m_SelectedItem = selectedItem;

		// enabled the new selected
		m_InventoryModel->SetMeshDrawState( (SELECTED_SLOT1+m_SelectedItem), true );	
	}
	else
	{
		if( clicked && m_PageIndex == 1 )
		{
			if( selectedItem == 0 )
			{
				// pistol
				m_Clicks++;
			}
			else if( selectedItem == 1 )
			{
				// shotgun
				m_Clicks++;
			}
			else
			{
				if( selectedItem >= 9 || selectedItem <= 18 )
					m_Clicks++;
			}
		}
	}

	if( m_PageIndex == 0 )
		SetupInventory();
	else
		SetupCollectables();
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGameInventory::Draw()
{
	renderer::OpenGL::GetInstance()->ClearColour( 0.0f, 0.0f, 0.0f, 1.0f );

	renderer::OpenGL::GetInstance()->SetNearFarClip( -500.0f, 100.0f );
	renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_ALWAYS );
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	renderer::OpenGL::GetInstance()->AlphaMode( false, GL_ALWAYS, 0.0f );

	m_InventoryModel->Draw();
	
	renderer::OpenGL::GetInstance()->DepthMode( true, GL_ALWAYS );
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	renderer::OpenGL::GetInstance()->SetNearFarClip( -100.0f, 100.0f );
	renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );

	m_InventoryUI.Draw();

}

/////////////////////////////////////////////////////
/// Method: SetupInventory
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGameInventory::SetupInventory()
{
	int i=0;
	const char* description1 = 0;
	const char* description2 = 0;

	for( i=TORCH; i<=COSTUME9; ++i )
		m_InventoryModel->SetMeshDrawState( i, false );

	// selection disable
	for( i=EQUIPPED_SLOT1; i<=WEAR_SLOT18; ++i )
		m_InventoryModel->SetMeshDrawState( i, false );
	
//	AddItem(6000) -- paper (s01 room)
//	AddItem(6002) -- severed hand
//	AddItem(6003) -- fuse
//	AddItem(6005) -- lockerkey4
//	AddItem(6006) -- clipboard for slider hold
//	AddItem(6009) -- dw80
	
//	AddItem(6010) -- security card level 1
//	AddItem(6011) -- security card level 2
//	AddItem(6012) -- security card level 3

//	AddItem(6016) -- key green
//	AddItem(6017) -- key red

//	AddItem(6019) -- mobile phone
//	AddItem(6020) -- torch
//	AddItem(6021) -- pilots log
//	AddItem(6022) -- power relay
//	AddItem(6023) -- screwdriver
//	AddItem(6024) -- paper 2 (safe code)

	if( GameSystems::GetInstance()->HasItem(6020) )
	{
		m_InventoryModel->SetMeshDrawState( TORCH, true );

		if( m_SelectedItem == 0 )
		{
			description1 = res::GetScriptString( 2000 );
			description2 = res::GetScriptString( 2001 );
		}
	}
	else
		m_InventoryModel->SetMeshDrawState( TORCH, false );

	if( GameSystems::GetInstance()->HasItem(6003) )
	{
		m_InventoryModel->SetMeshDrawState( FUSES, true );

		if( m_SelectedItem == 1 )
		{
			description1 = res::GetScriptString( 2002 );
			description2 = res::GetScriptString( 2003 );
		}
	}
	else
		m_InventoryModel->SetMeshDrawState( FUSES, false );

	if( GameSystems::GetInstance()->HasItem(6006) )
	{
		m_InventoryModel->SetMeshDrawState( CLIPBOARD1, true );

		if( m_SelectedItem == 2 )
		{
			description1 = res::GetScriptString( 2004 );
			description2 = res::GetScriptString( 2005 );
		}
	}
	else
		m_InventoryModel->SetMeshDrawState( CLIPBOARD1, false );

	if( GameSystems::GetInstance()->HasItem(6000) )
	{
		m_InventoryModel->SetMeshDrawState( NOTE1, true );

		if( m_SelectedItem == 3 )
		{
			description1 = res::GetScriptString( 2006 );
			description2 = res::GetScriptString( 2007 );
		}
	}
	else
		m_InventoryModel->SetMeshDrawState( NOTE1, false );

	if( GameSystems::GetInstance()->HasItem(6019) )
	{
		m_InventoryModel->SetMeshDrawState( MOBILE, true );

		if( m_SelectedItem == 4 )
		{
			description1 = res::GetScriptString( 2008 );
			description2 = res::GetScriptString( 2009 );
		}
	}
	else
		m_InventoryModel->SetMeshDrawState( MOBILE, false );

	if( GameSystems::GetInstance()->HasItem(6009) )
	{
		m_InventoryModel->SetMeshDrawState( LUBE, true );

		if( m_SelectedItem == 5 )
		{
			description1 = res::GetScriptString( 2010 );
			description2 = res::GetScriptString( 2011 );
		}
	}
	else
		m_InventoryModel->SetMeshDrawState( LUBE, false );

	if( GameSystems::GetInstance()->HasItem(6005) )
	{
		m_InventoryModel->SetMeshDrawState( KEY1, true );

		if( m_SelectedItem == 6 )
		{
			description1 = res::GetScriptString( 2012 );
			description2 = res::GetScriptString( 2013 );
		}
	}
	else
		m_InventoryModel->SetMeshDrawState( KEY1, false );

	if( GameSystems::GetInstance()->HasItem(6016) )
	{
		m_InventoryModel->SetMeshDrawState( KEY2, true );

		if( m_SelectedItem == 7 )
		{
			description1 = res::GetScriptString( 2014 );
			description2 = res::GetScriptString( 2015 );
		}
	}
	else
		m_InventoryModel->SetMeshDrawState( KEY2, false );

	if( GameSystems::GetInstance()->HasItem(6017) )
	{
		m_InventoryModel->SetMeshDrawState( KEY3, true );

		if( m_SelectedItem == 8 )
		{
			description1 = res::GetScriptString( 2016 );
			description2 = res::GetScriptString( 2017 );
		}
	}
	else
		m_InventoryModel->SetMeshDrawState( KEY3, false );

	if( GameSystems::GetInstance()->HasItem(6010) )
	{
		m_InventoryModel->SetMeshDrawState( KEYCARD1, true );

		if( m_SelectedItem == 9 )
		{
			description1 = res::GetScriptString( 2018 );
			description2 = res::GetScriptString( 2019 );
		}
	}
	else
		m_InventoryModel->SetMeshDrawState( KEYCARD1, false );

	if( GameSystems::GetInstance()->HasItem(6011) )
	{
		m_InventoryModel->SetMeshDrawState( KEYCARD2, true );

		if( m_SelectedItem == 10 )
		{
			description1 = res::GetScriptString( 2020 );
			description2 = res::GetScriptString( 2021 );
		}
	}
	else
		m_InventoryModel->SetMeshDrawState( KEYCARD2, false );

	if( GameSystems::GetInstance()->HasItem(6012) )
	{
		m_InventoryModel->SetMeshDrawState( KEYCARD3, true );

		if( m_SelectedItem == 11 )
		{
			description1 = res::GetScriptString( 2022 );
			description2 = res::GetScriptString( 2023 );
		}
	}
	else
		m_InventoryModel->SetMeshDrawState( KEYCARD3, false );

	if( GameSystems::GetInstance()->HasItem(6022) )
	{
		m_InventoryModel->SetMeshDrawState( HELIPART, true );

		if( m_SelectedItem == 12 )
		{
			description1 = res::GetScriptString( 2024 );
			description2 = res::GetScriptString( 2025 );
		}
	}
	else
		m_InventoryModel->SetMeshDrawState( HELIPART, false );

	if( GameSystems::GetInstance()->HasItem(6021) )
	{
		m_InventoryModel->SetMeshDrawState( CLIPBOARD2, true );

		if( m_SelectedItem == 13 )
		{
			description1 = res::GetScriptString( 2026 );
			description2 = res::GetScriptString( 2027 );
		}
	}
	else
		m_InventoryModel->SetMeshDrawState( CLIPBOARD2, false );

	if( GameSystems::GetInstance()->HasItem(6002) )
	{
		m_InventoryModel->SetMeshDrawState( HAND, true );

		if( m_SelectedItem == 14 )
		{
			description1 = res::GetScriptString( 2028 );
			description2 = res::GetScriptString( 2029 );
		}
	}
	else
		m_InventoryModel->SetMeshDrawState( HAND, false );

	if( GameSystems::GetInstance()->HasItem(6024) )
	{
		m_InventoryModel->SetMeshDrawState( NOTE2, true );

		if( m_SelectedItem == 15 )
		{
			description1 = res::GetScriptString( 2030 );
			description2 = res::GetScriptString( 2031 );
		}
	}
	else
		m_InventoryModel->SetMeshDrawState( NOTE2, false );

	if( GameSystems::GetInstance()->HasItem(6023) )
	{
		m_InventoryModel->SetMeshDrawState( SCREWDRIVER, true );

		if( m_SelectedItem == 16 )
		{
			description1 = res::GetScriptString( 2032 );
			description2 = res::GetScriptString( 2033 );
		}
	}
	else
		m_InventoryModel->SetMeshDrawState( SCREWDRIVER, false );

	if( GameSystems::GetInstance()->HasItem(6031) )
	{
		m_InventoryModel->SetMeshDrawState( ROPE, true );

		if( m_SelectedItem == 17 )
		{
			description1 = res::GetScriptString( 2034 );
			description2 = res::GetScriptString( 2035 );
		}
	}
	else
		m_InventoryModel->SetMeshDrawState( ROPE, false );

	if( description1 )
		m_InventoryUI.ChangeElementText( LABEL_DESCRIPTION_LINE1, description1 );
	else
		m_InventoryUI.ChangeElementText( LABEL_DESCRIPTION_LINE1, "" );

	if( description2 )
		m_InventoryUI.ChangeElementText( LABEL_DESCRIPTION_LINE2, description2 );
	else
		m_InventoryUI.ChangeElementText( LABEL_DESCRIPTION_LINE2, "" );
}

/////////////////////////////////////////////////////
/// Method: SetupCollectables
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGameInventory::SetupCollectables()
{
	int i=0;
	const char* description1 = 0;
	const char* description2 = 0;
	std::memset( &m_ValString, 0, sizeof(char)*32 );
	math::Vec3 zeroVec( 0.0f, 0.0f, 0.0f );

	Player& player = GameSystems::GetInstance()->GetPlayer();
	int weaponIndex = player.GetWeaponIndex();
	int clothingIndex = player.GetClothingIndex();

	// remove items
	for( i=TORCH; i<=COSTUME9; ++i )
		m_InventoryModel->SetMeshDrawState( i, false );

	for( i=EQUIPPED_SLOT1; i<=WEAR_SLOT18; ++i )
		m_InventoryModel->SetMeshDrawState( i, false );

	// weapons
	if( GameSystems::GetInstance()->HasItem(6001) )
	{
		m_InventoryModel->SetMeshDrawState( PISTOL, true );

		if( m_SelectedItem == 0 )
		{
			description1 = res::GetScriptString( 3000 );
			snprintf( m_ValString, 32, "Tap to equip/unequip" );
			description2 = &m_ValString[0];

			if( weaponIndex == 0 )
			{
				// current weapon is the pistol
				if( m_Clicks >= 1 )
				{
					m_Clicks = 0;
					weaponIndex = -1;
					player.SetWeaponIndex( -1 );

					AudioSystem::GetInstance()->PlayAudio( m_UnEquipAudio, zeroVec, true );
					m_InventoryModel->SetMeshDrawState( SELECTED_SLOT1, true );
				}
				else
				{
					//snprintf( m_ValString, 32, "EQUIPPED" );
					//description2 = &m_ValString[0];
				}
			}
			else
			{
				// current weapon is not the pistol
				if( m_Clicks >= 1 )
				{					
					//snprintf( m_ValString, 32, "EQUIPPED" );
					//description2 = &m_ValString[0];

					m_Clicks = 0;
					weaponIndex = 0;
					player.SetWeaponIndex( 0 );

					AudioSystem::GetInstance()->PlayAudio( m_PistolEquipAudio, zeroVec, true );
				}
				else
				{
					//snprintf( m_ValString, 32, "NOT EQUIPPED" );
					//description2 = &m_ValString[0];
				}
			}
		}

		if( weaponIndex == 0 )
		{
			m_InventoryModel->SetMeshDrawState( EQUIPPED_SLOT1, true );
			m_InventoryModel->SetMeshDrawState( SELECTED_SLOT1, false );
		}
		else
		{
			m_InventoryModel->SetMeshDrawState( EQUIPPED_SLOT1, false );
		}
	}
	else
		m_InventoryModel->SetMeshDrawState( PISTOL, false );

	if( GameSystems::GetInstance()->HasItem(6004) )
	{
		m_InventoryModel->SetMeshDrawState( SHOTGUN, true );

		if( m_SelectedItem == 1 )
		{
			description1 = res::GetScriptString( 3002 );
			snprintf( m_ValString, 32, "Tap to equip/unequip" );
			description2 = &m_ValString[0];
			//description2 = res::GetScriptString( 3003 );

			if( weaponIndex == 1 )
			{
				// current weapon is the shotgun
				if( m_Clicks >= 1 )
				{
					//snprintf( m_ValString, 32, "NOT EQUIPPED" );
					//description2 = &m_ValString[0];

					m_Clicks = 0;
					weaponIndex = -1;
					player.SetWeaponIndex( -1 );

					AudioSystem::GetInstance()->PlayAudio( m_UnEquipAudio, zeroVec, true );
					m_InventoryModel->SetMeshDrawState( SELECTED_SLOT2, true );
				}
				else
				{
					//snprintf( m_ValString, 32, "EQUIPPED" );
					//description2 = &m_ValString[0];
				}
			}
			else
			{
				// current weapon is not the shotgun
				if( m_Clicks >= 1 )
				{					
					//snprintf( m_ValString, 32, "EQUIPPED" );
					//description2 = &m_ValString[0];

					m_Clicks = 0;
					weaponIndex = 1;
					player.SetWeaponIndex( 1 );

					AudioSystem::GetInstance()->PlayAudio( m_ShotgunEquipAudio, zeroVec, true );
				}
				else
				{
					//snprintf( m_ValString, 32, "NOT EQUIPPED" );
					//description2 = &m_ValString[0];
				}
			}
		}

		if( weaponIndex == 1 )
		{
			m_InventoryModel->SetMeshDrawState( EQUIPPED_SLOT2, true );
			m_InventoryModel->SetMeshDrawState( SELECTED_SLOT2, false );
		}
		else
		{
			m_InventoryModel->SetMeshDrawState( EQUIPPED_SLOT2, false );
		}
	}
	else
		m_InventoryModel->SetMeshDrawState( SHOTGUN, false );

	int val = 0;

	// criminal docs
	val += GameSystems::GetInstance()->GetDataValue( "criminal_docs_1" );
	val += GameSystems::GetInstance()->GetDataValue( "criminal_docs_2" );
	val += GameSystems::GetInstance()->GetDataValue( "criminal_docs_3" );
	val += GameSystems::GetInstance()->GetDataValue( "criminal_docs_4" );
	val += GameSystems::GetInstance()->GetDataValue( "criminal_docs_5" );

	if( val > 0 )
	{
		m_InventoryModel->SetMeshDrawState( DOCUMENTS, true );

		if( m_SelectedItem == 2 )
		{
			description1 = res::GetScriptString( 3004 );

			snprintf( m_ValString, 32, "%d of 5 found", val );
			description2 = &m_ValString[0];
		}
	}
	else
		m_InventoryModel->SetMeshDrawState( DOCUMENTS, false );

	// hidden tokens
	val = 0;
	val += GameSystems::GetInstance()->GetDataValue( "hidden_token_1" );
	val += GameSystems::GetInstance()->GetDataValue( "hidden_token_2" );
	val += GameSystems::GetInstance()->GetDataValue( "hidden_token_3" );
	val += GameSystems::GetInstance()->GetDataValue( "hidden_token_4" );
	val += GameSystems::GetInstance()->GetDataValue( "hidden_token_5" );

	if( val > 0 )
	{
		m_InventoryModel->SetMeshDrawState( TOKENS, true );

		if( m_SelectedItem == 3 )
		{
			description1 = res::GetScriptString( 3006 );

			snprintf( m_ValString, 32, "%d of 5 found", val );
			description2 = &m_ValString[0];
		}
	}
	else
		m_InventoryModel->SetMeshDrawState( TOKENS, false );

	// heads
	val = GameSystems::GetInstance()->GetDataValue( "head_hunter_1" );
	if( val > 0 )
	{
		m_InventoryModel->SetMeshDrawState( HEAD1, true );

		if( m_SelectedItem == 4 )
		{
			description1 = res::GetScriptString( 3008 );
			description2 = res::GetScriptString( 3009 );
		}
	}
	else
		m_InventoryModel->SetMeshDrawState( HEAD1, false );

	val = GameSystems::GetInstance()->GetDataValue( "head_hunter_2" );
	if( val > 0 )
	{
		m_InventoryModel->SetMeshDrawState( HEAD2, true );

		if( m_SelectedItem == 5 )
		{
			description1 = res::GetScriptString( 3010 );
			description2 = res::GetScriptString( 3011 );
		}
	}
	else
		m_InventoryModel->SetMeshDrawState( HEAD2, false );

	val = GameSystems::GetInstance()->GetDataValue( "head_hunter_3" );
	if( val > 0 )
	{
		m_InventoryModel->SetMeshDrawState( HEAD3, true );

		if( m_SelectedItem == 6 )
		{
			description1 = res::GetScriptString( 3012 );
			description2 = res::GetScriptString( 3013 );
		}
	}
	else
		m_InventoryModel->SetMeshDrawState( HEAD3, false );

	val = GameSystems::GetInstance()->GetDataValue( "head_hunter_4" );
	if( val > 0 )
	{
		m_InventoryModel->SetMeshDrawState( HEAD4, true );

		if( m_SelectedItem == 7 )
		{
			description1 = res::GetScriptString( 3014 );
			description2 = res::GetScriptString( 3015 );
		}
	}
	else
		m_InventoryModel->SetMeshDrawState( HEAD4, false );

	val = GameSystems::GetInstance()->GetDataValue( "head_hunter_5" );
	if( val > 0 )
	{
		m_InventoryModel->SetMeshDrawState( HEAD5, true );

		if( m_SelectedItem == 8 )
		{
			description1 = res::GetScriptString( 3016 );
			description2 = res::GetScriptString( 3017 );
		}
	}
	else
		m_InventoryModel->SetMeshDrawState( HEAD5, false );

	// costume
///////////////////////////////////////////////////////////////////////////////////////////////////
	if( GameSystems::GetInstance()->HasItem(7000) )
	{
		m_InventoryModel->SetMeshDrawState( COSTUME1, true );

		if( m_SelectedItem == 9 )
		{
			description1 = res::GetScriptString( 3025 );
			snprintf( m_ValString, 32, "Tap to wear" );
			description2 = &m_ValString[0];

			if( clothingIndex == 0 )
				description2 = 0;

			if( m_Clicks >= 1 )
			{					
				m_Clicks = 0;
				clothingIndex = 0;
				player.SetClothingIndex( 0 );

				//AudioSystem::GetInstance()->PlayAudio( m_PistolEquipAudio, zeroVec, true );
			}
		}

		if( clothingIndex == 0 )
		{
			m_InventoryModel->SetMeshDrawState( WEAR_SLOT10, true );
			m_InventoryModel->SetMeshDrawState( SELECTED_SLOT10, false );
		}
		else
		{
			m_InventoryModel->SetMeshDrawState( WEAR_SLOT10, false );
		}
	}
	else
		m_InventoryModel->SetMeshDrawState( COSTUME1, false );
///////////////////////////////////////////////////////////////////////////////////////////////////
	if( GameSystems::GetInstance()->HasItem(7001) )
	{
		m_InventoryModel->SetMeshDrawState( COSTUME2, true );

		if( m_SelectedItem == 10 )
		{
			description1 = res::GetScriptString( 3027 );
			snprintf( m_ValString, 32, "Tap to wear" );
			description2 = &m_ValString[0];

			if( clothingIndex == 1 )
				description2 = 0;

			// current clothing is not this one
			if( m_Clicks >= 1 )
			{					
				m_Clicks = 0;
				clothingIndex = 1;
				player.SetClothingIndex( 1 );

				//AudioSystem::GetInstance()->PlayAudio( m_PistolEquipAudio, zeroVec, true );
			}
		}

		if( clothingIndex == 1 )
		{
			m_InventoryModel->SetMeshDrawState( WEAR_SLOT11, true );
			m_InventoryModel->SetMeshDrawState( SELECTED_SLOT11, false );
		}
		else
		{
			m_InventoryModel->SetMeshDrawState( WEAR_SLOT11, false );
		}
	}
	else
		m_InventoryModel->SetMeshDrawState( COSTUME2, false );
///////////////////////////////////////////////////////////////////////////////////////////////////
	if( GameSystems::GetInstance()->HasItem(7002) )
	{
		m_InventoryModel->SetMeshDrawState( COSTUME3, true );

		if( m_SelectedItem == 11 )
		{
			description1 = res::GetScriptString( 3029 );
			snprintf( m_ValString, 32, "Tap to wear" );
			description2 = &m_ValString[0];

			if( clothingIndex == 2 )
				description2 = 0;

			// current clothing is not this one
			if( m_Clicks >= 1 )
			{					
				m_Clicks = 0;
				clothingIndex = 2;
				player.SetClothingIndex( 2 );

				//AudioSystem::GetInstance()->PlayAudio( m_PistolEquipAudio, zeroVec, true );
			}
		}

		if( clothingIndex == 2 )
		{
			m_InventoryModel->SetMeshDrawState( WEAR_SLOT12, true );
			m_InventoryModel->SetMeshDrawState( SELECTED_SLOT12, false );
		}
		else
		{
			m_InventoryModel->SetMeshDrawState( WEAR_SLOT12, false );
		}
	}
	else
		m_InventoryModel->SetMeshDrawState( COSTUME3, false );
///////////////////////////////////////////////////////////////////////////////////////////////////
	if( GameSystems::GetInstance()->HasItem(7003) )
	{
		m_InventoryModel->SetMeshDrawState( COSTUME4, true );

		if( m_SelectedItem == 12 )
		{
			description1 = res::GetScriptString( 3031 );
			snprintf( m_ValString, 32, "Tap to wear" );
			description2 = &m_ValString[0];

			if( clothingIndex == 3 )
				description2 = 0;

			// current clothing is not this one
			if( m_Clicks >= 1 )
			{					
				m_Clicks = 0;
				clothingIndex = 3;
				player.SetClothingIndex( 3 );

				//AudioSystem::GetInstance()->PlayAudio( m_PistolEquipAudio, zeroVec, true );
			}
		}

		if( clothingIndex == 3 )
		{
			m_InventoryModel->SetMeshDrawState( WEAR_SLOT13, true );
			m_InventoryModel->SetMeshDrawState( SELECTED_SLOT13, false );
		}
		else
		{
			m_InventoryModel->SetMeshDrawState( WEAR_SLOT13, false );
		}
	}
	else
		m_InventoryModel->SetMeshDrawState( COSTUME4, false );
///////////////////////////////////////////////////////////////////////////////////////////////////
	if( GameSystems::GetInstance()->HasItem(7004) )
	{
		m_InventoryModel->SetMeshDrawState( COSTUME5, true );

		if( m_SelectedItem == 13 )
		{
			description1 = res::GetScriptString( 3033 );
			snprintf( m_ValString, 32, "Tap to wear" );
			description2 = &m_ValString[0];

			if( clothingIndex == 4 )
				description2 = 0;

			// current clothing is not this one
			if( m_Clicks >= 1 )
			{					
				m_Clicks = 0;
				clothingIndex = 4;
				player.SetClothingIndex( 4 );

				//AudioSystem::GetInstance()->PlayAudio( m_PistolEquipAudio, zeroVec, true );
			}
		}

		if( clothingIndex == 4 )
		{
			m_InventoryModel->SetMeshDrawState( WEAR_SLOT14, true );
			m_InventoryModel->SetMeshDrawState( SELECTED_SLOT14, false );
		}
		else
		{
			m_InventoryModel->SetMeshDrawState( WEAR_SLOT14, false );
		}
	}
	else
		m_InventoryModel->SetMeshDrawState( COSTUME5, false );
///////////////////////////////////////////////////////////////////////////////////////////////////
	if( GameSystems::GetInstance()->HasItem(7005) )
	{
		m_InventoryModel->SetMeshDrawState( COSTUME6, true );

		if( m_SelectedItem == 14 )
		{
			description1 = res::GetScriptString( 3035 );
			snprintf( m_ValString, 32, "Tap to wear" );
			description2 = &m_ValString[0];

			if( clothingIndex == 5 )
				description2 = 0;

			// current clothing is not this one
			if( m_Clicks >= 1 )
			{					
				m_Clicks = 0;
				clothingIndex = 5;
				player.SetClothingIndex( 5 );

				//AudioSystem::GetInstance()->PlayAudio( m_PistolEquipAudio, zeroVec, true );
			}
		}

		if( clothingIndex == 5 )
		{
			m_InventoryModel->SetMeshDrawState( WEAR_SLOT15, true );
			m_InventoryModel->SetMeshDrawState( SELECTED_SLOT15, false );
		}
		else
		{
			m_InventoryModel->SetMeshDrawState( WEAR_SLOT15, false );
		}
	}
	else
		m_InventoryModel->SetMeshDrawState( COSTUME6, false );
///////////////////////////////////////////////////////////////////////////////////////////////////
	if( GameSystems::GetInstance()->HasItem(7006) )
	{
		m_InventoryModel->SetMeshDrawState( COSTUME7, true );

		if( m_SelectedItem == 15 )
		{
			description1 = res::GetScriptString( 3037 );
			snprintf( m_ValString, 32, "Tap to wear" );
			description2 = &m_ValString[0];

			if( clothingIndex == 6 )
				description2 = 0;

			// current clothing is not this one
			if( m_Clicks >= 1 )
			{					
				m_Clicks = 0;
				clothingIndex = 6;
				player.SetClothingIndex( 6 );

				//AudioSystem::GetInstance()->PlayAudio( m_PistolEquipAudio, zeroVec, true );
			}
		}

		if( clothingIndex == 6 )
		{
			m_InventoryModel->SetMeshDrawState( WEAR_SLOT16, true );
			m_InventoryModel->SetMeshDrawState( SELECTED_SLOT16, false );
		}
		else
		{
			m_InventoryModel->SetMeshDrawState( WEAR_SLOT16, false );
		}
	}
	else
		m_InventoryModel->SetMeshDrawState( COSTUME7, false );
///////////////////////////////////////////////////////////////////////////////////////////////////
	if( GameSystems::GetInstance()->HasItem(7007) )
	{
		m_InventoryModel->SetMeshDrawState( COSTUME8, true );

		if( m_SelectedItem == 16 )
		{
			description1 = res::GetScriptString( 3039 );
			snprintf( m_ValString, 32, "Tap to wear" );
			description2 = &m_ValString[0];

			if( clothingIndex == 7 )
				description2 = 0;

			// current clothing is not this one
			if( m_Clicks >= 1 )
			{					
				m_Clicks = 0;
				clothingIndex = 7;
				player.SetClothingIndex( 7 );

				//AudioSystem::GetInstance()->PlayAudio( m_PistolEquipAudio, zeroVec, true );
			}
		}

		if( clothingIndex == 7 )
		{
			m_InventoryModel->SetMeshDrawState( WEAR_SLOT17, true );
			m_InventoryModel->SetMeshDrawState( SELECTED_SLOT17, false );
		}
		else
		{
			m_InventoryModel->SetMeshDrawState( WEAR_SLOT17, false );
		}
	}
	else
		m_InventoryModel->SetMeshDrawState( COSTUME8, false );
///////////////////////////////////////////////////////////////////////////////////////////////////
	if( GameSystems::GetInstance()->HasItem(7008) )
	{
		m_InventoryModel->SetMeshDrawState( COSTUME9, true );

		if( m_SelectedItem == 17 )
		{
			description1 = res::GetScriptString( 3041 );
			snprintf( m_ValString, 32, "Tap to wear" );
			description2 = &m_ValString[0];

			if( clothingIndex == 8 )
				description2 = 0;

			// current clothing is not this one
			if( m_Clicks >= 1 )
			{					
				m_Clicks = 0;
				clothingIndex = 8;
				player.SetClothingIndex( 8 );

				//AudioSystem::GetInstance()->PlayAudio( m_PistolEquipAudio, zeroVec, true );
			}
		}

		if( clothingIndex == 8 )
		{
			m_InventoryModel->SetMeshDrawState( WEAR_SLOT18, true );
			m_InventoryModel->SetMeshDrawState( SELECTED_SLOT18, false );
		}
		else
		{
			m_InventoryModel->SetMeshDrawState( WEAR_SLOT18, false );
		}
	}
	else
		m_InventoryModel->SetMeshDrawState( COSTUME9, false );

	// set up description
	if( description1 )
		m_InventoryUI.ChangeElementText( LABEL_DESCRIPTION_LINE1, description1 );
	else
		m_InventoryUI.ChangeElementText( LABEL_DESCRIPTION_LINE1, "" );

	if( description2 )
		m_InventoryUI.ChangeElementText( LABEL_DESCRIPTION_LINE2, description2 );
	else
		m_InventoryUI.ChangeElementText( LABEL_DESCRIPTION_LINE2, "" );
}

