
/*===================================================================
 File: main.cpp
 Game: H8
 
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
#include "H8.h"
#include "H8Consts.h"

#include "resource.h"

const bool APP_FULLSCREEN = false;

MAINFUNC
{
	H8 theApp;

	core::app::SetIconID(IDI_ICON1);
	core::app::SetFrameLock( core::FPS60 );

	core::app::SetAppRateURL(""); // http://itunes.apple.com/us/app/firewall/id603552348?mt=8

	theApp.Run(GAME_WIDTH, GAME_HEIGHT, APP_FULLSCREEN);
	return 0;
}

#endif // BASE_PLATFORM_WINDOWS