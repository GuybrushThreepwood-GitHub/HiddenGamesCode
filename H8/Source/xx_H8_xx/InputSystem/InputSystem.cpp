
/*===================================================================
	File: InputSystem.cpp
	Game: H8

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "SoundBase.h"
#include "CollisionBase.h"
#include "Model/ModelCommon.h"
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
	m_Ignore = false;

	m_Left = false;
	m_Right = false;
	m_Up = false;
	m_Down = false;

	m_MoveVec.setZero();
	m_MoveVecLen = 0.0f;

	m_Reset = false;

	m_UILeft = false;
	m_UIRight = false;
	m_UIUp = false;
	m_UIDown = false;
	m_UISelect = false;

	m_Pause = false;
    m_Menu = false;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
InputSystem::~InputSystem()
{

}
