
/*===================================================================
	File: H1.h
	Game: AirCadets

	(C)Hidden Games
=====================================================================*/

#ifndef __H1_H__
#define __H1_H__

#include "CoreBase.h"

#include "StateManage/IState.h"
#include "StateManage/StateManager.h"
#include "GameStates/IBaseGameState.h"
#include "InputSystem/InputSystem.h"

#include "ScriptAccess/ScriptDataHolder.h"
#include "GameSystems.h"

// forward declare
namespace input { class CInput; }
class StateManager;

class H1 : public core::app::App
{
	public:
		H1();
		~H1();

		virtual int Initialise( void );
		virtual int FrameMove( void );
		virtual int Render( void );
		virtual int Cleanup( void );

		void Input( void );

	private:
		InputSystem m_Input;
		StateManager m_MainStateManager;
		StateManager m_UIStateManager;

		char m_ProfileName[core::MAX_PATH+core::MAX_PATH];

		ScriptDataHolder* m_pScriptHolder;
		GameSystems* m_pGameSystems;

#ifdef BASE_SUPPORT_ADBAR
		renderer::AdvertBar* m_AdBar;
#endif // BASE_SUPPORT_ADBAR
};

#endif // __H1_H__

