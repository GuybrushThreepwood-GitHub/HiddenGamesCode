
#ifndef __ODEPHYSICSPRIMITIVES_H__
#define __ODEPHYSICSPRIMITIVES_H__

#ifdef BASE_SUPPORT_ODE
#if defined(BASE_SUPPORT_OPENGL) || defined(BASE_SUPPORT_OPENGLES)

#include <ode/ode.h>

namespace physics
{
	/// SetTransform - sets an opengl matrix with current position and rotation values
	/// \param pos - position values to set
	/// \param R - rotation values to set
	void SetTransform( const float pos[3], const float R[12] );
	/// PhysicsPrimitiveDrawBox - Draws a box
	/// \param sides - length of the sides based on lenght, breadth, height values
	void PhysicsPrimitiveDrawBox( const float sides[3] );
	/// PhysicsPrimitiveDrawPatch - Draws a patch
	/// \param p1 - 
	/// \param p2 - 
	/// \param p3 - 
	/// \param level - 
	void PhysicsPrimitiveDrawPatch( float p1[3], float p2[3], float p3[3], int level );
	/// PhysicsPrimitiveDrawSphere - Draws a sphere
	void PhysicsPrimitiveDrawSphere( void );
	/// PhysicsPrimitiveDrawTriangle - Draws a triangle
	/// \param v0 - vertex position 1
	/// \param v1 - vertex position 2
	/// \param v2 - vertex position 3
	/// \param solid - solid draw flag
	void PhysicsPrimitiveDrawTriangle( const float *v0, const float *v1, const float *v2, int solid );
	/// PhysicsPrimitiveDrawCappedCylinder - Draws a smooth ended cylinder
	/// \param l - length of cylinder
	/// \param r - radius of cylinder
	void PhysicsPrimitiveDrawCappedCylinder( float l, float r );
	/// PhysicsPrimitiveDrawCylinder - Draws a flat ended cylinder
	/// \param l - length of cylinder
	/// \param r - radius of cylinder
	/// \param zoffset - z offset
	void PhysicsPrimitiveDrawCylinder( float l, float r, float zoffset );

	/// DrawBox - Draws a 6 sided box
	/// \param pos - position of the box
	/// \param R - rotation of the box
	/// \param sides - 3 sizes for the length, breadth, height
	void DrawBox( const float pos[3], const float R[12], const float sides[3] );
	/// DrawSphere - Draws a sphere
	/// \param pos - position of the sphere
	/// \param R - rotation of the sphere
	/// \param radius - radius of the sphere
	void DrawSphere( const float pos[3], const float R[12], float radius );
	/// DrawTriangle - Draws a sphere
	/// \param pos - position of the triangle
	/// \param R - rotation of the triangle
	/// \param v0 - first vertex of the triangle
	/// \param v1 - second vertex of the triangle
	/// \param v2 - third vertex of the triangle
	/// \param solid - draw as a wireframe or solid triangle
	void DrawTriangle( const float pos[3], const float R[12], const float *v0, const float *v1, const float *v2, int solid );
	/// DrawCylinder - Draws a rounded cylinder
	/// \param pos - position of the cylinder
	/// \param R - rotation of the cylinder
	/// \param length - length of the cylinder
	/// \param radius - radius of the cylinder
	void DrawCylinder( const float pos[3], const float R[12], float length, float radius );
	/// DrawCappedCylinder - Draws a flat ended cylinder
	/// \param pos - position of the cylinder
	/// \param R - rotation of the cylinder
	/// \param length - length of the cylinder
	/// \param radius - radius of the cylinder
	void DrawCappedCylinder( const float pos[3], const float R[12], float length, float radius );
	/// DrawLine - Draws a line
	/// \param pos1 - start point of the line
	/// \param pos2 - end point of the line
	void DrawLine( const float pos1[3], const float pos2[3] );

	/// DrawPhysicsGeometry - draws a physics geometry object
	/// \param g - geometry ID
	/// \param pos - position
	/// \param R - rotation
	void DrawPhysicsGeometry( dGeomID g, const dReal *pos, const dReal *R);
	/// DrawPhysicsPlane - draws the drop out physics plane
	void DrawPhysicsPlane();

	/// CreatePlane - Creates an ODE plane
	/// \param a - normal on X-axis
	/// \param b - normal on Y-axis
	/// \param c - normal on Z-axis
	/// \param d - offset
	/// \return dGeomID - plane geometry id
	dGeomID CreatePlane( float a, float b, float c, float d );

	/// ShowAABBs - Turns the ODE AABBs on/off
	/// \param state - true is on, false is off
	void ShowAABBs( bool state );

	/// ShowPhysicsBodies - Turns the physics body drawing on/off
	/// \param state - true is on, false is off
	void ShowPhysicsBodies( bool state );

#ifdef BASE_ENABLE_ODE_TEXTURE
	/// InitialiseODETexture - create an opengl texture object for mapping on to physics objects
	void InitialiseODETexture();
	/// ReleaseODETexture - cleans up the ode texture
	void ReleaseODETexture();
	/// BindODETexture - binds the physics texture
	void BindODETexture();
#endif // BASE_ENABLE_ODE_TEXTURE

} // namespace physics

#endif // defined(BASE_SUPPORT_OPENGL) || defined(BASE_SUPPORT_OPENGLES)
#endif // BASE_SUPPORT_ODE

#endif // __ODEPHYSICSPRIMITIVES_H__

