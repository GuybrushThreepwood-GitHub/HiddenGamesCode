

/*===================================================================
	File: MainStateUI.h
	Game: H4

	(C)Hidden Games
=====================================================================*/

#ifndef __MAINSTATEUI_H__
#define __MAINSTATEUI_H__

// forward declare
#include "GameStates/UI/UIBaseState.h"
#include "GameStates/IBaseGameState.h"

#include "GameStates/UI/UIFileLoader.h"

class MainStateUI : public UIBaseState, public IBaseGameState
{
	public:
		MainStateUI( StateManager& stateManager, InputSystem& inputSystem );
		virtual ~MainStateUI();

		virtual void Enter() = 0;
		virtual void Exit() = 0;
		virtual int TransitionIn() = 0;
		virtual int TransitionOut() = 0;

		virtual void Update( float deltaTime ) = 0;
		virtual void Draw() = 0;

		virtual void SetLevelName( const char* name ) = 0;
		virtual void ClearSay() = 0;

		virtual bool IsPaused()								{ return m_Paused; }
		virtual bool HasQuit()								{ return m_Quit; }
		virtual void SetDeathContinueFlag( bool state )		{ m_ContinueFromDeath = state; }

	protected:
		virtual void UpdateLeftGameUI( float deltaTime ) = 0;
		virtual void UpdateRightGameUI( float deltaTime ) = 0;
		virtual void UpdatePauseUI( float deltaTime ) = 0;

		virtual void UpdateType1( float deltaTime ) = 0;
		virtual void UpdateType4( float deltaTime ) = 0;

		virtual void UpdateOptions() = 0;
		
	protected:
		float m_LastDeltaTime;

		bool m_LeftHanded;

		float m_LeftAnalogueAngle;
		float m_RightAnalogueAngle;

		collision::Sphere m_TouchPos;
		collision::Sphere m_LeftDeadZone;
		collision::Sphere m_RightDeadZone;

		int m_Debug1Press;

		mdl::ModelHGM* m_UIMesh;
		UIFileLoader m_Elements;

		mdl::ModelHGM* m_OptionsMesh;
		UIFileLoader m_OptionsElements;

		bool m_Paused;
		bool m_Quit;
		bool m_ContinueFromDeath;

		math::Vec3 m_TorchSway;
		math::Vec3 m_TorchPos;

		int m_Say1Index;
		int m_Say2Index;
		const char* m_SayLine1;
		const char* m_SayLine2;
		const char* m_SayCallFunc;

		int m_ControlTypeIndex;
		GameSystems::OptionsData m_CurrentOptions;
		GameSystems::GeneralData m_GeneralData;

		// noise bar
		bool m_NoiseHeld;
		math::Vec3 m_NoiseBarStartPos;
		math::Vec3 m_NoiseBarMinRange;
		math::Vec3 m_NoiseBarMaxRange;

		int m_NoiseBarRange;
		int m_NoiseBarXPos;

};

#endif // __MAINSTATEUI_H__

