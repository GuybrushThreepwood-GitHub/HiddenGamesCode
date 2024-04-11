
/*===================================================================
	File: OpenGLES.cpp
	Library: Render

	(C)Hidden Games
=====================================================================*/

#ifdef BASE_SUPPORT_OPENGLES

#include "CoreBase.h"

#include <cmath>

#include "Math/Vectors.h"
#include "Collision/AABB.h"

#include "Render/RenderConsts.h"
#include "Render/OpenGLCommon.h"
#include "Render/FF/OpenGLES/OpenGLES.h"
#include "Render/FF/OpenGLES/ExtensionsOES.h"

#include "Render/TextureShared.h"
#include "Render/Texture.h"
#include "Render/FF/OpenGLES/TextureLoadAndUploadOES.h"
#include "Render/FF/Primitives.h"

#if BASE_SUPPORT_FREETYPE
	#include "Render/FreetypeCommon.h"
	#include "Render/FF/FreetypeFont.h"
#endif // BASE_SUPPORT_FREETYPE

using renderer::OpenGL;

OpenGL* OpenGL::ms_Instance = 0;

namespace renderer
{
	const int VENDOR_VENDOR_SIZE = 64;
	const int VENDOR_VERSION_SIZE = 64;
	const int VENDOR_RENDERER_SIZE = 64;
	const int VENDOR_EXTENSIONS_SIZE = 8192;
    
} // namespace renderer

/////////////////////////////////////////////////////
/// Method: Initialise
/// Params: None
///
/////////////////////////////////////////////////////
void OpenGL::Initialise( void )
{
	DBG_ASSERT( (ms_Instance == 0) );

	DBG_MEMTRY
		ms_Instance = new OpenGL;
	DBG_MEMCATCH
}

/////////////////////////////////////////////////////
/// Method: Shutdown
/// Params: None
///
/////////////////////////////////////////////////////
void OpenGL::Shutdown( void )
{
	if( ms_Instance )
	{
		delete ms_Instance;
		ms_Instance = 0;
	}
}

/////////////////////////////////////////////////////
/// Default constructor
/// 
///
/////////////////////////////////////////////////////
OpenGL::OpenGL( )
{
	GLint i = 0;

	// initialise GL state values
	std::memset( &m_GLState, 0, sizeof(TGLState) );
	for( i = 0; i < MAX_USEABLE_TEXUNITS; i++ )
	{
		m_GLState.TexUnits[i].nCurrent1DTexture = INVALID_OBJECT;
		m_GLState.TexUnits[i].nCurrent2DTexture = INVALID_OBJECT;
		m_GLState.TexUnits[i].nCurrentRectangleTexture = INVALID_OBJECT;

		m_GLState.TexUnits[i].nCurrentCubemapTexture = INVALID_OBJECT;
	}
	m_GLState.bVertexArrayState = false;
	m_GLState.bTextureArrayState = false;
	m_GLState.bColourArrayState = false;
	m_GLState.bNormalArrayState = false;

	m_GLState.bTextureState = true;
	m_GLState.nCurrentTexture = INVALID_OBJECT;
	m_GLState.nCurrentFrameBuffer = INVALID_OBJECT;

	m_GLState.bFillMode = true;
	m_GLState.bLightingState = false;

	for( i = 0; i < MAX_USEABLE_LIGHTS; i++ )
	{
		m_GLState.LightUnits[i].bLightState = false;
		m_GLState.LightUnits[i].vAmbientColour	= math::Vec4(0.0f, 0.0f, 0.0f, 1.0f);
		if( i == 0 )
		{
			m_GLState.LightUnits[i].vDiffuseColour	= math::Vec4(1.0f, 1.0f, 1.0f, 1.0f);
			m_GLState.LightUnits[i].vSpecularColour = math::Vec4(1.0f, 1.0f, 1.0f, 1.0f);
		}
		else
		{
			m_GLState.LightUnits[i].vDiffuseColour	= math::Vec4(0.0f, 0.0f, 0.0f, 1.0f);
			m_GLState.LightUnits[i].vSpecularColour = math::Vec4(0.0f, 0.0f, 0.0f, 1.0f);
		}
		m_GLState.LightUnits[i].vEmissionColour	= math::Vec4(0.0f, 0.0f, 0.0f, 1.0f);
		
		m_GLState.LightUnits[i].vPosition		= math::Vec4(0.0f, 0.0f, 1.0f, 0.0f);
		m_GLState.LightUnits[i].vSpotDirection	= math::Vec3(0.0f, 0.0f, -1.0f);
		m_GLState.LightUnits[i].fSpotExponent	= 0.0f;
		m_GLState.LightUnits[i].fSpotCutOff		= 180.0f;
		m_GLState.LightUnits[i].vAttenuation	= math::Vec3(1.0f, 0.0f, 0.0f);
	}

	m_GLState.bDepthState	= true;
	m_GLState.eDepthFunc	= GL_LESS;
	m_GLState.bBlendState	= false;
	m_GLState.eBlendSrc		= GL_SRC_ALPHA;
	m_GLState.eBlendDest	= GL_ONE_MINUS_SRC_ALPHA;
	m_GLState.bAlphaState	= false;
	m_GLState.eAlphaFunc	= GL_ALWAYS;
	m_GLState.fAlphaClamp	= 0.0f;

	m_GLState.bCullState = true;
	m_GLState.eCullFace = GL_BACK;
	m_GLState.eFrontFace = GL_CCW;

	m_GLState.bFogState = false;
	m_GLState.eFogMode = GL_EXP;
	m_GLState.vFogColour = math::Vec3( 0.0f, 0.0f, 0.0f );
	m_GLState.fFogNearClip = 0.0f;
	m_GLState.fFogFarClip = 1.0f;
	m_GLState.fFogDensity = 1.0f;

	m_GLState.bColourMaterialState = false;
	m_GLState.vMaterialAmbient = math::Vec4( 0.2f, 0.2f, 0.2f, 1.0f );
	m_GLState.vMaterialDiffuse = math::Vec4( 0.8f, 0.8f, 0.8f, 1.0f ) ;
	m_GLState.vMaterialSpecular = math::Vec4( 0.0f, 0.0f, 0.0f, 1.0f );
	m_GLState.vMaterialEmission = math::Vec4( 0.0f, 0.0f, 0.0f, 1.0f );
	m_GLState.fMaterialShininess = 0.0f;

	m_GLState.vColour4f = math::Vec4( 0.0f, 0.0f, 0.0f, 1.0f );
	m_GLState.vColour4ub = math::Vec4Lite( 0, 0, 0, 255 );

	m_HasGLVersion1_0 = false;
	m_HasGLVersion1_1 = false;

	m_HasGLVersion2_0 = false;
	m_HasGLVersion2_1 = false;

	m_Width	= 320;
	m_Height	= 480;
	m_BPP		= 32;
	m_ZBuffer	= 24;
	m_AccumBits = 64;
	m_AlphaBits = 8;
	m_Rotate = false;
    m_RotationStyle = VIEWROTATION_PORTRAIT_BUTTON_BOTTOM;
    m_RotationAngle = 0.0f;
	m_Mirror = false;
	m_MirrorStyle = VIEWMIRROR_NONE;
	m_MirrorScales = math::Vec2( 1.0f, 1.0f );

	std::memset( &m_ProjMatrix, 0, sizeof(GLfloat)*16 );
	std::memset( &m_ModlMatrix, 0, sizeof(GLfloat)*16 );
	std::memset( &m_ClipMatrix, 0, sizeof(GLfloat)*16 );

	m_eViewState = VIEWSTATE_PERSPECTIVE;
	m_FOV		= 60.0f;
	m_AspectRatio = 1.0f;
	m_NearClip = 1.0f;
	m_FarClip	= 1000.0f;

	m_vLookAtEye.X = 0.0f;
	m_vLookAtEye.Y = 0.0f;
	m_vLookAtEye.Z = 0.0f;
	m_vLookAtCenter.X = 0.0f;
	m_vLookAtCenter.Y = 0.0f;
	m_vLookAtCenter.Z = 500.0f;

	m_MSAAUsable = false;
	m_MSAAIsActive = false;
	m_MSAASamples = 4;
    
	m_VolumetricFogEnabled = false;

	m_vClearColour = math::Vec4( 0.0f, 0.0f, 0.0f, 1.0f );
	m_ClearBits = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
	m_AmbientLighting = false;

	m_vAmbientColour = math::Vec4( 0.0f, 0.0f, 0.0f, 1.0f );

	// bilinear default texture filter
	m_TextureCompressFlag = false;
	m_eTextureMinFilter = GL_LINEAR;
	m_eTextureMagFilter = GL_LINEAR;

	m_AnisotropyLevel = 1.0f;
	m_MaxAnisotropyLevel = 1.0f;

	m_MaxTextureSize = 1;
	m_Max3DTextureSize = 1;
	m_MaxTextureRectangleSize = 1;
	m_MaxTextureCubemapSize = 1;
	m_MaxRenderBufferSize = 1;

	m_MaxLights = 8;

	m_MaxTextureUnits = 1;

	m_DebugTextureID = INVALID_OBJECT;
	m_DebugPhysicsTextureID = INVALID_OBJECT;

	nTotalTriangleCount = 0;
	nTotalVertexCount = 0;
	nTotalMaterialCount = 0;
	nTotalTextureCount = 0;
}

