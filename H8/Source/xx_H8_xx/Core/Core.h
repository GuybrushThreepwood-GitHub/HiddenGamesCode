
/*===================================================================
	File: Core.h
	Game: H8

	(C)Hidden Games
=====================================================================*/

#ifndef __CORE_H__
#define __CORE_H__

#include "PhysicsBase.h"

#include "InputSystem/InputSystem.h"
#include "Audio/AudioSystem.h"

#include "ScriptAccess/ScriptDataHolder.h"
#include "ScriptAccess/ScriptAccess.h"

#include "Effects/Emitter.h"

#include "Physics/PhysicsIds.h"

const int CORE_HIT_AUDIO_COUNT = 4;

class Core : public physics::PhysicsIdentifier
{
	public:
		enum PupilState
		{
			PupilState_Normal=0,			// normal full health
			PupilState_Angry_ShieldHit,		// shield mismatch
			PupilState_Wince_CoreHit,		// core hit/petal lost
			PupilState_Heart,				//
			PupilState_Sad_LastPetal,		//
			PupilState_Death
		};

	public:
		Core();
		~Core();

		void Setup( float radius );

		void Draw();
		void DrawDebug();
		void Update( float deltaTime );

		void Enable();
		void Disable();

		void Reset();

		void TakeDamage();

		void SetColour( int colourIndex, const math::Vec3& colour );
		void SetActivePupil( int meshId );
		void SetPupilState( PupilState state, float blinkTime=0.0f );

		void NextPupil();

		void PlayCoreHitAudio();

		int GetPetalCount()					{ return m_PetalCount; }

		bool IsEnabled()					{ return m_IsEnabled; }
		int GetColourIndex()				{ return m_ColourIndex; }

	private:
		float m_LastDelta;

		ScriptDataHolder::DevScriptData m_DevData;
		GameData m_GameData;

		ScriptDataHolder* m_pScriptData;

		math::Vec3 m_StartPosition;

		mdl::ModelHGM* m_pBaseModel;
		mdl::ModelHGM* m_pPetalsModel;
		mdl::ModelHGM* m_pVeinsModel;
		mdl::ModelHGM* m_pPupilsModel;

		b2Body* m_Body;

		bool m_IsEnabled;
		int m_ColourIndex;
		math::Vec3 m_Colour;

		int m_PetalCount;

		// pupil
		PupilState m_PupilState;
		PupilState m_NextState;

		int m_ActivePupil;
		float m_BlinkTime;

		bool m_DoLook;
		float m_RandomLookTime;
		float m_RandomLookTarget;
		math::Vec3 m_CoreRotation;

		int m_BlinkState; // -1 = idle, 0 = started, 1 = complete

		ALuint m_BlinkAudio;
};

#endif // __CORE_H__
