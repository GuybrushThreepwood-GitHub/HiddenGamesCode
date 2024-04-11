
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
#include "PhysicsBase.h"

#include "AppConsts.h"
#include "H4.h"

#include "Resources/ResourceHelper.h"

#include "GameEffects/FullscreenEffects.h"

namespace
{
	const int MAX_NOISE_FRAMES = 4;

	// noise
	int TexAnimFrame;
	GLuint NoiseTexId;
	float NoiseTime = 0.0f;
	int NoiseAlpha = 12;

	// fullscreen quad
	math::Vec3 vaPoints[4];
	math::Vec2 vaUVCoords[4][4];
	math::Vec4Lite vaColours[4];
}

/////////////////////////////////////////////////////
/// Function: LoadNoiseTextures
/// Params: None
///
/////////////////////////////////////////////////////
void LoadNoiseTextures()
{
	// NOISE
	TexAnimFrame = 0;
	NoiseTexId = renderer::INVALID_OBJECT;

	int textureSize = 512;
	NoiseTexId = res::LoadTextureName( "textures/fx", false, GL_NEAREST, GL_NEAREST );

	if( core::app::IsTablet() || 
	   core::app::IsRetinaDisplay() )
	{
		// frame 0
		vaUVCoords[0][0].U = renderer::GetUVCoord( 256, textureSize );
		vaUVCoords[0][0].V = renderer::GetUVCoord( 0, textureSize );
		// bottom right
		vaUVCoords[0][1].U = renderer::GetUVCoord( 512, textureSize );
		vaUVCoords[0][1].V = renderer::GetUVCoord( 0, textureSize );
		// top left
		vaUVCoords[0][2].U = renderer::GetUVCoord( 256, textureSize );
		vaUVCoords[0][2].V = renderer::GetUVCoord( 200, textureSize );
		// top right
		vaUVCoords[0][3].U = renderer::GetUVCoord( 512, textureSize );
		vaUVCoords[0][3].V = renderer::GetUVCoord( 200, textureSize );
		
		// frame 1
		vaUVCoords[1][0].U = renderer::GetUVCoord( 256, textureSize );
		vaUVCoords[1][0].V = renderer::GetUVCoord( 24, textureSize );
		// bottom right
		vaUVCoords[1][1].U = renderer::GetUVCoord( 512, textureSize );
		vaUVCoords[1][1].V = renderer::GetUVCoord( 24, textureSize );
		// top left
		vaUVCoords[1][2].U = renderer::GetUVCoord( 256, textureSize );
		vaUVCoords[1][2].V = renderer::GetUVCoord( 224, textureSize );
		// top right
		vaUVCoords[1][3].U = renderer::GetUVCoord( 512, textureSize );
		vaUVCoords[1][3].V = renderer::GetUVCoord( 224, textureSize );	
		
		// frame 2
		vaUVCoords[2][0].U = renderer::GetUVCoord( 256, textureSize );
		vaUVCoords[2][0].V = renderer::GetUVCoord( 56, textureSize );
		// bottom right
		vaUVCoords[2][1].U = renderer::GetUVCoord( 512, textureSize );
		vaUVCoords[2][1].V = renderer::GetUVCoord( 56, textureSize );
		// top left
		vaUVCoords[2][2].U = renderer::GetUVCoord( 256, textureSize );
		vaUVCoords[2][2].V = renderer::GetUVCoord( 256, textureSize );
		// top right
		vaUVCoords[2][3].U = renderer::GetUVCoord( 512, textureSize );
		vaUVCoords[2][3].V = renderer::GetUVCoord( 256, textureSize );
		
		// frame 3
		vaUVCoords[3][0].U = renderer::GetUVCoord( 256, textureSize );
		vaUVCoords[3][0].V = renderer::GetUVCoord( 4, textureSize );
		// bottom right
		vaUVCoords[3][1].U = renderer::GetUVCoord( 512, textureSize );
		vaUVCoords[3][1].V = renderer::GetUVCoord( 4, textureSize );
		// top left
		vaUVCoords[3][2].U = renderer::GetUVCoord( 256, textureSize );
		vaUVCoords[3][2].V = renderer::GetUVCoord( 204, textureSize );
		// top right
		vaUVCoords[3][3].U = renderer::GetUVCoord( 512, textureSize );
		vaUVCoords[3][3].V = renderer::GetUVCoord( 204, textureSize );
	}
	else 
	{
		// frame 0
		vaUVCoords[0][0].U = renderer::GetUVCoord( 256, textureSize );
		vaUVCoords[0][0].V = renderer::GetUVCoord( 128, textureSize );
		// bottom right
		vaUVCoords[0][1].U = renderer::GetUVCoord( 384, textureSize );
		vaUVCoords[0][1].V = renderer::GetUVCoord( 128, textureSize );
		// top left
		vaUVCoords[0][2].U = renderer::GetUVCoord( 256, textureSize );
		vaUVCoords[0][2].V = renderer::GetUVCoord( 256, textureSize );
		// top right
		vaUVCoords[0][3].U = renderer::GetUVCoord( 384, textureSize );
		vaUVCoords[0][3].V = renderer::GetUVCoord( 256, textureSize );
		
		// frame 1
		vaUVCoords[1][0].U = renderer::GetUVCoord( 384, textureSize );
		vaUVCoords[1][0].V = renderer::GetUVCoord( 128, textureSize );
		// bottom right
		vaUVCoords[1][1].U = renderer::GetUVCoord( 512, textureSize );
		vaUVCoords[1][1].V = renderer::GetUVCoord( 128, textureSize );
		// top left
		vaUVCoords[1][2].U = renderer::GetUVCoord( 384, textureSize );
		vaUVCoords[1][2].V = renderer::GetUVCoord( 256, textureSize );
		// top right
		vaUVCoords[1][3].U = renderer::GetUVCoord( 512, textureSize );
		vaUVCoords[1][3].V = renderer::GetUVCoord( 256, textureSize );
		
		// frame 2
		vaUVCoords[2][0].U = renderer::GetUVCoord( 256, textureSize );
		vaUVCoords[2][0].V = renderer::GetUVCoord( 0, textureSize );
		// bottom right
		vaUVCoords[2][1].U = renderer::GetUVCoord( 384, textureSize );
		vaUVCoords[2][1].V = renderer::GetUVCoord( 0, textureSize );
		// top left
		vaUVCoords[2][2].U = renderer::GetUVCoord( 256, textureSize );
		vaUVCoords[2][2].V = renderer::GetUVCoord( 128, textureSize );
		// top right
		vaUVCoords[2][3].U = renderer::GetUVCoord( 384, textureSize );
		vaUVCoords[2][3].V = renderer::GetUVCoord( 128, textureSize );
		
		// frame 3
		vaUVCoords[3][0].U = renderer::GetUVCoord( 384, textureSize );
		vaUVCoords[3][0].V = renderer::GetUVCoord( 0, textureSize );
		// bottom right
		vaUVCoords[3][1].U = renderer::GetUVCoord( 512, textureSize );
		vaUVCoords[3][1].V = renderer::GetUVCoord( 0, textureSize );
		// top left
		vaUVCoords[3][2].U = renderer::GetUVCoord( 384, textureSize );
		vaUVCoords[3][2].V = renderer::GetUVCoord( 128, textureSize );
		// top right
		vaUVCoords[3][3].U = renderer::GetUVCoord( 512, textureSize );
		vaUVCoords[3][3].V = renderer::GetUVCoord( 128, textureSize );		
	}


	vaColours[0] = math::Vec4Lite( 255, 255, 255, NoiseAlpha );
	vaColours[1] = math::Vec4Lite( 255, 255, 255, NoiseAlpha );
	vaColours[2] = math::Vec4Lite( 255, 255, 255, NoiseAlpha );
	vaColours[3] = math::Vec4Lite( 255, 255, 255, NoiseAlpha );
}

