
/*===================================================================
	File: CCustomerManagement.cpp
	Game: Taxi!

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"

#include "MathBase.h"
#include "RenderBase.h"

#include "MathBase.h"
#include "RenderBase.h"
#include "SoundBase.h"
#include "CollisionBase.h"
#include "ModelBase.h"
#include "InputBase.h"
#include "ScriptBase.h"

#include "AppConsts.h"
#include "H4.h"

#include "Level/Level.h"
#include "Player/Player.h"

#include "Resources/ModelResources.h"
#include "Resources/TextureResources.h"

#include "Enemy/EnemyAnimationLookup.h"
#include "Enemy/HangingSpiderAnimationLookup.h"
#include "Enemy/CrawlingSpiderAnimationLookup.h"
#include "Enemy/EnemyManagement.h"

namespace
{
	const int SKIN_HI_START = 100;
	const int SKIN_HI_END = 135;

	const int SKIN_START = 2;
	const int SKIN_END = 38;
	//const int SKIN_END = 109;

	const int FLOATINGHEAD_START = 0;
	const int FLOATINGHEAD_END = 8;

	const int FLOATINGHEAD_HI_START = 200;
	const int FLOATINGHEAD_HI_END = 216;

	const float ANIMATION_DISABLE_DISTANCE = 20.0f;
}

/////////////////////////////////////////////////////
/// Function: ScriptSetNPCModelIndex
/// Params: [in]pState
///
/////////////////////////////////////////////////////
int ScriptSetNPCModelIndex( lua_State* pState );
int ScriptSetNPCModelIndex( lua_State* pState )
{
	if( !H4::GetHiResMode() )
	{
		if( lua_istable( pState, 1 ) )
		{
			// grab the data
			Stump_Neck			= static_cast<int>(script::LuaGetNumberFromTableItem( "Stump_Neck", 1 )); 
			Stump_Head			= static_cast<int>(script::LuaGetNumberFromTableItem( "Stump_Head", 1 ));
			Stump_Shoulder_L	= static_cast<int>(script::LuaGetNumberFromTableItem( "Stump_Shoulder_L", 1 ));
			Stump_Arm_L			= static_cast<int>(script::LuaGetNumberFromTableItem( "Stump_Arm_L", 1 )); 
			Stump_Wrist_L		= static_cast<int>(script::LuaGetNumberFromTableItem( "Stump_Wrist_L", 1 )); 
			Stump_Elbow_L		= static_cast<int>(script::LuaGetNumberFromTableItem( "Stump_Elbow_L", 1 ));  
			Stump_Leg_L			= static_cast<int>(script::LuaGetNumberFromTableItem( "Stump_Leg_L", 1 )); 
			Stump_Hip_L			= static_cast<int>(script::LuaGetNumberFromTableItem( "Stump_Hip_L", 1 )); 
			Stump_Shin_L		= static_cast<int>(script::LuaGetNumberFromTableItem( "Stump_Shin_L", 1 )); 
			Stump_Knee_L		= static_cast<int>(script::LuaGetNumberFromTableItem( "Stump_Knee_L", 1 )); 
			Stump_Shoulder_R	= static_cast<int>(script::LuaGetNumberFromTableItem( "Stump_Shoulder_R", 1 )); 
			Stump_Arm_R			= static_cast<int>(script::LuaGetNumberFromTableItem( "Stump_Arm_R", 1 )); 
			Stump_Wrist_R		= static_cast<int>(script::LuaGetNumberFromTableItem( "Stump_Wrist_R", 1 )); 
			Stump_Elbow_R		= static_cast<int>(script::LuaGetNumberFromTableItem( "Stump_Elbow_R", 1 )); 
			Stump_Leg_R			= static_cast<int>(script::LuaGetNumberFromTableItem( "Stump_Leg_R", 1 )); 
			Stump_Hip_R			= static_cast<int>(script::LuaGetNumberFromTableItem( "Stump_Hip_R", 1 )); 
			Stump_Shin_R		= static_cast<int>(script::LuaGetNumberFromTableItem( "Stump_Shin_R", 1 )); 
			Stump_Knee_R		= static_cast<int>(script::LuaGetNumberFromTableItem( "Stump_Knee_R", 1 )); 

			Skeleton_Head		= static_cast<int>(script::LuaGetNumberFromTableItem( "Skeleton_Head", 1 )); 
			Skeleton_Body		= static_cast<int>(script::LuaGetNumberFromTableItem( "Skeleton_Body", 1 )); 
			Skeleton_Arm_R		= static_cast<int>(script::LuaGetNumberFromTableItem( "Skeleton_Arm_R", 1 )); 
			Skeleton_Wrist_R	= static_cast<int>(script::LuaGetNumberFromTableItem( "Skeleton_Wrist_R", 1 )); 
			Skeleton_Arm_L		= static_cast<int>(script::LuaGetNumberFromTableItem( "Skeleton_Arm_L", 1 )); 
			Skeleton_Wrist_L	= static_cast<int>(script::LuaGetNumberFromTableItem( "Skeleton_Wrist_L", 1 )); 
			Skeleton_Pelvis		= static_cast<int>(script::LuaGetNumberFromTableItem( "Skeleton_Pelvis", 1 )); 
			Skeleton_Hip_R		= static_cast<int>(script::LuaGetNumberFromTableItem( "Skeleton_Hip_R", 1 )); 
			Skeleton_Hip_L		= static_cast<int>(script::LuaGetNumberFromTableItem( "Skeleton_Hip_L", 1 )); 
			Skeleton_Shin_R		= static_cast<int>(script::LuaGetNumberFromTableItem( "Skeleton_Shin_R", 1 )); 
			Skeleton_Shin_L		= static_cast<int>(script::LuaGetNumberFromTableItem( "Skeleton_Shin_L", 1 )); 

			Normal_Head			= static_cast<int>(script::LuaGetNumberFromTableItem( "Normal_Head", 1 )); 
			Normal_Body			= static_cast<int>(script::LuaGetNumberFromTableItem( "Normal_Body", 1 )); 
			Normal_Arm_R		= static_cast<int>(script::LuaGetNumberFromTableItem( "Normal_Arm_R", 1 )); 
			Normal_Wrist_R		= static_cast<int>(script::LuaGetNumberFromTableItem( "Normal_Wrist_R", 1 )); 
			Normal_Arm_L		= static_cast<int>(script::LuaGetNumberFromTableItem( "Normal_Arm_L", 1 )); 
			Normal_Wrist_L		= static_cast<int>(script::LuaGetNumberFromTableItem( "Normal_Wrist_L", 1 )); 
			Normal_Hip_R		= static_cast<int>(script::LuaGetNumberFromTableItem( "Normal_Hip_R", 1 )); 
			Normal_Hip_L		= static_cast<int>(script::LuaGetNumberFromTableItem( "Normal_Hip_L", 1 )); 
			Normal_Shin_R		= static_cast<int>(script::LuaGetNumberFromTableItem( "Normal_Shin_R", 1 )); 
			Normal_Shin_L		= static_cast<int>(script::LuaGetNumberFromTableItem( "Normal_Shin_L", 1 )); 

			mShadow				= static_cast<int>(script::LuaGetNumberFromTableItem( "mShadow", 1 )); 
		}
	}
	return(0);
}

/////////////////////////////////////////////////////
/// Function: ScriptSetNPCModelHiIndex
/// Params: [in]pState
///
/////////////////////////////////////////////////////
int ScriptSetNPCModelHiIndex( lua_State* pState );
int ScriptSetNPCModelHiIndex( lua_State* pState )
{
	if( H4::GetHiResMode() )
	{
		if( lua_istable( pState, 1 ) )
		{
			// grab the data
			Stump_Neck			= static_cast<int>(script::LuaGetNumberFromTableItem( "Stump_Neck", 1 )); 
			Stump_Head			= static_cast<int>(script::LuaGetNumberFromTableItem( "Stump_Head", 1 ));
			Stump_Shoulder_L	= static_cast<int>(script::LuaGetNumberFromTableItem( "Stump_Shoulder_L", 1 ));
			Stump_Arm_L			= static_cast<int>(script::LuaGetNumberFromTableItem( "Stump_Arm_L", 1 )); 
			Stump_Wrist_L		= static_cast<int>(script::LuaGetNumberFromTableItem( "Stump_Wrist_L", 1 )); 
			Stump_Elbow_L		= static_cast<int>(script::LuaGetNumberFromTableItem( "Stump_Elbow_L", 1 ));  
			Stump_Leg_L			= static_cast<int>(script::LuaGetNumberFromTableItem( "Stump_Leg_L", 1 )); 
			Stump_Hip_L			= static_cast<int>(script::LuaGetNumberFromTableItem( "Stump_Hip_L", 1 )); 
			Stump_Shin_L		= static_cast<int>(script::LuaGetNumberFromTableItem( "Stump_Shin_L", 1 )); 
			Stump_Knee_L		= static_cast<int>(script::LuaGetNumberFromTableItem( "Stump_Knee_L", 1 )); 
			Stump_Shoulder_R	= static_cast<int>(script::LuaGetNumberFromTableItem( "Stump_Shoulder_R", 1 )); 
			Stump_Arm_R			= static_cast<int>(script::LuaGetNumberFromTableItem( "Stump_Arm_R", 1 )); 
			Stump_Wrist_R		= static_cast<int>(script::LuaGetNumberFromTableItem( "Stump_Wrist_R", 1 )); 
			Stump_Elbow_R		= static_cast<int>(script::LuaGetNumberFromTableItem( "Stump_Elbow_R", 1 )); 
			Stump_Leg_R			= static_cast<int>(script::LuaGetNumberFromTableItem( "Stump_Leg_R", 1 )); 
			Stump_Hip_R			= static_cast<int>(script::LuaGetNumberFromTableItem( "Stump_Hip_R", 1 )); 
			Stump_Shin_R		= static_cast<int>(script::LuaGetNumberFromTableItem( "Stump_Shin_R", 1 )); 
			Stump_Knee_R		= static_cast<int>(script::LuaGetNumberFromTableItem( "Stump_Knee_R", 1 )); 

			Skeleton_Head		= static_cast<int>(script::LuaGetNumberFromTableItem( "Skeleton_Head", 1 )); 
			Skeleton_Body		= static_cast<int>(script::LuaGetNumberFromTableItem( "Skeleton_Body", 1 )); 
			Skeleton_Arm_R		= static_cast<int>(script::LuaGetNumberFromTableItem( "Skeleton_Arm_R", 1 )); 
			Skeleton_Wrist_R	= static_cast<int>(script::LuaGetNumberFromTableItem( "Skeleton_Wrist_R", 1 )); 
			Skeleton_Arm_L		= static_cast<int>(script::LuaGetNumberFromTableItem( "Skeleton_Arm_L", 1 )); 
			Skeleton_Wrist_L	= static_cast<int>(script::LuaGetNumberFromTableItem( "Skeleton_Wrist_L", 1 )); 
			Skeleton_Pelvis		= static_cast<int>(script::LuaGetNumberFromTableItem( "Skeleton_Pelvis", 1 )); 
			Skeleton_Hip_R		= static_cast<int>(script::LuaGetNumberFromTableItem( "Skeleton_Hip_R", 1 )); 
			Skeleton_Hip_L		= static_cast<int>(script::LuaGetNumberFromTableItem( "Skeleton_Hip_L", 1 )); 
			Skeleton_Shin_R		= static_cast<int>(script::LuaGetNumberFromTableItem( "Skeleton_Shin_R", 1 )); 
			Skeleton_Shin_L		= static_cast<int>(script::LuaGetNumberFromTableItem( "Skeleton_Shin_L", 1 )); 

			Normal_Head			= static_cast<int>(script::LuaGetNumberFromTableItem( "Normal_Head", 1 )); 
			Normal_Body			= static_cast<int>(script::LuaGetNumberFromTableItem( "Normal_Body", 1 )); 
			Normal_Arm_R		= static_cast<int>(script::LuaGetNumberFromTableItem( "Normal_Arm_R", 1 )); 
			Normal_Wrist_R		= static_cast<int>(script::LuaGetNumberFromTableItem( "Normal_Wrist_R", 1 )); 
			Normal_Arm_L		= static_cast<int>(script::LuaGetNumberFromTableItem( "Normal_Arm_L", 1 )); 
			Normal_Wrist_L		= static_cast<int>(script::LuaGetNumberFromTableItem( "Normal_Wrist_L", 1 )); 
			Normal_Hip_R		= static_cast<int>(script::LuaGetNumberFromTableItem( "Normal_Hip_R", 1 )); 
			Normal_Hip_L		= static_cast<int>(script::LuaGetNumberFromTableItem( "Normal_Hip_L", 1 )); 
			Normal_Shin_R		= static_cast<int>(script::LuaGetNumberFromTableItem( "Normal_Shin_R", 1 )); 
			Normal_Shin_L		= static_cast<int>(script::LuaGetNumberFromTableItem( "Normal_Shin_L", 1 )); 

			mShadow				= static_cast<int>(script::LuaGetNumberFromTableItem( "mShadow", 1 )); 
		}
	}
	return(0);
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
EnemyManagement::EnemyManagement( Player& player )
	:	m_Player( player )
	,	m_Level(0)
	,	m_NavMesh(0)
{
	int i=0;

	m_ActiveEnemies = 0;
	m_MaxActiveEnemies = MAX_ENEMIES;

	m_NumGameKills = 0;

	m_LastDeltaTime = 0.0f;

	// ### WALKING ENEMY ###
	m_pEnemyModelData = 0;
	m_pEnemyModelData = new mdl::ModelHGA;
	DBG_ASSERT_MSG( (m_pEnemyModelData != 0), "Allocation of enemy HGA model failed" );

	script::LuaScripting::GetInstance()->RegisterFunction( "SetNPCModelHiIndex",		ScriptSetNPCModelHiIndex );
	script::LuaScripting::GetInstance()->RegisterFunction( "SetNPCModelIndex",		ScriptSetNPCModelIndex );
	script::LoadScript( "lua/model-index/npc-model-index.lua" );

	if( H4::GetHiResMode() )
	{
		m_pEnemyModelData->LoadBindPose( "hga-hi/npc_game_bind.hga", GL_NEAREST, GL_NEAREST_MIPMAP_NEAREST, core::app::GetLowPowerMode() );

		const res::TextureResourceStore* rs = res::GetTextureResource(300);
		DBG_ASSERT( rs != 0 );

		int matIndex = 0;
		m_pEnemyModelData->ChangeTextureOnMaterialId( Skeleton_Head, matIndex, rs->texId );
		m_pEnemyModelData->ChangeTextureOnMaterialId( Skeleton_Body, matIndex, rs->texId  );
		m_pEnemyModelData->ChangeTextureOnMaterialId( Skeleton_Arm_R, matIndex, rs->texId  );
		m_pEnemyModelData->ChangeTextureOnMaterialId( Skeleton_Wrist_R, matIndex, rs->texId  );
		m_pEnemyModelData->ChangeTextureOnMaterialId( Skeleton_Arm_L, matIndex, rs->texId  );
		m_pEnemyModelData->ChangeTextureOnMaterialId( Skeleton_Wrist_L, matIndex, rs->texId  );
		m_pEnemyModelData->ChangeTextureOnMaterialId( Skeleton_Pelvis, matIndex, rs->texId  );
		m_pEnemyModelData->ChangeTextureOnMaterialId( Skeleton_Hip_R, matIndex, rs->texId  );
		m_pEnemyModelData->ChangeTextureOnMaterialId( Skeleton_Hip_L, matIndex, rs->texId  );
		m_pEnemyModelData->ChangeTextureOnMaterialId( Skeleton_Shin_R, matIndex, rs->texId  );
		m_pEnemyModelData->ChangeTextureOnMaterialId( Skeleton_Shin_L, matIndex, rs->texId  );

		rs = res::GetTextureResource(301);
		DBG_ASSERT( rs != 0 );

		m_pEnemyModelData->ChangeTextureOnMaterialId( mShadow, matIndex, rs->texId );
	}
	else
	{
		m_pEnemyModelData->LoadBindPose( "hga/npc_game_bind.hga", GL_NEAREST, GL_NEAREST_MIPMAP_NEAREST, core::app::GetLowPowerMode() );
	}

	for( i=0; i < EnemyAnim::NUM_ANIMS; ++i )
	{
		const EnemyAnim::AnimData& animData = GetEnemyAnimationData(i);
		m_pEnemyModelData->LoadAnimation( animData.szFile, animData.animId, animData.loopFlag );
	}

	// no culling as they are moving models
	m_pEnemyModelData->SetModelGeneralFlags( 0 );
	m_pEnemyModelData->SetupAndFree();


	// ### FLOATING HEAD ENEMY ###
	m_pHeadModelData = 0;
	m_pHeadModelData = new mdl::ModelHGA;
	DBG_ASSERT_MSG( (m_pHeadModelData != 0), "Allocation of floating head enemy HGA model failed" );

	if( H4::GetHiResMode() )
		m_pHeadModelData->LoadBindPose( "hga-hi/enemy_head_bind.hga", GL_NEAREST, GL_NEAREST_MIPMAP_NEAREST, core::app::GetLowPowerMode() );
	else
		m_pHeadModelData->LoadBindPose( "hga/enemy_head_bind.hga", GL_NEAREST, GL_NEAREST_MIPMAP_NEAREST, core::app::GetLowPowerMode() );

	m_pHeadModelData->LoadAnimation( "hga/enemy_head_bounce.hga", 0, true );

	// no culling as they are moving models
	m_pHeadModelData->SetModelGeneralFlags( 0 );
	m_pHeadModelData->SetupAndFree();


	// ### HANGING SPIDER ENEMY ###
	m_pHangingSpiderModelData = 0;
	m_pHangingSpiderModelData = new mdl::ModelHGA;
	DBG_ASSERT_MSG( (m_pHangingSpiderModelData != 0), "Allocation of hanging spider HGA model failed" );

	m_pHangingSpiderModelData->LoadBindPose( "hga/enemy_spider_hanging_bind.hga", GL_NEAREST, GL_NEAREST_MIPMAP_NEAREST, core::app::GetLowPowerMode() );

	for( i=0; i < HangingSpiderAnim::NUM_ANIMS; ++i )
	{
		const HangingSpiderAnim::AnimData& animData = GetHangingSpiderAnimationData(i);
		m_pHangingSpiderModelData->LoadAnimation( animData.szFile, animData.animId, animData.loopFlag );
	}

	// no culling as they are moving models
	m_pHangingSpiderModelData->SetModelGeneralFlags( 0 );
	m_pHangingSpiderModelData->SetupAndFree();


	// ### CRAWLING SPIDER ENEMY ###
/*	m_pCrawlingSpiderModelData = 0;
	m_pCrawlingSpiderModelData = new mdl::ModelHGA;
	DBG_ASSERT_MSG( (m_pCrawlingSpiderModelData != 0), "Allocation of hanging spider HGA model failed" );

	m_pCrawlingSpiderModelData->LoadBindPose( "hga/enemy_spider_crawling_bind.hga", GL_NEAREST, GL_NEAREST_MIPMAP_NEAREST, core::app::GetLowPowerMode() );

	for( i=0; i < CrawlingSpiderAnim::NUM_ANIMS; ++i )
	{
		const CrawlingSpiderAnim::AnimData& animData = GetCrawlingSpiderAnimationData(i);
		m_pCrawlingSpiderModelData->LoadAnimation( animData.szFile, animData.animId, animData.loopFlag );
	}

	// no culling as they are moving models
	m_pCrawlingSpiderModelData->SetModelGeneralFlags( 0 );
	m_pCrawlingSpiderModelData->SetupAndFree();
*/

	// ### GIBS ###
	// id 0: "hgm/gib_small.hgm"
	m_pSmallGibData = res::LoadModel( 0, true );
	DBG_ASSERT_MSG( (m_pSmallGibData != 0), "Small gib model not loaded or found" );
	
	m_pSmallGibData->SetModelGeneralFlags( 0 );
	m_pSmallGibData->SetupAndFree();

	// id 1: "hgm/gib_large.hgm"
	m_pLargeGibData = res::LoadModel( 1, true );
	DBG_ASSERT_MSG( (m_pSmallGibData != 0), "Large gib model not loaded or found" );

	m_pLargeGibData->SetModelGeneralFlags( 0 );
	m_pLargeGibData->SetupAndFree();
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
EnemyManagement::~EnemyManagement()
{
	unsigned int i=0;

	for( i=0; i < m_EnemyList.size(); ++i )
	{
		delete m_EnemyList[i];
	}

	for( i=0; i < m_FloatingHeadList.size(); ++i )
	{
		delete m_FloatingHeadList[i];
	}

	for( i=0; i < m_HangingSpiderList.size(); ++i )
	{
		delete m_HangingSpiderList[i];
	}

	for( i=0; i < m_CrawlingSpiderList.size(); ++i )
	{
		delete m_CrawlingSpiderList[i];
	}

	for( i=0; i < m_InsectsList.size(); ++i )
	{
		delete m_InsectsList[i];
	}

	res::RemoveModel( m_pSmallGibData );
	res::RemoveModel( m_pLargeGibData );

	m_pSmallGibData = 0;
	m_pLargeGibData = 0;

	if( m_pEnemyModelData != 0 )
	{
		delete m_pEnemyModelData;
		m_pEnemyModelData = 0;
	}

	if( m_pHeadModelData != 0 )
	{
		delete m_pHeadModelData;
		m_pHeadModelData = 0;
	}

	if( m_pHangingSpiderModelData != 0 )
	{
		delete m_pHangingSpiderModelData;
		m_pHangingSpiderModelData = 0;
	}

/*	if( m_pCrawlingSpiderModelData != 0 )
	{
		delete m_pCrawlingSpiderModelData;
		m_pCrawlingSpiderModelData = 0;
	}
*/

	m_LightZones.clear();

	m_EnemySpawns.clear();
	m_ConstantSpawns.clear();
	m_FloatingHeadSpawns.clear();
	m_HangingSpiderSpawns.clear();
	m_CrawlingSpiderSpawns.clear();
	m_InsectSpawns.clear();
}

