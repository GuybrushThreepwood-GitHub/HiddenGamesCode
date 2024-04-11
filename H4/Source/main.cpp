
/*===================================================================
	File: main.cpp
	Game: H4

	(C)Hidden Games
=====================================================================*/

#ifdef BASE_PLATFORM_WINDOWS

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "CollisionBase.h"
#include "ModelBase.h"

#include "Input/InputInclude.h"
#include "Input/Input.h"

#include "ScriptAccess/ScriptAccess.h"

#include "H4.h"
#include "AppConsts.h"

#include "resource.h"

const bool APP_FULLSCREEN = false;

MAINFUNC
{
	H4 theApp;
	core::app::SetIconID(IDI_ICON1);

	core::app::SetAppRateURL(""); // http://itunes.apple.com/us/app/prisoner84/id380980096?mt=8

	//core::app::SetFlurryAPIKey( "" );

	// run the application
	theApp.Run(GAME_WIDTH, GAME_HEIGHT, APP_FULLSCREEN);

	return 0;
}

#endif // BASE_PLATFORM_WINDOWS