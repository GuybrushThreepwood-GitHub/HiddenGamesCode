
/*===================================================================
	File: Input.cpp
	Library: Input

	(C)Hidden Games
=====================================================================*/

#include "Core/CoreDefines.h"
#include <queue>

#include "Core/App.h"
#include "Debug/Assertion.h"
#include "Math/Vectors.h"

#ifdef BASE_PLATFORM_WINDOWS
	#include "Core/win/WinIncludes.h"
	#include "Input/win/WinInput.h"
#endif // BASE_PLATFORM_WINDOWS

#ifdef BASE_SUPPORT_OPENGL_GLSL
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/type_ptr.hpp"
#endif // BASE_SUPPORT_OPENGL_GLSL

#include "Render/PlatformRenderBase.h"
#include "Render/OpenGLCommon.h"

#include "Render/FF/OpenGL/OpenGL.h"
#include "Render/FF/OpenGLES/OpenGLES.h"
#include "Render/FF/Primitives.h"

#include "Render/GLSL/glewES.h"
#include "Render/GLSL/OpenGLSL.h"
#include "Render/GLSL/PrimitivesGLSL.h"

#include "Core/CoreFunctions.h"

#include "Input/Input.h"

using input::Input;

#ifdef BASE_PLATFORM_WINDOWS
	static bool bCursorShown = true;
#endif // BASE_PLATFORM_WINDOWS

static bool dirtyFlag = false;
input::TInputState input::gInputState;
std::queue<input::TInputEvent> m_TouchEvents;

void AddTouchFrame( input::TInputEvent e );
void AddTapFrame( input::TInputEvent e );
void TouchMovedFrame( input::TInputEvent e );
void RemoveTouchFrame( input::TInputEvent e );

#if !defined(BASE_INPUT_NO_TOUCH_SUPPORT)

/////////////////////////////////////////////////////
/// Function: AddTouch
/// Params: [in]rotationFlag, [in]bounds, [in]touch, [in]location
///
/////////////////////////////////////////////////////
	void input::AddTouch( CGRect bounds, UITouch* touch, CGPoint location )
	{
		int i=0;
		
		if( gInputState.nTouchCount < MAX_TOUCHES )
		{
			for( i=0; i < MAX_TOUCHES; ++i )
			{
				if( !gInputState.TouchesData[i].bActive )
				{
					gInputState.TouchesData[i].bActive = true;
					gInputState.TouchesData[i].bPress = true;
					gInputState.TouchesData[i].bRelease = false;

					// Convert touch point from UIView referential to OpenGL one (upside-down flip)
					if( renderer::OpenGL::GetInstance()->GetIsRotated() )
					{
                        switch( renderer::OpenGL::GetInstance()->GetRotationStyle() )
                        {
                            case renderer::VIEWROTATION_PORTRAIT_BUTTON_BOTTOM:
                            {
                                location.y = bounds.size.height - location.y;
                            }break;
                            case renderer::VIEWROTATION_PORTRAIT_BUTTON_TOP:
                            {
                                location.x = bounds.size.width - location.x;
                            }break;
                            case renderer::VIEWROTATION_LANDSCAPE_BUTTON_LEFT:
                            {
                                long swapVal = location.x;
                                
                                location.x = bounds.size.height - location.y;
                                location.y = bounds.size.width - swapVal;                                
                            }break;
                            case renderer::VIEWROTATION_LANDSCAPE_BUTTON_RIGHT:
                            {
                                long swapVal = location.x;
						
                                location.x = location.y;
                                location.y = swapVal;
                            }break;
							case renderer::VIEWROTATION_UNKNOWN:
							default:
								break;
                        }
					}
					else
					{
						location.y = bounds.size.height - location.y;
					}	
			
					gInputState.TouchesData[i].nTaps = 0;
					
					gInputState.TouchesData[i].nTouchX = location.x;
					gInputState.TouchesData[i].nTouchY = location.y;
					
					gInputState.TouchesData[i].nLastTouchX = location.x;
					gInputState.TouchesData[i].nLastTouchY = location.y;
					
					gInputState.UITouchesData[i] = touch;
					gInputState.TouchesData[i].vAccumulatedVec = math::Vec2( 0.0f, 0.0f);

					// treat 0 index as a mouse as well
					if( i == 0 )
					{
						input::gInputState.LButtonPressed = true;
						
						input::gInputState.nMouseX = gInputState.TouchesData[i].nTouchX;	
						input::gInputState.nMouseY = gInputState.TouchesData[i].nTouchY;	
						input::gInputState.nLastMouseX = gInputState.TouchesData[i].nLastTouchX;	
						input::gInputState.nLastMouseY = gInputState.TouchesData[i].nLastTouchY;	
					}
					
					gInputState.nTouchCount++;
					return;
				}
			}
		}
	}

