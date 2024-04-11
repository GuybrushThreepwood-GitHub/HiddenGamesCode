

#ifndef __OPENGLSL_H__
#define __OPENGLSL_H__

#ifndef __DEBUGBASE_H__
	#include "DebugBase.h"
#endif // __DEBUGBASE_H__

#ifdef BASE_PLATFORM_WINDOWS
	#ifndef __WININCLUDES_H__
		#include "Core/win/WinIncludes.h"
	#endif // __WININCLUDES_H__
#endif // BASE_PLATFORM_WINDOWS

#ifndef __PLATFORMRENDERBASE_H__
	#include "Render/PlatformRenderBase.h"
#endif // __PLATFORMRENDERBASE_H__

#ifndef __OPENGLCOMMON_H__
	#include "Render/OpenGLCommon.h"
#endif // __OPENGLCOMMON_H__

#ifdef BASE_SUPPORT_OPENGL_GLSL

#include "glm/glm.hpp"

// forward declare
namespace math { class Vec3; class Vec4; }
namespace collision { class AABB; }

namespace renderer
{
	class OpenGL
	{
		public:
			/// default constructor
			OpenGL();
			/// default destructor
			~OpenGL();

			/// Initialise - Sets up just the opengl default values
			void Init( void );
			/// cleanup
			/// Release - Clears out all allocated data by the class
			void Release( void );

#ifdef BASE_PLATFORM_WINDOWS
			/// SetupDisplay (WIN32) - Creates an OpenGL display 
			/// \param hDC - device context to use
			/// \param nBPP - bits-per-pixel to request
			/// \param nZBuffer - amount of bits in the zbuffer
			/// \param useMSAA - whether to request an MSAA format
			void SetupDisplay( HDC hDC, GLint nBPP, GLint nZBuffer );
			/// SetupPixelFormat (WIN32) - Finds a suitable pixel format based on the parameters
			/// \param nBPP - bits-per-pixel to request
			/// \param nZBuffer - amount of bits in the zbuffer
			/// \param useMSAA - whether to request an MSAA format
			/// \return integer - ( SUCCESS:OK or FAIL:FAIL )
			GLint SetupPixelFormat( GLint nBPP, GLint nZBuffer );
			/// GetGLRC - Gets the current GL render context
			/// \return HGLRC - handle to the GL render context
			HGLRC GetGLRC( void )			{ return m_hGLRC; }
#endif // BASE_PLATFORM_WINDOWS

#ifdef BASE_PLATFORM_RASPBERRYPI
			/// SetupDisplay - Creates an OpenGL display 
			/// \param width - width of the screen
			/// \param height - height of the screen
			void SetupDisplay( int width, int height ); 
#endif // BASE_PLATFORM_RASPBERRYPI
        
            /// ResetGLState - Puts the GL state back to its initial settings
            void ResetGLState();
        
			// standard GL setup methods

			/// UseProgram - Uses the passed program
			void UseProgram( GLuint program );
			/// ClearProgram - clears the current program
			void ClearProgram();

			/// EnableVertexArray - Enables vertex array client state
			void EnableVertexArray( void );
			/// DisableVertexArray - Disables vertex array client state
			void DisableVertexArray( void );
			/// EnableTextureArray - Enables texture array client state
			void EnableTextureArray( void );
			/// DisableTextureArray - Disables texture array client state
			void DisableTextureArray( void );
			/// EnableColourArray - Enables colour array client state
			void EnableColourArray( void );
			/// DisableColourArray - Disables colour array client state
			void DisableColourArray( void );
			/// EnableNormalArray - Enables normal array client state
			void EnableNormalArray( void );
			/// DisableNormalArray - Disables normal array client state
			void DisableNormalArray( void );

			/// DisableVBO - Disables VBO bound buffers
			void DisableVBO( void );

