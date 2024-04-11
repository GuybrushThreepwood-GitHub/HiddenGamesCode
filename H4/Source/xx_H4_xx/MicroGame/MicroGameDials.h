
#ifndef __MICROGAMEDIALS_H__
#define __MICROGAMEDIALS_H__

#include "MicroGame.h"
#include "GameStates/UI/UIFileLoader.h"

class MicroGameDials : public MicroGame
{
	public:
		MicroGameDials( unsigned int dial1, unsigned int dial2, unsigned int dial3 );
		virtual ~MicroGameDials();

		void Initialise();
		void Release();

		void OnEnter();
		void OnExit();

		void Update( float deltaTime );
		void Draw();

		bool IsComplete()		{ return m_Complete; }
		bool ReturnCode()		{ return m_ReturnCode; }

	private:
		mdl::ModelHGM* m_DialsModel;
		UIFileLoader m_DialsUI;

		unsigned int m_Dial1Goal;
		unsigned int m_Dial2Goal;
		unsigned int m_Dial3Goal;

		unsigned int m_Dial1Value;
		unsigned int m_Dial2Value;
		unsigned int m_Dial3Value;

		float m_Range;

		struct Dial
		{
			int dialValue;
			int angle;
		};

		Dial m_Dials[3];
};

#endif // __MICROGAMEDIALS_H__
