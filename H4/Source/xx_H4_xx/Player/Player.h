
/*===================================================================
	File: Player.h
	Game: H4

	(C)Hidden Games
=====================================================================*/

#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "InputSystem/InputSystem.h"
#include "Level/Level.h"
#include "Player/Player.h"

#include "Audio/AudioSystem.h"

#include "ScriptAccess/ScriptDataHolder.h"
#include "ScriptAccess/ScriptAccess.h"

#include "Physics/PhysicsIds.h"

#include "NavigationMesh/NavObject.h"
#include "Player/Bullet.h"

#include "GameConsts.h"

const int WEAPONINDEX_PISTOL = 0;
const int WEAPONINDEX_SHOTGUN = 1;

class Player : public physics::PhysicsIdentifier, public NavObject
{
	public:
		enum PlayerProps
		{
			PLAYERPOS = BaseObject::EXTRASTATE,
			PLAYERROT
		};

		// matches maya
		enum Steps
		{
			STEP_STONE=0,
			STEP_WET=1,
			STEP_GRATE=2,
			STEP_SNOW=3,
			STEP_GRASS=4,
			STEP_CARPET=5,
			STEP_FLESH=6,
			STEP_WATERWADE=7,

			STEP_AUDIO_COUNT
		};

		struct StepPlayback
		{
			ALuint StepLeft;
			ALuint StepRight;
		};

	public:
		Player( InputSystem& inputSystem );
		~Player();

		void Setup();
		void SetupPhysics();
		void SetupLevelData( Level& activeLevel, NavigationMesh* navMesh );

		void Draw( bool pauseFlag = false );
		void Draw2D();
		void DrawDebug();

		void SetPosition( math::Vec3 pos );
		math::Vec3& GetPosition()						{ return m_Pos; }

		void SetRotationAngle( float angle )			{ m_TargetRotation = angle; }
		void SetAngle( float angle )					{ m_RotationAngle = m_TargetRotation = angle; }
		float GetRotationAngle()						{ return m_RotationAngle; }

		void SetWeaponIndex( int index );
		int GetWeaponIndex()							{ return m_WeaponIndex; }
		void SetHasPistol( bool state )					{ m_HasPistol = state; }
		void SetHasShotgun( bool state )				{ m_HasShotgun = state; }
		void SetFPSMode( bool state )					{ m_FPSMode = state; }
		bool GetFPSMode()								{ return m_FPSMode; }
		void SetClothingIndex( int index );
		int GetClothingIndex()							{ return m_ClothingIndex; }

		void SetPistolBulletMagCount( int amount )		{ m_PistolBulletMagCount = amount; }
		int GetPistolBulletMagCount()					{ return m_PistolBulletMagCount; }
		void SetPistolBulletTotal( int amount )			{ m_PistolBulletTotal = amount; }
		int GetPistolBulletTotal()						{ return m_PistolBulletTotal; }

		void SetShotgunBulletMagCount( int amount )		{ m_ShotgunBulletMagCount = amount; }
		int GetShotgunBulletMagCount()					{ return m_ShotgunBulletMagCount; }
		void SetShotgunBulletTotal( int amount )		{ m_ShotgunBulletTotal = amount; }
		int GetShotgunBulletTotal()						{ return m_ShotgunBulletTotal; }

		void SetHealth( int health)						{ m_Health = health; }
		int GetHealth()									{ return m_Health; }

		void SetRespawnPosition( math::Vec3 pos, int direction )		
		{ 
			m_RespawnPosition = pos; 
			m_RespawnDirection = direction;
		}
		void Respawn();

		collision::Sphere& GetBoundingSphere()			{ return m_BoundSphere; }

		math::Vec3& GetCamPos()							{ return m_CamPos; }
		math::Vec3& GetCamTarget()						{ return m_CamTarget; }

		void Update( float deltaTime );
		void UpdateModel( float deltaTime );
		void UpdateNormalB2D( float deltaTime );
		void UpdateUsing( float deltaTime );
		void UpdateReloading( float deltaTime );

		void EnablePlayerControls()						{ m_AllowControl = true; }
		void DisablePlayerControls()					{ m_AllowControl = false; }
	
		void EnableFPSMode()							{ m_FPSMode = true; }
		void DisableFPSMode()							{ m_FPSMode = false; }