/////////////////////////////////////////////////////
/// Function: TouchMoved
/// Params: [in]rotationFlag, [in]bounds, [in]touch, [in]location
///
/////////////////////////////////////////////////////
	void input::TouchMoved( CGRect bounds, UITouch* touch, CGPoint location )
	{
		int i=0;
		long previousX;
		long previousY;
		
		for( i=0; i < MAX_TOUCHES; ++i )
		{
			if( gInputState.UITouchesData[i] == touch )
			{
				// Convert touch point from UIView referential to OpenGL one (upside-down flip)
				if( renderer::OpenGL::GetInstance()->GetIsRotated() )
				{
                    switch( renderer::OpenGL::GetInstance()->GetRotationStyle() )
                    {
                        case renderer::VIEWROTATION_PORTRAIT_BUTTON_BOTTOM:
                        {
                            location.y = bounds.size.height - location.y;
                        }break;
                        case renderer::VIEWROTATION_PORTRAIT_BUTTON_TOP:
                        {
                            location.x = bounds.size.width - location.x;
                        }break;
                        case renderer::VIEWROTATION_LANDSCAPE_BUTTON_LEFT:
                        {
                            long swapVal = location.x;
                            
                            location.x = bounds.size.height - location.y;
                            location.y = bounds.size.width - swapVal;                               
                        }break;
                        case renderer::VIEWROTATION_LANDSCAPE_BUTTON_RIGHT:
                        {
                            long swapVal = location.x;
                            
                            location.x = location.y;
                            location.y = swapVal;
                        }break;
						case renderer::VIEWROTATION_UNKNOWN:
						default:
							break;
                    }
				}
				else
				{
					location.y = bounds.size.height - location.y;
				}	
				
				previousX = gInputState.TouchesData[i].nTouchX;
				previousY = gInputState.TouchesData[i].nTouchY;
				
				gInputState.TouchesData[i].nTouchX = location.x;
				gInputState.TouchesData[i].nTouchY = location.y;
				
				gInputState.TouchesData[i].nLastTouchX = previousX;
				gInputState.TouchesData[i].nLastTouchY = previousY;
				
				gInputState.TouchesData[i].nXDelta = gInputState.TouchesData[i].nTouchX - gInputState.TouchesData[i].nLastTouchX;
				gInputState.TouchesData[i].nYDelta = gInputState.TouchesData[i].nTouchY - gInputState.TouchesData[i].nLastTouchY;
				gInputState.TouchesData[i].vAccumulatedVec += math::Vec2( static_cast<float>(gInputState.TouchesData[i].nXDelta), static_cast<float>(gInputState.TouchesData[i].nYDelta) );

				// treat 0 index as a mouse as well
				if( i == 0 )
				{
					input::gInputState.LButtonPressed = true;
					
					input::gInputState.nMouseX = gInputState.TouchesData[i].nTouchX;	
					input::gInputState.nMouseY = gInputState.TouchesData[i].nTouchY;	
					input::gInputState.nLastMouseX = gInputState.TouchesData[i].nLastTouchX;	
					input::gInputState.nLastMouseY = gInputState.TouchesData[i].nLastTouchY;	
					input::gInputState.nMouseXDelta = gInputState.TouchesData[i].nXDelta;
					input::gInputState.nMouseYDelta = gInputState.TouchesData[i].nYDelta;			
				}
				
				return;
			}
		}		
	}

/////////////////////////////////////////////////////
/// Function: RemoveTouch
/// Params: [in]touch
///
/////////////////////////////////////////////////////
	void input::RemoveTouch( UITouch* touch )
	{
		int i=0;
		
		for( i=0; i < MAX_TOUCHES; ++i )
		{
			if( gInputState.UITouchesData[i] == touch )
			{
				//gInputState.TouchesData[i].bActive = false;
				gInputState.TouchesData[i].bPress = false;
				gInputState.TouchesData[i].bRelease = true;
				gInputState.TouchesData[i].bHeld = false;

				gInputState.TouchesData[i].nTaps = 0;
				/*gInputState.TouchesData[i].nTouchX = -1000;
				gInputState.TouchesData[i].nTouchY = -1000;
				
				gInputState.TouchesData[i].nLastTouchX = -1000;
				gInputState.TouchesData[i].nLastTouchY = -1000;
				
				gInputState.TouchesData[i].nXDelta = 0;
				gInputState.TouchesData[i].nYDelta = 0;

				gInputState.TouchesData[i].Ticks = 0.0f;*/
				
				gInputState.UITouchesData[i] = 0;
				
				if( i == 0 )
				{
					input::gInputState.LButtonPressed = false;
					
					input::gInputState.nMouseX = gInputState.TouchesData[i].nTouchX;	
					input::gInputState.nMouseY = gInputState.TouchesData[i].nTouchY;	
					input::gInputState.nLastMouseX = gInputState.TouchesData[i].nLastTouchX;	
					input::gInputState.nLastMouseY = gInputState.TouchesData[i].nLastTouchY;	
					input::gInputState.nMouseXDelta = gInputState.TouchesData[i].nXDelta;
					input::gInputState.nMouseYDelta = gInputState.TouchesData[i].nYDelta;			
				}
				
				gInputState.nTouchCount--;
				return;
			}
		}	
	}
