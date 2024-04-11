
/*===================================================================
	File: UIBaseState.cpp
	Game: AirCadets

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"

#include "GameStates/UI/UIBaseState.h"

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
UIBaseState::UIBaseState()
{
	m_FinishedTransitionIn = false;
	m_FinishedTransitionOut = false;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
UIBaseState::~UIBaseState()
{

}
