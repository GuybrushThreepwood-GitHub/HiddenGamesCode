
/*===================================================================
 File: main.cpp
 Game: AirCadets
 
 (C)Hidden Games
 =====================================================================*/

#ifdef BASE_PLATFORM_WINDOWS

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"

#include "Input/InputInclude.h"
#include "Input/Input.h"

#include "StateManage/IState.h"
#include "StateManage/StateManager.h"
#include "H1.h"
#include "H1Consts.h"

#include "resource.h"

const bool APP_FULLSCREEN = false;

MAINFUNC
{
	H1 theApp;

	core::app::SetIconID(IDI_ICON1);
	core::app::SetFrameLock( core::FPS60 );

	core::app::SetAppRateURL(""); // http://itunes.apple.com/us/app/air-cadets/id298873144?mt=8

	theApp.Run(GAME_WIDTH, GAME_HEIGHT, APP_FULLSCREEN);
	return 0;
}

#endif // BASE_PLATFORM_WINDOWS