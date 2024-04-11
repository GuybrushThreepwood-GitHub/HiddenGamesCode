
/*===================================================================
	File: Level.h
	Game: H4

	(C)Hidden Games
=====================================================================*/

#ifndef __LEVEL_H__
#define __LEVEL_H__

#include <vector>
#include "SoundBase.h"

#include "Effects/Emitter.h"

#include "Physics/PhysicsContact.h"
#include "Physics/PhysicsIds.h"

#include "GameObjects/BaseObject.h"
#include "GameObjects/UseArea.h"
#include "GameObjects/Door.h"
#include "GameObjects/RoomInstance.h"
#include "GameObjects/AnimatedMesh.h"

class Level : public physics::PhysicsIdentifier
{
	public:
		struct LightZone
		{
			math::Vec3 pos;
			float angle;
			math::Vec3 dims;
			int type;
			math::Vec4 dir;
			float col[4];
				
			collision::AABB aabb;
			collision::OBB obb;
		};

		struct StepAudio
		{
			int type;

			math::Vec3 pos;
			float angle;
			math::Vec3 dims;
				
			collision::AABB aabb;
			collision::OBB obb;
		};

		enum InstanceTypes
		{
			TYPE_LIMITEDINSTANCE=0,
			
			TYPE_EXT_EMPTY,
			TYPE_EXT_AMMO,
			TYPE_EXT_HEALTH,
			TYPE_EXT_ZOMBIE,
			TYPE_EXT_LOCKED
		};

		struct Instance
		{
			bool used;
			math::Vec3 pos;
			float angle;		
			math::Vec3 scale;
		};

		struct SpriteVert
		{
			math::Vec3 v;
			math::Vec2 uv;
			math::Vec4Lite col;
		};

		struct SpriteType
		{
			int Id;
			math::Vec3 pos;
			float angle;
			int type;
			float w,h,d;
			math::Vec4Lite col;
			float sceneDepth;
		};

		struct SoundType
		{
			ALuint sourceId;
			ALuint bufferId;

			float gainControl;

			int type;
			float time;
			float lastTimeUpdate;

			math::Vec3 pos;
			math::Vec3 offset;
			float angle;

			float pitch;
			float gain;

			bool gainAtt;
			bool srcRel;
			bool loop;

			int areaType;
			collision::AABB aabb;
			collision::OBB obb;
			collision::Sphere sphere;
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

		/// LoadMeshData - loads the main hgm of the level
		int LoadMeshData( const char *szFilename, GLenum magFilter, GLenum minFilter );
		/// LoadPhysicsB2DData - loads a b2d file containing physics data
		int LoadPhysicsB2DData( const char *szFilename, const math::Vec3& pos, float angle, b2World* pWorld );
		/// LoadTypes - loads the dat file containing the h4 types
		int LoadTypes( const char *szFilename, bool inRoomInstance, RoomInstance* roomInstance, const math::Vec3& pos, float angle );

		/// LoadSkybox
		int LoadSkybox( int index );

		/// CallOnCreate - all BaseObjects get their OnCreate method called
		void CallOnCreate();

		/// DrawSkybox - Draws the optional skybox
		void DrawSkybox( );
		/// DrawOpaque - Draws the level
		void DrawOpaque( bool disableMeshDraw = false );
		/// DrawSorted - Draws the level
		void DrawSorted( bool disableMeshDraw = false );

		/// DrawEmitters - Draws the emitters
		void DrawEmitters( );

		/// DrawPhysicsObjects - Draws all the physics placed objects
		void DrawPhysicsObjects();

		/// Draw - Draws debug geometry
		void DrawDebug();

		/// Update - Updates a model
		/// \param deltaTime - time since last update
		void Update( float deltaTime );

		/// UpdatePhysicsObjects - Updates all the physics placed objects
		void UpdatePhysicsObjects( float deltaTime );

		/// UpdateAudio
		void UpdateAudio( collision::Sphere& player, float deltaTime );

		void ClearPhysicsObjects();
		void ResetPhysicsObjects();

		// SetExtendedRoomLimits
		void SetExtendedRoomLimits( unsigned int roomMaxEmpty, unsigned int roomMaxAmmo, unsigned int roomMaxHealth, unsigned int roomMaxZombie );

		void SetPlayerSpawnId( int playerSpawnId )				{ m_PlayerSpawnId = playerSpawnId; }

		int FindLimitedInstance();
		int FindExtendedInstance( int roomType );

		void AddLimitedInstanceRoom( const char* modelFile, const char* b2dFile, const char* typesFile );
		void AddExtendedInstanceRoom( int roomType, const char* modelFile, const char* b2dFile, const char* typesFile );

		math::Vec2& GetPlayerStart()							{ return(m_PlayerStartPoint); }

		const std::vector<BaseObject* >& GetGameObjectList()	{ return m_GameObjects; }
		const std::vector<UseArea* >& GetUseAreaList()			{ return m_UseAreas; }
		const std::vector<LightZone* >&	GetLightZoneList()		{ return m_LightZones; }
		const std::vector<StepAudio* >&	GetStepAudioZoneList()	{ return m_StepAudioZones; }

	private:
		bool SetAudioSourceData( SoundType& audioLocator );

	private:
		float m_LastDelta;
		bool m_FirstUpdate;
		mdl::ModelHGM* m_pMeshData;
		mdl::ModelHGM* m_pSkyData;
		b2Body* m_pBody;
		physics::PhysicsIdentifier* m_pGenericPhysicsId;

		GLenum m_MagFilter;
		GLenum m_MinFilter;

		int m_PlayerSpawnId;
		math::Vec2 m_PlayerStartPoint;

		std::vector<BaseObject *> m_GameObjects;
		std::vector<UseArea *> m_UseAreas;
		std::vector<Door *> m_Doors;
		std::vector<LightZone *> m_LightZones;
		std::vector<AnimatedMesh *> m_AnimatedMeshes;
		std::vector<StepAudio *> m_StepAudioZones;

		GLuint m_SpriteTextureId;
		std::vector<SpriteType *> m_SpriteData;
		unsigned int m_SpriteCount;
		SpriteVert* m_SpriteList;

		std::vector<SoundType *> m_SoundLocations;

		std::vector<Instance *> m_LimitedInstances;
		std::vector<Instance *> m_ExtendedInstances;

		std::vector<RoomInstance *> m_LimitedRoomInstances;
		std::vector<RoomInstance *> m_ExtendedRoomInstances;

		unsigned int m_TotalEmptyRooms;
		unsigned int m_MaxEmptyRooms;

		unsigned int m_TotalAmmoRooms;
		unsigned int m_MaxAmmoRooms;

		unsigned int m_TotalHealthRooms;
		unsigned int m_MaxHealthRooms;

		unsigned int m_TotalZombieRooms;
		unsigned int m_MaxZombieRooms;

#ifdef ODE_TESTING
		struct TriMesh
		{
			int numTriangles;
			int* pIndices;
			math::Vec3* pPoints;

			dTriMeshDataID triMeshDataID;
		};

		dSpaceID m_EntitySpace;

		std::vector<TriMesh *> m_TriMeshList;
		std::vector<dGeomID> m_ODEGeoms;
#endif // ODE_TESTING

		std::vector<physics::PhysicsIdentifier *> m_PhysicsIdList;
		std::vector<efx::Emitter *> m_EmitterList;
};

#endif // __LEVEL_H__


