
/*===================================================================
	File: OpenGL.cpp
	Library: Render

	(C)Hidden Games
=====================================================================*/

#ifdef BASE_SUPPORT_OPENGL

#include "CoreBase.h"

#include <cmath>

#include "Math/Vectors.h"
#include "Collision/AABB.h"

#include "Render/RenderConsts.h"
#include "Render/OpenGLCommon.h"
#include "Render/FF/OpenGL/Extensions.h"
#include "Render/FF/OpenGL/OpenGL.h"

#include "Render/TextureShared.h"
#include "Render/Texture.h"
#include "Render/FF/OpenGL/TextureLoadAndUpload.h"
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

	const int GLU_VERSION_SIZE = 64;
	const int GLU_EXTENSIONS_SIZE = 4096;
	
	math::Vec3 zeroVec3( 0.0f, 0.0f, 0.0f );

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
	m_GLState.bVertexArrayState		= false;
	m_GLState.bTextureArrayState	= false;
	m_GLState.bColourArrayState		= false;
	m_GLState.bNormalArrayState		= false;
	m_GLState.bVBOActive			= false;

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
	m_HasGLVersion1_2 = false;
	m_HasGLVersion1_3 = false;
	m_HasGLVersion1_4 = false;
	m_HasGLVersion1_5 = false;
	m_HasGLVersion2_0 = false;
	m_HasGLVersion2_1 = false;

	m_Width	= 640;
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

#ifdef BASE_PLATFORM_WINDOWS
	m_hGLRC = 0;
#endif // BASE_PLATFORM_WINDOWS
}

