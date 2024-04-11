
#ifndef __MICROGAMEMAP_H__
#define __MICROGAMEMAP_H__

#include "MicroGame.h"
#include "GameStates/UI/UIFileLoader.h"

class MicroGameMap : public MicroGame
{
	public:
		MicroGameMap( int mapIndex, float playerPosX, float playerPosY, float playerRot );
		virtual ~MicroGameMap();

		void Initialise();
		void Release();

		void OnEnter();
		void OnExit();

		void Update( float deltaTime );
		void Draw();

		bool IsComplete()		{ return m_Complete; }
		bool ReturnCode()		{ return m_ReturnCode; }

	private:
		mdl::ModelHGM* m_MapModel;
		UIFileLoader m_MapUI;

		ALuint m_ComputerLoopAudio;
		ALuint m_ComputerLoopSource;

		int m_MapIndex;
		float m_PlayerPosX;
		float m_PlayerPosY;
		float m_PlayerRot;

		bool m_KeyToggle;

		float m_BlinkTime;
		bool m_BlinkToggle;

		math::Vec3 m_ScrollTextPos;
		math::Vec3 m_ScrollText;
};

#endif // __MICROGAMEMAP_H__
