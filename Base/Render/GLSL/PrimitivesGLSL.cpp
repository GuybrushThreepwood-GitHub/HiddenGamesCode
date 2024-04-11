
/*===================================================================
	File: PrimitivesGLSL.cpp
	Library: Render

	(C)Hidden Games
=====================================================================*/

#ifdef BASE_SUPPORT_OPENGL_GLSL

#include "CoreBase.h"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "MathBase.h"
#include "CollisionBase.h"

#include <cmath>
#include <vector>

#include "Render/OpenGLCommon.h"
#include "Render/GLSL/OpenGLSL.h"
#include "Render/GLSL/glewES.h"
#include "Render/GLSL/ShaderShared.h"

#include "Render/GLSL/PrimitivesGLSL.h"

namespace Primitives
{
	namespace sphere
	{
		//const unsigned int SPHERE_SLICES	= 16;
		//const unsigned int SPHERE_STACKS	= 16;

	} // namespace sphere

	GLuint PrimitivesProgram = renderer::INVALID_OBJECT;
	GLuint PrimitivesProgramBox2D = renderer::INVALID_OBJECT;
	GLuint PrimitivesProgramODE = renderer::INVALID_OBJECT;

	const char PrimitivesVertexShader[] = 
	"	//_PRIMITIVES_ precision lowp float;\n\
		attribute vec3 base_v;\n\
		varying vec4 colour0;\n\
		uniform mat4 ogl_ModelViewProjectionMatrix;\n\
		uniform vec4 ogl_VertexColour;\n\
		void main()\n\
		{\n\
			colour0 = ogl_VertexColour;\n\
			vec4 vInVertex = ogl_ModelViewProjectionMatrix * vec4(base_v, 1.0);\n\
			gl_Position = vInVertex;\n\
		}\n\
	";
	const char PrimitivesFragmentShader[] = 
	"	//_PRIMITIVES_ precision lowp float;\n\
		varying vec4 colour0;\n\
		void main()\n\
		{\n\
			gl_FragColor = colour0;\n\
		}\n\
	";


	const char ODEPrimitivesVertexShader[] = 
	"	//_PRIMITIVESODE_ precision mediump float;\n\
		uniform mat4 ogl_ModelViewProjectionMatrix;\n\
		uniform mat4 ogl_ModelViewMatrix;\n\
		uniform mat4 ogl_ViewMatrix;\n\
		uniform mat3 ogl_NormalMatrix;\n\
		attribute vec3 base_v;\n\
		attribute vec3 base_n;\n\
		varying vec3 eyePos;\n\
		varying vec3 lightPos;\n\
		varying vec3 normal;\n\
		void main()\n\
		{\n\
			normal = normalize(ogl_NormalMatrix * base_n);\n\
			vec4 pos = vec4(base_v, 1.0);\n\
		    gl_Position = ogl_ModelViewProjectionMatrix * pos;\n\
			pos = ogl_ModelViewMatrix * pos;\n\
			eyePos = (pos.xyz / pos.w);\n\
			lightPos = vec3(ogl_ViewMatrix * vec4(vec3(0.0,1.0,1.0), 0.0));\n\
		}\n\
	";
	const char ODEPrimitivesFragmentShader[] = 
	"	//_PRIMITIVESODE_ precision mediump float;\n\
		uniform vec4 ogl_VertexColour;\n\
		varying vec3 eyePos;\n\
		varying vec3 lightPos;\n\
		varying vec3 normal;\n\
		void main()\n\
		{\n\
			vec3 L = normalize(lightPos);\n\
			vec3 N = normalize(normal);\n\
			float nDotL = max(0.0, dot(N, L));\n\
			vec4 colour = ogl_VertexColour;\n\
			if( nDotL > 0.0 )\n\
			{\n\
				colour += ogl_VertexColour * nDotL;\n\
				vec3 viewDir = normalize(eyePos);\n\
				vec3 reflection = reflect(L, N);\n\
				float nDotHV = max(0.0, dot(reflection, viewDir));\n\
				if (nDotHV > 0.0)\n\
					colour += vec4(1.0,1.0,1.0,1.0) * pow(nDotHV, 1.0);\n\
			}\n\
			colour.a = ogl_VertexColour.a;\n\
			gl_FragColor = colour;\n\
		}\n\
	";

} // namespace Primitives

