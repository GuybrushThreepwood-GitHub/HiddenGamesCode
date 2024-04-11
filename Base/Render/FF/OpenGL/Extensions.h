
#ifndef __EXTENSIONS_H__
#define __EXTENSIONS_H__

#ifdef BASE_PLATFORM_WINDOWS
	#ifndef __WININCLUDES_H__
		#include "Core/win/WinIncludes.h"
	#endif // __WININCLUDES_H__
#endif // BASE_PLATFORM_WINDOWS

#ifndef __PLATFORMRENDERBASE_H__
	#include "PlatformRenderBase.h"
#endif // __PLATFORMRENDERBASE_H__

#ifdef BASE_SUPPORT_OPENGL

namespace renderer
{
	// forward declare
	class OpenGL;

	extern bool bExtMultisample;
	extern bool bExtMultiTexture;
	extern bool bExtTexture3D;
	extern bool bExtFogCoord;
	extern bool bExtPointSprites;
	extern bool bExtPointParameters;
	extern bool bExtTextureAutomipmap;
	extern bool bExtTextureLod;
	extern bool bExtTextureLodBias;
	extern bool bExtTextureCompression;
	extern bool bExtTextureCompressionS3tc;
	extern bool bExtTextureEnvAdd;
	extern bool bExtTextureEnvCombine;
	extern bool bExtTextureEnvCrossbar;
	extern bool bExtTextureEnvDot3;
	extern bool bExtTextureCubeMap;
	extern bool bExtTextureFloat;
	extern bool bExtColourBufferFloat;
	extern bool bExtCompiledVertexArrays;
	extern bool bExtAnisotropicFiltering;
	extern bool bExtVertexBufferObjects;
	extern bool bExtMappedBuffer;
	extern bool bExtVertexPrograms;
	extern bool bExtFragmentPrograms;
	extern bool bExtShaderObjects;
	extern bool bExtVertexShaders;
	extern bool bExtFragmentShaders;
	extern bool bExtGeometryShaders;
	extern bool bExtGLShadingLanguage;
	extern bool bExtPixelBufferObjects;
	extern bool bExtFrameBufferObject;
	extern bool bExtFrameBufferBlit;
	extern bool bExtFrameBufferMultisample;
	extern bool bExtTextureRectangle;
	extern bool bExtTextureNonPowerOfTwo;
	extern bool bExtTextureBorderClamp;
	extern bool bExtShadow;
	extern bool bExtDepthTexture;
	extern bool bExtTextureEdgeClamp;
	extern bool bExtSeparateSpecularColor;
	extern bool bExtDrawBuffers;
	extern bool bExtPackedDepthStencil;

	/// CheckFramebufferStatus - Checks the current state of the active framebuffer, and returns if it is valid
	/// \return integer - (SUCCESS: OK, FAIL: FAIL )
	int CheckFramebufferStatus( void );

	const unsigned long ALL_EXTENSIONS			= -1;

	#ifdef BASE_PLATFORM_WINDOWS
		#define		GetProcAddress		wglGetProcAddress
	#endif // BASE_PLATFORM_WINDOWS

	/// isExtensionSupported - Checks to see if an extension name is found in the current GL driver
	/// \param extName - full extension name
	/// \return boolean - true or false
	bool isExtensionSupported(const char *extName);

	/// isWGLExtensionSupported - Checks to see if a wgl extension name is found in the current GL driver
	/// \param extName - full extension name
	/// \return boolean - true or false
	bool isWGLExtensionSupported(const char *extName);


	/// RequestExtensions - requests certain extensions via flag
	/// \param nExtRequests - extension flags
	void RequestExtensions( unsigned int nExtRequests, renderer::OpenGL* openGLContext=0 );

} // namespace renderer

// extension functions

#if defined( BASE_PLATFORM_WINDOWS )

extern PFNWGLSWAPINTERVALEXTPROC		wglSwapIntervalEXT;