			/// EnableTexturing - Enables all texturing
			void EnableTexturing( void );
			/// DisableTexturing - Disables all texturing
			void DisableTexturing( void );
			/// SetCullState - Sets the state of culling
			/// \param bCull - cull flag
			/// \param eFace - which face to cull
			void SetCullState( bool bCull, GLenum eFace );
			/// EnableTexturing - Enables lighting
			void EnableLighting( void );
			/// DisableTexturing - Disables lighting
			void DisableLighting( void );
			/// BindTexture - binds a new texture
			/// \param nTexID - GL texture object
			void BindTexture( GLuint nTexID );
			/// BindUnitTexture - binds a new texture to a tex unit
			/// \param nTexUnit - which texture unit
			/// \param eTarget - what type of target 1D/2D/3D
			/// \param nTexID - GL texture object
			/// \param bForceBind - makes the texture change regardless of whether it's already the bound texture
			void BindUnitTexture( GLint nTexUnit, GLenum eTarget, GLuint nTexID, bool bForceBind = false );
			/// BindFrameBufferObject - binds a new framebuffer object
			/// \param nFrameBufferID - GL framebuffer object
			void BindFrameBufferObject( GLuint nFrameBufferID );
			/// ClearUnitTextures - clears all textures for all units
			void ClearUnitTextures( void );
			/// ClearUnitTexture - clears all textures on a texture unit
			/// \param nTexUnit - which texture unit
			void ClearUnitTexture( GLint nTexUnit );
			/// DisableUnitTexture - disables a texture target on a texture unit
			/// \param nTexUnit - which texture unit
			void DisableUnitTexture( GLint nTexUnit );
			/// ClearFrameBufferObject - unbinds any current Frame Buffer Object
			void ClearFrameBufferObject( void );
			/// SetMipmapParameters - sets the current mipmap mode on a texture unit
			/// \param nTexUnit - which texture unit
			/// \param eTarget - what type of target
			/// \param eParameter - mipmap parameter to change
			/// \param fValue - value of the parameter
			void SetMipmapParameters( GLint nTexUnit, GLenum eTarget, GLenum eParameter, GLfloat fValue );
			/// SetMipmapLodParameters - sets the current mipmap lod mode params on a texture unit
			/// \param nTexUnit - which texture unit
			/// \param eTarget - what type of target
			/// \param eParameter - mipmap parameter to change
			/// \param fValue - value of the parameter
			void SetMipmapLodParameters( GLint nTexUnit, GLenum eTarget, GLenum eParameter, GLfloat fValue );
			/// DepthMode - Sets the current depth test state
			/// \param bDepthFlag - depth test flag
			/// \param eDepthFunc - depth test function
			void DepthMode( bool bDepthFlag, GLenum eDepthFunc );
			/// BlendMode - Sets the current blend test state
			/// \param bBlendFlag - blend flag
			/// \param eBlendSrc - source blend function
			/// \param eBlendDest - destination blend function
			void BlendMode( bool bBlendFlag, GLenum eBlendSrc, GLenum eBlendDest );
			/// AlphaMode - Sets the current alpha test state
			/// \param bAlphaFlag - alpha flag
			/// \param eAlphaFunc - alpha test function
			/// \param fAlphaClamp - alpha test clamp
			void AlphaMode( bool bAlphaFlag, GLenum eAlphaFunc, GLfloat fAlphaClamp );

			/// EnableColourMaterial - Enables colour material mode
			void EnableColourMaterial();
			/// DisableColourMaterial - Disables colour material mode
			void DisableColourMaterial();

			/// SetMaterialAmbient - Sets the material ambient
			/// \param ambientColour - the ambient colour
			void SetMaterialAmbient( const math::Vec4& ambientColour );
			/// SetMaterialDiffuse - Sets the material diffuse
			/// \param diffuseColour - the diffuse colour
			void SetMaterialDiffuse( const math::Vec4& diffuseColour );
			/// SetMaterialSpecular - Sets the material specular
			/// \param specularColour - the diffuse colour
			void SetMaterialSpecular( const math::Vec4& specularColour );
			/// SetMaterialEmission - Sets the material emission
			/// \param emissionColour -  the emission colour
			void SetMaterialEmission( const math::Vec4& emissionColour );
			/// SetMaterialShininess - Sets the material shininess
			/// \param shininess -  the shininess
			void SetMaterialShininess( GLfloat shininess );

