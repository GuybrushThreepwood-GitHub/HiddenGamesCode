
/*===================================================================
	File: Primitives.cpp
	Library: Render

	(C)Hidden Games
=====================================================================*/

#if defined(BASE_SUPPORT_OPENGL) || defined(BASE_SUPPORT_OPENGLES)

#include "CoreBase.h"
#include "MathBase.h"
#include "CollisionBase.h"

#include <cmath>
#include <vector>

#include "Render/OpenGLCommon.h"
#include "Render/FF/OpenGL/OpenGL.h"
#include "Render/FF/OpenGLES/OpenGLES.h"

#include "Render/FF/Primitives.h"

namespace Primitives
{
	namespace sphere
	{
		const unsigned int SPHERE_SLICES	= 16;
		const unsigned int SPHERE_STACKS	= 16;

	} // namespace sphere

} // namespace Primitives

/////////////////////////////////////////////////////
/// Function: DrawSphere
/// Params: [in]radius
///
/////////////////////////////////////////////////////
void renderer::DrawSphere( float radius )
{
	math::Vec2 vaPoints[16];	
	
	const float k_segments = 16.0f;
	const float k_increment = 2.0f * math::PI / k_segments;
	float theta = 0.0f;

	bool textureState = renderer::OpenGL::GetInstance()->GetTextureState();
	bool lightState = renderer::OpenGL::GetInstance()->GetLightingState();
	renderer::OpenGL::GetInstance()->DisableVBO();

	if( textureState )
		renderer::OpenGL::GetInstance()->DisableTexturing();

	if( lightState )
		renderer::OpenGL::GetInstance()->DisableLighting();

	for (int i = 0; i < static_cast<int>(k_segments); ++i)
	{
		math::Vec2 v = math::Vec2(std::cos(theta), std::sin(theta)) * radius;
		vaPoints[i] = v;
		theta += k_increment;
	}
	
	glVertexPointer(2, GL_FLOAT, sizeof(math::Vec2), vaPoints);

	glPushMatrix();
		glDrawArrays(GL_LINE_LOOP, 0, 16 );
	glPopMatrix();
	glPushMatrix();
		glRotatef( 90.0f, 0.0f, 1.0f, 0.0f );
		glDrawArrays(GL_LINE_LOOP, 0, 16 );
	glPopMatrix();
	glPushMatrix();
		glRotatef( 45.0f, 0.0f, 1.0f, 0.0f );
		glDrawArrays(GL_LINE_LOOP, 0, 16 );
	glPopMatrix();
	glPushMatrix();
		glRotatef( -45.0f, 0.0f, 1.0f, 0.0f );
		glDrawArrays(GL_LINE_LOOP, 0, 16 );
	glPopMatrix();

	if( textureState )
		renderer::OpenGL::GetInstance()->EnableTexturing();

	if( lightState )
		renderer::OpenGL::GetInstance()->EnableLighting();
}

