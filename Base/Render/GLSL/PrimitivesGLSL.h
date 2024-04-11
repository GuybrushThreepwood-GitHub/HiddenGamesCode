
#ifndef __PRIMITIVESGLSL_H__
#define __PRIMITIVESGLSL_H__

#ifdef BASE_SUPPORT_OPENGL_GLSL

namespace renderer
{
	/// InitialisePrimitives - loads the shaders for rendering primitives
	void InitialisePrimitives();
	/// ShutdownPrimitives - releases the shaders for rendering primitives
	void ShutdownPrimitives();

	/// GetBox2DProgram - get the shader program for box2d
	/// \return GLuint - program id
	GLuint GetBox2DProgram();

	/// GetODEProgram - gets the shader program for ode
	/// \return GLuint - program id
	GLuint GetODEProgram();

	/// DrawSphere - draws a sphere
	/// \param radius - radius of the sphere
	void DrawSphere( float radius );

	/// DrawAABB - draws an AABB
	/// \param vBoxMin - min bounds of box
	/// \param vBoxMax - max bounds of box
	/// \param fillBox - whether to draw as a filled box
	void DrawAABB( const math::Vec3& vBoxMin, const math::Vec3& vBoxMax, bool fillBox=false );

	/// DrawOBB - draws an OBB
	/// \param vCenter - center of box
	/// \param vAxis - axis rotation
	/// \param vHalfWidths - max bounds of box
	void DrawOBB( const math::Vec3& vCenter, const math::Vec3 vAxis[3], const math::Vec3& vHalfWidths );

	/// DrawLine - draws a line
	/// \param start - start point of the line
	/// \param end - end point of the line
	/// \param colour - colour of the colour
	void DrawLine( const math::Vec3 &start, const math::Vec3 &end, const math::Vec4& colour );

} // namespace renderer

#endif // BASE_SUPPORT_OPENGL_GLSL

#endif // __PRIMITIVESGLSL_H__
