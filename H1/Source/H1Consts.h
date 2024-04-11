
/*===================================================================
	File: H1Consts.h
	Game: AirCadets

	(C)Hidden Games
=====================================================================*/

#ifndef __H1CONSTS_H__
#define __H1CONSTS_H__

	// windows can do the view as it should be (in a window)
	const float ANIMATION_RATE = 1.0f/30.0f;

	const int GAME_WIDTH	=	480;
	const int GAME_HEIGHT	=	320;
	const float GAME_FOV	=	54.15f; // (iphone fov) - 20.85 ?

	const float NEAR_CLIP	= 1.0f;
	const float FAR_CLIP	= 100.0f;

	const float NEAR_CLIP_ORTHO	= -6.0f;
	const float FAR_CLIP_ORTHO	= 6.0f;

	const float TOUCH_SIZE_MENU	= 8.0f;
	const float TOUCH_SIZE_GAME	= 8.0f;

	const float TILT_CONTROL_THRESHOLD = 0.085f;
	const float ACCELEROMETER_FREQ = 30.0f;

	const float PHYSICS_TIMESTEP	= 1.0f / 60.0f;
	const int PHYSICS_ITERATIONS	= 10;

	const int STAR_TOKENS				= 5;
	const int HIDDEN_TOKENS				= 5;

	const int TOTAL_CADETS				= 6;
	const int TOTAL_TOKENS_PER_CADET	= 10;

	const int MAX_CONTACTS		= 5;

#endif // __H1CONSTS_H__
