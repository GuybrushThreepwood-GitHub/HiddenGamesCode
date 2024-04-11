
/*===================================================================
	File: Level.h
	Game: AirCadets

	(C)Hidden Games
=====================================================================*/

#ifndef __LEVEL_H__
#define __LEVEL_H__

#include <vector>
#include "PhysicsBase.h"

#include "H1Consts.h"
#include "Resources/PhysicsResources.h"
#include "Resources/SpriteResources.h"

#include "Effects/Emitter.h"
#include "Effects/WaveGrid.h"
#include "Effects/BatchSpriteRender.h"

#include "GameObjects/WorldObject.h"
#include "ScriptAccess/H1/H1Access.h"
#include "ScriptAccess/ScriptDataHolder.h"
#include "Level/LevelTypes.h"


// forward declare
namespace mdl { class ModelHGM; }
class Player;
class ScriptDataHolder;

const int MAX_OBJECT_BATCHES	= 32;
const int MAX_PER_BATCH			= 64;
const int MAX_TARGET_DRAWS		= 10;
const float HEIGHTMAP_SCALE		= 16000.0f;

class Level : public physics::PhysicsIdentifier
{
	public:
		enum 
		{
			LEVEL_DAY=0,
			LEVEL_AFTERNOON,
			LEVEL_NIGHT,
		};

		enum eAudioTrigger
		{
			eAudioTrigger_Box=0,
			eAudioTrigger_Sphere
		};

		enum eAudioType
		{
			eAudioType_Normal=0,
			eAudioType_Timed
		};

		struct SpawnPoint
		{
			math::Vec3 pos;
			math::Vec3 rot;
			int spawnType;
			int spawnGroup;
		};

		struct Gate
		{
			bool enabled;

			math::Vec3 pos;
			math::Vec3 rot;
			dMatrix3 orientation;

			int type;
			mdl::ModelHGM* model;
			int groupId;
			int number;
			int alwaysDraw;
			int collisionType;

			collision::OBB obb;
			collision::Sphere s;

			int achievementId;
			int textId;
		};

		struct GateGroup
		{
			bool enabled;

			math::Vec3 spawnPos;
			math::Vec3 spawnRot;

			int groupId;
			int numGates;
			Gate** gateList;
			int nextGateIndex;
			int highestGateNum;
		};

		struct AudioLocator
		{
			ALuint sourceId;
			ALuint bufferId;

			int triggerType;

			math::Vec3 pos;
			math::Vec3 offset;

			float pitch;
			float gain;
			float gainControl;
			bool loop;
			bool relative;

			int type;
			float time;
			float radius;
			collision::AABB aabb;
			collision::Sphere sphere;
			int gainAttenuation;

			float lastTimeUpdate;
		};

		struct SpriteVert
		{
			math::Vec3 v;
			math::Vec2 uv;
			math::Vec4Lite col;
		};

		struct TargetData
		{
			bool inUse;
			math::Vec3 unprojPoint;
			float distance;
			WorldObject* pObj;
		};

		struct TokenPickup
		{
			int tokenId;
			bool collected;

			collision::Sphere s;
			mdl::ModelHGM* pModel;
		};

	public:
		/// default constructor
		Level();
		/// default destructor
		virtual ~Level();
			
		/// Initialise - Initialise the models data
		void Initialise( void );
		/// Release - Frees all the models data
		void Release( void );

		int LoadMeshData( int meshResId, bool useAABBCull );

		int LoadComplexPhysicsData( const char *szFilename );
		int LoadData( const char* szFilename, bool targetMode, int whichResId, int objRes1, int objRes2, int objRes3 );
		
		int LoadMappingData( const char* szCoverageData, const spriteBatchData* spriteBatch, int spriteBatchCount );

		void DetailMapSetup( int textureResId, float scaleX, float scaleY );
		void SetupSkybox( int textureResId, bool useFog, const math::Vec3& offset );
		void SetTimeOfDay( int timeOfDay );
		void SetGateGroup( int groupId );
		void SetPlayer( Player* pPlayer );
		void SetTargetObjectId( int resId );

		// 
		void DrawMainLevel();
		void DrawPickups();
		void DrawSpriteAndEmitters();
		void DrawSkybox();
		void DrawGates();