/////////////////////////////////////////////////////
/// Method: ResetGLState
/// Params: None
///
/////////////////////////////////////////////////////
void OpenGL::ResetGLState()
{
	GLint i = 0;
    
	// initialise GL state values
	std::memset( &m_GLState, 0, sizeof(TGLState) );
	for( i = 0; i < MAX_USEABLE_TEXUNITS; i++ )
	{
		m_GLState.TexUnits[i].nCurrent1DTexture = INVALID_OBJECT;
		m_GLState.TexUnits[i].nCurrent2DTexture = INVALID_OBJECT;
		m_GLState.TexUnits[i].nCurrentRectangleTexture = INVALID_OBJECT;
        
		m_GLState.TexUnits[i].nCurrentCubemapTexture = INVALID_OBJECT;
	}
	m_GLState.bVertexArrayState = false;
	m_GLState.bTextureArrayState = false;
	m_GLState.bColourArrayState = false;
	m_GLState.bNormalArrayState = false;
    
	m_GLState.bTextureState = true;
	m_GLState.nCurrentTexture = INVALID_OBJECT;
	m_GLState.nCurrentFrameBuffer = INVALID_OBJECT;
    
	m_GLState.bFillMode = true;
	m_GLState.bLightingState = false;
    
	for( i = 0; i < MAX_USEABLE_LIGHTS; i++ )
	{
		m_GLState.LightUnits[i].bLightState = false;
		m_GLState.LightUnits[i].vAmbientColour	= math::Vec4(0.0f, 0.0f, 0.0f, 1.0f);
		if( i == 0 )
		{
			m_GLState.LightUnits[i].vDiffuseColour	= math::Vec4(1.0f, 1.0f, 1.0f, 1.0f);
			m_GLState.LightUnits[i].vSpecularColour = math::Vec4(1.0f, 1.0f, 1.0f, 1.0f);
		}
		else
		{
			m_GLState.LightUnits[i].vDiffuseColour	= math::Vec4(0.0f, 0.0f, 0.0f, 1.0f);
			m_GLState.LightUnits[i].vSpecularColour = math::Vec4(0.0f, 0.0f, 0.0f, 1.0f);
		}
		m_GLState.LightUnits[i].vEmissionColour	= math::Vec4(0.0f, 0.0f, 0.0f, 1.0f);
		
		m_GLState.LightUnits[i].vPosition		= math::Vec4(0.0f, 0.0f, 1.0f, 0.0f);
		m_GLState.LightUnits[i].vSpotDirection	= math::Vec3(0.0f, 0.0f, -1.0f);
		m_GLState.LightUnits[i].fSpotExponent	= 0.0f;
		m_GLState.LightUnits[i].fSpotCutOff		= 180.0f;
		m_GLState.LightUnits[i].vAttenuation	= math::Vec3(1.0f, 0.0f, 0.0f);
	}
    
	m_GLState.bDepthState	= true;
	m_GLState.eDepthFunc	= GL_LESS;
	m_GLState.bBlendState	= false;
	m_GLState.eBlendSrc		= GL_SRC_ALPHA;
	m_GLState.eBlendDest	= GL_ONE_MINUS_SRC_ALPHA;
	m_GLState.bAlphaState	= false;
	m_GLState.eAlphaFunc	= GL_ALWAYS;
	m_GLState.fAlphaClamp	= 0.0f;
    
	m_GLState.bCullState = true;
	m_GLState.eCullFace = GL_BACK;
	m_GLState.eFrontFace = GL_CCW;
    
	m_GLState.bFogState = false;
	m_GLState.eFogMode = GL_EXP;
	m_GLState.vFogColour = math::Vec3( 0.0f, 0.0f, 0.0f );
	m_GLState.fFogNearClip = 0.0f;
	m_GLState.fFogFarClip = 1.0f;
	m_GLState.fFogDensity = 1.0f;
    
	m_GLState.bColourMaterialState = false;
	m_GLState.vMaterialAmbient = math::Vec4( 0.2f, 0.2f, 0.2f, 1.0f );
	m_GLState.vMaterialDiffuse = math::Vec4( 0.8f, 0.8f, 0.8f, 1.0f ) ;
	m_GLState.vMaterialSpecular = math::Vec4( 0.0f, 0.0f, 0.0f, 1.0f );
	m_GLState.vMaterialEmission = math::Vec4( 0.0f, 0.0f, 0.0f, 1.0f );
	m_GLState.fMaterialShininess = 0.0f;
    
	m_GLState.vColour4f = math::Vec4( 0.0f, 0.0f, 0.0f, 1.0f );
	m_GLState.vColour4ub = math::Vec4Lite( 0, 0, 0, 255 );
    
	m_HasGLVersion1_0 = false;
	m_HasGLVersion1_1 = false;
    
	m_HasGLVersion2_0 = false;
	m_HasGLVersion2_1 = false;
    
	m_Width	= 320;
	m_Height	= 480;
	m_BPP		= 32;
	m_ZBuffer	= 24;
	m_AccumBits = 64;
	m_AlphaBits = 8;
	m_Rotate = false;
    m_RotationStyle = VIEWROTATION_PORTRAIT_BUTTON_BOTTOM;
    m_RotationAngle = 0.0f;
	m_Mirror = false;
	m_MirrorStyle = VIEWMIRROR_NONE;
	m_MirrorScales = math::Vec2( 1.0f, 1.0f );
    
	std::memset( &m_ProjMatrix, 0, sizeof(GLfloat)*16 );
	std::memset( &m_ModlMatrix, 0, sizeof(GLfloat)*16 );
	std::memset( &m_ClipMatrix, 0, sizeof(GLfloat)*16 );
    
	m_eViewState = VIEWSTATE_PERSPECTIVE;
	m_FOV		= 60.0f;
	m_AspectRatio = 1.0f;
	m_NearClip = 1.0f;
	m_FarClip	= 1000.0f;
    
	m_vLookAtEye.X = 0.0f;
	m_vLookAtEye.Y = 0.0f;
	m_vLookAtEye.Z = 0.0f;
	m_vLookAtCenter.X = 0.0f;
	m_vLookAtCenter.Y = 0.0f;
	m_vLookAtCenter.Z = 500.0f;
    
	m_MSAAUsable = false;
	m_MSAAIsActive = false;
	m_MSAASamples = 4;
    
	m_VolumetricFogEnabled = false;
    
	m_vClearColour = math::Vec4( 0.0f, 0.0f, 0.0f, 1.0f );
	m_ClearBits = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
	m_AmbientLighting = false;
    
	m_vAmbientColour = math::Vec4( 0.0f, 0.0f, 0.0f, 1.0f );
    
	// bilinear default texture filter
	m_TextureCompressFlag = false;
	m_eTextureMinFilter = GL_LINEAR;
	m_eTextureMagFilter = GL_LINEAR;
    
////
	glHint(	GL_FOG_HINT, GL_NICEST );
	glHint(	GL_GENERATE_MIPMAP_HINT, GL_NICEST );
	glHint(	GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
	glHint(	GL_LINE_SMOOTH_HINT, GL_FASTEST );
	glHint(	GL_POINT_SMOOTH_HINT, GL_FASTEST );
    
	// now do some simple OpenGL initialisation
	glClearColor( m_vClearColour.R, m_vClearColour.G, m_vClearColour.B, m_vClearColour.A );
	//SetClearBits( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glShadeModel(GL_SMOOTH);
	glClearDepthf(1.0f);
    
	//SetRotated( false );
	//SetMirrored( false );
    
	glEnable( GL_CULL_FACE );
	glCullFace( GL_BACK );
	//SetCullState( true, GL_BACK );
	glFrontFace(GL_CCW);
    
	glEnable( GL_DEPTH_TEST );
	glDisable( GL_FOG );
    
	// some defaults
	EnableTexturing();
	glActiveTexture( GL_TEXTURE0 );
	glEnable(GL_TEXTURE_2D);
	glBindTexture( GL_TEXTURE_2D, 0 );
    
	//SetNearFarClip( 1.0f, 10000.0f );
	//SetFieldOfView( 60.0f );
    
	EnableVertexArray();
    
	glDepthMask( GL_TRUE );
	glDepthFunc( GL_LESS );
	//DepthMode( true, GL_LESS );
    
	glDisable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	//BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    
	glDisable( GL_ALPHA_TEST );
	glAlphaFunc( GL_ALWAYS, 0.0f );
	//AlphaMode( false, GL_ALWAYS, 0.0f );
    
	//SetAmbientFlag( false );
	//SetAmbientColour( 1.0f, 1.0f, 1.0f, 1.0f );
    
	math::Vec3 fogColour = math::Vec3( 0.0f, 0.0f, 0.0f );
	//FogMode( false, GL_EXP, fogColour, 0.0f, 1.0f, 1.0f );
    
	glDisable( GL_COLOR_MATERIAL );
	
	m_GLState.bColourMaterialState = false;
	m_GLState.vMaterialAmbient = math::Vec4( 0.2f, 0.2f, 0.2f, 1.0f );
	m_GLState.vMaterialDiffuse = math::Vec4( 0.8f, 0.8f, 0.8f, 1.0f ) ;
	m_GLState.vMaterialSpecular = math::Vec4( 0.0f, 0.0f, 0.0f, 1.0f );
	m_GLState.vMaterialEmission = math::Vec4( 0.0f, 0.0f, 0.0f, 1.0f );
	m_GLState.fMaterialShininess = 0.0f;
	
	DisableColourMaterial();
	SetMaterialAmbient( m_GLState.vMaterialAmbient );
	SetMaterialDiffuse( m_GLState.vMaterialDiffuse );
	SetMaterialSpecular( m_GLState.vMaterialSpecular );
	SetMaterialEmission( m_GLState.vMaterialEmission );
	SetMaterialShininess( m_GLState.fMaterialShininess );
    
	// light state
	glDisable( GL_LIGHTING );
	for( i = 0; i < MAX_USEABLE_LIGHTS; i++ )
	{
		glDisable( GL_LIGHT0 + i );
		
		glLightfv( GL_LIGHT0 + i, GL_AMBIENT, (GLfloat *)&m_GLState.LightUnits[i].vAmbientColour );
		glLightfv( GL_LIGHT0 + i, GL_DIFFUSE, (GLfloat *)&m_GLState.LightUnits[i].vDiffuseColour );
		glLightfv( GL_LIGHT0 + i, GL_SPECULAR, (GLfloat *)&m_GLState.LightUnits[i].vSpecularColour );
		
		glLightfv( GL_LIGHT0 + i, GL_POSITION, (GLfloat *)&m_GLState.LightUnits[i].vPosition );
		glLightfv( GL_LIGHT0 + i, GL_SPOT_DIRECTION, (GLfloat *)&m_GLState.LightUnits[i].vSpotDirection );
		glLightf( GL_LIGHT0 + i, GL_SPOT_EXPONENT, m_GLState.LightUnits[i].fSpotExponent );
		glLightf( GL_LIGHT0 + i, GL_SPOT_CUTOFF, m_GLState.LightUnits[i].fSpotCutOff );
		glLightf( GL_LIGHT0 + i, GL_CONSTANT_ATTENUATION, m_GLState.LightUnits[i].vAttenuation[0] );
		glLightf( GL_LIGHT0 + i, GL_LINEAR_ATTENUATION, m_GLState.LightUnits[i].vAttenuation[1] );
		glLightf( GL_LIGHT0 + i, GL_QUADRATIC_ATTENUATION, m_GLState.LightUnits[i].vAttenuation[2] );
	}
	
	// if the extension for separate colour is avaiable use it to stop oversaturated specular from the lights
	EnableSeparateSpecular();
}

/////////////////////////////////////////////////////
/// Default destructor
/// 
///
/////////////////////////////////////////////////////
OpenGL::~OpenGL( )
{
	// first do any cleanups of objects created by this class
	//gDebugGLFont.DestroyFont();

	Release();
}

#ifdef BASE_PLATFORM_RASPBERRYPI

/////////////////////////////////////////////////////
/// Method: SetupDisplay
/// Params: [in]width, [in]height
///
/////////////////////////////////////////////////////
void OpenGL::SetupDisplay( int width, int height )
{
	int32_t success = 0;
	EGLBoolean result;
	EGLint num_config;

	m_Width = width;
	m_Height = height;

	static EGL_DISPMANX_WINDOW_T nativewindow;

	DISPMANX_ELEMENT_HANDLE_T dispman_element;
	DISPMANX_DISPLAY_HANDLE_T dispman_display;
	DISPMANX_UPDATE_HANDLE_T dispman_update;
	VC_DISPMANX_ALPHA_T dispman_alpha;

	VC_RECT_T dst_rect;
	VC_RECT_T src_rect;

	static const EGLint attribute_list[] =
	{
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_ALPHA_SIZE, 8,
		EGL_DEPTH_SIZE, 16,
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_NONE
	};
   
	EGLConfig config;
	
	uint32_t currentScreenWidth;
	uint32_t currentScreenHeight;
			
	// get an EGL display connection
	m_EGLDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	DBG_ASSERT(m_EGLDisplay!=EGL_NO_DISPLAY);

	// initialize the EGL display connection
	result = eglInitialize(m_EGLDisplay, 0, 0);
	assert(EGL_FALSE != result);

	// get an appropriate EGL frame buffer configuration
	//result = eglChooseConfig(m_EGLDisplay, attribute_list, &config, 1, &num_config);
	result = eglSaneChooseConfigBRCM(m_EGLDisplay, attribute_list, &config, 1, &num_config);
	DBG_ASSERT(EGL_FALSE != result);

	result = eglBindAPI(EGL_OPENGL_ES_API);
	DBG_ASSERT(EGL_FALSE != result);

	// create an EGL rendering context
	m_EGLContext = eglCreateContext( m_EGLDisplay, config, EGL_NO_CONTEXT, 0);
	DBG_ASSERT(m_EGLContext!=EGL_NO_CONTEXT);

	// create an EGL window surface
	success = graphics_get_display_size(0 /* LCD */, &currentScreenWidth, &currentScreenHeight);
	DBG_ASSERT( success >= 0 );
	
	// the res to render to
	dst_rect.x = 0;
	dst_rect.y = 0;
	dst_rect.width = currentScreenWidth;
	dst_rect.height = currentScreenHeight;

	// the current screen
	src_rect.x = 0;
	src_rect.y = 0;
	src_rect.width = currentScreenWidth << 16;
	src_rect.height = currentScreenHeight << 16;        

	dispman_display = vc_dispmanx_display_open( 0 /* LCD */);
	dispman_update = vc_dispmanx_update_start( 0 );
    
	dispman_alpha.flags = DISPMANX_FLAGS_ALPHA_FIXED_ALL_PIXELS;
	dispman_alpha.opacity = 0xFF;
	dispman_alpha.mask = (DISPMANX_DISPLAY_HANDLE_T)0; 

	dispman_element = vc_dispmanx_element_add ( dispman_update, dispman_display,
      0/*layer*/, &dst_rect, 0/*src*/,
      &src_rect, DISPMANX_PROTECTION_NONE, &dispman_alpha, 0/*clamp*/, DISPMANX_NO_ROTATE/*transform*/);
      
	nativewindow.element = dispman_element;
	nativewindow.width = currentScreenWidth;
	nativewindow.height = currentScreenHeight;
	vc_dispmanx_update_submit_sync( dispman_update );
      
	m_EGLSurface = eglCreateWindowSurface( m_EGLDisplay, config, &nativewindow, 0 );
	DBG_ASSERT(m_EGLSurface != EGL_NO_SURFACE);

	// connect the context to the surface
	result = eglMakeCurrent(m_EGLDisplay, m_EGLSurface, m_EGLSurface, m_EGLContext);
	DBG_ASSERT(EGL_FALSE != result);
	
	bool vsync = true;
    result = eglSwapInterval(m_EGLDisplay, vsync);
	DBG_ASSERT(EGL_FALSE != result);

	m_Width = static_cast<int>(currentScreenWidth);
	m_Height = static_cast<int>(currentScreenHeight);
	
	DBGLOG( "OPENGLES: native res: %d x %d\n", m_Width, m_Height );
	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}
#endif // BASE_PLATFORM_RASPBERRYPI

/////////////////////////////////////////////////////
/// Method: Init
/// Params: None
///
/////////////////////////////////////////////////////
void OpenGL::Init( void )
{
	GLint i = 0;
	char szRenderer[renderer::VENDOR_RENDERER_SIZE];
	char szVersion[renderer::VENDOR_VERSION_SIZE];
	char szVendor[renderer::VENDOR_VENDOR_SIZE];
	char szExtensions[renderer::VENDOR_EXTENSIONS_SIZE];
	
	//core::WideString renderName( reinterpret_cast<const char*>( glGetString( GL_RENDERER ) ) );
	snprintf( szRenderer, renderer::VENDOR_RENDERER_SIZE, "Renderer: %s\n", glGetString( GL_RENDERER ) );
	//core::WideString versionName( reinterpret_cast<const char*>( glGetString( GL_VERSION ) ) );
	snprintf( szVersion, renderer::VENDOR_VERSION_SIZE, "Version: %s\n", glGetString( GL_VERSION ) );

	//core::WideString vendorName( reinterpret_cast<const char*>( glGetString( GL_VENDOR ) ) );
	snprintf( szVendor, renderer::VENDOR_VENDOR_SIZE, "Vendor: %s\n", glGetString( GL_VENDOR ) );
	//core::WideString extensionsName( reinterpret_cast<const char*>( glGetString( GL_EXTENSIONS ) ) );
	snprintf( szExtensions, renderer::VENDOR_EXTENSIONS_SIZE, "extensions: %s\n", glGetString( GL_EXTENSIONS ) );

	DBGLOG( "\n" );
	DBGLOG( "OpenGL driver info\n" );
	DBGLOG( "--------------------------------------\n" );
	DBGLOG( "%s", szRenderer );
	DBGLOG( "--------------------------------------\n" );
	DBGLOG( "%s", szVersion );
	DBGLOG( "--------------------------------------\n" );
	DBGLOG( "%s", szVendor );
	DBGLOG( "--------------------------------------\n" );
	DBGLOG( "%s", szExtensions );
	DBGLOG( "--------------------------------------\n" );
	DBGLOG( "\n" );

	GetGLProperties();

	// extensions
	RequestExtensions( ALL_EXTENSIONS, this );

	glHint(	GL_FOG_HINT, GL_NICEST );
	glHint(	GL_GENERATE_MIPMAP_HINT, GL_NICEST );
	glHint(	GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
	glHint(	GL_LINE_SMOOTH_HINT, GL_FASTEST );
	glHint(	GL_POINT_SMOOTH_HINT, GL_FASTEST );

	// now do some simple OpenGL initialisation
	glClearColor( m_vClearColour.R, m_vClearColour.G, m_vClearColour.B, m_vClearColour.A );		
	SetClearBits( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glShadeModel(GL_SMOOTH);					
	glClearDepthf(1.0f);

	SetRotated( false );
	SetMirrored( false );

	glEnable( GL_CULL_FACE );
	glCullFace( GL_BACK );
	SetCullState( true, GL_BACK );		
	glFrontFace(GL_CCW);

	glEnable( GL_DEPTH_TEST );
	glDisable( GL_FOG );

	// some defaults
	EnableTexturing();	
	glActiveTexture( GL_TEXTURE0 );
	glEnable(GL_TEXTURE_2D);
	glBindTexture( GL_TEXTURE_2D, 0 );

	SetNearFarClip( 1.0f, 10000.0f );
	SetFieldOfView( 60.0f );

	EnableVertexArray();

	glDepthMask( GL_TRUE );
	glDepthFunc( GL_LESS );
	DepthMode( true, GL_LESS );

	glDisable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	glDisable( GL_ALPHA_TEST );
	glAlphaFunc( GL_ALWAYS, 0.0f );
	AlphaMode( false, GL_ALWAYS, 0.0f );

	SetAmbientFlag( false );
	SetAmbientColour( 1.0f, 1.0f, 1.0f, 1.0f );

	math::Vec3 fogColour = math::Vec3( 0.0f, 0.0f, 0.0f );
	FogMode( false, GL_EXP, fogColour, 0.0f, 1.0f, 1.0f );

	glDisable( GL_COLOR_MATERIAL );
	
	m_GLState.bColourMaterialState = false;
	m_GLState.vMaterialAmbient = math::Vec4( 0.2f, 0.2f, 0.2f, 1.0f );
	m_GLState.vMaterialDiffuse = math::Vec4( 0.8f, 0.8f, 0.8f, 1.0f ) ;
	m_GLState.vMaterialSpecular = math::Vec4( 0.0f, 0.0f, 0.0f, 1.0f );
	m_GLState.vMaterialEmission = math::Vec4( 0.0f, 0.0f, 0.0f, 1.0f );
	m_GLState.fMaterialShininess = 0.0f;
	
	DisableColourMaterial();
	SetMaterialAmbient( m_GLState.vMaterialAmbient );
	SetMaterialDiffuse( m_GLState.vMaterialDiffuse );
	SetMaterialSpecular( m_GLState.vMaterialSpecular );
	SetMaterialEmission( m_GLState.vMaterialEmission );
	SetMaterialShininess( m_GLState.fMaterialShininess );

	// light state
	glDisable( GL_LIGHTING );
	for( i = 0; i < MAX_USEABLE_LIGHTS; i++ )
	{
		glDisable( GL_LIGHT0 + i );
		
		glLightfv( GL_LIGHT0 + i, GL_AMBIENT, (GLfloat *)&m_GLState.LightUnits[i].vAmbientColour );
		glLightfv( GL_LIGHT0 + i, GL_DIFFUSE, (GLfloat *)&m_GLState.LightUnits[i].vDiffuseColour );
		glLightfv( GL_LIGHT0 + i, GL_SPECULAR, (GLfloat *)&m_GLState.LightUnits[i].vSpecularColour );
		
		glLightfv( GL_LIGHT0 + i, GL_POSITION, (GLfloat *)&m_GLState.LightUnits[i].vPosition );
		glLightfv( GL_LIGHT0 + i, GL_SPOT_DIRECTION, (GLfloat *)&m_GLState.LightUnits[i].vSpotDirection );
		glLightf( GL_LIGHT0 + i, GL_SPOT_EXPONENT, m_GLState.LightUnits[i].fSpotExponent );
		glLightf( GL_LIGHT0 + i, GL_SPOT_CUTOFF, m_GLState.LightUnits[i].fSpotCutOff );
		glLightf( GL_LIGHT0 + i, GL_CONSTANT_ATTENUATION, m_GLState.LightUnits[i].vAttenuation[0] );
		glLightf( GL_LIGHT0 + i, GL_LINEAR_ATTENUATION, m_GLState.LightUnits[i].vAttenuation[1] );
		glLightf( GL_LIGHT0 + i, GL_QUADRATIC_ATTENUATION, m_GLState.LightUnits[i].vAttenuation[2] );
	}
	
	// if the extension for separate colour is avaiable use it to stop oversaturated specular from the lights
	EnableSeparateSpecular();

#if BASE_SUPPORT_FREETYPE
	renderer::InitialiseFreetype(this);
#endif // BASE_SUPPORT_FREETYPE

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: Release
/// Params:
///
/////////////////////////////////////////////////////
void OpenGL::Release( void )
{
	DBGLOG( "----- OpenGL Shutdown -----\n" );

#if BASE_SUPPORT_FREETYPE
	renderer::ShutdownFreetype();
#endif // BASE_SUPPORT_FREETYPE

	// first do any cleanups of objects created by this class
	//gDebugGLFont.DestroyFont();

	if( m_DebugTextureID != INVALID_OBJECT )
	{
		renderer::RemoveTexture( m_DebugTextureID );
		m_DebugTextureID = INVALID_OBJECT;
	}

	if( m_DebugPhysicsTextureID != INVALID_OBJECT )
	{
		renderer::RemoveTexture( m_DebugPhysicsTextureID );
		m_DebugPhysicsTextureID = INVALID_OBJECT;
	}

#ifdef BASE_PLATFORM_RASPBERRYPI
	// clear screen
	glClear( GL_COLOR_BUFFER_BIT );
	eglSwapBuffers( m_EGLDisplay, m_EGLSurface );

	// Release OpenGL resources
	eglMakeCurrent( m_EGLDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT );
	eglDestroySurface( m_EGLDisplay, m_EGLSurface );
	eglDestroyContext( m_EGLDisplay, m_EGLContext );
	eglTerminate( m_EGLDisplay );
#endif // BASE_PLATFORM_RASPBERRYPI
   
	DBGLOG( "----- OpenGL Shutdown Complete-----\n" );

}

/////////////////////////////////////////////////////
/// Method: SetGLContext
/// Params: [in]pContext
///
/////////////////////////////////////////////////////
void OpenGL::SetGLContext( void* pContext )
{

#ifdef BASE_PLATFORM_iOS
	//m_pEGLContext = (__bridge EAGLContext *)pContext;
#endif // BASE_PLATFORM_iOS 

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: EnableVertexArrays
/// Params: None
///
/////////////////////////////////////////////////////
void OpenGL::EnableVertexArray( void )
{
	// already on?
	if( m_GLState.bVertexArrayState == true )
		return;

	glEnableClientState( GL_VERTEX_ARRAY );
	m_GLState.bVertexArrayState = true;

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: DisableVertexArray
/// Params: None
///
/////////////////////////////////////////////////////
void OpenGL::DisableVertexArray( void )
{
	// already off?
	if( m_GLState.bVertexArrayState == false )
		return;

	glDisableClientState( GL_VERTEX_ARRAY );
	m_GLState.bVertexArrayState = false;

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: EnableTextureArray
/// Params: None
///
/////////////////////////////////////////////////////
void OpenGL::EnableTextureArray( void )
{
	// already on?
	if( m_GLState.bTextureArrayState == true )
		return;

	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	m_GLState.bTextureArrayState = true;

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: DisableTextureArray
/// Params: None
///
/////////////////////////////////////////////////////
void OpenGL::DisableTextureArray( void )
{
	// already off?
	if( m_GLState.bTextureArrayState == false )
		return;

	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	m_GLState.bTextureArrayState = false;

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: EnableColourArray
/// Params: None
///
/////////////////////////////////////////////////////
void OpenGL::EnableColourArray( void )
{
	// already on?
	if( m_GLState.bColourArrayState == true )
		return;

	glEnableClientState( GL_COLOR_ARRAY );
	m_GLState.bColourArrayState = true;

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: DisableColourArray
/// Params: None
///
/////////////////////////////////////////////////////
void OpenGL::DisableColourArray( void )
{
	// already off?
	if( m_GLState.bColourArrayState == false )
		return;

	glDisableClientState( GL_COLOR_ARRAY );
	m_GLState.bColourArrayState = false;

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: EnableNormalArray
/// Params: None
///
/////////////////////////////////////////////////////
void OpenGL::EnableNormalArray( void )
{
	// already on?
	if( m_GLState.bNormalArrayState == true )
		return;

	glEnableClientState( GL_NORMAL_ARRAY );
	m_GLState.bNormalArrayState = true;

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: DisableNormalArray
/// Params: None
///
/////////////////////////////////////////////////////
void OpenGL::DisableNormalArray( void )
{
	// already off?
	if( m_GLState.bNormalArrayState == false )
		return;

	glDisableClientState( GL_NORMAL_ARRAY );
	m_GLState.bNormalArrayState = false;

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: DisableVBO
/// Params: None
///
/////////////////////////////////////////////////////
void OpenGL::DisableVBO( void )
{
	// already off?
	//if( m_GLState.bVBOActive == false )
	//	return;

	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	m_GLState.bVBOActive = false;

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: EnableTexturing
/// Params: None
///
/////////////////////////////////////////////////////
void OpenGL::EnableTexturing( void )
{
	GLint i = 0;

	// simply set the main state
	m_GLState.bTextureState = true;

	// go through each unit and reenable texturing
	for( i = 0; i < m_MaxTextureUnits; i++ )
	{
		GLenum eActiveTexUnit = GL_TEXTURE0 + i;

		// make sure multitexturing is allowed
		glActiveTexture( eActiveTexUnit );

		if( m_GLState.TexUnits[i].nCurrent2DTexture != INVALID_OBJECT && m_GLState.TexUnits[i].nCurrent2DTexture != 0 )
		{
			glEnable( GL_TEXTURE_2D );
			glBindTexture( GL_TEXTURE_2D, m_GLState.TexUnits[i].nCurrent2DTexture );
		}
		else
		{
			glDisable( GL_TEXTURE_2D );
		}
	}

	glActiveTexture( GL_TEXTURE0 );
	glEnable( GL_TEXTURE_2D );

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: DisableTexturing
/// Params: None
///
/////////////////////////////////////////////////////		
void OpenGL::DisableTexturing( void )
{
	// already off?
	if( m_GLState.bTextureState == false )
		return;

	GLint i = 0;

	// go through each unit and disable texturing
	for( i = 0; i < m_MaxTextureUnits; i++ )
	{
		GLenum eActiveTexUnit = GL_TEXTURE0 + i;

		// make sure multitexturing is allowed
		glActiveTexture( eActiveTexUnit );
		
		glDisable( GL_TEXTURE_2D );
	}
	
	glActiveTexture( GL_TEXTURE0 );
	
	m_GLState.bTextureState = false;

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: SetFillMode
/// Params: [in]eFillMode
///
/////////////////////////////////////////////////////
void OpenGL::SetFillMode( GLenum eFillMode )
{
	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: SetCullState
/// Params: [in]bCull, [in]eFace
///
/////////////////////////////////////////////////////
void OpenGL::SetCullState( bool bCull, GLenum eFace )
{
	if( m_GLState.bCullState != bCull )
	{
		if( bCull )
			glEnable( GL_CULL_FACE );
		else
			glDisable( GL_CULL_FACE );

		m_GLState.bCullState = bCull;
	}

	if( m_GLState.eCullFace != eFace )
	{
		glCullFace( eFace );

		m_GLState.eCullFace = eFace;
	}

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: EnableLighting
/// Params: None
///
/////////////////////////////////////////////////////
void OpenGL::EnableLighting( void )
{
	// already on?
	if( m_GLState.bLightingState == true )
		return;

	glEnable( GL_LIGHTING );
	m_GLState.bLightingState = true;

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: DisableLighting
/// Params: None
///
/////////////////////////////////////////////////////
void OpenGL::DisableLighting( void )
{
	// already off?
	if( m_GLState.bLightingState == false )
		return;

	glDisable( GL_LIGHTING );
	m_GLState.bLightingState = false;

	// check for errors
	GL_CHECK
}


/////////////////////////////////////////////////////
/// Method: BindFrameBufferObject
/// Params: [in]nFrameBufferID
///
/////////////////////////////////////////////////////
void OpenGL::BindFrameBufferObject( GLuint nFrameBufferID )
{
	if( nFrameBufferID == INVALID_OBJECT )
		return;

	if( bExtFrameBufferObject )
	{
		if( nFrameBufferID != INVALID_OBJECT )
		{
			glBindFramebuffer( GL_FRAMEBUFFER, nFrameBufferID );
			m_GLState.nCurrentFrameBuffer = nFrameBufferID;	
		}
	}

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: ClearUnitTextures
/// Params: None
///
/////////////////////////////////////////////////////
void OpenGL::ClearUnitTextures( void )
{
	GLint i = 0;

	// go through each unit and set it to nothing

	for( i = 0; i < m_MaxTextureUnits; i++ )
	{
		GLenum eActiveTexUnit = GL_TEXTURE0 + i;

		// make sure multitexturing is allowed
		glActiveTexture( eActiveTexUnit );

		glBindTexture( GL_TEXTURE_2D, 0 );
		m_GLState.TexUnits[i].nCurrent2DTexture = 0;

		m_GLState.nCurrentTexture = 0;
	}

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: ClearUnitTexture
/// Params: [in]nTexUnit
///
/////////////////////////////////////////////////////
void OpenGL::ClearUnitTexture( GLint nTexUnit )
{
	// make sure it's a valid unit
	if( nTexUnit >= 0 && nTexUnit < m_MaxTextureUnits )
	{
		// which unit are we working on
		GLenum eActiveTexUnit = GL_TEXTURE0 + nTexUnit;

		glActiveTexture( eActiveTexUnit );	

		glBindTexture( GL_TEXTURE_2D, 0 );
		m_GLState.TexUnits[nTexUnit].nCurrent2DTexture = 0;

		m_GLState.nCurrentTexture = 0;
	}

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: DisableUnitTexture
/// Params: [in]nTexUnit
///
/////////////////////////////////////////////////////
void OpenGL::DisableUnitTexture( GLint nTexUnit )
{
	// make sure it's a valid unit
	if( nTexUnit >= 0 && nTexUnit < m_MaxTextureUnits )
	{
		// which unit are we working on
		GLenum eActiveTexUnit = GL_TEXTURE0 + nTexUnit;

		glActiveTexture( eActiveTexUnit );	

		glDisable( GL_TEXTURE_2D );
	}

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: ClearFrameBufferObject
/// Params: None
///
/////////////////////////////////////////////////////
void OpenGL::ClearFrameBufferObject( void )
{
	if( bExtFrameBufferObject  )
	{
		// unbind any framebuffer
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
		m_GLState.nCurrentFrameBuffer = 0;
	}

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: SetMipmapParameters
/// Params: [in]nTexUnit, [in]eTarget, [in]eParameter, [in]fValue
///
/////////////////////////////////////////////////////
void OpenGL::SetMipmapParameters( GLint nTexUnit, GLenum eTarget, GLenum eParameter, GLfloat fValue )
{
	// make sure it's a valid unit
	if( nTexUnit >= 0 && nTexUnit < m_MaxTextureUnits )
	{
		// which unit are we working on
		GLenum eActiveTexUnit = GL_TEXTURE0 + nTexUnit;

		glActiveTexture( eActiveTexUnit );	


		if( eTarget == GL_TEXTURE_2D )
		{
			// magnification filter
			if( eParameter == GL_TEXTURE_MAG_FILTER )
			{
				if( m_GLState.TexUnits[nTexUnit].fTex2DMagFilter != fValue )
				{
					glTexParameterf( eTarget, eParameter, fValue );
					m_GLState.TexUnits[nTexUnit].fTex2DMagFilter = (GLfloat)fValue;
				}
			}
			else if( eParameter == GL_TEXTURE_MIN_FILTER )
			{
				// minification filter
				if( m_GLState.TexUnits[nTexUnit].fTex2DMinFilter != fValue )
				{
					glTexParameterf( eTarget, eParameter, fValue );
					m_GLState.TexUnits[nTexUnit].fTex2DMinFilter = (GLfloat)fValue;
				}
			}
		}
	}

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: SetMipmapLodParameters
/// Params: [in]nTexUnit, [in]eTarget, [in]eParameter, [in]fValue
///
/////////////////////////////////////////////////////
void OpenGL::SetMipmapLodParameters( GLint nTexUnit, GLenum eTarget, GLenum eParameter, GLfloat fValue )
{
	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: DepthMode
/// Params: [in]bDepthFlag, [in]eDepthFunc
///
/////////////////////////////////////////////////////
void OpenGL::DepthMode( bool bDepthFlag, GLenum eDepthFunc )
{
	if( m_GLState.bDepthState != bDepthFlag )
	{
		glDepthMask( bDepthFlag );

		m_GLState.bDepthState = bDepthFlag;
	}

	if( m_GLState.eDepthFunc != eDepthFunc )
	{
		glDepthFunc( eDepthFunc );

		m_GLState.eDepthFunc = eDepthFunc;
	}

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: BlendMode
/// Params: [in]bBlendFlag, [in]eBlendSrc, [in]eBlendDest
///
/////////////////////////////////////////////////////
void OpenGL::BlendMode( bool bBlendFlag, GLenum eBlendSrc, GLenum eBlendDest )
{
	if( m_GLState.bBlendState != bBlendFlag )
	{
		if( bBlendFlag )
			glEnable( GL_BLEND );
		else
			glDisable( GL_BLEND );

		m_GLState.bBlendState = bBlendFlag;
	}

	if( m_GLState.eBlendSrc != eBlendSrc || m_GLState.eBlendDest != eBlendDest )
	{
		glBlendFunc( eBlendSrc, eBlendDest );

		m_GLState.eBlendSrc = eBlendSrc;
		m_GLState.eBlendDest = eBlendDest;
	}

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: AlphaMode
/// Params: [in]bAlphaFlag, [in]eAlphaFunc, [in]fAlphaClamp
///
/////////////////////////////////////////////////////
void OpenGL::AlphaMode( bool bAlphaFlag, GLenum eAlphaFunc, GLfloat fAlphaClamp )
{
	if( m_GLState.bAlphaState != bAlphaFlag )
	{
		if( bAlphaFlag )
			glEnable( GL_ALPHA_TEST );
		else
			glDisable( GL_ALPHA_TEST );

		m_GLState.bAlphaState = bAlphaFlag;
	}

	if( m_GLState.eAlphaFunc != eAlphaFunc || m_GLState.fAlphaClamp != fAlphaClamp )
	{
		glAlphaFunc( eAlphaFunc, fAlphaClamp );

		m_GLState.eAlphaFunc = eAlphaFunc;
		m_GLState.fAlphaClamp = fAlphaClamp;
	}

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: FogMode
/// Params: [in]bFogFlag, [in]eFogMode, [in]vFogColour, [in]fFogNearClip, [in]fFogFarClip, [in]fFogDensity
///
/////////////////////////////////////////////////////
void OpenGL::FogMode( bool bFogFlag, GLenum eFogMode, math::Vec3& vFogColour, GLfloat fFogNearClip, GLfloat fFogFarClip, GLfloat fFogDensity )
{
	if( m_GLState.bFogState != bFogFlag )
	{
		if( bFogFlag )
			glEnable( GL_FOG );
		else
			glDisable( GL_FOG );

		m_GLState.bFogState = bFogFlag;
	}

	if( eFogMode == GL_LINEAR ||
	   eFogMode == GL_EXP ||
	   eFogMode == GL_EXP2 )
	{
		if( m_GLState.eFogMode != eFogMode )
		{
			glFogf( GL_FOG_MODE, eFogMode );

			m_GLState.eFogMode = eFogMode;
		}

		GLfloat fogCol[4] = { vFogColour.R, vFogColour.G, vFogColour.B, 1.0f };
		if(		m_GLState.vFogColour.R != fogCol[0] || 
			m_GLState.vFogColour.G != fogCol[1] ||
			m_GLState.vFogColour.B != fogCol[2] )
		{
			glFogfv( GL_FOG_COLOR, &fogCol[0] );

			m_GLState.vFogColour = vFogColour;
		}

		//if( fFogFarClip < fFogNearClip )
		//	DBGLOG( "OPENGLES: *WARNING* the fog far clip distance has been set less than the fog near clip\n" );

		if( m_GLState.fFogNearClip != fFogNearClip )
		{
			glFogf( GL_FOG_START, fFogNearClip );
		
			m_GLState.fFogNearClip = fFogNearClip;
		}

		if( m_GLState.fFogFarClip != fFogFarClip )
		{
			glFogf( GL_FOG_END, fFogFarClip );

			m_GLState.fFogFarClip = fFogFarClip;
		}

		if( m_GLState.fFogDensity != fFogDensity )
		{
			glFogf( GL_FOG_DENSITY, fFogDensity );

			m_GLState.fFogDensity = fFogDensity;
		}
	}
	
	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: EnableColourMaterial
/// Params: None
///
/////////////////////////////////////////////////////
void OpenGL::EnableColourMaterial()
{
	// already on?
	if( m_GLState.bColourMaterialState == true )
		return;

	glEnable( GL_COLOR_MATERIAL );
	m_GLState.bColourMaterialState = true;

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: DisableColourMaterial
/// Params: None
///
/////////////////////////////////////////////////////
void OpenGL::DisableColourMaterial()
{
	// already off?
	if( m_GLState.bColourMaterialState == false )
		return;

	glDisable( GL_COLOR_MATERIAL );
	m_GLState.bColourMaterialState = false;

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: SetMaterialAmbient
/// Params: [in]ambientColour
///
/////////////////////////////////////////////////////
void OpenGL::SetMaterialAmbient( const math::Vec4& ambientColour )
{
	if( m_GLState.vMaterialAmbient.R != ambientColour.R || 
		m_GLState.vMaterialAmbient.G != ambientColour.G ||
		m_GLState.vMaterialAmbient.B != ambientColour.B ||
		m_GLState.vMaterialAmbient.A != ambientColour.A )
	{
		glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, (GLfloat *)&ambientColour );

		m_GLState.vMaterialAmbient = ambientColour;
	}

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: SetMaterialDiffuse
/// Params: [in]diffuseColour
///
/////////////////////////////////////////////////////
void OpenGL::SetMaterialDiffuse( const math::Vec4& diffuseColour )
{
	if( m_GLState.vMaterialDiffuse.R != diffuseColour.R || 
		m_GLState.vMaterialDiffuse.G != diffuseColour.G ||
		m_GLState.vMaterialDiffuse.B != diffuseColour.B ||
		m_GLState.vMaterialDiffuse.A != diffuseColour.A )
	{
		glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, (GLfloat *)&diffuseColour );

		m_GLState.vMaterialDiffuse = diffuseColour;
	}

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: SetMaterialSpecular
/// Params: [in]specularColour
///
/////////////////////////////////////////////////////
void OpenGL::SetMaterialSpecular( const math::Vec4& specularColour )
{
	if( m_GLState.vMaterialSpecular.R != specularColour.R || 
		m_GLState.vMaterialSpecular.G != specularColour.G ||
		m_GLState.vMaterialSpecular.B != specularColour.B ||
		m_GLState.vMaterialSpecular.A != specularColour.A )
	{
		glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, (GLfloat *)&specularColour );

		m_GLState.vMaterialSpecular = specularColour;
	}

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: SetMaterialEmission
/// Params: [in]emissionColour
///
/////////////////////////////////////////////////////
void OpenGL::SetMaterialEmission( const math::Vec4& emissionColour )
{
	if( m_GLState.vMaterialEmission.R != emissionColour.R || 
		m_GLState.vMaterialEmission.G != emissionColour.G ||
		m_GLState.vMaterialEmission.B != emissionColour.B ||
		m_GLState.vMaterialEmission.A != emissionColour.A )
	{
		glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, (GLfloat *)&emissionColour );

		m_GLState.vMaterialEmission = emissionColour;
	}

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: SetMaterialShininess
/// Params: [in]shininess
///
/////////////////////////////////////////////////////
void OpenGL::SetMaterialShininess( GLfloat shininess )
{
	if( m_GLState.fMaterialShininess != shininess )
	{
		glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, shininess );

		m_GLState.fMaterialShininess = shininess;
	}

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: EnableLight
/// Params: [in]nLightUnit
///
/////////////////////////////////////////////////////
void OpenGL::EnableLight( int nLightUnit )
{
	DBG_ASSERT( ((nLightUnit >= 0) && (nLightUnit < renderer::MAX_USEABLE_LIGHTS)) );
	
	// already on?
	if( m_GLState.LightUnits[nLightUnit].bLightState == true )
		return;
	
	GLenum lightUnit = GL_LIGHT0 + nLightUnit;
	
	glEnable( lightUnit );
	m_GLState.LightUnits[nLightUnit].bLightState = true;
	
	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: DisableLight
/// Params: [in]nLightUnit
///
/////////////////////////////////////////////////////
void OpenGL::DisableLight( int nLightUnit )
{
	DBG_ASSERT( ((nLightUnit >= 0) && (nLightUnit < renderer::MAX_USEABLE_LIGHTS)) );
	
	// already off?
	if( m_GLState.LightUnits[nLightUnit].bLightState == false )
		return;
	
	GLenum lightUnit = GL_LIGHT0 + nLightUnit;
	
	glDisable( lightUnit );
	m_GLState.LightUnits[nLightUnit].bLightState = false;
	
	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: SetLightPosition
/// Params: [in]nLightUnit, [in]pos
///
/////////////////////////////////////////////////////
void OpenGL::SetLightPosition( int nLightUnit, const math::Vec4& pos )
{
	DBG_ASSERT( ((nLightUnit >= 0) && (nLightUnit < renderer::MAX_USEABLE_LIGHTS)) );
	
	// position is always set as it must be called after the scene view is set
	GLenum lightUnit = GL_LIGHT0 + nLightUnit;
	glLightfv( lightUnit, GL_POSITION, (GLfloat *)&pos );
	
	m_GLState.LightUnits[nLightUnit].vPosition = pos;
	
	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: SetLightAmbient
/// Params: [in]nLightUnit, [in]ambientColour
///
/////////////////////////////////////////////////////
void OpenGL::SetLightAmbient( int nLightUnit, const math::Vec4& ambientColour )
{
	DBG_ASSERT( ((nLightUnit >= 0) && (nLightUnit < renderer::MAX_USEABLE_LIGHTS)) );
	
	GLenum lightUnit = GL_LIGHT0 + nLightUnit;
	
	if( m_GLState.LightUnits[nLightUnit].vAmbientColour.R != ambientColour.R || 
	   m_GLState.LightUnits[nLightUnit].vAmbientColour.G != ambientColour.G ||
	   m_GLState.LightUnits[nLightUnit].vAmbientColour.B != ambientColour.B ||
	   m_GLState.LightUnits[nLightUnit].vAmbientColour.A != ambientColour.A )
	{
		glLightfv( lightUnit, GL_AMBIENT, (GLfloat *)&ambientColour );
		
		m_GLState.LightUnits[nLightUnit].vAmbientColour = ambientColour;
	}
	
	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: SetLightDiffuse
/// Params: [in]nLightUnit, [in]diffuseColour
///
/////////////////////////////////////////////////////
void OpenGL::SetLightDiffuse( int nLightUnit, const math::Vec4& diffuseColour )
{
	DBG_ASSERT( ((nLightUnit >= 0) && (nLightUnit < renderer::MAX_USEABLE_LIGHTS)) );
	
	GLenum lightUnit = GL_LIGHT0 + nLightUnit;
	
	if( m_GLState.LightUnits[nLightUnit].vDiffuseColour.R != diffuseColour.R || 
	   m_GLState.LightUnits[nLightUnit].vDiffuseColour.G != diffuseColour.G ||
	   m_GLState.LightUnits[nLightUnit].vDiffuseColour.B != diffuseColour.B ||
	   m_GLState.LightUnits[nLightUnit].vDiffuseColour.A != diffuseColour.A )
	{
		glLightfv( lightUnit, GL_DIFFUSE, (GLfloat *)&diffuseColour );
		
		m_GLState.LightUnits[nLightUnit].vDiffuseColour = diffuseColour;
	}
	
	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: SetLightSpecular
/// Params: [in]nLightUnit, [in]specularColour
///
/////////////////////////////////////////////////////
void OpenGL::SetLightSpecular( int nLightUnit, const math::Vec4& specularColour )
{
	DBG_ASSERT( ((nLightUnit >= 0) && (nLightUnit < renderer::MAX_USEABLE_LIGHTS)) );
	
	GLenum lightUnit = GL_LIGHT0 + nLightUnit;
	
	if( m_GLState.LightUnits[nLightUnit].vSpecularColour.R != specularColour.R || 
	   m_GLState.LightUnits[nLightUnit].vSpecularColour.G != specularColour.G ||
	   m_GLState.LightUnits[nLightUnit].vSpecularColour.B != specularColour.B ||
	   m_GLState.LightUnits[nLightUnit].vSpecularColour.A != specularColour.A )
	{
		glLightfv( lightUnit, GL_SPECULAR, (GLfloat *)&specularColour );
		
		m_GLState.LightUnits[nLightUnit].vSpecularColour = specularColour;
	}
	
	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: SetLightEmission
/// Params: [in]nLightUnit, [in]emissionColour
///
/////////////////////////////////////////////////////
/*void OpenGL::SetLightEmission( int nLightUnit, const math::Vec4& emissionColour )
 {
 DBG_ASSERT( ((nLightUnit >= 0) && (nLightUnit < renderer::MAX_USEABLE_LIGHTS)) );
 
 GLenum lightUnit = GL_LIGHT0 + nLightUnit;
 
 if( m_GLState.LightUnits[nLightUnit].vEmissionColour.R != emissionColour.R || 
 m_GLState.LightUnits[nLightUnit].vEmissionColour.G != emissionColour.G ||
 m_GLState.LightUnits[nLightUnit].vEmissionColour.B != emissionColour.B ||
 m_GLState.LightUnits[nLightUnit].vEmissionColour.A != emissionColour.A )
 {
 glLightfv( lightUnit, GL_EMISSION, (GLfloat *)&emissionColour );
 
 m_GLState.LightUnits[nLightUnit].vEmissionColour = emissionColour;
 }
 
 // check for errors
 GL_CHECK
 }*/

/////////////////////////////////////////////////////
/// Method: SetLightAttenuation
/// Params: [in]nLightUnit, [in]type, [in]val
///
/////////////////////////////////////////////////////
void OpenGL::SetLightAttenuation( int nLightUnit, GLenum type, GLfloat val )
{
	DBG_ASSERT( ((nLightUnit >= 0) && (nLightUnit < renderer::MAX_USEABLE_LIGHTS)) );
	DBG_ASSERT( ((type == GL_CONSTANT_ATTENUATION) || (type == GL_LINEAR_ATTENUATION) || (type == GL_QUADRATIC_ATTENUATION)) );
	
	GLenum lightUnit = GL_LIGHT0 + nLightUnit;
	
	if( type == GL_CONSTANT_ATTENUATION )
	{
		if( m_GLState.LightUnits[nLightUnit].vAttenuation.X  != val )
		{
			glLightf( lightUnit, GL_CONSTANT_ATTENUATION, val );
			m_GLState.LightUnits[nLightUnit].vAttenuation.X = val;
		}
	}
	else if( type == GL_LINEAR_ATTENUATION )
	{
		if( m_GLState.LightUnits[nLightUnit].vAttenuation.Y  != val )
		{
			glLightf( lightUnit, GL_LINEAR_ATTENUATION, val );
			m_GLState.LightUnits[nLightUnit].vAttenuation.Y = val;
		}
	}
	else if( type == GL_QUADRATIC_ATTENUATION )
	{
		if( m_GLState.LightUnits[nLightUnit].vAttenuation.Z  != val )
		{
			glLightf( lightUnit, GL_QUADRATIC_ATTENUATION, val );
			m_GLState.LightUnits[nLightUnit].vAttenuation.Z = val;
		}
	}
	
	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: SetLightSpotDirection
/// Params: [in]nLightUnit, [in]dir
///
/////////////////////////////////////////////////////
void OpenGL::SetLightSpotDirection( int nLightUnit, const math::Vec3& dir )
{
	DBG_ASSERT( ((nLightUnit >= 0) && (nLightUnit < renderer::MAX_USEABLE_LIGHTS)) );
	
	GLenum lightUnit = GL_LIGHT0 + nLightUnit;
	
	if( m_GLState.LightUnits[nLightUnit].vSpotDirection.X != dir.X || 
	   m_GLState.LightUnits[nLightUnit].vSpotDirection.Y != dir.Y ||
	   m_GLState.LightUnits[nLightUnit].vSpotDirection.Z != dir.Z )
	{
		glLightfv( lightUnit, GL_SPOT_DIRECTION, (GLfloat *)&dir );
		
		m_GLState.LightUnits[nLightUnit].vSpotDirection = dir;
	}
	
	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: SetLightSpotExponent
/// Params: [in]nLightUnit, [in]val
///
/////////////////////////////////////////////////////
void OpenGL::SetLightSpotExponent( int nLightUnit, GLfloat val )
{
	DBG_ASSERT( ((nLightUnit >= 0) && (nLightUnit < renderer::MAX_USEABLE_LIGHTS)) );
	DBG_ASSERT( ((val >= 0) && (val < 128)) );
	
	GLenum lightUnit = GL_LIGHT0 + nLightUnit;
	
	if( m_GLState.LightUnits[nLightUnit].fSpotExponent != val )
	{
		glLightf( lightUnit, GL_SPOT_EXPONENT, val );
		m_GLState.LightUnits[nLightUnit].fSpotExponent = val;
	}
}

/////////////////////////////////////////////////////
/// Method: SetLightSpotCutoff
/// Params: [in]nLightUnit, [in]val
///
/////////////////////////////////////////////////////
void OpenGL::SetLightSpotCutoff( int nLightUnit, GLfloat val )
{
	DBG_ASSERT( ((nLightUnit >= 0) && (nLightUnit < renderer::MAX_USEABLE_LIGHTS)) );
	if( val != 180.0f )
		DBG_ASSERT( ((val >= 0) && (val < 90)) );
	
	GLenum lightUnit = GL_LIGHT0 + nLightUnit;
	
	if( m_GLState.LightUnits[nLightUnit].fSpotCutOff != val )
	{
		glLightf( lightUnit, GL_SPOT_CUTOFF, val );
		m_GLState.LightUnits[nLightUnit].fSpotCutOff = val;
	}
}

/////////////////////////////////////////////////////
/// Method: SetColour4f
/// Params: [in]r, [in]g, [in]b, [in]a
///
/////////////////////////////////////////////////////
void OpenGL::SetColour4f( GLfloat r, GLfloat g, GLfloat b, GLfloat a )
{
	if( m_GLState.vColour4f.R != r || 
		m_GLState.vColour4f.G != g ||
		m_GLState.vColour4f.B != b ||
		m_GLState.vColour4f.A != a )
	{
		glColor4f( r, g, b, a );
		m_GLState.vColour4f = math::Vec4( r, g, b, a );

		m_GLState.vColour4ub.R = static_cast<GLubyte>( r*255.0f );
		m_GLState.vColour4ub.G = static_cast<GLubyte>( g*255.0f );
		m_GLState.vColour4ub.B = static_cast<GLubyte>( b*255.0f );
		m_GLState.vColour4ub.A = static_cast<GLubyte>( a*255.0f );
	}

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: SetColour4ub
/// Params: [in]r, [in]g, [in]b, [in]a
///
/////////////////////////////////////////////////////
void OpenGL::SetColour4ub( GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
	if( m_GLState.vColour4ub.R != r || 
		m_GLState.vColour4ub.G != g ||
		m_GLState.vColour4ub.B != b ||
		m_GLState.vColour4ub.A != a )
	{
		glColor4ub( r, g, b, a );
		m_GLState.vColour4ub = math::Vec4Lite( r, g, b, a );

		m_GLState.vColour4f.R = static_cast<float>( static_cast<float>(r)/255.0f );
		m_GLState.vColour4f.G = static_cast<float>( static_cast<float>(g)/255.0f );
		m_GLState.vColour4f.B = static_cast<float>( static_cast<float>(b)/255.0f );
		m_GLState.vColour4f.A = static_cast<float>( static_cast<float>(a)/255.0f );
	}

	// check for errors
	GL_CHECK
}
/*			
/////////////////////////////////////////////////////
/// Method: SetColour
/// Params: [in]vColour
///
/////////////////////////////////////////////////////
void OpenGL::SetColour( math::Vec4& vColour )
{
	if( m_GLState.vColour.R != vColour.R || 
		m_GLState.vColour.G != vColour.G ||
		m_GLState.vColour.B != vColour.B ||
		m_GLState.vColour.A != vColour.A )
	{
		DBG_ASSERT(0);
	}

	// check for errors
	GL_CHECK
}
*/

/////////////////////////////////////////////////////
/// Method: GetGLProperties
/// Params: None
///
/////////////////////////////////////////////////////
void OpenGL::GetGLProperties( void )
{
	glGetIntegerv( GL_MAX_TEXTURE_SIZE, &m_MaxTextureSize );
	DBGLOG( "GL_MAX_TEXTURE_SIZE: (%d)\n", m_MaxTextureSize );

	glGetIntegerv( GL_MAX_LIGHTS, &m_MaxLights ); 
	DBGLOG( "GL_MAX_LIGHTS: (%d)\n", m_MaxLights );

	glGetIntegerv( GL_DEPTH_BITS, &m_DepthBits );
	DBGLOG( "GL_DEPTH_BITS: (%d)\n", m_DepthBits );
		

	// GL version checks
	m_HasGLVersion1_0 = SupportsGLVersion( 1, 0 );
	m_HasGLVersion1_1 = SupportsGLVersion( 1, 1 );

	m_HasGLVersion2_0 = SupportsGLVersion( 2, 0 );
	m_HasGLVersion2_1 = SupportsGLVersion( 2, 1 );

	DBGLOG( "\n" );

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: ClearScreen
/// Params: None
///
/////////////////////////////////////////////////////
void OpenGL::ClearScreen( void )
{
	// Set background clearing colour
	glClearColor( m_vClearColour.R, m_vClearColour.G, m_vClearColour.B, m_vClearColour.A );
    
	// Clear the window with current clearing colour
	glClear( m_ClearBits );
    
	// set the viewport for this frame
	glViewport( 0, 0, m_Width, m_Height );	
	
	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: ClearColour
/// Params: [in]fRed, [in]fGreen, [in]fBlue, [in]fAlpha
///
/////////////////////////////////////////////////////
void OpenGL::ClearColour( GLfloat fRed, GLfloat fGreen, GLfloat fBlue, GLfloat fAlpha )
{
	m_vClearColour = math::Vec4( fRed, fGreen, fBlue, fAlpha ); 

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: SetClearBits
/// Params: [in]bits
///
/////////////////////////////////////////////////////
void OpenGL::SetClearBits( GLbitfield bits )
{
	m_ClearBits = bits;
}

/////////////////////////////////////////////////////
/// Method: SetupPerspectiveView
/// Params: [in]nWidth, [in]nHeight
///
/////////////////////////////////////////////////////
void OpenGL::SetupPerspectiveView( GLint nWidth, GLint nHeight, bool bForceUpdate )
{
	if( nHeight == 0 )
		nHeight = 1;

	if( nWidth == 0 )
		nWidth = 1;

	// don't change if it's already there
	if( !bForceUpdate )
	{
		if( m_Width == nWidth &&
			m_Height == nHeight && 
			m_eViewState == VIEWSTATE_PERSPECTIVE )
			return;
	}

	m_Width = nWidth;
	m_Height = nHeight;

	m_eViewState = VIEWSTATE_PERSPECTIVE;
   
	glMatrixMode( GL_PROJECTION );
		glLoadIdentity( );
		if( m_Mirror )
			glScalef( m_MirrorScales.X, m_MirrorScales.Y, 1.0f );
		gluPerspective( m_FOV, m_AspectRatio, m_NearClip, m_FarClip );
		if( m_Rotate )
			glRotatef( m_RotationAngle, 0.0f, 0.0f, 1.0f );
	glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();		
    
	m_Viewport[0] = 0;
	m_Viewport[1] = 0;
	m_Viewport[2] = nWidth;
	m_Viewport[3] = nHeight;

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: SetupOrthographicView
/// Params: [in]nWidth, [in]nHeight
///
/////////////////////////////////////////////////////
void OpenGL::SetupOrthographicView( GLint nWidth, GLint nHeight, bool bForceUpdate )
{
	if( nHeight == 0 )
		nHeight = 1;

	if( nWidth == 0 )
		nWidth = 1;

	// don't change if it's already there
	if( !bForceUpdate )
	{
		if( m_Width == nWidth &&
			m_Height == nHeight && 
			m_eViewState == VIEWSTATE_ORTHOGRAPHIC )
			return;
	}

    m_Width = nWidth;
    m_Height = nHeight;
    
	m_eViewState = VIEWSTATE_ORTHOGRAPHIC;
	
	glMatrixMode( GL_PROJECTION );
		glLoadIdentity( );
		if( m_Mirror )
			glScalef( m_MirrorScales.X, m_MirrorScales.Y, 1.0f );
        glOrthof( 0.0f, static_cast<float>(m_Width), 0.0f, static_cast<float>(m_Height), m_NearClip, m_FarClip );
        if( m_Rotate )
            glRotatef( m_RotationAngle, 0.0f, 0.0f, 1.0f );	    
	glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();	
    
	// this translation seems to correct pixel shifting according to the GL tips section in the red book
	float pixelOffset = 0.4f;
	
	// make bottom left 0,0
	if( m_Rotate )
    {
        switch( m_RotationStyle )
        {
            case VIEWROTATION_PORTRAIT_BUTTON_BOTTOM:
            {
                // shouldn't need to do anything
				//if( !m_MSAAIsActive )
					glTranslatef( pixelOffset, pixelOffset, 0.0f );
				
            }break;                
            case VIEWROTATION_PORTRAIT_BUTTON_TOP:            
            {
				//if( !m_MSAAIsActive )
					glTranslatef( pixelOffset, pixelOffset, 0.0f );
				
               glTranslatef( -static_cast<float>(m_Width), -static_cast<float>(m_Height), 0.0f );
            }break;                
            case VIEWROTATION_LANDSCAPE_BUTTON_LEFT:
            {
				//if( !m_MSAAIsActive )
					glTranslatef( pixelOffset, pixelOffset, 0.0f );
				
                glTranslatef( 0.0f, -static_cast<float>(m_Width), 0.0f );
				
            }break;
            case VIEWROTATION_LANDSCAPE_BUTTON_RIGHT: 
            {
				//if( !m_MSAAIsActive )
					glTranslatef( pixelOffset, pixelOffset, 0.0f );
				
                glTranslatef( -static_cast<float>(m_Height), 0.0f, 0.0f );
            }break;
                
            default:
                break;
        }
    }

	m_Viewport[0] = 0;
	m_Viewport[1] = 0;
	m_Viewport[2] = nWidth;
	m_Viewport[3] = nHeight;

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: SetLookAt
/// Params: [in]fEyeX, [in]fEyeY, [in]fEyeZ, [in]fCenterX, [in]fCenterY, [in]fCenterZ
///
/////////////////////////////////////////////////////
void OpenGL::SetLookAt( GLfloat fEyeX, GLfloat fEyeY, GLfloat fEyeZ, GLfloat fCenterX, GLfloat fCenterY, GLfloat fCenterZ, GLfloat upX, GLfloat upY, GLfloat upZ )
{
	gluLookAt( fEyeX, fEyeY, fEyeZ, fCenterX, fCenterY, fCenterZ, upX, upY, upZ );

	m_vLookAtEye.X = fEyeX;
	m_vLookAtEye.Y = fEyeY;
	m_vLookAtEye.Z = fEyeZ;

	m_vLookAtCenter.X = fCenterX;
	m_vLookAtCenter.Y = fCenterY;
	m_vLookAtCenter.Z = fCenterZ;

	ExtractFrustum();

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: SetNearFarClip
/// Params: [in]fNearClip, [in]fFarClip
///
/////////////////////////////////////////////////////
void OpenGL::SetNearFarClip( GLfloat fNearClip, GLfloat fFarClip )
{
	if( fFarClip < fNearClip )
		DBGLOG( "OPENGLES: *WARNING* the far clip distance has been set less than the near clip\n" );

	m_NearClip = fNearClip;
	m_FarClip = fFarClip;

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: SetFieldOfView
/// Params: [in]fAngle
///
/////////////////////////////////////////////////////
void OpenGL::SetFieldOfView( GLfloat fAngle )
{
	if( fAngle > 270.0f )
	{
		DBGLOG( "OPENGLES: *WARNING* the FOV angle has been requested greater than 270degs forcing it to 270deg\n" );
		fAngle = 270.0f;
	}

	m_FOV = fAngle;

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: SetViewport
/// Params: [in]nWidth, [in]nHeight
///
/////////////////////////////////////////////////////
void OpenGL::SetViewport( GLint nWidth, GLint nHeight )
{
	if( nHeight == 0 )
		nHeight = 1;

	if( nWidth == 0 )
		nWidth = 1;

	// don't change if it's already there
	if( m_Width == nWidth &&
		m_Height == nHeight )
		return;

	m_Width = nWidth;
	m_Height = nHeight;

    glViewport( 0, 0, nWidth, nHeight );

	m_Viewport[0] = 0;
	m_Viewport[1] = 0;
	m_Viewport[2] = nWidth;
	m_Viewport[3] = nHeight;

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: SetPerspective
/// Params: [in]fFOV, [in]fAspectRatio, [in]fNearClip, [in]fFarClip
///
/////////////////////////////////////////////////////
void OpenGL::SetPerspective( GLfloat fFOV, GLfloat fAspectRatio, GLfloat fNearClip, GLfloat fFarClip )
{
	// don't change if it's already there
	if( m_FOV == fFOV &&
		m_NearClip == fNearClip &&
		m_FarClip == fFarClip &&
		m_AspectRatio == fAspectRatio &&
		m_eViewState == VIEWSTATE_PERSPECTIVE )
		return;

	m_FOV = fFOV;	
	m_NearClip = fNearClip;
	m_FarClip = fFarClip;
	m_AspectRatio = fAspectRatio;
	m_eViewState = VIEWSTATE_PERSPECTIVE;
   
	glMatrixMode( GL_PROJECTION );
		glLoadIdentity( );
		if( m_Mirror )
			glScalef( m_MirrorScales.X, m_MirrorScales.Y, 1.0f );
		gluPerspective( m_FOV, m_AspectRatio, fNearClip, fFarClip );
		if( m_Rotate )
			glRotatef( m_RotationAngle, 0.0f, 0.0f, 1.0f );
	glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();
	
	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: SaveDepthBlendAlphaState
/// Params: [in/out]pState
///
/////////////////////////////////////////////////////
void OpenGL::SaveDepthBlendAlphaState( renderer::TGLDepthBlendAlphaState *pState )
{
	if( pState )
	{
		// depth
		pState->bDepthState		= m_GLState.bDepthState;
		pState->eDepthFunc		= m_GLState.eDepthFunc;

		// blend
		pState->bBlendState		= m_GLState.bBlendState;
		pState->eBlendSrc		= m_GLState.eBlendSrc;
		pState->eBlendDest		= m_GLState.eBlendDest;

		// alpha
		pState->bAlphaState		= m_GLState.bAlphaState;
		pState->eAlphaFunc		= m_GLState.eAlphaFunc;
		pState->fAlphaClamp		= m_GLState.fAlphaClamp;
	}

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: SetDepthBlendAlphaState
/// Params: [in]State
///
/////////////////////////////////////////////////////
void OpenGL::SetDepthBlendAlphaState( renderer::TGLDepthBlendAlphaState* pState )
{
	DBG_ASSERT( pState != 0 );

	DepthMode( pState->bDepthState, pState->eDepthFunc );

	BlendMode( pState->bBlendState, pState->eBlendSrc, pState->eBlendDest );

	AlphaMode( pState->bAlphaState, pState->eAlphaFunc, pState->fAlphaClamp );

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: SaveFogState
/// Params: [in/out]pState
///
/////////////////////////////////////////////////////
void OpenGL::SaveFogState( renderer::TGLFogState *pState )
{
	if( pState )
	{
		// fog
		pState->bFogState		= m_GLState.bFogState;
		pState->vFogColour		= m_GLState.vFogColour;
		pState->eFogMode		= m_GLState.eFogMode;
		pState->fFogNearClip	= m_GLState.fFogNearClip;
		pState->fFogFarClip		= m_GLState.fFogFarClip;
		pState->fFogDensity		= m_GLState.fFogDensity;
	}

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: SetFogState
/// Params: [in]State
///
/////////////////////////////////////////////////////
void OpenGL::SetFogState( renderer::TGLFogState* pState )
{
	DBG_ASSERT( pState != 0 );

	FogMode( pState->bFogState, pState->eFogMode, pState->vFogColour, pState->fFogNearClip, pState->fFogFarClip, pState->fFogDensity );

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: SaveViewState
/// Params: [in/out]pState
///
/////////////////////////////////////////////////////
void OpenGL::SaveViewState( renderer::TViewState *pState )
{
	if( pState )
	{
		pState->eViewState = m_eViewState;
		pState->fFOV = m_FOV;
		pState->fNearClip = m_NearClip;
		pState->fFarClip = m_FarClip;
		pState->nWidth = m_Width;
		pState->nHeight = m_Height;
		pState->fAspectRatio = m_AspectRatio;
	}

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: SetViewState
/// Params: [in]pViewState
///
/////////////////////////////////////////////////////
void OpenGL::SetViewState( renderer::TViewState* pViewState )
{
	DBG_ASSERT( pViewState != 0 );

	m_eViewState	= pViewState->eViewState;
	m_FOV			= pViewState->fFOV;
	m_NearClip		= pViewState->fNearClip;
	m_FarClip		= pViewState->fFarClip;
	m_Width		= pViewState->nWidth;
	m_Height		= pViewState->nHeight;

	if( m_Height == 0 )
		m_Height = 1;

	if( m_Width == 0 )
		m_Width = 1;

	if( m_eViewState == VIEWSTATE_PERSPECTIVE )
		SetupPerspectiveView( m_Width, m_Height );

	if( m_eViewState == VIEWSTATE_ORTHOGRAPHIC )
		SetupOrthographicView( m_Width, m_Height );

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: SaveCullState
/// Params: [in]viewState
///
/////////////////////////////////////////////////////
void OpenGL::SaveCullState( renderer::TGLCullFaceState* pState )
{
	if( pState )
	{
		pState->bCullState	= m_GLState.bCullState;
		pState->eCullFace	= m_GLState.eCullFace;
	}

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: SetCullState
/// Params: [in]pCullState
///
/////////////////////////////////////////////////////
void OpenGL::SetCullState( renderer::TGLCullFaceState* pCullState )
{
	DBG_ASSERT( pCullState != 0 );

	SetCullState( pCullState->bCullState, pCullState->eCullFace );

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: SetAmbientColour
/// Params: [in]fAmbientRed, [in]fAmbientGreen, [in]fAmbientBlue, [in]fAmbientAlpha
///
/////////////////////////////////////////////////////
void OpenGL::SetAmbientColour( GLfloat fAmbientRed, GLfloat fAmbientGreen, GLfloat fAmbientBlue, GLfloat fAmbientAlpha )
{
	if( m_vAmbientColour.R != fAmbientRed || 
		m_vAmbientColour.G != fAmbientGreen ||
		m_vAmbientColour.B != fAmbientBlue ||
		m_vAmbientColour.A != fAmbientAlpha )
	{
		m_vAmbientColour = math::Vec4( fAmbientRed, fAmbientGreen, fAmbientBlue, fAmbientAlpha);
		
		glLightModelfv( GL_LIGHT_MODEL_AMBIENT, (GLfloat *)&m_vAmbientColour );
	}

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: SetActiveTextureFilter
/// Params: [in]eMinFilter, [in]eMagFilter
///
/////////////////////////////////////////////////////
void OpenGL::SetActiveTextureFilter( GLenum eMinFilter, GLenum eMagFilter )
{
	m_eTextureMinFilter = eMinFilter;
	m_eTextureMagFilter = eMagFilter;

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: SetAnisotropyLevel
/// Params: [in]fAnisotropyLevel
///
/////////////////////////////////////////////////////
void OpenGL::SetAnisotropyLevel( GLfloat fAnisotropyLevel )
{
	if( fAnisotropyLevel > m_MaxAnisotropyLevel )
	{
		m_AnisotropyLevel = m_MaxAnisotropyLevel;
		DBGLOG( "OPENGLES: *WARNING* Anisotropic level requested was higher than supported, setting to max supported level\n" );
	}
	else
		m_AnisotropyLevel = fAnisotropyLevel;

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: EnableSeparateSpecular
/// Params: None
///
/////////////////////////////////////////////////////
void OpenGL::EnableSeparateSpecular( void )
{
	// if the extension for separate colour is avaiable use it to stop oversaturated specular from the lights
	//if( bExtSeparateSpecularColor )
	//	glLightModeli( GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR );

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: DisableSeparateSpecular
/// Params: None
///
/////////////////////////////////////////////////////
void OpenGL::DisableSeparateSpecular( void )
{
	// reset to default additive specular
	//if( bExtSeparateSpecularColor )
	//	glLightModeli( GL_LIGHT_MODEL_COLOR_CONTROL, GL_SINGLE_COLOR );

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: SetRotationStyle
/// Params: [in]style
///
/////////////////////////////////////////////////////
void OpenGL::SetRotationStyle( renderer::EViewRotation style )
{
    switch (style) 
    {           
        case VIEWROTATION_PORTRAIT_BUTTON_BOTTOM:
        {
            m_RotationAngle = 0.0f;
        }break;
        case VIEWROTATION_PORTRAIT_BUTTON_TOP:
        {
            m_RotationAngle = 180.0f;
        }break;
        case VIEWROTATION_LANDSCAPE_BUTTON_LEFT:
        {
            m_RotationAngle = 90.0f;
        }break;
        case VIEWROTATION_LANDSCAPE_BUTTON_RIGHT:
        {
            m_RotationAngle = -90.0f;
        }break;
            
        default:
            DBG_ASSERT(0);
            break;
    }
    
    m_RotationStyle = style;
}

/////////////////////////////////////////////////////
/// Method: SetRotationStyle
/// Params: [in]style
///
/////////////////////////////////////////////////////
void OpenGL::SetMirrorStyle( renderer::EViewMirror style )
{
	if( style & renderer::VIEWMIRROR_VERTICAL )
		m_MirrorScales.Y = -1.0f;
	else
		m_MirrorScales.Y = 1.0f;

	if( style & renderer::VIEWMIRROR_HORIZONTAL )
		m_MirrorScales.X = -1.0f;
	else
		m_MirrorScales.X = 1.0f;

	m_MirrorStyle = style;
}

/////////////////////////////////////////////////////
/// Method: SetClipPlane
/// Params: None
///
/////////////////////////////////////////////////////
void OpenGL::SetClipPlane( GLenum plane, const math::Vec4& eq )
{
	glClipPlanef( plane, (GLfloat *)&eq );

	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: Flip
/// Params: None
///
/////////////////////////////////////////////////////
void OpenGL::Flip( void )
{
//	glFinish();

	// FIXME: clear all units for the start of next frame?
	//ClearUnitTextures();

#ifdef BASE_PLATFORM_RASPBERRYPI
	eglSwapBuffers( m_EGLDisplay, m_EGLSurface);
#endif // BASE_PLATFORM_RASPBERRYPI

	GL_CHECK

	// reset the counters
	nTotalTriangleCount = 0;
	nTotalVertexCount = 0;
	nTotalMaterialCount = 0;
	nTotalTextureCount = 0;
}

/////////////////////////////////////////////////////
/// Method: ExtractFrustum
/// Params: None
///
/////////////////////////////////////////////////////
void OpenGL::ExtractFrustum( void )
{
	GLfloat	t;

	// get the viewport
	//glGetIntegerv( GL_VIEWPORT, m_Viewport );
	
	// get projection matrix
	glGetFloatv( GL_PROJECTION_MATRIX, m_ProjMatrix );
	
	// get the model view matrix
	glGetFloatv( GL_MODELVIEW_MATRIX, m_ModlMatrix );
	
	// combine the two matrices (proj*mdlview)
	m_ClipMatrix[0] = m_ModlMatrix[0]*m_ProjMatrix[0] + m_ModlMatrix[1]*m_ProjMatrix[4] + m_ModlMatrix[2]*m_ProjMatrix[8] + m_ModlMatrix[3]*m_ProjMatrix[12];
	m_ClipMatrix[1] = m_ModlMatrix[0]*m_ProjMatrix[1] + m_ModlMatrix[1]*m_ProjMatrix[5] + m_ModlMatrix[2]*m_ProjMatrix[9] + m_ModlMatrix[3]*m_ProjMatrix[13];
	m_ClipMatrix[2] = m_ModlMatrix[0]*m_ProjMatrix[2] + m_ModlMatrix[1]*m_ProjMatrix[6] + m_ModlMatrix[2]*m_ProjMatrix[10] + m_ModlMatrix[3]*m_ProjMatrix[14];
	m_ClipMatrix[3] = m_ModlMatrix[0]*m_ProjMatrix[3] + m_ModlMatrix[1]*m_ProjMatrix[7] + m_ModlMatrix[2]*m_ProjMatrix[11] + m_ModlMatrix[3]*m_ProjMatrix[15];
	
	m_ClipMatrix[4] = m_ModlMatrix[4]*m_ProjMatrix[0] + m_ModlMatrix[5]*m_ProjMatrix[4] + m_ModlMatrix[6]*m_ProjMatrix[8] + m_ModlMatrix[7]*m_ProjMatrix[12];
	m_ClipMatrix[5] = m_ModlMatrix[4]*m_ProjMatrix[1] + m_ModlMatrix[5]*m_ProjMatrix[5] + m_ModlMatrix[6]*m_ProjMatrix[9] + m_ModlMatrix[7]*m_ProjMatrix[13];
	m_ClipMatrix[6] = m_ModlMatrix[4]*m_ProjMatrix[2] + m_ModlMatrix[5]*m_ProjMatrix[6] + m_ModlMatrix[6]*m_ProjMatrix[10] + m_ModlMatrix[7]*m_ProjMatrix[14];
	m_ClipMatrix[7] = m_ModlMatrix[4]*m_ProjMatrix[3] + m_ModlMatrix[5]*m_ProjMatrix[7] + m_ModlMatrix[6]*m_ProjMatrix[11] + m_ModlMatrix[7]*m_ProjMatrix[15];
	
	m_ClipMatrix[8] = m_ModlMatrix[8]*m_ProjMatrix[0] + m_ModlMatrix[9]*m_ProjMatrix[4] + m_ModlMatrix[10]*m_ProjMatrix[8] + m_ModlMatrix[11]*m_ProjMatrix[12];
	m_ClipMatrix[9] = m_ModlMatrix[8]*m_ProjMatrix[1] + m_ModlMatrix[9]*m_ProjMatrix[5] + m_ModlMatrix[10]*m_ProjMatrix[9] + m_ModlMatrix[11]*m_ProjMatrix[13];
	m_ClipMatrix[10] = m_ModlMatrix[8]*m_ProjMatrix[2] + m_ModlMatrix[9]*m_ProjMatrix[6] + m_ModlMatrix[10]*m_ProjMatrix[10] + m_ModlMatrix[11]*m_ProjMatrix[14];
	m_ClipMatrix[11] = m_ModlMatrix[8]*m_ProjMatrix[3] + m_ModlMatrix[9]*m_ProjMatrix[7] + m_ModlMatrix[10]*m_ProjMatrix[11] + m_ModlMatrix[11]*m_ProjMatrix[15];
	
	m_ClipMatrix[12] = m_ModlMatrix[12]*m_ProjMatrix[0] + m_ModlMatrix[13]*m_ProjMatrix[4] + m_ModlMatrix[14]*m_ProjMatrix[8] + m_ModlMatrix[15]*m_ProjMatrix[12];
	m_ClipMatrix[13] = m_ModlMatrix[12]*m_ProjMatrix[1] + m_ModlMatrix[13]*m_ProjMatrix[5] + m_ModlMatrix[14]*m_ProjMatrix[9] + m_ModlMatrix[15]*m_ProjMatrix[13];
	m_ClipMatrix[14] = m_ModlMatrix[12]*m_ProjMatrix[2] + m_ModlMatrix[13]*m_ProjMatrix[6] + m_ModlMatrix[14]*m_ProjMatrix[10] + m_ModlMatrix[15]*m_ProjMatrix[14];
	m_ClipMatrix[15] = m_ModlMatrix[12]*m_ProjMatrix[3] + m_ModlMatrix[13]*m_ProjMatrix[7] + m_ModlMatrix[14]*m_ProjMatrix[11] + m_ModlMatrix[15]*m_ProjMatrix[15];
	
	// the right plane
	m_Frustum[0][0] = m_ClipMatrix[3] - m_ClipMatrix[0];
	m_Frustum[0][1] = m_ClipMatrix[7] - m_ClipMatrix[4];
	m_Frustum[0][2] = m_ClipMatrix[11] - m_ClipMatrix[8];
	m_Frustum[0][3] = m_ClipMatrix[15] - m_ClipMatrix[12];
	
	// normalise
	t = std::sqrt(m_Frustum[0][0]*m_Frustum[0][0] + m_Frustum[0][1]*m_Frustum[0][1] + m_Frustum[0][2]*m_Frustum[0][2] );
	m_Frustum[0][0] /= t;
	m_Frustum[0][1] /= t;
	m_Frustum[0][2] /= t;
	m_Frustum[0][3] /= t;
	
	// the left plane
	m_Frustum[1][0] = m_ClipMatrix[3] + m_ClipMatrix[0];
	m_Frustum[1][1] = m_ClipMatrix[7] + m_ClipMatrix[4];
	m_Frustum[1][2] = m_ClipMatrix[11] + m_ClipMatrix[8];
	m_Frustum[1][3] = m_ClipMatrix[15] + m_ClipMatrix[12];
	
	// normalise
	t = std::sqrt(m_Frustum[1][0]*m_Frustum[1][0] + m_Frustum[1][1]*m_Frustum[1][1] + m_Frustum[1][2]*m_Frustum[1][2] );
	m_Frustum[1][0] /= t;
	m_Frustum[1][1] /= t;
	m_Frustum[1][2] /= t;
	m_Frustum[1][3] /= t;
	
	// bottom plane
	m_Frustum[2][0] = m_ClipMatrix[3] + m_ClipMatrix[1];
	m_Frustum[2][1] = m_ClipMatrix[7] + m_ClipMatrix[5];
	m_Frustum[2][2] = m_ClipMatrix[11] + m_ClipMatrix[9];
	m_Frustum[2][3] = m_ClipMatrix[15] + m_ClipMatrix[13];
	
	// normalise
	t = std::sqrt(m_Frustum[2][0]*m_Frustum[2][0] + m_Frustum[2][1]*m_Frustum[2][1] + m_Frustum[2][2]*m_Frustum[2][2] );
	m_Frustum[2][0] /= t;
	m_Frustum[2][1] /= t;
	m_Frustum[2][2] /= t;
	m_Frustum[2][3] /= t;
	
	// top plane
	m_Frustum[3][0] = m_ClipMatrix[3] - m_ClipMatrix[1];
	m_Frustum[3][1] = m_ClipMatrix[7] - m_ClipMatrix[5];
	m_Frustum[3][2] = m_ClipMatrix[11] - m_ClipMatrix[9];
	m_Frustum[3][3] = m_ClipMatrix[15] - m_ClipMatrix[13];
	
	// normalise
	t = std::sqrt(m_Frustum[3][0]*m_Frustum[3][0] + m_Frustum[3][1]*m_Frustum[3][1] + m_Frustum[3][2]*m_Frustum[3][2] );
	m_Frustum[3][0] /= t;
	m_Frustum[3][1] /= t;
	m_Frustum[3][2] /= t;
	m_Frustum[3][3] /= t;
	
	// far plane
	m_Frustum[4][0] = m_ClipMatrix[3] - m_ClipMatrix[2];
	m_Frustum[4][1] = m_ClipMatrix[7] - m_ClipMatrix[6];
	m_Frustum[4][2] = m_ClipMatrix[11] - m_ClipMatrix[10];
	m_Frustum[4][3] = m_ClipMatrix[15] - m_ClipMatrix[14];
	
	// normalise
	t = std::sqrt(m_Frustum[4][0]*m_Frustum[4][0] + m_Frustum[4][1]*m_Frustum[4][1] + m_Frustum[4][2]*m_Frustum[4][2] );
	m_Frustum[4][0] /= t;
	m_Frustum[4][1] /= t;
	m_Frustum[4][2] /= t;
	m_Frustum[4][3] /= t;
	
	// near plane
	m_Frustum[5][0] = m_ClipMatrix[3] + m_ClipMatrix[2];
	m_Frustum[5][1] = m_ClipMatrix[7] + m_ClipMatrix[6];
	m_Frustum[5][2] = m_ClipMatrix[11] + m_ClipMatrix[10];
	m_Frustum[5][3] = m_ClipMatrix[15] + m_ClipMatrix[14];
	
	// normalise
	t = std::sqrt(m_Frustum[5][0]*m_Frustum[5][0] + m_Frustum[5][1]*m_Frustum[5][1] + m_Frustum[5][2]*m_Frustum[5][2] );
	m_Frustum[5][0] /= t;
	m_Frustum[5][1] /= t;
	m_Frustum[5][2] /= t;
	m_Frustum[5][3] /= t;
	
	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: PointInFrustum
/// Params: [in]fX, [in]fY, [in]fZ
///
/////////////////////////////////////////////////////
bool OpenGL::PointInFrustum( GLfloat fX, GLfloat fY, GLfloat fZ )
{
	GLint nPlaneNum;

	for( nPlaneNum = 0; nPlaneNum < 6; nPlaneNum++ )
		if(m_Frustum[nPlaneNum][0] * fX + m_Frustum[nPlaneNum][1] * fY + 
			m_Frustum[nPlaneNum][2] * fZ + m_Frustum[nPlaneNum][3] <= 0 )
			return false;

	// check for errors
	GL_CHECK

	return true;
}

/////////////////////////////////////////////////////
/// Method: SphereInFrustum
/// Params: [in]fX, [in]fY, [in]fZ, [in]fRadius
///
/////////////////////////////////////////////////////
bool OpenGL::SphereInFrustum( GLfloat fX, GLfloat fY, GLfloat fZ, GLfloat fRadius )
{
	GLint nPlaneNum;

	for( nPlaneNum = 0; nPlaneNum < 6; nPlaneNum++ )
		if(m_Frustum[nPlaneNum][0] * fX + m_Frustum[nPlaneNum][1] * fY + 
			m_Frustum[nPlaneNum][2] * fZ + m_Frustum[nPlaneNum][3] <= -fRadius )
			return false;

	// check for errors
	GL_CHECK

	return true;
}

/////////////////////////////////////////////////////
/// Method: SphereDistanceFromFrustum
/// Params: [in]fX, [in]fY, [in]fZ, [in]fRadius
///
/////////////////////////////////////////////////////
GLfloat OpenGL::SphereDistanceFromFrustum( GLfloat fX, GLfloat fY, GLfloat fZ, GLfloat /*fRadius*/ )
{
	// this returns the distance from the near plane
	GLfloat fDiffX, fDiffY, fDiffZ;

	fDiffX = std::abs( m_vLookAtEye.X - fX );
	fDiffY = std::abs( m_vLookAtEye.Y - fY );
	fDiffZ = std::abs( m_vLookAtEye.Z - fZ );

	GLfloat distance = std::sqrt( (fDiffX*fDiffX) + (fDiffY*fDiffY) + (fDiffZ*fDiffZ) );

	// check for errors
	GL_CHECK

	return(distance);
}

/////////////////////////////////////////////////////
/// Method: CubeInFrustum
/// Params: [in]fX, [in]fY, [in]fZ, [in]fSize
///
/////////////////////////////////////////////////////
bool OpenGL::CubeInFrustum( GLfloat fX, GLfloat fY, GLfloat fZ, GLfloat fSize )
{
	for(GLint i = 0; i < 6; i++ )
	{
		if(m_Frustum[i][0] * (fX - fSize) + m_Frustum[i][1] * (fY - fSize) + m_Frustum[i][2] * (fZ - fSize) + m_Frustum[i][3] > 0)
		   continue;
		if(m_Frustum[i][0] * (fX + fSize) + m_Frustum[i][1] * (fY - fSize) + m_Frustum[i][2] * (fZ - fSize) + m_Frustum[i][3] > 0)
		   continue;
		if(m_Frustum[i][0] * (fX - fSize) + m_Frustum[i][1] * (fY + fSize) + m_Frustum[i][2] * (fZ - fSize) + m_Frustum[i][3] > 0)
		   continue;
		if(m_Frustum[i][0] * (fX + fSize) + m_Frustum[i][1] * (fY + fSize) + m_Frustum[i][2] * (fZ - fSize) + m_Frustum[i][3] > 0)
		   continue;
		if(m_Frustum[i][0] * (fX - fSize) + m_Frustum[i][1] * (fY - fSize) + m_Frustum[i][2] * (fZ + fSize) + m_Frustum[i][3] > 0)
		   continue;
		if(m_Frustum[i][0] * (fX + fSize) + m_Frustum[i][1] * (fY - fSize) + m_Frustum[i][2] * (fZ + fSize) + m_Frustum[i][3] > 0)
		   continue;
		if(m_Frustum[i][0] * (fX - fSize) + m_Frustum[i][1] * (fY + fSize) + m_Frustum[i][2] * (fZ + fSize) + m_Frustum[i][3] > 0)
		   continue;
		if(m_Frustum[i][0] * (fX + fSize) + m_Frustum[i][1] * (fY + fSize) + m_Frustum[i][2] * (fZ + fSize) + m_Frustum[i][3] > 0)
		   continue;

		// If we get here, it isn't in the frustum
		return false;
	}

	// check for errors
	GL_CHECK

	return true;
}

/////////////////////////////////////////////////////
/// Method: AABBInFrustum
/// Params: [in]aabb
///
/////////////////////////////////////////////////////
bool OpenGL::AABBInFrustum( const collision::AABB &aabb )
{
	for(GLint i = 0; i < 6; i++ )
	{
		if(m_Frustum[i][0] * (aabb.vBoxMin.X) + m_Frustum[i][1] * (aabb.vBoxMin.Y) + m_Frustum[i][2] * (aabb.vBoxMin.Z) + m_Frustum[i][3] > 0)
		   continue;
		if(m_Frustum[i][0] * (aabb.vBoxMax.X) + m_Frustum[i][1] * (aabb.vBoxMin.Y) + m_Frustum[i][2] * (aabb.vBoxMin.Z) + m_Frustum[i][3] > 0)
		   continue;
		if(m_Frustum[i][0] * (aabb.vBoxMin.X) + m_Frustum[i][1] * (aabb.vBoxMax.Y) + m_Frustum[i][2] * (aabb.vBoxMin.Z) + m_Frustum[i][3] > 0)
		   continue;
		if(m_Frustum[i][0] * (aabb.vBoxMax.X) + m_Frustum[i][1] * (aabb.vBoxMax.Y) + m_Frustum[i][2] * (aabb.vBoxMin.Z) + m_Frustum[i][3] > 0)
		   continue;
		if(m_Frustum[i][0] * (aabb.vBoxMin.X) + m_Frustum[i][1] * (aabb.vBoxMin.Y) + m_Frustum[i][2] * (aabb.vBoxMax.Z) + m_Frustum[i][3] > 0)
		   continue;
		if(m_Frustum[i][0] * (aabb.vBoxMax.X) + m_Frustum[i][1] * (aabb.vBoxMin.Y) + m_Frustum[i][2] * (aabb.vBoxMax.Z) + m_Frustum[i][3] > 0)
		   continue;
		if(m_Frustum[i][0] * (aabb.vBoxMin.X) + m_Frustum[i][1] * (aabb.vBoxMax.Y) + m_Frustum[i][2] * (aabb.vBoxMax.Z) + m_Frustum[i][3] > 0)
		   continue;
		if(m_Frustum[i][0] * (aabb.vBoxMax.X) + m_Frustum[i][1] * (aabb.vBoxMax.Y) + m_Frustum[i][2] * (aabb.vBoxMax.Z) + m_Frustum[i][3] > 0)
		   continue;

		// If we get here, it isn't in the frustum
		return false;
	}

	// check for errors
	GL_CHECK

	return true;
}

/////////////////////////////////////////////////////
/// Method: Screenshot
/// Params: None
///
/////////////////////////////////////////////////////
void OpenGL::Screenshot( void )
{
/*	static GLint nIndex = 0;
	static GLfloat fTimeSinceLast = 0.0f;
	Texture screenTexture = Texture();
	char szOutFile[_MAX_PATH+_MAX_PATH];

	if( fTimeSinceLast > 1.0f )
	{
		std::memset( szOutFile, 0, sizeof(char)*_MAX_PATH+_MAX_PATH );

		// setup the storage
		screenTexture.nFormat = GL_RGBA;
		screenTexture.nBPP = 32;
		screenTexture.nWidth = m_Width;
		screenTexture.nHeight = m_Height;

		// allocate
		DBG_MEMTRY
			screenTexture.image = new unsigned char[(m_Width*m_Height)*(screenTexture.nBPP/8)];
		DBG_MEMCATCH

		glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

		// get the screen
		glReadPixels( 0, 0, m_Width, m_Height, GL_RGBA, GL_UNSIGNED_BYTE, screenTexture.image );

		// create a directory if it doesn't exist
		core::file::CreateDirectory( "Screenshots" );

		// save the file
		std::sprintf( szOutFile, "Screenshots/image%d.png", nIndex );
		screenTexture.SavePNG( szOutFile );

		// clear the image
		screenTexture.Free();

		// increase image index
		nIndex++;

		fTimeSinceLast = 0.0f;
	}

	fTimeSinceLast += ::gFrameDelta;

#ifdef _DEBUG
	// check for errors
	GL_CHECK
#endif // _DEBUG
*/
}

/////////////////////////////////////////////////////
/// Method: GetDepthMode
/// Params: [out]bDepthFlag, [out]eDepthFunc
///
/////////////////////////////////////////////////////
void OpenGL::GetDepthMode( bool *bDepthFlag, GLenum *eDepthFunc )
{
	if( bDepthFlag )
		*bDepthFlag = m_GLState.bDepthState;
	if( eDepthFunc )
		*eDepthFunc = m_GLState.eDepthFunc;

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: GetBlendMode
/// Params: [out]bBlendFlag, [out]eBlendSrc, [out]eBlendDest
///
/////////////////////////////////////////////////////
void OpenGL::GetBlendMode( bool *bBlendFlag, GLenum *eBlendSrc, GLenum *eBlendDest )
{
	if( bBlendFlag )
		*bBlendFlag = m_GLState.bBlendState;
	if( eBlendSrc )
		*eBlendSrc = m_GLState.eBlendSrc;
	if( eBlendDest )
		*eBlendDest = m_GLState.eBlendDest;

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: GetAlphaMode
/// Params: [out]bAlphaFlag, [out]eAlphaFunc, [out]fAlphaClamp
///
/////////////////////////////////////////////////////
void OpenGL::GetAlphaMode( bool *bAlphaFlag, GLenum *eAlphaFunc, GLfloat *fAlphaClamp )
{
	if( bAlphaFlag )
		*bAlphaFlag = m_GLState.bAlphaState;
	if( eAlphaFunc )
		*eAlphaFunc = m_GLState.eAlphaFunc;
	if( fAlphaClamp )
		*fAlphaClamp = m_GLState.fAlphaClamp;

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: GetWidthHeight
/// Params: [out]nWidth, [out]nHeight
///
/////////////////////////////////////////////////////
void OpenGL::GetWidthHeight( GLint *nWidth, GLint *nHeight )
{
	if( nWidth )
		*nWidth = m_Width;

	if( nHeight )
		*nHeight = m_Height;

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: GetClearColour
/// Params: [out]fRed, [out]fGreen, [out]fBlue
///
/////////////////////////////////////////////////////
void OpenGL::GetClearColour( GLfloat *fRed, GLfloat *fGreen, GLfloat *fBlue )
{
	if( fRed )
	{
		*fRed = m_vClearColour.R;
	}
	if( fGreen )
	{
		*fGreen = m_vClearColour.G;
	}
	if( fBlue )
	{
		*fBlue = m_vClearColour.B;
	}

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: GetLookAt
/// Params: [out]vEye, [out]vCenter
///
/////////////////////////////////////////////////////
void OpenGL::GetLookAt( math::Vec3 &vEye, math::Vec3 &vCenter )
{
	vEye.X = m_vLookAtEye.X;
	vEye.Y = m_vLookAtEye.Y;
	vEye.Z = m_vLookAtEye.Z;

	vCenter.X = m_vLookAtCenter.X;
	vCenter.Y = m_vLookAtCenter.Y;
	vCenter.Z = m_vLookAtCenter.Z;

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: GetFOVNearFarClip
/// Params: [out]fFOV, [out]fNearClip, [out]fFarClip
///
/////////////////////////////////////////////////////
void OpenGL::GetFOVNearFarClip( GLfloat *fFOV, GLfloat *fNearClip, GLfloat *fFarClip )
{
	if( fFOV )
	{
		*fFOV = m_FOV;
	}
	if( fNearClip )
	{
		*fNearClip = m_NearClip;
	}
	if( fFarClip )
	{
		*fFarClip = m_FarClip;
	}

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: GetActiveTexture
/// Params: [in]nTexUnit, [in]eTarget
///
/////////////////////////////////////////////////////
GLuint OpenGL::GetActiveTexture( GLint nTexUnit, GLenum eTarget )
{
	// make sure it's a valid unit
	if( nTexUnit >= 0 && nTexUnit < m_MaxTextureUnits )
	{
		if( eTarget == GL_TEXTURE_2D )
		{					
			return( m_GLState.TexUnits[nTexUnit].nCurrent2DTexture );
		}
	}

	// check for errors
	GL_CHECK

	return( INVALID_OBJECT );
}

/////////////////////////////////////////////////////
/// Method: GetActiveFrameBuffer
/// Params: None
///
/////////////////////////////////////////////////////
GLuint OpenGL::GetActiveFrameBuffer( void )
{
	if( bExtFrameBufferObject )
	{
		return( m_GLState.nCurrentFrameBuffer );
	}

	// check for errors
	GL_CHECK

	return( INVALID_OBJECT );
}

/////////////////////////////////////////////////////
/// Method: GetAmbientColour
/// Params: [out]fAmbientRed, [out]fAmbientGreen, [out]fAmbientBlue
///
/////////////////////////////////////////////////////
void OpenGL::GetAmbientColour( GLfloat *fAmbientRed, GLfloat *fAmbientGreen, GLfloat *fAmbientBlue )
{
	if( fAmbientRed )
	{
		*fAmbientRed = m_vAmbientColour.R;
	}
	if( fAmbientGreen )
	{
		*fAmbientGreen = m_vAmbientColour.G;
	}
	if( fAmbientBlue )
	{
		*fAmbientBlue = m_vAmbientColour.B;
	}

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: GetVolumetricFogFlag
/// Params: [out]nFlag
///
/////////////////////////////////////////////////////
void OpenGL::GetVolumetricFogFlag( GLint *nFlag )
{
	if( nFlag )
	{
		*nFlag = m_VolumetricFogEnabled;
	}

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: GetActiveTextureFilter
/// Params: [out]eMinFilter, [out]eMagFilter
///
/////////////////////////////////////////////////////
void OpenGL::GetActiveTextureFilter( GLenum *eMinFilter, GLenum *eMagFilter )
{
	if( eMinFilter )
	{
		*eMinFilter = m_eTextureMinFilter;
	}
	if( eMagFilter )
	{
		*eMagFilter = m_eTextureMagFilter;
	}

	// check for errors
	GL_CHECK
}

#endif // BASE_SUPPORT_OPENGLES








