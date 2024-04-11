
#ifndef __INPUTSYSTEM_H__
#define __INPUTSYSTEM_H__

#include "InputBase.h"

class InputSystem
{
	public:
		InputSystem();
		~InputSystem();

		void SetThrottle( bool state );
		bool GetThrottle();

		void SetAirBrake( bool state );
		bool GetAirBrake();

		void SetLandingGear( bool state );
		bool GetLandingGear();

		void SetLeft( bool state );
		bool GetLeft();

		void SetRight( bool state );
		bool GetRight();

		void SetReset( bool state );
		bool GetReset();

		void IgnoreInput( bool state );
		void ResetAllInput();

		bool GetEscape();

		void SetUILeft(bool state);
		bool GetUILeft();

		void SetUIRight(bool state);
		bool GetUIRight();

		void SetUIUp(bool state);
		bool GetUIUp();

		void SetUIDown(bool state);
		bool GetUIDown();

		void SetUISelect(bool state);
		bool GetUISelect();

		void SetPause(bool state);
		bool GetPause();

        void SetMenu(bool state);
        bool GetMenu();
	
    private:

		input::Input m_Input;

		bool m_Left;
		bool m_Right;

		bool m_Throttle;
		bool m_AirBrake;

		bool m_LandingGear;
		
		bool m_Reset;

		bool m_Ignore;

		bool m_UILeft;
		bool m_UIRight;
		bool m_UIUp;
		bool m_UIDown;
		bool m_UISelect;

		bool m_Pause;
        bool m_Menu;
};

inline void InputSystem::SetThrottle( bool state )
{
	if( m_Ignore )
		return;

	if( m_Throttle != state )
		m_Throttle = state;
}

inline bool InputSystem::GetThrottle( )
{
	if( core::app::SupportsHardwareKeyboard() )
	{
		if( m_Input.IsKeyPressed( input::KEY_UPARROW, false, true ) )
			return(true);
	}

	return m_Throttle;
}

inline void InputSystem::SetAirBrake( bool state )
{
	if( m_Ignore )
		return;

	if( m_AirBrake != state )
		m_AirBrake = state;
}
		
inline bool InputSystem::GetAirBrake()
{
	if( core::app::SupportsHardwareKeyboard() )
	{
		if( m_Input.IsKeyPressed( input::KEY_DOWNARROW, false, true ) )
			return(true);
	}

	return(m_AirBrake);
}

inline void InputSystem::SetLandingGear( bool state )
{
	if( m_Ignore )
		return;

	if( m_LandingGear != state )
		m_LandingGear = state;
}

inline bool InputSystem::GetLandingGear( )
{
	if( core::app::SupportsHardwareKeyboard() )
	{
		if( m_Input.IsKeyPressed( input::KEY_SPACE, true, true ) )
			return(true);
	}

	return m_LandingGear;
}

inline void InputSystem::SetLeft( bool state )
{
	if( m_Ignore )
		return;

	if( m_Left != state )
		m_Left = state;
}

inline bool InputSystem::GetLeft( )
{
	if( core::app::SupportsHardwareKeyboard() )
	{
		if( m_Input.IsKeyPressed( input::KEY_LEFTARROW, false, true ) )
			return(true);
	}

	return m_Left;
}

inline void InputSystem::SetRight( bool state )
{
	if( m_Ignore )
		return;

	if( m_Right != state )
		m_Right = state;
}

inline bool InputSystem::GetRight( )
{
	if( core::app::SupportsHardwareKeyboard() )
	{
		if( m_Input.IsKeyPressed( input::KEY_RIGHTARROW, false, true ) )
			return(true);
	}

	return m_Right;
}

inline void InputSystem::SetReset( bool state )
{
	if( m_Ignore )
		return;

	if( m_Reset != state )
		m_Reset = state;
}

