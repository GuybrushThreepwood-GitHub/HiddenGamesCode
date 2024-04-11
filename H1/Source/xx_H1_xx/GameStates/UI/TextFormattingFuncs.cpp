
/*===================================================================
	File: TextFormattingFuncs.cpp
	Game: AirCadets

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "SoundBase.h"
#include "CollisionBase.h"
#include "Input/Input.h"
#include "ModelBase.h"
#include "ScriptBase.h"

#include "H1Consts.h"
#include "H1.h"

#include "GameStates/UI/UIFileLoader.h"
#include "GameStates/UI/TextFormattingFuncs.h"

namespace
{
	math::Vec3 vaPoints[4];
	math::Vec4Lite vaColours[4];
}

/////////////////////////////////////////////////////
/// Function: SetTime
/// Params:
///
/////////////////////////////////////////////////////
void SetTime( UIFileLoader& uiFile, int elementId, float timer )
{
	char text[UI_MAXSTATICTEXTBUFFER_SIZE];

	int minAsInt = static_cast<int>(timer / 60.0f);
	int secsAsInt = static_cast<int>(timer) % 60;
	int milliSecsAsInt = static_cast<int>( (timer-(static_cast<float>(minAsInt)*60.0f)-static_cast<float>(secsAsInt)) * 100.0f );

	if( minAsInt > 99 )
		minAsInt = 99;

	if( secsAsInt < 10 )
	{
		if( milliSecsAsInt < 10 )
			snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "%d:0%d.0%d", minAsInt, secsAsInt, milliSecsAsInt );
		else
			snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "%d:0%d.%d", minAsInt, secsAsInt, milliSecsAsInt );
	}
	else
	{
		if( milliSecsAsInt < 10 )
			snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "%d:%d.0%d", minAsInt, secsAsInt, milliSecsAsInt );
		else
			snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "%d:%d.%d", minAsInt, secsAsInt, milliSecsAsInt );
	}

	uiFile.ChangeElementText( elementId, "%s", text );

}
