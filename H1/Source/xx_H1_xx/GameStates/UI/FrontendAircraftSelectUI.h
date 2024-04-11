

/*===================================================================
	File: FrontendAircraftSelectUI.h
	Game: AirCadets

	(C)Hidden Games
=====================================================================*/

#ifndef __FRONTENDAIRCRAFTSELECTUI_H__
#define __FRONTENDAIRCRAFTSELECTUI_H__

// forward declare
#include "GameStates/UI/UIBaseState.h"
#include "GameStates/IBaseGameState.h"

#include "GameStates/UI/UIFileLoader.h"
#include "Profiles/ProfileManager.h"

class FrontendAircraftSelectUI : public UIBaseState, public IBaseGameState
{
	public:
		FrontendAircraftSelectUI( StateManager& stateManager, InputSystem& inputSystem);
		virtual ~FrontendAircraftSelectUI();

		virtual void Enter();
		virtual void Exit();
		virtual void PrepareTransitionIn();
		virtual int TransitionIn();
		virtual void PrepareTransitionOut();
		virtual int TransitionOut();

		virtual void Update( float deltaTime );
		virtual void Draw();
		void DrawBG();

	private:
		void SetupPlaneSelect();

		int UpdatePlaneSelect( float deltaTime );
		int VehicleSelected();

	protected:
		float m_LastDeltaTime;

		mdl::ModelHGM* m_UIMesh;
		UIFileLoader m_Elements;

		ScriptDataHolder* m_pScriptData;
		GameData m_GameData;
		
		ProfileManager::Profile* m_pProfile;
		std::vector<ScriptDataHolder::VehiclePackData* > m_VehiclePackList;

		bool m_ChangedPack;

		// vehicle
		int m_VehiclePackIndex;
		int m_VehicleSelection;
		int m_SelectedVehiclePack;
		int m_SelectedVehicle;
		bool m_ChangedVehicle;
};

#endif // __FRONTENDAIRCRAFTSELECTUI_H__

