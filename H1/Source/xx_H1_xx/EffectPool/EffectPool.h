
/*===================================================================
	File: EffectPool.h
	Game: AirCadets

	(C)Hidden Games
=====================================================================*/

#ifndef __EFFECTPOOL_H__
#define __EFFECTPOOL_H__

#include "Effects/Emitter.h"

// forward declare
class LODPiece;

class EffectPool
{
	public:
		EffectPool( int maxExplosions, int maxFires );
		~EffectPool();

		void Draw();
		void Update( float deltaTime );

		void Reset();

		void SpawnExplosion( const math::Vec3& pos );
		void SpawnFire( const math::Vec3& pos );
		void SpawnStars( const math::Vec3& pos );

	private:
		int m_TotalExplosions;
		int m_ExplosionIndex;
		int m_UsedExplosionSlots;
		efx::Emitter* m_ExplosionList;
		efx::Emitter* m_ExplosionSmokeList;

		int m_TotalFires;
		int m_UsedFireSlots;
		efx::Emitter* m_FireList;
		efx::Emitter* m_FireSmokeList;

		efx::Emitter* m_Stars;

};

#endif // __EFFECTPOOL_H__

