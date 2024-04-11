
/*===================================================================
	File: WorldAccess.cpp
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

#include "Effects/Emitter.h"
#include "Resources/StringResources.h"
#include "ScriptAccess/ScriptDataHolder.h"

#include "GameSystems.h"

#include "ScriptAccess/ScriptDataHolder.h"
#include "ScriptAccess/H4/H4Access.h"

namespace h4Script
{
	ScriptDataHolder* pScriptH4Data = 0;
}

/////////////////////////////////////////////////////
/// Function: RegisterH4Functions
/// Params: [in]dataHolder
///
/////////////////////////////////////////////////////
void RegisterH4Functions( ScriptDataHolder& dataHolder )
{
	h4Script::pScriptH4Data = &dataHolder;

	script::LuaScripting::GetInstance()->RegisterFunction( "SetGameData", ScriptSetGameData );
}

/////////////////////////////////////////////////////
/// Function: ScriptSetGameData
/// Params: [in]pState
///
/////////////////////////////////////////////////////
int ScriptSetGameData( lua_State* pState )
{
	if( lua_istable( pState, 1 ) )
	{
		GameData gameData = h4Script::pScriptH4Data->GetGameData();

		gameData.ACHIEVEMENT_ICON_MAXX	= static_cast<float>(script::LuaGetNumberFromTableItem( "ACHIEVEMENT_ICON_MAXX", 1 ));
		gameData.ACHIEVEMENT_ICON_SPEED	= static_cast<float>(script::LuaGetNumberFromTableItem( "ACHIEVEMENT_ICON_SPEED", 1 ));
		gameData.ACHIEVEMENT_ICON_SHOWTIME	= static_cast<float>(script::LuaGetNumberFromTableItem( "ACHIEVEMENT_ICON_SHOWTIME", 1 ));

		h4Script::pScriptH4Data->SetGameData( gameData );
	}

	return(0);
}
