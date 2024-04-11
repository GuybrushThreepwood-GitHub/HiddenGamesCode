
/*===================================================================
	File: H8Consts.h
	Game: H8

	(C)Hidden Games
=====================================================================*/

#ifndef __H8CONSTS_H__
#define __H8CONSTS_H__

	// windows can do the view as it should be (in a window)
	const float ANIMATION_RATE = 1.0f/30.0f;

	const int GAME_WIDTH	=	480;
	const int GAME_HEIGHT	=	320;
	const float GAME_FOV	=	54.15f; // (iphone fov) - 20.85 ?

	const float NEAR_CLIP	= 1.0f;
	const float FAR_CLIP	= 100.0f;

	const float NEAR_CLIP_ORTHO	= -10000.0f;
	const float FAR_CLIP_ORTHO	= 10000.0f;

	const float TOUCH_SIZE_MENU	= 8.0f;
	const float TOUCH_SIZE_GAME	= 8.0f;

	const float TILT_CONTROL_THRESHOLD	= 0.085f;
	const float ACCELEROMETER_FREQ		= 30.0f;

	const float PHYSICS_TIMESTEP		= 1.0f / 60.0f;
	const int PHYSICS_VEL_ITERATIONS	= 8;
	const int PHYSICS_POS_ITERATIONS	= 3;

	const int MAX_ENEMIES			= 48;
	const float SPAWN_RADIUS		= 32.0f;

	const int MAX_COLOURS			= 16;

#endif // __H8CONSTS_H__
