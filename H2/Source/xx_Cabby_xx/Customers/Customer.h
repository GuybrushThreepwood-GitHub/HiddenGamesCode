
/*===================================================================
	File: Customer.h
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#ifndef __CUSTOMER_H__
#define __CUSTOMER_H__

#include "Resources/TextureResources.h"
#include "ScriptAccess/Cabby/CabbyAccess.h"

class Customer
{
	public:
		enum CustomerType
		{
			CustomerType_Unknown=-1,

			// mesh ids
			CustomerType_Parcel=0,
			CustomerType_Male=1,
			CustomerType_Female=2,

			CustomerType_Max
		};

		enum CustomerTextures
		{
			CustomerTexture_Start=1,
			CustomerTexture_End=6,
		};

		enum ParcelTextures
		{
			ParcelTexture_Start=7,
			ParcelTexture_End=10,
		};

		enum CustomerValue
		{
			CustomerValue_MaleHigh = 1,
			CustomerValue_MaleMedium = 2,
			CustomerValue_MaleLow = 3,
			CustomerValue_FemaleHigh = 4,
			CustomerValue_FemaleMedium = 5,
			CustomerValue_FemaleLow = 6,
		};

		enum eGender
		{
			CUSTOMER_MALE=0,
			CUSTOMER_FEMALE
		};

		enum CustomerState
		{
			CustomerState_InActive=0,

			// customer A-to-B
			CustomerState_Waiting,
			CustomerState_WalkToTaxi,
			CustomerState_InTaxi,
			CustomerState_DroppedOff,

			// cargo
			CustomerState_WaitingWithCargo,
			CustomerState_WaitingForCargo,
			CustomerState_WalkToTaxiWithCargo,
			CustomerState_WalkToTaxiForCargo,
			CustomerState_WalkAway,
			CustomerState_WalkAwayWithCargo,
		};

	public:
		Customer();
		~Customer();

		void SetupCustomer( CustomerType type, bool hasCargo, mdl::ModelHGA* model );
		void SetupSubmeshes();

		void PreDrawSetup();
		void Draw( bool pauseFlag );
		void Update( float deltaTime );
		void UpdateTimer( float deltaTime );
		void UpdateSkins();
		void UpdateJoints( float deltaTime );
		void UpdateFade( float deltaTime );
		void SetIdleAnim();
		void SetAnimation( unsigned int nAnimId, int nFrameOffset = -1, bool bForceChange = true );

		CustomerType GetType();
		void SetDrawState( bool state );
		bool GetDrawState();

		bool IsFat();
		bool IsAngry();
		
		void SetAngry();

		void SetCargoState( bool state, GLuint textureId=renderer::INVALID_OBJECT );
		GLuint GetCargoTextureId()		{ return m_TextureParcel; }
		void SetCargoCustomerFlag();
		bool IsCargoCustomer();

		void SetState( CustomerState state );
		CustomerState GetState();

		void SetPosition( math::Vec3& pos );
		math::Vec3& GetPosition();

		void SetDropOffWalkPoint( math::Vec3& pos );
		math::Vec3& GetDropOffWalkPoint();

		void SetSpawnZone( int zone );
		int GetSpawnZone();

		void SetDestinationZone( int zone );
		int GetDestinationZone();

		void SetSpawnNumeric( int num );
		int GetSpawnNumeric();

		void SetDestinationNumeric( int num );
		int GetDestinationNumeric();

		void SetTargetRotation( float angle );

		float GetWaitingTime();
		void ResetCheckWatchTimer();
		float GetCheckWatchTime();

		void StartCargoTravelTimer()						{ m_CargoTravelTimer = true; }

		bool GetPerfectFareFlag()							{ return m_PerfectFare; }

		collision::Sphere& GetBoundingSphere()				{ return m_BoundSphere; }
		math::Vec2& GetUnprojection()						{ return m_UnProjectedPoint; }

		int GetFareValue();
		eGender GetGender()									{ return m_Gender; }

		void SetVoicePitchTweak( float tweak )				{ m_VoicePitchTweak = tweak; }
		float GetVoicePitchTweak()							{ return m_VoicePitchTweak; }

		int GetBodyModelIndex()								{ return m_BodyModel; }
		GLuint GetTexture()									{ return m_TextureCharacter; }

	private:
		void GetValue( int texture );

	private:
		struct LocalAnim
		{
			/// time
			float animTimer;
			// animation identifier
			unsigned int nAnimId;
			/// framerate of the animation
			float fAnimationFramerate;
			/// total amount of time this animation plays for
			double dTotalAnimationTime;
			/// total number of joints exported into this file
			unsigned int nTotalJoints;
			/// total number of frames in this animation
			int nTotalFrames;
			/// timer for tracking this animation
			//core::app::CTimer animTimer;
			/// looping flag
			bool bLooping;
			/// current active frame
			int nCurrentFrame;
			/// current time
			double dCurrentTime;
			/// end of the animation time
			float fEndTime;
		};

		GameData m_GameData;

		CustomerType m_Type;
		CustomerState m_State;
		bool m_DrawState;
		float m_AlphaUpdate;
		int m_CurrentAlpha;
		math::Vec3 m_Pos;

		float m_WaitTime;
		float m_TravelTime;
		bool m_CargoTravelTimer;
		float m_CheckWatchTimer;

		math::Vec3 m_DropOffWalkPoint;

		eGender m_Gender;
		bool m_IsFat;
		int m_BodyModel;

		bool m_IsAngry;
		float m_AngryTime;

		bool m_HasCargo;
		bool m_IsCargoCustomer;

		float m_LastDeltaTime;

		mdl::ModelHGA* m_pModelData;
		/// current primary animation
		mdl::BoneAnimation *m_pPrimaryAnim;
		LocalAnim	m_LocalPrimaryAnim;

		/// current polled animation
		mdl::BoneAnimation *m_pPolledAnim;
		LocalAnim	m_LocalPolledAnim;

		unsigned int m_JointCount;
		mdl::TBoneJoint* m_pJoints;

		// idle/wait anim
		int m_IdleAnim;

		// used for culling
		collision::Sphere m_BoundSphere;
		math::Vec2 m_UnProjectedPoint;

		float m_TargetRotation;
		float m_RotationAngle;

		int m_SpawnedZone;
		int m_SpawnNumeric;
		int m_DestinationZone;
		int m_DestinationNumeric;

		GLuint m_TextureCharacter;
		GLuint m_TextureParcel;

		// money
		int m_FareValue;
		bool m_PerfectFare;

		// stored voice pitch
		float m_VoicePitchTweak;
};

inline void Customer::SetDrawState( bool state )
{
	m_DrawState = state;
}

inline bool Customer::GetDrawState()
{
	return(m_DrawState);
}

inline Customer::CustomerType Customer::GetType()
{
	return(m_Type);
}

inline bool Customer::IsFat()
{
	return(m_IsFat);
}

inline bool Customer::IsAngry()
{
	return(m_IsAngry);
}

inline void Customer::SetCargoState( bool state, GLuint textureId )
{
	m_HasCargo = state;
	if( textureId != renderer::INVALID_OBJECT )
		m_TextureParcel = textureId;
}

inline void Customer::SetCargoCustomerFlag()
{
	m_IsCargoCustomer = true;
}

inline bool Customer::IsCargoCustomer()
{
	return(m_IsCargoCustomer);
}

inline void Customer::SetState( CustomerState state )
{
	m_State = state;
}

inline Customer::CustomerState Customer::GetState()
{
	return m_State;
}

inline void Customer::SetPosition( math::Vec3& pos )
{
	m_Pos = pos;

	m_BoundSphere.vCenterPoint = m_Pos;
	m_BoundSphere.vCenterPoint.Y = (m_Pos.Y+1.4f);
}

inline math::Vec3& Customer::GetPosition()
{
	return m_Pos;
}

inline void Customer::SetDropOffWalkPoint( math::Vec3& pos )
{
	m_DropOffWalkPoint = pos;
}
		
inline math::Vec3& Customer::GetDropOffWalkPoint()
{
	return m_DropOffWalkPoint;
}

inline void Customer::SetSpawnZone( int zone )
{
	m_SpawnedZone = zone;
}

inline int Customer::GetSpawnZone()
{
	return(m_SpawnedZone);
}

inline void Customer::SetSpawnNumeric( int num )
{
	m_SpawnNumeric = num;
}

inline int Customer::GetSpawnNumeric()
{
	return(m_SpawnNumeric);
}

inline void Customer::SetDestinationZone( int zone )
{
	m_DestinationZone = zone;
}

inline int Customer::GetDestinationZone()
{
	return(m_DestinationZone);
}

inline void Customer::SetDestinationNumeric( int num )
{
	m_DestinationNumeric = num;
}

inline int Customer::GetDestinationNumeric()
{
	return(m_DestinationNumeric);
}

inline void Customer::SetTargetRotation( float angle )
{
	m_TargetRotation = angle;
}

inline float Customer::GetWaitingTime()
{
	return( m_WaitTime );
}

inline void Customer::ResetCheckWatchTimer()
{
	m_CheckWatchTimer = 0.0f;
}

inline float Customer::GetCheckWatchTime()
{
	return(m_CheckWatchTimer);
}

inline int Customer::GetFareValue()
{
	return( m_FareValue );
}

#endif // __CUSTOMER_H__

