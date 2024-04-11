
/*===================================================================
	File: OpenGLSL.cpp
	Library: Render

	(C)Hidden Games
=====================================================================*/

#ifdef BASE_SUPPORT_OPENGL_GLSL

#include "CoreBase.h"

#include <cmath>

#include "Math/Vectors.h"
#include "Collision/AABB.h"

#include "Render/RenderConsts.h"
#include "Render/OpenGLCommon.h"
#include "Render/GLSL/OpenGLSL.h"
#include "Render/GLSL/glewES.h"
#include "Render/TextureShared.h"
#include "Render/Texture.h"
#include "Render/GLSL/TextureLoadAndUploadGLSL.h"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#if BASE_SUPPORT_FREETYPE
	#include "Render/FreetypeCommon.h"
	#include "Render/GLSL/FreetypeFontGLSL.h"
#endif // BASE_SUPPORT_FREETYPE

#if BASE_SUPPORT_FREETYPE_UTF8
	#include "Render/FreetypeCommon.h"
	#include "Render/GLSL/FreetypeFontUTF8GLSL.h"
#endif // BASE_SUPPORT_FREETYPE_UTF8

#include "Render/GLSL/PrimitivesGLSL.h"

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
		m_GLState.TexUnits[i].nCurrent1DTexture = renderer::INVALID_OBJECT;
		m_GLState.TexUnits[i].nCurrent2DTexture = renderer::INVALID_OBJECT;
		m_GLState.TexUnits[i].nCurrentRectangleTexture = renderer::INVALID_OBJECT;

		m_GLState.TexUnits[i].nCurrentCubemapTexture = renderer::INVALID_OBJECT;
	}

	m_GLState.bVBOActive			= false;

	m_GLState.nCurrentProgram		= renderer::INVALID_OBJECT;

	m_GLState.bTextureState			= true;
	m_GLState.nCurrentTexture		= renderer::INVALID_OBJECT;
	m_GLState.nCurrentFrameBuffer	= renderer::INVALID_OBJECT;

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
	m_GLState.eFogMode = 0;
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

	m_Width		= 640;
	m_Height	= 480;
	m_BPP		= 32;
	m_ZBuffer	= 24;
	m_AlphaBits = 8;
	m_Rotate = false;
    m_RotationStyle = VIEWROTATION_PORTRAIT_BUTTON_BOTTOM;
    m_RotationAngle = 0.0f;
	m_Mirror = false;
	m_MirrorStyle = VIEWMIRROR_NONE;
	m_MirrorScales = glm::vec3( 1.0f, 1.0f, 1.0f );

	m_ProjMatrix = glm::mat4(1.0f);
	m_ViewMatrix = glm::mat4(1.0f);
	m_ModlMatrix = glm::mat4(1.0f);
	m_ClipMatrix = glm::mat4(1.0f);

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

	m_vClearColour = math::Vec4( 0.0f, 0.0f, 0.0f, 1.0f );
	m_ClearBits = (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_AmbientLighting = false;

	m_vAmbientColour = math::Vec4( 0.0f, 0.0f, 0.0f, 1.0f );

	m_MaxTextureSize = 1;
	m_MaxTextureUnits = 1;
	m_MaxRenderBufferSize = 1;

	m_DebugTextureID = renderer::INVALID_OBJECT;
	m_DebugPhysicsTextureID = renderer::INVALID_OBJECT;

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
		m_GLState.TexUnits[i].nCurrent1DTexture = renderer::INVALID_OBJECT;
		m_GLState.TexUnits[i].nCurrent2DTexture = renderer::INVALID_OBJECT;
		m_GLState.TexUnits[i].nCurrentRectangleTexture = renderer::INVALID_OBJECT;
        
		m_GLState.TexUnits[i].nCurrentCubemapTexture = renderer::INVALID_OBJECT;
	}
    
	m_GLState.bVBOActive			= false;
    
	m_GLState.nCurrentProgram		= renderer::INVALID_OBJECT;
    
	m_GLState.bTextureState			= true;
	m_GLState.nCurrentTexture		= renderer::INVALID_OBJECT;
	m_GLState.nCurrentFrameBuffer	= renderer::INVALID_OBJECT;
    
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
	m_GLState.eFogMode = 0;
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
    
	m_Width		= 640;
	m_Height	= 480;
	m_BPP		= 32;
	m_ZBuffer	= 24;
	m_AlphaBits = 8;
	m_Rotate = false;
    m_RotationStyle = VIEWROTATION_PORTRAIT_BUTTON_BOTTOM;
    m_RotationAngle = 0.0f;
	m_Mirror = false;
	m_MirrorStyle = VIEWMIRROR_NONE;
	m_MirrorScales = glm::vec3( 1.0f, 1.0f, 1.0f );
    
	m_ProjMatrix = glm::mat4(1.0f);
	m_ViewMatrix = glm::mat4(1.0f);
	m_ModlMatrix = glm::mat4(1.0f);
	m_ClipMatrix = glm::mat4(1.0f);
    
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
    
	m_vClearColour = math::Vec4( 0.0f, 0.0f, 0.0f, 1.0f );
	m_ClearBits = (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
	m_AmbientLighting = false;
    
	m_vAmbientColour = math::Vec4( 0.0f, 0.0f, 0.0f, 1.0f );
    
////
	UseProgram( 0 );
	m_GLState.nCurrentProgram = 0;
    
	// now do some simple OpenGL initialisation
	
	glClearColor( m_vClearColour.R, m_vClearColour.G, m_vClearColour.B, m_vClearColour.A );
	//SetClearBits( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
	//SetRotated( false );
	//SetMirrored( false );
    
	glEnable( GL_CULL_FACE );
	glCullFace( GL_BACK );
	//SetCullState( true, GL_BACK );
	glFrontFace(GL_CCW);
    
	glEnable( GL_DEPTH_TEST );
    
	// some defaults
	EnableTexturing();
    
	glActiveTexture( GL_TEXTURE0 );
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
    
	//AlphaMode( false, GL_ALWAYS, 0.0f );
    
	//SetAmbientFlag( false );
	//SetAmbientColour( 1.0f, 1.0f, 1.0f, 1.0f );
    
	m_GLState.bColourMaterialState = false;
	m_GLState.vMaterialAmbient = math::Vec4( 0.2f, 0.2f, 0.2f, 1.0f );
	m_GLState.vMaterialDiffuse = math::Vec4( 0.8f, 0.8f, 0.8f, 1.0f ) ;
	m_GLState.vMaterialSpecular = math::Vec4( 0.0f, 0.0f, 0.0f, 1.0f );
	m_GLState.vMaterialEmission = math::Vec4( 0.0f, 0.0f, 0.0f, 1.0f );
	m_GLState.fMaterialShininess = 0.0f;
	
	//SetMaterialAmbient( m_GLState.vMaterialAmbient );
	//SetMaterialDiffuse( m_GLState.vMaterialDiffuse );
	//SetMaterialSpecular( m_GLState.vMaterialSpecular );
	//SetMaterialEmission( m_GLState.vMaterialEmission );
	//SetMaterialShininess( m_GLState.fMaterialShininess );
}

