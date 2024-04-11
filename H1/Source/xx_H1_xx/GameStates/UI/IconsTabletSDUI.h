

/*===================================================================
	File: IconsTabletSDUI.h
	Game: AirCadets

	(C)Hidden Games
=====================================================================*/

#ifndef __ICONSTABLETSDUI_H__
#define __ICONSTABLETSDUI_H__

// forward declare
#include "GameStates/UI/UIBaseState.h"
#include "GameStates/IBaseGameState.h"

#include "GameStates/UI/UIFileLoader.h"
#include "Level/Level.h"

class IconsTabletSDUI : public UIBaseState, public IBaseGameState
{
	public:
		IconsTabletSDUI( StateManager& stateManager, InputSystem& inputSystem );
		virtual ~IconsTabletSDUI();

		virtual void Enter();
		virtual void Exit();
		virtual void PrepareTransitionIn();
		virtual int TransitionIn();
		virtual void PrepareTransitionOut();
		virtual int TransitionOut();

		virtual void Update( float deltaTime );
		virtual void Draw();

	private:
		math::Vec3& GetSafeIconPosition( math::Vec3& iconPos, float oriWidth, float oriHeight );

	protected:
		float m_LastDeltaTime;

		mdl::ModelHGM* m_UIMesh;
		UIFileLoader m_Elements;

		Player* m_Player;
		GameData m_GameData;
		Level* m_ActiveLevel;

		int m_NumIconsDrawn;

		float m_HalfWidth;
		float m_HalfHeight;
	
		float m_OriWidth;
		float m_OriHeight;	
};

#endif // __ICONSTABLETSDUI_H__