/////////////////////////////////////////////////////
/// Method: SetupLevelData
/// Params: [in]activeLevel
///
/////////////////////////////////////////////////////
void EnemyManagement::SetupLevelData( Level& activeLevel, NavigationMesh* pNavMesh  )
{
	m_Level = &activeLevel;
	m_NavMesh = pNavMesh;

	m_LightZones.clear();
	m_LightZones = m_Level->GetLightZoneList();
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void EnemyManagement::Draw( bool pauseFlag )
{
	DrawEnemies( pauseFlag );

	DrawFloatingHeads( pauseFlag );

	DrawHangingSpiders( pauseFlag );

	//DrawCrawlingSpiders( pauseFlag );

	DrawInsects( pauseFlag );
}

/////////////////////////////////////////////////////
/// Method: Draw2D
/// Params: None
///
/////////////////////////////////////////////////////
void EnemyManagement::Draw2D()
{

}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void EnemyManagement::Update( float deltaTime )
{
	m_LastDeltaTime = deltaTime;

	UpdateEnemies( deltaTime );

	UpdateFloatingHeads( deltaTime );

	UpdateHangingSpiders( deltaTime );

	//UpdateCrawlingSpiders( deltaTime );

	UpdateInsects( deltaTime );
}

/////////////////////////////////////////////////////
/// Method: CreateLevelEnemies
/// Params: [in]maxEnemies
///
/////////////////////////////////////////////////////
void EnemyManagement::CreateLevelEnemies( int maxEnemies )
{
	unsigned int i=0;
	unsigned int count = static_cast<unsigned int>(maxEnemies);

	if( m_EnemySpawns.size() <= 0 )
		return;

	DBG_ASSERT_MSG( (count <= m_EnemySpawns.size()), "Not enough enemy spawns for enemy creation count" );
	if( count >= m_EnemySpawns.size() )
		count = static_cast<unsigned int>(m_EnemySpawns.size());

	for( i=0; i < count; ++i )
	{
		GLuint skinIndex = 0;

		if( H4::GetHiResMode() )
		{
			skinIndex = static_cast<GLuint>( math::RandomNumber( SKIN_HI_START, SKIN_HI_END-1 ) );
			const res::TextureResourceStore* rs = res::GetTextureResource(skinIndex);
			DBG_ASSERT( rs != 0 );

			if( !rs->preLoad )
			{
				res::LoadTexture( skinIndex );
			}

			CreateEnemy( rs->texId );
		}
		else
		{
			skinIndex = static_cast<GLuint>( math::RandomNumber( SKIN_START, SKIN_END-1 ) );
			
			CreateEnemy( skinIndex );
		}
	}
}

/////////////////////////////////////////////////////
/// Method: CreateLevelFloatingHeads
/// Params: [in]maxHeads
///
/////////////////////////////////////////////////////
void EnemyManagement::CreateLevelFloatingHeads( int maxHeads )
{
	unsigned int i=0;
	unsigned int count = static_cast<unsigned int>(maxHeads);

	if( m_FloatingHeadSpawns.size() <= 0 )
		return;

	DBG_ASSERT_MSG( (count <= m_FloatingHeadSpawns.size()), "Not enough floating head spawns for head creation count" );
	if( count >= m_FloatingHeadSpawns.size() )
		count = static_cast<unsigned int>(m_FloatingHeadSpawns.size());

	for( i=0; i < count; ++i )
	{
		GLuint skinIndex = 0;

		if( H4::GetHiResMode() )
		{
			skinIndex = static_cast<GLuint>( math::RandomNumber( FLOATINGHEAD_HI_START, FLOATINGHEAD_HI_END ) );
			const res::TextureResourceStore* rs = res::GetTextureResource(skinIndex);
			DBG_ASSERT( rs != 0 );

			if( !rs->preLoad )
			{
				res::LoadTexture( skinIndex );
			}

			CreateFloatingHead( rs->texId );
		}
		else
		{
			int skinIndex = math::RandomNumber( FLOATINGHEAD_START, FLOATINGHEAD_END );
			CreateFloatingHead( skinIndex );
		}
	}
}

/////////////////////////////////////////////////////
/// Method: CreateLevelHangingSpiders
/// Params: [in]maxHeads
///
/////////////////////////////////////////////////////
void EnemyManagement::CreateLevelHangingSpiders( int maxSpiders )
{
	unsigned int i=0;
	unsigned int count = static_cast<unsigned int>(maxSpiders);

	if( m_HangingSpiderSpawns.size() <= 0 )
		return;

	DBG_ASSERT_MSG( (count <= m_HangingSpiderSpawns.size()), "Not enough hanging spider spawns for spider creation count" );
	if( count >= m_HangingSpiderSpawns.size() )
		count = static_cast<unsigned int>(m_HangingSpiderSpawns.size());

	for( i=0; i < count; ++i )
	{
		CreateHangingSpider( );
	}
}

/////////////////////////////////////////////////////
/// Method: CreateLevelCrawlingSpiders
/// Params: [in]maxHeads
///
/////////////////////////////////////////////////////
void EnemyManagement::CreateLevelCrawlingSpiders( int maxSpiders )
{
	unsigned int i=0;
	unsigned int count = static_cast<unsigned int>(maxSpiders);

	if( m_CrawlingSpiderSpawns.size() <= 0 )
		return;

	DBG_ASSERT_MSG( (count <= m_CrawlingSpiderSpawns.size()), "Not enough crawling spider spawns for spider creation count" );
	if( count >= m_CrawlingSpiderSpawns.size() )
		count = static_cast<unsigned int>(m_CrawlingSpiderSpawns.size());

	for( i=0; i < count; ++i )
	{
		CreateCrawlingSpider( );
	}
}

/////////////////////////////////////////////////////
/// Method: CreateLevelInsects
/// Params: [in]maxHeads
///
/////////////////////////////////////////////////////
void EnemyManagement::CreateLevelInsects( int maxInsects )
{
	unsigned int i=0;
	unsigned int count = static_cast<unsigned int>(maxInsects);

	if( m_InsectSpawns.size() <= 0 )
		return;

	DBG_ASSERT_MSG( (count <= m_InsectSpawns.size()), "Not enough insect spawns for insect creation count" );
	if( count >= m_InsectSpawns.size() )
		count = static_cast<unsigned int>(m_InsectSpawns.size());

	for( i=0; i < count; ++i )
	{
		CreateInsect( );
	}
}

/////////////////////////////////////////////////////
/// Method: CreateEnemy
/// Params: [in]skinIndex
///
/////////////////////////////////////////////////////
bool EnemyManagement::CreateEnemy( GLuint skinIndex )
{
	// spawn a customer on the area
	Enemy* newEnemy = 0;
	newEnemy = new Enemy( m_NavMesh );
	DBG_ASSERT_MSG( (newEnemy != 0), "Could not create a new Enemy object" );

	// create
	newEnemy->SetupEnemy( m_pEnemyModelData, m_pSmallGibData, m_pLargeGibData, skinIndex );
	newEnemy->SetState( Enemy::EnemyState_Idle );
	newEnemy->SetAnimation( math::RandomNumber(EnemyAnim::IDLE, EnemyAnim::IDLE_ALT), 0, true );

	if( m_EnemySpawns.size() <= 0 )
	{
		math::Vec3 randPosition( math::RandomNumber( -2.0f, 2.0f ), 0.0f, math::RandomNumber( -2.0f, 2.0f ) );
		newEnemy->SetPosition( randPosition );
	}
	else
	{
		bool findSpawn = false;
		while( !findSpawn )
		{
			int spawnIndex = math::RandomNumber( 0, static_cast<int>(m_EnemySpawns.size())-1 );
			if( !m_EnemySpawns[spawnIndex].enemyPlaced )
			{
				m_EnemySpawns[spawnIndex].enemyPlaced = true;

				newEnemy->SetPosition( m_EnemySpawns[spawnIndex].pos );
				newEnemy->SetTargetRotation( m_EnemySpawns[spawnIndex].angle, true );

				findSpawn = true;
			}
		}
	}

	// add new enemy
	m_EnemyList.push_back(newEnemy);

	m_ActiveEnemies++;

	return(true);
}

/////////////////////////////////////////////////////
/// Method: CreateFloatingHead
/// Params: [in]skinIndex
///
/////////////////////////////////////////////////////
bool EnemyManagement::CreateFloatingHead( int skinIndex )
{
	// spawn a customer on the area
	FloatingHead* newHead = 0;
	newHead = new FloatingHead(  );
	DBG_ASSERT_MSG( (newHead != 0), "Could not create a new floating head object" );

	// create
	newHead->SetupFloatingHead( m_pHeadModelData, skinIndex );
	newHead->SetState( FloatingHead::FloatingHeadState_Idle );
	newHead->SetAnimation( 0, -1, true );

	if( m_FloatingHeadSpawns.size() <= 0 )
	{
		math::Vec3 randPosition( math::RandomNumber( -2.0f, 2.0f ), 0.0f, math::RandomNumber( -2.0f, 2.0f ) );
		newHead->SetPosition( randPosition );
	}
	else
	{
		bool findSpawn = false;
		while( !findSpawn )
		{
			int spawnIndex = math::RandomNumber( 0, static_cast<int>(m_FloatingHeadSpawns.size())-1 );
			if( !m_FloatingHeadSpawns[spawnIndex].enemyPlaced )
			{
				m_FloatingHeadSpawns[spawnIndex].enemyPlaced = true;

				newHead->SetPosition( m_FloatingHeadSpawns[spawnIndex].pos );
				newHead->SetTargetRotation( m_FloatingHeadSpawns[spawnIndex].angle, true );

				findSpawn = true;
			}
		}
	}

	// add new enemy
	m_FloatingHeadList.push_back(newHead);

	m_ActiveEnemies++;

	return(true);
}

/////////////////////////////////////////////////////
/// Method: CreateHangingSpider
/// Params: none
///
/////////////////////////////////////////////////////
bool EnemyManagement::CreateHangingSpider( )
{
	// spawn a customer on the area
	HangingSpider* newSpider = 0;
	newSpider = new HangingSpider(  );
	DBG_ASSERT_MSG( (newSpider != 0), "Could not create a new hanging spider object" );

	// create
	newSpider->SetupHangingSpider( m_pHangingSpiderModelData );
	newSpider->SetState( HangingSpider::HangingSpiderState_IdleUp );
	newSpider->SetAnimation( HangingSpiderAnim::IDLE_UP, -1, true );

	if( m_HangingSpiderSpawns.size() <= 0 )
	{
		math::Vec3 randPosition( math::RandomNumber( -2.0f, 2.0f ), 0.0f, math::RandomNumber( -2.0f, 2.0f ) );
		newSpider->SetPosition( randPosition );
	}
	else
	{
		bool findSpawn = false;
		while( !findSpawn )
		{
			int spawnIndex = math::RandomNumber( 0, static_cast<int>(m_HangingSpiderSpawns.size())-1 );
			if( !m_HangingSpiderSpawns[spawnIndex].enemyPlaced )
			{
				m_HangingSpiderSpawns[spawnIndex].enemyPlaced = true;

				newSpider->SetPosition( m_HangingSpiderSpawns[spawnIndex].pos );
				newSpider->SetRotation( m_HangingSpiderSpawns[spawnIndex].angle );

				findSpawn = true;
			}
		}
	}

	// add new enemy
	m_HangingSpiderList.push_back(newSpider);

	m_ActiveEnemies++;

	return(true);
}

/////////////////////////////////////////////////////
/// Method: CreateCrawlingSpider
/// Params: none
///
/////////////////////////////////////////////////////
bool EnemyManagement::CreateCrawlingSpider( )
{
	// spawn a customer on the area
	CrawlingSpider* newSpider = 0;
	newSpider = new CrawlingSpider( m_NavMesh );
	DBG_ASSERT_MSG( (newSpider != 0), "Could not create a new crawling spider object" );

	// create
	newSpider->SetupCrawlingSpider( m_pCrawlingSpiderModelData );
	newSpider->SetState( CrawlingSpider::CrawlingSpiderState_Idle );
	newSpider->SetAnimation( CrawlingSpiderAnim::IDLE, -1, true );

	if( m_CrawlingSpiderSpawns.size() <= 0 )
	{
		math::Vec3 randPosition( math::RandomNumber( -2.0f, 2.0f ), 0.0f, math::RandomNumber( -2.0f, 2.0f ) );
		newSpider->SetPosition( randPosition );
	}
	else
	{
		bool findSpawn = false;
		while( !findSpawn )
		{
			int spawnIndex = math::RandomNumber( 0, static_cast<int>(m_CrawlingSpiderSpawns.size())-1 );
			if( !m_CrawlingSpiderSpawns[spawnIndex].enemyPlaced )
			{
				m_CrawlingSpiderSpawns[spawnIndex].enemyPlaced = true;

				newSpider->SetPosition( m_CrawlingSpiderSpawns[spawnIndex].pos );
				newSpider->SetTargetRotation( m_CrawlingSpiderSpawns[spawnIndex].angle, true );

				findSpawn = true;
			}
		}
	}

	// add new enemy
	m_CrawlingSpiderList.push_back(newSpider);

	m_ActiveEnemies++;

	return(true);
}

/////////////////////////////////////////////////////
/// Method: CreateInsect
/// Params: None
///
/////////////////////////////////////////////////////
bool EnemyManagement::CreateInsect( )
{
	// spawn a customer on the area
	Insects* newInsect = 0;
	newInsect = new Insects( );
	DBG_ASSERT_MSG( (newInsect != 0), "Could not create a new insect object" );

	// create


	if( m_InsectSpawns.size() <= 0 )
	{
		math::Vec3 randPosition( math::RandomNumber( -2.0f, 2.0f ), 0.0f, math::RandomNumber( -2.0f, 2.0f ) );
		newInsect->SetPosition( randPosition );
	}
	else
	{
		bool findSpawn = false;
		while( !findSpawn )
		{
			int spawnIndex = math::RandomNumber( 0, static_cast<int>(m_InsectSpawns.size())-1 );
			if( !m_InsectSpawns[spawnIndex].enemyPlaced )
			{
				m_InsectSpawns[spawnIndex].enemyPlaced = true;

				newInsect->SetPosition( m_InsectSpawns[spawnIndex].pos );
				newInsect->SetRotation( m_InsectSpawns[spawnIndex].angle );

				findSpawn = true;
			}
		}
	}

	// add new enemy
	m_InsectsList.push_back(newInsect);

	m_ActiveEnemies++;

	return(true);
}

/////////////////////////////////////////////////////
/// Method: ClearLevelData
/// Params: None
///
/////////////////////////////////////////////////////
void EnemyManagement::ClearLevelData()
{
	unsigned int i=0;

	for( i=0; i < m_EnemyList.size(); ++i )
	{
		delete m_EnemyList[i];
	}

	for( i=0; i < m_FloatingHeadList.size(); ++i )
	{
		delete m_FloatingHeadList[i];
	}

	for( i=0; i < m_HangingSpiderList.size(); ++i )
	{
		delete m_HangingSpiderList[i];
	}

	for( i=0; i < m_CrawlingSpiderList.size(); ++i )
	{
		delete m_CrawlingSpiderList[i];
	}

	for( i=0; i < m_InsectsList.size(); ++i )
	{
		delete m_InsectsList[i];
	}

	m_EnemyList.clear();
	m_FloatingHeadList.clear();
	m_HangingSpiderList.clear();
	m_CrawlingSpiderList.clear();
	m_InsectsList.clear();

	m_EnemySpawns.clear();
	m_ConstantSpawns.clear();
	m_FloatingHeadSpawns.clear();
	m_HangingSpiderSpawns.clear();
	m_CrawlingSpiderSpawns.clear();
	m_InsectSpawns.clear();

	m_ActiveEnemies = 0;

	m_Level = 0;
	m_NavMesh = 0; 
}


/////////////////////////////////////////////////////
/// Method: DrawDebug
/// Params: None
///
/////////////////////////////////////////////////////
void EnemyManagement::DrawDebug()
{	
	std::vector<Enemy* >::iterator itEnemy = m_EnemyList.begin();

	while( itEnemy != m_EnemyList.end() )
	{
		// do not draw or animate when outside the frustun
		//const collision::Sphere& sphere = (*it)->GetBoundingSphere();
		//if( renderer::OpenGL::GetInstance()->SphereInFrustum( sphere.vCenterPoint.X, sphere.vCenterPoint.Y, sphere.vCenterPoint.Z, sphere.fRadius ) )
		{
			(*itEnemy)->DrawDebug();
		}

		// next
		itEnemy++;
	}

	std::vector<FloatingHead* >::iterator itHead = m_FloatingHeadList.begin();

	while( itHead != m_FloatingHeadList.end() )
	{
		// do not draw or animate when outside the frustun
		//const collision::Sphere& sphere = (*it)->GetBoundingSphere();
		//if( renderer::OpenGL::GetInstance()->SphereInFrustum( sphere.vCenterPoint.X, sphere.vCenterPoint.Y, sphere.vCenterPoint.Z, sphere.fRadius ) )
		{
			(*itHead)->DrawDebug();
		}

		// next
		itHead++;
	}

	std::vector<HangingSpider* >::iterator itHSpider = m_HangingSpiderList.begin();

	while( itHSpider != m_HangingSpiderList.end() )
	{
		// do not draw or animate when outside the frustun
		//const collision::Sphere& sphere = (*it)->GetBoundingSphere();
		//if( renderer::OpenGL::GetInstance()->SphereInFrustum( sphere.vCenterPoint.X, sphere.vCenterPoint.Y, sphere.vCenterPoint.Z, sphere.fRadius ) )
		{
			(*itHSpider)->DrawDebug();
		}

		// next
		itHSpider++;
	}

	std::vector<CrawlingSpider* >::iterator itCSpider = m_CrawlingSpiderList.begin();

	while( itCSpider != m_CrawlingSpiderList.end() )
	{
		// do not draw or animate when outside the frustun
		//const collision::Sphere& sphere = (*it)->GetBoundingSphere();
		//if( renderer::OpenGL::GetInstance()->SphereInFrustum( sphere.vCenterPoint.X, sphere.vCenterPoint.Y, sphere.vCenterPoint.Z, sphere.fRadius ) )
		{
			(*itCSpider)->DrawDebug();
		}

		// next
		itCSpider++;
	}

	std::vector<Insects* >::iterator itInsect = m_InsectsList.begin();

	while( itInsect != m_InsectsList.end() )
	{
		// do not draw or animate when outside the frustun
		//const collision::Sphere& sphere = (*it)->GetBoundingSphere();
		//if( renderer::OpenGL::GetInstance()->SphereInFrustum( sphere.vCenterPoint.X, sphere.vCenterPoint.Y, sphere.vCenterPoint.Z, sphere.fRadius ) )
		{
			(*itInsect)->DrawDebug();
		}

		// next
		itInsect++;
	}
}

/////////////////////////////////////////////////////
/// Method: DrawDebug2D
/// Params: None
///
/////////////////////////////////////////////////////
void EnemyManagement::DrawDebug2D()
{
	//int offset = 10;
	//int xPos = core::app::GetOrientationWidth()-120;
	//int yPos = core::app::GetOrientationHeight()-30;

	//renderer::OpenGL::GetInstance()->EnableTextureArray();

	//DBGPRINT( xPos, yPos-=offset, "ACTIVE ENEMY (%d)", m_ActiveEnemies );
	//DBGPRINT( xPos, yPos-=offset, "TAR ROT %.2f", m_EnemyList[0]->GetTargetRotation() );

	//renderer::OpenGL::GetInstance()->DisableTextureArray();
}

/////////////////////////////////////////////////////
/// Method: EnemyInLightZone
/// Params: [in]enemy, [in/out]zoneIndex
///
/////////////////////////////////////////////////////
bool EnemyManagement::EnemyInLightZone( Enemy* enemy, int* zoneIndex )
{
	unsigned int i=0;
	bool inArea = false;
	int index = *zoneIndex;

	for( i=0; i < m_LightZones.size(); ++i )
	{
		if( m_LightZones[i]->angle != 0.0f )
		{
			inArea = m_LightZones[i]->obb.OBBCollidesWithSphere( enemy->GetBoundingSphere() );
			index = i;
		}
		else
		{
			inArea = m_LightZones[i]->aabb.AABBCollidesWithSphere( enemy->GetBoundingSphere() );
			index = i;
		}

		if( inArea )
		{
			*zoneIndex = index;
			return(true);
		}
	}

	*zoneIndex = -1;
	return(false);
}

/////////////////////////////////////////////////////
/// Method: FloatingHeadInLightZone
/// Params: [in]enemy, [in/out]zoneIndex
///
/////////////////////////////////////////////////////
bool EnemyManagement::FloatingHeadInLightZone( FloatingHead* enemy, int* zoneIndex )
{
	unsigned int i=0;
	bool inArea = false;
	int index = *zoneIndex;

	for( i=0; i < m_LightZones.size(); ++i )
	{
		if( m_LightZones[i]->angle != 0.0f )
		{
			inArea = m_LightZones[i]->obb.OBBCollidesWithSphere( enemy->GetBoundingSphere() );
			index = i;
		}
		else
		{
			inArea = m_LightZones[i]->aabb.AABBCollidesWithSphere( enemy->GetBoundingSphere() );
			index = i;
		}

		if( inArea )
		{
			*zoneIndex = index;
			return(true);
		}
	}

	*zoneIndex = -1;
	return(false);
}

/////////////////////////////////////////////////////
/// Method: CrawlingSpiderInLightZone
/// Params: [in]enemy, [in/out]zoneIndex
///
/////////////////////////////////////////////////////
bool EnemyManagement::CrawlingSpiderInLightZone( CrawlingSpider* enemy, int* zoneIndex )
{
	unsigned int i=0;
	bool inArea = false;
	int index = *zoneIndex;

	for( i=0; i < m_LightZones.size(); ++i )
	{
		if( m_LightZones[i]->angle != 0.0f )
		{
			inArea = m_LightZones[i]->obb.OBBCollidesWithSphere( enemy->GetBoundingSphere() );
			index = i;
		}
		else
		{
			inArea = m_LightZones[i]->aabb.AABBCollidesWithSphere( enemy->GetBoundingSphere() );
			index = i;
		}

		if( inArea )
		{
			*zoneIndex = index;
			return(true);
		}
	}

	*zoneIndex = -1;
	return(false);
}

/////////////////////////////////////////////////////
/// Method: DrawEnemies
/// Params: [in]pauseFlag
///
/////////////////////////////////////////////////////
void EnemyManagement::DrawEnemies( bool pauseFlag )
{
	std::vector<Enemy* >::iterator it = m_EnemyList.begin();

	math::Vec3 playerPos = m_Player.GetPosition();
	math::Vec3 playerToEnemy(0.0f,0.0f,0.0f);

	while( it != m_EnemyList.end() )
	{
		if( (*it)->GetState() == Enemy::EnemyState_InActive )
		{
			it++;
			continue;
		}

		renderer::OpenGL::GetInstance()->DisableLighting();
			(*it)->DrawGibs();
		renderer::OpenGL::GetInstance()->EnableLighting();

		// next
		it++;
	}

	it = m_EnemyList.begin();
	while( it != m_EnemyList.end() )
	{
		if( (*it)->GetState() == Enemy::EnemyState_InActive )
		{
			it++;
			continue;
		}

		if( (*it)->InFrustum() )
		{
			(*it)->SetupSubmeshes();
			(*it)->UpdateSkins();

			int lightIndex = (*it)->GetLightZoneIndex();
			EnemyInLightZone( (*it), &lightIndex );

			(*it)->SetLightZoneIndex( lightIndex );

			if( lightIndex != -1 )
			{
				renderer::OpenGL::GetInstance()->SetLightPosition( 0, m_LightZones[lightIndex]->dir );
				math::Vec4 diff( m_LightZones[lightIndex]->col[0], m_LightZones[lightIndex]->col[1], m_LightZones[lightIndex]->col[2], 1.0f );
				renderer::OpenGL::GetInstance()->SetLightDiffuse( 0, diff );
				math::Vec4 amb( 0.1f, 0.1f, 0.1f, 1.0f );
				renderer::OpenGL::GetInstance()->SetLightAmbient( 0, amb );
				renderer::OpenGL::GetInstance()->SetLightSpotExponent( 0, 0.0f );
				renderer::OpenGL::GetInstance()->SetLightSpotCutoff( 0, 180.0f );

				if( m_LightZones[lightIndex]->type == 0 )
				{
					renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_CONSTANT_ATTENUATION, 1.0f );
					renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_LINEAR_ATTENUATION, 0.0f );
					renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_QUADRATIC_ATTENUATION, 0.0f );
				}
				else
				{
					renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_CONSTANT_ATTENUATION, 0.0f );
					renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_LINEAR_ATTENUATION, 0.0f );
					renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_QUADRATIC_ATTENUATION, 0.25f );
				}
			}
			else
			{
				math::Vec4 m_LightPos = math::Vec4( 0.0f, 1.0f, 0.0f, 0.0f );
				math::Vec4 m_LightDiffuse = math::Vec4( 1.0f, 1.0f, 1.0f, 1.0f );
				math::Vec4 m_LightAmbient = math::Vec4( 1.0f, 1.0f, 1.0f, 1.0f );
				math::Vec4 m_LightSpecular = math::Vec4( 0.0f, 0.0f, 0.0f, 1.0f );
				renderer::OpenGL::GetInstance()->SetLightPosition( 0, m_LightPos );
				renderer::OpenGL::GetInstance()->SetLightDiffuse( 0, m_LightDiffuse );

				renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_CONSTANT_ATTENUATION, 0.0f );
				renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_LINEAR_ATTENUATION, 0.0f );
				renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_QUADRATIC_ATTENUATION, 0.25f );
			}

			(*it)->Draw();
		}
		// next
		it++;
	}
}