#endif // BASE_INPUT_NO_TOUCH_SUPPORT

/////////////////////////////////////////////////////
/// Function: UpdateTouches
/// Params: None
///
/////////////////////////////////////////////////////
	void input::UpdateTouches( float deltaTime )
	{
		int i=0;
		
		for( i=0; i < MAX_TOUCHES; ++i )
		{
			if( gInputState.TouchesData[i].bActive )
			{
				if( gInputState.TouchesData[i].bRelease )
				{
					gInputState.TouchesData[i].bActive = false;
					gInputState.TouchesData[i].bHeld = false;
					gInputState.TouchesData[i].bRelease = false;
                        
					gInputState.TouchesData[i].nXDelta = 0;
					gInputState.TouchesData[i].nYDelta = 0;
                        
					gInputState.TouchesData[i].nTaps = 0;
					gInputState.TouchesData[i].Ticks = 0.0f;
				}
				else
                {
                    if( input::gInputState.TouchesData[i].bPress )
                    {
                        input::gInputState.TouchesData[i].Ticks += deltaTime;
                            
                        if( input::gInputState.TouchesData[i].Ticks > input::HOLD_TIME )
                            input::gInputState.TouchesData[i].bHeld = true;
                    }
                        
                    // track taps
					if (gInputState.TouchesData[i].bActive)
					{
#if !defined(BASE_INPUT_NO_TOUCH_SUPPORT)
						gInputState.TouchesData[i].nTaps = [gInputState.UITouchesData[i] tapCount];
#endif // BASE_INPUT_NO_TOUCH_SUPPORT
					}
                    else
                        gInputState.TouchesData[i].nTaps = 0;                        
                }
			}
			else
			{
				gInputState.TouchesData[i].Ticks = 0.0f;
                    
				gInputState.TouchesData[i].bPress = false;
				gInputState.TouchesData[i].bRelease = false;
				gInputState.TouchesData[i].bHeld = false;            
                    
				gInputState.TouchesData[i].nXDelta = 0;
				gInputState.TouchesData[i].nYDelta = 0;
                    
                gInputState.TouchesData[i].nTaps = 0;

				gInputState.TouchesData[i].vAccumulatedVec = math::Vec2( 0.0f, 0.0f );
			}
		}
#ifdef BASE_PLATFORM_ANDROID
		if (m_TouchEvents.size() > 0)
		{
			input::TInputEvent e = m_TouchEvents.front();

			switch (e.type)
			{
			case input::InputEventType_Press:
			{
				AddTouchFrame(e);
			}break;
			case input::InputEventType_Move:
			{
				TouchMovedFrame(e);
			}break;
			case input::InputEventType_Tap:
			{
				AddTapFrame(e);
			}break;
			case input::InputEventType_Release:
			{
				RemoveTouchFrame(e);
			}break;
			default:
				DBG_ASSERT(0);
				break;
			}

			// remove
			m_TouchEvents.pop();
		}
#endif // BASE_PLATFORM_ANDROID
	}

#ifdef BASE_PLATFORM_ANDROID	
/////////////////////////////////////////////////////
/// Function: AddTouch
/// Params: [in]rotationFlag, [in]pointerId, [in]pos
///
/////////////////////////////////////////////////////
	void input::AddTouch( bool rotationFlag, int pointerId, short x, short y )
	{
		TInputEvent e;
		e.type		= InputEventType_Press;
		e.nTouchId	= pointerId;
		e.nTouchX	= static_cast<long>(x);
		e.nTouchY	= static_cast<long>(y);

		//AddTouchFrame( e );
		m_TouchEvents.push(e);
	}

/////////////////////////////////////////////////////
/// Function: AddTap
/// Params: [in]pointerId
///
/////////////////////////////////////////////////////
	void input::AddTap( int pointerId )
	{
		TInputEvent e;
		e.type		= InputEventType_Tap;
		e.nTouchId	= pointerId;
		e.nTouchX	= -1000;
		e.nTouchY	= -1000;

		AddTapFrame( e );
		//m_TouchEvents.push(e);
	}

/////////////////////////////////////////////////////
/// Function: TouchMoved
/// Params: [in]rotationFlag, [in]pointerId, [in]pos
///
/////////////////////////////////////////////////////
	void input::TouchMoved( bool rotationFlag, int pointerId, short x, short y )
	{
		TInputEvent e;
		e.type		= InputEventType_Move;
		e.nTouchId		= pointerId;
		e.nTouchX	= static_cast<long>(x);
		e.nTouchY	= static_cast<long>(y);

		TouchMovedFrame( e );
		//m_TouchEvents.push(e);	
	}

