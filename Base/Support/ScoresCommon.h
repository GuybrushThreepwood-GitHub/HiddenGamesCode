
#ifndef __SCORESCOMMON_H__
#define __SCORESCOMMON_H__

const int MAX_ACHIEVEMENT_ID_LEN = 256;

namespace support
{	
	class ScoresCallback
	{
		public:
			ScoresCallback() {}
			virtual ~ScoresCallback() {}

			virtual void LoggedIn() = 0;
			virtual int GetAchievementId( const char* achievementString ) = 0;
			virtual const char* GetAchievementString( int achievementId ) = 0;

#ifdef BASE_SUPPORT_KIIP
			virtual int GetKiipAchievementId( const char* achievementString ) = 0;
			virtual const char* GetKiipAchievementString( int achievementId ) = 0;
#endif // BASE_SUPPORT_KIIP
		
			virtual void AchievementSubmitted( int achievementId, int errorId ) = 0;

		private:
	};

    enum EScoreOrder
    {
        SCORE_HIGHER_IS_BETTER=0,
        SCORE_LOWER_IS_BETTER
    };            
    
	enum EScoreType
    {
		SCORE_TYPE_POINTS,
		SCORE_TYPE_TIME
	};
	
	struct TLeaderboardScore
	{
		EScoreType type;
		
		int position;
		char leaderboardId[core::MAX_PATH];
		unsigned long long int points;
		float time;
	};

	struct TAchievement
	{
		int achievementId;
		char achievementString[MAX_ACHIEVEMENT_ID_LEN];
		float percentage;
	};
	
} // namespace support

#endif // __SCORESCOMMON_H__