/////////////////////////////////////////////////////
/// Method: DrawFloatingHeads
/// Params: [in]pauseFlag
///
/////////////////////////////////////////////////////
void EnemyManagement::DrawFloatingHeads( bool pauseFlag )
{
	std::vector<FloatingHead* >::iterator it = m_FloatingHeadList.begin();

	math::Vec3 playerPos = m_Player.GetPosition();
	math::Vec3 playerToEnemy(0.0f,0.0f,0.0f);

	renderer::OpenGL::GetInstance()->DisableLighting();

	it = m_FloatingHeadList.begin();
	while( it != m_FloatingHeadList.end() )
	{
		if( (*it)->GetState() == FloatingHead::FloatingHeadState_InActive )
		{
			it++;
			continue;
		}

		if( (*it)->InFrustum() )
		{
			(*it)->UpdateSkins();

			renderer::OpenGL::GetInstance()->SetColour4ub( 200,200,200,196 );
			(*it)->Draw();
		}
		// next
		it++;
	}
	
	renderer::OpenGL::GetInstance()->EnableLighting();
}

/////////////////////////////////////////////////////
/// Method: DrawHangingSpiders
/// Params: [in]pauseFlag
///
/////////////////////////////////////////////////////
void EnemyManagement::DrawHangingSpiders( bool pauseFlag )
{
	std::vector<HangingSpider* >::iterator it = m_HangingSpiderList.begin();

	math::Vec3 playerPos = m_Player.GetPosition();
	math::Vec3 playerToEnemy(0.0f,0.0f,0.0f);

	it = m_HangingSpiderList.begin();
	while( it != m_HangingSpiderList.end() )
	{
		if( (*it)->GetState() == HangingSpider::HangingSpiderState_InActive )
		{
			it++;
			continue;
		}

		if( (*it)->InFrustum() )
		{
			(*it)->UpdateSkins();

			math::Vec4 m_LightPos = math::Vec4( 0.1f, 1.0f, 0.1f, 0.0f );
			math::Vec4 m_LightDiffuse = math::Vec4( 1.0f, 1.0f, 1.0f, 1.0f );

			renderer::OpenGL::GetInstance()->SetLightPosition( 0, m_LightPos );
			renderer::OpenGL::GetInstance()->SetLightDiffuse( 0, m_LightDiffuse );

			math::Vec4 amb( 0.001f, 0.001f, 0.001f, 1.0f );
			renderer::OpenGL::GetInstance()->SetLightAmbient( 0, amb );

			renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_CONSTANT_ATTENUATION, 0.0f );
			renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_LINEAR_ATTENUATION, 0.0f );
			renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_QUADRATIC_ATTENUATION, 0.25f );

			(*it)->Draw();
		}

		// next
		it++;
	}
}

