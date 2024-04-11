
/*===================================================================
	File: Extensions.cpp
	Library: Render

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"

#include "Math/Vectors.h"

#include "Render/FF/OpenGL/OpenGL.h"
#include "Render/FF/OpenGL/Extensions.h"

#ifdef BASE_SUPPORT_OPENGL

namespace renderer
{
	bool bExtMultisample			= 0;
	bool bExtMultiTexture			= 0;
	bool bExtTexture3D				= 0;
	bool bExtFogCoord				= 0;
	bool bExtPointSprites			= 0;
	bool bExtPointParameters		= 0;
	bool bExtTextureAutomipmap		= 0;
	bool bExtTextureLod				= 0;
	bool bExtTextureLodBias			= 0;
	bool bExtTextureCompression		= 0;
	bool bExtTextureCompressionS3tc	= 0;
	bool bExtTextureEnvAdd			= 0;
	bool bExtTextureEnvCombine		= 0;
	bool bExtTextureEnvCrossbar		= 0;
	bool bExtTextureEnvDot3			= 0;
	bool bExtTextureCubeMap			= 0;
	bool bExtTextureFloat			= 0;
	bool bExtColourBufferFloat		= 0;
	bool bExtCompiledVertexArrays	= 0;
	bool bExtVertexBufferObjects	= 0;
	bool bExtMappedBuffer			= 0;
	bool bExtAnisotropicFiltering	= 0;
	bool bExtVertexPrograms			= 0;
	bool bExtFragmentPrograms		= 0;
	bool bExtShaderObjects			= 0;
	bool bExtVertexShaders			= 0;
	bool bExtFragmentShaders		= 0;
	bool bExtGeometryShaders		= 0;
	bool bExtGLShadingLanguage		= 0;
	bool bExtPixelBufferObjects		= 0;
	bool bExtFrameBufferObject		= 0;
	bool bExtFrameBufferBlit		= 0;
	bool bExtFrameBufferMultisample	= 0;
	bool bExtTextureRectangle		= 0;
	bool bExtTextureNonPowerOfTwo	= 0;
	bool bExtTextureBorderClamp		= 0;
	bool bExtShadow					= 0;
	bool bExtDepthTexture			= 0;
	bool bExtTextureEdgeClamp		= 0;
	bool bExtSeparateSpecularColor	= 0;
	bool bExtDrawBuffers			= 0;
	bool bExtPackedDepthStencil		= 0;
} // namespace renderer

#if defined( BASE_PLATFORM_WINDOWS )

	PFNWGLSWAPINTERVALEXTPROC			wglSwapIntervalEXT			= 0;
	PFNWGLCHOOSEPIXELFORMATARBPROC		wglChoosePixelFormatARB		= 0;

	// multitexturing
	PFNGLACTIVETEXTUREPROC					glActiveTexture			= 0;
	PFNGLCLIENTACTIVETEXTUREPROC			glClientActiveTexture	= 0;
	PFNGLMULTITEXCOORD1DPROC				glMultiTexCoord1d		= 0;
	PFNGLMULTITEXCOORD1DVPROC				glMultiTexCoord1dv		= 0;
	PFNGLMULTITEXCOORD1FPROC				glMultiTexCoord1f		= 0;
	PFNGLMULTITEXCOORD1FVPROC				glMultiTexCoord1fv		= 0;
	PFNGLMULTITEXCOORD1IPROC				glMultiTexCoord1i		= 0;
	PFNGLMULTITEXCOORD1IVPROC				glMultiTexCoord1iv		= 0;
	PFNGLMULTITEXCOORD1SPROC				glMultiTexCoord1s		= 0;
	PFNGLMULTITEXCOORD1SVPROC				glMultiTexCoord1sv		= 0;
	PFNGLMULTITEXCOORD2DPROC				glMultiTexCoord2d		= 0;
	PFNGLMULTITEXCOORD2DVPROC				glMultiTexCoord2dv		= 0;
	PFNGLMULTITEXCOORD2FPROC				glMultiTexCoord2f		= 0;
	PFNGLMULTITEXCOORD2FVPROC				glMultiTexCoord2fv		= 0;
	PFNGLMULTITEXCOORD2IPROC				glMultiTexCoord2i		= 0;
	PFNGLMULTITEXCOORD2IVPROC				glMultiTexCoord2iv		= 0;
	PFNGLMULTITEXCOORD2SPROC				glMultiTexCoord2s		= 0;
	PFNGLMULTITEXCOORD2SVPROC				glMultiTexCoord2sv		= 0;
	PFNGLMULTITEXCOORD3DPROC				glMultiTexCoord3d		= 0;
	PFNGLMULTITEXCOORD3DVPROC				glMultiTexCoord3dv		= 0;
	PFNGLMULTITEXCOORD3FPROC				glMultiTexCoord3f		= 0;
	PFNGLMULTITEXCOORD3FVPROC				glMultiTexCoord3fv		= 0;
	PFNGLMULTITEXCOORD3IPROC				glMultiTexCoord3i		= 0;
	PFNGLMULTITEXCOORD3IVPROC				glMultiTexCoord3iv		= 0;
	PFNGLMULTITEXCOORD3SPROC				glMultiTexCoord3s		= 0;
	PFNGLMULTITEXCOORD3SVPROC				glMultiTexCoord3sv		= 0;
	PFNGLMULTITEXCOORD4DPROC				glMultiTexCoord4d		= 0;
	PFNGLMULTITEXCOORD4DVPROC				glMultiTexCoord4dv		= 0;
	PFNGLMULTITEXCOORD4FPROC				glMultiTexCoord4f		= 0;
	PFNGLMULTITEXCOORD4FVPROC				glMultiTexCoord4fv		= 0;
	PFNGLMULTITEXCOORD4IPROC				glMultiTexCoord4i		= 0;
	PFNGLMULTITEXCOORD4IVPROC				glMultiTexCoord4iv		= 0;
	PFNGLMULTITEXCOORD4SPROC				glMultiTexCoord4s		= 0;
	PFNGLMULTITEXCOORD4SVPROC				glMultiTexCoord4sv		= 0;

	// texture 3D
	PFNGLTEXIMAGE3DPROC						glTexImage3D			= 0;
	PFNGLTEXSUBIMAGE3DPROC					glTexSubImage3D			= 0;

	// fog coord
	PFNGLFOGCOORDFPROC						glFogCoordf				= 0;
	PFNGLFOGCOORDFVPROC						glFogCoordfv			= 0;
	PFNGLFOGCOORDDPROC						glFogCoordd				= 0;
	PFNGLFOGCOORDDVPROC						glFogCoorddv			= 0;
	PFNGLFOGCOORDPOINTERPROC				glFogCoordPointer		= 0;

	// point sprites

	// point parameters
	PFNGLPOINTPARAMETERFPROC				glPointParameterf		= 0;
	PFNGLPOINTPARAMETERFVPROC				glPointParameterfv		= 0;

	// texture compression
	PFNGLCOMPRESSEDTEXIMAGE3DPROC			glCompressedTexImage3D		= 0;
	PFNGLCOMPRESSEDTEXIMAGE2DPROC			glCompressedTexImage2D		= 0;
	PFNGLCOMPRESSEDTEXIMAGE1DPROC			glCompressedTexImage1D		= 0;
	PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC		glCompressedTexSubImage3D	= 0;
	PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC		glCompressedTexSubImage2D	= 0;
	PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC		glCompressedTexSubImage1D	= 0;
	PFNGLGETCOMPRESSEDTEXIMAGEPROC			glGetCompressedTexImage		= 0;

	// environment modes

	// colour buffer float
	PFNGLCLAMPCOLORARBPROC					glClampColorARB			= 0;

	// compiled vertex arrays
	PFNGLLOCKARRAYSEXTPROC					glLockArraysEXT			= 0;
	PFNGLUNLOCKARRAYSEXTPROC				glUnlockArraysEXT		= 0;

	// vertex buffer objects
	PFNGLBINDBUFFERPROC						glBindBuffer			= 0;
	PFNGLDELETEBUFFERSPROC					glDeleteBuffers			= 0;
	PFNGLGENBUFFERSPROC						glGenBuffers			= 0;
	PFNGLISBUFFERPROC						glIsBuffer				= 0;
	PFNGLBUFFERDATAPROC						glBufferData			= 0;
	PFNGLBUFFERSUBDATAPROC					glBufferSubData			= 0;
	PFNGLGETBUFFERSUBDATAPROC				glGetBufferSubData		= 0;
	PFNGLMAPBUFFERPROC						glMapBuffer				= 0;
	PFNGLUNMAPBUFFERPROC					glUnmapBuffer			= 0;
	PFNGLGETBUFFERPARAMETERIVPROC			glGetBufferParameteriv	= 0;
	PFNGLGETBUFFERPOINTERVPROC				glGetBufferPointerv		= 0;

	// anisotropic filtering

	// ARB vertex/fragment programs
	PFNGLVERTEXATTRIB1DARBPROC					glVertexAttrib1dARB = 0;
	PFNGLVERTEXATTRIB1DVARBPROC					glVertexAttrib1dvARB = 0;
	PFNGLVERTEXATTRIB1FARBPROC					glVertexAttrib1fARB = 0;
	PFNGLVERTEXATTRIB1FVARBPROC					glVertexAttrib1fvARB = 0;
	PFNGLVERTEXATTRIB1SARBPROC					glVertexAttrib1sARB = 0;
	PFNGLVERTEXATTRIB1SVARBPROC					glVertexAttrib1svARB = 0;
	PFNGLVERTEXATTRIB2DARBPROC					glVertexAttrib2dARB = 0;
	PFNGLVERTEXATTRIB2DVARBPROC					glVertexAttrib2dvARB = 0;
	PFNGLVERTEXATTRIB2FARBPROC					glVertexAttrib2fARB = 0;
	PFNGLVERTEXATTRIB2FVARBPROC					glVertexAttrib2fvARB = 0;
	PFNGLVERTEXATTRIB2SARBPROC					glVertexAttrib2sARB = 0;
	PFNGLVERTEXATTRIB2SVARBPROC					glVertexAttrib2svARB = 0;
	PFNGLVERTEXATTRIB3DARBPROC					glVertexAttrib3dARB = 0;
	PFNGLVERTEXATTRIB3DVARBPROC					glVertexAttrib3dvARB = 0;
	PFNGLVERTEXATTRIB3FARBPROC					glVertexAttrib3fARB = 0;
	PFNGLVERTEXATTRIB3FVARBPROC					glVertexAttrib3fvARB = 0;
	PFNGLVERTEXATTRIB3SARBPROC					glVertexAttrib3sARB = 0;
	PFNGLVERTEXATTRIB3SVARBPROC					glVertexAttrib3svARB = 0;
	PFNGLVERTEXATTRIB4NBVARBPROC				glVertexAttrib4NbvARB = 0;
	PFNGLVERTEXATTRIB4NIVARBPROC				glVertexAttrib4NivARB = 0;
	PFNGLVERTEXATTRIB4NSVARBPROC				glVertexAttrib4NsvARB = 0;
	PFNGLVERTEXATTRIB4NUBARBPROC				glVertexAttrib4NubARB = 0;
	PFNGLVERTEXATTRIB4NUBVARBPROC				glVertexAttrib4NubvARB = 0;
	PFNGLVERTEXATTRIB4NUIVARBPROC				glVertexAttrib4NuivARB = 0;
	PFNGLVERTEXATTRIB4NUSVARBPROC				glVertexAttrib4NusvARB = 0;
	PFNGLVERTEXATTRIB4BVARBPROC					glVertexAttrib4bvARB = 0;
	PFNGLVERTEXATTRIB4DARBPROC					glVertexAttrib4dARB = 0;
	PFNGLVERTEXATTRIB4DVARBPROC					glVertexAttrib4dvARB = 0;
	PFNGLVERTEXATTRIB4FARBPROC					glVertexAttrib4fARB = 0;
	PFNGLVERTEXATTRIB4FVARBPROC					glVertexAttrib4fvARB = 0;
	PFNGLVERTEXATTRIB4IVARBPROC					glVertexAttrib4ivARB = 0;
	PFNGLVERTEXATTRIB4SARBPROC					glVertexAttrib4sARB = 0;
	PFNGLVERTEXATTRIB4SVARBPROC					glVertexAttrib4svARB = 0;
	PFNGLVERTEXATTRIB4UBVARBPROC				glVertexAttrib4ubvARB = 0;
	PFNGLVERTEXATTRIB4UIVARBPROC				glVertexAttrib4uivARB = 0;
	PFNGLVERTEXATTRIB4USVARBPROC				glVertexAttrib4usvARB = 0;
	PFNGLVERTEXATTRIBPOINTERARBPROC				glVertexAttribPointerARB = 0;
	PFNGLENABLEVERTEXATTRIBARRAYARBPROC			glEnableVertexAttribArrayARB = 0;
	PFNGLDISABLEVERTEXATTRIBARRAYARBPROC		glDisableVertexAttribArrayARB = 0;
	PFNGLPROGRAMSTRINGARBPROC					glProgramStringARB = 0;
	PFNGLBINDPROGRAMARBPROC						glBindProgramARB = 0;
	PFNGLDELETEPROGRAMSARBPROC					glDeleteProgramsARB = 0;
	PFNGLGENPROGRAMSARBPROC						glGenProgramsARB = 0;
	PFNGLPROGRAMENVPARAMETER4DARBPROC			glProgramEnvParameter4dARB = 0;
	PFNGLPROGRAMENVPARAMETER4DVARBPROC			glProgramEnvParameter4dvARB = 0;
	PFNGLPROGRAMENVPARAMETER4FARBPROC			glProgramEnvParameter4fARB = 0;
	PFNGLPROGRAMENVPARAMETER4FVARBPROC			glProgramEnvParameter4fvARB = 0;
	PFNGLPROGRAMLOCALPARAMETER4DARBPROC			glProgramLocalParameter4dARB = 0;
	PFNGLPROGRAMLOCALPARAMETER4DVARBPROC		glProgramLocalParameter4dvARB = 0;
	PFNGLPROGRAMLOCALPARAMETER4FARBPROC			glProgramLocalParameter4fARB = 0;
	PFNGLPROGRAMLOCALPARAMETER4FVARBPROC		glProgramLocalParameter4fvARB = 0;
	PFNGLGETPROGRAMENVPARAMETERDVARBPROC		glGetProgramEnvParameterdvARB = 0;
	PFNGLGETPROGRAMENVPARAMETERFVARBPROC		glGetProgramEnvParameterfvARB = 0;
	PFNGLGETPROGRAMLOCALPARAMETERDVARBPROC		glGetProgramLocalParameterdvARB = 0;
	PFNGLGETPROGRAMLOCALPARAMETERFVARBPROC		glGetProgramLocalParameterfvARB = 0;
	PFNGLGETPROGRAMIVARBPROC					glGetProgramivARB = 0;
	PFNGLGETPROGRAMSTRINGARBPROC				glGetProgramStringARB = 0;
	PFNGLGETVERTEXATTRIBDVARBPROC				glGetVertexAttribdvARB = 0;
	PFNGLGETVERTEXATTRIBFVARBPROC				glGetVertexAttribfvARB = 0;
	PFNGLGETVERTEXATTRIBIVARBPROC				glGetVertexAttribivARB = 0;
	PFNGLGETVERTEXATTRIBPOINTERVARBPROC			glGetVertexAttribPointervARB = 0;
	PFNGLISPROGRAMARBPROC						glIsProgramARB = 0;

	// shader objects
	/*
	PFNGLDELETEOBJECTARBPROC                    glDeleteObjectARB           = 0;
	PFNGLGETHANDLEARBPROC                       glGetHandleARB              = 0;
	PFNGLDETACHOBJECTARBPROC                    glDetachObjectARB           = 0;
	PFNGLCREATESHADEROBJECTARBPROC              glCreateShaderObjectARB     = 0;
	PFNGLSHADERSOURCEARBPROC                    glShaderSourceARB           = 0;
	PFNGLCOMPILESHADERARBPROC                   glCompileShaderARB          = 0;
	PFNGLCREATEPROGRAMOBJECTARBPROC             glCreateProgramObjectARB    = 0;
	PFNGLATTACHOBJECTARBPROC                    glAttachObjectARB           = 0;
	PFNGLLINKPROGRAMARBPROC                     glLinkProgramARB            = 0;
	PFNGLUSEPROGRAMOBJECTARBPROC                glUseProgramObjectARB       = 0;
	PFNGLVALIDATEPROGRAMARBPROC					glValidateProgramARB        = 0;

	PFNGLUNIFORM1FARBPROC                       glUniform1fARB              = 0;
	PFNGLUNIFORM2FARBPROC                       glUniform2fARB              = 0;
	PFNGLUNIFORM3FARBPROC                       glUniform3fARB              = 0;
	PFNGLUNIFORM4FARBPROC                       glUniform4fARB              = 0;
	PFNGLUNIFORM1IARBPROC                       glUniform1iARB              = 0;
	PFNGLUNIFORM2IARBPROC                       glUniform2iARB              = 0;
	PFNGLUNIFORM3IARBPROC                       glUniform3iARB              = 0;
	PFNGLUNIFORM4IARBPROC                       glUniform4iARB              = 0;
	PFNGLUNIFORM1FVARBPROC                      glUniform1fvARB             = 0;
	PFNGLUNIFORM2FVARBPROC                      glUniform2fvARB             = 0;
	PFNGLUNIFORM3FVARBPROC                      glUniform3fvARB             = 0;
	PFNGLUNIFORM4FVARBPROC                      glUniform4fvARB             = 0;
	PFNGLUNIFORM1IVARBPROC                      glUniform1ivARB             = 0;
	PFNGLUNIFORM2IVARBPROC                      glUniform2ivARB             = 0;
	PFNGLUNIFORM3IVARBPROC                      glUniform3ivARB             = 0;
	PFNGLUNIFORM4IVARBPROC                      glUniform4ivARB             = 0;
	PFNGLUNIFORMMATRIX2FVARBPROC                glUniformMatrix2fvARB       = 0;
	PFNGLUNIFORMMATRIX3FVARBPROC                glUniformMatrix3fvARB       = 0;
	PFNGLUNIFORMMATRIX4FVARBPROC                glUniformMatrix4fvARB       = 0;

	PFNGLGETOBJECTPARAMETERFVARBPROC            glGetObjectParameterfvARB   = 0;
	PFNGLGETOBJECTPARAMETERIVARBPROC            glGetObjectParameterivARB   = 0;
	PFNGLGETINFOLOGARBPROC                      glGetInfoLogARB             = 0;
	PFNGLGETATTACHEDOBJECTSARBPROC              glGetAttachedObjectsARB     = 0;
	PFNGLGETUNIFORMLOCATIONARBPROC              glGetUniformLocationARB     = 0;
	PFNGLGETACTIVEUNIFORMARBPROC                glGetActiveUniformARB       = 0;
	PFNGLGETUNIFORMFVARBPROC                    glGetUniformfvARB           = 0;
	PFNGLGETUNIFORMIVARBPROC                    glGetUniformivARB           = 0;
	PFNGLGETSHADERSOURCEARBPROC                 glGetShaderSourceARB        = 0;

	// ARB vertex/fragment shaders
	PFNGLBINDATTRIBLOCATIONARBPROC				glBindAttribLocationARB = 0;
	PFNGLGETACTIVEATTRIBARBPROC					glGetActiveAttribARB = 0;
	PFNGLGETATTRIBLOCATIONARBPROC				glGetAttribLocationARB = 0;

	// shading language
	*/

	PFNGLATTACHSHADERPROC						glAttachShader = 0;
	PFNGLBINDATTRIBLOCATIONPROC					glBindAttribLocation = 0;
	PFNGLCOMPILESHADERPROC						glCompileShader = 0;
	PFNGLCREATEPROGRAMPROC						glCreateProgram = 0;
	PFNGLCREATESHADERPROC						glCreateShader = 0;
	PFNGLDELETEPROGRAMPROC						glDeleteProgram = 0;
	PFNGLDELETESHADERPROC						glDeleteShader = 0;
	PFNGLDETACHSHADERPROC						glDetachShader = 0;
	PFNGLDISABLEVERTEXATTRIBARRAYPROC			glDisableVertexAttribArray = 0;
	PFNGLENABLEVERTEXATTRIBARRAYPROC			glEnableVertexAttribArray = 0;
	PFNGLGETACTIVEATTRIBPROC					glGetActiveAttrib = 0;
	PFNGLGETACTIVEUNIFORMPROC					glGetActiveUniform = 0;
	PFNGLGETATTACHEDSHADERSPROC					glGetAttachedShaders = 0;
	PFNGLGETATTRIBLOCATIONPROC					glGetAttribLocation = 0;
	PFNGLGETPROGRAMIVPROC						glGetProgramiv = 0;
	PFNGLGETPROGRAMINFOLOGPROC					glGetProgramInfoLog = 0;
	PFNGLGETSHADERIVPROC						glGetShaderiv = 0;
	PFNGLGETSHADERINFOLOGPROC					glGetShaderInfoLog = 0;
	PFNGLGETSHADERSOURCEPROC					glGetShaderSource = 0;
	PFNGLGETUNIFORMLOCATIONPROC					glGetUniformLocation = 0;
	PFNGLGETUNIFORMFVPROC						glGetUniformfv = 0;
	PFNGLGETUNIFORMIVPROC						glGetUniformiv = 0;
	PFNGLGETVERTEXATTRIBDVPROC					glGetVertexAttribdv = 0;
	PFNGLGETVERTEXATTRIBFVPROC					glGetVertexAttribfv = 0;
	PFNGLGETVERTEXATTRIBIVPROC					glGetVertexAttribiv = 0;
	PFNGLGETVERTEXATTRIBPOINTERVPROC			glGetVertexAttribPointerv = 0;
	PFNGLISPROGRAMPROC							glIsProgram = 0;
	PFNGLISSHADERPROC							glIsShader = 0;
	PFNGLLINKPROGRAMPROC						glLinkProgram = 0;
	PFNGLSHADERSOURCEPROC						glShaderSource = 0;
	PFNGLUSEPROGRAMPROC							glUseProgram = 0;
	PFNGLUNIFORM1FPROC							glUniform1f = 0;
	PFNGLUNIFORM2FPROC							glUniform2f = 0;
	PFNGLUNIFORM3FPROC							glUniform3f = 0;
	PFNGLUNIFORM4FPROC							glUniform4f = 0;
	PFNGLUNIFORM1IPROC							glUniform1i = 0;
	PFNGLUNIFORM2IPROC							glUniform2i = 0;
	PFNGLUNIFORM3IPROC							glUniform3i = 0;
	PFNGLUNIFORM4IPROC							glUniform4i = 0;
	PFNGLUNIFORM1FVPROC							glUniform1fv = 0;
	PFNGLUNIFORM2FVPROC							glUniform2fv = 0;
	PFNGLUNIFORM3FVPROC							glUniform3fv = 0;
	PFNGLUNIFORM4FVPROC							glUniform4fv = 0;
	PFNGLUNIFORM1IVPROC							glUniform1iv = 0;
	PFNGLUNIFORM2IVPROC							glUniform2iv = 0;
	PFNGLUNIFORM3IVPROC							glUniform3iv = 0;
	PFNGLUNIFORM4IVPROC							glUniform4iv = 0;
	PFNGLUNIFORMMATRIX2FVPROC					glUniformMatrix2fv = 0;
	PFNGLUNIFORMMATRIX3FVPROC					glUniformMatrix3fv = 0;
	PFNGLUNIFORMMATRIX4FVPROC					glUniformMatrix4fv = 0;
	PFNGLVALIDATEPROGRAMPROC					glValidateProgram = 0;
	PFNGLVERTEXATTRIB1DPROC						glVertexAttrib1d = 0;
	PFNGLVERTEXATTRIB1DVPROC					glVertexAttrib1dv = 0;
	PFNGLVERTEXATTRIB1FPROC						glVertexAttrib1f = 0;
	PFNGLVERTEXATTRIB1FVPROC					glVertexAttrib1fv = 0;
	PFNGLVERTEXATTRIB1SPROC						glVertexAttrib1s = 0;
	PFNGLVERTEXATTRIB1SVPROC					glVertexAttrib1sv = 0;
	PFNGLVERTEXATTRIB2DPROC						glVertexAttrib2d = 0;
	PFNGLVERTEXATTRIB2DVPROC					glVertexAttrib2dv = 0;
	PFNGLVERTEXATTRIB2FPROC						glVertexAttrib2f = 0;
	PFNGLVERTEXATTRIB2FVPROC					glVertexAttrib2fv = 0;
	PFNGLVERTEXATTRIB2SPROC						glVertexAttrib2s = 0;
	PFNGLVERTEXATTRIB2SVPROC					glVertexAttrib2sv = 0;
	PFNGLVERTEXATTRIB3DPROC						glVertexAttrib3d = 0;
	PFNGLVERTEXATTRIB3DVPROC					glVertexAttrib3dv = 0;
	PFNGLVERTEXATTRIB3FPROC						glVertexAttrib3f = 0;
	PFNGLVERTEXATTRIB3FVPROC					glVertexAttrib3fv = 0;
	PFNGLVERTEXATTRIB3SPROC						glVertexAttrib3s = 0;
	PFNGLVERTEXATTRIB3SVPROC					glVertexAttrib3sv = 0;
	PFNGLVERTEXATTRIB4NBVPROC					glVertexAttrib4Nbv = 0;
	PFNGLVERTEXATTRIB4NIVPROC					glVertexAttrib4Niv = 0;
	PFNGLVERTEXATTRIB4NSVPROC					glVertexAttrib4Nsv = 0;
	PFNGLVERTEXATTRIB4NUBPROC					glVertexAttrib4Nub = 0;
	PFNGLVERTEXATTRIB4NUBVPROC					glVertexAttrib4Nubv = 0;
	PFNGLVERTEXATTRIB4NUIVPROC					glVertexAttrib4Nuiv = 0;
	PFNGLVERTEXATTRIB4NUSVPROC					glVertexAttrib4Nusv = 0;
	PFNGLVERTEXATTRIB4BVPROC					glVertexAttrib4bv = 0;
	PFNGLVERTEXATTRIB4DPROC						glVertexAttrib4d = 0;
	PFNGLVERTEXATTRIB4DVPROC					glVertexAttrib4dv = 0;
	PFNGLVERTEXATTRIB4FPROC						glVertexAttrib4f = 0;
	PFNGLVERTEXATTRIB4FVPROC					glVertexAttrib4fv = 0;
	PFNGLVERTEXATTRIB4IVPROC					glVertexAttrib4iv = 0;
	PFNGLVERTEXATTRIB4SPROC						glVertexAttrib4s = 0;
	PFNGLVERTEXATTRIB4SVPROC					glVertexAttrib4sv = 0;
	PFNGLVERTEXATTRIB4UBVPROC					glVertexAttrib4ubv = 0;
	PFNGLVERTEXATTRIB4UIVPROC					glVertexAttrib4uiv = 0;
	PFNGLVERTEXATTRIB4USVPROC					glVertexAttrib4usv = 0;
	PFNGLVERTEXATTRIBPOINTERPROC				glVertexAttribPointer = 0;

	// geometry shader
	PFNGLPROGRAMPARAMETERIEXTPROC				glProgramParameteriEXT = 0;

	// frame buffer object
	PFNGLISRENDERBUFFEREXTPROC						glIsRenderbufferEXT							= 0;
	PFNGLBINDRENDERBUFFEREXTPROC					glBindRenderbufferEXT						= 0;
	PFNGLDELETERENDERBUFFERSEXTPROC					glDeleteRenderbuffersEXT					= 0;
	PFNGLGENRENDERBUFFERSEXTPROC					glGenRenderbuffersEXT						= 0;
	PFNGLRENDERBUFFERSTORAGEEXTPROC					glRenderbufferStorageEXT					= 0;
	PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC			glGetRenderbufferParameterivEXT				= 0;
	PFNGLISFRAMEBUFFEREXTPROC						glIsFramebufferEXT							= 0;
	PFNGLBINDFRAMEBUFFEREXTPROC						glBindFramebufferEXT						= 0;
	PFNGLDELETEFRAMEBUFFERSEXTPROC					glDeleteFramebuffersEXT						= 0;
	PFNGLGENFRAMEBUFFERSEXTPROC						glGenFramebuffersEXT						= 0;
	PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC				glCheckFramebufferStatusEXT					= 0;
	PFNGLFRAMEBUFFERTEXTURE1DEXTPROC				glFramebufferTexture1DEXT					= 0;
	PFNGLFRAMEBUFFERTEXTURE2DEXTPROC				glFramebufferTexture2DEXT					= 0;
	PFNGLFRAMEBUFFERTEXTURE3DEXTPROC				glFramebufferTexture3DEXT					= 0;
	PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC				glFramebufferRenderbufferEXT				= 0;
	PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC	glGetFramebufferAttachmentParameterivEXT	= 0;
	PFNGLGENERATEMIPMAPEXTPROC						glGenerateMipmapEXT							= 0;

	// frame buffer blit
	PFNGLBLITFRAMEBUFFEREXTPROC						glBlitFramebufferEXT						= 0;

	// frame buffer multisample
	PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC		glRenderbufferStorageMultisampleEXT			= 0;

	// draw buffers
	PFNGLDRAWBUFFERSPROC							glDrawBuffers								= 0;
