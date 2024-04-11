
#ifndef __IOSSCORES_H__
#define __IOSSCORES_H__

#ifdef BASE_PLATFORM_iOS

#include <vector>

#include "Core/CoreDefines.h"
#include "Core/CoreConsts.h"
#include "Core/ios/GameViewController.h"

#include "Support/ScoresCommon.h"

#include "GameCenterManager.h"

namespace support
{
    namespace ios 
    {
        class iOSScores
        {	
			private:
				enum EAchievementList
				{
					COMPLETE_LIST=0
				};

            public:
                /// default constructor
                iOSScores( support::ScoresCallback* callback );
                /// default destructor
                ~iOSScores();           
            
                void Login();
            
				void LoadLeaderboardFile( const char* filename );
				void AppendLeaderboards( const char* functionCall );
			
				// sync score
				void SendScore( unsigned long long int score, const char* leaderboardString );
				void SyncScore( support::EScoreOrder order, const char* leaderboardString );
			
				// sync achievements
				void SyncAchievements();
                void ResetAchievements();
            
				// leaderboards
                void ShowLeaderboard( const char* leaderboardString );
			
				// achievements
                void ShowAchievements();
				void AchievementReloaded( float percentageComplete, const char* achievementString ); 
				void AwardAchievement( float percentageComplete, int achievementId ); 
				bool IsNewAchievement( int achievementId );

				void AwardKiipAchievement( float percentageComplete, int achievementId ); 

				// callback
				void SetCallback( support::ScoresCallback* pCallback );

                // ios only
			
				// SetViewController - needs to be set to show the GC pop ups
                void SetViewController( GameViewController* controller )        { m_CurrentViewController = controller; }
                GameViewController* GetViewController()                         { return m_CurrentViewController; }
            
				// should be called by GC when player is authenticated
				void SetLoggedIn();
				bool IsLoggedIn()		{ return m_IsLoggedIn; }
			
				// should be called by GC when a new score is recieved
				void ScoreRecieved( const char* leaderboard, int64_t value );
				// should be called by GC when an achievement was successful or failed
				void AchievementSubmitted( const char* achievementId, int errorId );
								   
            private:				
				void UploadScore( support::EScoreOrder order, unsigned long long int score, const char* leaderboardString ); 
				void UploadTime( support::EScoreOrder order, float time, const char* leaderboardString );
				void SubmitAchievement( float percentageComplete, const char* achievementString );
			
                bool LoadLocalScore( const char* leaderboardString );
            
                void SaveLocalScore( support::EScoreOrder order, unsigned long long int score, const char* leaderboardString );
				void SaveLocalTime( support::EScoreOrder order, float time, const char* leaderboardString );

				int LoadAchievementsComplete();

                void SaveCompleteAchievements();

				bool DoesExistInList( EAchievementList whichList, int achievementId );
				void RemoveFromList( EAchievementList whichList, int achievementId );
				support::TAchievement* GetItemInList( EAchievementList whichList, int achievementId );
            
            private:            
				bool m_GCSupport;
				bool m_IsLoggedIn;

                GameCenterManager* m_GameCenterManager;
                GameViewController* m_CurrentViewController;
            
				support::ScoresCallback* m_Callback;

                // storing local scores for all game leaderboards
                file::TFileHandle m_LocalFileHandle;
				support::TLeaderboardScore m_LocalScore;

				char m_CompleteAchievementsFile[core::MAX_PATH+core::MAX_PATH];

				std::vector<TAchievement> m_CompleteAchievements;
            
        };
        
    } // namespace ios
    
} // namespace support

#endif // BASE_PLATFORM_iOS


#endif // __IOSSCORES_H__

