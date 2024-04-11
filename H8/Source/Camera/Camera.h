
/*===================================================================
	File: Camera.h
	Game: H8

	(C)Hidden Games
=====================================================================*/

#ifndef __CAMERA_H__
#define __CAMERA_H__

// forward declare
namespace math { class Vec3; }

class Camera
{
	public:
		Camera();
		virtual ~Camera();
		
		virtual void PositionCamera( float posX, float posY, float posZ, float targetX, float targetY, float targetZ, float upX, float upY, float upZ);
		virtual void MoveCamera(float cameraSpeed);
		virtual void RotateView(float X, float Y, float Z);
		virtual void RotateAroundPoint( math::Vec3& vCenter, float x, float y, float z);
		virtual void StrafeCamera(float speed);

		math::Vec3&	GetPosition();
		math::Vec3&	GetTarget();
		math::Vec3&	GetUp();
		float				GetAngle();

		virtual void SetPosition( math::Vec3& posVector );
		virtual void SetTarget( math::Vec3& targetVector );

		void SetAngle( float angle )
		{
			m_Angle = angle;
		}

	protected:
		math::Vec3 m_Pos;					// current position
		math::Vec3 m_Target;				// current target
		math::Vec3 m_Up;					// up vector

		float m_Angle;							// current angle
		float m_Speed;

	private:
		
};

/////////////////////////////////////////////////////
/// Method: GetPosition
/// 
///
/////////////////////////////////////////////////////
inline math::Vec3&	Camera::GetPosition()	
{ 
	return m_Pos; 
}

/////////////////////////////////////////////////////
/// Method: GetTarget
/// 
///
/////////////////////////////////////////////////////
inline math::Vec3&	Camera::GetTarget()	
{ 
	return m_Target; 
}

/////////////////////////////////////////////////////
/// Method: GetUp
/// 
///
/////////////////////////////////////////////////////
inline math::Vec3&	Camera::GetUp()		
{ 
	return m_Up; 
}

/////////////////////////////////////////////////////
/// Method: GetAngle
/// 
///
/////////////////////////////////////////////////////
inline float Camera::GetAngle()
{
	return m_Angle;
}

/////////////////////////////////////////////////////
/// Method: SetPosition
/// 
///
/////////////////////////////////////////////////////
inline void Camera::SetPosition( math::Vec3& posVector )	
{ 
	m_Pos = posVector; 
}

/////////////////////////////////////////////////////
/// Method: SetTarget
/// 
///
/////////////////////////////////////////////////////
inline void Camera::SetTarget( math::Vec3& targetVector )	
{ 
	m_Target = targetVector; 
}

#endif // __CAMERA_H__