/////////////////////////////////////////////////////
/// Method: Init
/// Params: None
///
/////////////////////////////////////////////////////
void OpenGL::Init( void )
{
	char szRenderer[renderer::VENDOR_RENDERER_SIZE];
	char szVersion[renderer::VENDOR_VERSION_SIZE];
	char szVendor[renderer::VENDOR_VENDOR_SIZE];
	char szExtensions[renderer::VENDOR_EXTENSIONS_SIZE];
	
	snprintf( szRenderer, renderer::VENDOR_RENDERER_SIZE, "Renderer: %s\n", glGetString( GL_RENDERER ) );
	snprintf( szVersion, renderer::VENDOR_VERSION_SIZE, "Version: %s\n", glGetString( GL_VERSION ) );

	snprintf( szVendor, renderer::VENDOR_VENDOR_SIZE, "Vendor: %s\n", glGetString( GL_VENDOR ) );
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
	glewInit();
	glewESInit();

	UseProgram( 0 );
	m_GLState.nCurrentProgram = 0;

	// now do some simple OpenGL initialisation
	
	glClearColor( m_vClearColour.R, m_vClearColour.G, m_vClearColour.B, m_vClearColour.A );		
	SetClearBits( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
					
	SetRotated( false );
	SetMirrored( false );

	glEnable( GL_CULL_FACE );
	glCullFace( GL_BACK );
	SetCullState( true, GL_BACK );		
	glFrontFace(GL_CCW);

	glEnable( GL_DEPTH_TEST );

	// some defaults
	EnableTexturing();

	glActiveTexture( GL_TEXTURE0 );
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

	AlphaMode( false, GL_ALWAYS, 0.0f );

	SetAmbientFlag( false );
	SetAmbientColour( 1.0f, 1.0f, 1.0f, 1.0f );

	m_GLState.bColourMaterialState = false;
	m_GLState.vMaterialAmbient = math::Vec4( 0.2f, 0.2f, 0.2f, 1.0f );
	m_GLState.vMaterialDiffuse = math::Vec4( 0.8f, 0.8f, 0.8f, 1.0f ) ;
	m_GLState.vMaterialSpecular = math::Vec4( 0.0f, 0.0f, 0.0f, 1.0f );
	m_GLState.vMaterialEmission = math::Vec4( 0.0f, 0.0f, 0.0f, 1.0f );
	m_GLState.fMaterialShininess = 0.0f;
	
	SetMaterialAmbient( m_GLState.vMaterialAmbient );
	SetMaterialDiffuse( m_GLState.vMaterialDiffuse );
	SetMaterialSpecular( m_GLState.vMaterialSpecular );
	SetMaterialEmission( m_GLState.vMaterialEmission );
	SetMaterialShininess( m_GLState.fMaterialShininess );

#ifdef _DEBUG
	// debug texture
	const int MISSING_TEX_SIZE = 16;

	// make the texture the current one			
	BindUnitTexture( 0, GL_TEXTURE_2D, 0 );

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
#else // _DEBUG
	// debug texture
	const int MISSING_TEX_SIZE = 16;

	// make the texture the current one			
	BindUnitTexture( 0, GL_TEXTURE_2D, 0 );

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
			int c = 255;//(((j&0x4)==0)^((k&0x4)==0))*255;
			tmpImage[j][k][_R_] = static_cast<GLubyte>(c);
			tmpImage[j][k][_G_] = static_cast<GLubyte>(c);
			tmpImage[j][k][_B_] = static_cast<GLubyte>(c);
		}
	}

	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, MISSING_TEX_SIZE, MISSING_TEX_SIZE, 0, GL_RGB, GL_UNSIGNED_BYTE, tmpImage );
#endif // 

	renderer::InitialisePrimitives();

#if defined(BASE_SUPPORT_FREETYPE) || defined(BASE_SUPPORT_FREETYPE_UTF8)
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

	UseProgram(0);

	renderer::ShutdownPrimitives();

#if defined(BASE_SUPPORT_FREETYPE) || defined(BASE_SUPPORT_FREETYPE_UTF8)
	renderer::ShutdownFreetype();
