

/*===================================================================
	File: FrontendInAppPurchaseUI.h
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#ifndef __FRONTENDINAPPPURCHASEUI_H__
#define __FRONTENDINAPPPURCHASEUI_H__

// forward declare
#include "GameStates/UI/UIBaseState.h"
#include "GameStates/IBaseGameState.h"

#include "GameStates/UI/UIFileLoader.h"
#include "GameStates/UI/FrontendItemSelectUI.h"
#include "Profiles/ProfileManager.h"

class FrontendInAppPurchaseUI : public UIBaseState, public IBaseGameState, public support::PurchaseCallback
{
	public:
		enum FrontendInAppPurchaseStates
		{
			FrontendInAppPurchaseStates_RestorePurchases,
			FrontendInAppPurchaseStates_InAppPurchase,
			FrontendInAppPurchaseStates_InAppPurchaseSuccess,
			FrontendInAppPurchaseStates_InAppPurchaseFailure,
			FrontendInAppPurchaseStates_InAppPurchaseWaiting,
		};

	public:
		FrontendInAppPurchaseUI( StateManager& stateManager, InputSystem& inputSystem, FrontendInAppPurchaseStates state, 
									FrontendItemSelectUI::FrontendItemSelectStates returnState, int purchaseId, int levelOrVehicle, int packIndex, int itemIndex );
		virtual ~FrontendInAppPurchaseUI();

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

		void SetModelLoaded()						{ m_WaitForModelLoad = false; }

	private:
		void SetupRestorePuchases(  );
		void SetupInAppPurchase(bool revealed);	
		void SetupInAppPurchaseSuccess();
		void SetupInAppPurchaseFailure();
		void SetupInAppPurchaseWaiting();

		int UpdateRestorePurchases( float deltaTime );
		int UpdateInAppPurchase( float deltaTime );
		int UpdateInAppPurchaseSuccess( float deltaTime );
		int UpdateInAppPurchaseFailure( float deltaTime );
		int UpdateInAppPurchaseWaiting( float deltaTime );

	protected:
		float m_LastDeltaTime;

		ScriptDataHolder* m_pScriptData;
		GameData m_GameData;
		
		ProfileManager::Profile* m_pProfile;

		mdl::ModelHGM* m_UIMesh;
		UIFileLoader m_Elements;

		// manual fade
		bool m_ManualFade;
		bool m_WaitForModelLoad;
		bool m_RequestLoad;

		FrontendInAppPurchaseStates m_FrontendState;
		FrontendInAppPurchaseStates m_NextFrontendState;

		FrontendItemSelectUI::FrontendItemSelectStates m_ReturnState;

		bool m_RestorePurchases;
		bool m_Revealed;
		int m_PurchaseIndex;
		int m_StoreErrorCode;

		int m_LevelOrVehicle;
		int m_PackIndex;
		int m_ItemIndex;
};

#endif // __FRONTENDINAPPPURCHASEUI_H__

