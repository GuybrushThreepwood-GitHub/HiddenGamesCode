
/*===================================================================
	File: FullscreenEffects.cpp
	Game: H4

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include <cmath>
#include "MathBase.h"
#include "RenderBase.h"
#include "SoundBase.h"
#include "CollisionBase.h"
#include "ModelBase.h"
#include "InputBase.h"
#include "ScriptBase.h"

#include "H1Consts.h"
#include "H1.h"

#include "Resources/ResourceHelper.h"

#include "GameEffects/FullscreenEffects.h"

namespace
{

	// fullscreen quad
	math::Vec3 vaPoints[4];
	math::Vec2 vaUVCoords[4][4];
	math::Vec4Lite vaColours[4];
}

/////////////////////////////////////////////////////
/// Function: DrawFullscreenQuad
/// Params: [in]zOffset, [in]alpha
///
/////////////////////////////////////////////////////
void DrawFullscreenQuad( float zOffset, int alpha )
{
	bool textureState = renderer::OpenGL::GetInstance()->GetTextureState();

	vaPoints[0].Z = zOffset;
	vaPoints[1].Z = zOffset;
	vaPoints[2].Z = zOffset;
	vaPoints[3].Z = zOffset;

	if( renderer::OpenGL::GetInstance()->GetIsRotated() )
	{
		vaPoints[0] = math::Vec3( -(static_cast<float>(core::app::GetOrientationHeight())*0.5f), -(static_cast<float>(core::app::GetOrientationWidth())*0.5f), 0.0f ); 
		vaPoints[1] = math::Vec3( (static_cast<float>(core::app::GetOrientationHeight())*0.5f), -(static_cast<float>(core::app::GetOrientationWidth())*0.5f), 0.0f );
		vaPoints[2] = math::Vec3( -(static_cast<float>(core::app::GetOrientationHeight())*0.5f), (static_cast<float>(core::app::GetOrientationWidth())*0.5f), 0.0f ); 
		vaPoints[3] = math::Vec3( (static_cast<float>(core::app::GetOrientationHeight())*0.5f), (static_cast<float>(core::app::GetOrientationWidth())*0.5f), 0.0f );		
	}
	else
	{
		vaPoints[0] = math::Vec3( -(static_cast<float>(core::app::GetOrientationWidth())*0.5f), -(static_cast<float>(core::app::GetOrientationHeight())*0.5f), 0.0f ); 
		vaPoints[1] = math::Vec3( (static_cast<float>(core::app::GetOrientationWidth())*0.5f), -(static_cast<float>(core::app::GetOrientationHeight())*0.5f), 0.0f );
		vaPoints[2] = math::Vec3( -(static_cast<float>(core::app::GetOrientationWidth())*0.5f), (static_cast<float>(core::app::GetOrientationHeight())*0.5f), 0.0f ); 
		vaPoints[3] = math::Vec3( (static_cast<float>(core::app::GetOrientationWidth())*0.5f), (static_cast<float>(core::app::GetOrientationHeight())*0.5f), 0.0f );
	}
	
	renderer::OpenGL::GetInstance()->DepthMode( true, GL_ALWAYS );
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	renderer::OpenGL::GetInstance()->AlphaMode( false, GL_ALWAYS, 0.0f );
	
	if( textureState )
		renderer::OpenGL::GetInstance()->DisableTexturing();

	renderer::OpenGL::GetInstance()->SetColour4ub( 0, 0, 0, alpha );

	glPushMatrix();
		if( renderer::OpenGL::GetInstance()->GetIsRotated() )
			glTranslatef( static_cast<float>(core::app::GetOrientationHeight())*0.5f, static_cast<float>(core::app::GetOrientationWidth())*0.5f, 1.0f );	
		else
			glTranslatef( static_cast<float>(core::app::GetOrientationWidth())*0.5f, static_cast<float>(core::app::GetOrientationHeight())*0.5f, 1.0f );			
	
		glVertexPointer( 3, GL_FLOAT, sizeof(math::Vec3), vaPoints );

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4 );
	glPopMatrix();

	if( textureState )
		renderer::OpenGL::GetInstance()->EnableTexturing();
	
	renderer::OpenGL::GetInstance()->SetColour4ub( 255, 255, 255, 255 );
	renderer::OpenGL::GetInstance()->DepthMode( true, GL_LESS );
	renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}

/////////////////////////////////////////////////////
/// Function: DrawFullscreenQuad
/// Params: [in]col
///
/////////////////////////////////////////////////////
void DrawFullscreenQuad( const math::Vec4Lite& col )
{
	bool textureState = renderer::OpenGL::GetInstance()->GetTextureState();

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_ALWAYS );
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	renderer::OpenGL::GetInstance()->AlphaMode( false, GL_ALWAYS, 0.0f );
	
	if( textureState )
		renderer::OpenGL::GetInstance()->DisableTexturing();

	renderer::OpenGL::GetInstance()->SetColour4ub( col.R, col.G, col.B, col.A );

	glPushMatrix();
		if( renderer::OpenGL::GetInstance()->GetIsRotated() )
			glTranslatef( static_cast<float>(core::app::GetOrientationHeight())*0.5f, static_cast<float>(core::app::GetOrientationWidth())*0.5f, 1.0f );	
		else
			glTranslatef( static_cast<float>(core::app::GetOrientationWidth())*0.5f, static_cast<float>(core::app::GetOrientationHeight())*0.5f, 1.0f );			
		glVertexPointer( 3, GL_FLOAT, sizeof(math::Vec3), vaPoints );

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4 );
	glPopMatrix();

	if( textureState )
		renderer::OpenGL::GetInstance()->EnableTexturing();
	
	renderer::OpenGL::GetInstance()->SetColour4ub( 255, 255, 255, 255 );
	renderer::OpenGL::GetInstance()->DepthMode( false, GL_LESS );
	renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}
