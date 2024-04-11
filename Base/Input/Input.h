
#ifndef __INPUT_H__
#define __INPUT_H__

#include "Math/Vectors.h"

namespace input
{
	const int MAX_KEYS = 256;
	const int MAX_TOUCHES = 10;
	const int FIRST_TOUCH = 0;
	const int SECOND_TOUCH = 1;
	const float HOLD_TIME = 0.2f;

	enum InputEventType
	{
		InputEventType_Press=0,
		InputEventType_Move,
		InputEventType_Tap,
		InputEventType_Release
	};

	struct TInputEvent
	{
		InputEventType type;
			
		int nTouchId;

		long nTouchX;
		long nTouchY;
	};

	struct TInputState
	{
		struct TKeyStates
		{
			// ascii table
			bool bKeys[MAX_KEYS];
			int bKeyPressTime[MAX_KEYS];

			// vk keys
			bool vkKeys[MAX_KEYS];
			int vkKeyPressTime[MAX_KEYS];
		} KeyStates;
			
		bool bKeyPressed;
		bool vkKeyPressed;
		int	nKeyPressCode;

		long nMouseX;
		long nMouseY;
		long nLastMouseX;
		long nLastMouseY;

		long nMouseXDelta;
		long nMouseYDelta;

		bool bMouseWheel;
		long nMouseWheelDelta;

		bool LButtonPressed;
		bool LButtonReleased;

		bool RButtonPressed;
		bool RButtonReleased;
        
        float upValue;
        float downValue;
        float leftValue;
        float rightValue;
		
		int nTouchCount;
		struct TouchData
		{
			bool bActive;

			bool bPress;
			bool bRelease;
			bool bHeld;

			int nTaps;
			long nTouchX;
			long nTouchY;
		
			long nLastTouchX;
			long nLastTouchY;
		
			long nXDelta;
			long nYDelta;

			int nSpecialId;

			math::Vec2 vAccumulatedVec;

			float Ticks;
		};
		
		// first index can be counted for mouse touch
		TouchData TouchesData[MAX_TOUCHES];

// ios
#if !defined( BASE_INPUT_NO_TOUCH_SUPPORT )
		UITouch* UITouchesData[MAX_TOUCHES];
#endif // BASE_INPUT_NO_TOUCH_SUPPORT

		// extends a box around the touch point
		int nTouchWidth;
		int nTouchHeight;
		
		// sphere extends from the touch
		int nTouchRadius;

		float Accelerometers[3];
	};

#if !defined( BASE_INPUT_NO_TOUCH_SUPPORT )
	void AddTouch( CGRect bounds, UITouch* touch, CGPoint location );
	void TouchMoved( CGRect bounds, UITouch* touch, CGPoint location );
	void RemoveTouch( UITouch* touch );
#endif // BASE_INPUT_NO_TOUCH_SUPPORT
	void UpdateTouches( float deltaTime );
	
#ifdef BASE_PLATFORM_ANDROID	
	void AddTouch( bool rotationFlag, int pointerId, short x, short y );
	void TouchMoved( bool rotationFlag, int pointerId, short x, short y );
	void AddTap( int pointerId );
	void RemoveTouch( int pointerId );
	void UpdateTouches( float deltaTime );
#endif // BASE_PLATFORM_ANDROID

	void ClearTouchDeltas();
	void DirtyTouchTicks();
	void ClearAllTouches();
	void DrawTouches();

	class Input
	{
		public:
			/// default constructor
			Input();
			/// default destructor
			~Input();

			/// IsKeyPressed - Is a certain key being pressed
			/// \param nKeyCode - key to check
			/// \param bIgnoreHeld - returns false if a key was held
			/// \return bool - ( SUCCESS: true or FAIL: false )
			bool IsKeyPressed( int nKeyCode, int bIgnoreHeld=false, bool specialKeyOnly=false );
			/// IsKeyPressed - Is a certain key being pressed
			/// \param cKey - key to check
			/// \param bIgnoreHeld - returns false if a key was held
			/// \return bool - ( SUCCESS: true or FAIL: false )
			//bool IsKeyPressed( char cKey, int bIgnoreHeld=false, bool specialKeyOnly=false );
			/// IsLeftMouseButtonPressed - Is the the left mouse button pressed
			/// \return bool - ( SUCCESS: true or FAIL: false )
			bool IsLeftMouseButtonPressed( void );
			/// IsRightMouseButtonPressed - Is the the right mouse button pressed
			/// \return bool - ( SUCCESS: true or FAIL: false )
			bool IsRightMouseButtonPressed( void );
			/// GetMousePosition - Returns the current mouse position
			/// \param pMouseX - X screen position of the mouse
			/// \param pMouseY - Y screen position of the mouse
			void GetMousePosition( long *pMouseX, long *pMouseY );
		
			/// SetMouseCursorPosition - Returns the current mouse cursor position
			/// \param nCursorX - X screen position of the mouse cursor
			/// \param nCursorY - Y screen position of the mouse cursor
			void SetMouseCursorPosition( int nCursorX, int nCursorY );
			/// GetMouseCursorPosition - Returns the current mouse cursor position
			/// \param pCursorX - X screen position of the mouse
			/// \param pCursorY - Y screen position of the mouse
			void GetMouseCursorPosition( int *pCursorX, int *pCursorY );
			
		private:

	};

	/// InitialiseInput - Initialise the input globals
	void InitialiseInput();
	/// EnableCursor - Enables the cursor
	void EnableCursor( void );
	/// DisableCursor - Disables the cursor
	void DisableCursor( void );
	/// GetTouch - Gets touch structure data
	/// \param index - structure to request
	/// \return const TouchData* - structure from array
	const input::TInputState::TouchData* GetTouch( int index );

	extern TInputState gInputState;

} // namespace input

#endif // __INPUT_H__