// multitexturing
extern PFNGLACTIVETEXTUREPROC			glActiveTexture;
extern PFNGLCLIENTACTIVETEXTUREPROC		glClientActiveTexture;
extern PFNGLMULTITEXCOORD1DPROC			glMultiTexCoord1d;
extern PFNGLMULTITEXCOORD1DVPROC		glMultiTexCoord1dv;
extern PFNGLMULTITEXCOORD1FPROC			glMultiTexCoord1f;
extern PFNGLMULTITEXCOORD1FVPROC		glMultiTexCoord1fv;
extern PFNGLMULTITEXCOORD1IPROC			glMultiTexCoord1i;
extern PFNGLMULTITEXCOORD1IVPROC		glMultiTexCoord1iv;
extern PFNGLMULTITEXCOORD1SPROC			glMultiTexCoord1s;
extern PFNGLMULTITEXCOORD1SVPROC		glMultiTexCoord1sv;
extern PFNGLMULTITEXCOORD2DPROC			glMultiTexCoord2d;
extern PFNGLMULTITEXCOORD2DVPROC		glMultiTexCoord2dv;
extern PFNGLMULTITEXCOORD2FPROC			glMultiTexCoord2f;
extern PFNGLMULTITEXCOORD2FVPROC		glMultiTexCoord2fv;
extern PFNGLMULTITEXCOORD2IPROC			glMultiTexCoord2i;
extern PFNGLMULTITEXCOORD2IVPROC		glMultiTexCoord2iv;
extern PFNGLMULTITEXCOORD2SPROC			glMultiTexCoord2s;
extern PFNGLMULTITEXCOORD2SVPROC		glMultiTexCoord2sv;
extern PFNGLMULTITEXCOORD3DPROC			glMultiTexCoord3d;
extern PFNGLMULTITEXCOORD3DVPROC		glMultiTexCoord3dv;
extern PFNGLMULTITEXCOORD3FPROC			glMultiTexCoord3f;
extern PFNGLMULTITEXCOORD3FVPROC		glMultiTexCoord3fv;
extern PFNGLMULTITEXCOORD3IPROC			glMultiTexCoord3i;
extern PFNGLMULTITEXCOORD3IVPROC		glMultiTexCoord3iv;
extern PFNGLMULTITEXCOORD3SPROC			glMultiTexCoord3s;
extern PFNGLMULTITEXCOORD3SVPROC		glMultiTexCoord3sv;
extern PFNGLMULTITEXCOORD4DPROC			glMultiTexCoord4d;
extern PFNGLMULTITEXCOORD4DVPROC		glMultiTexCoord4dv;
extern PFNGLMULTITEXCOORD4FPROC			glMultiTexCoord4f;
extern PFNGLMULTITEXCOORD4FVPROC		glMultiTexCoord4fv;
extern PFNGLMULTITEXCOORD4IPROC			glMultiTexCoord4i;
extern PFNGLMULTITEXCOORD4IVPROC		glMultiTexCoord4iv;
extern PFNGLMULTITEXCOORD4SPROC			glMultiTexCoord4s;
extern PFNGLMULTITEXCOORD4SVPROC		glMultiTexCoord4sv;

// texture 3D
extern PFNGLTEXIMAGE3DPROC				glTexImage3D;
extern PFNGLTEXSUBIMAGE3DPROC			glTexSubImage3D;

// fog coord
extern PFNGLFOGCOORDFPROC				glFogCoordf;
extern PFNGLFOGCOORDFVPROC				glFogCoordfv;
extern PFNGLFOGCOORDDPROC				glFogCoordd;
extern PFNGLFOGCOORDDVPROC				glFogCoorddv;
extern PFNGLFOGCOORDPOINTERPROC			glFogCoordPointer;

// point sprites

// point parameters
extern PFNGLPOINTPARAMETERFPROC			glPointParameterf;
extern PFNGLPOINTPARAMETERFVPROC		glPointParameterfv;

// texture automipmap

// texture compression
extern PFNGLCOMPRESSEDTEXIMAGE3DPROC	 glCompressedTexImage3D;
extern PFNGLCOMPRESSEDTEXIMAGE2DPROC	 glCompressedTexImage2D;
extern PFNGLCOMPRESSEDTEXIMAGE1DPROC	 glCompressedTexImage1D;
extern PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC	 glCompressedTexSubImage3D;
extern PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC	 glCompressedTexSubImage2D;
extern PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC	 glCompressedTexSubImage1D;
extern PFNGLGETCOMPRESSEDTEXIMAGEPROC	 glGetCompressedTexImage;

// texture cube map
// texture env add
// texture env combine
// texture env crossbar
// texture env dot3

// colour buffer float
extern PFNGLCLAMPCOLORARBPROC				glClampColorARB;

