
/*===================================================================
	File: InputSystem.cpp
	Game: H4

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "SoundBase.h"
#include "CollisionBase.h"
#include "ModelBase.h"
#include "InputBase.h"
#include "ScriptBase.h"

#include "InputSystem/InputSystem.h"

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
InputSystem::InputSystem()
{
	m_UpVal = 0.0f;
	m_DownVal = 0.0f;
	m_LeftVal = 0.0f;
	m_RightVal = 0.0f;
	
	m_LookUpVal = 0.0f;	
	m_LookDownVal = 0.0f;
	m_RotateLeftVal = 0.0f;
	m_RotateRightVal = 0.0f;
	
	m_Up = false;
	m_Down = false;

	m_Left = false;
	m_Right = false;

	m_Action = false;
	m_Use = false;
	m_Run = false;

	m_LookUp = false;
	m_LookDown = false;

	m_RotateLeft = false;
	m_RotateRight = false;

	m_WeaponSwap = false;
	m_Reload = false;

	m_DebugCameraSwap = false;
	m_Debug1 = false;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
InputSystem::~InputSystem()
{

}
