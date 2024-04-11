
/*===================================================================
	File: HangingSpiderAnimationLookup.h
	Game: H4

	(C)Hidden Games
=====================================================================*/

#ifndef __HANGINGSPIDERANIMATIONLOOKUP_H__
#define __HANGINGSPIDERANIMATIONLOOKUP_H__

namespace HangingSpiderAnim
{
	enum
	{
		IDLE_UP=0,
		IDLE_DOWN,
		SWING,
		CLIMB,
		DROP,

		NUM_ANIMS
	};

	struct AnimData
	{
		const char*	szFile;
		unsigned int animId;
		bool loopFlag;
	};
}

const HangingSpiderAnim::AnimData& GetHangingSpiderAnimationData( unsigned int index );

#endif // __HANGINGSPIDERANIMATIONLOOKUP_H__

