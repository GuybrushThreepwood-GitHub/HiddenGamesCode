
/*===================================================================
	File: H8.h
	Game: H8

	(C)Hidden Games
=====================================================================*/

#ifndef __H8_H__
#define __H8_H__

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

class H8 : public core::app::App
{
	public:
		H8();
		~H8();

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
};

#endif // __H8_H__