		b2Body* GetBody()								{ return m_pPlayerBody; }
		Bullet* GetBulletIndex( int index )				{ return m_Bullets[index]; }
		bool InUseArea()								{ return m_InUseArea; }
		bool IsReloading()								{ return m_IsReloading; }
		void TakeDamage( int healthLoss );
		bool DamageFlag()								{ return m_TakeDamage; }
		bool IsRunning()								{ return m_IsRunning; }
		void SetTorchState(bool state)					{ m_UseTorch = state; }
		bool TorchEnabled()								{ return m_UseTorch; }

		void SetDeadState( bool state )					{ m_IsDead = state; }
		bool IsDead()									{ return m_IsDead; }

		void Debug_DisablePlayerCollision();
		void Debug_EnablePlayerCollision();

		void HandleContact( int idNum, int castId, const ContactPoint* contact, const b2Fixture* pOtherFixture, const b2Body* pOtherBody );
		void ClearContact( int idNum, int castId, const ContactPoint* contact, const b2Fixture* pOtherFixture, const b2Body* pOtherBody );

		void SpawnBullet( Bullet::BulletSource src );
		void ClearBullets();

		bool PlayerInUseArea();
		bool PlayerInLightZone();
		bool PlayerInStepAudioZone();

		void ClearMovement( bool resetAngles );
		void ApplyImpulse( b2Vec2& impulse );

		void GiveHealth( int amount );
		void GiveAmmo( int pistolAmmo, int shotgunAmmo );

		void ClearLevelData()
		{
			m_Parent = 0;
			m_CurrentCell = 0;
			m_PathActive = false;

			// bullets
			m_NextBulletIndex = 0;
			m_LastBulletTime = 0.0f;

			m_IsUsing = false;
			m_InUseArea = false;
			m_pUseArea = 0;

			m_LightZoneIndex = -1;
			m_StepAudioIndex = -1;

			m_pLevel = 0;
			m_UseAreas.clear();
			m_LightZones.clear();
			m_StepAudioZones.clear();
		}

	private:
		std::vector<GLuint> m_TextureList;

		InputSystem& m_InputSystem;
		bool m_AllowControl;

		snd::Sound m_SndLoad;

		Level* m_pLevel;
		std::vector<UseArea *> m_UseAreas;
		std::vector<Level::LightZone *> m_LightZones;
		std::vector<Level::StepAudio *> m_StepAudioZones;

		float m_LastDelta;

		ScriptDataHolder* m_pScriptData;
		mdl::ModelHGA* m_pModelData;

		/// storage for the GL state
		renderer::TGLDepthBlendAlphaState m_DBAState;

		bool m_FPSMode;
		ScriptDataHolder::CameraSetup m_CameraData;
		math::Vec3 m_CamTarget;
		math::Vec3 m_CamPos;
		float m_CamHeight;
		float m_TiltAngle;

		// physics data
		b2Body* m_pPlayerBody;
		bool m_HasContact;
		b2Vec2 m_ContactNormal;

		math::Vec3 m_Pos;
		math::Vec3 m_PrevPos;
		math::Vec3 m_NavCellPos;
		math::Vec3 m_Dir;

		bool m_IsRunning;

		math::Vec3 m_RespawnPosition;
		int m_RespawnDirection;

		bool m_SmoothRotation;

		float m_TargetRotation;
		float m_RotationAngle;

		collision::Sphere m_BoundSphere;
		collision::Sphere m_UseSphere;

		int m_WeaponIndex;
		bool m_HasPistol;
		bool m_HasShotgun;
		bool m_UseTorch;

		// clothing
		int m_ClothingIndex;
		GLuint m_BaseClothing;
		GLuint m_BaseBloodClothing;
		GLuint m_CurrentClothes;

		// bullets
		int m_NextBulletIndex;
		float m_LastBulletTime;
		Bullet* m_Bullets[MAX_PLAYER_BULLETS];

		int m_PistolBulletMagCount;
		int m_PistolBulletTotal;

		int m_ShotgunBulletMagCount;
		int m_ShotgunBulletTotal;

		bool m_IsReloading;
		float m_ShotgunReloadTimer;
		int m_ShotgunShellAudioCount;

		// use area
		bool m_IsUsing;
		bool m_InUseArea;
		UseArea* m_pUseArea;