			/// EnableLight - Enables the hardware light
			/// \param nLightUnit - which light unit to set
			void EnableLight( int nLightUnit );
			/// DisableLight - Disables the hardware light
			/// \param nLightUnit - which light unit to set
			void DisableLight( int nLightUnit );
			/// SetLightPosition - Sets the light position
			/// \param nLightUnit - which light unit to set
			/// \param pos - the position
			void SetLightPosition( int nLightUnit, const math::Vec4& pos );
			/// SetLightAmbient - Sets the light ambient
			/// \param nLightUnit - which light unit to set
			/// \param ambientColour - the ambient colour
			void SetLightAmbient( int nLightUnit, const math::Vec4& ambientColour );
			/// SetLightDiffuse - Sets the light diffuse
			/// \param nLightUnit - which light unit to set
			/// \param diffuseColour - the diffuse colour
			void SetLightDiffuse( int nLightUnit, const math::Vec4& diffuseColour );
			/// SetLightSpecular - Sets the light specular
			/// \param nLightUnit - which light unit to set
			/// \param specularColour - the specular colour
			void SetLightSpecular( int nLightUnit, const math::Vec4& specularColour );
			/// SetLightEmission - Sets the light emission
			/// \param nLightUnit - which light unit to set
			/// \param emissionColour - the emission colour
			//void SetLightEmission( int nLightUnit, const math::Vec4& emissionColour );
			/// SetLightAttenuation - Sets the light attenuation
			/// \param nLightUnit - which light unit to set
			/// \param type - attenuation type
			/// \param shininess -  the val
			void SetLightAttenuation( int nLightUnit, GLenum type, GLfloat val );

			/// SetLightSpotDirection - Sets the light spot exponent
			/// \param nLightUnit - which light unit to set
			/// \param dir -  the spot direction
			void SetLightSpotDirection( int nLightUnit, const math::Vec3& dir );
			/// SetLightSpotExponent - Sets the light spot exponent
			/// \param nLightUnit - which light unit to set
			/// \param val -  the exponent
			void SetLightSpotExponent( int nLightUnit, GLfloat val );
			/// SetLightSpotCutoff - Sets the light spot cut off
			/// \param nLightUnit - which light unit to set
			/// \param val -  the cut off angle
			void SetLightSpotCutoff( int nLightUnit, GLfloat val );

			/// SetColour - Sets the current vertex colour
			/// \param r -  red component
			/// \param g -  green component
			/// \param b -  blue component
			/// \param a -  alpha component
			void SetColour4f( GLfloat r, GLfloat g, GLfloat b, GLfloat a );
			/// SetColour - Sets the current vertex colour
			/// \param r -  red component
			/// \param g -  green component
			/// \param b -  blue component
			/// \param a -  alpha component
			void SetColour4ub( GLubyte r, GLubyte g, GLubyte b, GLubyte a );
			/// SetColour - Sets the current vertex colour
			/// \param vColour -  the colour
			//void SetColour( math::Vec4& vColour );

			/// SetGLContext - attaches an external context to the local class
			/// \param pContext - context to attach to the class
			void SetGLContext( void* pContext );
			/// GetGLProperties - Goes through some standard glGet*() commands and prints their values
			void GetGLProperties( void );
			/// GoFullScreen - Requests to go into fullscreen mode
			void GoFullScreen();
			/// ClearScreen - Does a standard clear of the screen
			void ClearScreen( void );
			/// ClearColour - Sets the clear colour of the screen
			/// \param fRed - red clear colour
			/// \param fGreen - green clear colour
			/// \param fBlue - blue clear colour
			/// \param fAlpha - alpha clear colour
			void ClearColour( GLfloat fRed, GLfloat fGreen, GLfloat fBlue, GLfloat fAlpha );
			/// SetClearBits - Sets the bitfield for the glClear call
			/// \param bits - clear bits
			void SetClearBits( GLbitfield bits );
			/// SetupPerspectiveView - Sets the width and height of the GL viewport
			/// \param nWidth - width of the view
			/// \param nHeight - height of the view
			/// \param bForceUpdate - force the reset of the matrix setup
			void SetupPerspectiveView( GLint nWidth, GLint nHeight, bool bForceUpdate = false );
			/// SetupOrthographicView - Sets the width and height of the GL ortho viewport
			/// \param nWidth - width of the view
			/// \param nHeight - height of the view
			/// \param bForceUpdate - force the reset of the matrix setup
			void SetupOrthographicView( GLint nWidth, GLint nHeight, bool bForceUpdate = false );
			/// SetLookAt - same as gluLookAt but assumes Y is up and stores the values and extracts the frustum for the view
			/// \param fEyeX - eye look at x position
			/// \param fEyeY - eye look at y position
			/// \param fEyeZ - eye look at z position
			/// \param fCenterX - eye target x position
			/// \param fCenterY - eye target y position
			/// \param fCenterZ - eye target z position
			void SetLookAt( GLfloat fEyeX, GLfloat fEyeY, GLfloat fEyeZ, GLfloat fCenterX, GLfloat fCenterY, GLfloat fCenterZ, GLfloat upX=0.0f, GLfloat upY=1.0f, GLfloat upZ=0.0f ); 
			/// SetNearFarClip - Sets the near and far clip planes
			/// \param fNearClip - near clip value
			/// \param fFarClip - far clip value
			void SetNearFarClip( GLfloat fNearClip, GLfloat fFarClip );
			/// SetFieldOfView - Sets the field of view
			/// \param fAngle - angle to set the view
			void SetFieldOfView( GLfloat fAngle );
			/// SetViewport - Sets the viewport
			/// \param nWidth - width of viewport
			/// \param nHeight - height of viewport
			void SetViewport( GLint nWidth, GLint nHeight );
			/// SetPerspective - Sets the perspective view
			/// \param fFOV - field of view angle
			/// \param fAspectRatio - aspect ratio
			/// \param fNearClip - near clip value
			/// \param fFarClip - far clip value
			void SetPerspective( GLfloat fFOV, GLfloat fAspectRatio, GLfloat fNearClip, GLfloat fFarClip, bool force=false );