/////////////////////////////////////////////////////
/// Function: DrawAABB
/// Params: [in]vBoxMin, [in]vBoxMax
///
/////////////////////////////////////////////////////
void renderer::DrawAABB( const math::Vec3 &vBoxMin, const math::Vec3 &vBoxMax, bool filledBox )
{
	math::Vec3 vaSegmentPoints[10];

	renderer::OpenGL::GetInstance()->DisableVBO();
	
	bool textureState = renderer::OpenGL::GetInstance()->GetTextureState();
	bool lightState = renderer::OpenGL::GetInstance()->GetLightingState();

	if( textureState )
		renderer::OpenGL::GetInstance()->DisableTexturing();

	if( lightState )
		renderer::OpenGL::GetInstance()->DisableLighting();
	
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

		glVertexPointer(3, GL_FLOAT, sizeof(math::Vec3), vaSegmentPoints);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 10 );

		// top face
		vaSegmentPoints[0] = math::Vec3(-lx, -ly, lz);
		vaSegmentPoints[1] = math::Vec3(lx, -ly, lz);
		vaSegmentPoints[2] = math::Vec3(lx, ly, lz);
		vaSegmentPoints[3] = math::Vec3(-lx, ly, lz);

		glVertexPointer(3, GL_FLOAT, sizeof(math::Vec3), vaSegmentPoints);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4 );

		// bottom face
		vaSegmentPoints[0] = math::Vec3(-lx, -ly, -lz);
		vaSegmentPoints[1] = math::Vec3(-lx, ly, -lz);
		vaSegmentPoints[2] = math::Vec3(lx, ly, -lz);
		vaSegmentPoints[3] = math::Vec3(lx, -ly, -lz);

		glVertexPointer(3, GL_FLOAT, sizeof(math::Vec3), vaSegmentPoints);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4 );
	}
	else
	{
		vaSegmentPoints[0] = math::Vec3( vBoxMin.X, vBoxMin.Y, vBoxMin.Z );
		vaSegmentPoints[1] = math::Vec3( vBoxMin.X, vBoxMin.Y, vBoxMax.Z );
		vaSegmentPoints[2] = math::Vec3( vBoxMax.X, vBoxMin.Y, vBoxMax.Z );
		vaSegmentPoints[3] = math::Vec3( vBoxMax.X, vBoxMin.Y, vBoxMin.Z );

		glVertexPointer(3, GL_FLOAT, sizeof(math::Vec3), vaSegmentPoints);
		glDrawArrays(GL_LINE_LOOP, 0, 4 );

		vaSegmentPoints[0] = math::Vec3( vBoxMax.X, vBoxMax.Y, vBoxMax.Z );
		vaSegmentPoints[1] = math::Vec3( vBoxMax.X, vBoxMax.Y, vBoxMin.Z );
		vaSegmentPoints[2] = math::Vec3( vBoxMin.X, vBoxMax.Y, vBoxMin.Z );
		vaSegmentPoints[3] = math::Vec3( vBoxMin.X, vBoxMax.Y, vBoxMax.Z );

		glVertexPointer(3, GL_FLOAT, sizeof(math::Vec3), vaSegmentPoints);
		glDrawArrays(GL_LINE_LOOP, 0, 4 );

		vaSegmentPoints[0] = math::Vec3( vBoxMin.X, vBoxMin.Y, vBoxMin.Z  );
		vaSegmentPoints[1] = math::Vec3( vBoxMin.X, vBoxMax.Y, vBoxMin.Z );
		vaSegmentPoints[2] = math::Vec3( vBoxMin.X, vBoxMin.Y, vBoxMax.Z );
		vaSegmentPoints[3] = math::Vec3( vBoxMin.X, vBoxMax.Y, vBoxMax.Z );

		vaSegmentPoints[4] = math::Vec3( vBoxMax.X, vBoxMin.Y, vBoxMax.Z );
		vaSegmentPoints[5] = math::Vec3( vBoxMax.X, vBoxMax.Y, vBoxMax.Z );
		vaSegmentPoints[6] = math::Vec3( vBoxMax.X, vBoxMin.Y, vBoxMin.Z );
		vaSegmentPoints[7] = math::Vec3( vBoxMax.X, vBoxMax.Y, vBoxMin.Z );

		glVertexPointer(3, GL_FLOAT, sizeof(math::Vec3), vaSegmentPoints);
		glDrawArrays(GL_LINES, 0, 8 );
	}

	if( textureState )
		renderer::OpenGL::GetInstance()->EnableTexturing();

	if( lightState )
		renderer::OpenGL::GetInstance()->EnableLighting();
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
	math::Vec3 vaSegmentPoints[2];

	renderer::OpenGL::GetInstance()->DisableVBO();
	
	bool textureState = renderer::OpenGL::GetInstance()->GetTextureState();
	bool lightState = renderer::OpenGL::GetInstance()->GetLightingState();

	if( textureState )
		renderer::OpenGL::GetInstance()->DisableTexturing();

	if( lightState )
		renderer::OpenGL::GetInstance()->DisableLighting();

	renderer::OpenGL::GetInstance()->SetColour4f( colour.R, colour.G, colour.B, colour.A );

	vaSegmentPoints[0] = start;
	vaSegmentPoints[1] = end;

	glVertexPointer(3, GL_FLOAT, sizeof(math::Vec3), vaSegmentPoints);
	glDrawArrays(GL_LINES, 0, 2 );

	if( textureState )
		renderer::OpenGL::GetInstance()->EnableTexturing();

	if( lightState )
		renderer::OpenGL::GetInstance()->EnableLighting();
}

#endif // defined(BASE_SUPPORT_OPENGL) || defined(BASE_SUPPORT_OPENGLES)
