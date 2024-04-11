
/*===================================================================
	File: TitleScreenState.h
	Game: AirCadets

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
		void LoadRequest( int packIndex, int itemIndex, int leftOrRight );
		bool HasFinishedMove()												{ return m_FinishedMove; }

	private:
		void LoadAircraftModel();

	private:
		float m_LastDelta;

		bool m_ModelShowState;
		bool m_LoadRequest;

		int m_ModelIndex;
		int m_PackIndex;
		int m_ItemIndex;
		int m_LeftOrRight;
		math::Vec3 m_PosMove;
		bool m_FinishedMove;

		mdl::ModelHGM* m_pVehicleModel;
		float m_Rotation;
};

#endif // __TITLESCREENSTATE_H__

