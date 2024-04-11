
#ifndef __ACHIEVEMENTLIST_H__
#define __ACHIEVEMENTLIST_H__

#include <list>
#include "Support/ScoresCommon.h"

namespace res
{
	void CreateAchievementList();

	void ClearAchievementList();

	const char* GetAchievementString( int index );
	
	int GetAchievementIndex( const char* purchaseId );
	
	std::list<support::TAchievement> GetAchievementAsList();
}

#endif // __ACHIEVEMENTLIST_H__

