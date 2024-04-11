
/*===================================================================
	File: CrawlingSpiderAnimationLookup.cpp
	Game: H4

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"

#include "Enemy/CrawlingSpiderAnimationLookup.h"

namespace
{
	CrawlingSpiderAnim::AnimData CrawlingSpiderAnimations[CrawlingSpiderAnim::NUM_ANIMS] = 
	{
		// filename										anim id								loop flag
		{ "hga/enemy_spider_crawling_idle.hga",			CrawlingSpiderAnim::IDLE,			true	},	
		{ "hga/enemy_spider_crawling_walk.hga",			CrawlingSpiderAnim::WALK,			true	},	
	};
}

const CrawlingSpiderAnim::AnimData& GetCrawlingSpiderAnimationData( unsigned int index )
{
	if( index < 0 || 
		index >= CrawlingSpiderAnim::NUM_ANIMS )
		DBG_ASSERT_MSG(0, "Requesting crawling spider animation data outside of range" );

	return( CrawlingSpiderAnimations[index] );
}
