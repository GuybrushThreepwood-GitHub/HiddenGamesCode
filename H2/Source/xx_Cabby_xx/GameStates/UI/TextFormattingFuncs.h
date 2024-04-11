

/*===================================================================
	File: TextFormattingFuncs.h
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#ifndef __TEXTFORMATTINGFUNCS_H__
#define __TEXTFORMATTINGFUNCS_H__

// forward declare
#include "GameStates/UI/UIBaseState.h"
#include "GameStates/IBaseGameState.h"

#include "GameStates/UI/UIFileLoader.h"

void SetCashStringNoSymbol( UIFileLoader& uiFile, int elementId, int count );

void SetCashString( UIFileLoader& uiFile, int elementId, int count );

void SetLivesString( UIFileLoader& uiFile, int elementId, int count );

void SetLevelString( UIFileLoader& uiFile, int elementId, int major, int minor );

void DrawFuelGauge( const math::Vec3& pos, const math::Vec2& dims, float fuelLevel, float low, float medium, bool isRefueling );

#endif // __TEXTFORMATTINGFUNCS_H__