// compiled vertex arrays
extern PFNGLLOCKARRAYSEXTPROC				glLockArraysEXT;
extern PFNGLUNLOCKARRAYSEXTPROC				glUnlockArraysEXT;

// vertex buffer objects
extern PFNGLBINDBUFFERPROC					glBindBuffer;
extern PFNGLDELETEBUFFERSPROC				glDeleteBuffers;
extern PFNGLGENBUFFERSPROC					glGenBuffers;
extern PFNGLISBUFFERPROC					glIsBuffer;
extern PFNGLBUFFERDATAPROC					glBufferData;
extern PFNGLBUFFERSUBDATAPROC				glBufferSubData;
extern PFNGLGETBUFFERSUBDATAPROC			glGetBufferSubData;
extern PFNGLMAPBUFFERPROC					glMapBuffer;
extern PFNGLUNMAPBUFFERPROC					glUnmapBuffer;
extern PFNGLGETBUFFERPARAMETERIVPROC		glGetBufferParameteriv;
extern PFNGLGETBUFFERPOINTERVPROC			glGetBufferPointerv;

// anisotropic filter

// vertex and fragment programs
extern PFNGLVERTEXATTRIB1DARBPROC				glVertexAttrib1dARB;
extern PFNGLVERTEXATTRIB1DVARBPROC				glVertexAttrib1dvARB;
extern PFNGLVERTEXATTRIB1FARBPROC				glVertexAttrib1fARB;
extern PFNGLVERTEXATTRIB1FVARBPROC				glVertexAttrib1fvARB;
extern PFNGLVERTEXATTRIB1SARBPROC				glVertexAttrib1sARB;
extern PFNGLVERTEXATTRIB1SVARBPROC				glVertexAttrib1svARB;
extern PFNGLVERTEXATTRIB2DARBPROC				glVertexAttrib2dARB;
extern PFNGLVERTEXATTRIB2DVARBPROC				glVertexAttrib2dvARB;
extern PFNGLVERTEXATTRIB2FARBPROC				glVertexAttrib2fARB;
extern PFNGLVERTEXATTRIB2FVARBPROC				glVertexAttrib2fvARB;
extern PFNGLVERTEXATTRIB2SARBPROC				glVertexAttrib2sARB;
extern PFNGLVERTEXATTRIB2SVARBPROC				glVertexAttrib2svARB;
extern PFNGLVERTEXATTRIB3DARBPROC				glVertexAttrib3dARB;
extern PFNGLVERTEXATTRIB3DVARBPROC				glVertexAttrib3dvARB;
extern PFNGLVERTEXATTRIB3FARBPROC				glVertexAttrib3fARB;
extern PFNGLVERTEXATTRIB3FVARBPROC				glVertexAttrib3fvARB;
extern PFNGLVERTEXATTRIB3SARBPROC				glVertexAttrib3sARB;
extern PFNGLVERTEXATTRIB3SVARBPROC				glVertexAttrib3svARB;
extern PFNGLVERTEXATTRIB4NBVARBPROC				glVertexAttrib4NbvARB;
extern PFNGLVERTEXATTRIB4NIVARBPROC				glVertexAttrib4NivARB;
extern PFNGLVERTEXATTRIB4NSVARBPROC				glVertexAttrib4NsvARB;
extern PFNGLVERTEXATTRIB4NUBARBPROC				glVertexAttrib4NubARB;
extern PFNGLVERTEXATTRIB4NUBVARBPROC			glVertexAttrib4NubvARB;
extern PFNGLVERTEXATTRIB4NUIVARBPROC			glVertexAttrib4NuivARB;
extern PFNGLVERTEXATTRIB4NUSVARBPROC			glVertexAttrib4NusvARB;
extern PFNGLVERTEXATTRIB4BVARBPROC				glVertexAttrib4bvARB;
extern PFNGLVERTEXATTRIB4DARBPROC				glVertexAttrib4dARB;
extern PFNGLVERTEXATTRIB4DVARBPROC				glVertexAttrib4dvARB;
extern PFNGLVERTEXATTRIB4FARBPROC				glVertexAttrib4fARB;
extern PFNGLVERTEXATTRIB4FVARBPROC				glVertexAttrib4fvARB;
extern PFNGLVERTEXATTRIB4IVARBPROC				glVertexAttrib4ivARB;
extern PFNGLVERTEXATTRIB4SARBPROC				glVertexAttrib4sARB;
extern PFNGLVERTEXATTRIB4SVARBPROC				glVertexAttrib4svARB;
extern PFNGLVERTEXATTRIB4UBVARBPROC				glVertexAttrib4ubvARB;
extern PFNGLVERTEXATTRIB4UIVARBPROC				glVertexAttrib4uivARB;
extern PFNGLVERTEXATTRIB4USVARBPROC				glVertexAttrib4usvARB;
extern PFNGLVERTEXATTRIBPOINTERARBPROC			glVertexAttribPointerARB;
extern PFNGLENABLEVERTEXATTRIBARRAYARBPROC		glEnableVertexAttribArrayARB;
extern PFNGLDISABLEVERTEXATTRIBARRAYARBPROC		glDisableVertexAttribArrayARB;
extern PFNGLPROGRAMSTRINGARBPROC				glProgramStringARB;
extern PFNGLBINDPROGRAMARBPROC					glBindProgramARB;
extern PFNGLDELETEPROGRAMSARBPROC				glDeleteProgramsARB;
extern PFNGLGENPROGRAMSARBPROC					glGenProgramsARB;
extern PFNGLPROGRAMENVPARAMETER4DARBPROC		glProgramEnvParameter4dARB;
extern PFNGLPROGRAMENVPARAMETER4DVARBPROC		glProgramEnvParameter4dvARB;
extern PFNGLPROGRAMENVPARAMETER4FARBPROC		glProgramEnvParameter4fARB;
extern PFNGLPROGRAMENVPARAMETER4FVARBPROC		glProgramEnvParameter4fvARB;
extern PFNGLPROGRAMLOCALPARAMETER4DARBPROC		glProgramLocalParameter4dARB;
extern PFNGLPROGRAMLOCALPARAMETER4DVARBPROC		glProgramLocalParameter4dvARB;
extern PFNGLPROGRAMLOCALPARAMETER4FARBPROC		glProgramLocalParameter4fARB;
extern PFNGLPROGRAMLOCALPARAMETER4FVARBPROC		glProgramLocalParameter4fvARB;
extern PFNGLGETPROGRAMENVPARAMETERDVARBPROC		glGetProgramEnvParameterdvARB;
extern PFNGLGETPROGRAMENVPARAMETERFVARBPROC		glGetProgramEnvParameterfvARB;
extern PFNGLGETPROGRAMLOCALPARAMETERDVARBPROC	glGetProgramLocalParameterdvARB;
extern PFNGLGETPROGRAMLOCALPARAMETERFVARBPROC	glGetProgramLocalParameterfvARB;
extern PFNGLGETPROGRAMIVARBPROC					glGetProgramivARB;
extern PFNGLGETPROGRAMSTRINGARBPROC				glGetProgramStringARB;
extern PFNGLGETVERTEXATTRIBDVARBPROC			glGetVertexAttribdvARB;
extern PFNGLGETVERTEXATTRIBFVARBPROC			glGetVertexAttribfvARB;
extern PFNGLGETVERTEXATTRIBIVARBPROC			glGetVertexAttribivARB;
extern PFNGLGETVERTEXATTRIBPOINTERVARBPROC		glGetVertexAttribPointervARB;
extern PFNGLISPROGRAMARBPROC					glIsProgramARB;