/////////////////////////////////////////////////////
/// Method: DrawCrawlingSpiders
/// Params: [in]pauseFlag
///
/////////////////////////////////////////////////////
void EnemyManagement::DrawCrawlingSpiders( bool pauseFlag )
{
	std::vector<CrawlingSpider* >::iterator it = m_CrawlingSpiderList.begin();

	math::Vec3 playerPos = m_Player.GetPosition();
	math::Vec3 playerToEnemy(0.0f,0.0f,0.0f);
	it = m_CrawlingSpiderList.begin();
	while( it != m_CrawlingSpiderList.end() )
	{
		if( (*it)->GetState() == CrawlingSpider::CrawlingSpiderState_InActive )
		{
			it++;
			continue;
		}

		if( (*it)->InFrustum() )
		{
			(*it)->UpdateSkins();

			int lightIndex = (*it)->GetLightZoneIndex();
			CrawlingSpiderInLightZone( (*it), &lightIndex );

			(*it)->SetLightZoneIndex( lightIndex );

			if( lightIndex != -1 )
			{
				renderer::OpenGL::GetInstance()->SetLightPosition( 0, m_LightZones[lightIndex]->dir );
				math::Vec4 diff( m_LightZones[lightIndex]->col[0], m_LightZones[lightIndex]->col[1], m_LightZones[lightIndex]->col[2], 1.0f );
				renderer::OpenGL::GetInstance()->SetLightDiffuse( 0, diff );
				math::Vec4 amb( 0.001f, 0.001f, 0.001f, 1.0f );
				renderer::OpenGL::GetInstance()->SetLightAmbient( 0, amb );
				renderer::OpenGL::GetInstance()->SetLightSpotExponent( 0, 0.0f );
				renderer::OpenGL::GetInstance()->SetLightSpotCutoff( 0, 180.0f );

				if( m_LightZones[lightIndex]->type == 0 )
				{
					renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_CONSTANT_ATTENUATION, 1.0f );
					renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_LINEAR_ATTENUATION, 0.0f );
					renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_QUADRATIC_ATTENUATION, 0.0f );
				}
				else
				{
					//static float linear = 0.002f;
					//static float quadratic = 0.0005f;
					renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_CONSTANT_ATTENUATION, 0.0f );
					renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_LINEAR_ATTENUATION, 0.0f );
					renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_QUADRATIC_ATTENUATION, 0.25f );
				}
			}
			else
			{
				math::Vec4 m_LightPos = math::Vec4( 0.1f, 1.0f, 0.1f, 0.0f );
				math::Vec4 m_LightDiffuse = math::Vec4( 1.0f, 1.0f, 1.0f, 1.0f );
				math::Vec4 m_LightAmbient = math::Vec4( 1.0f, 1.0f, 1.0f, 1.0f );
				math::Vec4 m_LightSpecular = math::Vec4( 0.0f, 0.0f, 0.0f, 1.0f );
				renderer::OpenGL::GetInstance()->SetLightPosition( 0, m_LightPos );
				renderer::OpenGL::GetInstance()->SetLightDiffuse( 0, m_LightDiffuse );

				math::Vec4 amb( 0.001f, 0.001f, 0.001f, 1.0f );
				renderer::OpenGL::GetInstance()->SetLightAmbient( 0, amb );

				renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_CONSTANT_ATTENUATION, 0.0f );
				renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_LINEAR_ATTENUATION, 0.0f );
				renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_QUADRATIC_ATTENUATION, 0.25f );
			}

			(*it)->Draw();
		}
		// next
		it++;
	}
}

