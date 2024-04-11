
/*===================================================================
	File: AndroidScores.cpp
	Library: Support

	(C)Hidden Games
=====================================================================*/

#ifdef BASE_PLATFORM_ANDROID

#include "Core/CoreConsts.h"
#include "Core/CoreDefines.h"
#include "Core/FileIO.h"
#include "Core/App.h"
#include "Debug/Assertion.h"

#include "ScriptBase.h"

#include "Support/Leaderboards.h"
#include "AndroidScores.h"

using namespace support::aos;

/////////////////////////////////////////////////////
/// Default constructor
/// 
///
/////////////////////////////////////////////////////
AndroidScores::AndroidScores( support::ScoresCallback* callback )
	: m_Callback(callback)
{
	SetLoggedIn();
	
	std::memset( &m_CompleteAchievementsFile, 0, sizeof(char)*core::MAX_PATH+core::MAX_PATH );
	snprintf( m_CompleteAchievementsFile, core::MAX_PATH+core::MAX_PATH, "%s/ach_complete.sav", core::app::GetSavePath() );

	m_CompleteAchievements.clear();

	// load files to fill initial lists
	LoadAchievementsComplete();
}

/////////////////////////////////////////////////////
/// Default destructor
/// 
///
/////////////////////////////////////////////////////
AndroidScores::~AndroidScores()
{
    support::ClearLeaderboards();   
}

/////////////////////////////////////////////////////
/// Method: Login
/// Params: None
///
/////////////////////////////////////////////////////
void AndroidScores::Login()
{
    
}

/////////////////////////////////////////////////////
/// Method: LoadLeaderboardFile
/// Params: [in]filename
///
/////////////////////////////////////////////////////
void AndroidScores::LoadLeaderboardFile( const char* filename )
{
	// just load the script
	script::LoadScript( filename );
}

/////////////////////////////////////////////////////
/// Method: AppendLeaderboards
/// Params: [in]functionCall
///
/////////////////////////////////////////////////////
void AndroidScores::AppendLeaderboards( const char* functionCall )
{
	support::AppendLeaderboards(functionCall);
}

/////////////////////////////////////////////////////
/// Method: ResetAchievements
/// Params: None
///
/////////////////////////////////////////////////////
void AndroidScores::ResetAchievements()
{
    
}

/////////////////////////////////////////////////////
/// Method: SendScore
/// Params: [in]score, [in]leaderboardString
///
/////////////////////////////////////////////////////
void AndroidScores::SendScore( unsigned long long int score, const char* leaderboardString )
{
	
}

/////////////////////////////////////////////////////
/// Method: SyncScore
/// Params: [in]order, [in]leaderboardString
///
/////////////////////////////////////////////////////
void AndroidScores::SyncScore( support::EScoreOrder order, const char* leaderboardString )
{	

}

/////////////////////////////////////////////////////
/// Method: SyncAchievements
/// Params: None
///
/////////////////////////////////////////////////////
void AndroidScores::SyncAchievements()
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
				// on PC we always are successful
				AchievementSubmitted( achievementString, 0 );
			}
			
			// next
			it++;
		}

		// sync complete update the list
		//SavePendingAchievements();
	}
}

/////////////////////////////////////////////////////
/// Method: ShowLeaderboard
/// Params: None
///
/////////////////////////////////////////////////////
void AndroidScores::ShowLeaderboard( const char* leaderboardString )
{

}

/////////////////////////////////////////////////////
/// Method: ShowAchievements
/// Params: None
///
/////////////////////////////////////////////////////
void AndroidScores::ShowAchievements()
{
	if( !core::app::IsSupportingAchievements() )
		return;
}

/////////////////////////////////////////////////////
/// Method: AwardAchievement
/// Params: None
///
/////////////////////////////////////////////////////
void AndroidScores::AwardAchievement( float percentageComplete, int achievementId )
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
				// update the percentage if it already exists
				support::TAchievement* pItem = 0;
				pItem = GetItemInList( COMPLETE_LIST, achievementId );
				DBG_ASSERT( pItem != 0 );
				if( pItem != 0 )
				{
					if( percentageComplete > pItem->percentage )
					{
						pItem->percentage = percentageComplete;
					}
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
bool AndroidScores::IsNewAchievement( int achievementId )
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
void AndroidScores::AwardKiipAchievement( float percentageComplete, int achievementId )
{
#ifdef BASE_SUPPORT_KIIP
	DBG_ASSERT( m_Callback != 0 );
	if( m_Callback != 0 )
	{
		const char* achString = m_Callback->GetKiipAchievementString( achievementId );
		DBG_ASSERT( achString != 0 );

		if( achString != 0 )
		{
			// Get jmethod AwardKiipAchievement from KiipSupport class
			JNIEnv* env = 0;
			core::aos::GetVM()->GetEnv( (void **) &env, JNI_VERSION_1_6);
			jclass javaKiipSupport = env->FindClass("base/Support/KiipSupport");
			if( javaKiipSupport != 0 )
			{
				jmethodID jmethodAwardKiipAchievement = env->GetStaticMethodID(javaKiipSupport, "AwardKiipAchievement", "(Ljava/lang/String;)V");

				jstring theAchievement = (jstring)env->NewStringUTF(achString);

				// call it
				env->CallStaticVoidMethod( javaKiipSupport, jmethodAwardKiipAchievement, theAchievement );
			}
		}
	}
#endif // BASE_SUPPORT_KIIP
}

/////////////////////////////////////////////////////
/// Method: SetCallback
/// 
///
/////////////////////////////////////////////////////
void AndroidScores::SetCallback( support::ScoresCallback* pCallback )
{
	m_Callback = pCallback;
}

/////////////////////////////////////////////////////
/// Method: SetLoggedIn
/// Params: 
///
/////////////////////////////////////////////////////
void AndroidScores::SetLoggedIn()
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
void AndroidScores::ScoreRecieved( const char* leaderboard, int64_t value )
{

}

/////////////////////////////////////////////////////
/// Method: AchievementSubmitted
/// Params: [in]achievementString, [in]errorId
///
/////////////////////////////////////////////////////
void AndroidScores::AchievementSubmitted( const char* achievementString, int errorId )
{
	if( !core::app::IsSupportingAchievements() )
		return;

	// look up the achievement string and then remove
	DBG_ASSERT( m_Callback != 0 );

	if( m_Callback != 0 )
	{
		int achievementId = m_Callback->GetAchievementId( achievementString );

		m_Callback->AchievementSubmitted( achievementId, errorId );

		//if( errorId == 0 )
		//	RemoveFromList( PENDING_LIST, achievementId );
	}
}

/////////////////////////////////////////////////////
/// Method: LoadAchievementsComplete
/// Params: None
///
/////////////////////////////////////////////////////
int AndroidScores::LoadAchievementsComplete()
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
void AndroidScores::SaveCompleteAchievements()
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
bool AndroidScores::DoesExistInList( EAchievementList whichList, int achievementId )
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
void AndroidScores::RemoveFromList( EAchievementList whichList, int achievementId )
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
support::TAchievement* AndroidScores::GetItemInList( EAchievementList whichList, int achievementId )
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


#endif // BASE_PLATFORM_ANDROID