			/// SaveDepthBlendAlphaState - Saves the current depth, blend and alpha state to a structure
			/// \param pState - pointer to state storage
			void SaveDepthBlendAlphaState( TGLDepthBlendAlphaState* pState );
			/// SetDepthBlendAlphaState - Sets the depth, blend and alpha state from a structure
			/// \param pState - new state
			void SetDepthBlendAlphaState( TGLDepthBlendAlphaState* pState );
			
			/// SaveFogState - Saves the current fog state to a structure
			/// \param pState - pointer to state storage
			void SaveFogState( TGLFogState* pState );
			/// SetFogState - Sets the fog state from a structure
			/// \param pState - new state
			void SetFogState( TGLFogState* pState );

			/// SaveViewState - Saves the current view state to a structure
			/// \param pState - pointer to view state storage
			void SaveViewState( TViewState* pState );
			/// SetViewState - Sets the a view state from a structure
			/// \param viewState - new view state
			void SetViewState( TViewState* pViewState );

			/// SaveCullState - Saves the current cull state to a structure
			/// \param pState - pointer to view state storage
			void SaveCullState( TGLCullFaceState* pState );
			/// SetCullState - Sets the cull state from a structure
			/// \param cullState - new cull state
			void SetCullState( TGLCullFaceState* pCullState );

			/// SetAmbientColour - Sets the ambient light colour
			/// \param fAmbientRed - red ambient value
			/// \param fAmbientGreen - green ambient value
			/// \param fAmbientBlue - blue ambient value
			/// \param fAmbientAlpha - alpha ambient value
			void SetAmbientColour( GLfloat fAmbientRed, GLfloat fAmbientGreen, GLfloat fAmbientBlue, GLfloat fAmbientAlpha );

			/// SetAmbientFlag - Sets the ambient light flag
			/// \param bAmbient - flag to enable/disable the ambient light
			void SetAmbientFlag( bool bAmbient )	{ m_AmbientLighting = bAmbient; }

			/// SetMaxTextureSize - Sets the max texture units
			/// \param nMaxTextureUnits - max texture units
			void SetMaxTextureUnits( GLint nMaxTextureUnits )				{ m_MaxTextureUnits = nMaxTextureUnits; }
			/// GetMaxTextureSize - Gets the current max texture units
			/// \return integer - returned max texture units
			GLuint GetMaxTextureUnits( void )								{ return m_MaxTextureUnits; }

			/// SetMaxTextureSize - Sets the max texture size
			/// \param nMaxTextureSize - max texture size
			void SetMaxTextureSize( GLint nMaxTextureSize )				{ m_MaxTextureSize = nMaxTextureSize; }
			/// GetMaxTextureSize - Gets the current max texture size
			/// \return integer - returned max texture size
			GLint GetMaxTextureSize( void )								{ return m_MaxTextureSize; }

			/// SetMaxRenderBufferSize - If supported the max render buffer size gets set
			/// \param nMaxRenderBufferSize - max render buffer size
			void SetMaxRenderBufferSize( GLint nMaxRenderBufferSize )		{ m_MaxRenderBufferSize = nMaxRenderBufferSize; }
			/// GetMaxRenderBufferSize - Gets the current max allowed render buffer size
			/// \return integer - max size
			GLint GetMaxRenderBufferSize( void )							{ return m_MaxRenderBufferSize; }