		// in light zone
		int m_LightZoneIndex;
		math::Vec4 m_LightPos;
		math::Vec4 m_LightPosTarget;

		math::Vec4 m_LightDiffuse;
		math::Vec4 m_LightDiffuseTarget;

		// step audio
		int m_StepAudioIndex;

		// player main health
		int m_Health;
		bool m_IsDead;

		// damage
		bool m_TakeDamage;
		float m_DamageTimer;

		// audio
		ALuint m_PlayerDeathSound;

		ALuint m_PistolAudio;
		ALuint m_ShotgunAudio;

		ALuint m_PistolReloadAudio;
		ALuint m_ShotgunReloadAudio;

		ALuint m_PistolEmptyAudio;
		ALuint m_ShotgunEmptyAudio;

		int m_LastStepFrame;
		StepPlayback m_StepSounds[STEP_AUDIO_COUNT];
};

/////////////////////////////////////////////////////
/// Method: SetPosition
/// Params: [in]pos
///
/////////////////////////////////////////////////////
inline void Player::SetPosition( math::Vec3 pos ) 
{
	m_Pos = pos;
	m_PrevPos = pos;

	CalculateStartingCell( m_Pos );

	if( m_pPlayerBody != 0 )
	{
		m_pPlayerBody->SetTransform( b2Vec2(m_Pos.X, m_Pos.Z), 0.0f );	
	}

	m_BoundSphere.vCenterPoint = m_Pos;

	if( m_FPSMode )
	{
		m_CamPos = math::Vec3( m_Pos.X-0.1f, m_Pos.Y+1.60f, m_Pos.Z+0.2f );
		m_CamTarget = math::Vec3( m_Pos.X-0.1f, (m_Pos.Y+0.5f)+m_CamHeight, m_Pos.Z+10.0f);
		
		m_CamPos = math::RotateAroundPoint( m_CamPos, m_Pos, math::DegToRad(m_RotationAngle) );
		m_CamTarget = math::RotateAroundPoint( m_CamTarget, m_Pos, math::DegToRad(m_RotationAngle) );
	}
	else
	{
		m_CamPos = math::Vec3( m_Pos.X+m_CameraData.cam_posx_offset, (m_Pos.Y+m_CameraData.cam_posy_offset), m_Pos.Z+m_CameraData.cam_posz_offset );
		m_CamTarget = math::Vec3( m_Pos.X+m_CameraData.cam_targetx_offset, (m_Pos.Y+m_CameraData.cam_targety_offset)+m_CamHeight, m_Pos.Z+m_CameraData.cam_targetz_offset);
		
		m_CamPos = math::RotateAroundPoint( m_CamPos, m_Pos, math::DegToRad(m_RotationAngle) );
		m_CamTarget = math::RotateAroundPoint( m_CamTarget, m_Pos, math::DegToRad(m_RotationAngle) );
	}	
	
	m_TakeDamage = false; 
	m_DamageTimer = 0.0f;	
}

/////////////////////////////////////////////////////
/// Method: Respawn
/// Params: None
///
/////////////////////////////////////////////////////
inline void Player::Respawn()
{
	if( m_pPlayerBody != 0 )
	{
		m_pPlayerBody->SetTransform( b2Vec2(m_RespawnPosition.X, m_RespawnPosition.Y), 0.0f );	
		m_pPlayerBody->SetLinearVelocity( b2Vec2(0.0f, 0.0f) );
		m_pPlayerBody->SetAngularVelocity( 0.0f );

		m_BoundSphere.vCenterPoint = m_RespawnPosition;
	}
}

/////////////////////////////////////////////////////
/// Method: TakeDamage
/// Params: [in]healthLoss
///
/////////////////////////////////////////////////////
inline void Player::TakeDamage( int healthLoss )
{
	m_Health -= healthLoss;
	if( m_Health < 0 )
	{
		m_Health = 0;

		if(!m_IsDead)
		{
			math::Vec3 pos(0.0f, 0.0f, 0.0f);
			AudioSystem::GetInstance()->PlayAudio( m_PlayerDeathSound, pos, true, false );
			m_IsDead = true;
		}
	}

	SetClothingIndex(m_ClothingIndex);

	m_TakeDamage = true; 
	m_DamageTimer = 1.0f;
}

#endif // __PLAYER_H__
