
/*===================================================================
	File: Player.h
	Game: H8

	(C)Hidden Games
=====================================================================*/

#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "PhysicsBase.h"

#include "InputSystem/InputSystem.h"
#include "Audio/AudioSystem.h"

#include "ScriptAccess/ScriptDataHolder.h"
#include "ScriptAccess/ScriptAccess.h"

#include "Effects/Emitter.h"

#include "Physics/PhysicsContact.h"
#include "Physics/PhysicsIds.h"

// these have to match in scripts
enum
{
	SHAPE_TRIANGLE 	= 0,
	SHAPE_SQUARE 	= 1,
	SHAPE_HEXAGON	= 2,
	SHAPE_OCTAGON	= 3,
	SHAPE_CIRCLE	= 4,
	SHAPE_GENERIC	= 5,
};

const int AUDIO_CHIME_SUCCESS_COUNT = 25;
const int AUDIO_CHIME_SUCCESS_POOL = 5;
const int AUDIO_INOUT_COUNT = 8;

class Player : public physics::PhysicsIdentifier
{
	public:
		struct ShieldData
		{
			int colourIndex;
			int numAbsorptions;
			int numDamageHits;
		};

		enum AnimState
		{
			AnimState_Initial=-1,
			AnimState_In,		// show all pieces
			AnimState_Out,		// remove all pieces
			AnimState_InGame	// game mode
		};

	public:
		Player( InputSystem& inputSystem );
		~Player();

		void Initialise();
		void Setup( const char* shieldSetup );
		
		void Draw();
		void DrawDebug();
		void Update( float deltaTime );

		void AddShieldCollision( EnemyToShieldCollision collision );
		void AddShieldAbsorb( int colourIndex );
		void AddShieldDeflect( int colourIndex );

		//
		void NextLevel();											
		void PreviousLevel();
		void Reset();

		void SetLevelUp( bool levelupState )						{ m_LevelUpState = levelupState; }
		void SetAnimState( AnimState state );

		void PlayPositiveChime();
		void PlayNegativeChime();

		void ResetConsecutiveCount();
		void IncreaseConsecutiveCount();

		void SetInputState( bool state )							{ m_InputSystem.IgnoreInput( state ); }
		void SetShieldShape( int shape )							{ m_ShieldShape = shape; }
		int GetShieldShape()										{ return m_ShieldShape; }

		std::vector<int> GetValidColoursList()						{ return m_ValidColours; }

		void IncreaseNumProtections()								{ m_NumProtections++;	}
		int GetNumProtections()										{ return m_NumProtections; }

		void ConsecutiveProtections()								{ m_NumConsecutiveProtections++;	}
		int GetNumConsecutiveProtections()							{ return m_NumConsecutiveProtections; }

		int GetCurrentLevel()										{ return m_CurrentLevel; }
		int GetTotalNumLevels()										{ return m_TotalNumLevels; }

		AnimState GetAnimState()									{ return m_AnimState; }
		bool IsAnimComplete()										{ return m_AnimComplete; }

	private:
		void LoadShields();
		void ClearCurrentShield();
		void AddColourToValidList( int colourIndex );
		void RemoveColourFromValidList( int colourIndex );
		void AddShieldPieceData( int colourIndex );
		
	private:
		InputSystem& m_InputSystem;
		float m_LastDelta;

		ScriptDataHolder::DevScriptData m_DevData;
		GameData m_GameData;

		ScriptDataHolder* m_pScriptData;

		AnimState m_AnimState;
		bool m_AnimComplete;
		float m_AnimTimer;

		mdl::ModelHGM* m_pModelData;
		int m_NumSubMeshes;
		int m_TotalEnabledSubMeshes;

		b2Body* m_Body;
		b2RevoluteJoint* m_Joint;

		float m_RotationForce;
		bool m_Reset;
		bool m_LevelUpState;

		std::vector<EnemyToShieldCollision> m_FixturesHit;

		std::vector<int> m_ValidColours;
		std::vector<ShieldData> m_ShieldPieceData;

		int m_ShieldShape;

		int m_NumShieldPieces;
		int m_NumProtections;
		int m_NumConsecutiveProtections;
		int m_ConsecutiveMulti;

		int m_CurrentLevel;
		int m_TotalNumLevels;
		std::vector<const char*> m_LevelSetupList;

		int m_NextChime;
		bool m_ChimeUp;
		float m_PitchChange;

		ALuint m_CurrentSuccessList[AUDIO_CHIME_SUCCESS_POOL];

		int m_ShieldInOutIndex;
		ALuint m_ShieldInOutIds[AUDIO_INOUT_COUNT];
};

#endif // __PLAYER_H__
