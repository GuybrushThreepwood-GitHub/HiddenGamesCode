
/*===================================================================
	File: PlayerAnimationLookup.h
	Game: H4

	(C)Hidden Games
=====================================================================*/

#ifndef __PLAYERANIMATIONLOOKUP_H__
#define __PLAYERANIMATIONLOOKUP_H__

namespace PlayerAnim
{
	enum
	{
		IDLE=0,
		WALK,
		RUN,

		ACTION_IDLE,

		AIM_PISTOL,
		AIM_SHOTGUN,

		RECOIL_PISTOL,
		RECOIL_SHOTGUN,

		RELOAD_PISTOL,
		RELOAD_SHOTGUN,

		NUM_ANIMS
	};

	struct AnimData
	{
		const char*	szFile;
		unsigned int animId;
		bool loopFlag;
	};
}

PlayerAnim::AnimData& GetPlayerAnimationData( unsigned int index );

#endif // __PLAYERANIMATIONLOOKUP_H__