/////////////////////////////////////////////////////
/// Method: DrawInsects
/// Params: [in]pauseFlag
///
/////////////////////////////////////////////////////
void EnemyManagement::DrawInsects( bool pauseFlag )
{
	std::vector<Insects* >::iterator it = m_InsectsList.begin();

	math::Vec3 playerPos = m_Player.GetPosition();
	math::Vec3 playerToEnemy(0.0f,0.0f,0.0f);

	while( it != m_InsectsList.end() )
	{
		// do not draw or animate when outside the frustum
		const collision::Sphere& sphere = (*it)->GetBoundingSphere();
		if( renderer::OpenGL::GetInstance()->SphereInFrustum( sphere.vCenterPoint.X, sphere.vCenterPoint.Y, sphere.vCenterPoint.Z, sphere.fRadius ) )
		{
			(*it)->SetInFrustum( true );
		}
		else
			(*it)->SetInFrustum( false );

		// next
		it++;
	}

	it = m_InsectsList.begin();
	while( it != m_InsectsList.end() )
	{
		if( (*it)->InFrustum() )
		{
			(*it)->Draw();
		}

		// next
		it++;
	}
}

/////////////////////////////////////////////////////
/// Method: UpdateEnemies
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void EnemyManagement::UpdateEnemies( float deltaTime )
{
	std::vector<Enemy* >::iterator itEnemy = m_EnemyList.begin();
	bool pauseFlag = false;
	
	while( itEnemy != m_EnemyList.end() )
	{
		if( (*itEnemy)->GetAddKillFlag() )
		{
			m_NumGameKills++;
			(*itEnemy)->SetAddKillFlag( false );
		}

		if( (*itEnemy)->GetState() == Enemy::EnemyState_InActive )
		{
			itEnemy++;
			continue;
		}

		// enemy is inactive, remove from the list
		/*if( (*itEnemy)->GetState() ==  Enemy::EnemyState_InActive )
		{
			itEnemy = m_EnemyList.erase( itEnemy );
			m_ActiveEnemies--;

			if( itEnemy == m_EnemyList.end() )
				break;
		}*/
		
		(*itEnemy)->SetPlayerPosition( m_Player.GetPosition() );

		(*itEnemy)->Update( deltaTime );
		
		// do not draw or animate when outside the frustun
		const collision::Sphere& sphere = (*itEnemy)->GetBoundingSphere();
		if( renderer::OpenGL::GetInstance()->SphereInFrustum( sphere.vCenterPoint.X, sphere.vCenterPoint.Y, sphere.vCenterPoint.Z, sphere.fRadius ) )
			(*itEnemy)->SetInFrustum( true );
		else
			(*itEnemy)->SetInFrustum( false );		
		
		(*itEnemy)->PreDrawSetup(pauseFlag);
		
		// next enemy
		itEnemy++;
	}
}

