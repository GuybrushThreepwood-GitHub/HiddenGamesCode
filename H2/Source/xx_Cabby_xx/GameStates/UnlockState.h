
/*===================================================================
	File: UnlockState.h
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#ifndef __UNLOCKSTATE_H__
#define __UNLOCKSTATE_H__

#include "ScriptAccess/ScriptDataHolder.h"

// forward declare
class IState;
class IBaseGameState;

class UnlockState : public IBaseGameState
{
	public:
		UnlockState( StateManager& stateManager, InputSystem& inputSystem );
		virtual ~UnlockState();

		virtual void Enter();
		virtual void Exit();
		virtual int TransitionIn();
		virtual int TransitionOut();

		virtual void Update( float deltaTime );
		virtual void Draw();

	private:
		float m_LastDelta;
		ScriptDataHolder* m_pScriptData;
		GameData m_GameData;

		IState* m_pIState;

		mdl::ModelHGM* m_pVehicleModel;
		float m_Rotation;
		int m_UnlockVehicleId;

		math::Vec2 m_BGCoords[4];
};

#endif // __UNLOCKSTATE_H__