/////////////////////////////////////////////////////
/// Function: RemoveTouch
/// Params: [in]index
///
/////////////////////////////////////////////////////
	void input::RemoveTouch( int pointerId )
	{
		TInputEvent e;
		e.type		= InputEventType_Release;
		e.nTouchId	= pointerId;
		e.nTouchX	= -1000;
		e.nTouchY	= -1000;

		//RemoveTouchFrame( e );
		m_TouchEvents.push(e);
	}

/////////////////////////////////////////////////////
/// Function: UpdateTouches
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
/*	void input::UpdateTouches( float deltaTime )
	{
		int i=0;

		for( i=0; i < MAX_TOUCHES; ++i )
		{
			if( gInputState.TouchesData[i].bActive )
			{
				if( gInputState.TouchesData[i].bRelease )
				{
					gInputState.TouchesData[i].bActive = false;
					gInputState.TouchesData[i].bHeld = false;
					gInputState.TouchesData[i].bRelease = false;
                        
					gInputState.TouchesData[i].nXDelta = 0;
					gInputState.TouchesData[i].nYDelta = 0;
                        
					gInputState.TouchesData[i].nTaps = 0;
					gInputState.TouchesData[i].Ticks = 0.0f;
					gInputState.TouchesData[i].nSpecialId = -1;
					gInputState.TouchesData[i].vAccumulatedVec = math::Vec2( 0.0f, 0.0f );
				}
				else
                {
                    if( input::gInputState.TouchesData[i].bPress )
                    {
                        input::gInputState.TouchesData[i].Ticks += deltaTime;
                            
                        if( input::gInputState.TouchesData[i].Ticks > input::HOLD_TIME )
                            input::gInputState.TouchesData[i].bHeld = true;
                    }
                        
                    // track taps
                    //if( gInputState.TouchesData[i].bActive )
                    //    gInputState.TouchesData[i].nTaps = [gInputState.UITouchesData[i] tapCount];
                    //else
                        gInputState.TouchesData[i].nTaps = 0;                        
                }
			}
			else
			{
				gInputState.TouchesData[i].Ticks = 0.0f;
                    
				gInputState.TouchesData[i].bPress = false;
				gInputState.TouchesData[i].bRelease = false;
				gInputState.TouchesData[i].bHeld = false;            
                    
				gInputState.TouchesData[i].nXDelta = 0;
				gInputState.TouchesData[i].nYDelta = 0;
                    
                gInputState.TouchesData[i].nTaps = 0;
				gInputState.TouchesData[i].nSpecialId = -1;

				gInputState.TouchesData[i].vAccumulatedVec = math::Vec2( 0.0f, 0.0f );
			}
		}

		if( m_TouchEvents.size() > 0 )
		{
			input::TInputEvent e = m_TouchEvents.front();

			switch( e.type )
			{
				case input::InputEventType_Press:
				{
					AddTouchFrame( e );
				}break;
				case input::InputEventType_Move:
				{
					TouchMovedFrame( e );
				}break;
				case input::InputEventType_Tap:
				{
					AddTapFrame( e );
				}break;
				case input::InputEventType_Release:
				{
					RemoveTouchFrame( e );
				}break;
				default:
					DBG_ASSERT(0);
					break;
			}

			// remove
			m_TouchEvents.pop();
		}
	}
*/
#ifdef __cplusplus
extern "C" {
#endif

	/////////////////////////////////////////////////////
	/// Function: Java_base_Input_InputSensors_SetAccelerometerValues
	/// Params: [in]env, [in]thiz, [in]x, [in]y, [in]z
	///
	/////////////////////////////////////////////////////
	JNIEXPORT void JNICALL Java_base_Input_InputSensors_SetAccelerometerValues( JNIEnv* env, jobject thiz, jfloat x, jfloat y, jfloat z )
	{
		math::Vec3 normVals( x, y, z );
		normVals.normalise();

		input::gInputState.Accelerometers[0] = normVals.X;
		input::gInputState.Accelerometers[1] = normVals.Y;
		input::gInputState.Accelerometers[2] = normVals.Z;
	}

#ifdef  __cplusplus
}
#endif

#endif // BASE_PLATFORM_ANDROID

