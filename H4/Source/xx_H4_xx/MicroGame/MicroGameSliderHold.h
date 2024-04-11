
#ifndef __MICROGAMESLIDERHOLD_H__
#define __MICROGAMESLIDERHOLD_H__

#include "MicroGame.h"
#include "GameStates/UI/UIFileLoader.h"

class MicroGameSliderHold : public MicroGame
{
	public:
		MicroGameSliderHold( unsigned int slider1, unsigned int slider2, unsigned int slider3 );
		virtual ~MicroGameSliderHold();

		void Initialise();
		void Release();

		void OnEnter();
		void OnExit();

		void Update( float deltaTime );
		void Draw();

		bool IsComplete()		{ return m_Complete; }
		bool ReturnCode()		{ return m_ReturnCode; }

	private:
		mdl::ModelHGM* m_SliderModel;
		UIFileLoader m_SliderUI;

		ALuint m_TickAudio;
		ALuint m_ComputerLoopAudio;
		ALuint m_ComputerLoopSource;

		unsigned int m_Slider1;
		unsigned int m_Slider2;
		unsigned int m_Slider3;

		int m_Range;

		math::Vec3 m_Slider1StartPos;
		math::Vec3 m_Slider2StartPos;
		math::Vec3 m_Slider3StartPos;

		int m_Slider1Value;
		int m_Slider2Value;
		int m_Slider3Value;

		float m_HoldTimer;
		float m_TickTime;

		math::Vec3 m_ScrollTextPos;
		math::Vec3 m_ScrollText;
};

#endif // __MICROGAMESLIDERHOLD_H__
