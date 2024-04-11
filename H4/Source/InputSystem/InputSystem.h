
#ifndef __INPUTSYSTEM_H__
#define __INPUTSYSTEM_H__

#include "InputBase.h"

const float ANALOGUE_RANGE_SMALL = 128.0f;
const float ANALOGUE_RANGE_LARGE = 128.0f;

class InputSystem
{
	public:
		InputSystem();
		~InputSystem();

		void SetAnalogueVal( float val );
		float GetAnalogueVal();
	
		void SetUp( bool state );
		bool GetUp();
		void SetUpTouchIndex( int index );
		int GetUpTouchIndex();

		void SetUpVal( float val );
		float GetUpVal();
	
		void SetDown( bool state );
		bool GetDown();		
		
		void SetDownVal( float val );
		float GetDownVal();
	
		void SetLeft( bool state );
		bool GetLeft();

		void SetLeftVal( float val );
		float GetLeftVal();
	
		void SetRight( bool state );
		bool GetRight();

		void SetRightVal( float val );
		float GetRightVal();
	
		void SetAction( bool state );
		bool GetAction();

		void SetUse( bool state );
		bool GetUse();

		void SetRun( bool state );
		bool GetRun();

		/////////////////////////
	
		void SetLookUp( bool state );
		bool GetLookUp();

		void SetLookUpVal( float val );
		float GetLookUpVal();
	
		void SetLookDown( bool state );
		bool GetLookDown();	

		void SetLookDownVal( float val );
		float GetLookDownVal();

		void SetRotateLeft( bool state );
		bool GetRotateLeft();

		void SetRotateLeftVal( float val );
		float GetRotateLeftVal();
	
		void SetRotateRight( bool state );
		bool GetRotateRight();

		void SetRotateRightVal( float val );
		float GetRotateRightVal();
	
		//////////////////////////
	
		void SetWeaponSwap( bool state );
		bool GetWeaponSwap();

		void SetReload( bool state );
		bool GetReload();

		void SetDebugCamSwap( bool state );
		bool GetDebugCamSwap();

		void SetDebug1( bool state );
		bool GetDebug1();

		bool GetEscape();

	private:
		bool m_Up;
		float m_UpVal;
		int m_UpTouchIndex;
	
		bool m_Down;
		float m_DownVal;
	
		bool m_Left;
		float m_LeftVal;
	
		bool m_Right;
		float m_RightVal;
	
		bool m_Action;
		bool m_Use;
		bool m_Run;

		bool m_LookUp;
		float m_LookUpVal;
	
		bool m_LookDown;
		float m_LookDownVal;
	
		bool m_RotateLeft;
		float m_RotateLeftVal;
	
		bool m_RotateRight;
		float m_RotateRightVal;
	
		bool m_WeaponSwap;
		bool m_Reload;

		bool m_DebugCameraSwap;
		bool m_Debug1;

		float m_AnalogueVal;
	
		input::Input m_Input;

};

/////////////////////////////////////////////////////
/// Method: SetAnalogueVal
/// 
///
/////////////////////////////////////////////////////
inline void InputSystem::SetAnalogueVal( float val )
{
	m_AnalogueVal = val;
}

/////////////////////////////////////////////////////
/// Method: GetAnalogueVal
/// 
///
/////////////////////////////////////////////////////
inline float InputSystem::GetAnalogueVal()
{
	return m_AnalogueVal;
}

/////////////////////////////////////////////////////
/// Method: SetUp
/// 
///
/////////////////////////////////////////////////////
inline void InputSystem::SetUp( bool state )
{
	if( m_Up != state )
		m_Up = state;
}

/////////////////////////////////////////////////////
/// Method: GetUp
/// 
///
/////////////////////////////////////////////////////
inline bool InputSystem::GetUp( )
{
	if( core::app::SupportsHardwareKeyboard() )
	{
		if( m_Input.IsKeyPressed( input::KEY_W ) )
			return(true);
	}

	return m_Up;
}

/////////////////////////////////////////////////////
/// Method: SetUpTouchIndex
/// 
///
/////////////////////////////////////////////////////
inline void InputSystem::SetUpTouchIndex( int index )
{
	m_UpTouchIndex = index;
}

/////////////////////////////////////////////////////
/// Method: GetUpTouchIndex
/// 
///
/////////////////////////////////////////////////////
inline int InputSystem::GetUpTouchIndex( )
{	
	return m_UpTouchIndex;
}

/////////////////////////////////////////////////////
/// Method: SetUpVal
/// 
///
/////////////////////////////////////////////////////
inline void InputSystem::SetUpVal( float val )
{
	m_UpVal = val;
}