/////////////////////////////////////////////////////
/// Function: InitialisePrimitives
/// Params: None
///
/////////////////////////////////////////////////////
void renderer::InitialisePrimitives()
{
	ShutdownPrimitives();

	//Primitives::PrimitivesProgram = renderer::LoadShaderFilesForProgram( "shaders/ref/primitives.vert", "shaders/ref/primitives.frag" );
	Primitives::PrimitivesProgram = renderer::LoadShaderStringsForProgram( Primitives::PrimitivesVertexShader, Primitives::PrimitivesFragmentShader );

#ifdef BASE_SUPPORT_BOX2D
	Primitives::PrimitivesProgramBox2D = Primitives::PrimitivesProgram;//renderer::LoadShaderFilesForProgram( "shaders/ref/primitives-box2d.vert", "shaders/ref/primitives-box2d.frag" );
#endif // BASE_SUPPORT_BOX2D

#ifdef BASE_SUPPORT_ODE
	//Primitives::PrimitivesProgramODE = renderer::LoadShaderFilesForProgram( "shaders/ref/primitives-ode.vert", "shaders/ref/primitives-ode.frag" );
	Primitives::PrimitivesProgramODE = renderer::LoadShaderStringsForProgram( Primitives::ODEPrimitivesVertexShader, Primitives::ODEPrimitivesFragmentShader );
#endif // BASE_SUPPORT_ODE

}

/////////////////////////////////////////////////////
/// Method: ShutdownFreetype
/// Params: None
///
/////////////////////////////////////////////////////
void renderer::ShutdownPrimitives()
{
	if( Primitives::PrimitivesProgram != renderer::INVALID_OBJECT )
	{
		renderer::RemoveShaderProgram( Primitives::PrimitivesProgram );
		Primitives::PrimitivesProgram = renderer::INVALID_OBJECT;
		Primitives::PrimitivesProgramBox2D = renderer::INVALID_OBJECT;
	}

	if( Primitives::PrimitivesProgramODE != renderer::INVALID_OBJECT )
	{
		renderer::RemoveShaderProgram( Primitives::PrimitivesProgramODE );
		Primitives::PrimitivesProgramODE = renderer::INVALID_OBJECT;
	}
}

/////////////////////////////////////////////////////
/// Function: GetBox2DProgram
/// Params: None
///
/////////////////////////////////////////////////////
GLuint renderer::GetBox2DProgram()
{
	return Primitives::PrimitivesProgramBox2D;
}

/////////////////////////////////////////////////////
/// Function: GetODEProgram
/// Params: None
///
/////////////////////////////////////////////////////
GLuint renderer::GetODEProgram()
{
	return Primitives::PrimitivesProgramODE;
}