		/// DrawPhysicsObjects - Draws all the physics placed objects
		void DrawPhysicsObjects();

		/// Draw - Draws debug geometry
		void DrawDebug();

		/// Update - Updates a model
		/// \param deltaTime - time since last update
		void Update( float deltaTime );

		/// UpdatePhysicsObjects - Updates all the physics placed objects
		void UpdatePhysicsObjects( float deltaTime );

		void CreatePhysics();
		void ResetTokens();
		void ResetGateGroup(bool clearAll);
		void ResetTargets();
		void ResetAudio();

		// player requests
		GateGroup* GetActiveGateGroup()
		{
			if( m_ActiveGateGroup != -1 )
			{
				if( m_ActiveGateGroup < static_cast<int>( m_GateGroups.size() ) )
					return &m_GateGroups[m_ActiveGateGroup];
			}

			return 0;
		}
		
		std::vector<Gate>* GetSights()
		{
			return &m_Sights;
		}

		void GetRandomSpawn( math::Vec3* pos, math::Vec3* rot );
		void GetNearestSpawn( const math::Vec3& currentPos, math::Vec3* pos, math::Vec3* rot );

		const math::Vec3& GetPlayerStart()		{ return m_PlayerStartPoint; }
		const math::Vec3& GetPlayerStartRot()	{ return m_PlayerSpawnRot; }

		const TargetData* GetTargetData()		{ return &m_TargetData[0]; }
		int GetUsedTargets()					{ return m_NumTargetsUsed; }
		int GetTotalTargets()					{ return m_TotalNumTargets; }
		int GetTotalDestroyable()				{ return m_TotalDestroyable; }

		bool TokenCollectedLastFrame()			{ return m_TokenWasCollected; }
		bool AllCadetTokensCollected()			{ return m_AllCadetsCollected; }

		void GetCollectedTokenCounts( int* taku, int* mei, int* earl, int* fio, int* mito, int* uki )
		{
			*taku = m_TakuCollectedCount;
			*mei = m_MeiCollectedCount;
			*earl = m_EarlCollectedCount;
			*fio = m_FioCollectedCount;
			*mito = m_MitoCollectedCount;
			*uki = m_UkiCollectedCount;
		}

	private:
		dGeomID CreateGeom( int shapeClass, dSpaceID spaceId, const math::Vec3& dims, float radius=0.0f, float length=0.0f, dTriMeshDataID triMeshDataId=0 );
		void UpdateAudio( float deltaTime );
		bool SetAudioSourceData( AudioLocator* pAudioLocator );

		math::Vec3 ParsePosition( int tableIndex );

		void ParsePosRot( int paramIndex, int baseType, int baseId );
		void ParseSpawnPoint( int paramIndex, int baseType, int baseId );
		void ParseGate( int paramIndex, int baseType, int baseId );
		void ParseTarget( int paramIndex, int baseType, int baseId );
		void ParseObject( int paramIndex, int baseType, int baseId, bool targetMode, int whichResId, int objRes1, int objRes2, int objRes3 );
		void ParseSprite( int paramIndex, int baseType, int baseId );
		void ParseEmitter( int paramIndex, int baseType, int baseId );
		void ParseSound( int paramIndex, int baseType, int baseId );

		int GetSpawnPointForGroup( int groupId );
		void SetupGateGroups();
		void UpdateGates( float deltaTime );
		WorldObject* AddObject( int batchId, const collision::AABB& batchAABB, const math::Vec3& pos, const math::Vec3& rot, const math::Vec3& normal, int worldType, int resId, int groupId, bool snapToGrid, bool orientateToGrid );

	private:
		class TerrainTriMesh : public physics::PhysicsIdentifier
		{
			public:
				TerrainTriMesh()
				{
					geomId = 0;
					bodyId = 0;

					index = 0;

					numeric1 = -9999;
					numeric2 = -9999;
					numeric3 = -9999;
					numeric4 = -9999;
					numeric5 = -9999;

					pos.setZero();
					rot.setZero();

					totalMeshTriangles = 0;

					triMeshDataID = 0;
					triangleData = 0;
					indices = 0;
					normals = 0;

					meshAABB.Reset();
				}

			public:
				dGeomID geomId;
				dBodyID bodyId;

