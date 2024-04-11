
/*===================================================================
	File: DebugCamera.h
	Game: AirCadets

	(C)Hidden Games
=====================================================================*/

#ifndef __DEBUGCAMERA_H__
#define __DEBUGCAMERA_H__

// forward declare
namespace input { class Input; }

class DebugCamera : public Camera
{
	public:
		DebugCamera();
		virtual ~DebugCamera();

		void Update( float fDelta );
		int MoveCameraByMouse( float fDelta );

		void Enable()		{ m_Enabled = true; }
		void Disable()		{ m_Enabled = false; }
		bool IsEnabled()	{ return m_Enabled; }

	private:
		bool m_Enabled;
		input::Input m_Input;
};

extern DebugCamera gDebugCamera;

#endif // __DEBUGCAMERA_H__

