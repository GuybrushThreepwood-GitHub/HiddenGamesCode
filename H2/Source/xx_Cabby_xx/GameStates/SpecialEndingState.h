
/*===================================================================
	File: SpecialEndingState.h
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#ifndef __SPECIALENDINGSTATE_H__
#define __SPECIALENDINGSTATE_H__

#include "ScriptAccess/ScriptDataHolder.h"

// forward declare
class IState;
class IBaseGameState;

class SpecialEndingState : public IBaseGameState
{
	public:
		SpecialEndingState( StateManager& stateManager, InputSystem& inputSystem );
		virtual ~SpecialEndingState();

		virtual void Enter();
		virtual void Exit();
		virtual int TransitionIn();
		virtual int TransitionOut();

		virtual void Update( float deltaTime );
		virtual void Draw();
		void UpdatePhysics( float deltaTime );

	private:
		float m_LastDelta;
		ScriptDataHolder* m_pScriptData;
		ScriptDataHolder::DevScriptData m_DevData;	
		ScriptDataHolder::LevelScriptData m_LevelData;
		ScriptDataHolder::LevelLightBlock m_LevelLight;
		GameData m_GameData;

		math::Vec3 m_CamPos;
		math::Vec3 m_CamLookAt;

		GameCamera* m_GameCamera;

		renderer::TGLFogState m_LevelFog;
		Player* m_Player;
		Level m_Level;

		IState* m_pIState;

		bool m_LoadedLevel;

		float m_CamMoveTimer;
};

#endif // __SPECIALENDINGSTATE_H__

