
/*===================================================================
	File: IntroOutroState.h
	Game:

	(C)Hidden Games
=====================================================================*/

#ifndef __INTROOUTROSTATE_H__
#define __INTROOUTROSTATE_H__

#include "GameStates/UI/UIFileLoader.h"

#include "GameSystems.h"

// forward declare
class IState;
class IBaseGameState;

class IntroOutroState : public IBaseGameState
{	
	public:
		enum IntroOutro
		{
			IntroOutroState_Intro=0,
			IntroOutroState_Outro
		};

	private:
		// load states
		enum LoadState
		{
			LoadState_None=-1,
			LoadState_TransitionIn=0,
			LoadState_TransitionOut
		};

	public:
		IntroOutroState( IntroOutro state, StateManager& stateManager, InputSystem& inputSystem );
		virtual ~IntroOutroState();

		virtual void Enter();
		virtual void Exit();
		virtual int TransitionIn();
		virtual int TransitionOut();

		virtual void Update( float deltaTime );
		virtual void Draw();

	private:
		void DrawIntroState();
		void DrawOutroState();

		void UpdateIntroState( float deltaTime );
		void UpdateOutroState( float deltaTime );

		bool ScreenTap();

		bool UpdateLoadState();
		void SetupRank();
		void SetupGeneralData();

	private:
		float m_LastDelta;
		bool m_IntroToLoad;
		
		IntroOutro m_CurrentMode;

		LoadState m_LoadTransitionState;
		int m_TransitionAlpha;

		mdl::ModelHGM* m_EndingMesh;
		mdl::ModelHGM* m_RankMesh;

		UIFileLoader m_RankElements;

		mdl::ModelHGM* m_DocsModels;
		UIFileLoader m_DocsUI;

		ALuint m_PageFlip;

		float m_RankBreakDownTimer;
		int m_BreakDownIndex;
		ALuint m_BreakDownAudio;

		float m_ThunderTimer;
		float m_ThunderPlayTimer;
		ALuint m_WindAudio;
		ALuint m_ThunderAudio;

		int m_CurrentPage;
		int m_NumPages;
		int m_CurrentRank;

		bool m_QuitToFrontend;
		bool m_GoToDocs;
		bool m_ShowOutroRank;
		bool m_ShowOutroDocs;

};

#endif // __INTROOUTROSTATE_H__

