
/*===================================================================
	File: mainRaspberryPi.cpp
	Game: H4

	(C)Hidden Games
=====================================================================*/

#ifdef BASE_PLATFORM_RASPBERRYPI

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

const bool APP_FULLSCREEN = false;

MAINFUNC
{
	H4 theApp;

	core::app::SetAppRateURL("itms-apps://ax.itunes.apple.com/WebObjects/MZStore.woa/wa/viewContentsUserReviews?type=Purple+Software&id=380980096"); // http://itunes.apple.com/us/app/prisoner84/id380980096?mt=8

	core::app::SetFlurryAPIKey( "3RVFMQXZZ8Q4QBPT75VX" );

	// run the application
	theApp.Run(GAME_WIDTH, GAME_HEIGHT, APP_FULLSCREEN);

	return 0;
}

#endif // BASE_PLATFORM_RASPBERRYPI