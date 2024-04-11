
/*===================================================================
	File: HangingSpiderAnimationLookup.cpp
	Game: H4

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"

#include "Enemy/HangingSpiderAnimationLookup.h"

namespace
{
	HangingSpiderAnim::AnimData HangingSpiderAnimations[HangingSpiderAnim::NUM_ANIMS] = 
	{
		// filename											anim id									loop flag
		{ "hga/enemy_spider_hanging_idle_up.hga",			HangingSpiderAnim::IDLE_UP,				true	},	
		{ "hga/enemy_spider_hanging_idle_down.hga",			HangingSpiderAnim::IDLE_DOWN,			true	},	

		{ "hga/enemy_spider_hanging_swing.hga",				HangingSpiderAnim::SWING,				false	},	
		{ "hga/enemy_spider_hanging_climb.hga",				HangingSpiderAnim::CLIMB,				false	},	

		{ "hga/enemy_spider_hanging_drop.hga",				HangingSpiderAnim::DROP,				false	}
	};
}

const HangingSpiderAnim::AnimData& GetHangingSpiderAnimationData( unsigned int index )
{
	if( index < 0 || 
		index >= HangingSpiderAnim::NUM_ANIMS )
		DBG_ASSERT_MSG(0, "Requesting hanging spider animation data outside of range" );

	return( HangingSpiderAnimations[index] );
}