/////////////////////////////////////////////////////
/// Function: DrawSphere
/// Params: [in]radius
///
/////////////////////////////////////////////////////
void renderer::DrawSphere( float radius )
{
	renderer::OpenGL::GetInstance()->DisableVBO();

	if( Primitives::PrimitivesProgram == renderer::INVALID_OBJECT ||
		Primitives::PrimitivesProgram == 0 )
		return;

	GLuint prevProg = renderer::OpenGL::GetInstance()->GetCurrentProgram();

	renderer::OpenGL::GetInstance()->UseProgram( Primitives::PrimitivesProgram );

	math::Vec4Lite colour = renderer::OpenGL::GetInstance()->GetColour4ub();
	renderer::OpenGL::GetInstance()->SetColour4ub( colour.R, colour.G, colour.B, colour.A );

	// grab all matrices
	glm::mat4 projMatrix	= renderer::OpenGL::GetInstance()->GetProjectionMatrix();
	glm::mat4 viewMatrix	= renderer::OpenGL::GetInstance()->GetViewMatrix();
	glm::mat4 modelMatrix	= renderer::OpenGL::GetInstance()->GetModelMatrix();

	glm::mat4 modelViewMatrix = viewMatrix*modelMatrix;

	GLint ogl_ModelViewProjectionMatrix = glGetUniformLocation(Primitives::PrimitivesProgram, "ogl_ModelViewProjectionMatrix");
	GLint nVertexAttribLocation = glGetAttribLocation( Primitives::PrimitivesProgram, "base_v" );

	glEnableVertexAttribArray( nVertexAttribLocation );

	math::Vec2 vaPoints[16];	
	
	const float k_segments = 16.0f;
	const float k_increment = 2.0f * math::PI / k_segments;
	float theta = 0.0f;

	for (int i = 0; i < static_cast<int>(k_segments); ++i)
	{
		math::Vec2 v = math::Vec2(std::cos(theta), std::sin(theta)) * radius;
		vaPoints[i] = v;
		theta += k_increment;
	}

	glVertexAttribPointer( nVertexAttribLocation, 2, GL_FLOAT, GL_FALSE, sizeof(math::Vec2), vaPoints );

	if( ogl_ModelViewProjectionMatrix != -1 )
		glUniformMatrix4fv( ogl_ModelViewProjectionMatrix, 1, GL_FALSE, glm::value_ptr(projMatrix*modelViewMatrix) );

	glDrawArrays(GL_LINE_LOOP, 0, 16 );

	glm::mat4 objMatrix = glm::rotate( modelViewMatrix, 90.0f, glm::vec3(0.0f, 1.0f, 0.0f) );

	if( ogl_ModelViewProjectionMatrix != -1 )
		glUniformMatrix4fv( ogl_ModelViewProjectionMatrix, 1, GL_FALSE, glm::value_ptr(projMatrix*objMatrix) );

	glDrawArrays(GL_LINE_LOOP, 0, 16 );

	objMatrix = glm::rotate( modelViewMatrix, 45.0f, glm::vec3(0.0f, 1.0f, 0.0f) );

	if( ogl_ModelViewProjectionMatrix != -1 )
		glUniformMatrix4fv( ogl_ModelViewProjectionMatrix, 1, GL_FALSE, glm::value_ptr(projMatrix*objMatrix) );

	glDrawArrays(GL_LINE_LOOP, 0, 16 );

	objMatrix = glm::rotate( modelViewMatrix, -45.0f, glm::vec3(0.0f, 1.0f, 0.0f) );

	if( ogl_ModelViewProjectionMatrix != -1 )
		glUniformMatrix4fv( ogl_ModelViewProjectionMatrix, 1, GL_FALSE, glm::value_ptr(projMatrix*objMatrix) );
		
	glDrawArrays(GL_LINE_LOOP, 0, 16 );

	if( nVertexAttribLocation != -1 )
		glDisableVertexAttribArray( nVertexAttribLocation );

	renderer::OpenGL::GetInstance()->UseProgram( prevProg );
}