/////////////////////////////////////////////////////
/// Function: AddTouchFrame
/// Params: [in]e
///
/////////////////////////////////////////////////////
void AddTouchFrame( input::TInputEvent e )
{
	int i = 0;

	for( i=0; i < input::MAX_TOUCHES; ++i ) 
	{
		if( input::gInputState.TouchesData[i].nSpecialId == e.nTouchId )
			return;
	}

	for( i=0; i < input::MAX_TOUCHES; ++i )
	{
		if( !input::gInputState.TouchesData[i].bActive )
		{
			input::gInputState.TouchesData[i].bActive = true;
			input::gInputState.TouchesData[i].bPress = true;
			input::gInputState.TouchesData[i].bRelease = false;
	
			input::gInputState.TouchesData[i].nTaps = 0;
					
			input::gInputState.TouchesData[i].nTouchX = e.nTouchX;
			input::gInputState.TouchesData[i].nTouchY = e.nTouchY;
					
			input::gInputState.TouchesData[i].nLastTouchX = e.nTouchX;
			input::gInputState.TouchesData[i].nLastTouchY = e.nTouchY;
					
			input::gInputState.TouchesData[i].nSpecialId = e.nTouchId;
			input::gInputState.TouchesData[i].vAccumulatedVec = math::Vec2( 0.0f, 0.0f );

			// treat 0 index as a mouse as well
			if( i == 0 )
			{
				input::gInputState.LButtonPressed = true;
						
				input::gInputState.nMouseX = input::gInputState.TouchesData[i].nTouchX;	
				input::gInputState.nMouseY = input::gInputState.TouchesData[i].nTouchY;	
				input::gInputState.nLastMouseX = input::gInputState.TouchesData[i].nLastTouchX;	
				input::gInputState.nLastMouseY = input::gInputState.TouchesData[i].nLastTouchY;	
			}
					
			input::gInputState.nTouchCount++;
			return;
		}
	}
}

/////////////////////////////////////////////////////
/// Function: AddTapFrame
/// Params: [in]e
///
/////////////////////////////////////////////////////
void AddTapFrame( input::TInputEvent e )
{
	int i=0;

	for( i=0; i < input::MAX_TOUCHES; ++i )
	{
		if( e.nTouchId == input::gInputState.TouchesData[i].nSpecialId )
		{
			DBGLOG( "AddTapFrame %d\n", e.nTouchId );

			input::gInputState.TouchesData[i].nTaps = 2;
			return;
		}
	}
}

/////////////////////////////////////////////////////
/// Function: TouchMovedFrame
/// Params: [in]e
///
/////////////////////////////////////////////////////
void TouchMovedFrame( input::TInputEvent e )
{
	int  i=0;
		
	long previousX = 0;
	long previousY = 0;
	
	for( i=0; i < input::MAX_TOUCHES; ++i )
	{			
		if( input::gInputState.TouchesData[i].bActive )
		{
			if( e.nTouchId == input::gInputState.TouchesData[i].nSpecialId )
			{
				previousX = input::gInputState.TouchesData[i].nTouchX;
				previousY = input::gInputState.TouchesData[i].nTouchY;
				
				input::gInputState.TouchesData[i].nTouchX = e.nTouchX;
				input::gInputState.TouchesData[i].nTouchY = e.nTouchY;
				
				input::gInputState.TouchesData[i].nLastTouchX = previousX;
				input::gInputState.TouchesData[i].nLastTouchY = previousY;
				
				input::gInputState.TouchesData[i].nXDelta = input::gInputState.TouchesData[i].nTouchX - input::gInputState.TouchesData[i].nLastTouchX;
				input::gInputState.TouchesData[i].nYDelta = input::gInputState.TouchesData[i].nTouchY - input::gInputState.TouchesData[i].nLastTouchY;
				
				input::gInputState.TouchesData[i].vAccumulatedVec += math::Vec2( static_cast<float>(input::gInputState.TouchesData[i].nXDelta), static_cast<float>(input::gInputState.TouchesData[i].nYDelta) );

				// treat 0 index as a mouse as well
				if( i == 0 )
				{
					input::gInputState.LButtonPressed = true;
					
					input::gInputState.nMouseX = input::gInputState.TouchesData[i].nTouchX;	
					input::gInputState.nMouseY = input::gInputState.TouchesData[i].nTouchY;	
					input::gInputState.nLastMouseX = input::gInputState.TouchesData[i].nLastTouchX;	
					input::gInputState.nLastMouseY = input::gInputState.TouchesData[i].nLastTouchY;	
					input::gInputState.nMouseXDelta = input::gInputState.TouchesData[i].nXDelta;
					input::gInputState.nMouseYDelta = input::gInputState.TouchesData[i].nYDelta;			
				}

				return;
			}
		}
	}
}

