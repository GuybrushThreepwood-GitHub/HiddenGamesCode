
/*===================================================================
	File: EndingState.h
	Game:

	(C)Hidden Games
=====================================================================*/

#ifndef __ENDINGSTATE_H__
#define __ENDINGSTATE_H__

#include "GameStates/UI/UIFileLoader.h"

#include "GameSystems.h"

// forward declare
class IState;
class IBaseGameState;

class EndingState : public IBaseGameState
{	
	private:
		// load states
		enum LoadState
		{
			LoadState_None=-1,
			LoadState_TransitionIn=0,
			LoadState_TransitionOut
		};

	public:
		EndingState( int mode, StateManager& stateManager, InputSystem& inputSystem );
		virtual ~EndingState();

		virtual void Enter();
		virtual void Exit();
		virtual int TransitionIn();
		virtual int TransitionOut();

		virtual void Update( float deltaTime );
		virtual void Draw();

	private:
		bool ScreenTap();
		bool UpdateLoadState();

		void ProcessStartCameras();
		void ProcessEndingCameras();

	private:
		float m_LastDelta;
		
		float m_Timer;

		bool m_GoToGameStart;
		bool m_GoToGameEnd;

		LoadState m_LoadTransitionState;
		int m_TransitionAlpha;

		mdl::ModelHGA* m_Colonel;
		mdl::ModelHGM* m_EndingMesh;

		UIFileLoader m_TextElements;

		bool m_StartZoomOut;
		int m_EndingCameraIndex;

		math::Vec3 m_CamPos;
		math::Vec3 m_CamTarget;

		math::Vec3 m_CigaretteOffset;

		efx::Emitter* m_pSmokeEmitter;
		efx::Emitter* m_pBurnEmitter;

		efx::Emitter* m_pCupEmitter;
};

#endif // __ENDINGSTATE_H__