// shader objects
/*
extern PFNGLDELETEOBJECTARBPROC                    glDeleteObjectARB;
extern PFNGLGETHANDLEARBPROC                       glGetHandleARB;
extern PFNGLDETACHOBJECTARBPROC                    glDetachObjectARB;
extern PFNGLCREATESHADEROBJECTARBPROC              glCreateShaderObjectARB;
extern PFNGLSHADERSOURCEARBPROC                    glShaderSourceARB;
extern PFNGLCOMPILESHADERARBPROC                   glCompileShaderARB;
extern PFNGLCREATEPROGRAMOBJECTARBPROC             glCreateProgramObjectARB;
extern PFNGLATTACHOBJECTARBPROC                    glAttachObjectARB;
extern PFNGLLINKPROGRAMARBPROC                     glLinkProgramARB;
extern PFNGLUSEPROGRAMOBJECTARBPROC                glUseProgramObjectARB;
extern PFNGLVALIDATEPROGRAMARBPROC                 glValidateProgramARB;
extern PFNGLUNIFORM1FARBPROC                       glUniform1fARB;
extern PFNGLUNIFORM2FARBPROC                       glUniform2fARB;
extern PFNGLUNIFORM3FARBPROC                       glUniform3fARB;
extern PFNGLUNIFORM4FARBPROC                       glUniform4fARB;
extern PFNGLUNIFORM1IARBPROC                       glUniform1iARB;
extern PFNGLUNIFORM2IARBPROC                       glUniform2iARB;
extern PFNGLUNIFORM3IARBPROC                       glUniform3iARB;
extern PFNGLUNIFORM4IARBPROC                       glUniform4iARB;
extern PFNGLUNIFORM1FVARBPROC                      glUniform1fvARB;
extern PFNGLUNIFORM2FVARBPROC                      glUniform2fvARB;
extern PFNGLUNIFORM3FVARBPROC                      glUniform3fvARB;
extern PFNGLUNIFORM4FVARBPROC                      glUniform4fvARB;
extern PFNGLUNIFORM1IVARBPROC                      glUniform1ivARB;
extern PFNGLUNIFORM2IVARBPROC                      glUniform2ivARB;
extern PFNGLUNIFORM3IVARBPROC                      glUniform3ivARB;
extern PFNGLUNIFORM4IVARBPROC                      glUniform4ivARB;
extern PFNGLUNIFORMMATRIX2FVARBPROC                glUniformMatrix2fvARB;
extern PFNGLUNIFORMMATRIX3FVARBPROC                glUniformMatrix3fvARB;
extern PFNGLUNIFORMMATRIX4FVARBPROC                glUniformMatrix4fvARB;
extern PFNGLGETOBJECTPARAMETERFVARBPROC            glGetObjectParameterfvARB;
extern PFNGLGETOBJECTPARAMETERIVARBPROC            glGetObjectParameterivARB;
extern PFNGLGETINFOLOGARBPROC                      glGetInfoLogARB;
extern PFNGLGETATTACHEDOBJECTSARBPROC              glGetAttachedObjectsARB;
extern PFNGLGETUNIFORMLOCATIONARBPROC              glGetUniformLocationARB;
extern PFNGLGETACTIVEUNIFORMARBPROC                glGetActiveUniformARB;
extern PFNGLGETUNIFORMFVARBPROC                    glGetUniformfvARB;
extern PFNGLGETUNIFORMIVARBPROC                    glGetUniformivARB;
extern PFNGLGETSHADERSOURCEARBPROC                 glGetShaderSourceARB;

// vertex shader
extern PFNGLBINDATTRIBLOCATIONARBPROC       glBindAttribLocationARB;
extern PFNGLGETACTIVEATTRIBARBPROC          glGetActiveAttribARB;
extern PFNGLGETATTRIBLOCATIONARBPROC        glGetAttribLocationARB;
*/

