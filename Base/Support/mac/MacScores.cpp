
/*===================================================================
	File: MacScores.cpp
	Library: Support

	(C)Hidden Games
=====================================================================*/

#ifdef BASE_PLATFORM_MAC

#include "Core/CoreConsts.h"
#include "Core/CoreDefines.h"
#include "Core/FileIO.h"
#include "Core/App.h"
#include "Debug/Assertion.h"

#include "ScriptBase.h"

#include "Support/Leaderboards.h"
#include "MacScores.h"

using namespace support::mac;

/////////////////////////////////////////////////////
/// Default constructor
/// 
///
/////////////////////////////////////////////////////
MacScores::MacScores( support::ScoresCallback* callback )
	: m_Callback(callback)
{
	m_IsLoggedIn = false;
	
	std::memset( &m_CompleteAchievementsFile, 0, sizeof(char)*core::MAX_PATH+core::MAX_PATH );
	snprintf( m_CompleteAchievementsFile, core::MAX_PATH+core::MAX_PATH, "%s/ach_complete.sav", core::app::GetSavePath() );

	m_CompleteAchievements.clear();

	// load files to fill initial lists
	LoadAchievementsComplete();

	// create game center manager   
	m_GCSupport = IsGCAvailable();
	core::app::SetIsGameCenterAvailable(m_GCSupport);
	
    if( m_GCSupport )
    {
		m_GameCenterManager = [[GameCenterManager alloc] init];		
        [m_GameCenterManager setDelegate: m_GameCenterManager];
    }
}

/////////////////////////////////////////////////////
/// Default destructor
/// 
///
/////////////////////////////////////////////////////
MacScores::~MacScores()
{
    if( m_GCSupport )
    {
		if( m_GameCenterManager != nil )
		{
			m_GameCenterManager = nil;
		}
	}
	//m_PendingAchievements.clear();
	m_CompleteAchievements.clear();

	support::ClearLeaderboards();
}

/////////////////////////////////////////////////////
/// Method: LoadLeaderboardFile
/// Params: [in]filename
///
/////////////////////////////////////////////////////
void MacScores::LoadLeaderboardFile( const char* filename )
{
	// just load the script
	script::LoadScript( filename );
}

/////////////////////////////////////////////////////
/// Method: AppendLeaderboards
/// Params: [in]functionCall
///
/////////////////////////////////////////////////////
void MacScores::AppendLeaderboards( const char* functionCall )
{
	support::AppendLeaderboards(functionCall);
}

/////////////////////////////////////////////////////
/// Method: Login
/// Params: None
///
/////////////////////////////////////////////////////
void MacScores::Login()
{
    if( m_GCSupport )
	{
		if( core::app::IsNetworkAvailable() )
			[m_GameCenterManager authenticateLocalUser];
	}
}

/////////////////////////////////////////////////////
/// Method: SendScore
/// Params: [in]score, [in]leaderboardString
///
/////////////////////////////////////////////////////
void MacScores::SendScore( unsigned long long int score, const char* leaderboardString )
{
	DBG_ASSERT( leaderboardString != 0 );
	
	if( m_GCSupport )
    {
        if( leaderboardString )
        {			
            // convert the string to an NSString
            int64_t scoreVal = static_cast<int64_t>(score);
            [m_GameCenterManager submitScore:scoreVal forLeaderboardIdentifier:[NSString stringWithUTF8String:leaderboardString]];
        }
    }
}

/////////////////////////////////////////////////////
/// Method: SyncScore
/// Params: None
///
/////////////////////////////////////////////////////
void MacScores::SyncScore( support::EScoreOrder order, const char* leaderboardString )
{	
	// get the score from GC
	if( m_GCSupport )
		[ m_GameCenterManager reloadHighScoresForLeaderboardIdentifier: [NSString stringWithUTF8String:leaderboardString] ];
}

/////////////////////////////////////////////////////
/// Method: SyncAchievements
/// Params: None
///
/////////////////////////////////////////////////////
void MacScores::SyncAchievements()
{
	if( !core::app::IsSupportingAchievements() )
		return;

	if( m_IsLoggedIn )
	{
		std::vector<support::TAchievement>::iterator it;

		// send all pending achievements if we can
		it = m_CompleteAchievements.begin();
		while( it != m_CompleteAchievements.end() )
		{
			// look up the achievement string and send to server if online, then remove
			const char* achievementString = m_Callback->GetAchievementString( (*it).achievementId );
			DBG_ASSERT( achievementString != 0 );
			if( achievementString != 0 )
			{
				SubmitAchievement( (*it).percentage, achievementString );
			}
			
			// next
			it++;
		}
	}
}