/////////////////////////////////////////////////////
/// Function: RemoveTouchFrame
/// Params: [in]e
///
/////////////////////////////////////////////////////
void RemoveTouchFrame( input::TInputEvent e )
{
	int i=0;

	for( i=0; i < input::MAX_TOUCHES; ++i )
	{
		if( e.nTouchId == input::gInputState.TouchesData[i].nSpecialId )
		{
			input::gInputState.TouchesData[i].bPress = false;
			input::gInputState.TouchesData[i].bRelease = true;
			input::gInputState.TouchesData[i].bHeld = false;

			input::gInputState.TouchesData[i].nTaps = 0;

			input::gInputState.TouchesData[i].Ticks = 0.0f;
			input::gInputState.TouchesData[i].nSpecialId = -1;

			input::gInputState.TouchesData[i].nXDelta = 0;
			input::gInputState.TouchesData[i].nYDelta = 0;	

			if( i == 0 )
			{
				input::gInputState.LButtonPressed = false;
					
				input::gInputState.nMouseX = input::gInputState.TouchesData[i].nTouchX;	
				input::gInputState.nMouseY = input::gInputState.TouchesData[i].nTouchY;	
				input::gInputState.nLastMouseX = input::gInputState.TouchesData[i].nLastTouchX;	
				input::gInputState.nLastMouseY = input::gInputState.TouchesData[i].nLastTouchY;	
				input::gInputState.nMouseXDelta = input::gInputState.TouchesData[i].nXDelta;
				input::gInputState.nMouseYDelta = input::gInputState.TouchesData[i].nYDelta;			
			}
				
			input::gInputState.nTouchCount--;
			return;
		}
	}
}

/////////////////////////////////////////////////////
/// Function: ClearTouchDeltas
/// Params: None
///
/////////////////////////////////////////////////////
void input::ClearTouchDeltas()
{
	int i=0;
	
	for( i=0; i < MAX_TOUCHES; ++i )
	{
		gInputState.TouchesData[i].nXDelta = 0;
		gInputState.TouchesData[i].nYDelta = 0;
	}
}

/////////////////////////////////////////////////////
/// Function: DirtyTouchTicks
/// Params: None
///
/////////////////////////////////////////////////////
void input::DirtyTouchTicks()
{
	int i=0;
	
	for( i=0; i < MAX_TOUCHES; ++i )
	{
		gInputState.TouchesData[i].Ticks = 99.0f;
	}
	
	dirtyFlag = true;
}

/////////////////////////////////////////////////////
/// Function: ClearAllTouches
/// Params: None
///
/////////////////////////////////////////////////////
void input::ClearAllTouches()
{
	InitialiseInput();
}

/////////////////////////////////////////////////////
/// Function: DrawTouches
/// Params: None
///
/////////////////////////////////////////////////////
void input::DrawTouches()
{
	int i=0;
	
	for( i=0; i < MAX_TOUCHES; ++i )
	{
		if( gInputState.TouchesData[i].bActive )
		{
			renderer::OpenGL::GetInstance()->SetColour4ub( 255,255,0, 255 );
            
#ifdef BASE_SUPPORT_OPENGL_GLSL
            glm::mat4 modelMatrix(1.0f);
            modelMatrix = glm::translate( modelMatrix, glm::vec3(static_cast<float>(gInputState.TouchesData[i].nTouchX), static_cast<float>(gInputState.TouchesData[i].nTouchY), 0.0f ) );
            renderer::OpenGL::GetInstance()->SetModelMatrix(modelMatrix);
            renderer::DrawSphere( 32.0f );
#else
            glPushMatrix();
                glTranslatef( static_cast<float>(gInputState.TouchesData[i].nTouchX), static_cast<float>(gInputState.TouchesData[i].nTouchY), 1.0f );
                renderer::DrawSphere( 32.0f );
            glPopMatrix();
#endif
        
		}
	}
}

/////////////////////////////////////////////////////
/// Default constructor
/// 
///
/////////////////////////////////////////////////////
Input::Input()
{		
}

/////////////////////////////////////////////////////
/// Default destructor
/// 
///
/////////////////////////////////////////////////////
Input::~Input()
{

}

