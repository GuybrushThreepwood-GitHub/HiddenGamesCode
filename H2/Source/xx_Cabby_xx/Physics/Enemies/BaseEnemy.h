
/*===================================================================
	File: BaseEnemy.h
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#ifndef __BASEENEMY_H__
#define __BASEENEMY_H__

#include "Physics/PhysicsIds.h"

class BaseEnemy
{
	public:
		enum
		{
			//ENEMYTYPE_DUSTDEVIL=0,
			//ENEMYTYPE_PIRANHA,
			//ENEMYTYPE_RAT,
			//ENEMYTYPE_BOUNCINGBOMB,
			//ENEMYTYPE_BOMB,
			//ENEMYTYPE_PUFFERFISH,
			ENEMYTYPE_FISH=0,
			ENEMYTYPE_SENTRY,
			ENEMYTYPE_MINE
		};

		enum
		{
			MOVEMENT_HORIZONTAL=0,
			MOVEMENT_VERTICAL
		};

		enum
		{
			STARTPOINT_CENTER=0,
			STARTPOINT_LEFT_OR_TOP,
			STARTPOINT_RIGHT_OR_BOTTOM
		};

	public:
		BaseEnemy()
		{
			m_pBody = 0;
			m_SpawnLocation = math::Vec3( 0.0f, 0.0f, 0.0f );

			m_ResourceIndex = -1;
			m_pModel = 0;

			m_AreaDims = math::Vec2( 1.0f, 1.0f );

			m_BoundingSphere.vCenterPoint = math::Vec3( 0.0f, 0.0f, 0.0f );
			m_BoundingSphere.fRadius = 1.0f;

			m_BoundingAABB.vBoxMin = math::Vec3( -1.0f, -1.0f, -1.0f );
			m_BoundingAABB.vBoxMax = math::Vec3( 1.0f, 1.0f, 1.0f );

		}
		virtual ~BaseEnemy()
		{
		}

		virtual void Draw() {}
		virtual void Update( float deltaTime ) {}
		virtual void HandleContact( const b2ContactPoint* /*contact*/ ) {}
		virtual void Remove() {}
		virtual void Reset() {}
		virtual void DrawDebug() {}

	protected:
		b2Body* m_pBody;
		math::Vec3 m_SpawnLocation;

		collision::Sphere m_BoundingSphere;
		collision::AABB m_BoundingAABB;

		int m_ResourceIndex;
		mdl::ModelHGM* m_pModel;

		math::Vec3 m_AreaCenter;
		math::Vec2 m_AreaDims;

		math::Vec2 m_AreaMinExtends;
		math::Vec2 m_AreaMaxExtends;

	private:

};

#endif // __BASEDOOR_H__