#endif // BASE_PLATFORM_WINDOWS

/////////////////////////////////////////////////////
/// Function: isExtensionSupported
/// Params: [in]extName
///
/////////////////////////////////////////////////////
bool renderer::isExtensionSupported(const char *extName)
{
//	char ext[4096];
//	std::sprintf( ext, "%s", glGetString(GL_EXTENSIONS) );
//	return (0 != std::strstr(ext, extName));

	if( core::IsEmptyString( extName ) )
		return 0;

	static const GLubyte *extensions = 0;
	const GLubyte *start = 0;
	GLubyte *where = 0, *terminator = 0;

	// Extension names should not have spaces
	where = (GLubyte *) strchr(extName, ' ');
	if (where || *extName == '\0')
		return(false);

	if (!extensions) 
	{
		extensions = glGetString(GL_EXTENSIONS);
	}
	// It takes a bit of care to be fool-proof about parsing the
	// OpenGL extensions string.  Don't be fooled by sub-strings,
	// etc. 
	start = extensions;
	for (;;) 
	{
		where = (GLubyte *) std::strstr((const char *) start, extName);
		if (!where)
			break;
		terminator = where + std::strlen(extName);
		if (where == start || *(where - 1) == ' ') 
		{
			if (*terminator == ' ' || *terminator == '\0') 
			{
				return(true);
			}
		}
		start = terminator;
	}

	return(false);
}