/////////////////////////////////////////////////////
/// Method: GetUpVal
/// 
///
/////////////////////////////////////////////////////
inline float InputSystem::GetUpVal( )
{
	if( core::app::SupportsHardwareKeyboard() )
	{
		if( m_Input.IsKeyPressed( input::KEY_W ) )
			return(ANALOGUE_RANGE_LARGE);
	}
	
	return m_UpVal;
}

/////////////////////////////////////////////////////
/// Method: SetDown
/// 
///
/////////////////////////////////////////////////////
inline void InputSystem::SetDown( bool state )
{
	if( m_Down != state )
		m_Down = state;
}

/////////////////////////////////////////////////////
/// Method: GetDown
/// 
///
/////////////////////////////////////////////////////
inline bool InputSystem::GetDown( )
{
	if( core::app::SupportsHardwareKeyboard() )
	{
		if( m_Input.IsKeyPressed( input::KEY_S ) )
			return(true);
	}

	return m_Down;
}

/////////////////////////////////////////////////////
/// Method: SetDownVal
/// 
///
/////////////////////////////////////////////////////
inline void InputSystem::SetDownVal( float val )
{
	m_DownVal = val;
}

/////////////////////////////////////////////////////
/// Method: GetDownVal
/// 
///
/////////////////////////////////////////////////////
inline float InputSystem::GetDownVal( )
{
	if( core::app::SupportsHardwareKeyboard() )
	{
		if( m_Input.IsKeyPressed( input::KEY_S ) )
			return(-ANALOGUE_RANGE_LARGE);
	}
	
	return m_DownVal;
}

/////////////////////////////////////////////////////
/// Method: SetLeft
/// 
///
/////////////////////////////////////////////////////
inline void InputSystem::SetLeft( bool state )
{
	if( m_Left != state )
		m_Left = state;
}

/////////////////////////////////////////////////////
/// Method: GetLeft
/// 
///
/////////////////////////////////////////////////////
inline bool InputSystem::GetLeft( )
{
	if( core::app::SupportsHardwareKeyboard() )
	{
		if( m_Input.IsKeyPressed( input::KEY_A ) )
			return(true);
	}

	return m_Left;
}

/////////////////////////////////////////////////////
/// Method: SetLeftVal
/// 
///
/////////////////////////////////////////////////////
inline void InputSystem::SetLeftVal( float val )
{
	m_LeftVal = val;
}

/////////////////////////////////////////////////////
/// Method: GetLeftVal
/// 
///
/////////////////////////////////////////////////////
inline float InputSystem::GetLeftVal( )
{
	if( core::app::SupportsHardwareKeyboard() )
	{
		if( m_Input.IsKeyPressed( input::KEY_A ) )
			return(-ANALOGUE_RANGE_LARGE);
	}
	
	return m_LeftVal;
}

/////////////////////////////////////////////////////
/// Method: SetRight
/// 
///
/////////////////////////////////////////////////////
inline void InputSystem::SetRight( bool state )
{
	if( m_Right != state )
		m_Right = state;
}

/////////////////////////////////////////////////////
/// Method: GetRight
/// 
///
/////////////////////////////////////////////////////
inline bool InputSystem::GetRight( )
{
	if( core::app::SupportsHardwareKeyboard() )
	{
		if( m_Input.IsKeyPressed( input::KEY_D ) )
			return(true);
	}

	return m_Right;
}

/////////////////////////////////////////////////////
/// Method: SetRightVal
/// 
///
/////////////////////////////////////////////////////
inline void InputSystem::SetRightVal( float val )
{
	m_RightVal = val;
}

/////////////////////////////////////////////////////
/// Method: GetRightVal
/// 
///
/////////////////////////////////////////////////////
inline float InputSystem::GetRightVal( )
{
	if( core::app::SupportsHardwareKeyboard() )
	{
		if( m_Input.IsKeyPressed( input::KEY_D ) )
			return(ANALOGUE_RANGE_LARGE);
	}
	
	return m_RightVal;
}

/////////////////////////////////////////////////////
/// Method: SetAction
/// 
///
/////////////////////////////////////////////////////
inline void InputSystem::SetAction( bool state )
{
	if( m_Action != state )
		m_Action = state;
}

/////////////////////////////////////////////////////
/// Method: GetAction
/// 
///
/////////////////////////////////////////////////////
inline bool InputSystem::GetAction( )
{
	if( core::app::SupportsHardwareKeyboard() )
	{
		if( m_Input.IsKeyPressed( input::KEY_SPACE, true ) )
			return(true);
	}

	return m_Action;
}

