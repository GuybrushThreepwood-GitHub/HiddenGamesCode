

/*===================================================================
	File: GameCompleteUI.h
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#ifndef __GAMECOMPLETEUI_H__
#define __GAMECOMPLETEUI_H__

// forward declare
#include "GameStates/UI/UIBaseState.h"
#include "GameStates/IBaseGameState.h"

#include "GameStates/UI/UIFileLoader.h"
#include "Profiles/ProfileManager.h"

class GameCompleteUI : public UIBaseState, public IBaseGameState
{
	public:
		GameCompleteUI( StateManager& stateManager, InputSystem& inputSystem, ProfileManager::eGameMode mode );
		virtual ~GameCompleteUI();

		virtual void Enter();
		virtual void Exit();
		virtual void PrepareTransitionIn();
		virtual int TransitionIn();
		virtual void PrepareTransitionOut();
		virtual int TransitionOut();

		virtual void Update( float deltaTime );
		virtual void Draw();

		bool FinishedFade()		{ return m_FinishedTransitionOut; }

	protected:
		float m_LastDeltaTime;

		bool m_FullClear;
		bool m_SpecialEnding;
		float m_SpecialEndingWait;
		bool m_EndingAnimComplete;

		ProfileManager::Profile* m_pProfile;
		ProfileManager::eGameMode m_Mode;
		GameData m_GameData;

		mdl::ModelHGM* m_UIMesh;
		UIFileLoader m_Elements;
};

#endif // __GAMECOMPLETEUI_H__

