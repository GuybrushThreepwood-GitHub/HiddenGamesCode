
/*===================================================================
	File: DebugPrint.cpp
	Library: Debug

	(C)Hidden Games
=====================================================================*/

#if defined(_DEBUG) || defined(BASE_USE_DEBUG_FONT)
#if defined(BASE_SUPPORT_OPENGL) || defined(BASE_SUPPORT_OPENGLES)

#include "CoreBase.h"

#include "Math/Vectors.h"
#include "Render/RenderConsts.h"
#include "Render/OpenGLCommon.h"
#include "Render/FF/OpenGL/OpenGL.h"
#include "Render/FF/OpenGLES/OpenGLES.h"

#include "Render/TextureShared.h"
#include "Render/Texture.h"
#include "Render/FF/OpenGL/TextureLoadAndUpload.h"
#include "Render/FF/OpenGLES/TextureLoadAndUploadOES.h"

#include "Debug/DebugFont.h"
#include "Debug/FF/DebugPrint.h"

namespace DbgPrint
{
	const int FONT_WIDTH = 8+1;
	const int FONT_HEIGHT = 8+1;
	const int FONT_CHAR_PER_ROW = 16;
	const int FONT_CHAR_PER_COLUMN = 16;
	const float FONT_DEPTH		= 0.5f;
	const float FONT_ALPHA		= 0.45f;
	const float FONT_SPACING = 4.0f;

	struct FontQuadDef
	{
		math::Vec3 ShadowOffset;

		math::Vec2 textUV[4];
		math::Vec2 textPoint[4];
		math::Vec4 textColour[4];
		math::Vec4 textShadowColour[4];

	};
	
	FontQuadDef FontQuad[FONT_CHAR_PER_ROW * FONT_CHAR_PER_COLUMN];

	GLuint TexID = renderer::INVALID_OBJECT;
	GLubyte fontRed=255, fontGreen=255, fontBlue=255, fontAlpha=255;

    renderer::OpenGL* pOpenGLContext=0;
}

/////////////////////////////////////////////////////
/// Function: DebugCreateFont
/// Params: None
///
/////////////////////////////////////////////////////
void dbg::DebugCreateFont( renderer::OpenGL* openGLContext )
{
    if( openGLContext == 0 )
        DbgPrint::pOpenGLContext = renderer::OpenGL::GetInstance();
    else
        DbgPrint::pOpenGLContext = openGLContext;
    
	unsigned int i=0;
	
	float cx, cy;
	float cwx, cwy;

    if( DbgPrint::TexID != renderer::INVALID_OBJECT )
    {
        renderer::RemoveTexture( DbgPrint::TexID );
        DbgPrint::TexID = renderer::INVALID_OBJECT;
    }
    
	glGenTextures( 1, &DbgPrint::TexID );
	DbgPrint::pOpenGLContext->BindTexture( DbgPrint::TexID );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, 128, 128, 0, GL_RGBA, GL_UNSIGNED_BYTE, debug_font8x8 );

	cwx = (1.0f / (DbgPrint::FONT_CHAR_PER_ROW*DbgPrint::FONT_WIDTH)) * DbgPrint::FONT_WIDTH;
	cwy = (1.0f / (DbgPrint::FONT_CHAR_PER_COLUMN*DbgPrint::FONT_HEIGHT)) * DbgPrint::FONT_HEIGHT;

	for( i=0; i < (DbgPrint::FONT_CHAR_PER_ROW * DbgPrint::FONT_CHAR_PER_COLUMN); ++i )
	{
		// X position of the char
		cx = (float)(i % DbgPrint::FONT_CHAR_PER_ROW) * cwx;
			
		// Y position of the char
		cy = (float)(i / DbgPrint::FONT_CHAR_PER_COLUMN) * cwy;

		DbgPrint::FontQuad[i].textUV[0] = math::Vec2( cx, 1.0f-cy-cwy ); 
		DbgPrint::FontQuad[i].textPoint[0] = math::Vec2( 0.0f, 0.0f ); 

		DbgPrint::FontQuad[i].textUV[1] = math::Vec2( cx+cwx, 1.0f-cy-cwy ); 
		DbgPrint::FontQuad[i].textPoint[1] = math::Vec2( DbgPrint::FONT_WIDTH - 1.0f, 0.0f ); 

		DbgPrint::FontQuad[i].textUV[2] = math::Vec2( cx, 1.0f-cy ); 
		DbgPrint::FontQuad[i].textPoint[2] =math::Vec2( 0.0f, DbgPrint::FONT_HEIGHT - 1.0f ); 

		DbgPrint::FontQuad[i].textUV[3] = math::Vec2( cx+cwx, 1.0f-cy  ); 
		DbgPrint::FontQuad[i].textPoint[3] = math::Vec2( DbgPrint::FONT_WIDTH - 1.0f, DbgPrint::FONT_HEIGHT - 1.0f ); 
	}
}

/////////////////////////////////////////////////////
/// Method: DebugSetFontColour
/// Params: [in]r, [in]g, [in]b, [in]a
///
/////////////////////////////////////////////////////
void dbg::DebugSetFontColour( unsigned char r, unsigned char g, unsigned char b, unsigned char a )
{
	DbgPrint::fontRed = r;
	DbgPrint::fontGreen = g;
	DbgPrint::fontBlue = b;
	DbgPrint::fontAlpha = a;
}