extern PFNGLATTACHSHADERPROC						glAttachShader;
extern PFNGLBINDATTRIBLOCATIONPROC					glBindAttribLocation;
extern PFNGLCOMPILESHADERPROC						glCompileShader;
extern PFNGLCREATEPROGRAMPROC						glCreateProgram;
extern PFNGLCREATESHADERPROC						glCreateShader;
extern PFNGLDELETEPROGRAMPROC						glDeleteProgram;
extern PFNGLDELETESHADERPROC						glDeleteShader;
extern PFNGLDETACHSHADERPROC						glDetachShader;
extern PFNGLDISABLEVERTEXATTRIBARRAYPROC			glDisableVertexAttribArray;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC				glEnableVertexAttribArray;
extern PFNGLGETACTIVEATTRIBPROC						glGetActiveAttrib;
extern PFNGLGETACTIVEUNIFORMPROC					glGetActiveUniform;
extern PFNGLGETATTACHEDSHADERSPROC					glGetAttachedShaders;
extern PFNGLGETATTRIBLOCATIONPROC					glGetAttribLocation;
extern PFNGLGETPROGRAMIVPROC						glGetProgramiv;
extern PFNGLGETPROGRAMINFOLOGPROC					glGetProgramInfoLog;
extern PFNGLGETSHADERIVPROC							glGetShaderiv;
extern PFNGLGETSHADERINFOLOGPROC					glGetShaderInfoLog;
extern PFNGLGETSHADERSOURCEPROC						glGetShaderSource;
extern PFNGLGETUNIFORMLOCATIONPROC					glGetUniformLocation;
extern PFNGLGETUNIFORMFVPROC						glGetUniformfv;
extern PFNGLGETUNIFORMIVPROC						glGetUniformiv;
extern PFNGLGETVERTEXATTRIBDVPROC					glGetVertexAttribdv;
extern PFNGLGETVERTEXATTRIBFVPROC					glGetVertexAttribfv;
extern PFNGLGETVERTEXATTRIBIVPROC					glGetVertexAttribiv;
extern PFNGLGETVERTEXATTRIBPOINTERVPROC				glGetVertexAttribPointerv;
extern PFNGLISPROGRAMPROC							glIsProgram;
extern PFNGLISSHADERPROC							glIsShader;
extern PFNGLLINKPROGRAMPROC							glLinkProgram;
extern PFNGLSHADERSOURCEPROC						glShaderSource;
extern PFNGLUSEPROGRAMPROC							glUseProgram;
extern PFNGLUNIFORM1FPROC							glUniform1f;
extern PFNGLUNIFORM2FPROC							glUniform2f;
extern PFNGLUNIFORM3FPROC							glUniform3f;
extern PFNGLUNIFORM4FPROC							glUniform4f;
extern PFNGLUNIFORM1IPROC							glUniform1i;
extern PFNGLUNIFORM2IPROC							glUniform2i;
extern PFNGLUNIFORM3IPROC							glUniform3i;
extern PFNGLUNIFORM4IPROC							glUniform4i;
extern PFNGLUNIFORM1FVPROC							glUniform1fv;
extern PFNGLUNIFORM2FVPROC							glUniform2fv;
extern PFNGLUNIFORM3FVPROC							glUniform3fv;
extern PFNGLUNIFORM4FVPROC							glUniform4fv;
extern PFNGLUNIFORM1IVPROC							glUniform1iv;
extern PFNGLUNIFORM2IVPROC							glUniform2iv;
extern PFNGLUNIFORM3IVPROC							glUniform3iv;
extern PFNGLUNIFORM4IVPROC							glUniform4iv;
extern PFNGLUNIFORMMATRIX2FVPROC					glUniformMatrix2fv;
extern PFNGLUNIFORMMATRIX3FVPROC					glUniformMatrix3fv;
extern PFNGLUNIFORMMATRIX4FVPROC					glUniformMatrix4fv;
extern PFNGLVALIDATEPROGRAMPROC						glValidateProgram;
extern PFNGLVERTEXATTRIB1DPROC						glVertexAttrib1d;
extern PFNGLVERTEXATTRIB1DVPROC						glVertexAttrib1dv;
extern PFNGLVERTEXATTRIB1FPROC						glVertexAttrib1f;
extern PFNGLVERTEXATTRIB1FVPROC						glVertexAttrib1fv;
extern PFNGLVERTEXATTRIB1SPROC						glVertexAttrib1s;
extern PFNGLVERTEXATTRIB1SVPROC						glVertexAttrib1sv;
extern PFNGLVERTEXATTRIB2DPROC						glVertexAttrib2d;
extern PFNGLVERTEXATTRIB2DVPROC						glVertexAttrib2dv;
extern PFNGLVERTEXATTRIB2FPROC						glVertexAttrib2f;
extern PFNGLVERTEXATTRIB2FVPROC						glVertexAttrib2fv;
extern PFNGLVERTEXATTRIB2SPROC						glVertexAttrib2s;
extern PFNGLVERTEXATTRIB2SVPROC						glVertexAttrib2sv;
extern PFNGLVERTEXATTRIB3DPROC						glVertexAttrib3d;
extern PFNGLVERTEXATTRIB3DVPROC						glVertexAttrib3dv;
extern PFNGLVERTEXATTRIB3FPROC						glVertexAttrib3f;
extern PFNGLVERTEXATTRIB3FVPROC						glVertexAttrib3fv;
extern PFNGLVERTEXATTRIB3SPROC						glVertexAttrib3s;
extern PFNGLVERTEXATTRIB3SVPROC						glVertexAttrib3sv;
extern PFNGLVERTEXATTRIB4NBVPROC					glVertexAttrib4Nbv;
extern PFNGLVERTEXATTRIB4NIVPROC					glVertexAttrib4Niv;
extern PFNGLVERTEXATTRIB4NSVPROC					glVertexAttrib4Nsv;
extern PFNGLVERTEXATTRIB4NUBPROC					glVertexAttrib4Nub;
extern PFNGLVERTEXATTRIB4NUBVPROC					glVertexAttrib4Nubv;
extern PFNGLVERTEXATTRIB4NUIVPROC					glVertexAttrib4Nuiv;
extern PFNGLVERTEXATTRIB4NUSVPROC					glVertexAttrib4Nusv;
extern PFNGLVERTEXATTRIB4BVPROC						glVertexAttrib4bv;
extern PFNGLVERTEXATTRIB4DPROC						glVertexAttrib4d;
extern PFNGLVERTEXATTRIB4DVPROC						glVertexAttrib4dv;
extern PFNGLVERTEXATTRIB4FPROC						glVertexAttrib4f;
extern PFNGLVERTEXATTRIB4FVPROC						glVertexAttrib4fv;
extern PFNGLVERTEXATTRIB4IVPROC						glVertexAttrib4iv;
extern PFNGLVERTEXATTRIB4SPROC						glVertexAttrib4s;
extern PFNGLVERTEXATTRIB4SVPROC						glVertexAttrib4sv;
extern PFNGLVERTEXATTRIB4UBVPROC					glVertexAttrib4ubv;
extern PFNGLVERTEXATTRIB4UIVPROC					glVertexAttrib4uiv;
extern PFNGLVERTEXATTRIB4USVPROC					glVertexAttrib4usv;
extern PFNGLVERTEXATTRIBPOINTERPROC					glVertexAttribPointer;