/////////////////////////////////////////////////////
/// Function: isWGLExtensionSupported
/// Params: [in]extName
///
/////////////////////////////////////////////////////
bool renderer::isWGLExtensionSupported(const char *extName)
{
#ifdef BASE_PLATFORM_WINDOWS
	const size_t extlen = std::strlen(extName);
	const char *supported = 0;

	// Try To Use wglGetExtensionStringARB On Current DC, If Possible
	PROC wglGetExtString = wglGetProcAddress("wglGetExtensionsStringARB");

	if (wglGetExtString)
		supported = ((char*(__stdcall*)(HDC))wglGetExtString)(wglGetCurrentDC());

	// If That Failed, Try Standard Opengl Extensions String
	if (supported == 0)
		supported = (char*)glGetString(GL_EXTENSIONS);

	// If That Failed Too, Must Be No Extensions Supported
	if (supported == 0)
		return false;

	// Begin Examination At Start Of String, Increment By 1 On False Match
	for (const char* p = supported; ; p++)
	{
		// Advance p Up To The Next Possible Match
		p = strstr(p, extName);

		if (p == 0)
			return false;															// No Match

		// Make Sure That Match Is At The Start Of The String Or That
		// The Previous Char Is A Space, Or Else We Could Accidentally
		// Match "wglFunkywglExtension" With "wglExtension"

		// Also, Make Sure That The Following Character Is Space Or 0
		// Or Else "wglExtensionTwo" Might Match "wglExtension"
		if ((p==supported || p[-1]==' ') && (p[extlen]=='\0' || p[extlen]==' '))
			return true;															// Match
	}
#endif // BASE_PLATFORM_WINDOWS
	return false;
}

/////////////////////////////////////////////////////
/// Function: RequestExtensions
/// Params: [in]nExtRequests
///
/////////////////////////////////////////////////////
void renderer::RequestExtensions( unsigned int nExtRequests, renderer::OpenGL* openGLContext )
{
	int i = 0;

#if defined( BASE_PLATFORM_WINDOWS )
	if( renderer::isExtensionSupported( "WGL_EXT_swap_control" ) )
	{
		wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)GetProcAddress("wglSwapIntervalEXT");
	}
	if( renderer::isWGLExtensionSupported( "WGL_ARB_pixel_format" ) )
	{
		wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)GetProcAddress("wglChoosePixelFormatARB");
	}

	// multisampling
	if( renderer::isWGLExtensionSupported( "WGL_ARB_multisample" ) )
		bExtMultisample = 1;