/////////////////////////////////////////////////////
/// Method: SetUse
/// 
///
/////////////////////////////////////////////////////
inline void InputSystem::SetUse( bool state )
{
	if( m_Use != state )
		m_Use = state;
}

/////////////////////////////////////////////////////
/// Method: GetUse
/// 
///
/////////////////////////////////////////////////////
inline bool InputSystem::GetUse( )
{
	if( core::app::SupportsHardwareKeyboard() )
	{
		if( m_Input.IsKeyPressed( input::KEY_E, true ) )
			return(true);
	}

	return m_Use;
}

/////////////////////////////////////////////////////
/// Method: SetRun
/// 
///
/////////////////////////////////////////////////////
inline void InputSystem::SetRun( bool state )
{
	if( m_Run != state )
		m_Run = state;
}

/////////////////////////////////////////////////////
/// Method: GetRun
/// 
///
/////////////////////////////////////////////////////
inline bool InputSystem::GetRun( )
{
	if( core::app::SupportsHardwareKeyboard() )
	{
		if( m_Input.IsKeyPressed( input::KEY_LSHIFT ) )
			return(true);
	}

	return m_Run;
}

/////////////////////////////////////////////////////
/// Method: SetLookUp
/// 
///
/////////////////////////////////////////////////////
inline void InputSystem::SetLookUp( bool state )
{
	if( m_LookUp != state )
		m_LookUp = state;
}

/////////////////////////////////////////////////////
/// Method: GetLookUp
/// 
///
/////////////////////////////////////////////////////
inline bool InputSystem::GetLookUp( )
{
	if( core::app::SupportsHardwareKeyboard() )
	{
		if( m_Input.IsKeyPressed( input::KEY_UPARROW ) )
			return(true);
	}

	return m_LookUp;
}

/////////////////////////////////////////////////////
/// Method: SetLookUpVal
/// 
///
/////////////////////////////////////////////////////
inline void InputSystem::SetLookUpVal( float val )
{
	m_LookUpVal = val;
}

/////////////////////////////////////////////////////
/// Method: GetLookUpVal
/// 
///
/////////////////////////////////////////////////////
inline float InputSystem::GetLookUpVal( )
{
	if( core::app::SupportsHardwareKeyboard() )
	{
		if( m_Input.IsKeyPressed( input::KEY_UPARROW ) )
			return(ANALOGUE_RANGE_LARGE);
	}
	
	return m_LookUpVal;
}

/////////////////////////////////////////////////////
/// Method: SetLookDown
/// 
///
/////////////////////////////////////////////////////
inline void InputSystem::SetLookDown( bool state )
{
	if( m_LookDown != state )
		m_LookDown = state;
}

/////////////////////////////////////////////////////
/// Method: GetLookDown
/// 
///
/////////////////////////////////////////////////////
inline bool InputSystem::GetLookDown( )
{
	if( core::app::SupportsHardwareKeyboard() )
	{
		if( m_Input.IsKeyPressed( input::KEY_DOWNARROW ) )
			return(true);
	}

	return m_LookDown;
}

/////////////////////////////////////////////////////
/// Method: SetLookDownVal
/// 
///
/////////////////////////////////////////////////////
inline void InputSystem::SetLookDownVal( float val )
{
	m_LookDownVal = val;
}

/////////////////////////////////////////////////////
/// Method: GetLookDownVal
/// 
///
/////////////////////////////////////////////////////
inline float InputSystem::GetLookDownVal( )
{
	if( core::app::SupportsHardwareKeyboard() )
	{
		if( m_Input.IsKeyPressed( input::KEY_DOWNARROW ) )
			return(ANALOGUE_RANGE_LARGE);
	}
	
	return m_LookDownVal;
}

/////////////////////////////////////////////////////
/// Method: SetRotateLeft
/// 
///
/////////////////////////////////////////////////////
inline void InputSystem::SetRotateLeft( bool state )
{
	if( m_RotateLeft != state )
		m_RotateLeft = state;
}

/////////////////////////////////////////////////////
/// Method: GetRotateLeft
/// 
///
/////////////////////////////////////////////////////
inline bool InputSystem::GetRotateLeft( )
{
	if( core::app::SupportsHardwareKeyboard() )
	{
		if( m_Input.IsKeyPressed( input::KEY_LEFTARROW ) )
			return(true);
	}

	return m_RotateLeft;
}

/////////////////////////////////////////////////////
/// Method: SetRotateLeftVal
/// 
///
/////////////////////////////////////////////////////
inline void InputSystem::SetRotateLeftVal( float val )
{
	m_RotateLeftVal = val;
}