// geometry shader
extern PFNGLPROGRAMPARAMETERIEXTPROC				glProgramParameteriEXT;

// frame buffer object
extern PFNGLISRENDERBUFFEREXTPROC						glIsRenderbufferEXT;
extern PFNGLBINDRENDERBUFFEREXTPROC						glBindRenderbufferEXT;
extern PFNGLDELETERENDERBUFFERSEXTPROC					glDeleteRenderbuffersEXT;
extern PFNGLGENRENDERBUFFERSEXTPROC						glGenRenderbuffersEXT;
extern PFNGLRENDERBUFFERSTORAGEEXTPROC					glRenderbufferStorageEXT;
extern PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC			glGetRenderbufferParameterivEXT;
extern PFNGLISFRAMEBUFFEREXTPROC						glIsFramebufferEXT;
extern PFNGLBINDFRAMEBUFFEREXTPROC						glBindFramebufferEXT;
extern PFNGLDELETEFRAMEBUFFERSEXTPROC					glDeleteFramebuffersEXT;
extern PFNGLGENFRAMEBUFFERSEXTPROC						glGenFramebuffersEXT;
extern PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC				glCheckFramebufferStatusEXT;
extern PFNGLFRAMEBUFFERTEXTURE1DEXTPROC					glFramebufferTexture1DEXT;
extern PFNGLFRAMEBUFFERTEXTURE2DEXTPROC					glFramebufferTexture2DEXT;
extern PFNGLFRAMEBUFFERTEXTURE3DEXTPROC					glFramebufferTexture3DEXT;
extern PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC				glFramebufferRenderbufferEXT;
extern PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC	glGetFramebufferAttachmentParameterivEXT;
extern PFNGLGENERATEMIPMAPEXTPROC						glGenerateMipmapEXT;

// frame buffer blit
extern PFNGLBLITFRAMEBUFFEREXTPROC						glBlitFramebufferEXT;

// frame buffer multisample
extern PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC		glRenderbufferStorageMultisampleEXT;

// draw buffer
extern PFNGLDRAWBUFFERSPROC							glDrawBuffers;

#endif // BASE_PLATFORM_WINDOWS


#if defined(BASE_PLATFORM_MAC)
// GL_IMG_texture_compression_pvrtc
#define GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG                      0x8C00
#define GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG                      0x8C01
#define GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG                     0x8C02
#define GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG                     0x8C03

#define GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS					0x8CD9
#endif // defined(BASE_PLATFORM_MAC)

#endif // BASE_SUPPORT_OPENGL

#endif // __EXTENSIONS_H__


