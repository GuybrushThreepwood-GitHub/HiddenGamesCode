
/*===================================================================
	File: BackgroundUI.h
	Game: H8

	(C)Hidden Games
=====================================================================*/

#ifndef __BACKGROUNDUI_H__
#define __BACKGROUNDUI_H__

// forward declare
#include "GameStates/UI/UIBaseState.h"
#include "GameStates/IBaseGameState.h"

#include "GameStates/UI/UIFileLoader.h"

class BackgroundUI
{
	public:
		BackgroundUI( );
		virtual ~BackgroundUI();

		void Enter();
		void Exit();

		void Update( float deltaTime );
		void Draw();

	protected:
		float m_LastDeltaTime;
		GameData m_GameData;

		mdl::ModelHGM* m_UIMesh;

		mdl::ModelHGM* m_HLayer1;
		math::Vec3 m_HRotation1;
		mdl::ModelHGM* m_HLayer2;
		math::Vec3 m_HRotation2;
		mdl::ModelHGM* m_HLayer3;
		math::Vec3 m_HRotation3;

		mdl::ModelHGM* m_VLayer1;
		math::Vec3 m_VRotation1;
		mdl::ModelHGM* m_VLayer2;
		math::Vec3 m_VRotation2;
		mdl::ModelHGM* m_VLayer3;
		math::Vec3 m_VRotation3;

		UIFileLoader m_Elements;
		math::Vec3 m_CylinderPos;
};

#endif // __BACKGROUNDUI_H__

