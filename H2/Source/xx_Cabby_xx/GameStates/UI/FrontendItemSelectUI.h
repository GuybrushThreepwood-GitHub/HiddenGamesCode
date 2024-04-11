

/*===================================================================
	File: FrontendItemSelectUI.h
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#ifndef __FRONTENDITEMSELECTUI_H__
#define __FRONTENDITEMSELECTUI_H__

// forward declare
#include "GameStates/UI/UIBaseState.h"
#include "GameStates/IBaseGameState.h"

#include "GameStates/UI/UIFileLoader.h"
#include "Profiles/ProfileManager.h"

class FrontendItemSelectUI : public UIBaseState, public IBaseGameState
{
	public:
		enum FrontendItemSelectStates
		{
			FrontendItemSelectStates_None,
			FrontendItemSelectStates_TaxiSelect,
			FrontendItemSelectStates_LevelSelect,
		};

	public:
		FrontendItemSelectUI( StateManager& stateManager, InputSystem& inputSystem, FrontendItemSelectStates state );
		virtual ~FrontendItemSelectUI();

		virtual void Enter();
		virtual void Exit();
		virtual void PrepareTransitionIn();
		virtual int TransitionIn();
		virtual void PrepareTransitionOut();
		virtual int TransitionOut();

		virtual void Update( float deltaTime );
		virtual void Draw();

	private:
		void SetupTaxiSelect();
		void SetupLevelSelect();

		int UpdateTaxiSelect( float deltaTime );
		int UpdateLevelSelect( float deltaTime );
		int VehicleSelected();

	protected:
		float m_LastDeltaTime;

		mdl::ModelHGM* m_UIMesh;
		UIFileLoader m_Elements;

		ScriptDataHolder* m_pScriptData;
		GameData m_GameData;
		
		ProfileManager::Profile* m_pProfile;
		std::vector<ScriptDataHolder::VehiclePackData* > m_VehiclePackList;
		std::vector<ScriptDataHolder::LevelPackData* > m_LevelPackList;

		ProfileManager::ArcadeBest* m_pArcadeBest;

		// manual fade
		bool m_ManualFade;
		FrontendItemSelectStates m_FrontendState;
		FrontendItemSelectStates m_NextFrontendState;
		FrontendItemSelectStates m_ReturnFrontendState;

		bool m_ChangedPack;
		
		// level select
		int m_LevelPackIndex;
		int m_LevelSelection;
		int m_SelectedLevelPack;
		int m_SelectedLevel;
		bool m_ChangedLevel;

		// vehicle
		int m_VehiclePackIndex;
		int m_VehicleSelection;
		int m_SelectedVehiclePack;
		int m_SelectedVehicle;
		bool m_ChangedVehicle;
};

#endif // __FRONTENDITEMSELECTUI_H__