/////////////////////////////////////////////////////
/// Method: IsKeyPressed
/// Params: [in]nKeyCode, [in]bIgnoreHeld
///
/////////////////////////////////////////////////////
bool Input::IsKeyPressed( int nKeyCode, int bIgnoreHeld, bool specialKeyOnly )
{
	if( !core::app::SupportsHardwareKeyboard() )
		return false;

	if( nKeyCode > input::MAX_KEYS )
		return(false);

#ifdef BASE_PLATFORM_WINDOWS
	switch(nKeyCode)
	{
		case VK_ESCAPE:
		case VK_PRIOR:
		case VK_NEXT:
		case VK_F1:
		case VK_F2:
		case VK_F3:
		case VK_F4:
		case VK_F5:
		case VK_F6:
		case VK_F7:
		case VK_F8:
		case VK_F9:
		case VK_F10:

		case VK_BACK:
		case VK_UP:
		case VK_DOWN:
		case VK_RIGHT:
		case VK_LEFT:

		case VK_ADD:
		case VK_SUBTRACT:

		//case VK_OEM_4: // [ 
		//case VK_OEM_6: // ] 

		case VK_TAB:
		case VK_RETURN:
		case VK_SPACE:
		case VK_LCONTROL:
		case VK_RCONTROL:
		case VK_SHIFT:
		case VK_MENU:
		{
			if( ((GetAsyncKeyState(nKeyCode) & 0x8000) ? 1 : 0) )
			{
				if( bIgnoreHeld )
				{
					if( gInputState.KeyStates.vkKeys[nKeyCode] )
					{
						if( gInputState.KeyStates.vkKeyPressTime[nKeyCode] >= 1 )
						{
							return( false );
						}
					}
				}

				if( gInputState.KeyStates.vkKeys[nKeyCode] )
				{
					gInputState.KeyStates.vkKeyPressTime[nKeyCode]++;
					return(true);
				}
			}

			if( specialKeyOnly )
				return false;
		} break;
		default: 
			break;
	} // end switch

	if( (nKeyCode >= 65 && nKeyCode <= 90) || 
		(nKeyCode >= 97 && nKeyCode <= 122) )
	{
		// in keyboard input, don't bother checking for caps difference in alphabet codes
		if( (nKeyCode >= 65 && nKeyCode <= 90) )
		{
			if( bIgnoreHeld )
			{
				if( gInputState.KeyStates.bKeyPressTime[nKeyCode] >= 1 || gInputState.KeyStates.bKeyPressTime[nKeyCode+32] >= 1 )
					return( false );
			}

			if( gInputState.KeyStates.bKeys[nKeyCode] || gInputState.KeyStates.bKeys[nKeyCode+32] )
			{
				gInputState.KeyStates.bKeyPressTime[nKeyCode]++;
				gInputState.KeyStates.bKeyPressTime[nKeyCode+32]++;
				return(true);
			}
			else
				return(false);
		}
		else if( (nKeyCode >= 97 && nKeyCode <= 122) )
		{
			if( bIgnoreHeld )
			{
				if( gInputState.KeyStates.bKeyPressTime[nKeyCode] >= 1 || gInputState.KeyStates.bKeyPressTime[nKeyCode-32] >= 1 )
					return( false );
			}

			if( gInputState.KeyStates.bKeys[nKeyCode] || gInputState.KeyStates.bKeys[nKeyCode-32] )
			{
				gInputState.KeyStates.bKeyPressTime[nKeyCode]++;
				gInputState.KeyStates.bKeyPressTime[nKeyCode-32]++;
				return(true);
			}
			else
				return(false);
		}
	}
	else
	{
		if( (nKeyCode >= 0 && nKeyCode <= 254) )
		{
			if( bIgnoreHeld )
			{
				if( gInputState.KeyStates.bKeyPressTime[nKeyCode] >= 1 )
					return( false );
			}

			if( gInputState.KeyStates.bKeys[nKeyCode] )
			{
				gInputState.KeyStates.bKeyPressTime[nKeyCode]++;
				return(true);
			}
			else
				return(false);
		}
	}

	if( bIgnoreHeld )
	{
		if( gInputState.KeyStates.bKeyPressTime[nKeyCode] >= 1 )
			return( false );
	}

	if( gInputState.KeyStates.bKeys[nKeyCode] )
	{
		gInputState.KeyStates.bKeyPressTime[nKeyCode]++;
		return(true);
	}
	else
		return(false); 

	return false;
#else // BASE_PLATFORM_WINDOWS

	if( (nKeyCode >= 0 && nKeyCode <= 254) )
	{
		if( bIgnoreHeld )
		{
			if( gInputState.KeyStates.bKeyPressTime[nKeyCode] >= 1 )
				return( false );
		}

		if( gInputState.KeyStates.bKeys[nKeyCode] )
		{
			gInputState.KeyStates.bKeyPressTime[nKeyCode]++;
			return(true);
		}
		else
			return(false);
	}

	if( bIgnoreHeld )
	{
		if( gInputState.KeyStates.bKeyPressTime[nKeyCode] >= 1 )
			return( false );
	}

	if( gInputState.KeyStates.bKeys[nKeyCode] )
	{
		gInputState.KeyStates.bKeyPressTime[nKeyCode]++;
		return(true);
	}
	else
		return(false); 

#endif // BASE_PLATFORM_WINDOWS

	return false;
}

/////////////////////////////////////////////////////
/// Method: IsLeftMouseButtonPressed
/// Params: None
///
/////////////////////////////////////////////////////
bool Input::IsLeftMouseButtonPressed( void )
{
	if( gInputState.LButtonPressed )
		return(true);
	else
		return(false);
}

/////////////////////////////////////////////////////
/// Method: IsRightMouseButtonPressed
/// Params: None
///
/////////////////////////////////////////////////////
bool Input::IsRightMouseButtonPressed( void )
{
	if( gInputState.RButtonPressed )
		return(true);
	else
		return(false);
}

/////////////////////////////////////////////////////
/// Method: GetMousePosition
/// Params: [out]pMouseX, [out]pMouseY
///
/////////////////////////////////////////////////////
void Input::GetMousePosition( long *pMouseX, long *pMouseY )
{
	*pMouseX = gInputState.nMouseX;
	*pMouseY = gInputState.nMouseY;
}

