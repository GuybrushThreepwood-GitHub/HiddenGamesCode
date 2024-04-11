
/*===================================================================
	File: Player.h
	Game: AirCadets

	(C)Hidden Games
=====================================================================*/

#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "InputSystem/InputSystem.h"
#include "Audio/AudioSystem.h"

#include "ScriptAccess/ScriptDataHolder.h"
#include "ScriptAccess/ScriptAccess.h"

#include "Level/Level.h"
#include "Effects/Emitter.h"

#include "Physics/PhysicsIds.h"

class Level;
class RayObject;

#define PLANE_MAXNUM_BULLETS		15

class Player : public physics::PhysicsIdentifier
{
	public:
		struct Part
		{
			mdl::ModelHGM* partModel;
			math::Vec3 initialPos;
			math::Vec3 currentPos;
			math::Vec3 currentRot;
			math::Vec3 maxRot;
			math::Vec3 rotSpeed;
		};

		struct Effect
		{
			efx::Emitter* pEmitter;
			int resId;
			math::Vec3 initialPos;
			math::Vec3 currentPos;
		};

		class Bullet : public physics::PhysicsIdentifier
		{
			public:
				int index;
				bool stateChange;
				int active;
				math::Vec3 vStartPoint;
				math::Vec3 vPos;
				math::Vec3 vVel;
				math::Vec3 vRot;

				float life;
				float distanceTravelled;

				dGeomID geomId;

				bool doEffect;
				efx::Emitter *pEmitter;
		};

	public:
		Player( InputSystem& inputSystem, ScriptDataHolder::VehicleScriptData& vehicleData, Level& activeLevel );
		~Player();

		int LoadPhysicsData( const char *szFilename );

		void Setup();
		void SetInputState( bool state )					{ m_InputSystem.IgnoreInput( state ); }

		void Draw();
		void DrawAlpha();
		void DrawBullets();

		void DrawDebug();
		void DrawEmitters();
		void Update( float deltaTime );

		void SetPosition( math::Vec3 pos );
		math::Vec3 GetPosition()							{ return m_Pos; }

		void SetAngles( math::Vec3& newAngles )				{ m_Rot = newAngles; }
		math::Vec3 GetAngles()								{ return m_Rot; }

		void SetDirection( math::Vec3& newDir )				{ m_FinalDir = newDir; }
		math::Vec3 GetDirection()							{ return m_FinalDir; }

		float GetDistanceToGround()							{ return m_DistanceToGround; }
		math::Vec3 GetUpVector()							{ return m_FinalUp; }
		bool HasWorldCollision()							{ return m_WorldCollided; }
		void SetWorldCollision( bool state )				{ m_WorldCollided = state; }

		void PlayerCrashed();
		bool IsDead()										{ return m_IsDead; }

		void SetRespawnPosition( math::Vec3 pos, math::Vec3 rot )		
		{ 
			m_RespawnPosition = pos; 
			m_RespawnRot = rot;
		}
		void Respawn();
		void ResetRingPitch();

		collision::Sphere& GetBoundingSphere()				{ return m_BoundSphere; }
		collision::Sphere& GetDrawDistanceSphere()			{ return m_DrawDistanceSphere; }
		math::Vec3& GetUnprojection()						{ return m_UnProjectedPoint; }

		void SetMeshDrawState( int index, bool state );

		void CheckAgainstLevel( Level& activeLevel );

		void StartAudio();
		void StopAllAudio();

		void SetTargetSpeed( float speed )					{ m_TargetSpeed = speed; }
		float GetTargetSpeed()								{ return m_TargetSpeed; }
		float GetSpeed()									{ return m_Speed; }
		const math::Vec2& GetSpeedRange()					{ return m_SpeedRange; }
		float GetFlyLowTime()								{ return m_FlyingLowTime; }

	private:
		void InitialiseEmitters();
		void UpdateEmitters(float deltaTime);
		void UpdateBullets(float deltaTime);

		void CreatePhysics( bool create );
		dGeomID CreateGeom( int shapeClass, dSpaceID spaceId, const math::Vec3& dims, float radius=0.0f, float length=0.0f, dTriMeshDataID triMeshDataId=0 );