				int index;

				int numeric1;
				int numeric2;
				int numeric3;
				int numeric4;
				int numeric5;

				math::Vec3 pos;
				math::Vec3 rot;

				unsigned int totalMeshTriangles;

				dTriMeshDataID triMeshDataID;
				math::Vec3* triangleData;
				int* indices;
				math::Vec3* normals;
				collision::AABB meshAABB;
		};

		GameData m_GameData;
		ScriptDataHolder::DevScriptData m_DevData;

		collision::AABB m_AABB;
		Player* m_pPlayer;

		mdl::ModelHGM* m_pMeshData;
		res::PhysicsBlock* m_PhysicsData;

		int m_TotalTriMeshes;
		TerrainTriMesh* m_TriMeshData;

		// heightmap
		renderer::Texture m_Coverage;

		unsigned long m_CategoryBits;
		unsigned long m_CollideBits;

		dSpaceID m_EntitySpace;
		math::Vec3 m_Pos;
		math::Vec3 m_PosCOM;
		math::Vec3 m_Rot;
		math::Vec3 m_COM;

		int m_TimeOfDay;

		math::Vec3 m_PlayerStartPoint;
		math::Vec3 m_PlayerSpawnRot;

		std::vector<physics::PhysicsIdentifier *> m_PhysicsIdList;
		std::vector<efx::Emitter *> m_EmitterList;

		std::vector<SpawnPoint> m_SpawnPoints;
		std::vector<Gate> m_Gates;
		std::vector<Gate> m_Sights;
		std::vector<GateGroup> m_GateGroups;
		std::vector<AudioLocator *> m_SoundAreas;

		int m_NumObjects;
		struct ObjectBatch
		{
			int batchId;

			collision::AABB objBatchAABB;
			int numObjects;

			WorldObject objectList[MAX_PER_BATCH];
		};
		int m_NumBatchesInUse;
		ObjectBatch m_ObjectBatchList[MAX_OBJECT_BATCHES];

		// Active gateGroup
		int m_ActiveGateGroup;
		math::Vec3 m_PlayerRespawnPoint;
		math::Vec3 m_PlayerRespawnRotation;

		// sprites
		GLuint m_SpriteTextureId;
		std::vector<SpriteType *> m_SpriteData;
		int m_SpriteCount;
		SpriteVert* m_SpriteList;

		BatchSpriteRender m_BatchSprites;

		int m_TotalDestroyable;

		// targets
		int m_NumTargetsUsed;
		int m_TargetObjId;
		int m_TotalNumTargets;
		int m_NextTargetIndex;
		TargetData m_TargetData[MAX_TARGET_DRAWS];

		// tokens
		int m_TotalCadetTokens;
		int m_TotalTakuTokens;
		int m_TotalMeiTokens;
		int m_TotalEarlTokens;
		int m_TotalFioTokens;
		int m_TotalMitoTokens;
		int m_TotalUkiTokens;

		int m_AllCadetTokensCollectedCount;
		int m_TakuCollectedCount;
		int m_MeiCollectedCount;
		int m_EarlCollectedCount;
		int m_FioCollectedCount;
		int m_MitoCollectedCount;
		int m_UkiCollectedCount;

		int m_StarCollectedCount;
		int m_HiddenTokenCollectedCount;

		TokenPickup m_CadetTokens[TOTAL_CADETS*TOTAL_TOKENS_PER_CADET];

		int m_TotalStarTokens;
		TokenPickup m_StarTokens[STAR_TOKENS];

		int m_TotalHiddenTokens;
		TokenPickup m_HiddenTokens[HIDDEN_TOKENS];

		float m_TokenRotation;
		bool m_TokenWasCollected;
		bool m_AllCadetsCollected;

		// BG
		GLuint m_SkyTextureId;
		bool m_SkyUseFog;
		mdl::ModelHGM* m_pSkyData;
		math::Vec3 m_SkyboxOffset;
		float m_SkyRot;

		// detail mapping
		GLuint m_DetailMapTexture;
		float m_DetailMapScaleX;
		float m_DetailMapScaleY;

		// audio
		ALuint m_TokenCollect;
		ALuint m_StarCollect;
		ALuint m_HiddenIconCollect;
};

#endif // __LEVEL_H__


