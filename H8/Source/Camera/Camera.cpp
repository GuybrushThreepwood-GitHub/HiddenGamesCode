
/*===================================================================
	File: Camera.cpp
	Game: H8

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"

#include "Camera.h"

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
Camera::Camera()
{
	PositionCamera( 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f );
	m_Target.Z = m_Pos.Z+1.0f;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
Camera::~Camera()
{

}

/////////////////////////////////////////////////////
/// Method: PositionCamera
/// 
///
/////////////////////////////////////////////////////
void Camera::PositionCamera( float posX, float posY, float posZ, float targetX, float targetY, float targetZ, float upX, float upY, float upZ )
{
	m_Pos		= math::Vec3(posX,  posY,  posZ);
	m_Target	= math::Vec3(targetX, targetY, targetZ);
	m_Up		= math::Vec3(upX,   upY,   upZ);
	
	m_Angle = 0.0f;
}

/////////////////////////////////////////////////////
/// Method: MoveCamera
/// 
///
/////////////////////////////////////////////////////
void Camera::MoveCamera(float cameraSpeed)
{
	math::Vec3 vVector = math::Vec3(0.0f,0.0f,0.0f);		// init a new view vector

	// Get our view vector (The direciton we are facing)
	vVector.X = m_Target.X - m_Pos.X;		// This gets the direction of the X	
	vVector.Y = m_Target.Y - m_Pos.Y;		// This gets the direction of the Y
	vVector.Z = m_Target.Z - m_Pos.Z;		// This gets the direction of the Z

	m_Pos.X += vVector.X * cameraSpeed;		// Add our acceleration to our position's X
	m_Pos.Y += vVector.Y * cameraSpeed;
	m_Pos.Z += vVector.Z * cameraSpeed;		// Add our acceleration to our position's Z
	m_Target.X += vVector.X * cameraSpeed;	// Add our acceleration to our view's X
	m_Target.Y += vVector.Y * cameraSpeed;
	m_Target.Z += vVector.Z * cameraSpeed;	// Add our acceleration to our view's Z
}

/////////////////////////////////////////////////////
/// Method: RotateView
/// 
///
/////////////////////////////////////////////////////
void Camera::RotateView(float x, float y, float z)
{
	math::Vec3 vVector = m_Target - m_Pos;
	float sinVal;
	float cosVal;

	if(x != 0.0f) 
	{
		math::sinCos( &sinVal, &cosVal, x );

		m_Target.Y = (float)(m_Pos.Y + cosVal*vVector.Y - sinVal*vVector.Z);
		m_Target.Z = (float)(m_Pos.Z + sinVal*vVector.Y + cosVal*vVector.Z);
	}
	if(y != 0.0f) 
	{
		math::sinCos( &sinVal, &cosVal, y );

		m_Target.X = (float)(m_Pos.X + cosVal*vVector.X + sinVal*vVector.Z);
		m_Target.Z = (float)(m_Pos.Z - sinVal*vVector.X + cosVal*vVector.Z);
	}
	if(z != 0.0f) 
	{
		math::sinCos( &sinVal, &cosVal, z );

		m_Target.X = (float)(m_Pos.X + cosVal*vVector.X - sinVal*vVector.Y);		
		m_Target.Y = (float)(m_Pos.Y + sinVal*vVector.X + cosVal*vVector.Y);
	}
}

/////////////////////////////////////////////////////
/// Method: RotateAroundPoint
/// 
///
/////////////////////////////////////////////////////
void Camera::RotateAroundPoint(math::Vec3& vCenter, float x, float y, float z)
{	
	math::Vec3 vVector = m_Pos - vCenter;
	float sinVal;
	float cosVal;

	if(x != 0.0f)
	{
		math::sinCos( &sinVal, &cosVal, z );

		m_Pos.Y = (float)(vCenter.Z + cosVal*vVector.Y - sinVal*vVector.Z);
		m_Pos.Y = (float)(vCenter.Y + sinVal*vVector.Y + cosVal*vVector.Z);
	}
	if(y != 0.0f) 
	{
		math::sinCos( &sinVal, &cosVal, z );

		m_Pos.X = (float)(vCenter.X + cosVal*vVector.X + sinVal*vVector.Z);
		m_Pos.Z = (float)(vCenter.Z - sinVal*vVector.X + cosVal*vVector.Z);
	}
	if(z != 0.0f)
	{
		math::sinCos( &sinVal, &cosVal, z );

		m_Pos.X = (float)(vCenter.X + cosVal*vVector.Y - sinVal*vVector.X);		
		m_Pos.Y = (float)(vCenter.Y + sinVal*vVector.Y + cosVal*vVector.X);
	}

}

/////////////////////////////////////////////////////
/// Method: StrafeCamera
/// 
///
/////////////////////////////////////////////////////
void Camera::StrafeCamera(float cameraSpeed)
{
	// Initialize a variable for the cross product result
	math::Vec3 vCross = math::Vec3( 0.0f, 0.0f, 0.0f );

	// Get the view vector of our camera and store it in a local variable
	math::Vec3 vViewVector = math::Vec3( m_Target.X - m_Pos.X, m_Target.Y - m_Pos.Y, m_Target.Z - m_Pos.Z );

	// Here we calculate the cross product of our up vector and view vector
	vCross = CrossProduct( m_Up, vViewVector );

	// Now we want to just add this new vector to our position and view, as well as
	// multiply it by our speed factor.  If the speed is negative it will strafe the
	// opposite way.

	// Add the resultant vector to our position
	m_Pos.X += vCross.X * cameraSpeed;
	m_Pos.Z += vCross.Z * cameraSpeed;

	// Add the resultant vector to our view
	m_Target.X += vCross.X * cameraSpeed;
	m_Target.Z += vCross.Z * cameraSpeed;
}