/////////////////////////////////////////////////////
/// Function: RemoveNoiseTextures
/// Params: None
///
/////////////////////////////////////////////////////
void RemoveNoiseTextures()
{
	renderer::RemoveTexture( NoiseTexId );
}

/////////////////////////////////////////////////////
/// Function: DrawNoise
/// Params: [in]zOffset, [in]deltaTime
///
/////////////////////////////////////////////////////
void DrawNoise( float zOffset, float deltaTime )
{
	vaPoints[0].Z = zOffset;
	vaPoints[1].Z = zOffset;
	vaPoints[2].Z = zOffset;
	vaPoints[3].Z = zOffset;
	
	if( NoiseTexId != renderer::INVALID_OBJECT )
	{
		renderer::OpenGL::GetInstance()->BindUnitTexture( 0, GL_TEXTURE_2D, NoiseTexId );
		renderer::OpenGL::GetInstance()->EnableTextureArray();
	}
	else
	{
		renderer::OpenGL::GetInstance()->DisableUnitTexture( 0 );
		renderer::OpenGL::GetInstance()->DisableTextureArray();
	}

	renderer::OpenGL::GetInstance()->EnableColourArray();

	glPushMatrix();
		if( renderer::OpenGL::GetInstance()->GetIsRotated() )
			glTranslatef( static_cast<float>(core::app::GetOrientationHeight())*0.5f, static_cast<float>(core::app::GetOrientationWidth())*0.5f, 1.0f );	
		else
			glTranslatef( static_cast<float>(core::app::GetOrientationWidth())*0.5f, static_cast<float>(core::app::GetOrientationHeight())*0.5f, 1.0f );			
		
		glColorPointer( 4, GL_UNSIGNED_BYTE, sizeof(math::Vec4Lite), vaColours );

		glVertexPointer( 3, GL_FLOAT, sizeof(math::Vec3), vaPoints );

		if( NoiseTexId != renderer::INVALID_OBJECT )
			glTexCoordPointer( 2, GL_FLOAT, sizeof(math::Vec2), vaUVCoords[TexAnimFrame] );

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4 );
	glPopMatrix();

	renderer::OpenGL::GetInstance()->DisableColourArray();

	if( NoiseTexId != renderer::INVALID_OBJECT )
		renderer::OpenGL::GetInstance()->DisableTextureArray();

	NoiseTime += deltaTime;
	
	float rate = ANIMATION_RATE;
	if( H4::GetHiResMode() )
		rate = ANIMATION_RATE_HI;
	
	if( NoiseTime >= rate )
	{
		TexAnimFrame++;
		NoiseTime = 0.0f;
	}

	if( TexAnimFrame >= MAX_NOISE_FRAMES )
		TexAnimFrame=0;
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
}

/////////////////////////////////////////////////////
/// Function: SetNoiseAlpha
/// Params: [in]alpha
///
/////////////////////////////////////////////////////
void SetNoiseAlpha( int alpha )
{
	NoiseAlpha = alpha;

	vaColours[0] = math::Vec4Lite( 255, 255, 255, NoiseAlpha );
	vaColours[1] = math::Vec4Lite( 255, 255, 255, NoiseAlpha );
	vaColours[2] = math::Vec4Lite( 255, 255, 255, NoiseAlpha );
	vaColours[3] = math::Vec4Lite( 255, 255, 255, NoiseAlpha );
}