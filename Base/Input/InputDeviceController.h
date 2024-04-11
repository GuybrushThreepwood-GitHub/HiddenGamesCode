
#ifndef __INPUTDEVICECONTROLLER_H__
#define __INPUTDEVICECONTROLLER_H__

#ifdef BASE_SUPPORT_SDL
    #ifndef _SDL_H
        #include "SDL.h"
    #endif // _SDL_H
#else
    typedef struct
    {
        int empty;
    }SDL_GameController;

    typedef struct
    {
        int empty;
    }SDL_Joystick;

    typedef struct
    {
        int empty;
    }SDL_Haptic;

    #define SDL_JoystickID      int

    enum {
        SDL_CONTROLLER_BUTTON_A=0,
        SDL_CONTROLLER_BUTTON_B,
        SDL_CONTROLLER_BUTTON_X,
        SDL_CONTROLLER_BUTTON_Y,
        SDL_CONTROLLER_BUTTON_BACK,
        SDL_CONTROLLER_BUTTON_GUIDE,
        SDL_CONTROLLER_BUTTON_START,
        SDL_CONTROLLER_BUTTON_LEFTSTICK,
        SDL_CONTROLLER_BUTTON_RIGHTSTICK,
        SDL_CONTROLLER_BUTTON_LEFTSHOULDER,
        SDL_CONTROLLER_BUTTON_RIGHTSHOULDER,
        SDL_CONTROLLER_BUTTON_DPAD_UP,
        SDL_CONTROLLER_BUTTON_DPAD_DOWN,
        SDL_CONTROLLER_BUTTON_DPAD_LEFT,
        SDL_CONTROLLER_BUTTON_DPAD_RIGHT,
        
        SDL_CONTROLLER_BUTTON_MAX,
        
        SDL_CONTROLLER_AXIS_LEFTX,
        SDL_CONTROLLER_AXIS_LEFTY,
        SDL_CONTROLLER_AXIS_RIGHTX,
        SDL_CONTROLLER_AXIS_RIGHTY,
        SDL_CONTROLLER_AXIS_TRIGGERLEFT,
        SDL_CONTROLLER_AXIS_TRIGGERRIGHT,
        
        SDL_HAT_CENTERED,
        SDL_HAT_UP,
        SDL_HAT_RIGHT,
        SDL_HAT_DOWN,
        SDL_HAT_LEFT,
    };
#endif // BASE_SUPPORT_SDL

namespace input
{
	class InputDeviceController
	{
		public:
			enum ControllerType
			{
				CONTROLLERTYPE_JOYSTICK,
				CONTROLLERTYPE_GAMEPAD
			};

		public:
			InputDeviceController(SDL_JoystickID instanceID, SDL_Joystick* joystick, SDL_Haptic* pHaptic = 0);
			InputDeviceController(SDL_JoystickID instanceID, SDL_GameController* gamepad, SDL_Haptic* pHaptic = 0);
			~InputDeviceController();

			void DoRumble(float strength, unsigned int length);

			int GetInstanceId()			{ return m_InstanceId; }
			ControllerType GetType()	{ return m_ControllerType; }
			const char* GetName()		{ return m_szName; }
			const char* GetGUID()		{ return m_szGUID; }

			SDL_Joystick* GetJoystickPtr()			{ return m_pJoystick; }
			SDL_GameController* GetGamePadPtr()		{ return m_pGamepad; }

			int GetNumButtons()			{ return m_NumButtons; }
			int GetNumAxes()			{ return m_NumAxes; }
			int GetNumHats()			{ return m_NumHats; }
			int GetNumBalls()			{ return m_NumBalls; }
			bool HasRumble()			{ return m_pHaptic != 0; }

			void StartBinding()			{ m_IsBindScanning = true; }
			bool IsBinding()			{ return m_IsBindScanning; }
			void StopBinding()			{ m_IsBindScanning = false; }

		private:
			int m_InstanceId;
			ControllerType m_ControllerType;
			SDL_Joystick* m_pJoystick;
			SDL_GameController* m_pGamepad;
			SDL_Haptic* m_pHaptic;
			bool m_RumbleReady;
			unsigned int m_HapticSupportFlags;

			char m_szName[64];
			char m_szGUID[64];

			int m_NumButtons;
			int m_NumAxes;
			int m_NumHats;
			int m_NumBalls;

			bool m_IsBindScanning;
	};
}

#endif // __INPUTDEVICECONTROLLER_H__

