
/*===================================================================
	File: DebugCamera.cpp
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"

#include "Input/InputInclude.h"
#include "Input/Input.h"

#include "Math/Vectors.h"
#include "Camera.h"
#include "DebugCamera.h"

const float TARGET_DISTANCE = 500.0f;

DebugCamera gDebugCamera;

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
DebugCamera::DebugCamera()
{
	m_Enabled = false;
	PositionCamera( 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f );
	m_Target.Z = m_Pos.Z+TARGET_DISTANCE;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
DebugCamera::~DebugCamera()
{

}

/////////////////////////////////////////////////////
/// Method: Update
/// 
///
/////////////////////////////////////////////////////
void DebugCamera::Update( float deltaTime )
{
	if( m_Enabled )
	{
		MoveCameraByMouse( deltaTime );

		float MOVE = 10.0f;

		if( m_Input.IsKeyPressed(input::KEY_RSHIFT) )
			MOVE = 200.0f;

		if( m_Input.IsKeyPressed( input::KEY_I ) )
			MoveCamera( (MOVE/TARGET_DISTANCE)*deltaTime );
		if( m_Input.IsKeyPressed( input::KEY_K ) )
			MoveCamera( (-MOVE/TARGET_DISTANCE)*deltaTime );
		if( m_Input.IsKeyPressed( input::KEY_J ) )
			StrafeCamera( (MOVE/TARGET_DISTANCE)*deltaTime );
		if( m_Input.IsKeyPressed( input::KEY_L ) )
			StrafeCamera( (-MOVE/TARGET_DISTANCE)*deltaTime );
	}
}

/////////////////////////////////////////////////////
/// Method: Update
/// 
///
/////////////////////////////////////////////////////
int DebugCamera::MoveCameraByMouse( float fDelta )
{
	int nCursorPosX=0, nCursorPosY=0;
	int nMiddleX=0, nMiddleY=0;		
	float deltaY  = 0.0f;							
	float rotateY = 0.0f;				
	
	core::GetScreenCenter( &nMiddleX, &nMiddleY );

	// Get the mouse's current X,Y position
	m_Input.GetMouseCursorPosition( &nCursorPosX, &nCursorPosY );						
	
	// If our cursor is still in the middle, we never moved... so don't update the screen
	if( (nCursorPosX == nMiddleX) && (nCursorPosY == nMiddleY) ) 
		return(false);

	m_Input.SetMouseCursorPosition( nMiddleX, nMiddleY );								

	// Now we need to get the direction (or VECTOR) that the mouse moved.
	// To do this, it's a simple subtraction.  Just take the middle point,
	// and subtract the new point from it:  VECTOR = P1 - P2;   with P1
	// being the middle point (400, 300) and P2 being the new mouse location.
	// Look below at the *Quick Notes* for more information and examples.
	// After we get the X and Y delta (or direction), I divide by 1000 to bring the number
	// down a bit, otherwise the camera would move lightning fast!

	// Get the direction the mouse moved in, but bring the number down to a reasonable amount
	rotateY = (float)( (nMiddleX - nCursorPosX)*2.0f ) * 0.001f;		
	deltaY  = (float)( (nMiddleY - nCursorPosY)*2.0f ) * 0.001f;

	// Here we just do simple addition to move the camera up and down,
	// depending on the direction (position goes up, negative goes down)
	// We could rotate it, but this is faster since we don't have to use sin() and cos()
	// Below I am multiplying the direction vector for Y by an acceleration.
	// I just chose the number 8 because it is close to the speed of looking left and right.
	// The higher the number (acceleration) the faster the camera looks up and down.
	m_Target.Y += deltaY * (3*TARGET_DISTANCE);

	// Since we aren't rotating, we need to set a point when we should stop the
	// camera from looking up or down, otherwise it will look up or down forever, and we
	// don't want this. By testing, I found that when the Y distance from the camera's position 
	// to the camera's view exceeds 10, that's a good stopping point.
	// Now if we look too high, we stop moving the camera up, and vice versa with looking down.
	// There are many better ways to do this type of camera movement, but this is the simplest.

	// Check if the distance of our view exceeds 1 from our position, if so, stop it. (UP)
	if( ( m_Target.Y - m_Pos.Y ) > 10.0f*TARGET_DISTANCE)  
		m_Target.Y = 10.0f*TARGET_DISTANCE;

	// Check if the distance of our view exceeds -1 from our position, if so, stop it. (DOWN)
	if( ( m_Target.Y - m_Pos.Y ) < -10.0f*TARGET_DISTANCE)  
		m_Target.Y = -10.0f*TARGET_DISTANCE;


	// Here we rotate the view along the Y avis depending on the direction (Left of Right)
	RotateView(0, rotateY, 0 );

	m_Angle += math::RadToDeg(-rotateY);
	
	// Return TRUE to say that we need to redraw the screen
	return(true);
}


