
#ifndef __RENDERTOTEXTURE_H__
#define __RENDERTOTEXTURE_H__

#ifdef BASE_SUPPORT_OPENGL

namespace renderer
{
	const float DEFAULT_RTT_DEPTH		= 0.0f;
	const int	_BL_ = 0; // bottom left
	const int	_BR_ = 1; // bottom right
	const int	_TR_ = 2; // top right
	const int	_TL_ = 3; // top left

	const short RTTFLAG_CREATE_COLOURBUFFER = 0x0001;
	const short RTTFLAG_CREATE_DEPTHBUFFER = 0x0002;
	const short RTTFLAG_CREATE_STENCILBUFFER = 0x0004;

	/// enumerated render to texture types
	enum ERTTMode
	{
		RTTMODE_SIMPLECOPY=0,
		RTTMODE_PBUFFER,
		RTTMODE_FBO
	};

	class RenderToTexture
	{
		public:
			/// default constructor
			RenderToTexture();
			/// default destructor
			~RenderToTexture();

			/// Initialise - Sets up all the members variables for the class
			void Initialise( void );
			/// Release - Cleans up any texture or image memory from the class
			void Release( void );
			/// Create - Creates a GL texture object for receiving renders
			/// \param nTexWidth - texture Width
			/// \param nTexHeight - texture Height
			/// \param flags - FBO creation flags
			/// \param nTexInternalFormat - internal texture format
			/// \param nTexFormat - external texture format
			/// \param nTexType - texture type
			/// \param eTexWrapMode - texture wrapping mode
			/// \param bMipMap - generate mipmaps for the texture
			/// \return integer - (SUCCESS:OK or FAIL:FAIL)
			int Create( int nTexWidth, int nTexHeight, short flags = (renderer::RTTFLAG_CREATE_COLOURBUFFER | renderer::RTTFLAG_CREATE_DEPTHBUFFER), GLenum nTexInternalFormat = GL_RGBA8, GLenum nTexFormat = GL_RGBA, GLenum nTexType = GL_UNSIGNED_BYTE, GLenum eTexWrapMode = GL_CLAMP, bool bMipMap = false );
			/// CreateDepthOnly - Creates a GL texture object for receiving depth only renders
			/// \param nTexWidth - texture Width
			/// \param nTexHeight - texture Height
			/// \return integer - (SUCCESS:OK or FAIL:FAIL)
			int CreateDepthOnly( int nTexWidth, int nTexHeight );		
			/// CreateDepthOnly - Creates a floating point GL texture object
			/// \param nTexWidth - texture Width
			/// \param nTexHeight - texture Height
			/// \return integer - (SUCCESS:OK or FAIL:FAIL)
			int CreateFloat( int nTexWidth, int nTexHeight );	