/////////////////////////////////////////////////////
/// Method: UpdateFloatingHeads
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void EnemyManagement::UpdateFloatingHeads( float deltaTime )
{
	std::vector<FloatingHead* >::iterator itHead = m_FloatingHeadList.begin();
	int i=0;
	bool pauseFlag = false;
	
	while( itHead != m_FloatingHeadList.end() )
	{
		if( (*itHead)->GetState() == FloatingHead::FloatingHeadState_InActive )
		{
			itHead++;
			continue;
		}
		
		(*itHead)->SetPlayerPosition( m_Player.GetPosition() );

		(*itHead)->Update( deltaTime );
		
		// do not draw or animate when outside the frustun
		const collision::Sphere& sphere = (*itHead)->GetBoundingSphere();
		if( renderer::OpenGL::GetInstance()->SphereInFrustum( sphere.vCenterPoint.X, sphere.vCenterPoint.Y, sphere.vCenterPoint.Z, sphere.fRadius ) )
			(*itHead)->SetInFrustum( true );
		else
			(*itHead)->SetInFrustum( false );
					
		(*itHead)->PreDrawSetup(pauseFlag);

		for( i=0; i<MAX_PLAYER_BULLETS; ++i )
		{
			Bullet* bulletOffset = m_Player.GetBulletIndex(i);

			if( !bulletOffset->IsActive() )
				continue;

			const collision::Sphere bulletSphere = bulletOffset->GetCollision();
			const collision::Sphere headSphere = (*itHead)->GetBoundingSphere();
			
			if( bulletSphere.SphereCollidesWithSphere(headSphere) )
			{
				(*itHead)->SetState( FloatingHead::FloatingHeadState_Dead );
				m_NumGameKills++;
			}
		}

		// next head
		itHead++;
	}
}