inline bool InputSystem::GetReset( )
{
	if( core::app::SupportsHardwareKeyboard() )
	{
		if( m_Input.IsKeyPressed( input::KEY_BACKSPACE, false, true ) )
			return(true);
	}

	return m_Reset;
}

inline void InputSystem::IgnoreInput( bool state )
{
	m_Ignore = state;
	if( m_Ignore )
		ResetAllInput();
}

inline void InputSystem::ResetAllInput()
{
	m_Left = false;
	m_Right = false;
	m_Throttle = false;
	m_AirBrake = false;
	m_LandingGear = false;

	m_Reset = false;

	m_UILeft = false;
	m_UIRight = false;
	m_UIUp = false;
	m_UIDown = false;
	m_UISelect = false;
	m_Pause = false;
    m_Menu = false;
}

inline bool InputSystem::GetEscape()
{
	if( core::app::SupportsHardwareKeyboard() )
	{
		if( m_Input.IsKeyPressed( input::KEY_ESCAPE, true, true ) )
			return(true);
		else
			return(false);
	}

	return false;
}

inline void InputSystem::SetUILeft(bool state)
{
	if (m_Ignore)
		return;

	if (m_UILeft != state)
		m_UILeft = state;
}

inline bool InputSystem::GetUILeft()
{
	if (core::app::SupportsHardwareKeyboard())
	{
		if (m_Input.IsKeyPressed(input::KEY_LEFTARROW, true, true))
			return(true);
	}

	return m_UILeft;
}

inline void InputSystem::SetUIRight(bool state)
{
	if (m_Ignore)
		return;

	if (m_UIRight != state)
		m_UIRight = state;
}

inline bool InputSystem::GetUIRight()
{
	if (core::app::SupportsHardwareKeyboard())
	{
		if (m_Input.IsKeyPressed(input::KEY_RIGHTARROW, true, true))
			return(true);
	}

	return m_UIRight;
}

inline void InputSystem::SetUIUp(bool state)
{
	if (m_Ignore)
		return;

	if (m_UIUp != state)
		m_UIUp = state;
}

inline bool InputSystem::GetUIUp()
{
	if (core::app::SupportsHardwareKeyboard())
	{
		if (m_Input.IsKeyPressed(input::KEY_UPARROW, true, true))
			return(true);
	}

	return m_UIUp;
}

inline void InputSystem::SetUIDown(bool state)
{
	if (m_Ignore)
		return;

	if (m_UIDown != state)
		m_UIDown = state;
}

inline bool InputSystem::GetUIDown()
{
	if (core::app::SupportsHardwareKeyboard())
	{
		if (m_Input.IsKeyPressed(input::KEY_DOWNARROW, true, true))
			return(true);
	}

	return m_UIDown;
}

inline void InputSystem::SetUISelect(bool state)
{
	if (m_Ignore)
		return;

	if (m_UISelect != state)
		m_UISelect = state;
}

inline bool InputSystem::GetUISelect()
{
	if (core::app::SupportsHardwareKeyboard())
	{
		if (m_Input.IsKeyPressed(input::KEY_ENTER, true, true))
			return(true);
	}

	return m_UISelect;
}

inline void InputSystem::SetPause(bool state)
{
	if (m_Ignore)
		return;

	if (m_Pause != state)
		m_Pause = state;
}

inline bool InputSystem::GetPause()
{
	if (core::app::SupportsHardwareKeyboard())
	{
		if (m_Input.IsKeyPressed(input::KEY_P, true, true))
			return(true);
	}

	return m_Pause;
}

inline void InputSystem::SetMenu(bool state)
{
    if (m_Ignore)
        return;
    
    if (m_Menu != state)
        m_Menu = state;
}

inline bool InputSystem::GetMenu()
{
    if (core::app::SupportsHardwareKeyboard())
    {
        if (m_Input.IsKeyPressed(input::KEY_M, true, true))
            return(true);
    }
    
    return m_Menu;
}

#endif // __INPUTSYSTEM_H__

