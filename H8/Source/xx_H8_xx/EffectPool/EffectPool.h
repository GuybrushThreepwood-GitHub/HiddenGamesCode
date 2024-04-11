
/*===================================================================
	File: EffectPool.h
	Game: H8

	(C)Hidden Games
=====================================================================*/

#ifndef __EFFECTPOOL_H__
#define __EFFECTPOOL_H__

#include "Effects/Emitter.h"

// forward declare
class LODPiece;

const int MAX_CIRCLE_EFFECTS = 32;
const int MAX_CIRCLE_POINTS = 32;

const int MAX_PIXEL_EXPLOSIONS = 20;

class EffectPool
{
	public:
		struct CircleEffect
		{
			bool active;
			math::Vec3 pos;
			math::Vec3 colour;
			float scale;
			math::Vec3 vaPoints[MAX_CIRCLE_POINTS];
		};

	public:
		EffectPool( );
		~EffectPool();

		void Draw();
		void Update( float deltaTime );

		void Reset();

		void SpawnExplosion( const math::Vec3& pos, const math::Vec3 colour );
		void SpawnCircle( const math::Vec3& pos, const math::Vec3& col );
		void SpawnHearts();

	private:
		int m_NextExplosionIndex;
		efx::Emitter* m_ExplosionList;

		int m_NextCircleIndex;
		CircleEffect m_CircleEffect[MAX_CIRCLE_EFFECTS];

		efx::Emitter m_Hearts;
};

#endif // __EFFECTPOOL_H__