#endif // BASE_PLATFORM_WINDOWS

	//if( nExtRequests & EXT_MULTITEXTURE )
	{
		// multitexturing
		bExtMultiTexture = isExtensionSupported( "GL_ARB_multitexture" );
		
		// check core first
		if( SupportsGLVersion( 1, 3 ) )
		{
			bExtMultiTexture = 1;
			DBGLOG( "EXTENSIONS:  GL_ARB_multitexture/GL Core 1.3 is supported\n" );

#if defined( BASE_PLATFORM_WINDOWS )
			glActiveTexture			= (PFNGLACTIVETEXTUREARBPROC)GetProcAddress("glActiveTexture");
			glClientActiveTexture	= (PFNGLCLIENTACTIVETEXTUREARBPROC)GetProcAddress("glClientActiveTexture");
			glMultiTexCoord1d		= (PFNGLMULTITEXCOORD1DARBPROC)GetProcAddress("glMultiTexCoord1d");
			glMultiTexCoord1dv		= (PFNGLMULTITEXCOORD1DVARBPROC)GetProcAddress("glMultiTexCoord1dv");
			glMultiTexCoord1f		= (PFNGLMULTITEXCOORD1FARBPROC)GetProcAddress("glMultiTexCoord1f");
			glMultiTexCoord1fv		= (PFNGLMULTITEXCOORD1FVARBPROC)GetProcAddress("glMultiTexCoord1fv");
			glMultiTexCoord1i		= (PFNGLMULTITEXCOORD1IARBPROC)GetProcAddress("glMultiTexCoord1i");
			glMultiTexCoord1iv		= (PFNGLMULTITEXCOORD1IVARBPROC)GetProcAddress("glMultiTexCoord1iv");
			glMultiTexCoord1s		= (PFNGLMULTITEXCOORD1SARBPROC)GetProcAddress("glMultiTexCoord1s");
			glMultiTexCoord1sv		= (PFNGLMULTITEXCOORD1SVARBPROC)GetProcAddress("glMultiTexCoord1sv");
			glMultiTexCoord2d		= (PFNGLMULTITEXCOORD2DARBPROC)GetProcAddress("glMultiTexCoord2d");
			glMultiTexCoord2dv		= (PFNGLMULTITEXCOORD2DVARBPROC)GetProcAddress("glMultiTexCoord2dv");
			glMultiTexCoord2f		= (PFNGLMULTITEXCOORD2FARBPROC)GetProcAddress("glMultiTexCoord2f");
			glMultiTexCoord2fv		= (PFNGLMULTITEXCOORD2FVARBPROC)GetProcAddress("glMultiTexCoord2fv");
			glMultiTexCoord2i		= (PFNGLMULTITEXCOORD2IARBPROC)GetProcAddress("glMultiTexCoord2i");
			glMultiTexCoord2iv		= (PFNGLMULTITEXCOORD2IVARBPROC)GetProcAddress("glMultiTexCoord2iv");
			glMultiTexCoord2s		= (PFNGLMULTITEXCOORD2SARBPROC)GetProcAddress("glMultiTexCoord2s");
			glMultiTexCoord2sv		= (PFNGLMULTITEXCOORD2SVARBPROC)GetProcAddress("glMultiTexCoord2sv");
			glMultiTexCoord3d		= (PFNGLMULTITEXCOORD3DARBPROC)GetProcAddress("glMultiTexCoord3d");
			glMultiTexCoord3dv		= (PFNGLMULTITEXCOORD3DVARBPROC)GetProcAddress("glMultiTexCoord3dv");
			glMultiTexCoord3f		= (PFNGLMULTITEXCOORD3FARBPROC)GetProcAddress("glMultiTexCoord3f");
			glMultiTexCoord3fv		= (PFNGLMULTITEXCOORD3FVARBPROC)GetProcAddress("glMultiTexCoord3fv");
			glMultiTexCoord3i		= (PFNGLMULTITEXCOORD3IARBPROC)GetProcAddress("glMultiTexCoord3i");
			glMultiTexCoord3iv		= (PFNGLMULTITEXCOORD3IVARBPROC)GetProcAddress("glMultiTexCoord3iv");
			glMultiTexCoord3s		= (PFNGLMULTITEXCOORD3SARBPROC)GetProcAddress("glMultiTexCoord3s");
			glMultiTexCoord3sv		= (PFNGLMULTITEXCOORD3SVARBPROC)GetProcAddress("glMultiTexCoord3sv");
			glMultiTexCoord4d		= (PFNGLMULTITEXCOORD4DARBPROC)GetProcAddress("glMultiTexCoord4d");
			glMultiTexCoord4dv		= (PFNGLMULTITEXCOORD4DVARBPROC)GetProcAddress("glMultiTexCoord4dv");
			glMultiTexCoord4f		= (PFNGLMULTITEXCOORD4FARBPROC)GetProcAddress("glMultiTexCoord4f");
			glMultiTexCoord4fv		= (PFNGLMULTITEXCOORD4FVARBPROC)GetProcAddress("glMultiTexCoord4fv");
			glMultiTexCoord4i		= (PFNGLMULTITEXCOORD4IARBPROC)GetProcAddress("glMultiTexCoord4i");
			glMultiTexCoord4iv		= (PFNGLMULTITEXCOORD4IVARBPROC)GetProcAddress("glMultiTexCoord4iv");
			glMultiTexCoord4s		= (PFNGLMULTITEXCOORD4SARBPROC)GetProcAddress("glMultiTexCoord4s");
			glMultiTexCoord4sv		= (PFNGLMULTITEXCOORD4SVARBPROC)GetProcAddress("glMultiTexCoord4sv");
#endif // BASE_PLATFORM_WINDOWS

			GLint nMaxTextureUnits = 1;
			glGetIntegerv( GL_MAX_TEXTURE_UNITS, &nMaxTextureUnits );
			DBGLOG( "GL_MAX_TEXTURE_UNITS: %d\n", nMaxTextureUnits );

			if( openGLContext != 0 )
				openGLContext->SetMaxTextureUnits( nMaxTextureUnits );
			else
				renderer::OpenGL::GetInstance()->SetMaxTextureUnits( nMaxTextureUnits );
		}
		else if( bExtMultiTexture )
		{

#if defined( BASE_PLATFORM_WINDOWS )
			glActiveTexture			= (PFNGLACTIVETEXTUREARBPROC)GetProcAddress("glActiveTextureARB");
			glClientActiveTexture	= (PFNGLCLIENTACTIVETEXTUREARBPROC)GetProcAddress("glClientActiveTextureARB");
			glMultiTexCoord1d		= (PFNGLMULTITEXCOORD1DARBPROC)GetProcAddress("glMultiTexCoord1dARB");
			glMultiTexCoord1dv		= (PFNGLMULTITEXCOORD1DVARBPROC)GetProcAddress("glMultiTexCoord1dvARB");
			glMultiTexCoord1f		= (PFNGLMULTITEXCOORD1FARBPROC)GetProcAddress("glMultiTexCoord1fARB");
			glMultiTexCoord1fv		= (PFNGLMULTITEXCOORD1FVARBPROC)GetProcAddress("glMultiTexCoord1fvARB");
			glMultiTexCoord1i		= (PFNGLMULTITEXCOORD1IARBPROC)GetProcAddress("glMultiTexCoord1iARB");
			glMultiTexCoord1iv		= (PFNGLMULTITEXCOORD1IVARBPROC)GetProcAddress("glMultiTexCoord1ivARB");
			glMultiTexCoord1s		= (PFNGLMULTITEXCOORD1SARBPROC)GetProcAddress("glMultiTexCoord1sARB");
			glMultiTexCoord1sv		= (PFNGLMULTITEXCOORD1SVARBPROC)GetProcAddress("glMultiTexCoord1svARB");
			glMultiTexCoord2d		= (PFNGLMULTITEXCOORD2DARBPROC)GetProcAddress("glMultiTexCoord2dARB");
			glMultiTexCoord2dv		= (PFNGLMULTITEXCOORD2DVARBPROC)GetProcAddress("glMultiTexCoord2dvARB");
			glMultiTexCoord2f		= (PFNGLMULTITEXCOORD2FARBPROC)GetProcAddress("glMultiTexCoord2fARB");
			glMultiTexCoord2fv		= (PFNGLMULTITEXCOORD2FVARBPROC)GetProcAddress("glMultiTexCoord2fvARB");
			glMultiTexCoord2i		= (PFNGLMULTITEXCOORD2IARBPROC)GetProcAddress("glMultiTexCoord2iARB");
			glMultiTexCoord2iv		= (PFNGLMULTITEXCOORD2IVARBPROC)GetProcAddress("glMultiTexCoord2ivARB");
			glMultiTexCoord2s		= (PFNGLMULTITEXCOORD2SARBPROC)GetProcAddress("glMultiTexCoord2sARB");
			glMultiTexCoord2sv		= (PFNGLMULTITEXCOORD2SVARBPROC)GetProcAddress("glMultiTexCoord2svARB");
			glMultiTexCoord3d		= (PFNGLMULTITEXCOORD3DARBPROC)GetProcAddress("glMultiTexCoord3dARB");
			glMultiTexCoord3dv		= (PFNGLMULTITEXCOORD3DVARBPROC)GetProcAddress("glMultiTexCoord3dvARB");
			glMultiTexCoord3f		= (PFNGLMULTITEXCOORD3FARBPROC)GetProcAddress("glMultiTexCoord3fARB");
			glMultiTexCoord3fv		= (PFNGLMULTITEXCOORD3FVARBPROC)GetProcAddress("glMultiTexCoord3fvARB");
			glMultiTexCoord3i		= (PFNGLMULTITEXCOORD3IARBPROC)GetProcAddress("glMultiTexCoord3iARB");
			glMultiTexCoord3iv		= (PFNGLMULTITEXCOORD3IVARBPROC)GetProcAddress("glMultiTexCoord3ivARB");
			glMultiTexCoord3s		= (PFNGLMULTITEXCOORD3SARBPROC)GetProcAddress("glMultiTexCoord3sARB");
			glMultiTexCoord3sv		= (PFNGLMULTITEXCOORD3SVARBPROC)GetProcAddress("glMultiTexCoord3svARB");
			glMultiTexCoord4d		= (PFNGLMULTITEXCOORD4DARBPROC)GetProcAddress("glMultiTexCoord4dARB");
			glMultiTexCoord4dv		= (PFNGLMULTITEXCOORD4DVARBPROC)GetProcAddress("glMultiTexCoord4dvARB");
			glMultiTexCoord4f		= (PFNGLMULTITEXCOORD4FARBPROC)GetProcAddress("glMultiTexCoord4fARB");
			glMultiTexCoord4fv		= (PFNGLMULTITEXCOORD4FVARBPROC)GetProcAddress("glMultiTexCoord4fvARB");
			glMultiTexCoord4i		= (PFNGLMULTITEXCOORD4IARBPROC)GetProcAddress("glMultiTexCoord4iARB");
			glMultiTexCoord4iv		= (PFNGLMULTITEXCOORD4IVARBPROC)GetProcAddress("glMultiTexCoord4ivARB");
			glMultiTexCoord4s		= (PFNGLMULTITEXCOORD4SARBPROC)GetProcAddress("glMultiTexCoord4sARB");
			glMultiTexCoord4sv		= (PFNGLMULTITEXCOORD4SVARBPROC)GetProcAddress("glMultiTexCoord4svARB");
#endif // BASE_PLATFORM_WINDOWS

			DBGLOG( "EXTENSIONS:  GL_ARB_multitexture is supported\n" );

			GLint nMaxTextureUnits = 1;
			glGetIntegerv( GL_MAX_TEXTURE_UNITS_ARB, &nMaxTextureUnits );
			DBGLOG( "GL_MAX_TEXTURE_UNITS_ARB: %d\n", nMaxTextureUnits );

			if( openGLContext != 0 )
				openGLContext->SetMaxTextureUnits( nMaxTextureUnits );
			else
				renderer::OpenGL::GetInstance()->SetMaxTextureUnits( nMaxTextureUnits );

		}
		else
			DBGLOG( "EXTENSIONS:  GL_ARB_multitexture requested but not supported\n" );
	}

	// texture 3D
	//if( nExtRequests & EXT_TEXTURE3D )
	{
		// 3D textures
		bExtTexture3D = isExtensionSupported( "GL_EXT_texture3D" );
		
		// check core first
		if( SupportsGLVersion( 1, 2 ) )
		{
			bExtTexture3D = 1;

#if defined( BASE_PLATFORM_WINDOWS )
			glTexImage3D		= (PFNGLTEXIMAGE3DPROC)GetProcAddress("glTexImage3D");
			glTexSubImage3D		= (PFNGLTEXSUBIMAGE3DPROC)GetProcAddress("glTexSubImage3D");
#endif // BASE_PLATFORM_WINDOWS

			DBGLOG( "EXTENSIONS:  GL_EXT_texture3D/GL Core 1.2 is supported\n" );

			GLint nMax3DTexSize = 0;
			glGetIntegerv( GL_MAX_3D_TEXTURE_SIZE, &nMax3DTexSize );
			DBGLOG( "GL_MAX_3D_TEXTURE_SIZE: %dx%dx%d\n", nMax3DTexSize, nMax3DTexSize, nMax3DTexSize );

			if( openGLContext != 0 )
				openGLContext->SetMax3DTextureSize( nMax3DTexSize );
			else
				renderer::OpenGL::GetInstance()->SetMax3DTextureSize( nMax3DTexSize );
		}
		else if( bExtTexture3D )
		{

#if defined( BASE_PLATFORM_WINDOWS )
			glTexImage3D		= (PFNGLTEXIMAGE3DPROC)GetProcAddress("glTexImage3DEXT");
			glTexSubImage3D		= (PFNGLTEXSUBIMAGE3DPROC)GetProcAddress("glTexSubImage3DEXT");
#endif // BASE_PLATFORM_WINDOWS

			DBGLOG( "EXTENSIONS:  GL_EXT_texture3D is supported\n" );

			GLint nMax3DTexSize = 0;
			glGetIntegerv( GL_MAX_3D_TEXTURE_SIZE, &nMax3DTexSize );
			DBGLOG( "GL_MAX_3D_TEXTURE_SIZE_EXT: %dx%dx%d\n", nMax3DTexSize, nMax3DTexSize, nMax3DTexSize );

			if( openGLContext != 0 )
				openGLContext->SetMax3DTextureSize( nMax3DTexSize );
			else
				renderer::OpenGL::GetInstance()->SetMax3DTextureSize( nMax3DTexSize );
		}
		else
			DBGLOG( "EXTENSIONS:  GL_EXT_texture3D requested but not supported\n" );
	}

	// volumetric fog
	//if( nExtRequests & EXT_FOG_COORD )
	{
		bExtFogCoord = isExtensionSupported( "GL_EXT_fog_coord" );
		
		// check core first
		if( SupportsGLVersion( 1, 4 ) )
		{
			bExtFogCoord = 1;

#if defined( BASE_PLATFORM_WINDOWS )
			glFogCoordf			= (PFNGLFOGCOORDFPROC)GetProcAddress("glFogCoordf");
			glFogCoordfv		= (PFNGLFOGCOORDFVPROC)GetProcAddress("glFogCoordfv");
			glFogCoordd			= (PFNGLFOGCOORDDPROC)GetProcAddress("glFogCoordd");
			glFogCoorddv		= (PFNGLFOGCOORDDVPROC)GetProcAddress("glFogCoorddv");
			glFogCoordPointer	= (PFNGLFOGCOORDPOINTERPROC)GetProcAddress("glFogCoordPointer");
#endif // BASE_PLATFORM_WINDOWS

			DBGLOG( "EXTENSIONS:  GL_EXT_fog_coord/GL Core 1.4 is supported\n" );
		}
		else if( bExtFogCoord )
		{

#if defined( BASE_PLATFORM_WINDOWS )
			glFogCoordf			= (PFNGLFOGCOORDFPROC)GetProcAddress("glFogCoordfEXT");
			glFogCoordfv		= (PFNGLFOGCOORDFVPROC)GetProcAddress("glFogCoordfvEXT");
			glFogCoordd			= (PFNGLFOGCOORDDPROC)GetProcAddress("glFogCoorddEXT");
			glFogCoorddv		= (PFNGLFOGCOORDDVPROC)GetProcAddress("glFogCoorddvEXT");
			glFogCoordPointer	= (PFNGLFOGCOORDPOINTERPROC)GetProcAddress("glFogCoordPointerEXT");
#endif // BASE_PLATFORM_WINDOWS

			DBGLOG( "EXTENSIONS:  GL_EXT_fog_coord is supported\n" );
		}
		else
			DBGLOG( "EXTENSIONS:  GL_EXT_fog_coord requested but not supported\n" );
	}

	// point sprites
	//if( nExtRequests & EXT_POINT_SPRITES )
	{
		bExtPointSprites = isExtensionSupported( "GL_ARB_point_sprite" );

		bExtPointParameters = isExtensionSupported( "GL_ARB_point_parameters" )|| isExtensionSupported( "GL_EXT_point_parameters" );
		
		// check core first
		if( SupportsGLVersion( 2, 0 ) )
		{
			bExtPointSprites = 1;
			DBGLOG( "EXTENSIONS:  GL_ARB_point_sprite/GL Core 2.0 is supported\n" );
		}
		else if( bExtPointSprites )
		{
			DBGLOG( "EXTENSIONS:  GL_ARB_point_sprite is supported\n" );
		}
		else
		{
			DBGLOG( "EXTENSIONS:  GL_ARB_point_sprite requested but not supported\n" );
		}		
		
		if( SupportsGLVersion( 1, 4 ) )
		{
			bExtPointParameters = 1;

#if defined( BASE_PLATFORM_WINDOWS )
			glPointParameterf		= (PFNGLPOINTPARAMETERFPROC)GetProcAddress("glPointParameterf");
			glPointParameterfv		= (PFNGLPOINTPARAMETERFVPROC)GetProcAddress("glPointParameterfv");
#endif // BASE_PLATFORM_WINDOWS

			DBGLOG( "EXTENSIONS:  GL_ARB_point_parameters/GL Core 1.4 is supported\n" );

			GLint nPointSizeMin = 0;
			glGetIntegerv( GL_POINT_SIZE_MIN, &nPointSizeMin );
			DBGLOG( "GL_POINT_SIZE_MIN: %d\n", nPointSizeMin );

			GLint nPointSizeMax = 0;
			glGetIntegerv( GL_POINT_SIZE_MAX, &nPointSizeMax );
			DBGLOG( "GL_POINT_SIZE_MAX: %d\n", nPointSizeMax );

			GLint nPointFadeThresholdSize = 0;
			glGetIntegerv( GL_POINT_FADE_THRESHOLD_SIZE, &nPointFadeThresholdSize );
			DBGLOG( "GL_POINT_FADE_THRESHOLD_SIZE: %d\n", nPointFadeThresholdSize );
			
		}
		else if( bExtPointParameters )
		{

#if defined( BASE_PLATFORM_WINDOWS )
			glPointParameterf		= (PFNGLPOINTPARAMETERFPROC)GetProcAddress("glPointParameterfARB");
			glPointParameterfv		= (PFNGLPOINTPARAMETERFVPROC)GetProcAddress("glPointParameterfvARB");
#endif // BASE_PLATFORM_WINDOWS

			DBGLOG( "EXTENSIONS:  GL_ARB_point_parameters/GL_EXT_point_parameters is supported\n" );
		}
		else
		{
			DBGLOG( "EXTENSIONS:  GL_ARB_point_parameters requested but not supported\n" );
		}
	}

	//if( nExtRequests & EXT_TEXTURE_AUTOMIPMAP )
	{
		// hardware mimap generation
		bExtTextureAutomipmap = isExtensionSupported( "GL_SGIS_generate_mipmap" );
		
		if( bExtTextureAutomipmap )
		{	
			DBGLOG( "EXTENSIONS:  GL_SGIS_generate_mipmap is supported\n" );
		}
		else
			DBGLOG( "EXTENSIONS:  GL_SGIS_generate_mipmap requested but not supported\n" );
	}

	//if( nExtRequests & EXT_TEXTURE_LOD )
	{
		// texture lod
		bExtTextureLod = ( isExtensionSupported( "GL_EXT_texture_lod" ) || isExtensionSupported( "GL_SGIS_texture_lod" ) );

		// texture lod bias
		bExtTextureLodBias = isExtensionSupported( "GL_EXT_texture_lod_bias" );
		
		if( SupportsGLVersion( 1, 2 ) )
		{
			bExtTextureLod = 1;

			DBGLOG( "EXTENSIONS:  GL_EXT_texture_lod/GL Core 1.2 is supported\n" );
		}
		else if( bExtTextureLod )
		{	
			DBGLOG( "EXTENSIONS:  GL_EXT_texture_lod/GL_SGIS_texture_lod is supported\n" );
		}
		else
			DBGLOG( "EXTENSIONS:  GL_EXT_texture_lod requested but not supported\n" );

		if( SupportsGLVersion( 1, 4 ) )
		{
			bExtTextureLodBias = 1;

			DBGLOG( "EXTENSIONS:  GL_EXT_texture_lod_bias/GL Core 1.4 is supported\n" );

		}
		else if( bExtTextureLodBias )
		{	
			DBGLOG( "EXTENSIONS:  GL_EXT_texture_lod_bias is supported\n" );

		}
		else
			DBGLOG( "EXTENSIONS:  GL_EXT_texture_lod_bias requested but not supported\n" );
	}

	//if( nExtRequests & EXT_TEXTURE_COMPRESSION )
	{
		// texture compression
		bExtTextureCompression = isExtensionSupported( "GL_ARB_texture_compression" );
		
		if( SupportsGLVersion( 1, 3 ) )
		{
			bExtTextureCompression = 1;

#if defined( BASE_PLATFORM_WINDOWS )
			glCompressedTexImage3D		= (PFNGLCOMPRESSEDTEXIMAGE3DPROC)GetProcAddress("glCompressedTexImage3D");	
			glCompressedTexImage2D		= (PFNGLCOMPRESSEDTEXIMAGE2DPROC)GetProcAddress("glCompressedTexImage2D");		
			glCompressedTexImage1D		= (PFNGLCOMPRESSEDTEXIMAGE1DPROC)GetProcAddress("glCompressedTexImage1D");		
			glCompressedTexSubImage3D	= (PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC)GetProcAddress("glCompressedTexSubImage3D");	
			glCompressedTexSubImage2D	= (PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC)GetProcAddress("glCompressedTexSubImage2D");	
			glCompressedTexSubImage1D	= (PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC)GetProcAddress("glCompressedTexSubImage1D");	
			glGetCompressedTexImage		= (PFNGLGETCOMPRESSEDTEXIMAGEPROC)GetProcAddress("glGetCompressedTexImage");	
#endif // BASE_PLATFORM_WINDOWS

			DBGLOG( "EXTENSIONS:  GL_ARB_texture_compression/GL Core 1.3 is supported\n" );
		}
		else if( bExtTextureCompression )
		{

#if defined( BASE_PLATFORM_WINDOWS )
			glCompressedTexImage3D		= (PFNGLCOMPRESSEDTEXIMAGE3DPROC)GetProcAddress("glCompressedTexImage3DARB");	
			glCompressedTexImage2D		= (PFNGLCOMPRESSEDTEXIMAGE2DPROC)GetProcAddress("glCompressedTexImage2DARB");		
			glCompressedTexImage1D		= (PFNGLCOMPRESSEDTEXIMAGE1DPROC)GetProcAddress("glCompressedTexImage1DARB");		
			glCompressedTexSubImage3D	= (PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC)GetProcAddress("glCompressedTexSubImage3DARB");	
			glCompressedTexSubImage2D	= (PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC)GetProcAddress("glCompressedTexSubImage2DARB");	
			glCompressedTexSubImage1D	= (PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC)GetProcAddress("glCompressedTexSubImage1DARB");	
			glGetCompressedTexImage		= (PFNGLGETCOMPRESSEDTEXIMAGEPROC)GetProcAddress("glGetCompressedTexImageARB");	
#endif // BASE_PLATFORM_WINDOWS
		}
		else
			DBGLOG( "EXTENSIONS:  GL_ARB_texture_compression requested but not supported\n" );
		
		// S3TC/DXT
		bExtTextureCompressionS3tc = isExtensionSupported( "GL_EXT_texture_compression_s3tc" );

		if( bExtTextureCompressionS3tc )
			DBGLOG( "EXTENSIONS:  GL_EXT_texture_compression_s3tc is supported\n" );
		else
			DBGLOG( "EXTENSIONS:  GL_EXT_texture_compression_s3tc requested but not supported\n" );
	}

	//if( nExtRequests & EXT_TEXTURE_ENVIRONMENT_MODES )
	{

		bExtTextureEnvAdd = isExtensionSupported( "GL_ARB_texture_env_add" );
		bExtTextureEnvCombine = isExtensionSupported( "GL_ARB_texture_env_combine" );
		bExtTextureEnvCrossbar = isExtensionSupported( "GL_ARB_texture_env_crossbar" );
		bExtTextureEnvDot3 = isExtensionSupported( "GL_ARB_texture_env_dot3" );

		if( bExtTextureEnvAdd )
			DBGLOG( "EXTENSIONS:  GL_ARB_texture_env_add is supported\n" );
		else
			DBGLOG( "EXTENSIONS:  GL_ARB_texture_env_add requested but not supported\n" );

		if( SupportsGLVersion( 1, 3 ) )
		{
			bExtTextureEnvCombine = 1;
			DBGLOG( "EXTENSIONS:  GL_ARB_texture_env_combine/GL Core 1.3 is supported\n" );
		}
		else if( bExtTextureEnvCombine )
			DBGLOG( "EXTENSIONS:  GL_ARB_texture_env_combine is supported\n" );
		else
			DBGLOG( "EXTENSIONS:  GL_ARB_texture_env_combine requested but not supported\n" );

		if( bExtTextureEnvCrossbar )
			DBGLOG( "EXTENSIONS:  GL_ARB_texture_env_crossbar is supported\n" );
		else
			DBGLOG( "EXTENSIONS:  GL_ARB_texture_env_crossbar requested but not supported\n" );

		if( SupportsGLVersion( 1, 3 ) )
		{
			bExtTextureEnvDot3 = 1;

			DBGLOG( "EXTENSIONS:  GL_ARB_texture_env_dot3/GL Core 1.3 is supported\n" );
		}
		else if( bExtTextureEnvDot3 )
			DBGLOG( "EXTENSIONS:  GL_ARB_texture_env_dot3 is supported\n" );
		else
			DBGLOG( "EXTENSIONS:  GL_ARB_texture_env_dot3 requested but not supported\n" );

	}

	//if( nExtRequests & EXT_TEXTURE_CUBEMAP )
	{
		// cube map texturing
		bExtTextureCubeMap = ( isExtensionSupported( "GL_ARB_texture_cube_map" ) || isExtensionSupported( "GL_EXT_texture_cube_map" ) );
		
		if( SupportsGLVersion( 1, 3 ) )
		{
			bExtTextureCubeMap = 1;

			DBGLOG( "EXTENSIONS:  GL_ARB_texture_cube_map/GL Core 1.3 is supported\n" );

			GLint nMaxCubeMapTexSize = 0;
			glGetIntegerv( GL_MAX_CUBE_MAP_TEXTURE_SIZE, &nMaxCubeMapTexSize );
			DBGLOG( "GL_MAX_CUBE_MAP_TEXTURE_SIZE: %dx%d\n", nMaxCubeMapTexSize, nMaxCubeMapTexSize );

			if( openGLContext != 0 )
				openGLContext->SetMaxTextureCubemapSize( nMaxCubeMapTexSize );
			else
				renderer::OpenGL::GetInstance()->SetMaxTextureCubemapSize( nMaxCubeMapTexSize );
		}
		else if( bExtTextureCubeMap )
		{
			DBGLOG( "EXTENSIONS:  GL_ARB_texture_cube_map/GL_EXT_texture_cube_map is supported\n" );

			GLint nMaxCubeMapTexSize = 0;
			glGetIntegerv( GL_MAX_CUBE_MAP_TEXTURE_SIZE_ARB, &nMaxCubeMapTexSize );
			DBGLOG( "GL_MAX_CUBE_MAP_TEXTURE_SIZE_ARB: %dx%d\n", nMaxCubeMapTexSize, nMaxCubeMapTexSize );

			if( openGLContext != 0 )
				openGLContext->SetMaxTextureCubemapSize( nMaxCubeMapTexSize );
			else
				renderer::OpenGL::GetInstance()->SetMaxTextureCubemapSize( nMaxCubeMapTexSize );
		}
		else
			DBGLOG( "EXTENSIONS:  GL_ARB_texture_cube_map/GL_EXT_texture_cube_map requested but not supported\n" );
	}

	//if( nExtRequests & EXT_TEXTURE_FLOAT )
	{
		// floating point texturing
		bExtTextureFloat = isExtensionSupported( "GL_ARB_texture_float" );

		if( bExtTextureFloat )
		{
			DBGLOG( "EXTENSIONS:  GL_ARB_texture_float is supported\n" );
		}
		else
			DBGLOG( "EXTENSIONS:  GL_ARB_texture_float requested but not supported\n" );
	}

	//if( nExtRequests & EXT_COLOUR_BUFFER_FLOAT )
	{
		bExtColourBufferFloat = isExtensionSupported( "GL_ARB_color_buffer_float" );
		
		if( bExtColourBufferFloat )
		{

#if defined( BASE_PLATFORM_WINDOWS )
			glClampColorARB		= (PFNGLCLAMPCOLORARBPROC)GetProcAddress("glClampColorARB");	
#endif // BASE_PLATFORM_WINDOWS

			DBGLOG( "EXTENSIONS:  GL_ARB_color_buffer_float is supported\n" );
		}
		else
			DBGLOG( "EXTENSIONS:  GL_ARB_color_buffer_float requested but not supported\n" );
	}

	//if( nExtRequests & EXT_COMPILED_VERTEX_ARRAYS )
	{
		// compiled vertex arrays
		bExtCompiledVertexArrays = isExtensionSupported( "GL_EXT_compiled_vertex_array" );
		
		if( bExtCompiledVertexArrays )
		{	

#if defined( BASE_PLATFORM_WINDOWS )
			glLockArraysEXT   = (PFNGLLOCKARRAYSEXTPROC)GetProcAddress("glLockArraysEXT");
			glUnlockArraysEXT = (PFNGLUNLOCKARRAYSEXTPROC)GetProcAddress("glUnlockArraysEXT");
#endif // BASE_PLATFORM_WINDOWS

			DBGLOG( "EXTENSIONS:  GL_EXT_compiled_vertex_array is supported\n" );
		}
		else
			DBGLOG( "EXTENSIONS:  GL_EXT_compiled_vertex_array requested but not supported\n" );

	}

	//if( nExtRequests & EXT_VERTEX_BUFFER_OBJECTS )
	{
		// vertex buffer objects
		bExtMappedBuffer = bExtVertexBufferObjects = isExtensionSupported( "GL_ARB_vertex_buffer_object" );

		if( SupportsGLVersion( 1, 5 ) )
		{
			bExtVertexBufferObjects = 1;

#if defined( BASE_PLATFORM_WINDOWS )
			glBindBuffer			= (PFNGLBINDBUFFERPROC)GetProcAddress("glBindBuffer");
			glDeleteBuffers			= (PFNGLDELETEBUFFERSPROC)GetProcAddress("glDeleteBuffers");
			glGenBuffers			= (PFNGLGENBUFFERSPROC)GetProcAddress("glGenBuffers");
			glIsBuffer				= (PFNGLISBUFFERPROC)GetProcAddress("glIsBuffer");
			glBufferData			= (PFNGLBUFFERDATAPROC)GetProcAddress("glBufferData");
			glBufferSubData			= (PFNGLBUFFERSUBDATAPROC)GetProcAddress("glBufferSubData");
			glGetBufferSubData		= (PFNGLGETBUFFERSUBDATAPROC)GetProcAddress("glGetBufferSubData");
			glMapBuffer				= (PFNGLMAPBUFFERPROC)GetProcAddress("glMapBuffer");
			glUnmapBuffer			= (PFNGLUNMAPBUFFERPROC)GetProcAddress("glUnmapBuffer");
			glGetBufferParameteriv	= (PFNGLGETBUFFERPARAMETERIVPROC)GetProcAddress("glGetBufferParameteriv");
			glGetBufferPointerv		= (PFNGLGETBUFFERPOINTERVPROC)GetProcAddress("glGetBufferPointerv");
#endif // BASE_PLATFORM_WINDOWS

			DBGLOG( "EXTENSIONS:  GL_ARB_vertex_buffer_object/GL Core 1.5 is supported\n" );

		}
		else if( bExtVertexBufferObjects )
		{	

#if defined( BASE_PLATFORM_WINDOWS )
			glBindBuffer			= (PFNGLBINDBUFFERARBPROC)GetProcAddress("glBindBufferARB");
			glDeleteBuffers			= (PFNGLDELETEBUFFERSARBPROC)GetProcAddress("glDeleteBuffersARB");
			glGenBuffers			= (PFNGLGENBUFFERSARBPROC)GetProcAddress("glGenBuffersARB");
			glIsBuffer				= (PFNGLISBUFFERARBPROC)GetProcAddress("glIsBufferARB");
			glBufferData			= (PFNGLBUFFERDATAARBPROC)GetProcAddress("glBufferDataARB");
			glBufferSubData			= (PFNGLBUFFERSUBDATAARBPROC)GetProcAddress("glBufferSubDataARB");
			glGetBufferSubData		= (PFNGLGETBUFFERSUBDATAARBPROC)GetProcAddress("glGetBufferSubDataARB");
			glMapBuffer				= (PFNGLMAPBUFFERARBPROC)GetProcAddress("glMapBufferARB");
			glUnmapBuffer			= (PFNGLUNMAPBUFFERARBPROC)GetProcAddress("glUnmapBufferARB");
			glGetBufferParameteriv	= (PFNGLGETBUFFERPARAMETERIVARBPROC)GetProcAddress("glGetBufferParameterivARB");
			glGetBufferPointerv		= (PFNGLGETBUFFERPOINTERVARBPROC)GetProcAddress("glGetBufferPointervARB");
#endif // BASE_PLATFORM_WINDOWS
				
			DBGLOG( "EXTENSIONS:  GL_ARB_vertex_buffer_object is supported\n" );
		}
		else
			DBGLOG( "EXTENSIONS:  GL_ARB_vertex_buffer_object requested but not supported\n" );
	}

	//if( nExtRequests & EXT_ANISOTROPIC_FILTER )
	{
		// anisotropic filtering
		bExtAnisotropicFiltering = ( isExtensionSupported( "GL_EXT_texture_filter_anisotropic" ) );

		if( bExtAnisotropicFiltering )
		{
			DBGLOG( "EXTENSIONS:  GL_EXT_texture_filter_anisotropic is supported\n" );
			GLint maxAnisotropyLevel = 0;
			glGetIntegerv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, (GLint *)&maxAnisotropyLevel );

			if( openGLContext != 0 )
				openGLContext->SetMaxAnisotropyLevel( (float)maxAnisotropyLevel );
			else
				renderer::OpenGL::GetInstance()->SetMaxAnisotropyLevel( (float)maxAnisotropyLevel );

			DBGLOG( "GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT: %d\n", maxAnisotropyLevel );
		}
		else
			DBGLOG( "EXTENSIONS:  GL_EXT_texture_filter_anisotropic requested but not supported\n" );

	}

	//if( nExtRequests & EXT_VERTEX_AND_FRAGMENT_PROGRAMS )
	{
		// vertex and fragment programs
		bExtVertexPrograms = isExtensionSupported( "GL_ARB_vertex_program" );
		bExtFragmentPrograms = isExtensionSupported( "GL_ARB_fragment_program" );

		if( bExtVertexPrograms || bExtFragmentPrograms )
		{

#if defined( BASE_PLATFORM_WINDOWS )
			glVertexAttrib1dARB			= (PFNGLVERTEXATTRIB1DARBPROC)GetProcAddress("glVertexAttrib1dARB");				
			glVertexAttrib1dvARB		= (PFNGLVERTEXATTRIB1DVARBPROC)GetProcAddress("glVertexAttrib1dvARB");			
			glVertexAttrib1fARB			= (PFNGLVERTEXATTRIB1FARBPROC)GetProcAddress("glVertexAttrib1fARB");				
			glVertexAttrib1fvARB		= (PFNGLVERTEXATTRIB1FVARBPROC)GetProcAddress("glVertexAttrib1fvARB");				
			glVertexAttrib1sARB			= (PFNGLVERTEXATTRIB1SARBPROC)GetProcAddress("glVertexAttrib1sARB");				
			glVertexAttrib1svARB		= (PFNGLVERTEXATTRIB1SVARBPROC)GetProcAddress("glVertexAttrib1svARB");				
			glVertexAttrib2dARB			= (PFNGLVERTEXATTRIB2DARBPROC)GetProcAddress("glVertexAttrib2dARB");				
			glVertexAttrib2dvARB		= (PFNGLVERTEXATTRIB2DVARBPROC)GetProcAddress("glVertexAttrib2dvARB");				
			glVertexAttrib2fARB			= (PFNGLVERTEXATTRIB2FARBPROC)GetProcAddress("glVertexAttrib2fARB");				
			glVertexAttrib2fvARB		= (PFNGLVERTEXATTRIB2FVARBPROC)GetProcAddress("glVertexAttrib2fvARB");				
			glVertexAttrib2sARB			= (PFNGLVERTEXATTRIB2SARBPROC)GetProcAddress("glVertexAttrib2sARB");				
			glVertexAttrib2svARB		= (PFNGLVERTEXATTRIB2SVARBPROC)GetProcAddress("glVertexAttrib2svARB");				
			glVertexAttrib3dARB			= (PFNGLVERTEXATTRIB3DARBPROC)GetProcAddress("glVertexAttrib3dARB");				
			glVertexAttrib3dvARB		= (PFNGLVERTEXATTRIB3DVARBPROC)GetProcAddress("glVertexAttrib3dvARB");				
			glVertexAttrib3fARB			= (PFNGLVERTEXATTRIB3FARBPROC)GetProcAddress("glVertexAttrib3fARB");				
			glVertexAttrib3fvARB		= (PFNGLVERTEXATTRIB3FVARBPROC)GetProcAddress("glVertexAttrib3fvARB");				
			glVertexAttrib3sARB			= (PFNGLVERTEXATTRIB3SARBPROC)GetProcAddress("glVertexAttrib3sARB");				
			glVertexAttrib3svARB		= (PFNGLVERTEXATTRIB3SVARBPROC)GetProcAddress("glVertexAttrib3svARB");				
			glVertexAttrib4NbvARB		= (PFNGLVERTEXATTRIB4NBVARBPROC)GetProcAddress("glVertexAttrib4NbvARB");		
			glVertexAttrib4NivARB		= (PFNGLVERTEXATTRIB4NIVARBPROC)GetProcAddress("glVertexAttrib4NivARB");			
			glVertexAttrib4NsvARB		= (PFNGLVERTEXATTRIB4NSVARBPROC)GetProcAddress("glVertexAttrib4NsvARB");			
			glVertexAttrib4NubARB		= (PFNGLVERTEXATTRIB4NUBARBPROC)GetProcAddress("glVertexAttrib4NubARB");			
			glVertexAttrib4NubvARB		= (PFNGLVERTEXATTRIB4NUBVARBPROC)GetProcAddress("glVertexAttrib4NubvARB");			
			glVertexAttrib4NuivARB		= (PFNGLVERTEXATTRIB4NUIVARBPROC)GetProcAddress("glVertexAttrib4NusvARB");			
			glVertexAttrib4NusvARB		= (PFNGLVERTEXATTRIB4NUSVARBPROC)GetProcAddress("glVertexAttrib4NusvARB");			
			glVertexAttrib4bvARB		= (PFNGLVERTEXATTRIB4BVARBPROC)GetProcAddress("glVertexAttrib4bvARB");				
			glVertexAttrib4dARB			= (PFNGLVERTEXATTRIB4DARBPROC)GetProcAddress("glVertexAttrib4dARB");				
			glVertexAttrib4dvARB		= (PFNGLVERTEXATTRIB4DVARBPROC)GetProcAddress("glVertexAttrib4dvARB");				
			glVertexAttrib4fARB			= (PFNGLVERTEXATTRIB4FARBPROC)GetProcAddress("glVertexAttrib4fARB");				
			glVertexAttrib4fvARB		= (PFNGLVERTEXATTRIB4FVARBPROC)GetProcAddress("glVertexAttrib4fvARB");				
			glVertexAttrib4ivARB		= (PFNGLVERTEXATTRIB4IVARBPROC)GetProcAddress("glVertexAttrib4ivARB");				
			glVertexAttrib4sARB			= (PFNGLVERTEXATTRIB4SARBPROC)GetProcAddress("glVertexAttrib4sARB");				
			glVertexAttrib4svARB		= (PFNGLVERTEXATTRIB4SVARBPROC)GetProcAddress("glVertexAttrib4svARB");				
			glVertexAttrib4ubvARB		= (PFNGLVERTEXATTRIB4UBVARBPROC)GetProcAddress("glVertexAttrib4ubvARB");			
			glVertexAttrib4uivARB		= (PFNGLVERTEXATTRIB4UIVARBPROC)GetProcAddress("glVertexAttrib4uivARB");			
			glVertexAttrib4usvARB		= (PFNGLVERTEXATTRIB4USVARBPROC)GetProcAddress("glVertexAttrib4usvARB");			
			glVertexAttribPointerARB	= (PFNGLVERTEXATTRIBPOINTERARBPROC)GetProcAddress("glVertexAttribPointerARB");			
			glEnableVertexAttribArrayARB	= (PFNGLENABLEVERTEXATTRIBARRAYARBPROC)GetProcAddress("glEnableVertexAttribArrayARB");		
			glDisableVertexAttribArrayARB	= (PFNGLDISABLEVERTEXATTRIBARRAYARBPROC)GetProcAddress("glDisableVertexAttribArrayARB");	
			glProgramStringARB			= (PFNGLPROGRAMSTRINGARBPROC)GetProcAddress("glProgramStringARB");				
			glBindProgramARB			= (PFNGLBINDPROGRAMARBPROC)GetProcAddress("glBindProgramARB");					
			glDeleteProgramsARB			= (PFNGLDELETEPROGRAMSARBPROC)GetProcAddress("glDeleteProgramsARB");				
			glGenProgramsARB			= (PFNGLGENPROGRAMSARBPROC)GetProcAddress("glGenProgramsARB");					
			glProgramEnvParameter4dARB	= (PFNGLPROGRAMENVPARAMETER4DARBPROC)GetProcAddress("glProgramEnvParameter4dARB");		
			glProgramEnvParameter4dvARB = (PFNGLPROGRAMENVPARAMETER4DVARBPROC)GetProcAddress("glProgramEnvParameter4dvARB");		
			glProgramEnvParameter4fARB	= (PFNGLPROGRAMENVPARAMETER4FARBPROC)GetProcAddress("glProgramEnvParameter4fARB");		
			glProgramEnvParameter4fvARB = (PFNGLPROGRAMENVPARAMETER4FVARBPROC)GetProcAddress("glProgramEnvParameter4fvARB");		
			glProgramLocalParameter4dARB	= (PFNGLPROGRAMLOCALPARAMETER4DARBPROC)GetProcAddress("glProgramLocalParameter4dARB");		
			glProgramLocalParameter4dvARB	= (PFNGLPROGRAMLOCALPARAMETER4DVARBPROC)GetProcAddress("glProgramLocalParameter4dvARB");
			glProgramLocalParameter4fARB	= (PFNGLPROGRAMLOCALPARAMETER4FARBPROC)GetProcAddress("glProgramLocalParameter4fARB");		
			glProgramLocalParameter4fvARB	= (PFNGLPROGRAMLOCALPARAMETER4FVARBPROC)GetProcAddress("glProgramLocalParameter4fvARB");	
			glGetProgramEnvParameterdvARB	= (PFNGLGETPROGRAMENVPARAMETERDVARBPROC)GetProcAddress("glGetProgramEnvParameterdvARB");	
			glGetProgramEnvParameterfvARB	= (PFNGLGETPROGRAMENVPARAMETERFVARBPROC)GetProcAddress("glGetProgramEnvParameterfvARB");	
			glGetProgramLocalParameterdvARB	= (PFNGLGETPROGRAMLOCALPARAMETERDVARBPROC)GetProcAddress("glGetProgramLocalParameterdvARB");	
			glGetProgramLocalParameterfvARB = (PFNGLGETPROGRAMLOCALPARAMETERFVARBPROC)GetProcAddress("glGetProgramLocalParameterfvARB");	
			glGetProgramivARB			= (PFNGLGETPROGRAMIVARBPROC)GetProcAddress("glGetProgramivARB");				
			glGetProgramStringARB		= (PFNGLGETPROGRAMSTRINGARBPROC)GetProcAddress("glGetProgramStringARB");			
			glGetVertexAttribdvARB		= (PFNGLGETVERTEXATTRIBDVARBPROC)GetProcAddress("glGetVertexAttribdvARB");			
			glGetVertexAttribfvARB		= (PFNGLGETVERTEXATTRIBFVARBPROC)GetProcAddress("glGetVertexAttribfvARB");			
			glGetVertexAttribivARB		= (PFNGLGETVERTEXATTRIBIVARBPROC)GetProcAddress("glGetVertexAttribivARB");			
			glGetVertexAttribPointervARB = (PFNGLGETVERTEXATTRIBPOINTERVARBPROC)GetProcAddress("glGetVertexAttribPointervARB");		
			glIsProgramARB				= (PFNGLISPROGRAMARBPROC)GetProcAddress("glIsProgramARB");
#endif // BASE_PLATFORM_WINDOWS

			if( bExtVertexPrograms )
				DBGLOG( "EXTENSIONS:  GL_ARB_vertex_program is supported\n" );
			else
				DBGLOG( "EXTENSIONS:  GL_ARB_vertex_program requested but not supported\n" );

			if( bExtFragmentPrograms )
				DBGLOG( "EXTENSIONS:  GL_ARB_fragment_program is supported\n" );
			else
				DBGLOG( "EXTENSIONS:  GL_ARB_fragment_program requested but not supported\n" );
		}
		else
		{
			if( !bExtVertexPrograms )
				DBGLOG( "EXTENSIONS:  GL_ARB_vertex_program requested but not supported\n" );
			if( !bExtFragmentPrograms )
				DBGLOG( "EXTENSIONS:  GL_ARB_fragment_program requested but not supported\n" );
		}
	}

	// GL Shading Language Extensions
	//if( nExtRequests & EXT_SHADER_OBJECTS )
	{
		// shader objects
		bExtShaderObjects = isExtensionSupported( "GL_ARB_shader_objects" );

		if( bExtShaderObjects )
		{

#if defined( BASE_PLATFORM_WINDOWS )
/*			glDeleteObjectARB			= (PFNGLDELETEOBJECTARBPROC)GetProcAddress("glDeleteObjectARB");	         
			glGetHandleARB				= (PFNGLGETHANDLEARBPROC)GetProcAddress("glGetHandleARB");	            
			glDetachObjectARB			= (PFNGLDETACHOBJECTARBPROC)GetProcAddress("glDetachObjectARB");	         
			glCreateShaderObjectARB		= (PFNGLCREATESHADEROBJECTARBPROC)GetProcAddress("glCreateShaderObjectARB");	   
			glShaderSourceARB			= (PFNGLSHADERSOURCEARBPROC)GetProcAddress("glShaderSourceARB");	         
			glCompileShaderARB			= (PFNGLCOMPILESHADERARBPROC)GetProcAddress("glCompileShaderARB");	        
			glCreateProgramObjectARB	= (PFNGLCREATEPROGRAMOBJECTARBPROC)GetProcAddress("glCreateProgramObjectARB");	  
			glAttachObjectARB			= (PFNGLATTACHOBJECTARBPROC)GetProcAddress("glAttachObjectARB");	         
			glLinkProgramARB			= (PFNGLLINKPROGRAMARBPROC)GetProcAddress("glLinkProgramARB");	          
			glUseProgramObjectARB		= (PFNGLUSEPROGRAMOBJECTARBPROC)GetProcAddress("glUseProgramObjectARB");	     
			glValidateProgramARB		= (PFNGLVALIDATEPROGRAMARBPROC)GetProcAddress("glValidateProgramARB");	      
			glUniform1fARB				= (PFNGLUNIFORM1FARBPROC)GetProcAddress("glUniform1fARB");	            
			glUniform2fARB				= (PFNGLUNIFORM2FARBPROC)GetProcAddress("glUniform2fARB");	            
			glUniform3fARB				= (PFNGLUNIFORM3FARBPROC)GetProcAddress("glUniform3fARB");	            
			glUniform4fARB				= (PFNGLUNIFORM4FARBPROC)GetProcAddress("glUniform4fARB");	            
			glUniform1iARB				= (PFNGLUNIFORM1IARBPROC)GetProcAddress("glUniform1iARB");	            
			glUniform2iARB				= (PFNGLUNIFORM2IARBPROC)GetProcAddress("glUniform2iARB");	            
			glUniform3iARB				= (PFNGLUNIFORM3IARBPROC)GetProcAddress("glUniform3iARB");	            
			glUniform4iARB				= (PFNGLUNIFORM4IARBPROC)GetProcAddress("glUniform4iARB");	            
			glUniform1fvARB				= (PFNGLUNIFORM1FVARBPROC)GetProcAddress("glUniform1fvARB");	          
			glUniform2fvARB				= (PFNGLUNIFORM2FVARBPROC)GetProcAddress("glUniform2fvARB");	           
			glUniform3fvARB				= (PFNGLUNIFORM3FVARBPROC)GetProcAddress("glUniform3fvARB");	           
			glUniform4fvARB				= (PFNGLUNIFORM4FVARBPROC)GetProcAddress("glUniform4fvARB");	          
			glUniform1ivARB				= (PFNGLUNIFORM1IVARBPROC)GetProcAddress("glUniform1ivARB");	           
			glUniform2ivARB				= (PFNGLUNIFORM2IVARBPROC)GetProcAddress("glUniform2ivARB");	          
			glUniform3ivARB				= (PFNGLUNIFORM3IVARBPROC)GetProcAddress("glUniform3ivARB");	           
			glUniform4ivARB				= (PFNGLUNIFORM4IVARBPROC)GetProcAddress("glUniform4ivARB ");	          
			glUniformMatrix2fvARB		= (PFNGLUNIFORMMATRIX2FVARBPROC)GetProcAddress("glUniformMatrix2fvARB");	     
			glUniformMatrix3fvARB		= (PFNGLUNIFORMMATRIX3FVARBPROC)GetProcAddress("glUniformMatrix3fvARB");	     
			glUniformMatrix4fvARB		= (PFNGLUNIFORMMATRIX4FVARBPROC)GetProcAddress("glUniformMatrix4fvARB");	     
			glGetObjectParameterfvARB	= (PFNGLGETOBJECTPARAMETERFVARBPROC)GetProcAddress("glGetObjectParameterfvARB");	 
			glGetObjectParameterivARB	= (PFNGLGETOBJECTPARAMETERIVARBPROC)GetProcAddress("glGetObjectParameterivARB");	 
			glGetInfoLogARB				= (PFNGLGETINFOLOGARBPROC)GetProcAddress("glGetInfoLogARB");	           
			glGetAttachedObjectsARB		= (PFNGLGETATTACHEDOBJECTSARBPROC)GetProcAddress("glGetAttachedObjectsARB");	   
			glGetUniformLocationARB		= (PFNGLGETUNIFORMLOCATIONARBPROC)GetProcAddress("glGetUniformLocationARB");	   
			glGetActiveUniformARB		= (PFNGLGETACTIVEUNIFORMARBPROC)GetProcAddress("glGetActiveUniformARB");	     
			glGetUniformfvARB			= (PFNGLGETUNIFORMFVARBPROC)GetProcAddress("glGetUniformfvARB");	         
			glGetUniformivARB			= (PFNGLGETUNIFORMIVARBPROC)GetProcAddress("glGetUniformivARB");	         
			glGetShaderSourceARB		= (PFNGLGETSHADERSOURCEARBPROC)GetProcAddress("glGetShaderSourceARB");
*/

			glAttachShader = (PFNGLATTACHSHADERPROC)GetProcAddress("glAttachShader");
			glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC)GetProcAddress("glBindAttribLocation");
			glCompileShader = (PFNGLCOMPILESHADERPROC)GetProcAddress("glCompileShader");
			glCreateProgram = (PFNGLCREATEPROGRAMPROC)GetProcAddress("glCreateProgram");
			glCreateShader = (PFNGLCREATESHADERPROC)GetProcAddress("glCreateShader");
			glDeleteProgram = (PFNGLDELETEPROGRAMPROC)GetProcAddress("glDeleteProgram");
			glDeleteShader = (PFNGLDELETESHADERPROC)GetProcAddress("glDeleteShader");
			glDetachShader = (PFNGLDETACHSHADERPROC)GetProcAddress("glDetachShader");
			glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)GetProcAddress("glDisableVertexAttribArray");
			glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)GetProcAddress("glEnableVertexAttribArray");
			glGetActiveUniform = (PFNGLGETACTIVEATTRIBPROC)GetProcAddress("glGetActiveAttrib");
			glGetActiveUniform = (PFNGLGETACTIVEUNIFORMPROC)GetProcAddress("glGetActiveUniform");
			glGetAttachedShaders = (PFNGLGETATTACHEDSHADERSPROC)GetProcAddress("glGetAttachedShaders");
			glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC)GetProcAddress("glGetAttribLocation");
			glGetProgramiv = (PFNGLGETPROGRAMIVPROC)GetProcAddress("glGetProgramiv");
			glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)GetProcAddress("glGetProgramInfoLog");
			glGetShaderiv = (PFNGLGETSHADERIVPROC)GetProcAddress("glGetShaderiv");
			glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)GetProcAddress("glGetShaderInfoLog");
			glGetShaderSource = (PFNGLGETSHADERSOURCEPROC)GetProcAddress("glGetShaderSource");
			glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)GetProcAddress("glGetUniformLocation");
			glGetUniformfv = (PFNGLGETUNIFORMFVPROC)GetProcAddress("glGetUniformfv");
			glGetUniformiv = (PFNGLGETUNIFORMIVPROC)GetProcAddress("glGetUniformiv");
			glGetVertexAttribdv = (PFNGLGETVERTEXATTRIBDVPROC)GetProcAddress("glGetVertexAttribdv");
			glGetVertexAttribfv = (PFNGLGETVERTEXATTRIBFVPROC)GetProcAddress("glGetVertexAttribfv");
			glGetVertexAttribiv = (PFNGLGETVERTEXATTRIBIVPROC)GetProcAddress("glGetVertexAttribiv");
			glGetVertexAttribPointerv = (PFNGLGETVERTEXATTRIBPOINTERVPROC)GetProcAddress("glGetVertexAttribPointerv");
			glIsProgram = (PFNGLISPROGRAMPROC)GetProcAddress("glIsProgram");
			glIsShader = (PFNGLISSHADERPROC)GetProcAddress("glIsShader");
			glLinkProgram = (PFNGLLINKPROGRAMPROC)GetProcAddress("glLinkProgram");
			glShaderSource = (PFNGLSHADERSOURCEPROC)GetProcAddress("glShaderSource");
			glUseProgram = (PFNGLUSEPROGRAMPROC)GetProcAddress("glUseProgram");
			glUniform1f = (PFNGLUNIFORM1FPROC)GetProcAddress("glUniform1f");
			glUniform2f = (PFNGLUNIFORM2FPROC)GetProcAddress("glUniform2f");
			glUniform3f = (PFNGLUNIFORM3FPROC)GetProcAddress("glUniform3f");
			glUniform4f = (PFNGLUNIFORM4FPROC)GetProcAddress("glUniform4f");
			glUniform1i = (PFNGLUNIFORM1IPROC)GetProcAddress("glUniform1i");
			glUniform2i = (PFNGLUNIFORM2IPROC)GetProcAddress("glUniform2i");
			glUniform3i = (PFNGLUNIFORM3IPROC)GetProcAddress("glUniform3i");
			glUniform4i = (PFNGLUNIFORM4IPROC)GetProcAddress("glUniform4i");
			glUniform1fv = (PFNGLUNIFORM1FVPROC)GetProcAddress("glUniform1fv");
			glUniform2fv = (PFNGLUNIFORM2FVPROC)GetProcAddress("glUniform2fv");
			glUniform3fv = (PFNGLUNIFORM3FVPROC)GetProcAddress("glUniform3fv");
			glUniform4fv = (PFNGLUNIFORM4FVPROC)GetProcAddress("glUniform4fv");
			glUniform1iv = (PFNGLUNIFORM1IVPROC)GetProcAddress("glUniform1iv");
			glUniform2iv = (PFNGLUNIFORM2IVPROC)GetProcAddress("glUniform2iv");
			glUniform3iv = (PFNGLUNIFORM3IVPROC)GetProcAddress("glUniform3iv");
			glUniform4iv = (PFNGLUNIFORM4IVPROC)GetProcAddress("glUniform4iv");
			glUniformMatrix2fv = (PFNGLUNIFORMMATRIX2FVPROC)GetProcAddress("glUniformMatrix2fv");
			glUniformMatrix3fv = (PFNGLUNIFORMMATRIX3FVPROC)GetProcAddress("glUniformMatrix3fv");
			glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)GetProcAddress("glUniformMatrix4fv");
			glValidateProgram = (PFNGLVALIDATEPROGRAMPROC)GetProcAddress("glValidateProgram");
			glVertexAttrib1d = (PFNGLVERTEXATTRIB1DPROC)GetProcAddress("glVertexAttrib1d");
			glVertexAttrib1dv = (PFNGLVERTEXATTRIB1DVPROC)GetProcAddress("glVertexAttrib1dv");
			glVertexAttrib1f = (PFNGLVERTEXATTRIB1FPROC)GetProcAddress("glVertexAttrib1f");
			glVertexAttrib1fv = (PFNGLVERTEXATTRIB1FVPROC)GetProcAddress("glVertexAttrib1fv");
			glVertexAttrib1s = (PFNGLVERTEXATTRIB1SPROC)GetProcAddress("glVertexAttrib1s");
			glVertexAttrib1sv = (PFNGLVERTEXATTRIB1SVPROC)GetProcAddress("glVertexAttrib1sv");
			glVertexAttrib2d = (PFNGLVERTEXATTRIB2DPROC)GetProcAddress("glVertexAttrib2d");
			glVertexAttrib2dv = (PFNGLVERTEXATTRIB2DVPROC)GetProcAddress("glVertexAttrib2dv");
			glVertexAttrib2f = (PFNGLVERTEXATTRIB2FPROC)GetProcAddress("glVertexAttrib2f");
			glVertexAttrib2fv = (PFNGLVERTEXATTRIB2FVPROC)GetProcAddress("glVertexAttrib2fv");
			glVertexAttrib2s = (PFNGLVERTEXATTRIB2SPROC)GetProcAddress("glVertexAttrib2s");
			glVertexAttrib2sv = (PFNGLVERTEXATTRIB2SVPROC)GetProcAddress("glVertexAttrib2sv");
			glVertexAttrib3d = (PFNGLVERTEXATTRIB3DPROC)GetProcAddress("glVertexAttrib3d");
			glVertexAttrib3dv = (PFNGLVERTEXATTRIB3DVPROC)GetProcAddress("glVertexAttrib3dv");
			glVertexAttrib3f = (PFNGLVERTEXATTRIB3FPROC)GetProcAddress("glVertexAttrib3f");
			glVertexAttrib3fv = (PFNGLVERTEXATTRIB3FVPROC)GetProcAddress("glVertexAttrib3fv");
			glVertexAttrib3s = (PFNGLVERTEXATTRIB3SPROC)GetProcAddress("glVertexAttrib3s");
			glVertexAttrib3sv = (PFNGLVERTEXATTRIB3SVPROC)GetProcAddress("glVertexAttrib3sv");
			glVertexAttrib4Nbv = (PFNGLVERTEXATTRIB4NBVPROC)GetProcAddress("glVertexAttrib4Nbv");
			glVertexAttrib4Niv = (PFNGLVERTEXATTRIB4NIVPROC)GetProcAddress("glVertexAttrib4Niv");
			glVertexAttrib4Nsv = (PFNGLVERTEXATTRIB4NSVPROC)GetProcAddress("glVertexAttrib4Nsv");
			glVertexAttrib4Nub = (PFNGLVERTEXATTRIB4NUBPROC)GetProcAddress("glVertexAttrib4Nub");
			glVertexAttrib4Nubv = (PFNGLVERTEXATTRIB4NUBVPROC)GetProcAddress("glVertexAttrib4Nubv");
			glVertexAttrib4Nuiv = (PFNGLVERTEXATTRIB4NUIVPROC)GetProcAddress("glVertexAttrib4Nuiv");
			glVertexAttrib4Nusv = (PFNGLVERTEXATTRIB4NUSVPROC)GetProcAddress("glVertexAttrib4Nusv");
			glVertexAttrib4bv = (PFNGLVERTEXATTRIB4BVPROC)GetProcAddress("glVertexAttrib4bv");
			glVertexAttrib4d = (PFNGLVERTEXATTRIB4DPROC)GetProcAddress("glVertexAttrib4d");
			glVertexAttrib4dv = (PFNGLVERTEXATTRIB4DVPROC)GetProcAddress("glVertexAttrib4dv");
			glVertexAttrib4f = (PFNGLVERTEXATTRIB4FPROC)GetProcAddress("glVertexAttrib4f");
			glVertexAttrib4fv = (PFNGLVERTEXATTRIB4FVPROC)GetProcAddress("glVertexAttrib4fv");
			glVertexAttrib4iv = (PFNGLVERTEXATTRIB4IVPROC)GetProcAddress("glVertexAttrib4iv");
			glVertexAttrib4s = (PFNGLVERTEXATTRIB4SPROC)GetProcAddress("glVertexAttrib4s");
			glVertexAttrib4sv = (PFNGLVERTEXATTRIB4SVPROC)GetProcAddress("glVertexAttrib4sv");
			glVertexAttrib4ubv = (PFNGLVERTEXATTRIB4UBVPROC)GetProcAddress("glVertexAttrib4ubv");
			glVertexAttrib4uiv = (PFNGLVERTEXATTRIB4UIVPROC)GetProcAddress("glVertexAttrib4uiv");
			glVertexAttrib4usv = (PFNGLVERTEXATTRIB4USVPROC)GetProcAddress("glVertexAttrib4usv");
			glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)GetProcAddress("glVertexAttribPointer");
