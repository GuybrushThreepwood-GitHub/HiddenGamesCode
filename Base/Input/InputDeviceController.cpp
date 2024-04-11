
/*===================================================================
	File: InputDeviceController.cpp
	Library: Input

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "Input/InputDeviceController.h"

using input::InputDeviceController;

#ifdef BASE_SUPPORT_SDL

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
InputDeviceController::InputDeviceController(SDL_JoystickID instanceID, SDL_Joystick* joystick, SDL_Haptic* pHaptic)
{
	m_ControllerType = CONTROLLERTYPE_JOYSTICK;
	m_pJoystick = joystick;
	m_pGamepad = 0;
	m_pHaptic = pHaptic;
	m_RumbleReady = false;
	m_IsBindScanning = false;

	m_HapticSupportFlags = 0;

	std::memset(m_szName, 0, sizeof(char) * 64);
	std::memset(m_szGUID, 0, sizeof(char) * 64);

	if (m_pJoystick)
	{
		m_InstanceId = instanceID;

		m_NumButtons = SDL_JoystickNumButtons(m_pJoystick);
		m_NumAxes = SDL_JoystickNumAxes(m_pJoystick);
		m_NumHats = SDL_JoystickNumHats(m_pJoystick);
		m_NumBalls = SDL_JoystickNumBalls(m_pJoystick);

		snprintf(m_szName, 64, SDL_JoystickName(m_pJoystick));

		SDL_JoystickGUID tmpGUID = SDL_JoystickGetGUID(m_pJoystick);
		SDL_JoystickGetGUIDString(tmpGUID, m_szGUID, 64);

		if (m_pHaptic)
		{
			m_HapticSupportFlags = SDL_HapticQuery(m_pHaptic);

			if (m_HapticSupportFlags & SDL_HAPTIC_CONSTANT)
				DBGLOG("\nSDL_HAPTIC_CONSTANT supported\n");
			if (m_HapticSupportFlags & SDL_HAPTIC_SINE)
				DBGLOG("SDL_HAPTIC_SINE supported\n");
			if (m_HapticSupportFlags & SDL_HAPTIC_LEFTRIGHT)
				DBGLOG("SDL_HAPTIC_LEFTRIGHT supported\n");
			if (m_HapticSupportFlags & SDL_HAPTIC_TRIANGLE)
				DBGLOG("SDL_HAPTIC_TRIANGLE supported\n");
			if (m_HapticSupportFlags & SDL_HAPTIC_SAWTOOTHUP)
				DBGLOG("SDL_HAPTIC_SAWTOOTHUP supported\n");
			if (m_HapticSupportFlags & SDL_HAPTIC_SAWTOOTHDOWN)
				DBGLOG("SDL_HAPTIC_SAWTOOTHDOWN supported\n");
			if (m_HapticSupportFlags & SDL_HAPTIC_RAMP)
				DBGLOG("SDL_HAPTIC_RAMP supported\n");
			if (m_HapticSupportFlags & SDL_HAPTIC_SPRING)
				DBGLOG("SDL_HAPTIC_SPRING supported\n");
			if (m_HapticSupportFlags & SDL_HAPTIC_DAMPER)
				DBGLOG("SDL_HAPTIC_DAMPER supported\n");
			if (m_HapticSupportFlags & SDL_HAPTIC_INERTIA)
				DBGLOG("SDL_HAPTIC_INERTIA supported\n");
			if (m_HapticSupportFlags & SDL_HAPTIC_FRICTION)
				DBGLOG("SDL_HAPTIC_FRICTION supported\n");
			if (m_HapticSupportFlags & SDL_HAPTIC_CUSTOM)
				DBGLOG("SDL_HAPTIC_CUSTOM supported\n");
			if (m_HapticSupportFlags & SDL_HAPTIC_GAIN)
				DBGLOG("SDL_HAPTIC_GAIN supported\n");
			if (m_HapticSupportFlags & SDL_HAPTIC_AUTOCENTER)
				DBGLOG("SDL_HAPTIC_AUTOCENTER supported\n");
			if (m_HapticSupportFlags & SDL_HAPTIC_STATUS)
				DBGLOG("SDL_HAPTIC_STATUS supported\n");
			if (m_HapticSupportFlags & SDL_HAPTIC_PAUSE)
				DBGLOG("SDL_HAPTIC_PAUSE supported\n");

			/*SDL_HapticEffect effect;

			memset(&effect, 0, sizeof(SDL_HapticEffect)); // 0 is safe default
			effect.type = SDL_HAPTIC_SINE;
			effect.periodic.direction.type = SDL_HAPTIC_POLAR; // Polar coordinates
			effect.periodic.direction.dir[0] = 18000; // Force comes from south
			effect.periodic.period = 1000; // 1000 ms
			effect.periodic.magnitude = 20000; // 20000/32767 strength
			effect.periodic.length = 5000; // 5 seconds long
			effect.periodic.attack_length = 1000; // Takes 1 second to get max strength
			effect.periodic.fade_length = 1000; // Takes 1 second to fade away

			// Upload the effect
			int effect_id = SDL_HapticNewEffect(m_pHaptic, &effect);

			// Test the effect
			SDL_HapticRunEffect(m_pHaptic, effect_id, 1);
			SDL_Delay(5000); // Wait for the effect to finish

			// We destroy the effect, although closing the device also does this
			SDL_HapticDestroyEffect(m_pHaptic, effect_id);*/

			if (SDL_HapticRumbleSupported(m_pHaptic) == SDL_TRUE)
			{ 
				if (SDL_HapticRumbleInit(m_pHaptic) == 0)
				{
					m_RumbleReady = true;
				}
			}
		}
	}
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
InputDeviceController::InputDeviceController(SDL_JoystickID instanceID, SDL_GameController* gamepad, SDL_Haptic* pHaptic)
{
	m_ControllerType = CONTROLLERTYPE_GAMEPAD;
	m_pJoystick = 0;
	m_pGamepad = gamepad;
	m_pHaptic = pHaptic;
	m_RumbleReady = false;
	m_IsBindScanning = false;

	m_HapticSupportFlags = 0;

	std::memset(m_szName, 0, sizeof(char) * 64);
	std::memset(m_szGUID, 0, sizeof(char) * 64);

	if (m_pGamepad)
	{
		m_InstanceId = instanceID;

		m_NumButtons = SDL_CONTROLLER_BUTTON_MAX;
		m_NumAxes = SDL_CONTROLLER_AXIS_MAX;
		m_NumHats = 1;
		m_NumBalls = 0;

		snprintf(m_szName, 64, SDL_GameControllerName(m_pGamepad));

		//SDL_JoystickGUID tmpGUID = SDL_JoystickGetGUID(m_pJoystick);
		//SDL_JoystickGetGUIDString(tmpGUID, m_szGUID, 64);

		if (m_pHaptic)
		{
			if (SDL_HapticRumbleSupported(m_pHaptic) == SDL_TRUE)
			{
				if (SDL_HapticRumbleInit(m_pHaptic) == 0)
				{
					m_RumbleReady = true;
				}
			}
		}
	}
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
InputDeviceController::~InputDeviceController()
{
	if (m_pHaptic != 0)
	{
		SDL_HapticStopAll(m_pHaptic);

		SDL_HapticClose(m_pHaptic);
		m_pHaptic = 0;

		m_RumbleReady = false;
	}

	if (m_pJoystick)
	{
		SDL_JoystickClose(m_pJoystick);
		m_pJoystick = 0;
	}

	if (m_pGamepad)
	{
		SDL_GameControllerClose(m_pGamepad);
		m_pGamepad = 0;
	}
}

/////////////////////////////////////////////////////
/// Method: DoRumble
/// 
///
/////////////////////////////////////////////////////
void InputDeviceController::DoRumble(float strength, unsigned int length)
{
	if (m_RumbleReady &&
		m_pHaptic)
	{
		SDL_HapticRumblePlay(m_pHaptic, strength, length);
	}
}

#else // no SDL

/////////////////////////////////////////////////////
/// Default Constructor
///
///
/////////////////////////////////////////////////////
InputDeviceController::InputDeviceController(SDL_JoystickID instanceID, SDL_Joystick* joystick, SDL_Haptic* pHaptic)
{

}

/////////////////////////////////////////////////////
/// Default Constructor
///
///
/////////////////////////////////////////////////////
InputDeviceController::InputDeviceController(SDL_JoystickID instanceID, SDL_GameController* gamepad, SDL_Haptic* pHaptic)
{

}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
InputDeviceController::~InputDeviceController()
{

}

/////////////////////////////////////////////////////
/// Method: DoRumble
/// 
///
/////////////////////////////////////////////////////
void InputDeviceController::DoRumble(float strength, unsigned int length)
{

}

#endif // BASE_SUPPORT_SDL
