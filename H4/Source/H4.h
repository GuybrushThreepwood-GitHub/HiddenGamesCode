
/*===================================================================
	File: H4.h
	Game: H4

	(C)Hidden Games
=====================================================================*/

#ifndef __H4_H__
#define __H4_H__

#ifndef __APP_H__
	#include "Core/App.h"
#endif // __APP_H__

#include "StateManage/IState.h"
#include "StateManage/StateManager.h"
#include "GameStates/IBaseGameState.h"
#include "InputSystem/InputSystem.h"

#include "ScriptAccess/ScriptDataHolder.h"
#include "GameSystems.h"

// forward declare
namespace input { class Input; }

class H4 : public core::app::App
{
	public:
		H4();
		~H4();

		virtual int Initialise( void );
		virtual int FrameMove( void );
		virtual int Render( void );
		virtual int Cleanup( void );

		void Input( void );

		static void SetHiResMode( bool flag );
		static bool GetHiResMode();
	
	private:
		InputSystem m_Input;
		StateManager m_MainStateManager;
		
		char m_ProfileName[core::MAX_PATH+core::MAX_PATH];
		ScriptDataHolder* m_pScriptHolder;

		GameSystems* m_pGameSystems;

		static bool m_HiResMode;
};

#endif // __H4_H__

