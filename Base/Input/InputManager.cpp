
/*===================================================================
	File: InputManager.cpp
	Library: Input

	(C)Hidden Games
=====================================================================*/



#include "Core/CoreDefines.h"
#include "Script/input/InputAccess.h"
#include "Input/InputManager.h"

using input::InputManager;

InputManager* InputManager::ms_Instance = 0;

/////////////////////////////////////////////////////
/// Method: Initialise
/// Params: None
///
/////////////////////////////////////////////////////
void InputManager::Initialise(void)
{
	DBG_ASSERT((ms_Instance == 0));

	DBG_MEMTRY
		ms_Instance = new InputManager;
	DBG_MEMCATCH
}

/////////////////////////////////////////////////////
/// Method: Shutdown
/// Params: None
///
/////////////////////////////////////////////////////
void InputManager::Shutdown(void)
{
	if (ms_Instance)
	{
		delete ms_Instance;
		ms_Instance = 0;
	}
}

/////////////////////////////////////////////////////
/// Default constructor
/// 
///
/////////////////////////////////////////////////////
InputManager::InputManager()
{
	m_EventCallback = 0;
}

/////////////////////////////////////////////////////
/// Default destructor
/// 
///
/////////////////////////////////////////////////////
InputManager::~InputManager()
{
	Release();
}

/////////////////////////////////////////////////////
/// Method: Init
/// Params: None
///
/////////////////////////////////////////////////////
void InputManager::Init(void)
{
	m_DeviceList.clear();

	//script::RegisterInputFunctions();
}

/////////////////////////////////////////////////////
/// Method: Release
/// Params:
///
/////////////////////////////////////////////////////
void InputManager::Release(void)
{
	for (int i = 0; i < m_DeviceList.size(); ++i)
	{
		InputDeviceController *pObj = m_DeviceList[i];
		delete pObj;
	}

	m_DeviceList.clear();
}


/////////////////////////////////////////////////////
/// Method: Release
/// Params:
///
/////////////////////////////////////////////////////
void InputManager::Update(float deltaTime)
{

}

#ifdef BASE_SUPPORT_SDL

/////////////////////////////////////////////////////
/// Method: AddDevice
/// Params: None
///
/////////////////////////////////////////////////////
void InputManager::AddDevice(int whichId)
{
	SDL_Joystick* joy = SDL_JoystickOpen(whichId);

	if (joy)
	{
		SDL_JoystickID instanceID = SDL_JoystickInstanceID(joy);
		SDL_Haptic *pHaptic = 0;

		if (SDL_JoystickIsHaptic(joy))
			pHaptic = SDL_HapticOpen(whichId);

		if (SDL_IsGameController(whichId))
		{
			SDL_GameController* controller = SDL_GameControllerOpen(whichId);
			AddGamepad(instanceID, controller, pHaptic);

			SDL_JoystickClose(joy);
		}
		else
			AddJoystick(instanceID, joy, pHaptic);
	}
}

/////////////////////////////////////////////////////
/// Method: RemoveDevice
/// Params: None
///
/////////////////////////////////////////////////////
void InputManager::RemoveDevice(SDL_JoystickID instanceID)
{
	std::vector<InputDeviceController *>::iterator it = m_DeviceList.begin();

	InputDeviceController* pObjToDelete = 0;

	while (it != m_DeviceList.end())
	{
		if ((*it)->GetInstanceId() == instanceID)
		{
			pObjToDelete = *it;
			it = m_DeviceList.erase(it);
		}
		else
			it++;
	}

	if (pObjToDelete)
	{
		if (m_EventCallback != 0)
			m_EventCallback->DeviceRemoved(pObjToDelete);

		delete pObjToDelete;
		pObjToDelete = 0;
	}
}

/////////////////////////////////////////////////////
/// Method: DeviceStillExists
/// Params: None
///
/////////////////////////////////////////////////////
bool InputManager::DeviceStillExists(input::InputDeviceController *pDevice)
{
	if (pDevice == 0)
		return false;

	if (m_DeviceList.size() == 0)
		return false;

	const char* szName = pDevice->GetName();
	int instanceID = pDevice->GetInstanceId();

	if (szName == 0)
		return false;

	std::vector<input::InputDeviceController *>::iterator it = m_DeviceList.begin();
	while (it != m_DeviceList.end())
	{
		if (((*it)->GetInstanceId() == instanceID) ||
			(strcmp((*it)->GetName(), szName) == 0))
		{
			return true;
		}
		it++;
	}

	return false;
}