/////////////////////////////////////////////////////
/// Method: ResetAchievements
/// Params: None
///
/////////////////////////////////////////////////////
void MacScores::ResetAchievements()
{
	if( !core::app::IsSupportingAchievements() )
		return;

    if( m_GCSupport )
        [m_GameCenterManager resetAchievements];       

	m_CompleteAchievements.clear();

	SaveCompleteAchievements();
}

/////////////////////////////////////////////////////
/// Method: ShowLeaderboard
/// Params: None
///
/////////////////////////////////////////////////////
void MacScores::ShowLeaderboard( const char* leaderboardString )
{
    if( m_GCSupport )
    {
        if( leaderboardString )
        {
            // convert the string to an NSString
			//if( m_CurrentViewController != nil )
			//	[m_CurrentViewController showLeaderboard:[NSString stringWithUTF8String:leaderboardString]];
        }
    }
}

/////////////////////////////////////////////////////
/// Method: ShowAchievements
/// Params: None
///
/////////////////////////////////////////////////////
void MacScores::ShowAchievements()
{
	if( !core::app::IsSupportingAchievements() )
		return;

    if( m_GCSupport )
    {
		if( [m_GameCenterManager isPlayerAuthenticated ] && 
			core::app::IsNetworkAvailable() )
		{
			//if( m_CurrentViewController != nil )
			//	[m_CurrentViewController showAchievements];
		}
		else 
		{
			Login();
		}
    }
}

/////////////////////////////////////////////////////
/// Method: AwardAchievement
/// Params: [in]percentageComplete, [in]achievementString
///
/////////////////////////////////////////////////////
void MacScores::AchievementReloaded( float percentageComplete, const char* achievementString )
{
	if( !core::app::IsSupportingAchievements() )
		return;

	DBG_ASSERT( m_Callback != 0 );
	
	// need to get the lua index id to save
	if( m_Callback != 0 )
	{	
		int achievementId = m_Callback->GetAchievementId( achievementString );
	
		//if( achString != 0 )
		{
			// add to the complete list
			if( !DoesExistInList( COMPLETE_LIST, achievementId ) )
			{
				support::TAchievement item;
			
				item.percentage = percentageComplete;
				snprintf( item.achievementString, MAX_ACHIEVEMENT_ID_LEN, "%s", achievementString );
				item.achievementId = achievementId;
			
				m_CompleteAchievements.push_back(item);
			}
			else
			{
				// update the percentage
				support::TAchievement* pItem = 0;
				pItem = GetItemInList( COMPLETE_LIST, achievementId );
				DBG_ASSERT( pItem != 0 );
				if( pItem != 0 )
				{
					if( percentageComplete > pItem->percentage )
						pItem->percentage = percentageComplete;
				}
			}	
		
			SaveCompleteAchievements();
		}
	}
}

/////////////////////////////////////////////////////
/// Method: AwardAchievement
/// Params: [in]percentageComplete, [in]achievementId
///
/////////////////////////////////////////////////////
void MacScores::AwardAchievement( float percentageComplete, int achievementId  )
{
	if( !core::app::IsSupportingAchievements() )
		return;

	DBG_ASSERT( m_Callback != 0 );

	// need to get the lua index id to save
	if( m_Callback != 0 )
	{
		const char* achString = m_Callback->GetAchievementString( achievementId );
		DBG_ASSERT( achString != 0 );

		if( achString != 0 )
		{
			// add to the complete list
			if( !DoesExistInList( COMPLETE_LIST, achievementId ) )
			{
				support::TAchievement item;

				item.percentage = percentageComplete;
				snprintf( item.achievementString, MAX_ACHIEVEMENT_ID_LEN, "%s", achString );
				item.achievementId = achievementId;

				m_CompleteAchievements.push_back(item);

				SaveCompleteAchievements();
			}
			else
			{
				// update the percentage
				support::TAchievement* pItem = 0;
				pItem = GetItemInList( COMPLETE_LIST, achievementId );
				DBG_ASSERT( pItem != 0 );
				if( pItem != 0 )
				{
					if( percentageComplete > pItem->percentage )
						pItem->percentage = percentageComplete;
				}

				SaveCompleteAchievements();
			}
		}
	}
}

