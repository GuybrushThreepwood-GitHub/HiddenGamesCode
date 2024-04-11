

/*===================================================================
	File: UnlockUI.h
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#ifndef __UNLOCKUI_H__
#define __UNLOCKUI_H__

// forward declare
#include "GameStates/UI/UIBaseState.h"
#include "GameStates/IBaseGameState.h"

#include "GameStates/UI/UIFileLoader.h"
#include "Profiles/ProfileManager.h"

class UnlockUI : public UIBaseState, public IBaseGameState
{
	public:
		UnlockUI( StateManager& stateManager, InputSystem& inputSystem, ProfileManager::eGameMode mode );
		virtual ~UnlockUI();

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
		
		ProfileManager::Profile* m_pProfile;
		ProfileManager::eGameMode m_Mode;
		GameData m_GameData;

		mdl::ModelHGM* m_UIMesh;
		UIFileLoader m_Elements;
};

#endif // __UNLOCKUI_H__