#endif // BASE_SUPPORT_FREETYPE

	if( m_DebugTextureID != renderer::INVALID_OBJECT )
	{
		renderer::RemoveTexture( m_DebugTextureID );
		m_DebugTextureID = renderer::INVALID_OBJECT;
	}

	if( m_DebugPhysicsTextureID != renderer::INVALID_OBJECT )
	{
		renderer::RemoveTexture( m_DebugPhysicsTextureID );
		m_DebugPhysicsTextureID = renderer::INVALID_OBJECT;
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
	m_AlphaBits = chosen_pfd.cAlphaBits;

	DBGLOG( "Colour Bits: %d, ZBuffer: %d\n", m_BPP, m_ZBuffer );
	DBGLOG( "Alpha Bits: %d\n", m_AlphaBits );

	return(0);
}

#endif // BASE_PLATFORM_WINDOWS

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
	static const EGLint context_attributes[] = 
	{
		EGL_CONTEXT_CLIENT_VERSION, 2,
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
	m_EGLContext = eglCreateContext( m_EGLDisplay, config, EGL_NO_CONTEXT, context_attributes);
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
/// Method: UseProgram
/// Params: [in]program
///
/////////////////////////////////////////////////////
void OpenGL::UseProgram( GLuint program )
{
	//if( m_GLState.nCurrentProgram == program )
	//	return;

	if( program == 0 ||
		program == renderer::INVALID_OBJECT )
	{
		ClearProgram();
		return;
	}

	if( glIsProgram(program) )
	{
		glUseProgram( program );
		m_GLState.nCurrentProgram = program;
	}
	else
	{
		ClearProgram();
		return;
	}

	// check for errors
	GL_CHECK
}
			
/////////////////////////////////////////////////////
/// Method: ClearProgram
/// Params: None
///
/////////////////////////////////////////////////////
void OpenGL::ClearProgram()
{
	m_GLState.nCurrentProgram = renderer::INVALID_OBJECT;
	glUseProgram(0);

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
}

/////////////////////////////////////////////////////
/// Method: DisableVertexArray
/// Params: None
///
/////////////////////////////////////////////////////
void OpenGL::DisableVertexArray( void )
{
}

/////////////////////////////////////////////////////
/// Method: EnableTextureArray
/// Params: None
///
/////////////////////////////////////////////////////
void OpenGL::EnableTextureArray( void )
{
}

/////////////////////////////////////////////////////
/// Method: DisableTextureArray
/// Params: None
///
/////////////////////////////////////////////////////
void OpenGL::DisableTextureArray( void )
{
}

/////////////////////////////////////////////////////
/// Method: EnableColourArray
/// Params: None
///
/////////////////////////////////////////////////////
void OpenGL::EnableColourArray( void )
{
}

/////////////////////////////////////////////////////
/// Method: DisableColourArray
/// Params: None
///
/////////////////////////////////////////////////////
void OpenGL::DisableColourArray( void )
{
}

/////////////////////////////////////////////////////
/// Method: EnableNormalArray
/// Params: None
///
/////////////////////////////////////////////////////
void OpenGL::EnableNormalArray( void )
{
}

/////////////////////////////////////////////////////
/// Method: DisableNormalArray
/// Params: None
///
/////////////////////////////////////////////////////
void OpenGL::DisableNormalArray( void )
{
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
        if( i != 0 )
            glActiveTexture( eActiveTexUnit );

		if( m_GLState.TexUnits[i].nCurrentCubemapTexture != renderer::INVALID_OBJECT && m_GLState.TexUnits[i].nCurrentCubemapTexture != 0 )
			glBindTexture( GL_TEXTURE_CUBE_MAP, m_GLState.TexUnits[i].nCurrentCubemapTexture );

		if( m_GLState.TexUnits[i].nCurrent2DTexture != renderer::INVALID_OBJECT && m_GLState.TexUnits[i].nCurrent2DTexture != 0 )
			glBindTexture( GL_TEXTURE_2D, m_GLState.TexUnits[i].nCurrent2DTexture );
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

	m_GLState.bTextureState = false;

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

	if( nFrameBufferID != renderer::INVALID_OBJECT )
	{
		glBindFramebuffer( GL_FRAMEBUFFER, nFrameBufferID );
		m_GLState.nCurrentFrameBuffer = nFrameBufferID;	
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

		glBindTexture( GL_TEXTURE_CUBE_MAP, 0 );
		m_GLState.TexUnits[i].nCurrentCubemapTexture = 0;

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
	if( nTexUnit >= 0 /*&& nTexUnit < m_MaxTextureUnits*/ )
	{
		// make sure multitexturing is allowed, or if it's only unit 0 allow it
		// which unit are we working on
		GLenum eActiveTexUnit = GL_TEXTURE0 + nTexUnit;

		glActiveTexture( eActiveTexUnit );	

		glBindTexture( GL_TEXTURE_CUBE_MAP, 0 );
		m_GLState.TexUnits[nTexUnit].nCurrentCubemapTexture = 0;

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
	if( nTexUnit >= 0 /*&& nTexUnit < m_MaxTextureUnits*/ )
	{
		ClearUnitTexture( nTexUnit );

		// make sure multitexturing is allowed, or if it's only unit 0 allow it
		// which unit are we working on
		//GLenum eActiveTexUnit = GL_TEXTURE0 + nTexUnit;

		//glActiveTexture( eActiveTexUnit );	
		//glBindTexture( GL_TEXTURE_2D, 0 );
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
	// unbind any framebuffer
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	m_GLState.nCurrentFrameBuffer = 0;

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
	if( nTexUnit >= 0 /*&& nTexUnit < m_MaxTextureUnits*/ )
	{
		// make sure multitexturing is allowed, or if it's only unit 0 allow it
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
		else if( eTarget == GL_TEXTURE_CUBE_MAP )
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
	// FIXME: SetMipmapLodParameters - update

	// make sure it's a valid unit
	/*if( nTexUnit >= 0 && nTexUnit < m_MaxTextureUnits )
	{
		// make sure lod controls are valid and if multitexturing is allowed, switch to correct active texture
		if( bExtTextureLod && (bExtMultiTexture || nTexUnit == 0) )
		{
			// which unit are we working on
			GLenum eActiveTexUnit = GL_TEXTURE0 + nTexUnit;

			glActiveTexture( eActiveTexUnit );	

			if( eTarget == GL_TEXTURE_2D )
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
			else if( eTarget == GL_TEXTURE_CUBE_MAP )
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
	}*/

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
	/*if( m_GLState.bAlphaState != bAlphaFlag )
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
	}*/

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
/*	// already on?
	if( m_GLState.bColourMaterialState == true )
		return;

	m_GLState.bColourMaterialState = true;
*/
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
/*	// already off?
	if( m_GLState.bColourMaterialState == false )
		return;

	m_GLState.bColourMaterialState = false;
*/
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
		m_GLState.vMaterialAmbient = ambientColour;
	}

	if( m_GLState.nCurrentProgram != renderer::INVALID_OBJECT &&
		m_GLState.nCurrentProgram != 0 )
	{
		GLint loc = glGetUniformLocation(m_GLState.nCurrentProgram, "material.ambient");
		if( loc != -1 )
			glUniform4f(loc, m_GLState.vMaterialAmbient.R, m_GLState.vMaterialAmbient.G, m_GLState.vMaterialAmbient.B, m_GLState.vMaterialAmbient.A);
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
		m_GLState.vMaterialDiffuse = diffuseColour;
	}

	if( m_GLState.nCurrentProgram != renderer::INVALID_OBJECT &&
		m_GLState.nCurrentProgram != 0 )
	{
		GLint loc = glGetUniformLocation(m_GLState.nCurrentProgram, "material.diffuse");
		if( loc != -1 )
			glUniform4f(loc, m_GLState.vMaterialDiffuse.R, m_GLState.vMaterialDiffuse.G, m_GLState.vMaterialDiffuse.B, m_GLState.vMaterialDiffuse.A);
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
		m_GLState.vMaterialSpecular = specularColour;
	}

	if( m_GLState.nCurrentProgram != renderer::INVALID_OBJECT &&
		m_GLState.nCurrentProgram != 0 )
	{
		GLint loc = glGetUniformLocation(m_GLState.nCurrentProgram, "material.specular");
		if( loc != -1 )
			glUniform4f(loc, m_GLState.vMaterialSpecular.R, m_GLState.vMaterialSpecular.G, m_GLState.vMaterialSpecular.B, m_GLState.vMaterialSpecular.A);
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
		m_GLState.fMaterialShininess = shininess;
	}

	if( m_GLState.nCurrentProgram != renderer::INVALID_OBJECT &&
		m_GLState.nCurrentProgram != 0 )
	{
		GLint loc = glGetUniformLocation( m_GLState.nCurrentProgram, "material.shininess");
		if( loc != -1 )
			glUniform1f(loc, m_GLState.fMaterialShininess);
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
	//if( m_GLState.LightUnits[nLightUnit].bLightState == true )
	//	return;

	m_GLState.LightUnits[nLightUnit].bLightState = true;

	if( m_GLState.nCurrentProgram != renderer::INVALID_OBJECT &&
		m_GLState.nCurrentProgram != 0 )
	{
		GLint loc = glGetUniformLocation(m_GLState.nCurrentProgram, "light0.enabled");
		if( loc != -1 )
			glUniform1i(loc, 1 );
	}

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
	//if( m_GLState.LightUnits[nLightUnit].bLightState == false )
	//	return;

	m_GLState.LightUnits[nLightUnit].bLightState = false;

	if( m_GLState.nCurrentProgram != renderer::INVALID_OBJECT &&
		m_GLState.nCurrentProgram != 0 )
	{
		GLint loc = glGetUniformLocation(m_GLState.nCurrentProgram, "light0.enabled");
		if( loc != -1 )
			glUniform1i(loc, 0 );
	}

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

	m_GLState.LightUnits[nLightUnit].vPosition = pos;

	// Setup a directional light that is shining down the negative z world axis.
	if( m_GLState.nCurrentProgram != renderer::INVALID_OBJECT &&
		m_GLState.nCurrentProgram != 0 )
	{
		GLint loc = glGetUniformLocation(m_GLState.nCurrentProgram, "light0.position");
		if( loc != -1 )
			glUniform4f(loc, m_GLState.LightUnits[nLightUnit].vPosition.X, m_GLState.LightUnits[nLightUnit].vPosition.Y, m_GLState.LightUnits[nLightUnit].vPosition.Z, m_GLState.LightUnits[nLightUnit].vPosition.W );
	}

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

	if( m_GLState.LightUnits[nLightUnit].vAmbientColour.R != ambientColour.R || 
		m_GLState.LightUnits[nLightUnit].vAmbientColour.G != ambientColour.G ||
		m_GLState.LightUnits[nLightUnit].vAmbientColour.B != ambientColour.B ||
		m_GLState.LightUnits[nLightUnit].vAmbientColour.A != ambientColour.A )
	{
		m_GLState.LightUnits[nLightUnit].vAmbientColour = ambientColour;
	}

	if( m_GLState.nCurrentProgram != renderer::INVALID_OBJECT &&
		m_GLState.nCurrentProgram != 0 )
	{
		GLint loc = glGetUniformLocation(m_GLState.nCurrentProgram, "light0.ambient");
		if( loc != -1 )
			glUniform4f(loc, m_GLState.LightUnits[nLightUnit].vAmbientColour.R, m_GLState.LightUnits[nLightUnit].vAmbientColour.G, m_GLState.LightUnits[nLightUnit].vAmbientColour.B, m_GLState.LightUnits[nLightUnit].vAmbientColour.A );
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

	if( m_GLState.LightUnits[nLightUnit].vDiffuseColour.R != diffuseColour.R || 
		m_GLState.LightUnits[nLightUnit].vDiffuseColour.G != diffuseColour.G ||
		m_GLState.LightUnits[nLightUnit].vDiffuseColour.B != diffuseColour.B ||
		m_GLState.LightUnits[nLightUnit].vDiffuseColour.A != diffuseColour.A )
	{
		m_GLState.LightUnits[nLightUnit].vDiffuseColour = diffuseColour;
	}

	if( m_GLState.nCurrentProgram != renderer::INVALID_OBJECT &&
		m_GLState.nCurrentProgram != 0 )
	{
		GLint loc = glGetUniformLocation(m_GLState.nCurrentProgram, "light0.diffuse");
		if( loc != -1 )
			glUniform4f(loc, m_GLState.LightUnits[nLightUnit].vDiffuseColour.R, m_GLState.LightUnits[nLightUnit].vDiffuseColour.G, m_GLState.LightUnits[nLightUnit].vDiffuseColour.B, m_GLState.LightUnits[nLightUnit].vDiffuseColour.A );
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

	if( m_GLState.LightUnits[nLightUnit].vSpecularColour.R != specularColour.R || 
		m_GLState.LightUnits[nLightUnit].vSpecularColour.G != specularColour.G ||
		m_GLState.LightUnits[nLightUnit].vSpecularColour.B != specularColour.B ||
		m_GLState.LightUnits[nLightUnit].vSpecularColour.A != specularColour.A )
	{
		m_GLState.LightUnits[nLightUnit].vSpecularColour = specularColour;
	}

	if( m_GLState.nCurrentProgram != renderer::INVALID_OBJECT &&
		m_GLState.nCurrentProgram != 0 )
	{
		GLint loc = glGetUniformLocation(m_GLState.nCurrentProgram, "light0.specular");
		if( loc != -1 )
			glUniform4f(loc, m_GLState.LightUnits[nLightUnit].vSpecularColour.R, m_GLState.LightUnits[nLightUnit].vSpecularColour.G, m_GLState.LightUnits[nLightUnit].vSpecularColour.B, m_GLState.LightUnits[nLightUnit].vSpecularColour.A );
	}

	// check for errors
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Method: SetLightAttenuation
/// Params: [in]nLightUnit, [in]type, [in]val
///
/////////////////////////////////////////////////////
void OpenGL::SetLightAttenuation( int nLightUnit, GLenum type, GLfloat val )
{
	DBG_ASSERT( ((nLightUnit >= 0) && (nLightUnit < renderer::MAX_USEABLE_LIGHTS)) );
	DBG_ASSERT( ((type == /*GL_CONSTANT_ATTENUATION*/0) || (type == /*GL_LINEAR_ATTENUATION*/1) || (type == /*GL_QUADRATIC_ATTENUATION*/2)) );

	if( type == /*GL_CONSTANT_ATTENUATION*/0 )
	{
		if( m_GLState.LightUnits[nLightUnit].vAttenuation.X  != val )
		{
			m_GLState.LightUnits[nLightUnit].vAttenuation.X = val;
		}
	}
	else if( type == /*GL_LINEAR_ATTENUATION*/1 )
	{
		if( m_GLState.LightUnits[nLightUnit].vAttenuation.Y  != val )
		{
			m_GLState.LightUnits[nLightUnit].vAttenuation.Y = val;
		}
	}
	else if( type == /*GL_QUADRATIC_ATTENUATION*/2 )
	{
		if( m_GLState.LightUnits[nLightUnit].vAttenuation.Z  != val )
		{
			m_GLState.LightUnits[nLightUnit].vAttenuation.Z = val;
		}
	}

	if( m_GLState.nCurrentProgram != renderer::INVALID_OBJECT &&
		m_GLState.nCurrentProgram != 0 )
	{
		GLint loc = glGetUniformLocation(m_GLState.nCurrentProgram, "light0.attenuation_factors");
		if( loc != -1 )
			glUniform3f(loc, m_GLState.LightUnits[nLightUnit].vAttenuation.X, m_GLState.LightUnits[nLightUnit].vAttenuation.Y, m_GLState.LightUnits[nLightUnit].vAttenuation.Z );
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

	if( m_GLState.LightUnits[nLightUnit].vSpotDirection.X != dir.X || 
		m_GLState.LightUnits[nLightUnit].vSpotDirection.Y != dir.Y ||
		m_GLState.LightUnits[nLightUnit].vSpotDirection.Z != dir.Z )
	{
		m_GLState.LightUnits[nLightUnit].vSpotDirection = dir;
	}

	if( m_GLState.nCurrentProgram != renderer::INVALID_OBJECT &&
		m_GLState.nCurrentProgram != 0 )
	{
		GLint loc = glGetUniformLocation(m_GLState.nCurrentProgram, "light0.spot_direction");
		if( loc != -1 )
			glUniform3f(loc, m_GLState.LightUnits[nLightUnit].vSpotDirection.X, m_GLState.LightUnits[nLightUnit].vSpotDirection.Y, m_GLState.LightUnits[nLightUnit].vSpotDirection.Z );
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

	if( m_GLState.LightUnits[nLightUnit].fSpotExponent != val )
	{
		m_GLState.LightUnits[nLightUnit].fSpotExponent = val;
	}

	if( m_GLState.nCurrentProgram != renderer::INVALID_OBJECT &&
		m_GLState.nCurrentProgram != 0 )
	{
		GLint loc = glGetUniformLocation(m_GLState.nCurrentProgram, "light0.spot_exponent");
		if( loc != -1 )
			glUniform1f(loc, m_GLState.LightUnits[nLightUnit].fSpotExponent );
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

	if( m_GLState.LightUnits[nLightUnit].fSpotCutOff != val )
	{
		m_GLState.LightUnits[nLightUnit].fSpotCutOff = val;
	}

	if( m_GLState.nCurrentProgram != renderer::INVALID_OBJECT &&
		m_GLState.nCurrentProgram != 0 )
	{
		GLint loc = glGetUniformLocation(m_GLState.nCurrentProgram, "light0.spot_cutoff_angle");
		if( loc != -1 )
			glUniform1f(loc, m_GLState.LightUnits[nLightUnit].fSpotCutOff );
	}
}

/////////////////////////////////////////////////////
/// Method: SetColour4f
/// Params: [in]r, [in]g, [in]b, [in]a
///
/////////////////////////////////////////////////////
void OpenGL::SetColour4f( GLfloat r, GLfloat g, GLfloat b, GLfloat a )
{
	/*if( m_GLState.vColour4f.R != r || 
		m_GLState.vColour4f.G != g ||
		m_GLState.vColour4f.B != b ||
		m_GLState.vColour4f.A != a )*/
	{
		m_GLState.vColour4f = math::Vec4( r, g, b, a );

		m_GLState.vColour4ub.R = static_cast<GLubyte>( r*255.0f );
		m_GLState.vColour4ub.G = static_cast<GLubyte>( g*255.0f );
		m_GLState.vColour4ub.B = static_cast<GLubyte>( b*255.0f );
		m_GLState.vColour4ub.A = static_cast<GLubyte>( a*255.0f );

		if( m_GLState.nCurrentProgram != 0 &&
			m_GLState.nCurrentProgram != renderer::INVALID_OBJECT )
		{
			GLint ogl_VertexColour = glGetUniformLocation(m_GLState.nCurrentProgram, "ogl_VertexColour");
			if( ogl_VertexColour != -1 )
			{
				glUniform4f( ogl_VertexColour, m_GLState.vColour4f.R, m_GLState.vColour4f.G, m_GLState.vColour4f.B, m_GLState.vColour4f.A );
			}
		}
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
	/*if( m_GLState.vColour4ub.R != r || 
		m_GLState.vColour4ub.G != g ||
		m_GLState.vColour4ub.B != b ||
		m_GLState.vColour4ub.A != a )*/
	{
		m_GLState.vColour4ub = math::Vec4Lite( r, g, b, a );

		m_GLState.vColour4f.R = static_cast<float>( static_cast<float>(r)/255.0f );
		m_GLState.vColour4f.G = static_cast<float>( static_cast<float>(g)/255.0f );
		m_GLState.vColour4f.B = static_cast<float>( static_cast<float>(b)/255.0f );
		m_GLState.vColour4f.A = static_cast<float>( static_cast<float>(a)/255.0f );

		if( m_GLState.nCurrentProgram != 0 &&
			m_GLState.nCurrentProgram != renderer::INVALID_OBJECT )
		{
			GLint ogl_VertexColour = glGetUniformLocation(m_GLState.nCurrentProgram, "ogl_VertexColour");
			if( ogl_VertexColour != -1 )
			{
				glUniform4f( ogl_VertexColour, m_GLState.vColour4f.R, m_GLState.vColour4f.G, m_GLState.vColour4f.B, m_GLState.vColour4f.A );
			}
		}
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

	glGetIntegerv( GL_MAX_RENDERBUFFER_SIZE, &m_MaxRenderBufferSize );
	DBGLOG( "GL_MAX_RENDERBUFFER_SIZE: (%d)\n", m_MaxRenderBufferSize );

	glGetIntegerv( GL_DEPTH_BITS, &m_DepthBits );
	DBGLOG( "GL_DEPTH_BITS: (%d)\n", m_DepthBits );

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

	// set perspective
    glm::mat4 scale = glm::mat4(1.0f);
    glm::mat4 rotate = glm::mat4(1.0f);
    
	if( m_Mirror )
		scale = glm::scale(glm::mat4(1.0f), m_MirrorScales);
    
	if( m_Rotate )
		rotate = glm::rotate( glm::mat4(1.0f), m_RotationAngle, glm::vec3(0.0f, 0.0f, 1.0f) );
    
    m_ProjMatrix = scale * glm::perspective(m_FOV, m_AspectRatio, m_NearClip, m_FarClip) * rotate;

	// reset modelview
	m_ModlMatrix = glm::mat4(1.0f);
	m_ViewMatrix = glm::mat4(1.0f);
	
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

    glm::mat4 scale = glm::mat4(1.0f);
    glm::mat4 rotate = glm::mat4(1.0f);
    
	if( m_Mirror )
		scale = glm::scale(glm::mat4(1.0f), m_MirrorScales);
    
	if( m_Rotate )
		rotate = glm::rotate( glm::mat4(1.0f), m_RotationAngle, glm::vec3(0.0f, 0.0f, 1.0f) );
    
    m_ProjMatrix = scale * glm::ortho( 0.0f, static_cast<GLfloat>(m_Width), 0.0f, static_cast<GLfloat>(m_Height), m_NearClip, m_FarClip ) * rotate;
	
	// this translation seems to correct pixel shifting according to the GL tips section in the red book
	m_ModlMatrix = glm::translate( glm::mat4(1.0f), glm::vec3(0.4f, 0.4f, 0.0f) );
	m_ViewMatrix = glm::mat4(1.0f);
	
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
			   m_ModlMatrix = glm::translate( m_ModlMatrix, glm::vec3(-static_cast<float>(m_Width), -static_cast<float>(m_Height), 0.0f) );
            }break;                
            case VIEWROTATION_LANDSCAPE_BUTTON_LEFT:
            {
				m_ModlMatrix = glm::translate( m_ModlMatrix, glm::vec3(0.0f, -static_cast<float>(m_Width), 0.0f) );
            }break;
            case VIEWROTATION_LANDSCAPE_BUTTON_RIGHT: 
            {
				m_ModlMatrix = glm::translate( m_ModlMatrix, glm::vec3(-static_cast<float>(m_Height), 0.0f, 0.0f) );
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
	m_ViewMatrix = glm::lookAt(glm::vec3(fEyeX, fEyeY, fEyeZ), glm::vec3(fCenterX, fCenterY, fCenterZ), glm::vec3(0.0f, 1.0f, 0.0f));

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
void OpenGL::SetPerspective( GLfloat fFOV, GLfloat fAspectRatio, GLfloat fNearClip, GLfloat fFarClip, bool force )
{
	// don't change if it's already there
	if( force == false &&
		m_FOV == fFOV &&
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

	// set perspective
    glm::mat4 scale = glm::mat4(1.0f);
    glm::mat4 rotate = glm::mat4(1.0f);
    
	if( m_Mirror )
		scale = glm::scale(glm::mat4(1.0f), m_MirrorScales);

	if( m_Rotate )
		rotate = glm::rotate( glm::mat4(1.0f), m_RotationAngle, glm::vec3(0.0f, 0.0f, 1.0f) );
    
    m_ProjMatrix = scale * glm::perspective(m_FOV, m_AspectRatio, m_NearClip, m_FarClip) * rotate;

	// reset modelview
	m_ModlMatrix = glm::mat4(1.0f);
	m_ViewMatrix = glm::mat4(1.0f);
	
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
	}

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
		m_MirrorScales.y = -1.0f;
	else
		m_MirrorScales.y = 1.0f;

	if( style & renderer::VIEWMIRROR_HORIZONTAL )
		m_MirrorScales.x = -1.0f;
	else
		m_MirrorScales.x = 1.0f;

	m_MirrorStyle = style;
}

/////////////////////////////////////////////////////
/// Method: SetClipPlane
/// Params: None
///
/////////////////////////////////////////////////////
void OpenGL::SetClipPlane( GLenum plane, const math::Vec4& eq )
{
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

#ifdef BASE_PLATFORM_RASPBERRYPI
	eglSwapBuffers( m_EGLDisplay, m_EGLSurface);
#endif // BASE_PLATFORM_RASPBERRYPI

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

	const float *pMdlMtx = (const float*)glm::value_ptr(m_ModlMatrix);
	const float *pProjMtx = (const float*)glm::value_ptr(m_ProjMatrix);
	float *pClipMtx = (float*)glm::value_ptr(m_ClipMatrix);

	// combine the two matrices (proj*mdlview)
	pClipMtx[0] = pMdlMtx[0]*pProjMtx[0] + pMdlMtx[1]*pProjMtx[4] + pMdlMtx[2]*pProjMtx[8] + pMdlMtx[3]*pProjMtx[12];
	pClipMtx[1] = pMdlMtx[0]*pProjMtx[1] + pMdlMtx[1]*pProjMtx[5] + pMdlMtx[2]*pProjMtx[9] + pMdlMtx[3]*pProjMtx[13];
	pClipMtx[2] = pMdlMtx[0]*pProjMtx[2] + pMdlMtx[1]*pProjMtx[6] + pMdlMtx[2]*pProjMtx[10] + pMdlMtx[3]*pProjMtx[14];
	pClipMtx[3] = pMdlMtx[0]*pProjMtx[3] + pMdlMtx[1]*pProjMtx[7] + pMdlMtx[2]*pProjMtx[11] + pMdlMtx[3]*pProjMtx[15];

	pClipMtx[4] = pMdlMtx[4]*pProjMtx[0] + pMdlMtx[5]*pProjMtx[4] + pMdlMtx[6]*pProjMtx[8] + pMdlMtx[7]*pProjMtx[12];
	pClipMtx[5] = pMdlMtx[4]*pProjMtx[1] + pMdlMtx[5]*pProjMtx[5] + pMdlMtx[6]*pProjMtx[9] + pMdlMtx[7]*pProjMtx[13];
	pClipMtx[6] = pMdlMtx[4]*pProjMtx[2] + pMdlMtx[5]*pProjMtx[6] + pMdlMtx[6]*pProjMtx[10] + pMdlMtx[7]*pProjMtx[14];
	pClipMtx[7] = pMdlMtx[4]*pProjMtx[3] + pMdlMtx[5]*pProjMtx[7] + pMdlMtx[6]*pProjMtx[11] + pMdlMtx[7]*pProjMtx[15];

	pClipMtx[8] = pMdlMtx[8]*pProjMtx[0] + pMdlMtx[9]*pProjMtx[4] + pMdlMtx[10]*pProjMtx[8] + pMdlMtx[11]*pProjMtx[12];
	pClipMtx[9] = pMdlMtx[8]*pProjMtx[1] + pMdlMtx[9]*pProjMtx[5] + pMdlMtx[10]*pProjMtx[9] + pMdlMtx[11]*pProjMtx[13];
	pClipMtx[10] = pMdlMtx[8]*pProjMtx[2] + pMdlMtx[9]*pProjMtx[6] + pMdlMtx[10]*pProjMtx[10] + pMdlMtx[11]*pProjMtx[14];
	pClipMtx[11] = pMdlMtx[8]*pProjMtx[3] + pMdlMtx[9]*pProjMtx[7] + pMdlMtx[10]*pProjMtx[11] + pMdlMtx[11]*pProjMtx[15];

	pClipMtx[12] = pMdlMtx[12]*pProjMtx[0] + pMdlMtx[13]*pProjMtx[4] + pMdlMtx[14]*pProjMtx[8] + pMdlMtx[15]*pProjMtx[12];
	pClipMtx[13] = pMdlMtx[12]*pProjMtx[1] + pMdlMtx[13]*pProjMtx[5] + pMdlMtx[14]*pProjMtx[9] + pMdlMtx[15]*pProjMtx[13];
	pClipMtx[14] = pMdlMtx[12]*pProjMtx[2] + pMdlMtx[13]*pProjMtx[6] + pMdlMtx[14]*pProjMtx[10] + pMdlMtx[15]*pProjMtx[14];
	pClipMtx[15] = pMdlMtx[12]*pProjMtx[3] + pMdlMtx[13]*pProjMtx[7] + pMdlMtx[14]*pProjMtx[11] + pMdlMtx[15]*pProjMtx[15];

	// the right plane
	m_Frustum[0][0] = pClipMtx[3] - pClipMtx[0];
	m_Frustum[0][1] = pClipMtx[7] - pClipMtx[4];
	m_Frustum[0][2] = pClipMtx[11] - pClipMtx[8];
	m_Frustum[0][3] = pClipMtx[15] - pClipMtx[12];

	// normalise
	t = std::sqrt(m_Frustum[0][0]*m_Frustum[0][0] + m_Frustum[0][1]*m_Frustum[0][1] + m_Frustum[0][2]*m_Frustum[0][2] );
	m_Frustum[0][0] /= t;
	m_Frustum[0][1] /= t;
	m_Frustum[0][2] /= t;
	m_Frustum[0][3] /= t;

	// the left plane
	m_Frustum[1][0] = pClipMtx[3] + pClipMtx[0];
	m_Frustum[1][1] = pClipMtx[7] + pClipMtx[4];
	m_Frustum[1][2] = pClipMtx[11] + pClipMtx[8];
	m_Frustum[1][3] = pClipMtx[15] + pClipMtx[12];

	// normalise
	t = std::sqrt(m_Frustum[1][0]*m_Frustum[1][0] + m_Frustum[1][1]*m_Frustum[1][1] + m_Frustum[1][2]*m_Frustum[1][2] );
	m_Frustum[1][0] /= t;
	m_Frustum[1][1] /= t;
	m_Frustum[1][2] /= t;
	m_Frustum[1][3] /= t;

	// bottom plane
	m_Frustum[2][0] = pClipMtx[3] + pClipMtx[1];
	m_Frustum[2][1] = pClipMtx[7] + pClipMtx[5];
	m_Frustum[2][2] = pClipMtx[11] + pClipMtx[9];
	m_Frustum[2][3] = pClipMtx[15] + pClipMtx[13];

	// normalise
	t = std::sqrt(m_Frustum[2][0]*m_Frustum[2][0] + m_Frustum[2][1]*m_Frustum[2][1] + m_Frustum[2][2]*m_Frustum[2][2] );
	m_Frustum[2][0] /= t;
	m_Frustum[2][1] /= t;
	m_Frustum[2][2] /= t;
	m_Frustum[2][3] /= t;

	// top plane
	m_Frustum[3][0] = pClipMtx[3] - pClipMtx[1];
	m_Frustum[3][1] = pClipMtx[7] - pClipMtx[5];
	m_Frustum[3][2] = pClipMtx[11] - pClipMtx[9];
	m_Frustum[3][3] = pClipMtx[15] - pClipMtx[13];

	// normalise
	t = std::sqrt(m_Frustum[3][0]*m_Frustum[3][0] + m_Frustum[3][1]*m_Frustum[3][1] + m_Frustum[3][2]*m_Frustum[3][2] );
	m_Frustum[3][0] /= t;
	m_Frustum[3][1] /= t;
	m_Frustum[3][2] /= t;
	m_Frustum[3][3] /= t;

	// far plane
	m_Frustum[4][0] = pClipMtx[3] - pClipMtx[2];
	m_Frustum[4][1] = pClipMtx[7] - pClipMtx[6];
	m_Frustum[4][2] = pClipMtx[11] - pClipMtx[10];
	m_Frustum[4][3] = pClipMtx[15] - pClipMtx[14];

	// normalise
	t = std::sqrt(m_Frustum[4][0]*m_Frustum[4][0] + m_Frustum[4][1]*m_Frustum[4][1] + m_Frustum[4][2]*m_Frustum[4][2] );
	m_Frustum[4][0] /= t;
	m_Frustum[4][1] /= t;
	m_Frustum[4][2] /= t;
	m_Frustum[4][3] /= t;

	// near plane
	m_Frustum[5][0] = pClipMtx[3] + pClipMtx[2];
	m_Frustum[5][1] = pClipMtx[7] + pClipMtx[6];
	m_Frustum[5][2] = pClipMtx[11] + pClipMtx[10];
	m_Frustum[5][3] = pClipMtx[15] + pClipMtx[14];

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
	if( nTexUnit >= 0 /*&& nTexUnit < m_MaxTextureUnits*/ )
	{
		// make sure multitexturing is allowed, or if it's only unit 0 allow it
		if( eTarget == GL_TEXTURE_2D )
		{					
			return( m_GLState.TexUnits[nTexUnit].nCurrent2DTexture );
		}
		else if( eTarget == GL_TEXTURE_CUBE_MAP )
		{					
			return( m_GLState.TexUnits[nTexUnit].nCurrentCubemapTexture );
		}
	}

	// check for errors
	GL_CHECK

	return( renderer::INVALID_OBJECT );
}

/////////////////////////////////////////////////////
/// Method: GetActiveFrameBuffer
/// Params: None
///
/////////////////////////////////////////////////////
GLuint OpenGL::GetActiveFrameBuffer( void )
{
	return( m_GLState.nCurrentFrameBuffer );
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

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // BASE_SUPPORT_OPENGL_GLSL