/////////////////////////////////////////////////////
/// Function: DebugPrint
/// Params: [in]x, [in]y, [in]szString
///
/////////////////////////////////////////////////////
void dbg::DebugPrint( int x, int y, const char* szString, ... )
{
	char szText[256];
	unsigned int i=0;
	std::va_list ap;				

	if( szString == 0 )		
		return;				

	va_start( ap, szString );					
		std::vsprintf( szText, szString, ap );			
	va_end( ap );

	DbgPrint::pOpenGLContext->DepthMode( false, GL_ALWAYS );
	DbgPrint::pOpenGLContext->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	DbgPrint::pOpenGLContext->AlphaMode( true, GL_GREATER, DbgPrint::FONT_ALPHA );
	DbgPrint::pOpenGLContext->DisableVBO();

	if( DbgPrint::TexID  != renderer::INVALID_OBJECT )
	{
		DbgPrint::pOpenGLContext->BindTexture( DbgPrint::TexID  );
	}
	else
		DbgPrint::pOpenGLContext->ClearUnitTexture( 0 );

	// set the font colour
	DbgPrint::pOpenGLContext->SetColour4ub( DbgPrint::fontRed, DbgPrint::fontGreen, DbgPrint::fontBlue, DbgPrint::fontAlpha );
	glClientActiveTexture( GL_TEXTURE0 );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	
	glPushMatrix();	
		glTranslatef( static_cast<float>(x), static_cast<float>(y), DbgPrint::FONT_DEPTH );
		//glScalef( 1.5f, 1.5f, 0.0f );
		for( i=0; i < (int)std::strlen(szText); ++i )
		{
			char index = szText[i];

			// don't want lower case
			index -= 32;

			glVertexPointer( 2, GL_FLOAT, 0, &DbgPrint::FontQuad[index].textPoint[0] );
			glTexCoordPointer( 2, GL_FLOAT, 0, &DbgPrint::FontQuad[index].textUV[0] );

			glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );	
			
			glTranslatef( DbgPrint::FONT_SPACING, 0.0f, 0.0f );
		}
	glPopMatrix();

	glClientActiveTexture( GL_TEXTURE0 );
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	
	DbgPrint::pOpenGLContext->DepthMode( true, GL_LESS );
	DbgPrint::pOpenGLContext->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	DbgPrint::pOpenGLContext->AlphaMode( false, GL_ALWAYS, 0.0f );
}

/////////////////////////////////////////////////////
/// Function: DebugPrint3D
/// Params: [in]x, [in]y, [in]z, [in]szString
///
/////////////////////////////////////////////////////
void dbg::DebugPrint3D( float x, float y, float z, const char* szString, ... )
{
	char szText[256];
	unsigned int i=0;
	std::va_list ap;				

	if( szString == 0 )		
		return;				

	va_start( ap, szString );					
		std::vsprintf( szText, szString, ap );			
	va_end( ap );

	//DbgPrint::pOpenGLContext->BlendMode( true, GL_ONE, GL_ZERO );
	DbgPrint::pOpenGLContext->AlphaMode( true, GL_GREATER, DbgPrint::FONT_ALPHA );
	DbgPrint::pOpenGLContext->DisableVBO();

	if( DbgPrint::TexID  != renderer::INVALID_OBJECT )
	{
		DbgPrint::pOpenGLContext->BindTexture( DbgPrint::TexID  );
	}
	else
		DbgPrint::pOpenGLContext->DisableUnitTexture( 0 );

	// set the font colour
	DbgPrint::pOpenGLContext->SetColour4ub( DbgPrint::fontRed, DbgPrint::fontGreen, DbgPrint::fontBlue, DbgPrint::fontAlpha );

	glClientActiveTexture( GL_TEXTURE0 );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );

	glPushMatrix();	
		glTranslatef( x, y, z );
		glScalef( 1.5f, 1.5f, 0.0f );
		for( i=0; i < (int)std::strlen(szText); ++i )
		{
			char index = szText[i];

			// don't want lower case
			index -= 32;

			glPushMatrix();
				glVertexPointer( 2, GL_FLOAT, 0, &DbgPrint::FontQuad[index].textPoint[0] );
				glTexCoordPointer( 2, GL_FLOAT, 0, &DbgPrint::FontQuad[index].textUV[0] );

				glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );	
			glPopMatrix();
			
			glTranslatef( DbgPrint::FONT_SPACING, 0.0f, 0.0f );
		}
		glScalef( 1.0f, 1.0f, 0.0f );
	glPopMatrix();

	glClientActiveTexture( GL_TEXTURE0 );
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );

	//DbgPrint::pOpenGLContext->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	DbgPrint::pOpenGLContext->AlphaMode( false, GL_ALWAYS, 0.0f );
}

/////////////////////////////////////////////////////
/// Function: DebugDestroyFont
/// Params: None
///
/////////////////////////////////////////////////////
void dbg::DebugDestroyFont()
{
	if( DbgPrint::TexID != renderer::INVALID_OBJECT )
	{
		renderer::RemoveTexture( DbgPrint::TexID );
		DbgPrint::TexID = renderer::INVALID_OBJECT;
	}
}

#endif // defined(BASE_SUPPORT_OPENGL) || defined(BASE_SUPPORT_OPENGLES)
#endif // defined(_DEBUG) || defined(BASE_USE_DEBUG_FONT)