			/// SetRotated - Should the viewport be rotated
			/// \param bRotate - rotation flag
			void SetRotated( bool bRotate )                             { m_Rotate = bRotate; }
			/// GetIsRotated - Find out if the view is just rotated
			/// \return boolean - returns the rotation flag
			bool GetIsRotated()                                         { return(m_Rotate); }

            /// SetRotationStyle - which rotation
            /// \param bRotate - rotation flag
            void SetRotationStyle( renderer::EViewRotation style );
            /// GetRotationStyle - Which rotation is currently active
            /// \return EViewRotation - returns the rotation style
            EViewRotation GetRotationStyle()                            { return(m_RotationStyle); }

			/// SetMirrored - Should the viewport be mirrored
			/// \param bMirror - mirror flag
			void SetMirrored( bool bMirror )                             { m_Mirror = bMirror; }
			/// GetIsMirrored - Find out if the view is mirrored
			/// \return boolean - returns the mirror flag
			bool GetIsMirrored()                                         { return(m_Mirror); }

            /// SetRotationStyle - which mirroring
            /// \param style - mirror flags
            void SetMirrorStyle( renderer::EViewMirror style );
            /// GetMirrorStyle - Which mirror mode is currently active
            /// \return EViewMirror - returns the mirror style
            EViewMirror GetMirrorStyle()								{ return(m_MirrorStyle); }

			/// SetClipPlane
			/// \param plane - plane id
			/// \param eq - plane equation
			void SetClipPlane( GLenum plane, const math::Vec4& eq );

			/// Flip - Swaps the backbuffer with the frontbuffer
			void Flip( void );

			// Frustum calls
			/// ExtractFrustum - This grabs the current modelview and projection and calculates the frustum box
			void ExtractFrustum( void );
			/// PointInFrustum - Checks to see if a point is within the current frustum
			/// \param X - x position of the point
			/// \param Y - y position of the point
			/// \param Z - z position of the point
			/// \return boolean ( SUCCESS:true or FAIL:false )
			bool PointInFrustum( GLfloat X, GLfloat Y, GLfloat Z );
			/// SphereInFrustum - Checks to see if a sphere is within the current frustum
			/// \param X - center x point of the sphere
			/// \param Y - center y point of the sphere
			/// \param Z - center z point of the sphere
			/// \param fRadius - radius of the sphere
			/// \return boolean ( SUCCESS:true or FAIL:false )
			bool SphereInFrustum( GLfloat X, GLfloat Y, GLfloat Z, GLfloat fRadius );
			/// SphereDistanceFromFrustum - Checks to see if a sphere is within the current frustum and how far it is from the near clip plane
			/// \param X - center x point of the sphere
			/// \param Y - center y point of the sphere
			/// \param Z - center z point of the sphere
			/// \param fRadius - radius of the sphere
			/// \return GLfloat - distance from the eye position of the frustum
			GLfloat SphereDistanceFromFrustum( GLfloat X, GLfloat Y, GLfloat Z, GLfloat fRadius );
			/// CubeInFrustum - Checks to see if a cube is within the current frustum
			/// \param X - center x point of the cube
			/// \param Y - center y point of the cube
			/// \param Z - center z point of the cube
			/// \param fSize - size of the cube
			/// \return boolean ( SUCCESS:true or FAIL:false )
			bool CubeInFrustum( GLfloat X, GLfloat Y, GLfloat Z, GLfloat fSize );
			/// AABBInFrustum - Checks to see if an AABB is within the current frustum
			/// \param aabb - Axis aligned bounding box to check
			/// \return boolean ( SUCCESS:true or FAIL:false )
			bool AABBInFrustum( const collision::AABB &aabb );

			GLuint GetCurrentProgram()			{ return m_GLState.nCurrentProgram; }

