

#ifndef __MICROGAMEKEYPAD_H__
#define __MICROGAMEKEYPAD_H__

#include "MicroGame.h"
#include "GameStates/UI/UIFileLoader.h"

const int NUM_KEYPAD_DIGITS = 5;
const int NUM_KEYPAD_CHARS = NUM_KEYPAD_DIGITS+1;

class MicroGameKeypad : public MicroGame
{
	public:
		MicroGameKeypad( unsigned int correctCode );
		virtual ~MicroGameKeypad();

		void Initialise();
		void Release();

		void OnEnter();
		void OnExit();

		void Update( float deltaTime );
		void Draw();

		bool IsComplete()		{ return m_Complete; }
		bool ReturnCode()		{ return m_ReturnCode; }

	private:
		mdl::ModelHGM* m_KeypadModel;
		UIFileLoader m_KeypadUI;

		ALuint m_KeyAudio;
		ALuint m_KeyEnterAudio;

		collision::Sphere m_TouchPos;

		unsigned int m_CorrectCode;

		int m_TotalDigits;
		char m_DigitString[NUM_KEYPAD_CHARS];

		int m_ClrPressCount;
		int m_ZeroPressCount;
		int m_OnePressCount;
		int m_TwoPressCount;
		int m_ThreePressCount;
		int m_FourPressCount;
		int m_FivePressCount;
		int m_SixPressCount;
		int m_SevenPressCount;
		int m_EightPressCount;
		int m_NinePressCount;

		bool m_ShowCorrect;
		bool m_ShowIncorrect;
		float m_ShowTimer;

		bool m_WaitForRelease;

};

#endif // __MICROGAMEKEYPAD_H__

