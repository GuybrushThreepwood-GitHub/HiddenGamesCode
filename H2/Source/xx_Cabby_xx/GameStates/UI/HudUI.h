

/*===================================================================
	File: HudUI.h
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#ifndef __HUDUI_H__
#define __HUDUI_H__

// forward declare
#include "GameStates/UI/UIBaseState.h"
#include "GameStates/IBaseGameState.h"

#include "GameStates/UI/UIFileLoader.h"

class HudUI : public UIBaseState, public IBaseGameState
{
	public:
		HudUI( StateManager& stateManager, InputSystem& inputSystem );
		virtual ~HudUI();

		virtual void Enter();
		virtual void Exit();
		virtual void PrepareTransitionIn();
		virtual int TransitionIn();
		virtual void PrepareTransitionOut();
		virtual int TransitionOut();

		virtual void Update( float deltaTime );
		virtual void Draw();

		void InitTowTruck();
		void UpdateTowTruck( float deltaTime );
		void DrawTowTruck();
		bool TowTruckFinished() { return m_TowTruckFinished; }

		void SetGameTime( float timer );

	protected:
		float m_LastDeltaTime;

		mdl::ModelHGM* m_UIMesh;
		UIFileLoader m_Elements;

		GameData m_GameData;
		Player* m_Player;
		ScriptDataHolder::LevelScriptData m_LevelData;

		math::Vec3 m_TowTruckPos;
		float m_TowEndX;
		bool m_TowTruckFinished;

		bool m_TowTruckHitMiddle;
};

#endif // __HUDUI_H__