/////////////////////////////////////////////////////
/// Method: IsNewAchievement
/// Params: [in]achievementId
///
/////////////////////////////////////////////////////
bool MacScores::IsNewAchievement( int achievementId )
{
	DBG_ASSERT( m_Callback != 0 );
	if( m_Callback != 0 )
	{
		const char* achString = m_Callback->GetAchievementString( achievementId );
		DBG_ASSERT( achString != 0 );

		if( achString != 0 )
		{
			// add to the complete list
			return ( !DoesExistInList( COMPLETE_LIST, achievementId ) );
		}
	}

	return false;
}

/////////////////////////////////////////////////////
/// Method: AwardKiipAchievement
/// Params: None
///
/////////////////////////////////////////////////////
void MacScores::AwardKiipAchievement( float percentageComplete, int achievementId )
{

}

/////////////////////////////////////////////////////
/// Method: SetCallback
/// 
///
/////////////////////////////////////////////////////
void MacScores::SetCallback( support::ScoresCallback* pCallback )
{
	m_Callback = pCallback;
}

/////////////////////////////////////////////////////
/// Method: SetLoggedIn
/// Params: 
///
/////////////////////////////////////////////////////
void MacScores::SetLoggedIn()
{
	DBG_ASSERT( m_Callback != 0 );

	m_IsLoggedIn = true;
		
	if( m_Callback != 0 )
	{
		m_Callback->LoggedIn();
	}
}

/////////////////////////////////////////////////////
/// Method: ScoreRecieved
/// Params: [in]leaderboard, [in]value
///
/////////////////////////////////////////////////////
void MacScores::ScoreRecieved( const char* leaderboard, int64_t value )
{
	const support::LeaderboardStore* leaderboardStore = support::GetLeaderboard(leaderboard);
	
	DBG_ASSERT_MSG( (leaderboardStore != 0), "MACSCORES: *ERROR* leaderboard %s is not in the games leaderboard list", leaderboard);

	if( leaderboardStore )
	{
		// update from GameCenter, see if local is higher than GC - if so send, or update
		if( LoadLocalScore(leaderboard) )
		{
			// only upload if score is better
			if( leaderboardStore->order == support::SCORE_HIGHER_IS_BETTER )
			{
				if( m_LocalScore.type == support::SCORE_TYPE_POINTS )
				{
					if( m_LocalScore.points > static_cast<unsigned long long int>(value) )
						UploadScore( leaderboardStore->order, m_LocalScore.points, leaderboard );
					else
						SaveLocalScore( leaderboardStore->order, static_cast<unsigned long long int>(value), leaderboard );
				}
				else if( m_LocalScore.type == support::SCORE_TYPE_TIME )
				{
					if( m_LocalScore.time > static_cast<long>(value) )
						UploadTime( leaderboardStore->order, m_LocalScore.time, leaderboard );
					else
						SaveLocalTime( leaderboardStore->order, static_cast<float>(value), leaderboard );
				}
			}
			else if( leaderboardStore->order == support::SCORE_LOWER_IS_BETTER )
			{
				if( m_LocalScore.type == support::SCORE_TYPE_POINTS )
				{
					if( m_LocalScore.points < static_cast<unsigned long long int>(value) )
						UploadScore( leaderboardStore->order, m_LocalScore.points, leaderboard );
					else
						SaveLocalScore( leaderboardStore->order, static_cast<unsigned long long int>(value), leaderboard );
				}
				else if( m_LocalScore.type == support::SCORE_TYPE_TIME )
				{
					if( m_LocalScore.time < static_cast<long>(value) )
						UploadTime( leaderboardStore->order, m_LocalScore.time, leaderboard );
					else
						SaveLocalTime( leaderboardStore->order, static_cast<float>(value), leaderboard );
				}				
			}
		}
		else
		{
			// save locally
			if( leaderboardStore->type == support::SCORE_TYPE_POINTS )
				SaveLocalScore( leaderboardStore->order, static_cast<unsigned long long int>(value), leaderboard );
			else if( leaderboardStore->type == support::SCORE_TYPE_TIME )
				SaveLocalTime( leaderboardStore->order, static_cast<float>(value), leaderboard );
		}
	}
}

