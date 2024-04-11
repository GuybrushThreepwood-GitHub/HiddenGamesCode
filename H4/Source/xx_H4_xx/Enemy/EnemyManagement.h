
/*===================================================================
	File: EnemyManagement.h
	Game: H4

	(C)Hidden Games
=====================================================================*/

#ifndef __ENEMYMANAGEMENT_H__
#define __ENEMYMANAGEMENT_H__

#include "Enemy/Enemy.h"
#include "Enemy/FloatingHead.h"
#include "Enemy/HangingSpider.h"
#include "Enemy/CrawlingSpider.h"
#include "Enemy/Insects.h"

class EnemyManagement
{
	public:	
		struct EnemySpawn
		{
			EnemySpawn()
			{
				spawnId = -1;
				enemyPlaced = false;
			}

			int spawnId;

			math::Vec3 pos;
			float angle;

			bool enemyPlaced;
		};

		struct ConstantSpawn
		{
			ConstantSpawn()
			{
				spawnId = -1;
				enabled = false;
				cooldownTimer = 0.0f;
			}

			int spawnId;

			bool enabled;
			math::Vec3 pos;
			float angle;

			float cooldownTimer;
		};

	public:
		EnemyManagement( Player& player );
		~EnemyManagement();

		void SetupLevelData( Level& activeLevel, NavigationMesh* navMesh );

		void Draw( bool pauseFlag );
		void Draw2D();
		void Update( float deltaTime );

		void DrawDebug();
		void DrawDebug2D();
		
		void CreateLevelEnemies( int maxEnemies );
		void CreateLevelFloatingHeads( int maxHeads );
		void CreateLevelHangingSpiders( int maxSpiders );
		void CreateLevelCrawlingSpiders( int maxSpiders );
		void CreateLevelInsects( int maxInsects );

		Enemy* GetEnemy(int idx)								{return m_EnemyList[idx];}

		void ClearLevelData();
		void AddEnemySpawn( EnemySpawn& enemySpawn )			{m_EnemySpawns.push_back(enemySpawn);}
		void AddConstantSpawn( ConstantSpawn& constantSpawn )	{m_ConstantSpawns.push_back(constantSpawn);}
		void AddFloatingHeadSpawn( EnemySpawn& enemySpawn )		{m_FloatingHeadSpawns.push_back(enemySpawn);}
		void AddHangingSpiderSpawn( EnemySpawn& enemySpawn )	{m_HangingSpiderSpawns.push_back(enemySpawn);}
		void AddCrawlingSpiderSpawn( EnemySpawn& enemySpawn )	{m_CrawlingSpiderSpawns.push_back(enemySpawn);}
		void AddInsectSpawn( EnemySpawn& enemySpawn )			{m_InsectSpawns.push_back(enemySpawn);}

		void SetNumGameKills(int kills)							{m_NumGameKills=kills;}
		int GetNumGameKills()									{return m_NumGameKills;}

	private:
		bool CreateEnemy( GLuint skinIndex );
		bool CreateFloatingHead( int skinIndex );
		bool CreateHangingSpider( );
		bool CreateCrawlingSpider( );
		bool CreateInsect( );

		bool EnemyInLightZone( Enemy* enemy, int* zoneIndex );
		bool FloatingHeadInLightZone( FloatingHead* enemy, int* zoneIndex );
		bool CrawlingSpiderInLightZone( CrawlingSpider* enemy, int* zoneIndex );

		void DrawEnemies( bool pauseFlag );
		void DrawFloatingHeads( bool pauseFlag );
		void DrawHangingSpiders( bool pauseFlag );
		void DrawCrawlingSpiders( bool pauseFlag );
		void DrawInsects( bool pauseFlag );

		void UpdateEnemies( float deltaTime );
		void UpdateFloatingHeads( float deltaTime );
		void UpdateHangingSpiders( float deltaTime );
		void UpdateCrawlingSpiders( float deltaTime );
		void UpdateInsects( float deltaTime );

	private:
		int m_ActiveEnemies;
		int m_MaxActiveEnemies;

		int m_NumGameKills;
		
		float m_LastDeltaTime;

		Player& m_Player;
		Level* m_Level;
		NavigationMesh* m_NavMesh; 

		/// enemy models
		mdl::ModelHGA* m_pEnemyModelData;
		mdl::ModelHGM* m_pSmallGibData;
		mdl::ModelHGM* m_pLargeGibData;

		// head
		mdl::ModelHGA* m_pHeadModelData;

		// hanging spider
		mdl::ModelHGA* m_pHangingSpiderModelData;

		// crawling spider
		mdl::ModelHGA* m_pCrawlingSpiderModelData;

		/// storage for the GL state
		renderer::TGLDepthBlendAlphaState m_DBAState;

		std::vector<EnemySpawn> m_EnemySpawns;
		std::vector<ConstantSpawn> m_ConstantSpawns;
		std::vector<EnemySpawn> m_FloatingHeadSpawns;
		std::vector<EnemySpawn> m_HangingSpiderSpawns;
		std::vector<EnemySpawn> m_CrawlingSpiderSpawns;
		std::vector<EnemySpawn> m_InsectSpawns;

		std::vector<Enemy* > m_EnemyList;
		std::vector<FloatingHead* > m_FloatingHeadList;
		std::vector<HangingSpider* > m_HangingSpiderList;
		std::vector<CrawlingSpider* > m_CrawlingSpiderList;
		std::vector<Insects* > m_InsectsList;

		std::vector<Level::LightZone *> m_LightZones;
};

#endif // __ENEMYMANAGEMENT_H__