#endif // BASE_PLATFORM_WINDOWS

			DBGLOG( "EXTENSIONS:  GL_ARB_shader_objects is supported\n" );
		}
		else
			DBGLOG( "EXTENSIONS:  GL_ARB_shader_objects requested but not supported\n" );
	}

	//if( nExtRequests & EXT_VERTEX_AND_FRAGMENT_AND_GEOMETRY_SHADERS )
	{
		// GLSL support

		// vertex shader
		bExtVertexShaders = isExtensionSupported( "GL_ARB_vertex_shader" );
		// fragment shader
		bExtFragmentShaders = isExtensionSupported( "GL_ARB_fragment_shader" );
		// geomtry shader
		bExtGeometryShaders = isExtensionSupported( "GL_EXT_geometry_shader4" );

		if( bExtVertexShaders || bExtFragmentShaders || bExtGeometryShaders )
		{

#if defined( BASE_PLATFORM_WINDOWS )
			if( bExtGeometryShaders )
			{
				glProgramParameteriEXT = (PFNGLPROGRAMPARAMETERIEXTPROC)GetProcAddress("glProgramParameteriEXT");
			}
#endif // BASE_PLATFORM_WINDOWS

			if( bExtVertexShaders )
				DBGLOG( "EXTENSIONS:  GL_ARB_vertex_shader is supported\n" );
			else
				DBGLOG( "EXTENSIONS:  GL_ARB_vertex_shader requested but not supported\n" );

			if( bExtFragmentShaders )
				DBGLOG( "EXTENSIONS:  GL_ARB_fragment_shader is supported\n" );
			else
				DBGLOG( "EXTENSIONS:  GL_ARB_fragment_shader requested but not supported\n" );

			if( bExtGeometryShaders )
				DBGLOG( "EXTENSIONS:  GL_EXT_geometry_shader4 is supported\n" );
			else
				DBGLOG( "EXTENSIONS:  GL_EXT_geometry_shader4 requested but not supported\n" );
		}
		else
		{
			if( !bExtVertexShaders )
				DBGLOG( "EXTENSIONS:  GL_ARB_vertex_shader requested but not supported\n" );
			if( !bExtFragmentShaders )
				DBGLOG( "EXTENSIONS:  GL_ARB_fragment_shader requested but not supported\n" );
			if( !bExtGeometryShaders )
				DBGLOG( "EXTENSIONS:  GL_EXT_geometry_shader4 requested but not supported\n" );
		}
	}

	//if( nExtRequests & EXT_GL_SHADING_LANGUAGE )
	{
		// shading language
		bExtGLShadingLanguage = isExtensionSupported( "GL_ARB_shading_language_100" );

		if(bExtGLShadingLanguage)
		{
			//core::WideString glslVersionName( reinterpret_cast<const char*>( glGetString( GL_SHADING_LANGUAGE_VERSION ) ) );
			DBGLOG( "EXTENSIONS:  GL_ARB_shading_language_100 supported version: %s\n", glGetString( GL_SHADING_LANGUAGE_VERSION ) );
		}
		else
			DBGLOG( "EXTENSIONS:  GL_ARB_shading_language_100 requested but not supported\n"  );

	}

	//if( nExtRequests & EXT_PIXEL_BUFFER_OBJECTS )
	{
		// PBO
		bExtPixelBufferObjects = isExtensionSupported( "GL_ARB_pixel_buffer_object" ) || isExtensionSupported( "GL_EXT_pixel_buffer_object" );

		if( SupportsGLVersion( 2, 1 ) )
		{
			bExtVertexBufferObjects = 1;

			DBGLOG( "EXTENSIONS:  GL_ARB_pixel_buffer_object/GL Core 2.1 is supported \n" );
		}
		else if( bExtPixelBufferObjects )
		{
			DBGLOG( "EXTENSIONS:  GL_ARB_pixel_buffer_object/GL_EXT_pixel_buffer_object is supported \n" );
		}
		else
			DBGLOG( "EXTENSIONS:  GL_ARB_pixel_buffer_object/GL_EXT_pixel_buffer_object requested but not supported\n" );
	}

	//if( nExtRequests & EXT_FRAME_BUFFER_OBJECT )
	{
		// Frame buffer objects
		bExtFrameBufferObject = isExtensionSupported( "GL_EXT_framebuffer_object" );

		// Frame buffer blit
		bExtFrameBufferBlit = isExtensionSupported( "GL_EXT_framebuffer_blit" );

		// Frame buffer multisample
		bExtFrameBufferMultisample = isExtensionSupported( "GL_EXT_framebuffer_multisample" );

		if( bExtFrameBufferObject )
		{
		
#if defined( BASE_PLATFORM_WINDOWS )
			glIsRenderbufferEXT							= (PFNGLISRENDERBUFFEREXTPROC)GetProcAddress("glIsRenderbufferEXT");
			glBindRenderbufferEXT						= (PFNGLBINDRENDERBUFFEREXTPROC)GetProcAddress("glBindRenderbufferEXT");
			glDeleteRenderbuffersEXT					= (PFNGLDELETERENDERBUFFERSEXTPROC)GetProcAddress("glDeleteRenderbuffersEXT");
			glGenRenderbuffersEXT						= (PFNGLGENRENDERBUFFERSEXTPROC)GetProcAddress("glGenRenderbuffersEXT");	
			glRenderbufferStorageEXT					= (PFNGLRENDERBUFFERSTORAGEEXTPROC)GetProcAddress("glRenderbufferStorageEXT");	
			glGetRenderbufferParameterivEXT				= (PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC)GetProcAddress("glGetRenderbufferParameterivEXT");
			glIsFramebufferEXT							= (PFNGLISFRAMEBUFFEREXTPROC)GetProcAddress("glIsFramebufferEXT");
			glBindFramebufferEXT						= (PFNGLBINDFRAMEBUFFEREXTPROC)GetProcAddress("glBindFramebufferEXT");
			glDeleteFramebuffersEXT						= (PFNGLDELETEFRAMEBUFFERSEXTPROC)GetProcAddress("glDeleteFramebuffersEXT");
			glGenFramebuffersEXT						= (PFNGLGENFRAMEBUFFERSEXTPROC)GetProcAddress("glGenFramebuffersEXT");
			glCheckFramebufferStatusEXT					= (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC)GetProcAddress("glCheckFramebufferStatusEXT");
			glFramebufferTexture1DEXT					= (PFNGLFRAMEBUFFERTEXTURE1DEXTPROC)GetProcAddress("glFramebufferTexture1DEXT");	
			glFramebufferTexture2DEXT					= (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC)GetProcAddress("glFramebufferTexture2DEXT");	
			glFramebufferTexture3DEXT					= (PFNGLFRAMEBUFFERTEXTURE3DEXTPROC)GetProcAddress("glFramebufferTexture3DEXT");
			glFramebufferRenderbufferEXT				= (PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC)GetProcAddress("glFramebufferRenderbufferEXT");	
			glGetFramebufferAttachmentParameterivEXT	= (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC)GetProcAddress("glGetFramebufferAttachmentParameterivEXT");
			glGenerateMipmapEXT							= (PFNGLGENERATEMIPMAPEXTPROC)GetProcAddress("glGenerateMipmapEXT");
#endif // BASE_PLATFORM_WINDOWS

			DBGLOG( "EXTENSIONS:  GL_EXT_framebuffer_object is supported \n" );
			GLint maxRenderBufferSize = 0;
			glGetIntegerv( GL_MAX_RENDERBUFFER_SIZE_EXT, (GLint *)&maxRenderBufferSize );
			DBGLOG( "GL_MAX_RENDERBUFFER_SIZE_EXT: %dx%d\n", maxRenderBufferSize, maxRenderBufferSize );
		}
		else
			DBGLOG( "EXTENSIONS:  GL_EXT_framebuffer_object requested but not supported\n" );

		if( bExtFrameBufferBlit )
		{
		
#if defined( BASE_PLATFORM_WINDOWS )
			glBlitFramebufferEXT						= (PFNGLBLITFRAMEBUFFEREXTPROC)GetProcAddress("glBlitFramebufferEXT");
#endif // BASE_PLATFORM_WINDOWS

			DBGLOG( "EXTENSIONS:  GL_EXT_framebuffer_blit is supported \n" );
		}
		else
			DBGLOG( "EXTENSIONS:  GL_EXT_framebuffer_blit requested but not supported\n" );

		if( bExtFrameBufferMultisample )
		{
		
#if defined( BASE_PLATFORM_WINDOWS )
			glRenderbufferStorageMultisampleEXT			= (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC)GetProcAddress("glRenderbufferStorageMultisampleEXT");
#endif // BASE_PLATFORM_WINDOWS

			DBGLOG( "EXTENSIONS:  GL_EXT_framebuffer_multisample is supported \n" );
		}
		else
			DBGLOG( "EXTENSIONS:  GL_EXT_framebuffer_multisample requested but not supported\n" );
	}

	//if( nExtRequests & EXT_TEXTURE_RECTANGLE )
	{
		// Texture Recangle
		bExtTextureRectangle = isExtensionSupported( "GL_ARB_texture_rectangle" ) || isExtensionSupported( "GL_EXT_texture_rectangle" );

		if( bExtTextureRectangle )
		{
			DBGLOG( "EXTENSIONS:  GL_ARB_texture_rectangle/GL_EXT_texture_rectangle is supported \n" );

			GLint maxTextureRectangle = 0;
			glGetIntegerv( GL_MAX_RECTANGLE_TEXTURE_SIZE_ARB, (GLint *)&maxTextureRectangle );
			DBGLOG( "GL_MAX_RECTANGLE_TEXTURE_SIZE_ARB LEVEL: %dx%d\n", maxTextureRectangle, maxTextureRectangle );

			if( openGLContext != 0 )
				openGLContext->SetMaxTextureRectangleSize( maxTextureRectangle );
			else
				renderer::OpenGL::GetInstance()->SetMaxTextureRectangleSize( maxTextureRectangle );
		}
		else
			DBGLOG( "EXTENSIONS:  GL_ARB_texture_rectangle/GL_EXT_texture_rectangle requested but not supported\n" );
	}

	//if( nExtRequests & EXT_TEXTURE_NON_POWER_OF_TWO )
	{
		// Texture NPOT
		bExtTextureNonPowerOfTwo = isExtensionSupported( "GL_ARB_texture_non_power_of_two" );

		if( bExtTextureNonPowerOfTwo )
		{
			DBGLOG( "EXTENSIONS:  GL_ARB_texture_non_power_of_two is supported \n" );
		}
		else
			DBGLOG( "EXTENSIONS:  GL_ARB_texture_non_power_of_two requested but not supported\n" );
	}

	//if( nExtRequests & EXT_TEXTURE_BORDER_CLAMP )
	{
		// Texture Border clamp
		bExtTextureBorderClamp = isExtensionSupported( "GL_ARB_texture_border_clamp" ) || isExtensionSupported( "GL_SGIS_texture_border_clamp" );

		if( SupportsGLVersion( 1, 3 ) )
		{
			bExtTextureBorderClamp = 1;
			DBGLOG( "EXTENSIONS:  GL_ARB_texture_border_clamp/GL Core 1.3 is supported \n" );
		}
		else if( bExtTextureBorderClamp )
		{
			DBGLOG( "EXTENSIONS:  GL_ARB_texture_border_clamp/GL_SGIS_texture_border_clamp is supported \n" );
		}
		else
			DBGLOG( "EXTENSIONS:  GL_ARB_texture_border_clamp/GL_SGIS_texture_border_clamp requested but not supported\n" );
	}

	//if( nExtRequests & EXT_SHADOW )
	{
		// ARB shadow
		bExtShadow = isExtensionSupported( "GL_ARB_shadow" ) || isExtensionSupported( "GL_SGIX_shadow" );

		if( SupportsGLVersion( 1, 4 ) )
		{
			bExtShadow = 1;
			DBGLOG( "EXTENSIONS:  GL_ARB_shadow/GL Core 1.4 is supported \n" );
		}
		else if( bExtShadow )
		{
			DBGLOG( "EXTENSIONS:  GL_ARB_shadow/GL_SGIX_shadow is supported \n" );
		}
		else
			DBGLOG( "EXTENSIONS:  GL_ARB_shadow/GL_SGIX_shadow requested but not supported\n" );
	}

	//if( nExtRequests & EXT_DEPTH_TEXTURE )
	{
		// ARB shadow
		bExtDepthTexture = isExtensionSupported( "GL_ARB_depth_texture" ) || isExtensionSupported( "GL_SGIX_depth_texture" );

		if( SupportsGLVersion( 1, 4 ) )
		{
			bExtDepthTexture = 1;
			DBGLOG( "EXTENSIONS:  GL_ARB_depth_texture/GL Core 1.4 is supported \n" );
		}
		else if( bExtDepthTexture )
		{
			DBGLOG( "EXTENSIONS:  GL_ARB_depth_texture/GL_SGIX_depth_texture is supported \n" );
		}
		else
			DBGLOG( "EXTENSIONS:  GL_ARB_depth_texture/GL_SGIX_depth_texture requested but not supported\n" );
	}

	//if( nExtRequests & EXT_TEXTURE_EDGE_CLAMP )
	{
		// texture edge clamp
		bExtTextureEdgeClamp = isExtensionSupported( "GL_EXT_texture_edge_clamp" ) || isExtensionSupported( "GL_SGIS_texture_edge_clamp" );
		
		if( SupportsGLVersion( 1, 2 ) )
		{
			bExtTextureEdgeClamp = 1;
			DBGLOG( "EXTENSIONS:  GL_EXT_texture_edge_clamp/GL Core 1.2 is supported \n" );
		}
		else if( bExtTextureEdgeClamp )
		{
			DBGLOG( "EXTENSIONS:  GL_EXT_texture_edge_clamp/GL_SGIS_texture_edge_clamp is supported \n" );
		}
		else
			DBGLOG( "EXTENSIONS:  GL_EXT_texture_edge_clamp/GL_SGIS_texture_edge_clamp requested but not supported\n" );
	}

	//if( nExtRequests & EXT_SEPARATE_SPECULAR_COLOR )
	{
		// separate specular colour
		bExtSeparateSpecularColor = isExtensionSupported( "GL_EXT_separate_specular_color" );
		
		if( SupportsGLVersion( 1, 2 ) )
		{
			bExtSeparateSpecularColor = 1;
			DBGLOG( "EXTENSIONS:  GL_EXT_separate_specular_color/GL Core 1.2 is supported \n" );
		}
		else if( bExtSeparateSpecularColor )
		{
			DBGLOG( "EXTENSIONS:  GL_EXT_separate_specular_color is supported \n" );
		}
		else
			DBGLOG( "EXTENSIONS:  GL_EXT_separate_specular_color requested but not supported\n" );
	}

	//if( nExtRequests & EXT_DRAW_BUFFERS )
	{
		// draw buffers
		bExtDrawBuffers = isExtensionSupported( "GL_ARB_draw_buffers" );

		if( SupportsGLVersion( 2, 0 ) )
		{
			bExtDrawBuffers = 1;

#if defined( BASE_PLATFORM_WINDOWS )
			glDrawBuffers			= (PFNGLDRAWBUFFERSPROC)GetProcAddress("glDrawBuffers");
#endif // BASE_PLATFORM_WINDOWS

			DBGLOG( "EXTENSIONS:  GL_ARB_draw_buffers/GL Core 2.0 is supported \n" );

		}
		else if( bExtDrawBuffers )
		{
#if defined( BASE_PLATFORM_WINDOWS )
			glDrawBuffers			= (PFNGLDRAWBUFFERSPROC)GetProcAddress("glDrawBuffersARB");
#endif // BASE_PLATFORM_WINDOWS

			DBGLOG( "EXTENSIONS:  GL_ARB_draw_buffers is supported \n" );
		}
		else
			DBGLOG( "EXTENSIONS:  GL_ARB_draw_buffers requested but not supported\n" );
	}

	//if( nExtRequests & EXT_PACKED_DEPTH_STENCIL )
	{
		// Frame buffer packed depth and stencil
		bExtPackedDepthStencil = isExtensionSupported( "GL_EXT_packed_depth_stenci" );

		if( bExtPackedDepthStencil )
		{
			DBGLOG( "EXTENSIONS:  GL_EXT_packed_depth_stencil is supported \n" );
		}
		else
			DBGLOG( "EXTENSIONS:  GL_EXT_packed_depth_stencil requested but not supported\n" );
	}
}

