

/*===================================================================
	File: TextFormattingFuncs.h
	Game: H8

	(C)Hidden Games
=====================================================================*/

#ifndef __TEXTFORMATTINGFUNCS_H__
#define __TEXTFORMATTINGFUNCS_H__

// forward declare
#include "GameStates/UI/UIBaseState.h"
#include "GameStates/IBaseGameState.h"

#include "GameStates/UI/UIFileLoader.h"

void SetTime( UIFileLoader& uiFile, int elementId, float timer );
void SetScore( UIFileLoader& uiFile, int elementId, unsigned long long int score );
void DrawDownloadBar( const math::Vec3& pos, const math::Vec2& dims, float currentTime, float maxTime, UIFileLoader& uiFile, int elementId );
const char* FormatScoreToHTML( unsigned long long int score );

#endif // __TEXTFORMATTINGFUNCS_H__