/////////////////////////////////////////////////////
/// Method: UploadScore
/// Params: [in]order, [in]score, [in]leaderboardId, [in]leaderboardString
///
/////////////////////////////////////////////////////
void MacScores::UploadScore( support::EScoreOrder order, unsigned long long int score, const char* leaderboardString )
{
	DBG_ASSERT( leaderboardString != 0 );
	
	if( m_GCSupport )
    {
        if( leaderboardString )
        {
			SaveLocalScore( order, score, leaderboardString );

            // convert the string to an NSString
            int64_t scoreVal = static_cast<int64_t>(score);
            [m_GameCenterManager submitScore:scoreVal forLeaderboardIdentifier:[NSString stringWithUTF8String:leaderboardString]];
        }
    }    
}

/////////////////////////////////////////////////////
/// Method: UploadTime
/// Params: [in]order, [in]time, [in]leaderboardId, [in]leaderboardString
///
/////////////////////////////////////////////////////
void MacScores::UploadTime( support::EScoreOrder order, float time, const char* leaderboardString )
{
	DBG_ASSERT( leaderboardString != 0 );
    
	if( m_GCSupport )
    {
        if( leaderboardString != 0 )
        {
			SaveLocalTime( order, time, leaderboardString );
			
            // convert the string to an NSString
            
            [m_GameCenterManager submitScore:time forLeaderboardIdentifier:[NSString stringWithUTF8String:leaderboardString]];
        }
    } 
}

/////////////////////////////////////////////////////
/// Method: SubmitAchievement
/// Params: [in]achievementId, [in]achievementString, [in]percentageComplete
///
/////////////////////////////////////////////////////
void MacScores::SubmitAchievement( float percentageComplete, const char* achievementString )
{
	if( !core::app::IsSupportingAchievements() )
		return;

	DBG_ASSERT( achievementString != 0 );
  
	if( m_GCSupport &&
		achievementString != 0 )
    {
        // convert the string to an NSString and submit
        [m_GameCenterManager submitAchievement:[NSString stringWithUTF8String:achievementString] percentComplete:static_cast<double>(percentageComplete)];
    }
	
}

/////////////////////////////////////////////////////
/// Method: LoadLocalScores
/// Params: [in]leaderboardString
///
/////////////////////////////////////////////////////
bool MacScores::LoadLocalScore( const char* leaderboardString )
{
	DBG_ASSERT( leaderboardString != 0 );
	
    const char* savePath = core::app::GetSavePath();
    char saveFile[core::MAX_PATH];
    snprintf(saveFile, core::MAX_PATH, "%s/%s.sav", savePath, leaderboardString );
    
    if( file::FileOpen(saveFile, file::FILETYPE_BINARY_READ, &m_LocalFileHandle ) )
    {
        // get the current leaderboard score
		file::FileReadInt( &m_LocalScore.type, sizeof(int), 1, &m_LocalFileHandle );
		file::FileReadChar( &m_LocalScore.leaderboardId, sizeof(char), core::MAX_PATH, &m_LocalFileHandle );
		
		if( m_LocalScore.type == support::SCORE_TYPE_POINTS )
			file::FileReadUInt( &m_LocalScore.points, sizeof(long), 1, &m_LocalFileHandle );
		else if( m_LocalScore.type == support::SCORE_TYPE_TIME )
			file::FileReadFloat( &m_LocalScore.time, sizeof(float), 1, &m_LocalFileHandle );
		
        file::FileClose(&m_LocalFileHandle);
		
		return true;
    }
	
	return false;
}

