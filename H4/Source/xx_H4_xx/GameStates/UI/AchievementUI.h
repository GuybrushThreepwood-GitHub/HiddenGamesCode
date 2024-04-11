

/*===================================================================
	File: AchievementUI.h
	Game: H4

	(C)Hidden Games
=====================================================================*/

#ifndef __ACHIEVEMENTUI_H__
#define __ACHIEVEMENTUI_H__

#include <queue>
#include "GameStates/UI/UIFileLoader.h"

class AchievementUI
{
	public:
		AchievementUI();
		~AchievementUI();

		void Update( float deltaTime );
		void Draw();

		void Show( int achievementId );

	protected:
		float m_LastDeltaTime;

		mdl::ModelHGM* m_UIMesh;
		UIFileLoader m_Elements;

		std::queue<int> m_AchievementsToShow;

		ALuint m_BufferId;
		GameData m_GameData;

		math::Vec3 m_IconStartPos;
		math::Vec3 m_IconCurPos;

		float m_ShowTimer;
		float m_MaxXPos;
		float m_MovementSpeed;
		float m_CurrentShowTimer;

		bool m_IsIdle;
		bool m_MoveOut;
		bool m_MoveIn;
};

#endif // __ACHIEVEMENTUI_H__