/////////////////////////////////////////////////////
/// Method: GetMousePosition
/// Params: [in]nCursorX, [in]nCursorY
///
/////////////////////////////////////////////////////
void Input::SetMouseCursorPosition( int nCursorX, int nCursorY )
{
	core::SetCursorPosition( nCursorX, nCursorY );
}
		
/////////////////////////////////////////////////////
/// Method: GetMousePosition
/// Params: [out]pMouseX, [out]pMouseY
///
/////////////////////////////////////////////////////
void Input::GetMouseCursorPosition( int *pCursorX, int *pCursorY )
{

#ifdef BASE_PLATFORM_WINDOWS
	POINT pnt;
	GetCursorPos( &pnt );
	*pCursorX = pnt.x;
	*pCursorY = pnt.y;
#endif // BASE_PLATFORM_WINDOWS

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////
/// Function: InitialiseInput
/// Params: None
///
/////////////////////////////////////////////////////
void input::InitialiseInput()
{
	int i=0;
	
	gInputState.bKeyPressed = false;
	gInputState.nKeyPressCode = -1;
	
	gInputState.bMouseWheel = false;
	gInputState.nMouseWheelDelta = 0;

	// don't reset mouse as it could be relative movements and this puts it at the origin
	//gInputState.nMouseX = -1000;
	//gInputState.nMouseY = -1000;
	//gInputState.nLastMouseX = -1000;
	//gInputState.nLastMouseY = -1000;
	
	gInputState.nMouseXDelta = 0;
	gInputState.nMouseYDelta = 0;
	
	gInputState.LButtonPressed = false;
	gInputState.LButtonReleased = false;

	gInputState.RButtonPressed = false;
	gInputState.RButtonReleased = false;
    
    gInputState.upValue = 0.0f;
    gInputState.downValue = 0.0f;
    gInputState.leftValue = 0.0f;
    gInputState.rightValue = 0.0f;
	
	gInputState.nTouchCount = 0;
	for( i=0; i < MAX_TOUCHES; ++i )
	{
		gInputState.TouchesData[i].bActive = false;

		gInputState.TouchesData[i].bPress = false;
		gInputState.TouchesData[i].bRelease = false;
		gInputState.TouchesData[i].bHeld = false;

		gInputState.TouchesData[i].nTaps = 0;
		gInputState.TouchesData[i].nTouchX = -1000;
		gInputState.TouchesData[i].nTouchY = -1000;
	
		gInputState.TouchesData[i].nLastTouchX = -1000;
		gInputState.TouchesData[i].nLastTouchY = -1000;
		
		gInputState.TouchesData[i].nXDelta = 0;
		gInputState.TouchesData[i].nYDelta = 0;
	
		gInputState.TouchesData[i].nSpecialId = -1;
		gInputState.TouchesData[i].vAccumulatedVec = math::Vec2( 0.0f, 0.0f );

		// ios only
#if !defined(BASE_INPUT_NO_TOUCH_SUPPORT)
		gInputState.UITouchesData[i] = 0;
#endif // BASE_INPUT_NO_TOUCH_SUPPORT
	}

	// extends a box around the touch point
	gInputState.nTouchWidth = 2;
	gInputState.nTouchHeight = 2;
			
	// sphere extends from the touch
	input::gInputState.nTouchRadius = 1;
	
	gInputState.Accelerometers[0] = 0.0f;
	gInputState.Accelerometers[1] = 0.0f;
	gInputState.Accelerometers[2] = 0.0f;		

	while( !m_TouchEvents.empty() )
		m_TouchEvents.pop();
}

/////////////////////////////////////////////////////
/// Function: EnableCursor
/// Params: None
///
/////////////////////////////////////////////////////
void input::EnableCursor( void )
{
#ifdef BASE_PLATFORM_WINDOWS
	if( bCursorShown == false )
	{
		ShowCursor( true );
		bCursorShown = true;
	}
#endif // BASE_PLATFORM_WINDOWS

}
		
/////////////////////////////////////////////////////
/// Function: DisableCursor
/// Params: None
///
/////////////////////////////////////////////////////
void input::DisableCursor( void )
{
#ifdef BASE_PLATFORM_WINDOWS
	if( bCursorShown == true )
	{
		ShowCursor( false );
		bCursorShown = false;
	}
#endif // BASE_PLATFORM_WINDOWS
}

/////////////////////////////////////////////////////
/// Function: GetTouch
/// Params: [in]index
///
/////////////////////////////////////////////////////
const input::TInputState::TouchData* input::GetTouch( int index )
{
	DBG_ASSERT_MSG( ((index >= 0) && (index < input::MAX_TOUCHES)), "Out of range touch index" );
	
	return( &gInputState.TouchesData[index] );
}

