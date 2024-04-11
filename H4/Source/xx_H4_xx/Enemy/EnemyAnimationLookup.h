
/*===================================================================
	File: EnemyAnimationLookup.h
	Game: H4

	(C)Hidden Games
=====================================================================*/

#ifndef __ENEMYANIMATIONLOOKUP_H__
#define __ENEMYANIMATIONLOOKUP_H__

namespace EnemyAnim
{
	enum
	{
		IDLE=0,
		IDLE_ALT,

		FLINCH_LEFT,
		FLINCH_RIGHT,

		/*WALK,
		WALK_RAISED_ARMS,

		WALK_ALT,
		WALK_ALT_RAISED_ARMS,*/

		RUN,
		/*RUN_SLOW,*/

		ATTACK_HEADBUTT,
		ATTACK_LEFT,
		ATTACK_RIGHT,

		DIE,

		NUM_ANIMS
	};

	struct AnimData
	{
		const char*	szFile;
		unsigned int animId;
		bool loopFlag;
	};
}

const EnemyAnim::AnimData& GetEnemyAnimationData( unsigned int index );

#endif // __ENEMYANIMATIONLOOKUP_H__