/////////////////////////////////////////////////////
/// Method: SaveLocalScore
/// Params: None
///
/////////////////////////////////////////////////////
void MacScores::SaveLocalScore( support::EScoreOrder order, unsigned long long int score, const char* leaderboardString )
{
	DBG_ASSERT( leaderboardString != 0 );
	
	if( LoadLocalScore( leaderboardString )	)
	{
		if( order == support::SCORE_HIGHER_IS_BETTER )
		{
			if( m_LocalScore.type == support::SCORE_TYPE_POINTS )
			{
				if( m_LocalScore.points > score )
					score = m_LocalScore.points;
			}
			else
				DBG_ASSERT(0);
		}
		else if( order == support::SCORE_LOWER_IS_BETTER )
		{
			if( m_LocalScore.type == support::SCORE_TYPE_POINTS )
			{			
				if( m_LocalScore.points < score )
					score = m_LocalScore.points;
			}
			else
				DBG_ASSERT(0);
		}
	}
	
	const char* savePath = core::app::GetSavePath();
	char saveFile[core::MAX_PATH];
	snprintf(saveFile, core::MAX_PATH, "%s/%s.sav", savePath, leaderboardString );

	int type = support::SCORE_TYPE_POINTS;
	
	if( file::FileOpen(saveFile, file::FILETYPE_BINARY_WRITE, &m_LocalFileHandle ) )
	{
		// set the list of current leaderboard saves
		file::FileWriteInt( &type, sizeof(int), 1, &m_LocalFileHandle );
		file::FileWriteChar( const_cast<char *>(leaderboardString), sizeof(char), core::MAX_PATH, &m_LocalFileHandle );
		file::FileWriteUInt( &score, sizeof(unsigned long long int), 1, &m_LocalFileHandle );
		
		file::FileClose(&m_LocalFileHandle);
	}
}

/////////////////////////////////////////////////////
/// Method: SaveLocalTime
/// Params: None
///
/////////////////////////////////////////////////////
void MacScores::SaveLocalTime( support::EScoreOrder order, float time, const char* leaderboardString )
{
	DBG_ASSERT( leaderboardString != 0 );
	
	if( LoadLocalScore( leaderboardString )	)
	{
		if( order == support::SCORE_HIGHER_IS_BETTER )
		{
			if( m_LocalScore.type == support::SCORE_TYPE_TIME )
			{
				if( m_LocalScore.time > time )
					time = m_LocalScore.time;
			}
			else
				DBG_ASSERT(0);
		}
		else if( order == support::SCORE_LOWER_IS_BETTER )
		{
			if( m_LocalScore.type == support::SCORE_TYPE_TIME )
			{			
				if( m_LocalScore.points < time )
					time = m_LocalScore.time;
			}
			else
				DBG_ASSERT(0);
		}
	}
	
    const char* savePath = core::app::GetSavePath();
    char saveFile[core::MAX_PATH];
    snprintf(saveFile, core::MAX_PATH, "%s/%s.sav", savePath, leaderboardString );
	
	int type = support::SCORE_TYPE_TIME;
	
    if( file::FileOpen(saveFile, file::FILETYPE_BINARY_WRITE, &m_LocalFileHandle ) )
    {
        // set the list of current leaderboard saves
		file::FileWriteInt( &type, sizeof(int), 1, &m_LocalFileHandle );
		file::FileWriteChar( const_cast<char *>(leaderboardString), sizeof(char), core::MAX_PATH, &m_LocalFileHandle );
		file::FileWriteFloat( &time, sizeof(float), 1, &m_LocalFileHandle );
		
        file::FileClose(&m_LocalFileHandle);
    }	
}

/////////////////////////////////////////////////////
/// Method: AchievementSubmitted
/// Params: [in]achievementString, [in]errorId
///
/////////////////////////////////////////////////////
void MacScores::AchievementSubmitted( const char* achievementString, int errorId )
{
	if( !core::app::IsSupportingAchievements() )
		return;

	// look up the achievement string and then remove
	DBG_ASSERT( m_Callback != 0 );

	if( m_Callback != 0 &&
	   achievementString != 0 )
	{
		int achievementId = m_Callback->GetAchievementId( achievementString );

		m_Callback->AchievementSubmitted( achievementId, errorId );
	}
}

