
/*===================================================================
	File: Level.h
	Game: Taxi!

	(C)Hidden Games
=====================================================================*/

#ifndef __LEVEL_H__
#define __LEVEL_H__

#include <vector>

#include "Box2D.h"
#include "Physics/PhysicsWorld.h"
#include "Effects/Emitter.h"
#include "Effects/WaveGrid.h"
#include "Audio/Sfx.h"

#include "Physics/PhysicsIds.h"

#include "Physics/Objects/Port.h"

#include "Physics/Objects/BaseShape.h"
#include "Physics/Objects/BaseDoor.h"
#include "Physics/Enemies/BaseEnemy.h"

#include "ScriptAccess/Cabby/CabbyAccess.h"
#include "Level/LevelTypes.h"

// forward declare
namespace mdl { class ModelHGM; }

const int BG_QUAD_POINTS = 4;

class Level : public physics::PhysicsIdentifier
{
	public:
		enum 
		{
			LEVEL_DAY=0,
			LEVEL_AFTERNOON,
			LEVEL_NIGHT,
		};

		enum
		{
			ROTATING_HSHAPE,
			ROTATING_ISHAPE,
			ROTATING_USHAPE,
			ROTATING_XSHAPE,
			ROTATING_YSHAPE,
		};

		enum
		{
			MOVING_RIGHT=0,
			MOVING_LEFT,
			MOVING_UP,
			MOVING_DOWN
		};

		enum eGravityType
		{
			eGravityType_Air=0,
			eGravityType_Vacuum,
			eGravityType_Water,

			eGravityType_UnKnown=999
		};

		enum ePortType
		{
			/// must match BasePort order
			ePortType_LandingZone=0,
			ePortType_HQ,
			ePortType_Refuel,
			ePortType_Toll,

			ePortType_UnKnown=999
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

		struct ForceArea
		{
			math::Vec3 pos;
			math::Vec2 dim;
			math::Vec2 force;
			float forceTimeOn;
			float forceTimeOff;
		};

		struct GravityArea
		{
			math::Vec3 pos;
			math::Vec2 dim;
			eGravityType type;
		};

		struct PortArea
		{
			math::Vec3 pos;
			math::Vec2 dim;
			ePortType type;
			int numeric;
		};

		struct StarLocator
		{
			math::Vec3 pos;
			math::Vec3 rot;
			collision::Sphere s;
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

		struct SpriteType
		{
			int resId;
			int baseId;
			math::Vec3 pos;
			float angle;
			int type;
			float w,h,d;
			math::Vec4Lite col;
			float sceneDepth;
		};

		struct WaterGrid
		{
			math::Vec3 pos;
			WaveGrid* grid;
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

		int LoadMeshData( int meshResId );
		int LoadBackgroundModel( int meshResId );
		int LoadForegroundModel( int meshResId );

		int LoadComplexPhysicsData( const char *szFilename, b2World* pWorld );
		int LoadData( const char* szFilename );

		void DetailMapSetup(int textureResId, float scaleX, float scaleY);
		void SetupBGQuad( int textureResId, bool useFog, const math::Vec2& dims, const math::Vec3& pos );
		void SetTimeOfDay( int timeOfDay );

		void SetupStarLocation();
		void SetCustomerMaxSpawn( int maxSpawn )	{ m_CustomerMaxSpawn = maxSpawn; }
		int GetCustomerMaxSpawn()					{ return m_CustomerMaxSpawn; }
		bool WasStarPickedUp()						{ return m_StarPickedUp; }
		// 
		void DrawBackground();
		void DrawForeground();
		void DrawMainLevel();
		void DrawWater();
		void DrawPorts();
		void DrawEmitters();
		void DrawBGQuad();

		/// DrawPhysicsObjects - Draws all the physics placed objects
		void DrawPhysicsObjects();

		/// Draw - Draws debug geometry
		void DrawDebug();

		/// Update - Updates a model
		/// \param deltaTime - time since last update
		void Update( float deltaTime );