/////////////////////////////////////////////////////
/// Method: GetRotateLeftVal
/// 
///
/////////////////////////////////////////////////////
inline float InputSystem::GetRotateLeftVal( )
{
	if( core::app::SupportsHardwareKeyboard() )
	{
		if( m_Input.IsKeyPressed( input::KEY_LEFTARROW ) )
			return(ANALOGUE_RANGE_LARGE);
	}
	
	return m_RotateLeftVal;
}

/////////////////////////////////////////////////////
/// Method: SetRotateRight
/// 
///
/////////////////////////////////////////////////////
inline void InputSystem::SetRotateRight( bool state )
{
	if( m_RotateRight != state )
		m_RotateRight = state;
}

/////////////////////////////////////////////////////
/// Method: GetRotateRight
/// 
///
/////////////////////////////////////////////////////
inline bool InputSystem::GetRotateRight( )
{
	if( core::app::SupportsHardwareKeyboard() )
	{
		if( m_Input.IsKeyPressed( input::KEY_RIGHTARROW ) )
			return(true);
	}

	return m_RotateRight;
}

/////////////////////////////////////////////////////
/// Method: SetRotateRightVal
/// 
///
/////////////////////////////////////////////////////
inline void InputSystem::SetRotateRightVal( float val )
{
	m_RotateRightVal = val;
}

/////////////////////////////////////////////////////
/// Method: GetRotateRightVal
/// 
///
/////////////////////////////////////////////////////
inline float InputSystem::GetRotateRightVal( )
{
	if( core::app::SupportsHardwareKeyboard() )
	{
		if( m_Input.IsKeyPressed( input::KEY_RIGHTARROW ) )
			return(ANALOGUE_RANGE_LARGE);
	}
	
	return m_RotateRightVal;
}

/////////////////////////////////////////////////////
/// Method: SetWeaponSwap
/// 
///
/////////////////////////////////////////////////////
inline void InputSystem::SetWeaponSwap( bool state )
{
	if( m_WeaponSwap != state )
		m_WeaponSwap = state;
}

/////////////////////////////////////////////////////
/// Method: GetWeaponSwap
/// 
///
/////////////////////////////////////////////////////
inline bool InputSystem::GetWeaponSwap( )
{
	if( core::app::SupportsHardwareKeyboard() )
	{
		if( m_Input.IsKeyPressed( input::KEY_1, true ) )
			return(true);
	}

	return m_WeaponSwap;
}

/////////////////////////////////////////////////////
/// Method: SetReload
/// 
///
/////////////////////////////////////////////////////
inline void InputSystem::SetReload( bool state )
{
	if( m_Reload != state )
		m_Reload = state;
}

/////////////////////////////////////////////////////
/// Method: GetReload
/// 
///
/////////////////////////////////////////////////////
inline bool InputSystem::GetReload( )
{
	if( core::app::SupportsHardwareKeyboard() )
	{
		if( m_Input.IsKeyPressed( input::KEY_R, true ) )
			return(true);
	}

	return m_Reload;
}

/////////////////////////////////////////////////////
/// Method: SetDebugCamSwap
/// 
///
/////////////////////////////////////////////////////
inline void InputSystem::SetDebugCamSwap( bool state )
{
	if( m_DebugCameraSwap != state )
		m_DebugCameraSwap = state;
}

/////////////////////////////////////////////////////
/// Method: GetDebugCamSwap
/// 
///
/////////////////////////////////////////////////////
inline bool InputSystem::GetDebugCamSwap( )
{
	if( core::app::SupportsHardwareKeyboard() )
	{
		if( m_Input.IsKeyPressed( input::KEY_2, true ) )
			return(true);
	}

	return m_DebugCameraSwap;
}

/////////////////////////////////////////////////////
/// Method: SetDebug1
/// 
///
/////////////////////////////////////////////////////
inline void InputSystem::SetDebug1( bool state )
{
	if( m_Debug1 != state )
		m_Debug1 = state;
}

/////////////////////////////////////////////////////
/// Method: GetDebug1
/// 
///
/////////////////////////////////////////////////////
inline bool InputSystem::GetDebug1( )
{
	if( core::app::SupportsHardwareKeyboard() )
	{
		if( m_Input.IsKeyPressed( input::KEY_F1, true ) )
			return(true);
	}

	return m_Debug1;
}

/////////////////////////////////////////////////////
/// Method: GetEscape
/// 
///
/////////////////////////////////////////////////////
inline bool InputSystem::GetEscape()
{
	if( core::app::SupportsHardwareKeyboard() )
	{
		if( m_Input.IsKeyPressed( input::KEY_ESCAPE ) )
			return(true);
		else
			return(false);
	}

	return false;
}

#endif // __INPUTSYSTEM_H__