/////////////////////////////////////////////////////
/// Method: UpdateHangingSpiders
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void EnemyManagement::UpdateHangingSpiders( float deltaTime )
{
	std::vector<HangingSpider* >::iterator itSpider = m_HangingSpiderList.begin();
	int i=0;
	const float IMPULSE = 0.5f;

	while( itSpider != m_HangingSpiderList.end() )
	{
		if( (*itSpider)->GetState() == HangingSpider::HangingSpiderState_InActive )
		{
			itSpider++;
			continue;
		}
		
		const collision::Sphere spiderSphere = (*itSpider)->GetBoundingSphere();
		const collision::Sphere playerSphere = m_Player.GetBoundingSphere();
		float angle = m_Player.GetRotationAngle();

		(*itSpider)->SetPlayerPosition( m_Player.GetPosition() );

		(*itSpider)->Update( deltaTime );

		// can hurt the player
		if( (*itSpider)->GetState() != HangingSpider::HangingSpiderState_Dead &&
			(*itSpider)->CanHurtPlayer() &&
			playerSphere.SphereCollidesWithSphere(spiderSphere) )
		{
			float angleX = -std::sin( math::DegToRad(angle) );
			float angleZ = -std::cos( math::DegToRad(angle) );

			b2Vec2 impulse( angleX*IMPULSE, angleZ*IMPULSE );

			m_Player.ApplyImpulse( impulse );
			
			int clothingIndex = m_Player.GetClothingIndex();
			if( clothingIndex == 2 )
			{
				// naked does more damage
				m_Player.TakeDamage(HANGINGSPIDER_DAMAGE*2);
			}
			else if( clothingIndex == 4 )
			{
				// hidden tattoo does half damage
				m_Player.TakeDamage(HANGINGSPIDER_DAMAGE/2);
			}
			else
				m_Player.TakeDamage(HANGINGSPIDER_DAMAGE);


			// cool down timer
			(*itSpider)->HurtPlayer();
		}
		
		// normal update
		(*itSpider)->PreDrawSetup(false);
			
		// do not draw or animate when outside the frustun
		const collision::Sphere& sphere = (*itSpider)->GetBoundingSphere();
		if( renderer::OpenGL::GetInstance()->SphereInFrustum( sphere.vCenterPoint.X, sphere.vCenterPoint.Y, sphere.vCenterPoint.Z, sphere.fRadius ) )
		{
			(*itSpider)->SetInFrustum( true );
		}
		else
			(*itSpider)->SetInFrustum( false );	
		
		for( i=0; i<MAX_PLAYER_BULLETS; ++i )
		{
			Bullet* bulletOffset = m_Player.GetBulletIndex(i);

			if( !bulletOffset->IsActive() )
				continue;

			const collision::Sphere bulletSphere = bulletOffset->GetCollision();
			
			if( bulletSphere.SphereCollidesWithSphere(spiderSphere) )
			{
				int value = (*itSpider)->GetHealth();
				Bullet::BulletSource type = bulletOffset->GetBulletSource();

				if( type == Bullet::BULLET_PISTOL )
					value -= 1;
				else if( type == Bullet::BULLET_SHOTGUN )
					value -= 3;

				(*itSpider)->SetHealth(value);

				if( value <= 0 )
				{
					(*itSpider)->SetState( HangingSpider::HangingSpiderState_Dead );
					m_NumGameKills++;
				}
				else
					(*itSpider)->SpawnBlood();
			}
		}

		// next head
		itSpider++;
	}
}

