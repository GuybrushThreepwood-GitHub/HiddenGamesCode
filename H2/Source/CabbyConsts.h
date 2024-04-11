
/*===================================================================
	File: CabbyConsts.h
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#ifndef _CABBYCONSTS_H__
#define _CABBYCONSTS_H__

	const float NEAR_CLIP	= 1.0f;
	const float FAR_CLIP	= 100.0f;
	const float NEAR_CLIP_ORTHO	= -6.0f;
	const float FAR_CLIP_ORTHO	= 6.0f;
	const float TOUCH_SIZE_MENU	= 8.0f;
	const float TOUCH_SIZE_GAME	= 32.0f;

	const float ANIMATION_RATE = 1.0f/30.0f;

	const int GAME_WIDTH	=	480;
	const int GAME_HEIGHT	=	320;
	const float GAME_FOV	=	54.15f; // (iphone fov) - 20.85 ?

#if defined(BASE_PLATFORM_WINDOWS) || defined(BASE_PLATFORM_iOS) || defined(BASE_PLATFORM_RASPBERRYPI) || defined(BASE_PLATFORM_MAC)
	const float TILT_CONTROL_THRESHOLD = 0.085f;
#else // android
	const float TILT_CONTROL_THRESHOLD = 0.20f; // android value is normalised
#endif //

	const float ACCELEROMETER_FREQ = 30.0f;

	const float PHYSICS_TIMESTEP = 1.0f / 60.0f;
	const int PHYSICS_ITERATIONS = 10;

	const int STAR_REWARDS = 5;
	const int LEVELS_IN_A_CAREER = 10;

#endif // _CABBYCONSTS_H__