			/// GetTextureState - Gets the current texturing enabled/disabled state
			/// \return boolean - ( true or false )
			bool GetTextureState( void )		{ return m_GLState.bTextureState; }
			/// GetLightingState - Gets the current lighting enabled/disabled state
			/// \return boolean - ( true or false )
			bool GetLightingState( void )		{ return m_GLState.bLightingState; }
			/// GetDepthMode - Gets the current depth test state 
			/// \param bDepthFlag - depth test flag holder
			/// \param eDepthFunc - depth test function holder
			void GetDepthMode( bool *bDepthFlag, GLenum *eDepthFunc );
			/// GetBlendMode - Gets the current blend test state
			/// \param bBlendFlag - blend flag holder
			/// \param eBlendSrc - source blend function holder
			/// \param eBlendDest - destination blend function holder
			void GetBlendMode( bool *bBlendFlag, GLenum *eBlendSrc, GLenum *eBlendDest );
			/// GetAlphaMode - Gets the current alpha test state
			/// \param bAlphaFlag - alpha flag holder
			/// \param eAlphaFunc - alpha test function holder
			/// \param fAlphaClamp - alpha test clamp holder
			void GetAlphaMode( bool *bAlphaFlag, GLenum *eAlphaFunc, GLfloat *fAlphaClamp );
			/// GetFillMode - Gets the current fillmode state
			/// \return boolean - ( true or false )
			bool GetFillMode( void )		{ return m_GLState.bFillMode; }
			/// GetCullState - Gets the current cull state
			/// \return boolean - ( true or false )
			bool GetCullState( void )		{ return m_GLState.bCullState; }
			/// GetCullFace - Gets the current cull face
			/// \return GLenum - current culling face (should be either GL_FRONT, GL_BACK or GL_FRONT_AND_BACK)
			GLenum GetCullFace( void )		{ return m_GLState.eCullFace; }
			/// GetWidthHeight - Gets the width and height of the current view
			/// \param nWidth - parameter to hold the returned width
			/// \param nHeight - parameter to hold the returned height
			void GetWidthHeight( GLint *nWidth, GLint *nHeight );
			/// GetClearColour - Gets the current GL clear screen colour
			/// \param fRed - parameter to hold the returned red value
			/// \param fGreen - parameter to hold the returned green value
			/// \param fBlue - parameter to hold the returned blue value
			void GetClearColour( GLfloat *fRed, GLfloat *fGreen, GLfloat *fBlue );
			/// GetClearBits - Gets the glClear mask bits
			GLbitfield GetClearBits( )		{ return m_ClearBits; }
			/// GetLookAt - Gets the current eye details
			/// \param vEye - parameter to hold the returned eye position
			/// \param vCenter - parameter to hold the returned eye target
			void GetLookAt( math::Vec3 &vEye, math::Vec3 &vCenter );
			/// GetFOVNearFarClip - Returns the current field of view, near and far clip values
			/// \param fFOV - parameter to hold the returned field of view value
			/// \param fNearClip - parameter to hold the returned near clip value
			/// \param fFarClip - parameter to hold the returned far clip value
			void GetFOVNearFarClip( GLfloat *fFOV, GLfloat *fNearClip, GLfloat *fFarClip );
			/// GetActiveTexture - Gets the current active texture on a unit
			/// \param nTexUnit - unit to grab current texture from
			/// \param eTarget - texture target
			/// \return GLuint - ( SUCCESS: GL texture id or FAIL: INVALID_OBJECT
			GLuint GetActiveTexture( GLint nTexUnit, GLenum eTarget );
			/// GetActiveFrameBuffer - Gets the current active texture on a unit
			/// \return GLuint - ( SUCCESS: GL framebuffer id or FAIL: INVALID_OBJECT
			GLuint GetActiveFrameBuffer( void );
			/// GetAmbientColour - Gets the current ambient light colour
			/// \param fAmbientRed - parameter to hold the returned red value
			/// \param fAmbientGreen - parameter to hold the returned green value
			/// \param fAmbientBlue - parameter to hold the returned blue value
			void GetAmbientColour( GLfloat *fAmbientRed, GLfloat *fAmbientGreen, GLfloat *fAmbientBlue );
			/// GetColour4ub - Gets the current colour
			/// \return math::Vec4Lite - the vertex colour currently active
			const math::Vec4Lite& GetColour4ub()	{ return m_GLState.vColour4ub; }
	
			/// GetAmbientFlag - Returns whether ambient lighting is enables/disabled
			/// \return boolean - ( SUCCESS:true or FAIL:false )
			bool GetAmbientFlag( void ) 	{ return(m_AmbientLighting); }