			/// StartRenderToTexture - Prepares the texture for rendering
			/// \param mask - GL clear bits
			void StartRenderToTexture( GLbitfield mask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
			// EndRenderToTexture - Cleans up the texture after rendering
			void EndRenderToTexture( void );

			void DoCopy();

			/// Bind - Binds the texture
			/// \param nUnit - texture unit to bind the texture to
			void Bind( int nUnit );
			/// Bind - UnBinds the texture
			void UnBind( void );

			/// RenderToScreen - Renders the texture to a full screen quad
			/// \param nTexUnit - texture unit to bind to
			void RenderToScreen( int nTexUnit );

			/// SetClearColour - Sets the clear colour
			/// \param vColour - colour to set
			void SetClearColour( const math::Vec4 &vColour )				{ vClearColour = vColour; }
			/// GetClearColour - Gets the clear colour
			/// \return Vec4 - returned colour
			math::Vec4 GetClearColour( void ) const								{ return vClearColour; }
			/// SetQuadColour - Sets the colour of the full screen quad
			/// \param vNewColour - colour to set
			void SetQuadColour( const math::Vec4 &vNewColour )					{ vColour = vNewColour; }
			/// GetQuadColour - Gets the colour of the full screen quad
			/// \return Vec4 - returned colour
			math::Vec4 GetQuadColour( void ) const								{ return vColour; }
			/// SetFullscreenFilter - Sets the filtering of the full screen textured quad
			/// \param eFilter - filter to set
			void SetFullscreenFilter( const GLenum eFilter )		{ eFullscreenFilter = eFilter; }
			/// GetFullscreenFilter - Gets the filtering of the full screen textured quad
			/// \return GLenum - returned filter
			GLenum GetFullscreenFilter( void ) const				{ return eFullscreenFilter; }
			/// SetWidthHeight - Sets the render to texture, width and height
			/// \param nTexWidth - width of the texture
			/// \param nTexHeight - height of the texture 
			void SetWidthHeight( int nTexWidth, int nTexHeight );
			/// GetFBO - Returns the GL framebuffer object id
			/// \return unsigned int - (SUCCESS: valid texture id FAIL: INVALID_OBJECT)
			GLuint GetFBO( void )			{ return nFramebufferID; }
			/// GetTexture - Returns the GL texture object id
			/// \return unsigned int - (SUCCESS: valid texture id FAIL: INVALID_OBJECT)
			GLuint GetTexture( void )		{ return nID; }
			/// GetTarget - Returns the GL texture target
			/// \return GLenum - GL_TEXTURE_2D or GL_TEXTURE_RECTANGLE_ARB
			GLenum GetTarget( void )		{ return eTarget; }
			/// IsValid - Returns whether the texture is valid to be used
			/// \return integer - (SUCCESS: OK FAIL: FAIL )
			int IsValid( void )		{ return bValidTexture; }

		private:
			/// CreateCopyTex - Creates a GL texture object for receiving renders, but uses glCopyTex only
			/// \param nTexWidth - texture Width
			/// \param nTexHeight - texture Height
			/// \param nTexInternalFormat - internal texture format
			/// \param nTexFormat - external texture format
			/// \param nTexType - texture type
			/// \param eTexWrapMode - texture wrapping mode
			/// \param bMipMap - generate mipmaps for the texture
			/// \return integer - (SUCCESS:OK or FAIL:FAIL)
			int CreateCopyTex( int nTexWidth, int nTexHeight, GLenum nTexInternalFormat = GL_RGBA8, GLenum nTexFormat = GL_RGBA, GLenum nTexType = GL_UNSIGNED_BYTE, GLenum eTexWrapMode = GL_CLAMP, bool bMipMap = false );
			/// CreateCopyTexDepthOnly - Creates a GL texture object for receiving depth only renders, but uses glCopyTex only
			/// \param nTexWidth - texture Width
			/// \param nTexHeight - texture Height
			/// \return integer - (SUCCESS:OK or FAIL:FAIL)
			int CreateCopyTexDepthOnly( int nTexWidth, int nTexHeight );
			/// CreateCopyTexFloat - Creates a GL texture object for a floating point texture, but uses glCopyTex only
			/// \param nTexWidth - texture Width
			/// \param nTexHeight - texture Height
			/// \return integer - (SUCCESS:OK or FAIL:FAIL)
			int CreateCopyTexFloat( int nTexWidth, int nTexHeight );
			/// CreateFBO - Creates a GL frame buffer object for receiving renders, but uses frame buffer objects
			/// \param nTexWidth - texture Width
			/// \param nTexHeight - texture Height
			/// \param nTexInternalFormat - internal texture format
			/// \param nTexFormat - external texture format
			/// \param nTexType - texture type
			/// \param eTexWrapMode - texture wrapping mode
			/// \param bMipMap - generate mipmaps for the texture
			/// \return integer - (SUCCESS:OK or FAIL:FAIL)
			int CreateFBO( int nTexWidth, int nTexHeight,  short flags = (renderer::RTTFLAG_CREATE_COLOURBUFFER | renderer::RTTFLAG_CREATE_DEPTHBUFFER), GLenum nTexInternalFormat = GL_RGBA8, GLenum nTexFormat = GL_RGBA, GLenum nTexType = GL_FLOAT, GLenum eTexWrapMode = GL_CLAMP, bool bMipMap = false );
			/// CreateFBODepthOnly - Creates a GL frame buffer object for receiving depth only renders, using frame buffer objects
			/// \param nTexWidth - texture Width
			/// \param nTexHeight - texture Height
			/// \return integer - (SUCCESS:OK or FAIL:FAIL)
			int CreateFBODepthOnly( int nTexWidth, int nTexHeight );
			/// CreateFBOFloat - Creates a GL frame buffer object for a floating point texture, using frame buffer objects
			/// \param nTexWidth - texture Width
			/// \param nTexHeight - texture Height
			/// \return integer - (SUCCESS:OK or FAIL:FAIL)
			int CreateFBOFloat( int nTexWidth, int nTexHeight);

		private:
			/// Render to Texture mode
			ERTTMode eRTTMode;
			/// GL texture object id
			GLuint nID;
			/// clear colour
			math::Vec4 vClearColour;
			/// full screen colour
			math::Vec4 vColour;
			/// full screen uv coords
			math::Vec2 vUVCoords[4];
			/// GL FBO object id
			GLuint nFramebufferID;
			/// GL FBO depth render buffer object id
			GLuint nDepth_rb;
			/// GL FBO stencil render buffer object id
			GLuint nStencil_rb;
			/// width of the texture
			int nWidth;
			/// height of the texture
			int nHeight;
			/// bits-per-pixel of the texture
			int nBPP;
			/// flags whether the texture contains alpha information
			bool bHasAlpha;
			/// flags whether this FBO contains any mipmaps
			bool bHasMipMaps;
			/// depth only flag
			bool bDepthOnly;
			/// GL target
			GLenum eTarget;
			/// GL internal format
			GLenum eInternalFormat;
			/// GL external format
			GLenum eFormat;
			/// GL target
			GLenum eWrapMode;
			/// GL type
			GLenum eType;
			/// filter when applying to a fullscreen quad
			GLenum eFullscreenFilter;
			/// pointer to the actual image data
			unsigned char *image;
			/// pointer to the depth image data
			unsigned char *depthBuffer;
			/// whether the texture is initialised
			bool bInitialised;
			/// unit the texture is currently bound to
			int nUnit;
			/// flag whether the texture is valid, whether FBO or vanilla texture
			bool bValidTexture;
			/// previous viewport width before rendering to texture
			int nPreviousWidth;
			/// previous viewport height before rendering to texture
			int nPreviousHeight;
			/// if framebuffer drawing is nested, you need to know what framebuffer to switch back to
			GLuint nPreviousBoundFrameBuffer;
	};

} // namespace renderer

#endif // BASE_SUPPORT_OPENGL

#endif // __RENDERTOTEXTURE_H__




