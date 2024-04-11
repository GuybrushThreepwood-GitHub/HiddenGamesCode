
#ifndef __LEADERBOARDS_H__
#define __LEADERBOARDS_H__

#include "CoreBase.h"
#include "Support/ScoresCommon.h"

const int MAX_LEADERBOARD_NAME = 32;

namespace support
{
	// leaderboard
	struct LeaderboardStore
	{
		char name[MAX_LEADERBOARD_NAME];
		support::EScoreType type;
		support::EScoreOrder order;
	};
	
	void AppendLeaderboards( const char* functionCall );
	
	void ClearLeaderboards();

	const support::LeaderboardStore* GetLeaderboard( const char* name );
}

#endif // __LEADERBOARDS_H__

