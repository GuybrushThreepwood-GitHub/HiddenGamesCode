
/*===================================================================
	File: EnemyManager.h
	Game: H8

	(C)Hidden Games
=====================================================================*/

#ifndef __ENEMYMANAGER_H__
#define __ENEMYMANAGER_H__

#include "PhysicsBase.h"

#include "InputSystem/InputSystem.h"
#include "Audio/AudioSystem.h"

#include "ScriptAccess/ScriptDataHolder.h"
#include "ScriptAccess/ScriptAccess.h"

#include "Effects/Emitter.h"

#include "Physics/PhysicsIds.h"
#include "Enemy/Enemy.h"

const int EXPLOSION_AUDIO = 4;

class EnemyManager
{
	public:
		EnemyManager();
		~EnemyManager();

		void Initialise();

		void Draw();
		void DrawDebug();
		void Update( float deltaTime );

		void PauseAttack();
		void UnPauseAttack();

		void SpawnBullet( math::Vec3 pos, float angle, int colourIndex, const math::Vec3& colour );

		void DestroyActiveEnemies(bool awardPoints);


		bool IsPaused()				{ return m_AttackPaused; }

	private:
		void LoadEnemyTypes();

	private:
		float m_LastDelta;
		float m_CountdownToAttack;

		ScriptDataHolder::DevScriptData m_DevData;
		GameData m_GameData;

		ScriptDataHolder* m_pScriptData;

		int m_NumEnemyTypes;

		EnemyTypeData** m_EnemyTypes;
		Enemy** m_Enemies;
		Enemy** m_Bullets;

		bool m_LinePulseUp;
		float m_LineSize;

		bool m_AttackPaused;

		ALuint m_TrojanAppearAudio;
		ALuint m_VirusAppearAudio;

		ALuint m_ExplosionAudio[EXPLOSION_AUDIO];
};

#endif // __ENEMYMANAGER_H__