/////////////////////////////////////////////////////
/// Method: UpdateCrawlingSpiders
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void EnemyManagement::UpdateCrawlingSpiders( float deltaTime )
{
	std::vector<CrawlingSpider* >::iterator itSpider = m_CrawlingSpiderList.begin();
	int i=0;
	bool pauseFlag = false;
	
	while( itSpider != m_CrawlingSpiderList.end() )
	{
		if( (*itSpider)->GetState() == CrawlingSpider::CrawlingSpiderState_InActive )
		{
			itSpider++;
			continue;
		}
		
		(*itSpider)->SetPlayerPosition( m_Player.GetPosition() );

		(*itSpider)->Update( deltaTime );
			
		// do not draw or animate when outside the frustun
		const collision::Sphere& sphere = (*itSpider)->GetBoundingSphere();
		if( renderer::OpenGL::GetInstance()->SphereInFrustum( sphere.vCenterPoint.X, sphere.vCenterPoint.Y, sphere.vCenterPoint.Z, sphere.fRadius ) )
			(*itSpider)->SetInFrustum( true );
		else
			(*itSpider)->SetInFrustum( false );
				
		(*itSpider)->PreDrawSetup(pauseFlag);

		for( i=0; i<MAX_PLAYER_BULLETS; ++i )
		{
			Bullet* bulletOffset = m_Player.GetBulletIndex(i);

			if( !bulletOffset->IsActive() )
				continue;

			const collision::Sphere bulletSphere = bulletOffset->GetCollision();
			const collision::Sphere headSphere = (*itSpider)->GetBoundingSphere();
			
			if( bulletSphere.SphereCollidesWithSphere(headSphere) )
			{
				(*itSpider)->SetState( CrawlingSpider::CrawlingSpiderState_Dead );
				m_NumGameKills++;
			}
		}
		
		// next spider
		itSpider++;
	}
}

/////////////////////////////////////////////////////
/// Method: UpdateInsects
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void EnemyManagement::UpdateInsects( float deltaTime )
{
	std::vector<Insects* >::iterator itInsect = m_InsectsList.begin();

	while( itInsect != m_InsectsList.end() )
	{
		//(*itInsects)->SetPlayerPosition( m_Player.GetPosition() );

		(*itInsect)->Update( deltaTime );

		// next insect
		itInsect++;
	}
}
