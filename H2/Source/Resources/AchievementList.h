
#ifndef __ACHIEVEMENTLIST_H__
#define __ACHIEVEMENTLIST_H__

#include <list>
#include "Support/ScoresCommon.h"

namespace res
{
	void CreateAchievementList();

	void ClearAchievementList();

	const char* GetAchievementString( int index );
	
	int GetAchievementIndex( const char* achievementId );


	const char* GetKiipAchievementString( int index );
	
	int GetKiipAchievementIndex( const char* achievementId );

	std::list<support::TAchievement> GetAchievementAsList();
	std::list<support::TAchievement> GetKiipAchievementAsList();
}

#endif // __ACHIEVEMENTLIST_H__

