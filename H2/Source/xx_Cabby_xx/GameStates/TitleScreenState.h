
/*===================================================================
	File: TitleScreenState.h
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#ifndef __TITLESCREENSTATE_H__
#define __TITLESCREENSTATE_H__

#include "ScriptAccess/ScriptDataHolder.h"

// forward declare
class IState;
class IBaseGameState;

class TitleScreenState : public IBaseGameState
{
	public:
		TitleScreenState( StateManager& stateManager, InputSystem& inputSystem );
		virtual ~TitleScreenState();

		virtual void Enter();
		virtual void Exit();
		virtual int TransitionIn();
		virtual int TransitionOut();

		virtual void Update( float deltaTime );
		virtual void Draw();

		void SetShowModelState( bool state, int modelIndex ) 
		{ 
			m_ModelShowState = state; 
			m_ModelIndex = modelIndex;
		}
		void ClearVehicleModel( );
		void LoadRequest( int packIndex, int itemIndex );

	private:
		float m_LastDelta;

		bool m_ModelShowState;

		int m_ModelIndex;
		mdl::ModelHGM* m_QMarkModel;
		mdl::ModelHGM* m_RestoreModel;
		mdl::ModelHGM* m_LevelPackModel;

		mdl::ModelHGM* m_pVehicleModel;
		float m_Rotation;
};

#endif // __TITLESCREENSTATE_H__