/////////////////////////////////////////////////////
/// Method: AddJoystick
/// Params: None
///
/////////////////////////////////////////////////////
void InputManager::AddJoystick(SDL_JoystickID instanceID, SDL_Joystick* pJoy, SDL_Haptic* pHaptic)
{
	if (pJoy)
	{
		const char* szName = SDL_JoystickName(pJoy);

		if (szName == 0)
			return;

		bool exists = false;

		std::vector<InputDeviceController *>::iterator it = m_DeviceList.begin();
		while (it != m_DeviceList.end())
		{
			if (((*it)->GetInstanceId() == instanceID) ||
				(strcmp((*it)->GetName(), szName) == 0))
			{
				exists = true;
			}
			it++;
		}

		if (!exists)
		{
			InputDeviceController *pNewJoy = new InputDeviceController(instanceID, pJoy, pHaptic);
			m_DeviceList.push_back(pNewJoy);

			if (m_EventCallback != 0)
				m_EventCallback->DeviceAdded(pNewJoy);
		}
	}
}

/////////////////////////////////////////////////////
/// Method: AddGamepad
/// Params: None
///
/////////////////////////////////////////////////////
void InputManager::AddGamepad(SDL_JoystickID instanceID, SDL_GameController* pController, SDL_Haptic* pHaptic)
{
	if (pController)
	{
		const char* szName = SDL_GameControllerName(pController);

		if (szName == 0)
			return;

		bool exists = false;

		std::vector<InputDeviceController *>::iterator it = m_DeviceList.begin();
		while (it != m_DeviceList.end())
		{
			if (((*it)->GetInstanceId() == instanceID) ||
				(strcmp((*it)->GetName(), szName) == 0))
			{
				exists = true;
			}
			it++;
		}

		if (!exists)
		{
			InputDeviceController *pNewJoy = new InputDeviceController(instanceID, pController, pHaptic);
			m_DeviceList.push_back(pNewJoy);

			if (m_EventCallback != 0)
				m_EventCallback->DeviceAdded(pNewJoy);
		}
	}
}

/////////////////////////////////////////////////////
/// Method: ButtonEvent
/// Params: None
///
/////////////////////////////////////////////////////
input::InputDeviceController* InputManager::FindDevice(int instanceID)
{
	std::vector<input::InputDeviceController *>::iterator it = m_DeviceList.begin();
	while (it != m_DeviceList.end())
	{
		if (((*it)->GetInstanceId() == instanceID))
		{
			return (*it);
		}
		it++;
	}

	return 0;
}

/////////////////////////////////////////////////////
/// Method: ButtonEvent
/// Params: None
///
/////////////////////////////////////////////////////
void InputManager::ButtonEvent(int instanceID, int buttonIndex, bool state)
{
	if (m_EventCallback != 0)
	{
		input::InputDeviceController* pDevice = FindDevice(instanceID);

		if (pDevice != 0)
		{
			if (state)
				m_EventCallback->InputControllerButtonDown(pDevice, buttonIndex);
			else
				m_EventCallback->InputControllerButtonUp(pDevice, buttonIndex);
		}
	}
}

/////////////////////////////////////////////////////
/// Method: AxisEvent
/// Params: None
///
/////////////////////////////////////////////////////
void InputManager::AxisEvent(int instanceID, int axisIndex, signed short value)
{
	if (m_EventCallback != 0)
	{
		input::InputDeviceController* pDevice = FindDevice(instanceID);

		if (pDevice != 0)
			m_EventCallback->InputControllerAxisMotion(pDevice, axisIndex, value);
	}
}

/////////////////////////////////////////////////////
/// Method: HatEvent
/// Params: None
///
/////////////////////////////////////////////////////
void InputManager::HatEvent(int instanceID, unsigned char value)
{
    if (m_EventCallback != 0)
    {
        input::InputDeviceController* pDevice = FindDevice(instanceID);
        
        if (pDevice != 0)
            m_EventCallback->InputControllerHatMotion(pDevice, value);
    }
}

