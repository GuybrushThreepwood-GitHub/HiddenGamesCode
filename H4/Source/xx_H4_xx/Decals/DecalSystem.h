
/*===================================================================
	File: DecalSystem.h
	Game: H4

	(C)Hidden Games
=====================================================================*/

#ifndef __DECALSYSTEM_H__
#define __DECALSYSTEM_H__

#include "GameConsts.h"

class DecalSystem
{
	public:
		struct DecalVert
		{
			math::Vec3 v;
			math::Vec2 uv;
		};

	public:
		DecalSystem();
		~DecalSystem();

		void AddBulletHole( const math::Vec3& pos, const math::Vec3& normal );
		void AddBloodSplat( bool floorHit, const math::Vec3& pos, const math::Vec3& normal );
		
		void DrawDecals();

		void ClearDecals();

	private:
		int m_NumActiveBulletHoles;
		int m_NumActiveBloodSplats;

		GLuint m_DecalTex;

		int m_NextBulletIndex;
		DecalVert m_BulletDecals[MAX_PLAYER_BULLETS*6]; // 6 vert per decal

		int m_NextBloodSplatIndex;
		DecalVert m_BloodDecals[(MAX_ENEMIES*(MAX_ENEMY_GIBS*MAX_ENEMY_BLOOD_SPLATS))*6]; // 6 vert per decal

};

#endif // __DECALSYSTEM_H__

