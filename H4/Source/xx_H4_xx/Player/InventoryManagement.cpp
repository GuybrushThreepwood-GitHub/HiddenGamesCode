
/*===================================================================
	File: InventoryManagement.cpp
	Game: H4

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "ScriptBase.h"

#include "GameSystems.h"
#include "Player/InventoryManagement.h"

namespace
{
	// taken from lua/resources/model_resources.lua
	const int PISTOL_IDX = 6001;
	const int SHOTGUN_IDX = 6004;
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
InventoryManagement::InventoryManagement( )
{

}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
InventoryManagement::~InventoryManagement( )
{
	m_ItemList.clear();
}

/////////////////////////////////////////////////////
/// Method: AddItem
/// Params:
///
/////////////////////////////////////////////////////
void InventoryManagement::AddItem( int index )
{
	std::vector<int>::iterator it;

	it = m_ItemList.begin();

	while( it != m_ItemList.end() )
	{
		if( (*it) == index )
		{
			DBGLOG( "*WARNING* AddItem called with %d, item is already there\n", index );
			return;
		}

		it++;
	}

	// if the pistol/shotgun is added, set the player flags
	if( index == PISTOL_IDX )
	{
		//GameSystems::GetInstance()->GetPlayer().SetWeaponIndex(0);
		GameSystems::GetInstance()->GetPlayer().SetHasPistol( true );
	}
	else if( index == SHOTGUN_IDX )
	{
		//GameSystems::GetInstance()->GetPlayer().SetWeaponIndex(1);
		GameSystems::GetInstance()->GetPlayer().SetHasShotgun( true );
	}

	m_ItemList.push_back(index);
}

/////////////////////////////////////////////////////
/// Method: HasItem
/// Params:
///
/////////////////////////////////////////////////////
bool InventoryManagement::HasItem( int index )
{
	std::vector<int>::iterator it;

	it = m_ItemList.begin();

	while( it != m_ItemList.end() )
	{
		if( (*it) == index )
		{
			return(true);
		}

		it++;
	}

	return(false);
}
	
/////////////////////////////////////////////////////
/// Method: RemoveItem
/// Params:
///
/////////////////////////////////////////////////////
void InventoryManagement::RemoveItem( int index )
{

}

