
/*===================================================================
	File: AppConsts.h
	Game: H4

	(C)Hidden Games
=====================================================================*/

#ifndef __APPCONSTS_H__
#define __APPCONSTS_H__

	// windows can do the view as it should be (in a window)
	const float kGameFrameIntervalNormal	= 1.0f/120.0f;
	const float kGameFrameIntervalNormalHi	= 1.0f/240.0f;

	const float kGameFrameIntervalIdle		= 1.0f/10.0f;

	const float kGameFrameStepInterval		= 1.0f/60.0f;
	const float kGameFrameStepIntervalHi	= 1.0f/120.0f;

	const float kGameFrameDrawInterval		= 1.0f/25.0f;
	const float kGameFrameDrawIntervalHi	= 1.0f/60.0f;

	const float kGameFrameMaxDelta			= 1.0f/10.0f;
	const float kGameFrameMaxDeltaHi		= 1.0f/30.0f;

	const int GAME_WIDTH	=	480;
	const int GAME_HEIGHT	=	320;
	const float GAME_FOV	=	30.85f; // (iphone fov 45.0f)

	const float ANIMATION_RATE = 1.0f/25.0f;
	const float ANIMATION_RATE_HI = 1.0f/60.0f;

	const float NEAR_CLIP	= 0.1f;
	const float FAR_CLIP	= 100.0f;

	const int LUAFUNC_STRBUFFER = 32;

#define DEVELOPER_SETUP

#endif // __APPCONSTS_H__
