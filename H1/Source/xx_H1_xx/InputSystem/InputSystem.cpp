
/*===================================================================
	File: InputSystem.cpp
	Game: AirCadets

	(C)Hidden Games
=====================================================================*/

#if defined(BASE_PLATFORM_tvOS)
    #import <GameKit/GameKit.h>
#endif

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
	m_Lift = false;
	m_Dive = false;
	m_Shoot = false;

	m_Accel = false;
	m_Decel = false;

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
const math::Vec2& InputSystem::GetAnalogueValues( )
{
    if( core::app::SupportsHardwareKeyboard() )
    {
        
        bool upPress = m_Input.IsKeyPressed(input::KEY_UPARROW, false, true);
        bool downPress = m_Input.IsKeyPressed(input::KEY_DOWNARROW, false, true);
        
        if (upPress || downPress)
        {
            if (upPress)
                m_AnalogueValues.Y = 1.0f;
            if (downPress)
                m_AnalogueValues.Y = -1.0f;
        }
        else
            m_AnalogueValues.Y = 0.0f;
        
        bool leftPress = m_Input.IsKeyPressed(input::KEY_LEFTARROW, false, true);
        bool rightPress = m_Input.IsKeyPressed(input::KEY_RIGHTARROW, false, true);
        
        if (leftPress || rightPress)
        {
            if (leftPress)
                m_AnalogueValues.X = -1.0f;
            if (rightPress)
                m_AnalogueValues.X = 1.0f;
        }
        else
            m_AnalogueValues.X = 0.0f;
        
#if defined(BASE_PLATFORM_tvOS)
        if( ([[GCController controllers] count] > 0) )
        {
            for( GCController* controller in [GCController controllers] )
            {
                if( controller.extendedGamepad )
                {
                    if(controller.extendedGamepad.leftThumbstick.yAxis.value <= -0.1f ||
                       controller.extendedGamepad.leftThumbstick.yAxis.value >= 0.1f )
                    {
                        m_AnalogueValues.Y = controller.extendedGamepad.leftThumbstick.yAxis.value;
                    }
                    
                    if(controller.extendedGamepad.leftThumbstick.xAxis.value >= 0.1f||
                       controller.extendedGamepad.leftThumbstick.xAxis.value <= -0.1f )
                    {
                        m_AnalogueValues.X = controller.extendedGamepad.leftThumbstick.xAxis.value;
                    }
                }
            }
        }
#endif
        
    }
    
    return m_AnalogueValues;
}