		/// UpdatePhysicsObjects - Updates all the physics placed objects
		void UpdatePhysicsObjects( float deltaTime );

		void ClearPhysicsObjects();
		void ResetPhysicsObjects();
		void ResetStarPickup();
		void ResetAudio();

		std::vector<PortArea>& GetPortAreaList()
		{
			return(m_PortAreas);
		}
		std::vector<GravityArea>& GetGravityAreaList()
		{
			return(m_GravityAreas);
		}
		std::vector<ForceArea>& GetForceAreaList()
		{
			return(m_ForceAreas);
		}
		std::vector<WaterGrid>& GetWaterGridList()
		{
			return(m_Water);
		}

		math::Vec2& GetPlayerStart()
		{
			return(m_PlayerStartPoint);
		}
		int GetPlayerStartDirection()
		{
			return(m_PlayerSpawnDir);
		}

	private:
		void UpdateAudio( float deltaTime );
		bool SetAudioSourceData( AudioLocator* pAudioLocator );

		math::Vec3 ParsePosition( int tableIndex );

		void ParsePosRot( int tableIndex, int baseType, int baseId );
		void ParseAnimatedMesh( int tableIndex, int baseType, int baseId );
		void ParseSprite( int tableIndex, int baseType, int baseId );
		void ParseEmitter( int tableIndex, int baseType, int baseId );
		void ParseSound( int tableIndex, int baseType, int baseId );
		void ParseEnemySpawn( int tableIndex, int baseType, int baseId );
		void ParseRotatingShape( int tableIndex, int baseType, int baseId );
		void ParseMovingShape( int tableIndex, int baseType, int baseId );
		void ParsePort( int tableIndex, int baseType, int baseId );
		void ParseGravityArea( int tableIndex, int baseType, int baseId );
		void ParseForceArea( int tableIndex, int baseType, int baseId );
		void ParseWaterGrid( int tableIndex, int baseType, int baseId );

	private:
		GameData m_GameData;

		mdl::ModelHGM* m_pMeshData;
		mdl::ModelHGM* m_pBackground;
		mdl::ModelHGM* m_pForeground;

		int m_TimeOfDay;

		// star
		StarLocator m_StarLocation;
		bool m_StarPickedUp;
		float m_StarRotation;
		mdl::ModelHGM* m_pArcadeStar;
		ALuint m_StarPickupId;
		efx::Emitter* m_pStarFX;

		int m_CustomerMaxSpawn;
		math::Vec2 m_PlayerStartPoint;
		int m_PlayerSpawnDir;

		std::vector<PhysicsIdentifier *> m_PhysicsIdList;
		std::vector<efx::Emitter *> m_EmitterList;

		std::vector<PortArea> m_PortAreas;
		std::vector<GravityArea> m_GravityAreas;
		std::vector<ForceArea> m_ForceAreas;
		std::vector<StarLocator> m_StarLocators;
		
		std::vector<WaterGrid> m_Water;
		std::vector<AudioLocator *> m_SoundAreas;

		std::vector<BasePort *> m_PortList;
		std::vector<BaseShape *> m_ShapeList;
		std::vector<BaseDoor *> m_DoorSlideList;

		std::vector<BaseEnemy *> m_EnemyList;

		// sprites
		GLuint m_SpriteTextureId;
		std::vector<SpriteType *> m_SpriteData;
		int m_SpriteCount;
		SpriteVert* m_SpriteList;

		// grid water
		GLuint m_WaterTextureId;

		// BG
		GLuint m_BGTextureId;
		bool m_BGUseFog;
		math::Vec3 m_BGPos;
		math::Vec2 m_BGDims;
		math::Vec3 m_BGPoints[BG_QUAD_POINTS];
		math::Vec2 m_BGUVCoords[BG_QUAD_POINTS];
		math::Vec4Lite m_BGColours[BG_QUAD_POINTS];

		// detail mapping
		GLuint m_DetailMapTexture;
		float m_DetailMapScaleX;
		float m_DetailMapScaleY;
};

#endif // __LEVEL_H__