/////////////////////////////////////////////////////
/// Function: CheckFramebufferStatus
/// Params: None
///
/////////////////////////////////////////////////////
int renderer::CheckFramebufferStatus( void )
{
	GLenum eStatus;
	eStatus = (GLenum)glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	switch(eStatus) 
	{
		case GL_FRAMEBUFFER_COMPLETE_EXT:
			DBGLOG( "Framebuffer status: GL_FRAMEBUFFER_COMPLETE_EXT\n" );
			return(0);
		break;
		case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
			DBGLOG( "Unsupported framebuffer format\n");
			return(1);
		break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
			DBGLOG( "Framebuffer incomplete, missing attachment\n");
			return(1);
		break;
//		case GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT:
//			DBGLOG( "Framebuffer incomplete, duplicate attachment\n");
//			return(1);
//		break;
		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
			DBGLOG( "Framebuffer incomplete, attached images must have same dimensions\n");
			return(1);
		break;
		case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
			DBGLOG( "Framebuffer incomplete, attached images must have same format\n");
			return(1);
		break;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
			DBGLOG( "Framebuffer incomplete, missing draw buffer\n");
			return(1);
		break;
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
			DBGLOG( "Framebuffer incomplete, missing read buffer\n");
			return(1);
		break;

		case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS_EXT:
			DBGLOG( "Framebuffer incomplete, layer targets are incorrect \n");
			return(1);
		break;
		case GL_FRAMEBUFFER_INCOMPLETE_LAYER_COUNT_EXT:
			DBGLOG( "Framebuffer incomplete, layer count is wrong\n");
			return(1);
		break;

		default:
			DBG_ASSERT(0);
	}

	return(1);
}

#endif // BASE_SUPPORT_OPENGL


