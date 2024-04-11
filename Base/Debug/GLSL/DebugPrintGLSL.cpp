
/*===================================================================
	File: DebugPrintGLSL.cpp
	Library: Debug

	(C)Hidden Games
=====================================================================*/

#if defined(_DEBUG) || defined(BASE_USE_DEBUG_FONT)
#ifdef BASE_SUPPORT_OPENGL_GLSL

#include "CoreBase.h"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Math/Vectors.h"
#include "Render/RenderConsts.h"
#include "Render/OpenGLCommon.h"
#include "Render/GLSL/OpenGLSL.h"
#include "Render/GLSL/glewES.h"
#include "Render/GLSL/ShaderShared.h"

#include "Render/TextureShared.h"
#include "Render/Texture.h"
#include "Render/GLSL/TextureLoadAndUploadGLSL.h"

#include "Debug/DebugFont.h"
#include "Debug/GLSL/DebugPrintGLSL.h"

namespace DbgPrint
{
	const int FONT_WIDTH = 8+1;
	const int FONT_HEIGHT = 8+1;
	const int FONT_CHAR_PER_ROW = 16;
	const int FONT_CHAR_PER_COLUMN = 16;
	const float FONT_DEPTH		= 0.5f;
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

	GLuint DebugPrintProgram = renderer::INVALID_OBJECT;
    renderer::OpenGL* pOpenGLContext=0;
    
	const char DbgPrintVertexShader[] = 
	"	//_DBGPRINT_ precision highp float;\n\
		attribute vec3 base_v;\n\
		attribute vec2 base_uv0;\n\
		varying vec2 tu0;\n\
		varying vec4 colour0;\n\
		uniform mat4 ogl_ModelViewProjectionMatrix;\n\
		uniform vec4 ogl_VertexColour;\n\
		void main()\n\
		{\n\
			tu0 = base_uv0;\n\
			colour0 = ogl_VertexColour;\n\
			vec4 vInVertex = ogl_ModelViewProjectionMatrix * vec4(base_v, 1.0);\n\
			gl_Position = vInVertex;\n\
		}\n\
	";
	const char DbgPrintFragmentShader[] = 
	"	//_DBGPRINT_ precision highp float;\n\
		uniform sampler2D texUnit0;\n\
		varying vec2 tu0;\n\
		varying vec4 colour0;\n\
		void main()\n\
		{\n\
			vec4 color = colour0 * texture2D(texUnit0, tu0.xy);\n\
			if( color.a < 0.4 )\n\
				discard;\n\
			gl_FragColor = color;\n\
		}\n\
	";
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
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
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

    if( DbgPrint::DebugPrintProgram != renderer::INVALID_OBJECT )
    {
        renderer::RemoveShaderProgram( DbgPrint::DebugPrintProgram );
        DbgPrint::DebugPrintProgram = renderer::INVALID_OBJECT;
    }
    
	DbgPrint::DebugPrintProgram = renderer::LoadShaderStringsForProgram( DbgPrint::DbgPrintVertexShader, DbgPrint::DbgPrintFragmentShader );
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

	if( DbgPrint::DebugPrintProgram == renderer::INVALID_OBJECT ||
		DbgPrint::DebugPrintProgram == 0 )
		return;

	va_start( ap, szString );					
		std::vsprintf( szText, szString, ap );			
	va_end( ap );

	DbgPrint::pOpenGLContext->DepthMode( true, GL_ALWAYS );
	DbgPrint::pOpenGLContext->DisableVBO();

	GLuint prevProg = DbgPrint::pOpenGLContext->GetCurrentProgram();
	DbgPrint::pOpenGLContext->UseProgram(DbgPrint::DebugPrintProgram);

	// grab all matrices
	glm::mat4 projMat	= DbgPrint::pOpenGLContext->GetProjectionMatrix();
	glm::mat4 modelMat	= DbgPrint::pOpenGLContext->GetModelMatrix();
	
	GLint nVertexAttribLocation = glGetAttribLocation( DbgPrint::DebugPrintProgram, "base_v" );
	GLint nTexCoordsAttribLocation = glGetAttribLocation( DbgPrint::DebugPrintProgram, "base_uv0" );

	if( DbgPrint::TexID  != renderer::INVALID_OBJECT )
		DbgPrint::pOpenGLContext->BindTexture( DbgPrint::TexID  );

	// set the font colour
	DbgPrint::pOpenGLContext->SetColour4ub( DbgPrint::fontRed, DbgPrint::fontGreen, DbgPrint::fontBlue, DbgPrint::fontAlpha );
		
	glm::mat4 objMatrix = glm::translate( modelMat, glm::vec3(static_cast<float>(x), static_cast<float>(y), DbgPrint::FONT_DEPTH) );

	glEnableVertexAttribArray( nVertexAttribLocation );
	glEnableVertexAttribArray( nTexCoordsAttribLocation );

	for( i=0; i < (int)std::strlen(szText); ++i )
	{
		char index = szText[i];

		// don't want lower case
		index -= 32;

		// gl_ModelViewProjectionMatrix
		GLint ogl_ModelViewProjectionMatrix = glGetUniformLocation(DbgPrint::DebugPrintProgram, "ogl_ModelViewProjectionMatrix");
		if( ogl_ModelViewProjectionMatrix != -1 )
			glUniformMatrix4fv( ogl_ModelViewProjectionMatrix, 1, GL_FALSE, glm::value_ptr(projMat*objMatrix) );

		glVertexAttribPointer( nVertexAttribLocation, 2, GL_FLOAT, GL_FALSE, sizeof(math::Vec2), &DbgPrint::FontQuad[index].textPoint[0]  );

		glVertexAttribPointer( nTexCoordsAttribLocation, 2, GL_FLOAT, GL_FALSE, sizeof(math::Vec2), &DbgPrint::FontQuad[index].textUV[0]  );

		glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );	

		objMatrix = glm::translate( objMatrix, glm::vec3(DbgPrint::FONT_SPACING, 0.0f, 0.0f) );
	}

	if( nVertexAttribLocation != -1 )
		glDisableVertexAttribArray( nVertexAttribLocation );

	if( nTexCoordsAttribLocation != -1 )
		glDisableVertexAttribArray( nTexCoordsAttribLocation );

	DbgPrint::pOpenGLContext->DepthMode( true, GL_LESS );
	DbgPrint::pOpenGLContext->AlphaMode( false, GL_ALWAYS, 0.0f );

	DbgPrint::pOpenGLContext->UseProgram(prevProg);
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

	if( DbgPrint::DebugPrintProgram != renderer::INVALID_OBJECT )
	{
		renderer::RemoveShaderProgram( DbgPrint::DebugPrintProgram );
		DbgPrint::DebugPrintProgram = renderer::INVALID_OBJECT;
	}
}

#endif // BASE_SUPPORT_OPENGL_GLSL
#endif // defined(_DEBUG) || defined(BASE_USE_DEBUG_FONT)