/////////////////////////////////////////////////////
/// Function: DrawAABB
/// Params: [in]vBoxMin, [in]vBoxMax
///
/////////////////////////////////////////////////////
void renderer::DrawAABB( const math::Vec3 &vBoxMin, const math::Vec3 &vBoxMax, bool filledBox )
{
	if( Primitives::PrimitivesProgram == renderer::INVALID_OBJECT ||
		Primitives::PrimitivesProgram == 0 )
		return;

	math::Vec3 vaSegmentPoints[10];

	renderer::OpenGL::GetInstance()->DisableVBO();

	GLuint prevProg = renderer::OpenGL::GetInstance()->GetCurrentProgram();

	renderer::OpenGL::GetInstance()->UseProgram( Primitives::PrimitivesProgram );

	math::Vec4Lite colour = renderer::OpenGL::GetInstance()->GetColour4ub();
	renderer::OpenGL::GetInstance()->SetColour4ub( colour.R, colour.G, colour.B, colour.A );

	// grab all matrices
	glm::mat4 projMatrix	= renderer::OpenGL::GetInstance()->GetProjectionMatrix();
	glm::mat4 viewMatrix	= renderer::OpenGL::GetInstance()->GetViewMatrix();
	glm::mat4 modelMatrix	= renderer::OpenGL::GetInstance()->GetModelMatrix();

	glm::mat4 modelViewMatrix = viewMatrix*modelMatrix;

	GLint ogl_ModelViewProjectionMatrix = glGetUniformLocation(Primitives::PrimitivesProgram, "ogl_ModelViewProjectionMatrix");
	GLint nVertexAttribLocation = glGetAttribLocation( Primitives::PrimitivesProgram, "base_v" );

	glEnableVertexAttribArray( nVertexAttribLocation );
	
	if( ogl_ModelViewProjectionMatrix != -1 )
		glUniformMatrix4fv( ogl_ModelViewProjectionMatrix, 1, GL_FALSE, glm::value_ptr(projMatrix*modelViewMatrix) );

	if( filledBox )
	{
		int i=0;

		math::Vec3 bbsides;
		for( i = 0; i < 3; i++ ) 
			bbsides[i] = vBoxMin[i] - vBoxMax[i];

		float lx = bbsides[0]*0.5f;
		float ly = bbsides[1]*0.5f;
		float lz = bbsides[2]*0.5f;

		// sides
		vaSegmentPoints[0] = math::Vec3(-lx, -ly, -lz);
		vaSegmentPoints[1] = math::Vec3(-lx, -ly, lz);
		vaSegmentPoints[2] = math::Vec3(-lx, ly, -lz);
		vaSegmentPoints[3] = math::Vec3(-lx, ly, lz);

		vaSegmentPoints[4] = math::Vec3(lx, ly, -lz);
		vaSegmentPoints[5] = math::Vec3(lx, ly, lz);

		vaSegmentPoints[6] = math::Vec3(lx, -ly, -lz);
		vaSegmentPoints[7] = math::Vec3(lx, -ly, lz);

		vaSegmentPoints[8] = math::Vec3(-lx, -ly, -lz);
		vaSegmentPoints[9] = math::Vec3(-lx, -ly, lz);

		glVertexAttribPointer( nVertexAttribLocation, 3, GL_FLOAT, GL_FALSE, sizeof(math::Vec3), vaSegmentPoints );

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 10 );

		// top face
		vaSegmentPoints[0] = math::Vec3(-lx, -ly, lz);
		vaSegmentPoints[1] = math::Vec3(lx, -ly, lz);
		vaSegmentPoints[2] = math::Vec3(lx, ly, lz);
		vaSegmentPoints[3] = math::Vec3(-lx, ly, lz);

		glVertexAttribPointer( nVertexAttribLocation, 3, GL_FLOAT, GL_FALSE, sizeof(math::Vec3), vaSegmentPoints );
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4 );

		// bottom face
		vaSegmentPoints[0] = math::Vec3(-lx, -ly, -lz);
		vaSegmentPoints[1] = math::Vec3(-lx, ly, -lz);
		vaSegmentPoints[2] = math::Vec3(lx, ly, -lz);
		vaSegmentPoints[3] = math::Vec3(lx, -ly, -lz);

		glVertexAttribPointer( nVertexAttribLocation, 3, GL_FLOAT, GL_FALSE, sizeof(math::Vec3), vaSegmentPoints );
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4 );
	}
	else
	{
		vaSegmentPoints[0] = math::Vec3( vBoxMin.X, vBoxMin.Y, vBoxMin.Z );
		vaSegmentPoints[1] = math::Vec3( vBoxMin.X, vBoxMin.Y, vBoxMax.Z );
		vaSegmentPoints[2] = math::Vec3( vBoxMax.X, vBoxMin.Y, vBoxMax.Z );
		vaSegmentPoints[3] = math::Vec3( vBoxMax.X, vBoxMin.Y, vBoxMin.Z );

		glVertexAttribPointer( nVertexAttribLocation, 3, GL_FLOAT, GL_FALSE, sizeof(math::Vec3), vaSegmentPoints );
		glDrawArrays(GL_LINE_LOOP, 0, 4 );

		vaSegmentPoints[0] = math::Vec3( vBoxMax.X, vBoxMax.Y, vBoxMax.Z );
		vaSegmentPoints[1] = math::Vec3( vBoxMax.X, vBoxMax.Y, vBoxMin.Z );
		vaSegmentPoints[2] = math::Vec3( vBoxMin.X, vBoxMax.Y, vBoxMin.Z );
		vaSegmentPoints[3] = math::Vec3( vBoxMin.X, vBoxMax.Y, vBoxMax.Z );

		glVertexAttribPointer( nVertexAttribLocation, 3, GL_FLOAT, GL_FALSE, sizeof(math::Vec3), vaSegmentPoints );
		glDrawArrays(GL_LINE_LOOP, 0, 4 );

		vaSegmentPoints[0] = math::Vec3( vBoxMin.X, vBoxMin.Y, vBoxMin.Z  );
		vaSegmentPoints[1] = math::Vec3( vBoxMin.X, vBoxMax.Y, vBoxMin.Z );
		vaSegmentPoints[2] = math::Vec3( vBoxMin.X, vBoxMin.Y, vBoxMax.Z );
		vaSegmentPoints[3] = math::Vec3( vBoxMin.X, vBoxMax.Y, vBoxMax.Z );

		vaSegmentPoints[4] = math::Vec3( vBoxMax.X, vBoxMin.Y, vBoxMax.Z );
		vaSegmentPoints[5] = math::Vec3( vBoxMax.X, vBoxMax.Y, vBoxMax.Z );
		vaSegmentPoints[6] = math::Vec3( vBoxMax.X, vBoxMin.Y, vBoxMin.Z );
		vaSegmentPoints[7] = math::Vec3( vBoxMax.X, vBoxMax.Y, vBoxMin.Z );

		glVertexAttribPointer( nVertexAttribLocation, 3, GL_FLOAT, GL_FALSE, sizeof(math::Vec3), vaSegmentPoints );
		glDrawArrays(GL_LINES, 0, 8 );
	}

	if( nVertexAttribLocation != -1 )
		glDisableVertexAttribArray( nVertexAttribLocation );

	renderer::OpenGL::GetInstance()->UseProgram( prevProg );
}

