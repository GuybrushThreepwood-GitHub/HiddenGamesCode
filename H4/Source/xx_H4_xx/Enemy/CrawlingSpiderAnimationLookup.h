
/*===================================================================
	File: CrawlingSpiderAnimationLookup.h
	Game: H4

	(C)Hidden Games
=====================================================================*/

#ifndef __CRAWLINGSPIDERANIMATIONLOOKUP_H__
#define __CRAWLINGSPIDERANIMATIONLOOKUP_H__

namespace CrawlingSpiderAnim
{
	enum
	{
		IDLE=0,
		WALK,

		NUM_ANIMS
	};

	struct AnimData
	{
		const char*	szFile;
		unsigned int animId;
		bool loopFlag;
	};
}

const CrawlingSpiderAnim::AnimData& GetCrawlingSpiderAnimationData( unsigned int index );

#endif // __CRAWLINGSPIDERANIMATIONLOOKUP_H__

