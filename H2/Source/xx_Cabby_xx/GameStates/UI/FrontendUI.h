

/*===================================================================
	File: FrontendUI.h
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#ifndef __FRONTENDUI_H__
#define __FRONTENDUI_H__

#include "Support/PurchaseCommon.h"
#include "GameStates/UI/UIBaseState.h"
#include "GameStates/IBaseGameState.h"

#include "GameStates/UI/UIFileLoader.h"
#include "Profiles/ProfileManager.h"

class FrontendUI : public UIBaseState, public IBaseGameState
{
	public:
		enum FrontendStates
		{
			FrontendStates_MainMenu,
			FrontendStates_CareerStats,
			FrontendStates_Credits
		};

	public:
		FrontendUI( StateManager& stateManager, InputSystem& inputSystem, FrontendStates state );
		virtual ~FrontendUI();

		virtual void Enter();
		virtual void Exit();
		virtual void PrepareTransitionIn();
		virtual int TransitionIn();
		virtual void PrepareTransitionOut();
		virtual int TransitionOut();

		virtual void Update( float deltaTime );
		virtual void Draw();
	
	private:
		void SetupMainMenu();
		void SetupCareerStats();
		void SetupCredits();

		int UpdateMainMenu( float deltaTime );
		int UpdateCareerStats( float deltaTime );
		int UpdateCredits( float deltaTime );

	protected:
		float m_LastDeltaTime;
		
		mdl::ModelHGM* m_UIMesh;
		UIFileLoader m_Elements;

		ScriptDataHolder* m_pScriptData;
		GameData m_GameData;
		
		ProfileManager::Profile* m_pProfile;
		FrontendStates m_FrontendState;
		
		std::vector<ScriptDataHolder::LevelPackData* > m_PackList;
		ProfileManager::ArcadeBest* m_pArcadeBest;
	
		// manual fade
		bool m_ManualFade;
		FrontendStates m_NextFrontendState;
		FrontendStates m_ReturnFrontendState;
};

#endif // __FRONTENDUI_H__

