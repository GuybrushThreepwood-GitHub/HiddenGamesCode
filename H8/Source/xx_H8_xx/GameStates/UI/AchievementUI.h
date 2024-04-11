

/*===================================================================
	File: AchievementUI.h
	Game: H8

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

		void Show( int achievementId, int textId );
		void Show( int achievementId, const char* text );

	protected:
		float m_LastDeltaTime;

		mdl::ModelHGM* m_UIMesh;
		UIFileLoader m_Elements;

		struct Popup
		{
			int achievementId;
			int textId;
			char textAsString[64];
		};
		std::queue<Popup> m_AchievementsToShow;

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
		bool m_ShowAchievementIcon;

		int textAlpha;
};

#endif // __ACHIEVEMENTUI_H__

