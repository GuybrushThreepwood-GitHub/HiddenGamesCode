

/*===================================================================
	File: FrontendMainMenuUI.h
	Game: AirCadets

	(C)Hidden Games
=====================================================================*/

#ifndef __FRONTENDMAINMENUUI_H__
#define __FRONTENDMAINMENUUI_H__

#include "Support/PurchaseCommon.h"
#include "GameStates/UI/UIBaseState.h"
#include "GameStates/IBaseGameState.h"

#include "GameStates/UI/UIFileLoader.h"
#include "Profiles/ProfileManager.h"

class FrontendMainMenuUI : public UIBaseState, public IBaseGameState, public support::PurchaseCallback
{
	public:
		enum FrontendStates
		{
			FrontendStates_MainMenu,
			FrontendStates_Credits
		};

	public:
		FrontendMainMenuUI( StateManager& stateManager, InputSystem& inputSystem, FrontendStates state );
		virtual ~FrontendMainMenuUI();

		virtual void Enter();
		virtual void Exit();
		virtual void PrepareTransitionIn();
		virtual int TransitionIn();
		virtual void PrepareTransitionOut();
		virtual int TransitionOut();

		virtual void Update( float deltaTime );
		virtual void Draw();
	
		// support::PurchaseCallback
		virtual void Success( const char* purchaseId );
		virtual void Failure( const char* purchaseId, const char* errorString, int errorId );
		virtual void Restored( const char* purchaseId );
		virtual void RestoreComplete();
		virtual void RestoreFailed( int errorId );

		FrontendStates GetFrontendState()			{ return m_FrontendState; }

	private:
		void SetupMainMenu();
		void SetupCredits();

		int UpdateMainMenu( float deltaTime );
		int UpdateCredits( float deltaTime );

	protected:
		float m_LastDeltaTime;
		
		mdl::ModelHGM* m_UIMesh;
		UIFileLoader m_Elements;

		ScriptDataHolder* m_pScriptData;
		GameData m_GameData;
		ScriptDataHolder::DevScriptData m_DevData;

		ProfileManager::Profile* m_pProfile;
		FrontendStates m_FrontendState;
		
		std::vector<ScriptDataHolder::LevelPackData* > m_PackList;
		ProfileManager::LevelBest* m_pBestData;
	
		// manual fade
		bool m_ManualFade;
		FrontendStates m_NextFrontendState;
		FrontendStates m_ReturnFrontendState;

		float m_CadetChangeTime;
		int m_CurrentCadet;
		int m_NextCadet;
		int m_CadetAlpha;

		bool m_WaitingForPurchaseFeedback;
};

#endif // __FRONTENDMAINMENUUI_H__

