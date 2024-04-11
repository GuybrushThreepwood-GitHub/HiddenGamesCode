
/*===================================================================
	File: Player.h
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Box2D.h"

#include "InputSystem/InputSystem.h"
#include "Audio/AudioSystem.h"


#include "ScriptAccess/ScriptDataHolder.h"
#include "ScriptAccess/ScriptAccess.h"

#include "Level/Level.h"
#include "Effects/Emitter.h"

#include "Physics/PhysicsIds.h"

#include "Customers/Customer.h"

class Level;

const float CUSTOMER_REWARD_TIMER = 3.0f;
const float EXTRA_LIFE_REWARD_TIMER = 3.0f;

class Player : public physics::PhysicsIdentifier
{
	public:
		enum
		{
			PLAYERSPAWN_LEFT=0,
			PLAYERSPAWN_RIGHT
		};

	public:
		Player( InputSystem& inputSystem, ScriptDataHolder::VehicleScriptData& vehicleData, Level& taxiLevel );
		~Player();

		void Setup();
		void SetInputState( bool state )					{ m_InputSystem.IgnoreInput( state ); }
		//void SetAnimation( unsigned int animId );

		void Draw( bool disableMeshDraw = false, bool pauseFlag = false );
		void DrawNightMode();
		void DrawDebug();
		void DrawEmitters();
		void Update( float deltaTime );

		b2Body* GetBody()									{ return m_pPlayerBody; }
		void SetPosition( math::Vec3 pos );
		math::Vec3& GetPosition()							{ return m_Pos; }

		bool IsDead()										{ return m_IsDead; }
		void SetRotationAngle( float angle )				{ m_TargetRotation = angle; }
		float GetRotationAngle()							{ return m_RotationAngle; }

		void SetRespawnPosition( math::Vec3 pos, int direction )		
		{ 
			m_RespawnPosition = pos; 
			m_RespawnDirection = direction;
		}
		void Respawn();

		collision::Sphere& GetBoundingSphere()				{ return m_BoundSphere; }

		void SetContactState( bool state )					{ m_IsContactWorld = state; }
		void HandleContact( int idNum, int castId, const b2ContactPoint* point, b2Shape* pOtherShape, b2Body* pOtherBody );
		void HandlePersistantContact( int idNum, int castId, const b2ContactPoint* point, b2Shape* pOtherShape, b2Body* pOtherBody );
		void ClearContact( int idNum );

		void SetMeshDrawState( int index, bool state );

		void CheckAgainstLevel( Level& taxiLevel );

		void StopAllAudio();

		int GetVehicleModelIndex()							{ return m_VehicleData.modelIndex; }
		float GetMaxXVel()									{ return m_MaxXVel; }
		float GetMinXVel()									{ return m_MinXVel; }
		float GetMaxYVel()									{ return m_MaxYVel; }
		float GetMoveForce()								{ return m_MoveForce; }
		float GetUpwardsForce()								{ return m_UpwardsForce; }

		void SetFuelCount( int count )						{ m_FuelCount = count; }
		int GetFuelCount()									{ return m_FuelCount; }
		bool IsInContactWithWorld()							{ return m_IsContactWorld; }
		bool IsOnGroundIdle()								{ return m_IsOnGroundIdle; }
		bool IsRefueling()									{ return m_IsRefueling; }

		bool IsInLandingZone()								{ return m_bInLandingZone; }
		int GetLandingZoneIndex()							{ return m_LandingZoneIndex; }
		Level::ePortType GetLandingZoneType()				{ return m_LandingType; }

		float GetLandedTime()								{ return m_LandedTime; }

		void SetCustomersComplete( int complete )			{ m_CustomersComplete = complete; }
		int GetCustomersComplete()							{ return m_CustomersComplete; }

		void SetLevelCustomerTotal( int total )				{ m_CustomersTotal = total; }
		int GetLevelCustomerTotal()							{ return m_CustomersTotal; }

		bool ShowCustomerReward()							{ return m_ShowCustomerReward; }
		int CustomerReward()								{ return m_CustomerReward; }

		bool ShowPortNumber()								{ return m_ShowPortNumber; }
		int PortRequested()									{ return m_PortRequest; }

		bool ShowExtraLifeReward()							{ return m_ShowExtraLifeReward; }

		int GetRefuelZoneIndex()							{ return m_RefuelZoneIndex; }
		int GetHQZoneIndex()								{ return m_HQZoneIndex; }

		//std::vector<Level::BoundingArea>& GetAreaList()			{ return m_AreaList; }
		std::vector<Level::PortArea>& GetPortAreaList()			{ return m_PortAreaList; }
		std::vector<Level::GravityArea>& GetGravityAreaList()	{ return m_GravityAreaList; }
		std::vector<Level::ForceArea>& GetForceAreaList()		{ return m_ForceAreaList; }

		math::Vec3& GetUnprojection()						{ return m_UnProjectedPoint; }

		void AddPassenger( int portNumber, Customer& customer );
		void RemovePassenger();
		bool HasPassenger();

		void AddCargo( int portNumber, Customer& customer );
		void RemoveCargo();
		bool HasCargo();
		GLuint GetCargoTextureId()							{ return m_CargoTextureId; }
		int GetCargoDestination();
		int GetPassengerDestination()						{ return m_PortRequest; }

		void SetLivesCount( int count )						{ m_LivesCount = count; }
		int GetLivesCount();
		int GetMoneyToAdd();
		void SetTotalMoney( int amount );
		int GetTotalMoney();
		void AddMoney( int amount, bool rewardMoney=false );
		void ShowExtraLife( );
		void ClearMoneyToAdd()								{ m_MoneyToAdd = 0; }

		float GetDamage()									{ return m_Damage; }

		bool HasSurvivedFuelRunout()						{ return m_SurvivedFuelRunout; }

		void SpawnCrashCharacters()
		{
			// throw out the driver 
			m_CrashPositionDriver = math::Vec3(m_Pos.X,m_Pos.Y,m_Pos.Z+1.0f);
			m_CrashPositionPassenger = math::Vec3(m_Pos.X,m_Pos.Y,m_Pos.Z+1.0f);

			if( m_TargetRotation == 0.0f )
				m_CrashVelocity = math::Vec3(-3.0f,5.0f,5.0f);
			else
				m_CrashVelocity = math::Vec3(3.0f,5.0f,5.0f);

			m_HadCustomerOnCrash = m_HasPassenger;
		}

		void SetNightMode( bool state )
		{
			m_NightMode = state;
		}

		bool InWater()				{ return m_InWater; }
    
        void SetWasPaused(bool state)   {m_SetWasPaused = state;}

	private:
		void ProcessSounds();
		
		void InitialiseEmitters();
		void UpdateEmitters(float deltaTime);

		void SetCrashAnimation( unsigned int nAnimId, int nFrameOffset, bool bForceChange );
		void UpdateCrashModel( float deltaTime );

	private:
		InputSystem& m_InputSystem;
		float m_LastDelta;

		ScriptDataHolder::DevScriptData m_DevData;
		GameData m_GameData;

		Sfx m_PropulsionSFX;
		Sfx m_BrakeSFX;
		Sfx m_OneShotSFX;
		Sfx m_LoopedSFX;

		ScriptDataHolder* m_pScriptData;
		ScriptDataHolder::VehicleScriptData& m_VehicleData;
		ScriptDataHolder::AtmosphereScriptData m_AirSettings;
		ScriptDataHolder::AtmosphereScriptData m_VacSettings;
		ScriptDataHolder::AtmosphereScriptData m_WaterSettings;

		Level& m_TaxiLevel;
		//std::vector<Level::BoundingArea> m_AreaList;
		std::vector<Level::PortArea> m_PortAreaList;
		std::vector<Level::GravityArea> m_GravityAreaList;
		std::vector<Level::ForceArea> m_ForceAreaList;

		mdl::ModelHGM* m_pModelData;

		// particle emitters
		efx::Emitter* m_pEmitHoriz;
		efx::Emitter* m_pEmitVert[2];

		efx::Emitter* m_pEmitHorizWater;
		efx::Emitter* m_pEmitVertWater[2];

		efx::Emitter* m_pLowDamage;
		efx::Emitter* m_pHighDamage;

		efx::Emitter* m_pWaterEnter;
		efx::Emitter* m_pWaterExit;

		efx::Emitter* m_pExplodeSparks;
		efx::Emitter* m_pExplodeSmoke;

		// physics data
		b2Body* m_pPlayerBody;
		b2PolygonDef* m_pLandingGearShapeDef;
		b2Shape* m_pLandingGearShape;
		math::Vec3 m_Pos;
		math::Vec3 m_RespawnPosition;
		int m_RespawnDirection;

		// dead player and passenger
		mdl::ModelHGA* m_pCrashModel;
		/// current primary animation
		mdl::BoneAnimation *m_pCrashAnim;
		math::Vec3 m_CrashPositionDriver;
		math::Vec3 m_CrashPositionPassenger;
		math::Vec3 m_CrashVelocity;
		float m_CrashRotation;
		bool m_HadCustomerOnCrash;
		GLuint m_PlayerTexture;
		int m_CustomerModelIndex;
		GLuint m_CustomerTexture;
		Customer::eGender m_CustomerGender;

		bool m_SmoothRotation;
		bool m_NightMode;

		float m_UpwardsForce;
		float m_MoveForce;
		float m_MaxXVel;
		float m_MinXVel;
		float m_MaxYVel;
		float m_MaxFallVel;

		float m_CurrentUpForce;
		float m_CurrentMoveForce;

		float m_TargetRotation;
		float m_RotationAngle;

		bool m_IsOnGroundIdle;
		bool m_IsContactWorld;
		bool m_WorldEdgeContact;
		bool m_IsContactPort;
		bool m_IsContactEnemy;
		bool m_IsContactShape;
		bool m_IsContactDoor;

		float m_TargetConformAngle;
		float m_ConformAngle;
		
		bool m_IsRefueling;
		bool m_IsRefusedFuel;
		bool m_OutOfFuel;
		int m_FuelCount;
		float m_FuelUseTimer;
		float m_FuelIncreaseTimer;
		int m_RefuelZoneIndex;
		bool m_SurvivedFuelRunout;

		bool m_LandingGear;
		float m_LandedTime;

		collision::Sphere m_BoundSphere;

		Level::eGravityType m_AtmosType;
		Level::eGravityType m_OldAtmosType;

		bool m_bInLandingZone;
		Level::ePortType m_LandingType;
		int m_LandingZoneIndex;

		int m_CustomersTotal;
		int m_CustomersComplete;

		//bool m_ShowSplash;
		float m_SplashInTime;
		float m_SplashOutTime;

		bool m_InWater;

		bool m_ShowCrash;
		float m_CrashTime;

		bool m_ShowPortNumber;
		float m_PortNumberTime;
		int m_PortRequest;

		bool m_ShowCustomerReward;
		float m_CustomerRewardTime;
		int m_CustomerReward;

		bool m_ShowExtraLifeReward;
		float m_ExtraLifeRewardTime;
		int m_ExtraLifeReward;

		bool m_HasPassenger;
		bool m_HasCargo;
		int m_CargoDestination;
		GLuint m_CargoTextureId;

		int m_HQZoneIndex;

		// audio
		ALuint m_VacBufferId;
		ALuint m_AirBufferId;
		ALuint m_SubBufferId;

		ALuint m_AtmosChangeAir2WaterId;
		ALuint m_AtmosChangeWater2AirId;

		ALuint m_LandingGearExtendId;

		ALuint m_FuelFillId;
		ALuint m_FuelLowId;
		ALuint m_FuelRefuseId;

		ALuint m_CollisionSmallId;
		ALuint m_CollisionLargeId;
		ALuint m_CollisionMassiveId;
		ALuint m_DestroyedId;

		ALuint m_CrashMaleAudioId;
		ALuint m_CrashFemaleAudioId;

		ALuint m_DriverScream;
		bool m_DriverAscend;
		float m_DriverScreamGain;

		ALuint m_PassengerScream;
		bool m_PassengerAscend;
		float m_PassengerScreamGain;

		math::Vec3 m_UnProjectedPoint;

		// points
		int m_LivesCount;
		int m_TotalMoney;
		int m_MoneyToAdd;
		int m_CustomerFare;

		// damage
		bool m_IsDead;
		bool m_AllowCollisionSound;
		float m_Damage;

		float m_ThrustTiltAngle;
		float m_ThrustTiltTarget;

		// current light details
		math::Vec4 m_LightPos;
		math::Vec4 m_LightDiffuse;
		math::Vec4 m_LightAmbient;
		math::Vec4 m_LightSpecular;

		math::Vec4 m_LightPosTarget;
		math::Vec4 m_LightDiffuseTarget;
		math::Vec4 m_LightAmbientTarget;
		math::Vec4 m_LightSpecularTarget;
    
        bool m_SetWasPaused;
};

/////////////////////////////////////////////////////
/// Method: SetPosition
/// Params: [in]pos
///
/////////////////////////////////////////////////////
inline void Player::SetPosition( math::Vec3 pos ) 
{
	int i=0;

	if( m_pPlayerBody != 0 )
	{
		m_pPlayerBody->SetXForm( b2Vec2(pos.X, pos.Y), 0.0f );	
	}

	m_Pos = pos;
	m_BoundSphere.vCenterPoint = m_Pos;

	std::vector<Level::GravityArea>::iterator it = m_TaxiLevel.GetGravityAreaList().begin();
	while( it != m_TaxiLevel.GetGravityAreaList().end() )
	{
		math::Vec3 boxMin( (*it).pos.X - ((*it).dim.Width*0.5f), (*it).pos.Y - ((*it).dim.Height*0.5f), 0.0f );
		math::Vec3 boxMax( (*it).pos.X + ((*it).dim.Width*0.5f), (*it).pos.Y + ((*it).dim.Height*0.5f), 0.0f );

		collision::AABB aabb;
		aabb.vBoxMin = boxMin; 
		aabb.vBoxMax = boxMax;

		if( m_BoundSphere.SphereCollidesWithAABB( aabb ) )
		{
			// inside an atmospheric type
			if( (*it).type == Level::eGravityType_Vacuum ||
				(*it).type == Level::eGravityType_Air ||
				(*it).type == Level::eGravityType_Water )
			{
				m_AtmosType = (*it).type;
			}
		}

		it++;
	}

	switch( m_AtmosType )
	{
		case Level::eGravityType_Vacuum:
		{
			if( m_VehicleData.hasSubMeshChanges )
			{
				// astro disables
				for( i=0; i < m_VehicleData.astroSubMeshCount; ++i )
					SetMeshDrawState( m_VehicleData.astroSubMeshList[i], true );
					
				// aqua disables
				for( i=0; i < m_VehicleData.aquaSubMeshCount; ++i )
					SetMeshDrawState( m_VehicleData.aquaSubMeshList[i], false );
					
				// aero enables
				for( i=0; i < m_VehicleData.aeroSubMeshCount; ++i )
					SetMeshDrawState( m_VehicleData.aeroSubMeshList[i], false );
			}

			m_InWater					= false;
			m_MaxFallVel				= m_VehicleData.maxFallVelVac;
			m_MaxXVel					= m_VehicleData.maxXVelVac;
			m_MaxYVel					= m_VehicleData.maxYVelVac;

			m_UpwardsForce				= m_VehicleData.upwardsForceVac;
			m_MoveForce					= m_VehicleData.movementForceVac;

			PhysicsWorld::GetWorld()->SetGravity( m_VacSettings.gravity );

			m_OldAtmosType = m_AtmosType;

		}break;
		case Level::eGravityType_Air:
		{
			if( m_VehicleData.hasSubMeshChanges )
			{
				// astro disables
				for( i=0; i < m_VehicleData.astroSubMeshCount; ++i )
					SetMeshDrawState( m_VehicleData.astroSubMeshList[i], false );
					
				// aqua disables
				for( i=0; i < m_VehicleData.aquaSubMeshCount; ++i )
					SetMeshDrawState( m_VehicleData.aquaSubMeshList[i], false );
					
				// aero enables
				for( i=0; i < m_VehicleData.aeroSubMeshCount; ++i )
					SetMeshDrawState( m_VehicleData.aeroSubMeshList[i], true );
			}

			m_InWater					= false;
			m_MaxFallVel				= m_VehicleData.maxFallVelAir;
			m_MaxXVel					= m_VehicleData.maxXVelAir;
			m_MaxYVel					= m_VehicleData.maxYVelAir;

			m_UpwardsForce				= m_VehicleData.upwardsForceAir;
			m_MoveForce					= m_VehicleData.movementForceAir;

			PhysicsWorld::GetWorld()->SetGravity( m_AirSettings.gravity );

			m_OldAtmosType = m_AtmosType;

		}break;
		case Level::eGravityType_Water:
		{
			if( m_VehicleData.hasSubMeshChanges )
			{
				// astro disables
				for( i=0; i < m_VehicleData.astroSubMeshCount; ++i )
					SetMeshDrawState( m_VehicleData.astroSubMeshList[i], false );
					
				// aqua disables
				for( i=0; i < m_VehicleData.aquaSubMeshCount; ++i )
					SetMeshDrawState( m_VehicleData.aquaSubMeshList[i], true );
					
				// aero enables
				for( i=0; i < m_VehicleData.aeroSubMeshCount; ++i )
					SetMeshDrawState( m_VehicleData.aeroSubMeshList[i], false );
			}
			
			m_InWater					= true;
			m_MaxFallVel				= m_VehicleData.maxFallVelWater;
			m_MaxXVel					= m_VehicleData.maxXVelWater;
			m_MaxYVel					= m_VehicleData.maxYVelWater;

			m_UpwardsForce				= m_VehicleData.upwardsForceWater;
			m_MoveForce					= m_VehicleData.movementForceWater;

			PhysicsWorld::GetWorld()->SetGravity( m_WaterSettings.gravity );

			m_OldAtmosType = m_AtmosType;

		}break;
			
		default:
			break;			
	}
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

/////////////////////////////////////////////////////
/// Method: Respawn
/// Params: None
///
/////////////////////////////////////////////////////
inline void Player::Respawn()
{
	int i=0;
	m_InputSystem.ResetAllInput();

	m_InWater = false;
	if( m_pPlayerBody != 0 )
	{
		m_pPlayerBody->SetFixedRotation(true);

		m_pPlayerBody->SetXForm( b2Vec2(m_RespawnPosition.X, m_RespawnPosition.Y), 0.0f );	
		m_pPlayerBody->SetLinearVelocity( b2Vec2(0.0f, 0.0f) );
		m_pPlayerBody->SetAngularVelocity( 0.0f );

		m_BoundSphere.vCenterPoint = m_RespawnPosition;
	}

	if( m_RespawnDirection == Player::PLAYERSPAWN_LEFT )
	{
		m_TargetRotation = 0.0f;
		m_RotationAngle = 0.0f;
	}
	else
	{
		m_TargetRotation = 180.0f;
		m_RotationAngle = 180.0f;
	}

	std::vector<Level::GravityArea>::iterator it = m_TaxiLevel.GetGravityAreaList().begin();
	while( it != m_TaxiLevel.GetGravityAreaList().end() )
	{
		math::Vec3 boxMin( (*it).pos.X - ((*it).dim.Width*0.5f), (*it).pos.Y - ((*it).dim.Height*0.5f), 0.0f );
		math::Vec3 boxMax( (*it).pos.X + ((*it).dim.Width*0.5f), (*it).pos.Y + ((*it).dim.Height*0.5f), 0.0f );

		collision::AABB aabb;
		aabb.vBoxMin = boxMin; 
		aabb.vBoxMax = boxMax;

		if( m_BoundSphere.SphereCollidesWithAABB( aabb ) )
		{
			// inside an atmospheric type
			if( (*it).type == Level::eGravityType_Vacuum ||
				(*it).type == Level::eGravityType_Air ||
				(*it).type == Level::eGravityType_Water )
			{
				m_AtmosType = (*it).type;
			}
		}

		it++;
	}

	switch( m_AtmosType )
	{
		case Level::eGravityType_Vacuum:
		{
			if( m_VehicleData.hasSubMeshChanges )
			{
				// astro disables
				for( i=0; i < m_VehicleData.astroSubMeshCount; ++i )
					SetMeshDrawState( m_VehicleData.astroSubMeshList[i], true );
					
				// aqua disables
				for( i=0; i < m_VehicleData.aquaSubMeshCount; ++i )
					SetMeshDrawState( m_VehicleData.aquaSubMeshList[i], false );
					
				// aero enables
				for( i=0; i < m_VehicleData.aeroSubMeshCount; ++i )
					SetMeshDrawState( m_VehicleData.aeroSubMeshList[i], false );
			}

			m_InWater					= false;
			m_MaxFallVel				= m_VehicleData.maxFallVelVac;
			m_MaxXVel					= m_VehicleData.maxXVelVac;
			m_MaxYVel					= m_VehicleData.maxYVelVac;

			m_UpwardsForce				= m_VehicleData.upwardsForceVac;
			m_MoveForce					= m_VehicleData.movementForceVac;

			PhysicsWorld::GetWorld()->SetGravity( m_VacSettings.gravity );

			m_OldAtmosType = m_AtmosType;

		}break;
		case Level::eGravityType_Air:
		{
			if( m_VehicleData.hasSubMeshChanges )
			{
				// astro disables
				for( i=0; i < m_VehicleData.astroSubMeshCount; ++i )
					SetMeshDrawState( m_VehicleData.astroSubMeshList[i], false );
					
				// aqua disables
				for( i=0; i < m_VehicleData.aquaSubMeshCount; ++i )
					SetMeshDrawState( m_VehicleData.aquaSubMeshList[i], false );
					
				// aero enables
				for( i=0; i < m_VehicleData.aeroSubMeshCount; ++i )
					SetMeshDrawState( m_VehicleData.aeroSubMeshList[i], true );
			}

			m_InWater					= false;
			m_MaxFallVel				= m_VehicleData.maxFallVelAir;
			m_MaxXVel					= m_VehicleData.maxXVelAir;
			m_MaxYVel					= m_VehicleData.maxYVelAir;

			m_UpwardsForce				= m_VehicleData.upwardsForceAir;
			m_MoveForce					= m_VehicleData.movementForceAir;

			PhysicsWorld::GetWorld()->SetGravity( m_AirSettings.gravity );

			m_OldAtmosType = m_AtmosType;

		}break;
		case Level::eGravityType_Water:
		{
			if( m_VehicleData.hasSubMeshChanges )
			{
				// astro disables
				for( i=0; i < m_VehicleData.astroSubMeshCount; ++i )
					SetMeshDrawState( m_VehicleData.astroSubMeshList[i], false );
					
				// aqua disables
				for( i=0; i < m_VehicleData.aquaSubMeshCount; ++i )
					SetMeshDrawState( m_VehicleData.aquaSubMeshList[i], true );
					
				// aero enables
				for( i=0; i < m_VehicleData.aeroSubMeshCount; ++i )
					SetMeshDrawState( m_VehicleData.aeroSubMeshList[i], false );
			}
				
			m_InWater					= true;
			m_MaxFallVel				= m_VehicleData.maxFallVelWater;
			m_MaxXVel					= m_VehicleData.maxXVelWater;
			m_MaxYVel					= m_VehicleData.maxYVelWater;

			m_UpwardsForce				= m_VehicleData.upwardsForceWater;
			m_MoveForce					= m_VehicleData.movementForceWater;

			PhysicsWorld::GetWorld()->SetGravity( m_WaterSettings.gravity );

			m_OldAtmosType = m_AtmosType;

		}break;	
			
		default:
			break;
	}

	m_Damage = 0.0f;
	m_IsDead = false;

	m_IsContactWorld = false;
	m_IsContactPort = false;
	m_IsContactEnemy = false;
	m_IsContactShape = false;
	m_IsContactDoor = false;

	m_IsOnGroundIdle = true;

	m_IsRefueling = false;
	m_OutOfFuel = false;
	m_FuelCount = 100;
	m_FuelUseTimer = 0.0f;
	m_FuelIncreaseTimer = 0.0f;
	m_SurvivedFuelRunout = false;

	m_LandingGear = false;
	m_LandedTime = 0.0f;

	m_bInLandingZone = false;
	m_LandingType = Level::ePortType_UnKnown;
	m_LandingZoneIndex = (~0);

	m_Damage = 0;
	m_IsDead = false;
	m_AllowCollisionSound = true;

	m_ThrustTiltAngle = 0.0f;
	m_ThrustTiltTarget = 0.0f;

	m_HasPassenger = false;
	m_HasCargo = false;
	m_CargoDestination = -1;
	m_CargoTextureId = renderer::INVALID_OBJECT;

	m_SplashInTime = 0.0f;
	m_SplashOutTime = 0.0f;
	m_InWater = false;

	m_ShowCrash = false;
	m_CrashTime = 0.0f;
	m_ShowPortNumber = false;
	m_PortNumberTime = 0.0f;
	m_PortRequest = 0;

	m_ShowCustomerReward = false;
	m_CustomerRewardTime = 0.0f;
	m_CustomerReward = 0;

	m_ShowExtraLifeReward = false;
	m_ExtraLifeRewardTime = 0.0f;
	m_ExtraLifeReward = 0;

	m_HasPassenger = false;
	m_HasCargo = false;
	m_CargoDestination = -1;
	m_CargoTextureId = renderer::INVALID_OBJECT;
}

/////////////////////////////////////////////////////
/// Method: RemovePassenger
/// Params: None
///
/////////////////////////////////////////////////////
inline void Player::RemovePassenger()
{
	m_HasPassenger = false;
}

/////////////////////////////////////////////////////
/// Method: HasPassenger
/// Params: None
///
/////////////////////////////////////////////////////
inline bool Player::HasPassenger()
{
	return m_HasPassenger;
}

/////////////////////////////////////////////////////
/// Method: RemoveCargo
/// Params: None
///
/////////////////////////////////////////////////////
inline void Player::RemoveCargo()
{
	m_HasCargo = false;
	m_CargoDestination = -1;
	m_CargoTextureId = renderer::INVALID_OBJECT;
}

/////////////////////////////////////////////////////
/// Method: HasCargo
/// Params: None
///
/////////////////////////////////////////////////////
inline bool Player::HasCargo()
{
	return m_HasCargo;
}

/////////////////////////////////////////////////////
/// Method: GetCargoDestination
/// Params: None
///
/////////////////////////////////////////////////////
inline int Player::GetCargoDestination()
{
	return m_CargoDestination;
}

/////////////////////////////////////////////////////
/// Method: GetLivesCount
/// Params: None
///
/////////////////////////////////////////////////////
inline int Player::GetLivesCount()
{
	return m_LivesCount;
}

/////////////////////////////////////////////////////
/// Method: GetMoneyToAdd
/// Params: None
///
/////////////////////////////////////////////////////
inline int Player::GetMoneyToAdd()
{
	return m_MoneyToAdd;
}

/////////////////////////////////////////////////////
/// Method: SetTotalMoney
/// Params: [in]amount
///
/////////////////////////////////////////////////////
inline void Player::SetTotalMoney( int amount )
{
	m_TotalMoney = amount;
}

/////////////////////////////////////////////////////
/// Method: GetTotalMoney
/// Params: None
///
/////////////////////////////////////////////////////
inline int Player::GetTotalMoney()
{
	return m_TotalMoney;
}

/////////////////////////////////////////////////////
/// Method: AddMoney
/// Params: [in]amount, [in]rewardMoney
///
/////////////////////////////////////////////////////
inline void Player::AddMoney( int amount, bool rewardMoney )
{
	m_MoneyToAdd += amount;

	if( rewardMoney )
	{
		m_ShowCustomerReward = true;
		m_CustomerReward = amount;
		m_CustomerRewardTime = CUSTOMER_REWARD_TIMER;
	}
}

/////////////////////////////////////////////////////
/// Method: ShowExtraLife
/// Params: None
///
/////////////////////////////////////////////////////
inline void Player::ShowExtraLife( )
{
	m_ShowExtraLifeReward = true;
	m_ExtraLifeRewardTime = EXTRA_LIFE_REWARD_TIMER;
}

#endif // __PLAYER_H__
