
/*===================================================================
	File: ScriptedPlayerAnimationLookup.h
	Game: H4

	(C)Hidden Games
=====================================================================*/

#ifndef __SCRIPTEDPLAYERANIMATIONLOOKUP_H__
#define __SCRIPTEDPLAYERANIMATIONLOOKUP_H__

namespace ScriptedPlayerAnim
{
	enum
	{
		IDLE=0,
		WALK,
		RUN,

		NUM_ANIMS
	};

	struct AnimData
	{
		const char*	szFile;
		unsigned int animId;
		bool loopFlag;
	};
}

ScriptedPlayerAnim::AnimData& GetScriptedPlayerAnimationData( unsigned int index );

#endif // __SCRIPTEDPLAYERANIMATIONLOOKUP_H__

