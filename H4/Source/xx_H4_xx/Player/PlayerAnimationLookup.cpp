
/*===================================================================
	File: PlayerAnimationLookup.cpp
	Game: H4

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"

#include "Player/PlayerAnimationLookup.h"

namespace
{
	PlayerAnim::AnimData PlayerAnimations[PlayerAnim::NUM_ANIMS] = 
	{
		// filename									anim id						loop flag
		{ "hga/pl_game_idle.hga",					PlayerAnim::IDLE,			true	},
		{ "hga/pl_game_walk.hga",					PlayerAnim::WALK,			true	},
		{ "hga/pl_game_run.hga",					PlayerAnim::RUN,			true	},

		{ "hga/pl_game_actionidle.hga",				PlayerAnim::ACTION_IDLE,	false	},

		{ "hga/pl_game_blend-aim-pistol.hga",		PlayerAnim::AIM_PISTOL,		true	},		
		{ "hga/pl_game_blend-aim-shotgun.hga",		PlayerAnim::AIM_SHOTGUN,	true	},	

		{ "hga/pl_game_blend-recoil-pistol.hga",	PlayerAnim::RECOIL_PISTOL,	false	},		
		{ "hga/pl_game_blend-recoil-shotgun.hga",	PlayerAnim::RECOIL_SHOTGUN,	false	},

		{ "hga/pl_game_blend-reload-pistol.hga",	PlayerAnim::RELOAD_PISTOL,	false	},		
		{ "hga/pl_game_blend-reload-shotgun.hga",	PlayerAnim::RELOAD_SHOTGUN,	false	},
	};
}

PlayerAnim::AnimData& GetPlayerAnimationData( unsigned int index )
{
	if( index < 0 || 
		index >= PlayerAnim::NUM_ANIMS )
		DBG_ASSERT_MSG( 0, "Player AnimData request is out of range [%d]", index );

	return( PlayerAnimations[index] );
}
