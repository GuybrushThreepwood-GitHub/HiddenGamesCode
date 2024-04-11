
#ifndef __WINSCORES_H__
#define __WINSCORES_H__

#ifdef BASE_PLATFORM_WINDOWS

#include <vector>

#include "Core/CoreDefines.h"
#include "Core/CoreConsts.h"

#include "Support/ScoresCommon.h"

namespace support
{
    namespace win 
    { 
        class WinScores
        {
			private:
				enum EAchievementList
				{
					COMPLETE_LIST=0
				};

            public:
                /// default constructor
                WinScores( support::ScoresCallback* callback );
                /// default destructor
                ~WinScores(); 

                void Login();
            
				void LoadLeaderboardFile( const char* filename );
				void AppendLeaderboards( const char* functionCall );
			
				//	sync score
				void SendScore( unsigned long long int score, const char* leaderboardString );
				void SyncScore( support::EScoreOrder order, const char* leaderboardString );
				
				// achievements
				void SyncAchievements();
                void ResetAchievements();
            
				// score
                void ShowLeaderboard( const char* leaderboardString );
			
				// achievements
                void ShowAchievements();     
				void AchievementReloaded( float percentageComplete, const char* achievementString ); 
				void AwardAchievement( float percentageComplete, int achievementId ); 
				bool IsNewAchievement( int achievementId );

				void AwardKiipAchievement( float percentageComplete, int achievementId ); 

				// callback
				void SetCallback( support::ScoresCallback* pCallback );

				// should be called by server when player is authenticated
				void SetLoggedIn();
				bool IsLoggedIn()		{ return m_IsLoggedIn; }
			
				// should be called by server when a new score is recieved
				void ScoreRecieved( const char* leaderboard, __int64 value );
				// should be called by server when an achievement was successful or failed
				void AchievementSubmitted( const char* achievementId, int errorId );

			private:
				int LoadAchievementsComplete();
                void SaveCompleteAchievements();

				bool DoesExistInList( EAchievementList whichList, int achievementId );
				void RemoveFromList( EAchievementList whichList, int achievementId );
				support::TAchievement* GetItemInList( EAchievementList whichList, int achievementId );

			private:
				bool m_IsLoggedIn;

				file::TFileHandle m_LocalFileHandle;

				support::ScoresCallback* m_Callback;

				char m_CompleteAchievementsFile[core::MAX_PATH+core::MAX_PATH];

				std::vector<TAchievement> m_CompleteAchievements;
        };
        
    } // namespace win
    
} // namespace support
    
#endif // BASE_PLATFORM_WINDOWS


#endif // __WINSCORES_H__