			/// GetFontID - Returns the built in debug font, GL texture id
			/// \return unsigned integer - (SUCCESS: GL texture id or FAIL: INVALID_OBJECT)
			GLuint GetFontID()						{ return m_DebugTextureID; }
			/// GetPhysicsTexID - Returns the built in debug physics texture, GL texture id
			/// \return unsigned integer - (SUCCESS: GL texture id or FAIL: INVALID_OBJECT)
			GLuint GetPhysicsTexID()				{ return m_DebugPhysicsTextureID; }
			/// GetTotalTrianglesDrawn - Gets the total amount of triangles drawn since the last flip
			/// \return integer - (SUCCESS: number of triangles drawn since last flip)
			GLint GetTotalTrianglesDrawn( void )	{ return nTotalTriangleCount; }
			/// GetTotalVerticesDrawn - Gets the total amount of vertices drawn since the last flip
			/// \return integer - (SUCCESS: number of vertices drawn since last flip)
			GLint GetTotalVerticesDrawn( void )		{ return nTotalVertexCount; }

			/// GetViewport - Gets the last known viewport extends
			GLint* GetViewport()					{ return (&m_Viewport[0]); }

			/// SetProjectionMatrix - Set the projection matrix
			/// \param m - matrix to set
			void SetProjectionMatrix( glm::mat4& m )	{ m_ProjMatrix = m; }
			/// GetProjectionMatrix - Gets the last known projection matrix
			glm::mat4& GetProjectionMatrix()			{ return (m_ProjMatrix); }

			/// SetViewMatrix - Sets the view matrix
			/// \param m - matrix to set
			void SetViewMatrix( glm::mat4& m )		{ m_ViewMatrix = m; }
			/// GetProjectionMatrix - Gets the last known view matrix
			glm::mat4& GetViewMatrix()				{ return (m_ViewMatrix); }

			/// SetModelMatrix - Sets the model matrix
			/// \param m - matrix to set
			void SetModelMatrix(glm::mat4& m)		{ m_ModlMatrix = m; }
			/// GetModelMatrix - Gets the last known model  matrix
			glm::mat4& GetModelMatrix()				{ return (m_ModlMatrix); }

			/// GetClipMatrix - Gets the last known clip matrix
			glm::mat4& GetClipMatrix()				{ return (m_ClipMatrix); }

			/// Tool preference structure holder
			//TToolPreferences toolPrefs;
			/// Current total triangle count
			GLint nTotalTriangleCount;
			/// Current total vertex count
			GLint nTotalVertexCount;
			/// Current total material count
			GLint nTotalMaterialCount;
			/// Current total texture count
			GLint nTotalTextureCount;

			static void Initialise( void );
			static void Shutdown( void );

			static OpenGL *GetInstance( void ) 
			{
				DBG_ASSERT( (ms_Instance != 0) );

				return( ms_Instance );
			}
			static bool IsInitialised( void ) 
			{
				return( ms_Instance != 0 );
			}

		private:
			static OpenGL* ms_Instance;

#ifdef BASE_PLATFORM_WINDOWS
			/// Win32 device context handle
			HDC			m_hDC;
			/// Win32 GL render context handle
			HGLRC		m_hGLRC;
			/// Win32 device mode structure
			DEVMODE		m_dmScreen;
#endif // BASE_PLATFORM_WINDOWS
	
#ifdef BASE_PLATFORM_RASPBERRYPI
			EGLDisplay m_EGLDisplay;
			EGLSurface m_EGLSurface;
			EGLContext m_EGLContext;
#endif // BASE_PLATFORM_RASPBERRYPI	

			/// State tracking structure
			TGLState	m_GLState;

			/// Current Pixel Format value
			GLint		m_PixelFormat;
			/// Window width value
			GLint		m_Width;
			/// Window height value
			GLint		m_Height;
			/// GL Bits per Pixel value
			GLint	m_BPP;
			/// GL Z buffer value
			GLint	m_ZBuffer;
			/// GL alpha bits value
			GLint	m_AlphaBits;
			/// GL depth bits value
			GLint	m_DepthBits;
			/// do rotation from portait to landscape
			bool	m_Rotate;
            /// rotation style
            EViewRotation m_RotationStyle;
            /// rotation angle
            float m_RotationAngle;
			/// viewport mirror
			bool	m_Mirror;
            /// mirror style
            EViewMirror m_MirrorStyle;
			/// mirror values
			glm::vec3 m_MirrorScales;

