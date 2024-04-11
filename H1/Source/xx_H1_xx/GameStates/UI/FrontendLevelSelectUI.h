

/*===================================================================
	File: FrontendLevelSelectUI.h
	Game: AirCadets

	(C)Hidden Games
=====================================================================*/

#ifndef __FRONTENDLEVELSELECTUI_H__
#define __FRONTENDLEVELSELECTUI_H__

// forward declare
#include "GameStates/UI/UIBaseState.h"
#include "GameStates/IBaseGameState.h"

#include "GameStates/UI/UIFileLoader.h"
#include "Profiles/ProfileManager.h"

class FrontendLevelSelectUI : public UIBaseState, public IBaseGameState
{
	public:
		enum FrontendItemSelectStates
		{
			FrontendItemSelectStates_None,
			FrontendItemSelectStates_LevelSelect,
		};

	public:
		FrontendLevelSelectUI( StateManager& stateManager, InputSystem& inputSystem, FrontendItemSelectStates state );
		virtual ~FrontendLevelSelectUI();

		virtual void Enter();
		virtual void Exit();
		virtual void PrepareTransitionIn();
		virtual int TransitionIn();
		virtual void PrepareTransitionOut();
		virtual int TransitionOut();

		virtual void Update( float deltaTime );
		virtual void Draw();

	private:
		void SetupPlaneSelect();
		void SetupLevelSelect();

		int UpdatePlaneSelect( float deltaTime );
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

		ProfileManager::LevelBest* m_pBestData;

		// manual fade
		bool m_ManualFade;
		FrontendItemSelectStates m_FrontendState;
		FrontendItemSelectStates m_NextFrontendState;
		FrontendItemSelectStates m_ReturnFrontendState;

		GameSystems::GameMode m_GameMode;

		bool m_ChangedPack;
		bool m_ChangedLayout;
		
		int m_LayoutIndex;

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

#endif // __FRONTENDLEVELSELECTUI_H__