/////////////////////////////////////////////////////
/// Function: DrawOBB
/// Params: [in]vCenter, [in]vAxis, [in]vHalfWidths
///
/////////////////////////////////////////////////////
void renderer::DrawOBB( const math::Vec3& vCenter, const math::Vec3 vAxis[3], const math::Vec3& vHalfWidths )
{
	GLfloat m[16];

	m[0] = vAxis[0].X;	m[4] = vAxis[1].X;	m[8] = vAxis[2].X;	m[12] = 0.0f;
	m[1] = vAxis[0].Y;	m[5] = vAxis[1].Y;	m[9] = vAxis[2].Y;	m[13] = 0.0f;
	m[2] = vAxis[0].Z;	m[6] = vAxis[1].Z;	m[10] = vAxis[2].Z; m[14] = 0.0f;
	m[3] = 0.0f;		m[7] = 0.0f;		m[11] = 0.0f;		m[15] = 1.0f;

	glTranslatef( vCenter.X, vCenter.Y, vCenter.Z );
	glMultMatrixf( m );

	renderer::DrawAABB( -vHalfWidths, vHalfWidths );
}


/////////////////////////////////////////////////////
/// Function: DrawLine
/// Params: [in]start, [in]vBoxMax
///
/////////////////////////////////////////////////////
void renderer::DrawLine( const math::Vec3 &start, const math::Vec3 &end, const math::Vec4& colour )
{
	if( Primitives::PrimitivesProgram == renderer::INVALID_OBJECT ||
		Primitives::PrimitivesProgram == 0 )
		return;

	math::Vec3 vaSegmentPoints[2];

	renderer::OpenGL::GetInstance()->DisableVBO();

	GLuint prevProg = renderer::OpenGL::GetInstance()->GetCurrentProgram();

	renderer::OpenGL::GetInstance()->UseProgram( Primitives::PrimitivesProgram );

	renderer::OpenGL::GetInstance()->SetColour4f( colour.R, colour.G, colour.B, colour.A );

	// grab all matrices
	glm::mat4 projMatrix	= renderer::OpenGL::GetInstance()->GetProjectionMatrix();
	glm::mat4 viewMatrix	= renderer::OpenGL::GetInstance()->GetViewMatrix();
	glm::mat4 modelMatrix	= renderer::OpenGL::GetInstance()->GetModelMatrix();

	glm::mat4 modelViewMatrix = viewMatrix*modelMatrix;

	GLint ogl_ModelViewProjectionMatrix = glGetUniformLocation(Primitives::PrimitivesProgram, "ogl_ModelViewProjectionMatrix");
	GLint nVertexAttribLocation = glGetAttribLocation( Primitives::PrimitivesProgram, "base_v" );

	glEnableVertexAttribArray( nVertexAttribLocation );
	
	if( ogl_ModelViewProjectionMatrix != -1 )
		glUniformMatrix4fv( ogl_ModelViewProjectionMatrix, 1, GL_FALSE, glm::value_ptr(projMatrix*modelViewMatrix) );

	vaSegmentPoints[0] = start;
	vaSegmentPoints[1] = end;

	glVertexAttribPointer( nVertexAttribLocation, 3, GL_FLOAT, GL_FALSE, sizeof(math::Vec3), vaSegmentPoints );
	glDrawArrays(GL_LINES, 0, 2 );

	if( nVertexAttribLocation != -1 )
		glDisableVertexAttribArray( nVertexAttribLocation );

	renderer::OpenGL::GetInstance()->UseProgram( prevProg );
}

#endif // BASE_SUPPORT_OPENGL_GLSL