/////////////////////////////////////////////////////
/// Method: LoadAchievementsComplete
/// Params: None
///
/////////////////////////////////////////////////////
int MacScores::LoadAchievementsComplete()
{
	int tableIndex = 1;
	int i=0;

	m_CompleteAchievements.clear();

	if( file::FileExists(m_CompleteAchievementsFile) )
	{
		if( script::LoadScript(m_CompleteAchievementsFile) )
		{
			DBG_ASSERT(0);
		}
		else
		{
			lua_getglobal( script::LuaScripting::GetState(), "ach_complete" );
			tableIndex = -1;

			if( lua_istable( script::LuaScripting::GetState(), tableIndex ) )
			{
				int n = luaL_len( script::LuaScripting::GetState(), -1 );
				
				// go through all the tables in this table
				for( i = 1; i <= n; ++i )
				{
					lua_rawgeti( script::LuaScripting::GetState(), -1, i );
					if( lua_istable( script::LuaScripting::GetState(), -1 ) )
					{
						int paramIndex = 1;
						
						lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
						int achievementId = static_cast<int>( lua_tonumber( script::LuaScripting::GetState(), -1 ) ) ;
						lua_pop( script::LuaScripting::GetState(), 1 );

						lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
						float percent = static_cast<float>( lua_tonumber( script::LuaScripting::GetState(), -1 ) );
						lua_pop( script::LuaScripting::GetState(), 1 );

						support::TAchievement item;
						item.achievementId = achievementId;
						item.percentage = percent;
						std::memset( &item.achievementString, 0, sizeof(char)*MAX_ACHIEVEMENT_ID_LEN );

						m_CompleteAchievements.push_back(item);
					}
					lua_pop( script::LuaScripting::GetState(), 1 );
				}
			}

			lua_pop( script::LuaScripting::GetState(), 1 );
		}
	}

	return 0;
}

/////////////////////////////////////////////////////
/// Method: SaveCompleteAchievements
/// Params: None
///
/////////////////////////////////////////////////////
void MacScores::SaveCompleteAchievements()
{
	std::vector<support::TAchievement>::iterator it;

	// write the new file
	if( !file::FileOpen( m_CompleteAchievementsFile, file::FILETYPE_TEXT_WRITE, &m_LocalFileHandle ) )
	{
		DBG_ASSERT(0);
	}
	else
	{
		int i=0;

		fprintf( m_LocalFileHandle.fp, "ach_complete = {\n" );
		it = m_CompleteAchievements.begin();
		while( it != m_CompleteAchievements.end() )
		{
			if( i==0 )
				fprintf( m_LocalFileHandle.fp, "{ %d, %f }\n", (*it).achievementId, (*it).percentage );
			else
				fprintf( m_LocalFileHandle.fp, ",{ %d, %f }\n", (*it).achievementId, (*it).percentage );

			it++;
			i++;
		}
		fprintf( m_LocalFileHandle.fp, "}\n" );

		file::FileClose( &m_LocalFileHandle );
	}
}

/////////////////////////////////////////////////////
/// Method: DoesExistInList
/// Params: [in]whichList, [in]achievementId
///
/////////////////////////////////////////////////////
bool MacScores::DoesExistInList( EAchievementList whichList, int achievementId )
{
	std::vector<support::TAchievement>::iterator it;

    if( whichList == COMPLETE_LIST )
	{
		it = m_CompleteAchievements.begin();
		while( it != m_CompleteAchievements.end() )
		{
			if( (*it).achievementId == achievementId )
				return true;

			// next
			it++;
		}
	}

	return false;
}

/////////////////////////////////////////////////////
/// Method: RemoveFromList
/// Params: [in]whichList, [in]achievementId
///
/////////////////////////////////////////////////////
void MacScores::RemoveFromList( EAchievementList whichList, int achievementId )
{
	std::vector<support::TAchievement>::iterator it;

    if( whichList == COMPLETE_LIST )
	{
		it = m_CompleteAchievements.begin();
		while( it != m_CompleteAchievements.end() )
		{
			if( (*it).achievementId == achievementId )
			{
				m_CompleteAchievements.erase( it );
				it = m_CompleteAchievements.begin();
				continue;
			}

			// next
			it++;
		}
	}
}

/////////////////////////////////////////////////////
/// Method: GetItemInList
/// Params: [in]whichList, [in]achievementId
///
/////////////////////////////////////////////////////
support::TAchievement* MacScores::GetItemInList( EAchievementList whichList, int achievementId )
{
	std::vector<support::TAchievement>::iterator it;

    if( whichList == COMPLETE_LIST )
	{
		it = m_CompleteAchievements.begin();
		while( it != m_CompleteAchievements.end() )
		{
			if( (*it).achievementId == achievementId )
			{
				return &(*it);
			}

			// next
			it++;
		}
	}

	return 0;
}

#endif // BASE_PLATFORM_MAC