/////////////////////////////////////////////////////
/// Method: BindCheck
/// Params: None
///
/////////////////////////////////////////////////////
bool InputManager::BindCheck(int btnBind, input::InputDeviceController *pPreferredDevice)
{
	if (pPreferredDevice != 0)
	{
		if (pPreferredDevice->GetType() == input::InputDeviceController::CONTROLLERTYPE_GAMEPAD)
		{
			SDL_GameController *controller = pPreferredDevice->GetGamePadPtr();

			if (controller)
			{
				if (btnBind >= SDL_CONTROLLER_BUTTON_A &&
					btnBind < SDL_CONTROLLER_BUTTON_MAX)
					return (SDL_GameControllerGetButton(controller, static_cast<SDL_GameControllerButton>(btnBind)) != 0);
			}
		}
		else
		if (pPreferredDevice->GetType() == input::InputDeviceController::CONTROLLERTYPE_JOYSTICK)
		{
			SDL_Joystick *joy = pPreferredDevice->GetJoystickPtr();

			if (joy)
			{
				int numButtons = SDL_JoystickNumButtons(joy);
				if (btnBind >= 0 &&
					btnBind < numButtons)
				{
					int state = SDL_JoystickGetButton(joy, btnBind);
					return (state != 0);
				}
			}
		}
	}
	else
	{
		std::vector<input::InputDeviceController *>::iterator it = m_DeviceList.begin();
		while (it != m_DeviceList.end())
		{
			InputDeviceController *pCurrentDevice = (*it);
			if (pCurrentDevice)
			{
				if (pCurrentDevice->GetType() == input::InputDeviceController::CONTROLLERTYPE_GAMEPAD)
				{
					SDL_GameController *controller = pCurrentDevice->GetGamePadPtr();

					if (controller)
					{
						if (btnBind >= SDL_CONTROLLER_BUTTON_A &&
							btnBind < SDL_CONTROLLER_BUTTON_MAX)
						{
							if (SDL_GameControllerGetButton(controller, static_cast<SDL_GameControllerButton>(btnBind)))
								return true;
						}
					}
				}
				else
				if (pCurrentDevice->GetType() == input::InputDeviceController::CONTROLLERTYPE_JOYSTICK)
				{
					SDL_Joystick *joy = pCurrentDevice->GetJoystickPtr();

					if (joy)
					{
						int numButtons = SDL_JoystickNumButtons(joy);
						if (btnBind >= 0 &&
							btnBind < numButtons)
						{
							if (SDL_JoystickGetButton(joy, btnBind))
								return true;							
						}
					}
				}
			}
			it++;
		}
	}
	return false;
}

#else

/////////////////////////////////////////////////////
/// Method: AddDevice
/// Params: None
///
/////////////////////////////////////////////////////
void InputManager::AddDevice(int whichId)
{
}

/////////////////////////////////////////////////////
/// Method: RemoveDevice
/// Params: None
///
/////////////////////////////////////////////////////
void InputManager::RemoveDevice(SDL_JoystickID instanceID)
{

}

/////////////////////////////////////////////////////
/// Method: DeviceStillExists
/// Params: None
///
/////////////////////////////////////////////////////
bool InputManager::DeviceStillExists(input::InputDeviceController *pDevice)
{
    return false;
}

/////////////////////////////////////////////////////
/// Method: AddJoystick
/// Params: None
///
/////////////////////////////////////////////////////
void InputManager::AddJoystick(SDL_JoystickID instanceID, SDL_Joystick* pJoy, SDL_Haptic* pHaptic)
{

}

/////////////////////////////////////////////////////
/// Method: AddGamepad
/// Params: None
///
/////////////////////////////////////////////////////
void InputManager::AddGamepad(SDL_JoystickID instanceID, SDL_GameController* pController, SDL_Haptic* pHaptic)
{

}

/////////////////////////////////////////////////////
/// Method: ButtonEvent
/// Params: None
///
/////////////////////////////////////////////////////
input::InputDeviceController* InputManager::FindDevice(int instanceID)
{
    return 0;
}

/////////////////////////////////////////////////////
/// Method: ButtonEvent
/// Params: None
///
/////////////////////////////////////////////////////
void InputManager::ButtonEvent(int instanceID, int buttonIndex, bool state)
{

}

/////////////////////////////////////////////////////
/// Method: AxisEvent
/// Params: None
///
/////////////////////////////////////////////////////
void InputManager::AxisEvent(int instanceID, int axisIndex, signed short value)
{

}

/////////////////////////////////////////////////////
/// Method: HatEvent
/// Params: None
///
/////////////////////////////////////////////////////
void InputManager::HatEvent(int instanceID, unsigned char value)
{

}

/////////////////////////////////////////////////////
/// Method: BindCheck
/// Params: None
///
/////////////////////////////////////////////////////
bool InputManager::BindCheck(int btnBind, input::InputDeviceController *pPreferredDevice)
{
    return false;
}
#endif //
