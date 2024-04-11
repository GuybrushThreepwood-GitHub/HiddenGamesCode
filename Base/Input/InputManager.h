
#ifndef __INPUTMANAGER_H__
#define __INPUTMANAGER_H__

#ifndef __DEBUGBASE_H__
	#include "DebugBase.h"
#endif // __DEBUGBASE_H__

#include <vector>
#include "Input/InputDeviceController.h"

namespace input
{
	class InputManager
	{
		public:
			class InputEventCallback
			{
				public:
					InputEventCallback() {}
					virtual ~InputEventCallback() {}

					virtual void DeviceAdded(input::InputDeviceController *pDevice) = 0;
					virtual void DeviceRemoved(input::InputDeviceController *pDevice) = 0;

					virtual void InputControllerButtonDown(InputDeviceController *pDevice, int buttonIndex) = 0;
					virtual void InputControllerButtonUp(InputDeviceController *pDevice, int buttonIndex) = 0;

					virtual void InputControllerAxisMotion(InputDeviceController *pDevice, int axisIndex, signed short value) = 0;
                
                    virtual void InputControllerHatMotion(InputDeviceController *pDevice, unsigned char value) = 0;
			};

		public:
			InputManager();
			~InputManager();

			/// Initialise - Sets up just the default values
			void Init(void);

			/// cleanup
			/// Release - Clears out all allocated data by the class
			void Release(void);

			static void Initialise(void);
			static void Shutdown(void);

			/// Update -
			void Update( float deltaTime );

			/// AddDevice -
			void AddDevice(int whichId);
			/// RemoveDevice -
			void RemoveDevice(SDL_JoystickID instanceID);

			/// AddDeviceEvent -
			void AddDeviceEvent(int whichId);
			/// RemoveDeviceEvent -
			void RemoveDeviceEvent(SDL_JoystickID instanceID);
			/// ButtonEvent
			void ButtonEvent(int instanceID, int buttonIndex, bool state);
			/// AxisEvent
			void AxisEvent(int instanceID, int axisIndex, signed short value);
            /// HatEvent
            void HatEvent(int instanceID, unsigned char value);

			/// DeviceStillExists -
			bool DeviceStillExists(InputDeviceController *pDevice);

			static InputManager *GetInstance(void)
			{
				DBG_ASSERT((ms_Instance != 0));

				return(ms_Instance);
			}
			static bool IsInitialised(void)
			{
				return(ms_Instance != 0);
			}

			/// GetDeviceList - get the current device list
			std::vector<InputDeviceController *> GetDeviceList()						{ return m_DeviceList; }
            /// SetCallback -
			void SetCallback(input::InputManager::InputEventCallback* pCallback)		{ m_EventCallback = pCallback;  }

			bool BindCheck(int btnBind, InputDeviceController *pPreferredDevice=0);

		private:
			/// AddJoystick -
			void AddJoystick(SDL_JoystickID instanceID, SDL_Joystick* pJoy, SDL_Haptic* pHaptic);

			/// AddGamepad -
			void AddGamepad(SDL_JoystickID instanceID, SDL_GameController* pController, SDL_Haptic* pHaptic);

			input::InputDeviceController* FindDevice(int instanceID);

		private:
			static InputManager* ms_Instance;

			InputEventCallback* m_EventCallback;

			std::vector<InputDeviceController *> m_DeviceList;

			bool m_IsBindScanning;
	};

} // namespace input

#endif // __INPUTMANAGER_H__