		void CalculateRudderAndFlaps();

	private:
		InputSystem& m_InputSystem;
		Level& m_ActiveLevel;
		float m_LastDelta;

		ScriptDataHolder::DevScriptData m_DevData;
		GameData m_GameData;

		ScriptDataHolder* m_pScriptData;
		ScriptDataHolder::VehicleScriptData& m_VehicleData;

		mdl::ModelHGM* m_pModelData;
		
		Part m_PropData;
		Part m_RudderData;
		Part m_LeftTailFlapData;
		Part m_RightTailFlapData;
		Part m_LeftWingFlapData;
		Part m_RightWingFlapData;
		Part m_MuzzleFlashData;

		Effect m_Exhaust1;
		Effect m_Exhaust2;
		Effect m_WingSlice1;
		Effect m_WingSlice2;

		bool m_ShadowDraw;
		math::Vec3 m_ShadowPos;
		dMatrix3 m_ShadowOri;
		mdl::ModelHGM* m_pShadow;

		// physics data
		res::PhysicsBlock* m_PhysicsData;
		
		dBodyID m_BodyID;
		dSpaceID m_EntitySpace;
		math::Vec3 m_Pos;
		math::Vec3 m_Dir;
		math::Vec3 m_UpVector;

		math::Vec3 m_FinalUp;
		math::Vec3 m_FinalDir;
		dMatrix3 m_Orientation;
		dQuaternion m_qPlaneOri;

		math::Vec3 m_PosCOM;
		math::Vec3 m_Rot;
		math::Vec3 m_RotSpeed;
		math::Vec3 m_TargetRot;
		math::Vec3 m_COM;

		float m_DistanceToGround;

		float m_Speed;
		float m_Accelerate;
		float m_Decelerate;

		math::Vec2 m_SpeedRange;
		float m_TargetSpeed;

		float m_PropellorSpeed;

		math::Vec3 m_RespawnPosition;
		math::Vec3 m_RespawnRot;

		bool m_IsDead;

		bool m_NightMode;

		collision::Sphere m_BoundSphere;
		collision::Sphere m_DrawDistanceSphere;

		bool m_SpawnedBullet;
		float m_MuzzleFlashTime;
		int m_NextBulletIndex;
		Bullet m_BulletList[PLANE_MAXNUM_BULLETS];
		mdl::ModelHGM* m_BulletModel;
		collision::AABB m_AABB;

		float m_ShootTimer;

		// current light details
		math::Vec4 m_LightPos;
		math::Vec4 m_LightDiffuse;
		math::Vec4 m_LightAmbient;
		math::Vec4 m_LightSpecular;

		math::Vec4 m_LightPosTarget;
		math::Vec4 m_LightDiffuseTarget;
		math::Vec4 m_LightAmbientTarget;
		math::Vec4 m_LightSpecularTarget;

		// gate details
		Level::GateGroup* m_ActiveGateGroup;
		std::vector<Level::Gate>* m_Sights;

		RayObject* m_pRayTest;
		unsigned int m_CategoryBits;
		unsigned int m_CollideBits;
		bool m_WorldCollided;

		math::Vec3 m_UnProjectedPoint;

		// audio files
		bool m_AudioStarted;

		ALuint m_EngineBufferId;
		ALuint m_EngineSrcId;

		ALuint m_GunBufferId;
		ALuint m_GunSrcId;
		
		ALuint m_PlayerCrashBufferId;
		float m_RingPitch;

		// achievements
		float m_FlyingLowTime;
};

/////////////////////////////////////////////////////
/// Method: SetPosition
/// Params: [in]pos
///
/////////////////////////////////////////////////////
inline void Player::SetPosition( math::Vec3 pos ) 
{
	m_Pos = pos;
	m_BoundSphere.vCenterPoint = m_Pos;
}

/////////////////////////////////////////////////////
/// Method: SetMeshDrawState
/// Params: [in]index, [in]state
///
/////////////////////////////////////////////////////
inline void Player::SetMeshDrawState( int index, bool state )
{
	if( m_pModelData != 0 )
	{
		m_pModelData->SetMeshDrawState( index, state );
	}
}

#endif // __PLAYER_H__
