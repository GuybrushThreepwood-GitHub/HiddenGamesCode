
/*===================================================================
	File: Enemy.h
	Game: H8

	(C)Hidden Games
=====================================================================*/

#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "PhysicsBase.h"

#include "InputSystem/InputSystem.h"
#include "Audio/AudioSystem.h"

#include "ScriptAccess/ScriptDataHolder.h"
#include "ScriptAccess/ScriptAccess.h"

#include "Effects/Emitter.h"

#include "Physics/PhysicsIds.h"

// forward declares
class EnemyManager;

// these have to match in scripts
enum 
{
	ENEMYTYPE_BITS_GENERIC = 0,
	ENEMYTYPE_BITS_TRIANGLE,
	ENEMYTYPE_BITS_SQUARE,
	ENEMYTYPE_BITS_HEXAGON,
	ENEMYTYPE_BITS_OCTAGON,
	ENEMYTYPE_BITS_CIRCLE,

	ENEMYTYPE_TROJAN,
	ENEMYTYPE_VIRUS,
	ENEMYTYPE_BULLET,
};

struct EnemyTypeData
{
	int typeId;
	int hgmModelIndex;
	float radius;
	int coreDamage;
	int matchPoints;
	int shieldDamage;
	float force;

	mdl::ModelHGM* modelData;
};

class Enemy : public physics::PhysicsIdentifier
{
	public:
		enum {

			ENEMYFLAG_DESTROY = 0x00000001,
			ENEMYFLAG_ABSORB = 0x00000002,
			ENEMYFLAG_REBOUND_GOOD = 0x00000004,
			ENEMYFLAG_REBOUND_BAD = 0x00000008,
			ENEMYFLAG_BAD_MATCH = 0x00000010,
		};

		enum EnemyVirusState
		{
			EnemyVirusState_MoveToPosition=0,
			EnemyVirusState_CircleCore,
			EnemyVirusState_AttackCore,
			EnemyVirusState_End
		};

	public:
		Enemy( EnemyManager* manager );
		~Enemy();

		void Setup();

		void Draw();
		void DrawDebug();
		void Update( float deltaTime );

		void Enable();
		void Disable();

		void Attack( const EnemyTypeData& typeData, int colourIndex, const math::Vec3& colour );
		void Reset();

		void Destroy();

		void SetFlags( unsigned long flag, bool addFlags=false );
		unsigned long GetFlags()									{ return m_Flags; }

		void SetStartPoint( const math::Vec3& pos, float angle )	{ m_StartPosition = pos; m_StartAngle = angle;	}
		bool IsEnabled()											{ return m_IsEnabled;		}
		void Rebound()												{ m_HasRebound = true;		}
		bool HasRebound()											{ return m_HasRebound;		}
		int GetPoints()												{ return m_Points;			}
		math::Vec3& GetCurrentPosition()							{ return m_CurrentPosition; }
		math::Vec3& GetColour()										{ return m_Colour;			}

	private:
		float m_LastDelta;

		ScriptDataHolder::DevScriptData m_DevData;
		GameData m_GameData;

		ScriptDataHolder* m_pScriptData;
		EnemyManager* m_Manager;

		math::Vec3 m_StartPosition;
		float m_StartAngle;
		math::Vec3 m_CurrentPosition;
		b2Vec2 m_BodyPos;

		EnemyTypeData m_TypeData;
		mdl::ModelHGM* m_pModelData;
		unsigned int m_SubMesh;

		b2Body* m_Body;

		bool m_IsEnabled;
		bool m_HasRebound;

		int m_ColourIndex;
		math::Vec3 m_Colour;

		int m_Type;
		int m_Damage;
		int m_Points;

		unsigned long m_Flags;

		float m_TrojanColourSwapTime;
		bool m_TrojanStopColourChange;
		math::Vec3 m_TrojanUnprojectedPoint;

		EnemyVirusState m_VirusState;
		float m_VirusBodyRotation;
		float m_VirusAngleAroundCore;
		int m_VirusBulletsFired;
		float m_BulletFireTime;
		float m_VirusCircleDistance;
		float m_VirusAttackAngle;
};

#endif // __ENEMY_H__