/////////////////////////////////////////////////////
/// Default destructor
/// 
///
/////////////////////////////////////////////////////
OpenGL::~OpenGL( )
{
	Release();
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
	m_GLState.bVertexArrayState		= false;
	m_GLState.bTextureArrayState	= false;
	m_GLState.bColourArrayState		= false;
	m_GLState.bNormalArrayState		= false;
	m_GLState.bVBOActive			= false;
    
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
	m_HasGLVersion1_2 = false;
	m_HasGLVersion1_3 = false;
	m_HasGLVersion1_4 = false;
	m_HasGLVersion1_5 = false;
	m_HasGLVersion2_0 = false;
	m_HasGLVersion2_1 = false;
    
	m_Width	= 640;
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
    
	m_AmbientLighting = false;
    
	m_vAmbientColour = math::Vec4( 0.0f, 0.0f, 0.0f, 1.0f );
    
	// bilinear default texture filter
	m_TextureCompressFlag = false;
	m_eTextureMinFilter = GL_LINEAR;
	m_eTextureMagFilter = GL_LINEAR;
    
    
    // reset
	glHint(	GL_FOG_HINT, GL_NICEST );
	glHint(	GL_GENERATE_MIPMAP_HINT, GL_NICEST );
	glHint(	GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
	glHint(	GL_LINE_SMOOTH_HINT, GL_FASTEST );
	glHint(	GL_POINT_SMOOTH_HINT, GL_FASTEST );
    
	// now do some simple OpenGL initialisation
	glClearAccum( 0.0f, 0.0f, 0.0f, 1.0f );
	glClearColor( m_vClearColour.R, m_vClearColour.G, m_vClearColour.B, m_vClearColour.A );
	//SetClearBits( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0f);
    
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
	if( bExtMultiTexture )
		glActiveTexture( GL_TEXTURE0 );
    
	glBindTexture( GL_TEXTURE_2D, 0 );
    
	//SetNearFarClip( 1.0f, 10000.0f );
	//SetFieldOfView( 60.0f );
    
	EnableVertexArray();
    
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	//SetFillMode( GL_FILL );
    
	glDepthMask( GL_TRUE );
	glDepthFunc( GL_LESS );
	DepthMode( true, GL_LESS );
    
	glDisable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	//BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    
	glDisable( GL_ALPHA_TEST );
	glAlphaFunc( GL_ALWAYS, 0.0f );
	//AlphaMode( false, GL_ALWAYS, 0.0f );
    
	//SetAmbientFlag( false );
	//SetAmbientColour( 1.0f, 1.0f, 1.0f, 1.0f );
    
	//FogMode( false, GL_EXP, zeroVec3, 0.0f, 1.0f, 1.0f );
    
	glDisable( GL_COLOR_MATERIAL );
	glColorMaterial( GL_FRONT, GL_AMBIENT_AND_DIFFUSE );
    
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

	char szGLUVersion[renderer::GLU_VERSION_SIZE];
	char szGLUExtensions[renderer::GLU_EXTENSIONS_SIZE];
	
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

	//core::WideString gluVersionName( reinterpret_cast<const char*>( gluGetString( GLU_VERSION ) ) );
	snprintf( szGLUVersion, renderer::GLU_VERSION_SIZE, "Version: %s\n", gluGetString( GLU_VERSION ) );
	//core::WideString gluExtensionsName( reinterpret_cast<const char*>( gluGetString( GLU_EXTENSIONS ) ) );
	snprintf( szGLUExtensions, renderer::GLU_EXTENSIONS_SIZE, "extensions: %s\n", gluGetString( GLU_EXTENSIONS ) );

	DBGLOG( "GLU info\n" );
	DBGLOG( "--------------------------------------\n" );
	DBGLOG( "%s", szGLUVersion );
	DBGLOG( "--------------------------------------\n" );
	DBGLOG( "%s", szGLUExtensions );
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
	glClearAccum( 0.0f, 0.0f, 0.0f, 1.0f ); 
	glClearColor( m_vClearColour.R, m_vClearColour.G, m_vClearColour.B, m_vClearColour.A );		
	SetClearBits( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glShadeModel(GL_SMOOTH);					
	glClearDepth(1.0f);

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
	if( bExtMultiTexture )
		glActiveTexture( GL_TEXTURE0 );

	glBindTexture( GL_TEXTURE_2D, 0 );

	SetNearFarClip( 1.0f, 10000.0f );
	SetFieldOfView( 60.0f );

	EnableVertexArray();

	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	SetFillMode( GL_FILL );

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

	FogMode( false, GL_EXP, zeroVec3, 0.0f, 1.0f, 1.0f );

	glDisable( GL_COLOR_MATERIAL );
	glColorMaterial( GL_FRONT, GL_AMBIENT_AND_DIFFUSE );

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

#ifdef _DEBUG
	// debug texture
	const int MISSING_TEX_SIZE = 16;

	// make the texture the current one			
	BindUnitTexture( 0, GL_TEXTURE_2D, renderer::INVALID_OBJECT );

	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	int j,k;
	unsigned char tmpImage[MISSING_TEX_SIZE][MISSING_TEX_SIZE][3]; 
	for( j=0; j < MISSING_TEX_SIZE; ++j )
	{
		for( k=0; k < MISSING_TEX_SIZE; ++k )
		{
			int c = (((j&0x4)==0)^((k&0x4)==0))*255;
			tmpImage[j][k][_R_] = static_cast<GLubyte>(c);
			tmpImage[j][k][_G_] = static_cast<GLubyte>(c);
			tmpImage[j][k][_B_] = static_cast<GLubyte>(c);
		}
	}

	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, MISSING_TEX_SIZE, MISSING_TEX_SIZE, 0, GL_RGB, GL_UNSIGNED_BYTE, tmpImage );
#endif // _DEBUG

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

#ifdef BASE_PLATFORM_WINDOWS
	if( m_hGLRC != 0 )
	{
		if( ChangeDisplaySettings(0, 0) != DISP_CHANGE_SUCCESSFUL )
		{
			DBGLOG( "OPENGL: *ERROR* could not change GL display settings during shutdown\n" );
			return;
		}

		if( wglMakeCurrent( 0, 0 ) == false )
		{
			DBGLOG( "OPENGL: *ERROR* could not change GL context during shutdown\n" );
			return;
		}

		if( wglDeleteContext( m_hGLRC ) == false )
		{
			DBGLOG( "OPENGL: *ERROR* could not delete GL context during shutdown\n" );
			return;
		}
	}
#endif // BASE_PLATFORM_WINDOWS

	DBGLOG( "----- OpenGL Shutdown Complete-----\n" );

}

#ifdef BASE_PLATFORM_WINDOWS

/////////////////////////////////////////////////////
/// Method: SetupDisplay
/// Params: [in]hDC
///
/////////////////////////////////////////////////////
void OpenGL::SetupDisplay( HDC hDC, GLint nBPP, GLint nZBuffer )
{
	DBGLOG( "----- OpenGL SetupDisplay -----\n" );

	// get a DC for client area
	if( hDC )
		m_hDC = hDC;

	if( m_hDC == 0 )
	{
		// did not get a device context
		DBGLOG( "OPENGL: *ERROR* failed to Get a Device Context for the Client Area\n" );
		return;
	}

	if( SetupPixelFormat( nBPP, nZBuffer ) != 0 )
	{
		// failed to setup the pixelformat
		DBGLOG( "OPENGL: *ERROR* failed to setup the PixelFormat\n" );
		return;
	}

	// now create a GL rendering context
	m_hGLRC = wglCreateContext( m_hDC );

	if( m_hGLRC == 0 )
	{
		DBGLOG( "OPENGL: *ERROR* failed to get a GL Rendering Context\n" );
		return;
	}

	// try and set GL rendering context to current device context
	if( wglMakeCurrent( m_hDC, m_hGLRC ) == false )
	{
		DBGLOG( "OPENGL: *ERROR* failed to set GL rendering context to current device context\n" );
		return;
	}

	DBGLOG( "----- OpenGL SetupDisplay Complete -----\n" );

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: SetupPixelFormat
/// Params: None
///
/////////////////////////////////////////////////////
GLint OpenGL::SetupPixelFormat( GLint nBPP, GLint nZbuffer )
{
	DBGLOG( "----- OpenGL SetupPixelFormat -----\n" );

	enum EGLFlags 
	{ 
		GL_FLAG_RGB			= 0,
		GL_FLAG_INDEX		= 1,
		GL_FLAG_SINGLE		= 0,
		GL_FLAG_DOUBLE		= 2,
		GL_FLAG_ACCUM		= 4,
		GL_FLAG_ALPHA		= 8,
		GL_FLAG_DEPTH		= 16,
		GL_FLAG_STENCIL		= 32,
		GL_FLAG_RGB8		= 64,
		GL_FLAG_MULTISAMPLE	= 128,
		GL_FLAG_STEREO		= 256,
		GL_FLAG_FAKE_SINGLE	= 512	// Fake single buffered windows using double-buffer
	};

	m_PixelFormat = 0;
	GLint m = GL_FLAG_RGB | GL_FLAG_DEPTH | GL_FLAG_DOUBLE | GL_FLAG_ACCUM | GL_FLAG_ALPHA;
	
	PIXELFORMATDESCRIPTOR chosen_pfd;
	PIXELFORMATDESCRIPTOR pfd;
	
	std::memset( &chosen_pfd, 0, sizeof(PIXELFORMATDESCRIPTOR) );
	std::memset( &pfd, 0, sizeof(PIXELFORMATDESCRIPTOR) );

	for (GLint i = 1; ; i++) 
	{
		// keep going through all pixel formats until DescribePixelFormat flags an error
		if (!DescribePixelFormat( m_hDC, i, sizeof(pfd), &pfd) ) 
			break;

		//DBGLOG( "Pixel Format[%d]: AlphaBits (%d), AccumBits (%d), DepthBits (%d), StencilBits (%d) Hardware (%s)\n", i, pfd.cAlphaBits, pfd.cAccumBits, pfd.cDepthBits, pfd.cStencilBits, (pfd.dwFlags & PFD_GENERIC_FORMAT) ? "N" : "Y" );

		// continue if it does not satisfy our requirements:
		if (~pfd.dwFlags & (PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL)) 
			continue;
		if (pfd.iPixelType != ((m&GL_FLAG_INDEX)?PFD_TYPE_COLORINDEX:PFD_TYPE_RGBA)) 
			continue;
		if ((m & GL_FLAG_ALPHA) && !pfd.cAlphaBits) 
			continue;
		if ((m & GL_FLAG_ACCUM) && !pfd.cAccumBits) 
			continue;
		if ((!(m & GL_FLAG_DOUBLE)) != (!(pfd.dwFlags & PFD_DOUBLEBUFFER))) 
			continue;
		if ((!(m & GL_FLAG_STEREO)) != (!(pfd.dwFlags & PFD_STEREO))) 
			continue;
		if ((m & GL_FLAG_DEPTH) && !pfd.cDepthBits) 
			continue;
		if ((m & GL_FLAG_STENCIL) && !pfd.cStencilBits) 
			continue;
		
		// see if better than the one we have already:
		if( m_PixelFormat ) 
		{
			// offering non-generic rendering is better (read: hardware acceleration)
			if (!(chosen_pfd.dwFlags & PFD_GENERIC_FORMAT) &&
				(pfd.dwFlags & PFD_GENERIC_FORMAT)) 
				continue;
			// offering overlay is better:
			else if (!(chosen_pfd.bReserved & 15) && (pfd.bReserved & 15)) {}
			// otherwise more bit planes is better:
			else if (chosen_pfd.cColorBits > pfd.cColorBits && pfd.cColorBits == nBPP ) 
				continue;
			else if (chosen_pfd.cDepthBits > pfd.cDepthBits && pfd.cColorBits == nZbuffer ) 
				continue;
		}
		m_PixelFormat = i;
		chosen_pfd = pfd;
	}
	
	DBGLOG( "Chosen pixel format is %d\n", m_PixelFormat );
	
	if(!m_PixelFormat) 
		return(1);

	SetPixelFormat( m_hDC, m_PixelFormat, &chosen_pfd );

	DBGLOG( "----- OpenGL SetupPixelFormat Complete -----\n" );

	// pixel format 0, save the bpp value
	m_BPP = chosen_pfd.cColorBits;
	m_ZBuffer = chosen_pfd.cDepthBits;
	m_AccumBits = chosen_pfd.cAccumBits;
	m_AlphaBits = chosen_pfd.cAlphaBits;

	DBGLOG( "Colour Bits: %d, ZBuffer: %d\n", m_BPP, m_ZBuffer );
	DBGLOG( "Alpha Bits: %d, Accum Bits: %d\n", m_AlphaBits, m_AccumBits );

	return(0);
}

#endif // BASE_PLATFORM_WINDOWS

/////////////////////////////////////////////////////
/// Method: SetGLContext
/// Params: [in]pContext
///
/////////////////////////////////////////////////////
void OpenGL::SetGLContext( void* pContext )
{
#ifdef BASE_PLATFORM_WINDOWS
	m_hGLRC = *((HGLRC *)pContext);
#endif // BASE_PLATFORM_WINDOWS

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
	if( bExtMultiTexture )
	{
		for( i = 0; i < m_MaxTextureUnits; i++ )
		{
			GLenum eActiveTexUnit = GL_TEXTURE0 + i;

			// make sure multitexturing is allowed
            if( i != 0 )
                glActiveTexture( eActiveTexUnit );

			if( m_GLState.TexUnits[i].nCurrentCubemapTexture != INVALID_OBJECT && m_GLState.TexUnits[i].nCurrentCubemapTexture != 0 )
			{
				if( bExtTextureCubeMap )
				{
					glEnable( GL_TEXTURE_CUBE_MAP );
					glBindTexture( GL_TEXTURE_CUBE_MAP, m_GLState.TexUnits[i].nCurrentCubemapTexture );
				}
			}
			else
			{
				if( bExtTextureCubeMap )
				{
					glDisable( GL_TEXTURE_CUBE_MAP );
				}
			}

			if( m_GLState.TexUnits[i].nCurrentRectangleTexture != INVALID_OBJECT && m_GLState.TexUnits[i].nCurrentRectangleTexture != 0 )
			{
				if( bExtTextureRectangle )
				{
					glEnable( GL_TEXTURE_RECTANGLE_ARB );
					glBindTexture( GL_TEXTURE_RECTANGLE_ARB, m_GLState.TexUnits[i].nCurrentRectangleTexture );
				}
			}
			else
			{
				if( bExtTextureRectangle )
				{
					glDisable( GL_TEXTURE_RECTANGLE_ARB );
				}
			}

			if( m_GLState.TexUnits[i].nCurrent1DTexture != INVALID_OBJECT && m_GLState.TexUnits[i].nCurrent1DTexture != 0 )
			{
				glEnable( GL_TEXTURE_1D );
				glBindTexture( GL_TEXTURE_1D, m_GLState.TexUnits[i].nCurrent1DTexture );
			}
			else
			{
				glDisable( GL_TEXTURE_1D );
			}

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
	}
	else
	{
		// use only unit 0
		i = 0;

		if( m_GLState.TexUnits[i].nCurrentCubemapTexture != INVALID_OBJECT && m_GLState.TexUnits[i].nCurrentCubemapTexture != 0 )
		{
			if( bExtTextureCubeMap )
			{
				glEnable( GL_TEXTURE_CUBE_MAP );
				glBindTexture( GL_TEXTURE_CUBE_MAP, m_GLState.TexUnits[i].nCurrentCubemapTexture );
			}
		}
		else
		{
			if( bExtTextureCubeMap )
			{
				glDisable( GL_TEXTURE_CUBE_MAP );
			}
		}

		if( m_GLState.TexUnits[i].nCurrentRectangleTexture != INVALID_OBJECT && m_GLState.TexUnits[i].nCurrentRectangleTexture != 0 )
		{
			if( bExtTextureRectangle )
			{
				glEnable( GL_TEXTURE_RECTANGLE_ARB );
				glBindTexture( GL_TEXTURE_RECTANGLE_ARB, m_GLState.TexUnits[i].nCurrentRectangleTexture );
			}
		}
		else
		{
			if( bExtTextureRectangle )
			{
				glDisable( GL_TEXTURE_RECTANGLE_ARB );
			}
		}

		if( m_GLState.TexUnits[i].nCurrent1DTexture != INVALID_OBJECT && m_GLState.TexUnits[i].nCurrent1DTexture != 0 )
		{
			glEnable( GL_TEXTURE_1D );
			glBindTexture( GL_TEXTURE_1D, m_GLState.TexUnits[i].nCurrent1DTexture );
		}
		else
		{
			glDisable( GL_TEXTURE_1D );
		}

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
	if( bExtMultiTexture )
	{
		for( i = 0; i < m_MaxTextureUnits; i++ )
		{
			GLenum eActiveTexUnit = GL_TEXTURE0 + i;

			// make sure multitexturing is allowed
			glActiveTexture( eActiveTexUnit );
			
			if( bExtTextureCubeMap )
				glDisable( GL_TEXTURE_CUBE_MAP );
			if( bExtTextureRectangle )
				glDisable( GL_TEXTURE_RECTANGLE_ARB );		

			glDisable( GL_TEXTURE_1D );
			glDisable( GL_TEXTURE_2D );
		}
	}
	else
	{
		if( bExtTextureCubeMap )
			glDisable( GL_TEXTURE_CUBE_MAP );
		if( bExtTextureRectangle )
			glDisable( GL_TEXTURE_RECTANGLE_ARB );

		glDisable( GL_TEXTURE_1D );
		glDisable( GL_TEXTURE_2D );
	}
	
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
	if( eFillMode != GL_LINE && eFillMode != GL_FILL )
		return;

	if( eFillMode == GL_LINE )
	{
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		m_GLState.bFillMode = false;
	}
	else if( eFillMode == GL_FILL )
	{
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		m_GLState.bFillMode = true;
	}

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
			glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, nFrameBufferID );
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
	if( bExtMultiTexture )
	{
		for( i = 0; i < m_MaxTextureUnits; i++ )
		{
			GLenum eActiveTexUnit = GL_TEXTURE0 + i;

			// make sure multitexturing is allowed
			glActiveTexture( eActiveTexUnit );

			if( bExtTextureCubeMap )
			{
				glBindTexture( GL_TEXTURE_CUBE_MAP, 0 );
				m_GLState.TexUnits[i].nCurrentCubemapTexture = 0;
			}

			if( bExtTextureRectangle )
			{
				glBindTexture( GL_TEXTURE_RECTANGLE_ARB, 0 );
				m_GLState.TexUnits[i].nCurrentRectangleTexture = 0;
			}

			glBindTexture( GL_TEXTURE_1D, 0 );
			m_GLState.TexUnits[i].nCurrent1DTexture = 0;

			glBindTexture( GL_TEXTURE_2D, 0 );
			m_GLState.TexUnits[i].nCurrent2DTexture = 0;

			m_GLState.nCurrentTexture = 0;
		}
	}
	else
	{
		if( bExtTextureCubeMap )
			glBindTexture( GL_TEXTURE_CUBE_MAP, 0 );

		if( bExtTextureRectangle )
			glBindTexture( GL_TEXTURE_RECTANGLE_ARB, 0 );

		glBindTexture( GL_TEXTURE_1D, 0 );

		glBindTexture( GL_TEXTURE_2D, 0 );

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
		// make sure multitexturing is allowed, or if it's only unit 0 allow it
		if( bExtMultiTexture || nTexUnit == 0 )
		{
			// which unit are we working on
			GLenum eActiveTexUnit = GL_TEXTURE0 + nTexUnit;

			if( bExtMultiTexture )
				glActiveTexture( eActiveTexUnit );	

			if( bExtTextureCubeMap )
			{
				glBindTexture( GL_TEXTURE_CUBE_MAP, 0 );
				m_GLState.TexUnits[nTexUnit].nCurrentCubemapTexture = 0;
			}

			if( bExtTextureRectangle )
			{
				glBindTexture( GL_TEXTURE_RECTANGLE_ARB, 0 );
				m_GLState.TexUnits[nTexUnit].nCurrentRectangleTexture = 0;
			}

			glBindTexture( GL_TEXTURE_1D, 0 );
			m_GLState.TexUnits[nTexUnit].nCurrent1DTexture = 0;

			glBindTexture( GL_TEXTURE_2D, 0 );
			m_GLState.TexUnits[nTexUnit].nCurrent2DTexture = 0;

			m_GLState.nCurrentTexture = 0;
		}
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
		// make sure multitexturing is allowed, or if it's only unit 0 allow it
		if( bExtMultiTexture || nTexUnit == 0 )
		{
			// which unit are we working on
			GLenum eActiveTexUnit = GL_TEXTURE0 + nTexUnit;

			if( bExtMultiTexture )
				glActiveTexture( eActiveTexUnit );	

			if( bExtTextureCubeMap )
				glDisable( GL_TEXTURE_CUBE_MAP );

			if( bExtTextureRectangle )
				glDisable( GL_TEXTURE_RECTANGLE_ARB );

			glDisable( GL_TEXTURE_1D );
			glDisable( GL_TEXTURE_2D );
		}
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
		glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
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
		// make sure multitexturing is allowed, or if it's only unit 0 allow it
		if( bExtMultiTexture || nTexUnit == 0 )
		{
			// which unit are we working on
			GLenum eActiveTexUnit = GL_TEXTURE0 + nTexUnit;

			if( bExtMultiTexture )
				glActiveTexture( eActiveTexUnit );	

			// 1D texture
			if( eTarget == GL_TEXTURE_1D )
			{
				// magnification filter
				if( eParameter == GL_TEXTURE_MAG_FILTER )
				{
					if( m_GLState.TexUnits[nTexUnit].fTex1DMagFilter != fValue )
					{
						glTexParameterf( eTarget, eParameter, fValue );
						m_GLState.TexUnits[nTexUnit].fTex1DMagFilter = (GLfloat)fValue;
					}
				}
				else if( eParameter == GL_TEXTURE_MIN_FILTER )
				{
					// minification filter
					if( m_GLState.TexUnits[nTexUnit].fTex1DMinFilter != fValue )
					{
						glTexParameterf( eTarget, eParameter, fValue );
						m_GLState.TexUnits[nTexUnit].fTex1DMinFilter = (GLfloat)fValue;
					}
				}
			}
			else if( eTarget == GL_TEXTURE_2D )
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
			else if( eTarget == GL_TEXTURE_RECTANGLE_ARB )
			{
				if( bExtTextureRectangle )
				{
					// magnification filter
					if( eParameter == GL_TEXTURE_MAG_FILTER )
					{
						if( m_GLState.TexUnits[nTexUnit].fTexRectangleMagFilter != fValue )
						{
							glTexParameterf( eTarget, eParameter, fValue );
							m_GLState.TexUnits[nTexUnit].fTexRectangleMagFilter = (GLfloat)fValue;
						}
					}
					else if( eParameter == GL_TEXTURE_MIN_FILTER )
					{
						// minification filter
						if( m_GLState.TexUnits[nTexUnit].fTexRectangleMinFilter != fValue )
						{
							glTexParameterf( eTarget, eParameter, fValue );
							m_GLState.TexUnits[nTexUnit].fTexRectangleMinFilter = (GLfloat)fValue;
						}
					}
				}
			}
			else if( eTarget == GL_TEXTURE_CUBE_MAP )
			{
				if( bExtTextureCubeMap )
				{
					// magnification filter
					if( eParameter == GL_TEXTURE_MAG_FILTER )
					{
						if( m_GLState.TexUnits[nTexUnit].fTexCubemapMagFilter != fValue )
						{
							glTexParameterf( eTarget, eParameter, fValue );
							m_GLState.TexUnits[nTexUnit].fTexCubemapMagFilter = (GLfloat)fValue;
						}
					}
					else if( eParameter == GL_TEXTURE_MIN_FILTER )
					{
						// minification filter
						if( m_GLState.TexUnits[nTexUnit].fTexCubemapMinFilter != fValue )
						{
							glTexParameterf( eTarget, eParameter, fValue );
							m_GLState.TexUnits[nTexUnit].fTexCubemapMinFilter = (GLfloat)fValue;
						}
					}
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
	// make sure it's a valid unit
	if( nTexUnit >= 0 && nTexUnit < m_MaxTextureUnits )
	{
		// make sure lod controls are valid and if multitexturing is allowed, switch to correct active texture
		if( bExtTextureLod && (bExtMultiTexture || nTexUnit == 0) )
		{
			// which unit are we working on
			GLenum eActiveTexUnit = GL_TEXTURE0 + nTexUnit;

			if( bExtMultiTexture )
				glActiveTexture( eActiveTexUnit );	

			// 1D texture
			if( eTarget == GL_TEXTURE_1D )
			{
				// min lod
				if( eParameter == GL_TEXTURE_MIN_LOD )
				{
					if( m_GLState.TexUnits[nTexUnit].fTex1DMinLod != fValue )
					{
						glTexParameterf( eTarget, eParameter, fValue );
						m_GLState.TexUnits[nTexUnit].fTex1DMinLod = (GLfloat)fValue;
					}
				}
				else if( eParameter == GL_TEXTURE_MAX_LOD )
				{
					// max lod
					if( m_GLState.TexUnits[nTexUnit].fTex1DMaxLod != fValue )
					{
						glTexParameterf( eTarget, eParameter, fValue );
						m_GLState.TexUnits[nTexUnit].fTex1DMaxLod = (GLfloat)fValue;
					}
				}
				else if( eParameter == GL_TEXTURE_BASE_LEVEL )
				{
					// base level
					if( m_GLState.TexUnits[nTexUnit].fTex1DBaseLevel != fValue )
					{
						glTexParameterf( eTarget, eParameter, fValue );
						m_GLState.TexUnits[nTexUnit].fTex1DBaseLevel = (GLfloat)fValue;
					}
				}
				else if( eParameter == GL_TEXTURE_MAX_LEVEL )
				{
					// max level
					if( m_GLState.TexUnits[nTexUnit].fTex1DMaxLevel != fValue )
					{
						glTexParameterf( eTarget, eParameter, fValue );
						m_GLState.TexUnits[nTexUnit].fTex1DMaxLevel = (GLfloat)fValue;
					}
				}
			}
			else if( eTarget == GL_TEXTURE_2D )
			{
				// min lod
				if( eParameter == GL_TEXTURE_MIN_LOD )
				{
					if( m_GLState.TexUnits[nTexUnit].fTex2DMinLod != fValue )
					{
						glTexParameterf( eTarget, eParameter, fValue );
						m_GLState.TexUnits[nTexUnit].fTex2DMinLod = (GLfloat)fValue;
					}
				}
				else if( eParameter == GL_TEXTURE_MAX_LOD )
				{
					// max lod
					if( m_GLState.TexUnits[nTexUnit].fTex2DMaxLod != fValue )
					{
						glTexParameterf( eTarget, eParameter, fValue );
						m_GLState.TexUnits[nTexUnit].fTex2DMaxLod = (GLfloat)fValue;
					}
				}
				else if( eParameter == GL_TEXTURE_BASE_LEVEL )
				{
					// base level
					if( m_GLState.TexUnits[nTexUnit].fTex2DBaseLevel != fValue )
					{
						glTexParameterf( eTarget, eParameter, fValue );
						m_GLState.TexUnits[nTexUnit].fTex2DBaseLevel = (GLfloat)fValue;
					}
				}
				else if( eParameter == GL_TEXTURE_MAX_LEVEL )
				{
					// max level
					if( m_GLState.TexUnits[nTexUnit].fTex2DMaxLevel != fValue )
					{
						glTexParameterf( eTarget, eParameter, fValue );
						m_GLState.TexUnits[nTexUnit].fTex2DMaxLevel = (GLfloat)fValue;
					}
				}
			}
			else if( eTarget == GL_TEXTURE_RECTANGLE_ARB )
			{
				if( bExtTextureRectangle )
				{
					// min lod
					if( eParameter == GL_TEXTURE_MIN_LOD )
					{
						if( m_GLState.TexUnits[nTexUnit].fTexRectangleMinLod != fValue )
						{
							glTexParameterf( eTarget, eParameter, fValue );
							m_GLState.TexUnits[nTexUnit].fTexRectangleMinLod = (GLfloat)fValue;
						}
					}
					else if( eParameter == GL_TEXTURE_MAX_LOD )
					{
						// max lod
						if( m_GLState.TexUnits[nTexUnit].fTexRectangleMaxLod != fValue )
						{
							glTexParameterf( eTarget, eParameter, fValue );
							m_GLState.TexUnits[nTexUnit].fTexRectangleMaxLod = (GLfloat)fValue;
						}
					}
					else if( eParameter == GL_TEXTURE_BASE_LEVEL )
					{
						// base level
						if( m_GLState.TexUnits[nTexUnit].fTexRectangleBaseLevel != fValue )
						{
							glTexParameterf( eTarget, eParameter, fValue );
							m_GLState.TexUnits[nTexUnit].fTexRectangleBaseLevel = (GLfloat)fValue;
						}
					}
					else if( eParameter == GL_TEXTURE_MAX_LEVEL )
					{
						// max level
						if( m_GLState.TexUnits[nTexUnit].fTexRectangleMaxLevel != fValue )
						{
							glTexParameterf( eTarget, eParameter, fValue );
							m_GLState.TexUnits[nTexUnit].fTexRectangleMaxLevel = (GLfloat)fValue;
						}
					}
				}
			}
			else if( eTarget == GL_TEXTURE_CUBE_MAP )
			{
				if( bExtTextureCubeMap )
				{
					// min lod
					if( eParameter == GL_TEXTURE_MIN_LOD )
					{
						if( m_GLState.TexUnits[nTexUnit].fTexCubemapMinLod != fValue )
						{
							glTexParameterf( eTarget, eParameter, fValue );
							m_GLState.TexUnits[nTexUnit].fTexCubemapMinLod = (GLfloat)fValue;
						}
					}
					else if( eParameter == GL_TEXTURE_MAX_LOD )
					{
						// max lod
						if( m_GLState.TexUnits[nTexUnit].fTexCubemapMaxLod != fValue )
						{
							glTexParameterf( eTarget, eParameter, fValue );
							m_GLState.TexUnits[nTexUnit].fTexCubemapMaxLod = (GLfloat)fValue;
						}
					}
					else if( eParameter == GL_TEXTURE_BASE_LEVEL )
					{
						// base level
						if( m_GLState.TexUnits[nTexUnit].fTexCubemapBaseLevel != fValue )
						{
							glTexParameterf( eTarget, eParameter, fValue );
							m_GLState.TexUnits[nTexUnit].fTexCubemapBaseLevel = (GLfloat)fValue;
						}
					}
					else if( eParameter == GL_TEXTURE_MAX_LEVEL )
					{
						// max level
						if( m_GLState.TexUnits[nTexUnit].fTexCubemapMaxLevel != fValue )
						{
							glTexParameterf( eTarget, eParameter, fValue );
							m_GLState.TexUnits[nTexUnit].fTexCubemapMaxLevel = (GLfloat)fValue;
						}
					}
				}
			}
		}
	}

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
			glFogi( GL_FOG_MODE, eFogMode );

			m_GLState.eFogMode = eFogMode;
		}

		GLfloat fogCol[4] = { vFogColour.R, vFogColour.G, vFogColour.B, 1.0f };
		if( m_GLState.vFogColour.R != fogCol[0] || 
		   m_GLState.vFogColour.G != fogCol[1] ||
		   m_GLState.vFogColour.B != fogCol[2] )
		{
			glFogfv( GL_FOG_COLOR, &fogCol[0] );
			
			m_GLState.vFogColour = vFogColour;
		}

		//if( fFogFarClip < fFogNearClip )
		//	DBGLOG( "OPENGL: *WARNING* the fog far clip distance has been set less than the fog near clip\n" );

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

	//if( m_GLState.LightUnits[nLightUnit].vSpotDirection.X != dir.X || 
	//	m_GLState.LightUnits[nLightUnit].vSpotDirection.Y != dir.Y ||
	//	m_GLState.LightUnits[nLightUnit].vSpotDirection.Z != dir.Z )
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

	//if( m_GLState.LightUnits[nLightUnit].fSpotExponent != val )
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

	//if( m_GLState.LightUnits[nLightUnit].fSpotCutOff != val )
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
	m_HasGLVersion1_2 = SupportsGLVersion( 1, 2 );
	m_HasGLVersion1_3 = SupportsGLVersion( 1, 3 );
	m_HasGLVersion1_4 = SupportsGLVersion( 1, 4 );
	m_HasGLVersion1_5 = SupportsGLVersion( 1, 5 );
	m_HasGLVersion2_0 = SupportsGLVersion( 2, 0 );
	m_HasGLVersion2_1 = SupportsGLVersion( 2, 1 );

	DBGLOG( "\n" );

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: GoFullScreen
/// Params: None
///
/////////////////////////////////////////////////////
void OpenGL::GoFullScreen()
{
	DBGLOG( "----- GoFullScreen -----\n" );

#ifdef BASE_PLATFORM_WINDOWS
	std::memset( &m_dmScreen,0,sizeof(m_dmScreen) );	

	m_dmScreen.dmSize			=	sizeof( m_dmScreen );			
	m_dmScreen.dmPelsWidth		=	m_Width;					
	m_dmScreen.dmPelsHeight		=	m_Height;					
	m_dmScreen.dmBitsPerPel		=	m_BPP;						
	m_dmScreen.dmFields			=	DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

	if( ChangeDisplaySettings( &m_dmScreen, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL )
	{
		//MSGBOX( "The Requested Fullscreen Mode Is Not Supported By\nYour Video Card.","OPENGL ERROR" );
		return;
	}
#endif // BASE_PLATFORM_WINDOWS

	DBGLOG( "----- GoFullScreen Complete-----\n" );

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
	
	// always reset to perspective
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
		glOrtho( 0.0f, m_Width, 0.0f, m_Height, m_NearClip, m_FarClip/*-1.0f, 1.0f*/ );	
        if( m_Rotate )
            glRotatef( m_RotationAngle, 0.0f, 0.0f, 1.0f );	
	glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();

	// this translation seems to correct pixel shifting according to the GL tips section in the red book
	glTranslatef( 0.4f, 0.4f, 0.0f );

	// make bottom left 0,0
	if( m_Rotate )
    {
        switch( m_RotationStyle )
        {
            case VIEWROTATION_PORTRAIT_BUTTON_BOTTOM:
            {
                // shouldn't need to do anything
            }break;                
            case VIEWROTATION_PORTRAIT_BUTTON_TOP:            
            {
               glTranslatef( -static_cast<float>(m_Width), -static_cast<float>(m_Height), 0.0f );
            }break;                
            case VIEWROTATION_LANDSCAPE_BUTTON_LEFT:
            {
                glTranslatef( 0.0f, -static_cast<float>(m_Width), 0.0f );
            }break;
            case VIEWROTATION_LANDSCAPE_BUTTON_RIGHT: 
            {
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
		DBGLOG( "OPENGL: *WARNING* the far clip distance has been set less than the near clip\n" );

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
		DBGLOG( "OPENGL: *WARNING* the FOV angle has been requested greater than 270degs forcing it to 270deg\n" );
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

	// make sure debug font ortho is correct size
	//gDebugGLFont.SetDisplayMode( m_Width, m_Height );

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
	DBG_ASSERT( pState != 0 );

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
/// Params: [in]pState
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
void OpenGL::SaveViewState( renderer::TViewState* pState )
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
/// Params: [in]viewState
///
/////////////////////////////////////////////////////
void OpenGL::SetViewState( renderer::TViewState* pViewState )
{
	DBG_ASSERT( pViewState != 0 );

	m_eViewState = pViewState->eViewState;
	m_FOV = pViewState->fFOV;
	m_NearClip	= pViewState->fNearClip;
	m_FarClip = pViewState->fFarClip;
	m_Width = pViewState->nWidth;
	m_Height = pViewState->nHeight;

	if( m_Height == 0 )
		m_Height = 1;

	if( m_Width == 0 )
		m_Width = 1;

	if( m_eViewState == VIEWSTATE_PERSPECTIVE )
		SetupPerspectiveView( m_Width, m_Height, true );

	if( m_eViewState == VIEWSTATE_ORTHOGRAPHIC )
		SetupOrthographicView( m_Width, m_Height, true );

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
/// Params: [in]cullState
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
		DBGLOG( "OPENGL: *WARNING* Anisotropic level requested was higher than supported, setting to max supported level\n" );
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
	if( bExtSeparateSpecularColor )
		glLightModeli( GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR );

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
	if( bExtSeparateSpecularColor )
		glLightModeli( GL_LIGHT_MODEL_COLOR_CONTROL, GL_SINGLE_COLOR );

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

	if( style & renderer::VIEWMIRROR_HORIZONTAL)
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
	GLdouble equation[4];
	equation[0] = static_cast<GLdouble>(eq.X);
	equation[1] = static_cast<GLdouble>(eq.Y);
	equation[2] = static_cast<GLdouble>(eq.Z);
	equation[3] = static_cast<GLdouble>(eq.W);

	glClipPlane( plane, &equation[0] );

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

	GL_CHECK

#ifdef BASE_PLATFORM_WINDOWS
	SwapBuffers( m_hDC );
#endif // BASE_PLATFORM_WINDOWS

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
/// Params: [in]X, [in]Y, [in]Z
///
/////////////////////////////////////////////////////
bool OpenGL::PointInFrustum( GLfloat X, GLfloat Y, GLfloat Z )
{
	GLint nPlaneNum;

	for( nPlaneNum = 0; nPlaneNum < 6; nPlaneNum++ )
		if(m_Frustum[nPlaneNum][0] * X + m_Frustum[nPlaneNum][1] * Y + 
			m_Frustum[nPlaneNum][2] * Z + m_Frustum[nPlaneNum][3] <= 0 )
			return false;

	// check for errors
	GL_CHECK

	return true;
}

/////////////////////////////////////////////////////
/// Method: SphereInFrustum
/// Params: [in]X, [in]Y, [in]Z, [in]fRadius
///
/////////////////////////////////////////////////////
bool OpenGL::SphereInFrustum( GLfloat X, GLfloat Y, GLfloat Z, GLfloat fRadius )
{
	GLint nPlaneNum;

	for( nPlaneNum = 0; nPlaneNum < 6; nPlaneNum++ )
		if(m_Frustum[nPlaneNum][0] * X + m_Frustum[nPlaneNum][1] * Y + 
			m_Frustum[nPlaneNum][2] * Z + m_Frustum[nPlaneNum][3] <= -fRadius )
			return false;

	// check for errors
	GL_CHECK

	return true;
}

/////////////////////////////////////////////////////
/// Method: SphereDistanceFromFrustum
/// Params: [in]X, [in]Y, [in]Z, [in]fRadius
///
/////////////////////////////////////////////////////
GLfloat OpenGL::SphereDistanceFromFrustum( GLfloat X, GLfloat Y, GLfloat Z, GLfloat /*fRadius*/ )
{
	// this returns the distance from the near plane
/*	GLint nPlaneNum;
	GLfloat d = 0.0f;

	for( nPlaneNum = 0; nPlaneNum < 6; nPlaneNum++ )
	{
		d = m_Frustum[nPlaneNum][0] * X + m_Frustum[nPlaneNum][1] * Y + 
			m_Frustum[nPlaneNum][2] * Z + m_Frustum[nPlaneNum][3];

		if( d <= -fRadius )
			return(0.0f);
	}

	return(d + fRadius);
*/
	GLfloat fDiffX, fDiffY, fDiffZ;

	fDiffX = std::abs( m_vLookAtEye.X - X );
	fDiffY = std::abs( m_vLookAtEye.Y - Y );
	fDiffZ = std::abs( m_vLookAtEye.Z - Z );

	GLfloat distance = std::sqrt( (fDiffX*fDiffX) + (fDiffY*fDiffY) + (fDiffZ*fDiffZ) );

	// check for errors
	GL_CHECK

	return(distance);
}

/////////////////////////////////////////////////////
/// Method: CubeInFrustum
/// Params: [in]X, [in]Y, [in]Z, [in]fSize
///
/////////////////////////////////////////////////////
bool OpenGL::CubeInFrustum( GLfloat X, GLfloat Y, GLfloat Z, GLfloat fSize )
{
	for(GLint i = 0; i < 6; i++ )
	{
		if(m_Frustum[i][0] * (X - fSize) + m_Frustum[i][1] * (Y - fSize) + m_Frustum[i][2] * (Z - fSize) + m_Frustum[i][3] > 0)
		   continue;
		if(m_Frustum[i][0] * (X + fSize) + m_Frustum[i][1] * (Y - fSize) + m_Frustum[i][2] * (Z - fSize) + m_Frustum[i][3] > 0)
		   continue;
		if(m_Frustum[i][0] * (X - fSize) + m_Frustum[i][1] * (Y + fSize) + m_Frustum[i][2] * (Z - fSize) + m_Frustum[i][3] > 0)
		   continue;
		if(m_Frustum[i][0] * (X + fSize) + m_Frustum[i][1] * (Y + fSize) + m_Frustum[i][2] * (Z - fSize) + m_Frustum[i][3] > 0)
		   continue;
		if(m_Frustum[i][0] * (X - fSize) + m_Frustum[i][1] * (Y - fSize) + m_Frustum[i][2] * (Z + fSize) + m_Frustum[i][3] > 0)
		   continue;
		if(m_Frustum[i][0] * (X + fSize) + m_Frustum[i][1] * (Y - fSize) + m_Frustum[i][2] * (Z + fSize) + m_Frustum[i][3] > 0)
		   continue;
		if(m_Frustum[i][0] * (X - fSize) + m_Frustum[i][1] * (Y + fSize) + m_Frustum[i][2] * (Z + fSize) + m_Frustum[i][3] > 0)
		   continue;
		if(m_Frustum[i][0] * (X + fSize) + m_Frustum[i][1] * (Y + fSize) + m_Frustum[i][2] * (Z + fSize) + m_Frustum[i][3] > 0)
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
	char szOutFile[core::MAX_PATH+core::MAX_PATH];

	if( fTimeSinceLast > 1.0f )
	{
		std::memset( szOutFile, 0, sizeof(char)*core::MAX_PATH+core::MAX_PATH );

		// setup the storage
		screenTexture.nFormat = GL_RGBA;
		screenTexture.nBPP = 32;
		screenTexture.nWidth = m_Width;
		screenTexture.nHeight = m_Height;

		// allocate
		try
		{
			screenTexture.image = new unsigned char[(m_Width*m_Height)*(screenTexture.nBPP/8)];

			if( !screenTexture.image )
			{
				DBGLOG( "OPENGL *ERROR* allocation failed (line:%d)\n",  __LINE__ );
				return;
			}
		}
		catch( std::bad_alloc &ba )
		{
			DBGLOG( "OPENGL: *ERROR* allocation failed with %s (line:%d)\n", ba.what(), __LINE__ );
			return;
		}

		glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

		// get the screen
		glReadPixels( 0, 0, m_Width, m_Height, GL_RGBA, GL_UNSIGNED_BYTE, screenTexture.image );

		// create a directory if it doesn't exist
		file::CreateDirectory( "Screenshots" );

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
		// make sure multitexturing is allowed, or if it's only unit 0 allow it
		if( bExtMultiTexture || nTexUnit == 0 )
		{
			if( eTarget == GL_TEXTURE_1D )				
			{
				return( m_GLState.TexUnits[nTexUnit].nCurrent1DTexture );
			}
			else if( eTarget == GL_TEXTURE_2D )
			{					
				return( m_GLState.TexUnits[nTexUnit].nCurrent2DTexture );
			}
			else if( eTarget == GL_TEXTURE_RECTANGLE_ARB )
			{					
				return( m_GLState.TexUnits[nTexUnit].nCurrentRectangleTexture );
			}
			else if( eTarget == GL_TEXTURE_CUBE_MAP )
			{					
				return( m_GLState.TexUnits[nTexUnit].nCurrentCubemapTexture );
			}
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

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // BASE_SUPPORT_OPENGL