			/// last viewport
			GLint m_Viewport[4];

			/// projection matrix
			glm::mat4	m_ProjMatrix;
			/// view matrix
			glm::mat4	m_ViewMatrix;
			/// modelview matrix
			glm::mat4	m_ModlMatrix;
			/// clip matrix
			glm::mat4	m_ClipMatrix;

			/// GL clear colour
			math::Vec4	m_vClearColour;
			/// Clear bits
			GLbitfield m_ClearBits;
			/// GL view type
			EViewState	m_eViewState;
			/// GL field of view
			GLfloat	m_FOV;
			/// GL aspect ratio
			GLfloat m_AspectRatio;
			/// GL near clip
			GLfloat	m_NearClip;
			/// GL far clip
			GLfloat	m_FarClip;
			/// GL frustum values, automatically filled each frame
			GLfloat	m_Frustum[6][4];
			/// GL current look at eye vector
			math::Vec3 m_vLookAtEye;
			/// GL current look at center vector
			math::Vec3 m_vLookAtCenter;

			/// GL ambient light enable/disable flag
			bool	m_AmbientLighting;
			/// GL ambient light colour
			math::Vec4	m_vAmbientColour;

			/// Max texture size supported by the driver
			GLint m_MaxTextureSize;
			/// Max texture units supported by driver
			GLint m_MaxTextureUnits;
			/// Max render buffer size supported by driver
			GLint m_MaxRenderBufferSize;

			/// Debug font texture object
			GLuint m_DebugTextureID;
			/// Debug font texture object
			GLuint m_DebugPhysicsTextureID;
	};

/////////////////////////////////////////////////////
/// Method: BindTexture
/// Params: [in]nTexID
///
/////////////////////////////////////////////////////
inline void OpenGL::BindTexture( GLuint nTexID )
{
	if( nTexID == INVALID_OBJECT )
		return;

	BindUnitTexture( 0, GL_TEXTURE_2D, nTexID );

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: BindUnitTexture
/// Params: None
///
/////////////////////////////////////////////////////
inline void OpenGL::BindUnitTexture( GLint nTexUnit, GLenum eTarget, GLuint nTexID, bool bForceBind )
{
	if( nTexID == INVALID_OBJECT )
		return;

	// make sure it's a valid unit
	if( nTexUnit >= 0 /*&& nTexUnit < m_MaxTextureUnits*/ )
	{
		// which unit are we working on
		GLenum eActiveTexUnit = GL_TEXTURE0 + nTexUnit;

		if( eTarget == GL_TEXTURE_2D )
		{					
			glActiveTexture( eActiveTexUnit );

			if( bForceBind )
			{
				glBindTexture( GL_TEXTURE_2D, nTexID );
				m_GLState.TexUnits[nTexUnit].nCurrent2DTexture = nTexID;

				if( nTexUnit == 0 )
					m_GLState.nCurrentTexture = nTexID;
			}
			else
			{
//				if( m_GLState.TexUnits[nTexUnit].nCurrent2DTexture != nTexID )
				{
					glBindTexture( GL_TEXTURE_2D, nTexID );
					m_GLState.TexUnits[nTexUnit].nCurrent2DTexture = nTexID;

					if( nTexUnit == 0 )
						m_GLState.nCurrentTexture = nTexID;
				}
			}

		}
		else if( eTarget == GL_TEXTURE_CUBE_MAP )
		{					
			glActiveTexture( eActiveTexUnit );

			if( bForceBind )
			{
				glBindTexture( GL_TEXTURE_CUBE_MAP, nTexID );
				m_GLState.TexUnits[nTexUnit].nCurrentCubemapTexture = nTexID;

				if( nTexUnit == 0 )
					m_GLState.nCurrentTexture = nTexID;
			}
			else
			{
//					if( m_GLState.TexUnits[nTexUnit].nCurrentCubemapTexture != nTexID )
				{	
					glBindTexture( GL_TEXTURE_CUBE_MAP, nTexID );
					m_GLState.TexUnits[nTexUnit].nCurrentCubemapTexture = nTexID;

					if( nTexUnit == 0 )
						m_GLState.nCurrentTexture = nTexID;
				}
			}
		}
	}

	// check for errors
	GL_CHECK
}

} // namespace renderer

#endif // BASE_SUPPORT_OPENGL_GLSL

#endif // __OPENGLSL_H__


