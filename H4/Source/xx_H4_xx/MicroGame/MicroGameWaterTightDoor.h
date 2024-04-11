
#ifndef __MICROGAMEWATERTIGHTDOOR_H__
#define __MICROGAMEWATERTIGHTDOOR_H__

#include "MicroGame.h"
#include "GameStates/UI/UIFileLoader.h"

class MicroGameWaterTightDoor : public MicroGame
{
	public:
		MicroGameWaterTightDoor( );
		virtual ~MicroGameWaterTightDoor();

		void Initialise();
		void Release();

		void OnEnter();
		void OnExit();

		void Update( float deltaTime );
		void Draw();

		bool IsComplete()		{ return m_Complete; }
		bool ReturnCode()		{ return m_ReturnCode; }

	private:
		mdl::ModelHGM* m_DoorModel;
		UIFileLoader m_DoorUI;

		ALuint m_TurnAudio;
		float m_AudioPlayValue;

		math::Vec3 m_WheelStartPos;

		float m_Range;

		int m_LastVal;
		float m_LastAngle;

		int m_TurnValue;
		int m_TurnAngle;

};

#endif // __MICROGAMEWATERTIGHTDOOR_H__
