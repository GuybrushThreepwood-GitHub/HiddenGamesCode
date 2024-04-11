
/*===================================================================
	File: CustomerAnimationLookup.cpp
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"

#include "Customers/CustomerAnimationLookup.h"

namespace
{
	CustomerAnim::AnimData CustomerAnimations[CustomerAnim::NUM_ANIMS] = 
	{
		// filename															anim id									loop flag
		{ "assets/models/customers/anim_idle.hga",							CustomerAnim::IDLE,						true	},
		{ "assets/models/customers/anim_waving_1handed.hga",				CustomerAnim::WAVING1,					true	},
		{ "assets/models/customers/anim_waving_2handed.hga",				CustomerAnim::WAVING2,					true	},	
		{ "assets/models/customers/anim_idle_parcel.hga",					CustomerAnim::IDLE_WITHPARCEL,			true	},
		{ "assets/models/customers/anim_waving_parcel.hga",					CustomerAnim::WAVING_WITHPARCEL,		true	},
		//{ "assets/models/customers/customers_idle_to_asleep.hga",			CustomerAnim::IDLE_TO_ASLEEP,			false	},
		//{ "assets/models/customers/customers_idle_to_checkswatch.hga",	CustomerAnim::IDLE_TO_CHECKSWATCH,		false	},
		{ "assets/models/customers/anim_walk.hga",							CustomerAnim::WALK,						true	},
		//{ "assets/models/customers/anim_walk_fat.hga",					CustomerAnim::WALK_FAT,					true	},
		{ "assets/models/customers/anim_walk_parcel.hga",					CustomerAnim::WALK_WITHPARCEL,			true	},
		//{ "assets/models/customers/customers_run.hga",					CustomerAnim::RUN,						true	},
		//{ "assets/models/customers/customers_run_fat.hga",				CustomerAnim::RUN_FAT,					true	},
		//{ "assets/models/customers/customers_run_withparcel.hga",			CustomerAnim::RUN_WITHPARCEL,			true	},
		//{ "assets/models/customers/customers_checkswatch.hga",			CustomerAnim::CHECKSWATCH,				false	},
		//{ "assets/models/customers/customers_asleep.hga",					CustomerAnim::ASLEEP,					true	},
	};
}

CustomerAnim::AnimData& GetCustomerAnimationData( unsigned int index )
{
	if( /*index < 0 || */
		index >= CustomerAnim::NUM_ANIMS )
		DBG_ASSERT(0);

	return( CustomerAnimations[index] );
}
