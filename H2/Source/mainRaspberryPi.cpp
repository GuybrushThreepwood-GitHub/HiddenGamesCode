
/*===================================================================
 File: mainRaspberryPi.cpp
 Game: Cabby
 
 (C)Hidden Games
 =====================================================================*/

#ifdef BASE_PLATFORM_RASPBERRYPI

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"

#include "Input/InputInclude.h"
#include "Input/Input.h"

#include "StateManage/IState.h"
#include "StateManage/StateManager.h"
#include "Cabby.h"
#include "CabbyConsts.h"

const bool APP_FULLSCREEN = false;

MAINFUNC
{
	Cabby theApp;

	core::app::SetFrameLock( core::FPS30 );

#ifdef CABBY_LITE
	// lite
	core::app::SetAppRateURL(""); // http://itunes.apple.com/us/app/cabby-lite/id339386175?mt=8
	
#elif CABBY_FREE
	// free
	core::app::SetAppRateURL(""); // http://itunes.apple.com/us/app/cabby-free/id563600742?mt=8
	
#else
	// full
	//core::app::SetAppRateURL(""); // http://itunes.apple.com/us/app/cabby/id339383913?mt=8
	
	// kiip support
	//core::app::SetKiipKeys("", "");
	
#endif


	theApp.Run(GAME_WIDTH, GAME_HEIGHT, APP_FULLSCREEN);
	return 0;
}

#endif // BASE_PLATFORM_RASPBERRYPI
