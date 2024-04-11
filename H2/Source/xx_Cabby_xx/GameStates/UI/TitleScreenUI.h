

/*===================================================================
	File: TitleScreenUI.h
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#ifndef __TITLESCREENUI_H__
#define __TITLESCREENUI_H__

// forward declare
#include "GameStates/UI/UIBaseState.h"
#include "GameStates/IBaseGameState.h"

#include "GameStates/UI/UIFileLoader.h"

const int TOTAL_TITLE_ICONS = 10;

class TitleScreenUI : public UIBaseState, public IBaseGameState
{
	public:
		TitleScreenUI( StateManager& stateManager, InputSystem& inputSystem );
		virtual ~TitleScreenUI();

		virtual void Enter();
		virtual void Exit();
		virtual void PrepareTransitionIn();
		virtual int TransitionIn();
		virtual void PrepareTransitionOut();
		virtual int TransitionOut();

		virtual void Update( float deltaTime );
		virtual void Draw();

	protected:
		struct TVehicleIcon
		{
			bool unlocked;
			math::Vec3 screenPos;
			ALuint soundSourceId;
			float gain;
			float sinVal;
			float sinVal2;
			bool allowAudio;
			float speed;
			int index;
		};

		float m_LastDeltaTime;

		mdl::ModelHGM* m_UIMesh;
		UIFileLoader m_Elements;

		GLuint m_Background;

		TVehicleIcon m_VehicleIcon[TOTAL_TITLE_ICONS];
};

#endif // __TITLESCREENUI_H__

