
/*===================================================================
	File: EnemyAnimationLookup.cpp
	Game: H4

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"

#include "Enemy/EnemyAnimationLookup.h"

namespace
{
	EnemyAnim::AnimData EnemyAnimations[EnemyAnim::NUM_ANIMS] = 
	{
		// filename											anim id									loop flag
		{ "hga/npc_game-idle.hga",						EnemyAnim::IDLE,						true	},	
		{ "hga/npc_game-idle-alt.hga",					EnemyAnim::IDLE_ALT,					true	},	

		{ "hga/npc_game-flinch-left.hga",				EnemyAnim::FLINCH_LEFT,					false	},	
		{ "hga/npc_game-flinch-right.hga",				EnemyAnim::FLINCH_RIGHT,				false	},	

		/*{ "hga/npc_game-walk-normal.hga",				EnemyAnim::WALK,						true	},
		{ "hga/npc_game-walk-raisedarms.hga",			EnemyAnim::WALK_RAISED_ARMS,			true	},

		{ "hga/npc_game-walk-alt.hga",					EnemyAnim::WALK_ALT,					true	},
		{ "hga/npc_game-walk-alt-raisedarms.hga",		EnemyAnim::WALK_ALT_RAISED_ARMS,		true	},*/

		{ "hga/npc_game_run.hga",						EnemyAnim::RUN,							true	},
		/*{ "hga/npc_game_run_slow.hga",				EnemyAnim::RUN_SLOW,					true	},*/

		{ "hga/npc_game-attack-headbutt.hga",			EnemyAnim::ATTACK_HEADBUTT,				false	},
		{ "hga/npc_game-attack-left.hga",				EnemyAnim::ATTACK_LEFT,					false	},
		{ "hga/npc_game-attack-right.hga",				EnemyAnim::ATTACK_RIGHT,				false	},

		{ "hga/npc_die.hga",							EnemyAnim::DIE,							false	},
	};
}

const EnemyAnim::AnimData& GetEnemyAnimationData( unsigned int index )
{
	if( index < 0 || 
		index >= EnemyAnim::NUM_ANIMS )
		DBG_ASSERT_MSG(0, "Requesting enemy animation data outside of range" );

	return( EnemyAnimations[index] );
}
