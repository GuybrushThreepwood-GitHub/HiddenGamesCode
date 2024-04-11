
#ifndef __MICROGAMELIFTBUTTONS_H__
#define __MICROGAMELIFTBUTTONS_H__

#include "MicroGame.h"
#include "GameStates/UI/UIFileLoader.h"

class MicroGameLiftButtons : public MicroGame
{
	public:
		MicroGameLiftButtons( int currentFloor,  
								bool button1Active, const char* button1Label, const char* button1Call, 
								bool button2Active, const char* button2Label, const char* button2Call, 
								bool button3Active, const char* button3Label, const char* button3Call );
		virtual ~MicroGameLiftButtons();

		void Initialise();
		void Release();

		void OnEnter();
		void OnExit();

		void Update( float deltaTime );
		void Draw();

		bool IsComplete()		{ return m_Complete; }
		bool ReturnCode()		{ return m_ReturnCode; }

	private:
		mdl::ModelHGM* m_LiftButtonsModel;
		UIFileLoader m_LiftButtonsUI;

		ALuint m_PressInactiveAudio;
		ALuint m_PressActiveAudio;

		bool m_FloorSelected;
		float m_ExitTimer;
		int m_CurrentFloor;

		bool m_CallFunc;
		const char* m_CallScriptFunc;
		int m_SelectedFloor;

		bool m_Button1Active;
		const char* m_Button1Call;
		const char* m_Button1Label;

		bool m_Button2Active;
		const char* m_Button2Call;
		const char* m_Button2Label;

		bool m_Button3Active;
		const char* m_Button3Call;
		const char* m_Button3Label;

		bool m_WaitAudioDelay;
		float m_AudioDelay;
};

#endif // __MICROGAMELIFTBUTTONS_H__
