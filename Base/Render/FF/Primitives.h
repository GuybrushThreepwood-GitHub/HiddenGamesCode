
#ifndef __PRIMITIVES_H__
#define __PRIMITIVES_H__

#if defined(BASE_SUPPORT_OPENGL) || defined(BASE_SUPPORT_OPENGLES)

namespace renderer
{
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

#endif // defined(BASE_SUPPORT_OPENGL) || defined(BASE_SUPPORT_OPENGLES)

#endif // __PRIMITIVES_H__
