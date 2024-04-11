
/*===================================================================
	File: CustomerAnimationLookup.h
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#ifndef __CUSTOMERANIMATIONLOOKUP_H__
#define __CUSTOMERANIMATIONLOOKUP_H__

namespace CustomerAnim
{
	enum
	{
		IDLE=0,
		WAVING1,
		WAVING2,

		IDLE_WITHPARCEL,
		WAVING_WITHPARCEL,
		//IDLE_TO_ASLEEP,
		//IDLE_TO_CHECKSWATCH,
		WALK,
		//WALK_FAT,
		WALK_WITHPARCEL,
		//RUN,
		//RUN_FAT,
		//RUN_WITHPARCEL,
		//CHECKSWATCH,	
		//ASLEEP,


		NUM_ANIMS
	};

	struct AnimData
	{
		const char*	szFile;
		unsigned int animId;
		bool loopFlag;
	};
}

CustomerAnim::AnimData& GetCustomerAnimationData( unsigned int index );

#endif // __CUSTOMERANIMATIONLOOKUP_H__

