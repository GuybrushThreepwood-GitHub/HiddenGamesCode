/*
* Copyright (c) 2006-2007 Erin Catto http://www.gphysics.com
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/

#ifdef BASE_SUPPORT_BOX2D
#if defined(BASE_SUPPORT_OPENGL) || defined(BASE_SUPPORT_OPENGLES)

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"

#include "Physics/box2d/FF/Box2DDebugDraw.h"

/////////////////////////////////////////////////////
/// Method: Box2DPreDebugDraw
/// Params: None
///
/////////////////////////////////////////////////////
void physics::Box2DPreDebugDraw()
{

}

/////////////////////////////////////////////////////
/// Method: Box2DPostDebugDraw
/// Params: None
///
/////////////////////////////////////////////////////
void physics::Box2DPostDebugDraw()
{

}

/////////////////////////////////////////////////////
/// Method: DrawPolygon
/// Params: [in]vertices, [in]vertexCount, [in]color
///
/////////////////////////////////////////////////////
void physics::Box2DDebugDraw::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	renderer::OpenGL::GetInstance()->DepthMode( false, GL_LESS );

	renderer::OpenGL::GetInstance()->SetColour4f(color.r, color.g, color.b,1.0f);

		glVertexPointer(2, GL_FLOAT, sizeof(b2Vec2), vertices);
		glDrawArrays(GL_LINE_LOOP, 0, vertexCount );

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_LESS );
}

/////////////////////////////////////////////////////
/// Method: DrawSolidPolygon
/// Params: [in]vertices, [in]vertexCount, [in]color
///
/////////////////////////////////////////////////////
void physics::Box2DDebugDraw::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	renderer::OpenGL::GetInstance()->DepthMode( false, GL_LESS );

	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	renderer::OpenGL::GetInstance()->SetColour4f(0.5f * color.r, 0.5f * color.g, 0.5f * color.b, 0.5f);

		glVertexPointer(2, GL_FLOAT, sizeof(b2Vec2), vertices);
		glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount );
	renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	renderer::OpenGL::GetInstance()->SetColour4f(color.r, color.g, color.b, 1.0f);
		glVertexPointer(2, GL_FLOAT, sizeof(b2Vec2), vertices);
		glDrawArrays(GL_LINE_LOOP, 0, vertexCount );

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_LESS );
}

/////////////////////////////////////////////////////
/// Method: DrawCircle
/// Params: [in]vertices, [in]vertexCount, [in]color
///
/////////////////////////////////////////////////////
void physics::Box2DDebugDraw::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color)
{
	renderer::OpenGL::GetInstance()->DepthMode( false, GL_LESS );

	const float32 k_segments = 16.0f;
	const float32 k_increment = 2.0f * b2_pi / k_segments;
	float32 theta = 0.0f;
	renderer::OpenGL::GetInstance()->SetColour4f(color.r, color.g, color.b,1.0f);

		b2Vec2 vaPoints[16];	
		for (int32 i = 0; i < static_cast<int>(k_segments); ++i)
		{
			b2Vec2 v = center + radius * b2Vec2(std::cos(theta), std::sin(theta));
			vaPoints[i] = v;
			theta += k_increment;
		}
	
		glVertexPointer(2, GL_FLOAT, sizeof(b2Vec2), vaPoints);
		glDrawArrays(GL_LINE_LOOP, 0, 16 );


	renderer::OpenGL::GetInstance()->DepthMode( true, GL_LESS );
}

/////////////////////////////////////////////////////
/// Method: DrawSolidCircle
/// Params: [in]center, [in]radius, [in]axis, [in]color
///
/////////////////////////////////////////////////////
void physics::Box2DDebugDraw::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color)
{
	renderer::OpenGL::GetInstance()->DepthMode( false, GL_LESS );

	const float32 k_segments = 16.0f;
	const float32 k_increment = 2.0f * b2_pi / k_segments;
	float32 theta = 0.0f;
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	renderer::OpenGL::GetInstance()->SetColour4f(0.5f * color.r, 0.5f * color.g, 0.5f * color.b, 0.5f);

	b2Vec2 vaCirclePoints[16];	
	for (int32 i = 0; i < static_cast<int>(k_segments); ++i)
	{
		b2Vec2 v = center + radius * b2Vec2(std::cos(theta), std::sin(theta));
		vaCirclePoints[i] = v;
		theta += k_increment;
	}	

		glVertexPointer(2, GL_FLOAT, sizeof(b2Vec2), vaCirclePoints);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 16 );
	
		renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

		renderer::OpenGL::GetInstance()->SetColour4f(color.r, color.g, color.b, 1.0f);
		glVertexPointer(2, GL_FLOAT, sizeof(b2Vec2), vaCirclePoints);
		glDrawArrays(GL_LINE_LOOP, 0, 16 );

		b2Vec2 p = center + radius * axis;
		vaCirclePoints[0] = center;
		vaCirclePoints[1] = p;

		glVertexPointer(2, GL_FLOAT, sizeof(b2Vec2), vaCirclePoints);
		glDrawArrays(GL_LINES, 0, 2 );

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_LESS );
}

/////////////////////////////////////////////////////
/// Method: DrawSegment
/// Params: [in]p1, [in]p2, [in]color
///
/////////////////////////////////////////////////////
void physics::Box2DDebugDraw::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{
	renderer::OpenGL::GetInstance()->DepthMode( false, GL_LESS );

	renderer::OpenGL::GetInstance()->SetColour4f(color.r, color.g, color.b,1.0f);

		b2Vec2 vaSegmentPoints[2];

		vaSegmentPoints[0] = p1;
		vaSegmentPoints[1] = p2;

		glVertexPointer(2, GL_FLOAT, sizeof(b2Vec2), vaSegmentPoints);
		glDrawArrays(GL_LINES, 0, 2 );

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_LESS );
}

/////////////////////////////////////////////////////
/// Method: DrawTransform
/// Params: [in]xf
///
/////////////////////////////////////////////////////
void physics::Box2DDebugDraw::DrawTransform(const b2Transform& xf)
{
	renderer::OpenGL::GetInstance()->DepthMode( false, GL_LESS );

	b2Vec2 p1 = xf.p, p2;
	const float32 k_axisScale = 0.4f;

		b2Vec2 vaXFormPoints[2];

		vaXFormPoints[0] = p1;
		vaXFormPoints[1] = p1 + k_axisScale * xf.q.GetXAxis();
		
		renderer::OpenGL::GetInstance()->SetColour4f(1.0f, 0.0f, 0.0f,1.0f);

		glVertexPointer(2, GL_FLOAT, sizeof(b2Vec2), vaXFormPoints);
		glDrawArrays(GL_LINES, 0, 2 );

		vaXFormPoints[0] = p1;
		vaXFormPoints[1] = p1 + k_axisScale * xf.q.GetYAxis();

		renderer::OpenGL::GetInstance()->SetColour4f(0.0f, 1.0f, 0.0f, 1.0f);

		glVertexPointer(2, GL_FLOAT, sizeof(b2Vec2), vaXFormPoints);
		glDrawArrays(GL_LINES, 0, 2 );

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_LESS );
}

/////////////////////////////////////////////////////
/// Method: DrawPoint
/// Params: [in]p, [in]size, [in]color
///
/////////////////////////////////////////////////////
void physics::DrawPoint(const b2Vec2& p, float32 size, const b2Color& color)
{
	renderer::OpenGL::GetInstance()->DepthMode( false, GL_LESS );

	glPointSize(size);
	renderer::OpenGL::GetInstance()->SetColour4f(color.r, color.g, color.b,1.0f);

		glVertexPointer(2, GL_FLOAT, sizeof(b2Vec2), &p);
		glDrawArrays(GL_POINTS, 0, 1 );

	glPointSize(1.0f);

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_LESS );
}

/////////////////////////////////////////////////////
/// Method: DrawSegment
/// Params: [in]p1, [in]p2, [in]color
///
/////////////////////////////////////////////////////
void physics::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{
	renderer::OpenGL::GetInstance()->DepthMode( false, GL_LESS );

		b2Vec2 vaSegmentPoints[2];

		vaSegmentPoints[0] = p1;
		vaSegmentPoints[1] = p2;

		renderer::OpenGL::GetInstance()->SetColour4f(color.r, color.g, color.b,1.0f);

		glVertexPointer(2, GL_FLOAT, sizeof(b2Vec2), vaSegmentPoints);
		glDrawArrays(GL_LINES, 0, 2 );

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_LESS );
}

/////////////////////////////////////////////////////
/// Method: DrawString
/// Params: [in]x, [in]y, [in]string
///
/////////////////////////////////////////////////////
void physics::DrawString(int x, int y, const char *string, ...)
{

}

/////////////////////////////////////////////////////
/// Method: DrawAABB
/// Params: [in]aabb, [in]c
///
/////////////////////////////////////////////////////
void physics::DrawAABB(b2AABB* aabb, const b2Color& c)
{
	renderer::OpenGL::GetInstance()->DepthMode( false, GL_LESS );

	renderer::OpenGL::GetInstance()->SetColour4f(c.r, c.g, c.b,1.0f);

		b2Vec2 vaSegmentPoints[4];

		vaSegmentPoints[0] = b2Vec2( aabb->lowerBound.x, aabb->lowerBound.y );
		vaSegmentPoints[1] = b2Vec2( aabb->upperBound.x, aabb->lowerBound.y );
		vaSegmentPoints[2] = b2Vec2( aabb->upperBound.x, aabb->upperBound.y );
		vaSegmentPoints[3] = b2Vec2( aabb->lowerBound.x, aabb->upperBound.y );

		glVertexPointer(2, GL_FLOAT, sizeof(b2Vec2), vaSegmentPoints);
		glDrawArrays(GL_LINE_LOOP, 0, 4 );

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_LESS );
}

#endif // defined(BASE_SUPPORT_OPENGL) || defined(BASE_SUPPORT_OPENGLES)
#endif // BASE_SUPPORT_BOX2D

