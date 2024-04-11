
/*===================================================================
	File: MicroGameUseItem.cpp
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
#include "MicroGameUseItem.h"

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
MicroGameUseItem::MicroGameUseItem( int correctSlot, const char* giveupFunc )
{
	m_InventoryModel = 0;
	
	m_SelectedItem = -1;

	m_CorrectSlot = correctSlot;
	m_GiveUpFunc = giveupFunc;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
MicroGameUseItem::~MicroGameUseItem()
{

}

/////////////////////////////////////////////////////
/// Method: Initialise
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGameUseItem::Initialise()
{

}

/////////////////////////////////////////////////////
/// Method: Release
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGameUseItem::Release()
{

}

/////////////////////////////////////////////////////
/// Method: OnEnter
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGameUseItem::OnEnter()
{
	int i=0;

	m_InventoryModel = res::LoadModel( 2012 );
	DBG_ASSERT( (m_InventoryModel != 0) );

	math::Vec2 srcAssetDims( static_cast<float>(core::app::GetBaseAssetsWidth()), static_cast<float>(core::app::GetBaseAssetsHeight()) );
	m_InventoryUI.Load( "hui/inventory.hui", srcAssetDims, m_InventoryModel );

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

	SetupInventory();
}

/////////////////////////////////////////////////////
/// Method: OnExit
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGameUseItem::OnExit()
{
	res::RemoveModel( m_InventoryModel );
	m_InventoryModel = 0;
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void MicroGameUseItem::Update( float deltaTime )
{
	int i=0, j=0;
	math::Vec3 zeroVec( 0.0f, 0.0f, 0.0f );

	// update UI elements
	m_InventoryUI.Update( TOUCH_SIZE*0.5f, MAX_TOUCH_TESTS, deltaTime );

	if( m_InventoryUI.CheckElementForSingleTouch( HUI_BTN_EXIT ) )
	{
		if( !core::IsEmptyString(m_GiveUpFunc) )
		{
			int result = 0;
			int errorFuncIndex;
			errorFuncIndex = script::GetErrorFuncIndex();

			lua_pop( script::LuaScripting::GetState(), 1 );

			lua_getglobal( script::LuaScripting::GetState(), m_GiveUpFunc );

			result = lua_pcall( script::LuaScripting::GetState(), 0, 0, errorFuncIndex );

			// LUA_ERRRUN --- a runtime error. 
			// LUA_ERRMEM --- memory allocation error. For such errors, Lua does not call the error handler function. 
			// LUA_ERRERR --- error while running the error handler function. 

			if( result == LUA_ERRRUN || result == LUA_ERRMEM || result == LUA_ERRERR )
			{
				DBGLOG( "*ERROR* Calling function '%s' failed\n", m_GiveUpFunc );
				DBGLOG( "\tLUA_TRACEBACK: %s\n", lua_tostring( script::LuaScripting::GetState(), -1 ) );

				DBG_ASSERT_MSG( 0, "*ERROR* Calling function '%s' failed", m_GiveUpFunc );
			}
		}

		m_ReturnCode = false;
		m_Complete = true;

		return;
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

	m_SelectedItem = selectedItem;

	m_HasItem = false;

	//order matters for this
	SetupInventory();

	// check
	if( clicked &&
		(m_CorrectSlot == m_SelectedItem) )
	{
		// clicked on correct slot
		if( m_HasItem )
		{
			// has item done
			m_ReturnCode = true;
			m_Complete = true;

			return;
		}

		// no item does nothing
	}
	else if( clicked &&
		(m_CorrectSlot != m_SelectedItem) )
	{
		if( !m_HasItem )
		{
			// does nothing
		}
		else
		{
			// something in the slot, but it's the wrong item to use
			if( (m_CorrectSlot != m_SelectedItem) )
			{
				int wrongItemString = math::RandomNumber( 300, 305 );

				GameSystems::GetInstance()->Say( wrongItemString, -1, "" );

				m_ReturnCode = false;
				m_Complete = true;

				return;
			}
		}
	}
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGameUseItem::Draw()
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
void MicroGameUseItem::SetupInventory()
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
			m_HasItem = true;
		}
	}
	else
		m_InventoryModel->SetMeshDrawState( TORCH, false );

	if( GameSystems::GetInstance()->HasItem(6003) )
	{
		m_InventoryModel->SetMeshDrawState( FUSES, true );

		if( m_SelectedItem == 1 )
		{
			m_HasItem = true;
		}
	}
	else
		m_InventoryModel->SetMeshDrawState( FUSES, false );

	if( GameSystems::GetInstance()->HasItem(6006) )
	{
		m_InventoryModel->SetMeshDrawState( CLIPBOARD1, true );

		if( m_SelectedItem == 2 )
		{
			m_HasItem = true;
		}
	}
	else
		m_InventoryModel->SetMeshDrawState( CLIPBOARD1, false );

	if( GameSystems::GetInstance()->HasItem(6000) )
	{
		m_InventoryModel->SetMeshDrawState( NOTE1, true );

		if( m_SelectedItem == 3 )
		{
			m_HasItem = true;
		}
	}
	else
		m_InventoryModel->SetMeshDrawState( NOTE1, false );

	if( GameSystems::GetInstance()->HasItem(6019) )
	{
		m_InventoryModel->SetMeshDrawState( MOBILE, true );

		if( m_SelectedItem == 4 )
		{
			m_HasItem = true;
		}
	}
	else
		m_InventoryModel->SetMeshDrawState( MOBILE, false );

	if( GameSystems::GetInstance()->HasItem(6009) )
	{
		m_InventoryModel->SetMeshDrawState( LUBE, true );

		if( m_SelectedItem == 5 )
		{
			m_HasItem = true;
		}
	}
	else
		m_InventoryModel->SetMeshDrawState( LUBE, false );

	if( GameSystems::GetInstance()->HasItem(6005) )
	{
		m_InventoryModel->SetMeshDrawState( KEY1, true );

		if( m_SelectedItem == 6 )
		{
			m_HasItem = true;
		}
	}
	else
		m_InventoryModel->SetMeshDrawState( KEY1, false );

	if( GameSystems::GetInstance()->HasItem(6016) )
	{
		m_InventoryModel->SetMeshDrawState( KEY2, true );

		if( m_SelectedItem == 7 )
		{
			m_HasItem = true;
		}
	}
	else
		m_InventoryModel->SetMeshDrawState( KEY2, false );

	if( GameSystems::GetInstance()->HasItem(6017) )
	{
		m_InventoryModel->SetMeshDrawState( KEY3, true );

		if( m_SelectedItem == 8 )
		{
			m_HasItem = true;
		}
	}
	else
		m_InventoryModel->SetMeshDrawState( KEY3, false );

	if( GameSystems::GetInstance()->HasItem(6010) )
	{
		m_InventoryModel->SetMeshDrawState( KEYCARD1, true );

		if( m_SelectedItem == 9 )
		{
			m_HasItem = true;
		}
	}
	else
		m_InventoryModel->SetMeshDrawState( KEYCARD1, false );

	if( GameSystems::GetInstance()->HasItem(6011) )
	{
		m_InventoryModel->SetMeshDrawState( KEYCARD2, true );

		if( m_SelectedItem == 10 )
		{
			m_HasItem = true;
		}
	}
	else
		m_InventoryModel->SetMeshDrawState( KEYCARD2, false );

	if( GameSystems::GetInstance()->HasItem(6012) )
	{
		m_InventoryModel->SetMeshDrawState( KEYCARD3, true );

		if( m_SelectedItem == 11 )
		{
			m_HasItem = true;
		}
	}
	else
		m_InventoryModel->SetMeshDrawState( KEYCARD3, false );

	if( GameSystems::GetInstance()->HasItem(6022) )
	{
		m_InventoryModel->SetMeshDrawState( HELIPART, true );

		if( m_SelectedItem == 12 )
		{
			m_HasItem = true;
		}
	}
	else
		m_InventoryModel->SetMeshDrawState( HELIPART, false );

	if( GameSystems::GetInstance()->HasItem(6021) )
	{
		m_InventoryModel->SetMeshDrawState( CLIPBOARD2, true );

		if( m_SelectedItem == 13 )
		{
			m_HasItem = true;
		}
	}
	else
		m_InventoryModel->SetMeshDrawState( CLIPBOARD2, false );

	if( GameSystems::GetInstance()->HasItem(6002) )
	{
		m_InventoryModel->SetMeshDrawState( HAND, true );

		if( m_SelectedItem == 14 )
		{
			m_HasItem = true;
		}
	}
	else
		m_InventoryModel->SetMeshDrawState( HAND, false );

	if( GameSystems::GetInstance()->HasItem(6024) )
	{
		m_InventoryModel->SetMeshDrawState( NOTE2, true );

		if( m_SelectedItem == 15 )
		{
			m_HasItem = true;
		}
	}
	else
		m_InventoryModel->SetMeshDrawState( NOTE2, false );

	if( GameSystems::GetInstance()->HasItem(6023) )
	{
		m_InventoryModel->SetMeshDrawState( SCREWDRIVER, true );

		if( m_SelectedItem == 16 )
		{
			m_HasItem = true;
		}
	}
	else
		m_InventoryModel->SetMeshDrawState( SCREWDRIVER, false );

	if( GameSystems::GetInstance()->HasItem(6031) )
	{
		m_InventoryModel->SetMeshDrawState( ROPE, true );

		if( m_SelectedItem == 17 )
		{
			m_HasItem = true;
		}
	}
	else
		m_InventoryModel->SetMeshDrawState( ROPE, false );

	if( description1 )
		m_InventoryUI.ChangeElementText( LABEL_DESCRIPTION_LINE1, description1 );
	else
		m_InventoryUI.ChangeElementText( LABEL_DESCRIPTION_LINE1, "Tap an item to use." );

	if( description2 )
		m_InventoryUI.ChangeElementText( LABEL_DESCRIPTION_LINE2, description2 );
	else
		m_InventoryUI.ChangeElementText( LABEL_DESCRIPTION_LINE2, "" );
}
