
/*===================================================================
	File: H4Access.h
	Game: H4

	(C)Hidden Games
=====================================================================*/

#ifndef __H4ACCESS_H__
#define __H4ACCESS_H__

#include "ScriptBase.h"

// forward declare
class ScriptDataHolder;

struct GameData
{
	float ACHIEVEMENT_ICON_MAXX;
	float ACHIEVEMENT_ICON_SPEED;
	float ACHIEVEMENT_ICON_SHOWTIME;
};

void RegisterH4Functions( ScriptDataHolder& dataHolder );

int ScriptSetGameData( lua_State* pState );

#endif // __H4ACCESS_H__
