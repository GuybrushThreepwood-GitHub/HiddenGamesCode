
/*===================================================================
	File: ScriptedPlayerAnimationLookup.cpp
	Game: H4

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"

#include "Player/Scripted/ScriptedPlayerAnimationLookup.h"

namespace
{
	ScriptedPlayerAnim::AnimData PlayerAnimations[ScriptedPlayerAnim::NUM_ANIMS] = 
	{
		// filename									anim id								loop flag
		{ "hga/pl_scene_idle-nogun.hga",			ScriptedPlayerAnim::IDLE,			true	},
		{ "hga/pl_scene_walk.hga",					ScriptedPlayerAnim::WALK,			true	},
		{ "hga/pl_scene_run.hga",					ScriptedPlayerAnim::RUN,			true	},
	};
}

ScriptedPlayerAnim::AnimData& GetScriptedPlayerAnimationData( unsigned int index )
{
	if( index < 0 || 
		index >= ScriptedPlayerAnim::NUM_ANIMS )
		DBG_ASSERT_MSG( 0, "Player AnimData request is out of range [%d]", index );

	return( PlayerAnimations[index] );
}
