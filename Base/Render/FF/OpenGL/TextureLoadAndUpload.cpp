
/*===================================================================
	File: TextureLoadAndUpload.cpp
	Library: Render

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"

#ifdef BASE_SUPPORT_OPENGL

#include <cstring>
#include <cmath>

#include "Math/Vectors.h"

#include "Render/OpenGLCommon.h"
#include "Render/FF/OpenGL/OpenGL.h"
#include "Render/FF/OpenGL/Extensions.h"
#include "Render/TextureShared.h"
#include "Render/Texture.h"

#include "Render/FF/OpenGL/TextureLoadAndUpload.h"

namespace
{
	struct lstr
	{
		bool operator() (std::string s1, std::string s2) const
		{
			return( (s1.compare(s2)) < 0);
		}
	};

	struct TexStore
	{
		char fullFilename[core::MAX_PATH+core::MAX_PATH];

		int refCount;
		GLuint texId;
		int texUnit;
		GLenum minFilter;
		GLenum magFilter;
		int imageWidth;
		int imageHeight;
		int imageBPP;
		bool resized;
		int originalWidth;
		int originalHeight;
		renderer::ETextureFormat texFormat;
		bool genMipmaps;
	};

	typedef std::map< std::string, TexStore, struct lstr> TNameIntMap;
	typedef std::pair< std::string, TexStore> TNameIntPair;

	TNameIntMap TextureMap;
}

/////////////////////////////////////////////////////
/// Function: GetTextureFormat
/// Params: 
///
/////////////////////////////////////////////////////
renderer::ETextureFormat renderer::GetTextureFormat( const char* szFilename )
{
	char extStr[core::MAX_EXT];
	std::memset( extStr, 0, sizeof( char )*core::MAX_EXT );
	core::SplitPath( szFilename, 0, 0, 0, extStr );

	if( !core::IsEmptyString( szFilename ) )
	{
		if( ( std::strcmp( extStr, ".TGA" ) == 0 ) || ( std::strcmp( extStr, ".tga" ) == 0 ) )
		{
			return renderer::TEXTURE_TGA;
		}
#ifdef BASE_SUPPORT_BMP
		else if( ( std::strcmp( extStr, ".BMP" ) == 0 ) || ( std::strcmp( extStr, ".bmp" ) == 0 ) )
		{
			return renderer::TEXTURE_BMP;
		}
#endif // BASE_SUPPORT_BMP
#ifdef BASE_SUPPORT_PNG
		else if( ( std::strcmp( extStr, ".PNG" ) == 0 ) || ( std::strcmp( extStr, ".png" ) == 0 ) )
		{
			return renderer::TEXTURE_PNG;
		}
#endif // BASE_SUPPORT_PNG
#ifdef BASE_SUPPORT_DDS
		else if( ( std::strcmp( extStr, ".DDS" ) == 0 ) || ( std::strcmp( extStr, ".dds" ) == 0 ) )
		{
			return renderer::TEXTURE_DDS;
		}
#endif // BASE_SUPPORT_DDS
#ifdef BASE_SUPPORT_JPEG
		else if( ( std::strcmp( extStr, ".JPEG" ) == 0 ) || ( std::strcmp( extStr, ".jpeg" ) == 0 ) || ( std::strcmp( extStr, ".JPG" ) == 0 ) || ( std::strcmp( extStr, ".jpg" ) == 0 ) )
		{
			return renderer::TEXTURE_JPEG;
		}
#endif // BASE_SUPPORT_JPEG
#ifdef BASE_SUPPORT_PCX
		else if( ( std::strcmp( extStr, ".PCX" ) == 0 ) || ( std::strcmp( extStr, ".pcx" ) == 0 ) )
		{
			return renderer::TEXTURE_PCX;
		}
#endif // BASE_SUPPORT_PCX
#ifdef BASE_SUPPORT_PVR
		else if( ( std::strcmp( extStr, ".PVR" ) == 0 ) || ( std::strcmp( extStr, ".pvr" ) == 0 ) )
		{
			return renderer::TEXTURE_PVR;
		}
#endif // BASE_SUPPORT_PVR
	}

	return renderer::TEXTURE_UNKNOWN;
}

/////////////////////////////////////////////////////
/// Function: ClearTextureMap
/// Params: 
///
/////////////////////////////////////////////////////
void renderer::ClearTextureMap()
{
	TNameIntMap::iterator it;

	it = TextureMap.begin();
	
	while( it != TextureMap.end() )
	{
#ifdef _DEBUG
		DBGLOG( "*WARNING* Texture %s still active\n", it->first.c_str() );
#endif // _DEBUG

		if( glIsTexture(it->second.texId) )
			glDeleteTextures( 1, &it->second.texId );

		it++;
	}

	TextureMap.clear();
}

/////////////////////////////////////////////////////
/// Function: TextureLoad
/// Params: [in]szFilename
///
/////////////////////////////////////////////////////
GLuint renderer::TextureLoad(const char *szFilename, Texture& tex, ETextureFormat texFormat, file::TFileHandle* pFile, bool genMipMaps, GLenum magFilter, GLenum minFilter, int texUnit, bool reload, GLuint texObjectId, renderer::OpenGL* openGLContext)
{
	int i = 0;
	GLuint nTextureID = renderer::INVALID_OBJECT;

	GLenum eTarget = GL_TEXTURE_2D;
	GLenum eWrapS = GL_REPEAT;
	GLenum eWrapT = GL_REPEAT;
	GLenum eWrapR = GL_REPEAT;

	GLenum eMagFilter = magFilter;
	GLenum eMinFilter = minFilter;

	float fAnisotropyLevel = 1.0f;
	bool bCanResize = true;
	bool bResizeByStretch = true;
	bool bGenMipmaps = genMipMaps;
	bool bCompress = false;

	file::TFile fileData;
	if( szFilename )
	{	
		file::CreateFileStructure( szFilename, &fileData );

		TNameIntMap::iterator it;
		it = TextureMap.find( std::string(fileData.szFile) );

		// did iterator not hit the end
		if( it != TextureMap.end() )
		{
			it->second.refCount++;
			tex.nWidth = it->second.imageWidth;
			tex.nHeight = it->second.imageHeight;
			tex.nOriginalWidth = it->second.originalWidth;
			tex.nOriginalHeight = it->second.originalHeight;
			tex.nBPP = it->second.imageBPP;
			return( static_cast<int>(it->second.texId) );
		}
	}
	
	bool wasResized = false;

	// assign the new texture
	if( tex.Load( szFilename, texFormat, pFile ) == 0 )
	{
		if( tex.bIsCubemap )
			eTarget = GL_TEXTURE_CUBE_MAP;

		int nIsHeightPOW2 = IsPowerOfTwo( tex.nHeight );
		int nIsWidthPOW2 = IsPowerOfTwo( tex.nWidth );

		// create one texture
		if (reload &&
			texObjectId != renderer::INVALID_OBJECT)
			nTextureID = texObjectId;
		else
			glGenTextures(1, &nTextureID);

		// make the texture the current one
		if (openGLContext == 0)
			renderer::OpenGL::GetInstance()->BindUnitTexture(texUnit, eTarget, nTextureID);
		else
			openGLContext->BindUnitTexture(texUnit, eTarget, nTextureID);

		// texture parameters
		glTexParameterf( eTarget, GL_TEXTURE_WRAP_S, (GLfloat)eWrapS );
		glTexParameterf( eTarget, GL_TEXTURE_WRAP_T, (GLfloat)eWrapT );
		if( tex.bIsCubemap )
			glTexParameterf( eTarget, GL_TEXTURE_WRAP_R, (GLfloat)eWrapR );
	
		if( !renderer::bExtTextureNonPowerOfTwo && (!nIsHeightPOW2 || !nIsWidthPOW2) )
		{
			// see if resizing is allowed, don't try and resize a DDS file as it might be a compressed format
			if( bCanResize && tex.eFileFormat != TEXTURE_DDS )
			{
				tex.Flip();
				tex.Resize( bResizeByStretch );
				tex.Flip();
				wasResized = true;
				DBGLOG( "TEXMANAGER: *WARNING* Texture resized to w:%d, h:%d\n", tex.nWidth, tex.nHeight );
			}
			else
			{
				DBGLOG( "TEXMANAGER: *WARNING* texture '%s' had NPOT size, cannot resize textures\n", szFilename );
				return(INVALID_OBJECT);
			}

		}

		// set correct filter state
		glTexParameterf( eTarget, GL_TEXTURE_MAG_FILTER, (GLfloat)eMagFilter );
		glTexParameterf( eTarget, GL_TEXTURE_MIN_FILTER, (GLfloat)eMinFilter );

		if( renderer::bExtAnisotropicFiltering )
			glTexParameterf( eTarget, GL_TEXTURE_MAX_ANISOTROPY_EXT, fAnisotropyLevel );

		// DDS special case of auto loading the mipmaps stored within it
		if( tex.eFileFormat == TEXTURE_DDS && renderer::bExtTextureCompression && renderer::bExtTextureCompressionS3tc )
		{
			int w = tex.nWidth;
			int h = tex.nHeight;
			int d = (tex.nDepth) ? tex.nDepth : 1;

			// turn on auto mipmap if there aren't any in the file
			if( bGenMipmaps && tex.nMipMapCount <= 1 )
			{
				if( renderer::bExtTextureAutomipmap )
				{
					// hardware mipmap creation
					glTexParameteri( eTarget, GL_GENERATE_MIPMAP_SGIS, GL_TRUE );
				}
			}

			if( tex.bIsCubemap )
			{
				for( i = 0; i < 6; i++ )
				{
					GLenum eTexImageTarget = GL_TEXTURE_CUBE_MAP_POSITIVE_X+i;

					// reset size for each face
					w = tex.nWidth;
					h = tex.nHeight;
					d = (tex.nDepth) ? tex.nDepth : 1;

					for( int level = 0; level < tex.nMipMapCount; level++ ) 
					{
						int bw = (tex.bDXTCompressed) ? (w+3)/4 : w;
						int bh = (tex.bDXTCompressed) ? (h+3)/4 : h;
						int size = bw*bh*d*tex.nBytesPerElement;

						if( tex.bDXTCompressed )
						{
							glCompressedTexImage2D( eTexImageTarget, level, tex.nFormat, w, h, 0, size,
														tex.imageList[ level + (i*tex.nMipMapCount) ] );
						}
						else
						{
							glTexImage2D( eTexImageTarget, level, tex.nInternalFormat, w, h, 0, tex.nFormat,
											tex.nTextureType, tex.imageList[ level + (i*tex.nMipMapCount) ] );
						}

						// reduce mip sizes
						w = ( w > 1 ) ? w >> 1 : 1;
						h = ( h > 1 ) ? h >> 1 : 1;
						d = ( d > 1 ) ? d >> 1 : 1;
					}
				}
			}
			else
			{
				for( int level = 0; level < tex.nMipMapCount; level++ ) 
				{
					int bw = (tex.bDXTCompressed) ? (w+3)/4 : w;
					int bh = (tex.bDXTCompressed) ? (h+3)/4 : h;
					int size = bw*bh*d*tex.nBytesPerElement;

					// no mipmaps in the DDS, but are needed
					if( bGenMipmaps && tex.nMipMapCount == 1 )
					{
						if( renderer::bExtTextureAutomipmap )
						{
							// hardware mipmap creation
							glTexParameteri( eTarget, GL_GENERATE_MIPMAP_SGIS, GL_TRUE );
						}
					}

					// FIXME: what to do when you need mipmaps that aren't already in a DDS file, and you don't
					// have the on card mipmap generation ???
					if( tex.bDXTCompressed )
					{
						if( eTarget == GL_TEXTURE_1D )
						{
							glCompressedTexImage1D( eTarget, level, tex.nFormat, w, 0, size,
														tex.imageList[level] );
						}
						else if( eTarget == GL_TEXTURE_2D )
						{
							glCompressedTexImage2D( eTarget, level, tex.nFormat, w, h, 0, size,
														tex.imageList[level] );
						}
					}
					else
					{
						if( eTarget == GL_TEXTURE_1D )
						{
							glTexImage1D( eTarget, level, tex.nInternalFormat, w, 0, tex.nFormat,
											tex.nTextureType, tex.imageList[level] );
						}
						else if( eTarget == GL_TEXTURE_2D )
						{
							glTexImage2D( eTarget, level, tex.nInternalFormat, w, h, 0, tex.nFormat,
											tex.nTextureType, tex.imageList[level] );
						}
					}

					// reduce mip sizes
					w = ( w > 1 ) ? w >> 1 : 1;
					h = ( h > 1 ) ? h >> 1 : 1;
					d = ( d > 1 ) ? d >> 1 : 1;
				}
			}
		}
		else
		{
			// should it be compressed ?
			if( bCompress && renderer::bExtTextureCompression )
			{
				// change internal format if there is going to be compression
				switch( tex.nInternalFormat )
				{
					case GL_LUMINANCE8:
					case GL_LUMINANCE8_ALPHA8:
					case GL_RGB8:
//						tex.nInternalFormat = GL_COMPRESSED_LUMINANCE_ARB;
//						tex.nInternalFormat = GL_COMPRESSED_LUMINANCE_ALPHA;
						tex.nInternalFormat = GL_COMPRESSED_RGB;
					break;

					case GL_RGBA8:
						tex.nInternalFormat = GL_COMPRESSED_RGBA;
					break;
				}
			}

			if( bGenMipmaps )
			{
				if( renderer::bExtTextureAutomipmap )
				{
					// hardware mipmap creation
					glTexParameteri( eTarget, GL_GENERATE_MIPMAP_SGIS, GL_TRUE );

					if( eTarget == GL_TEXTURE_1D )
					{
						glTexImage1D( eTarget, 0, tex.nInternalFormat, tex.nWidth, 0, tex.nFormat,
											tex.nTextureType, tex.image );
					}
					else if( eTarget == GL_TEXTURE_2D )
					{
						glTexImage2D( eTarget, 0, tex.nInternalFormat, tex.nWidth, tex.nHeight, 0, tex.nFormat,
											tex.nTextureType, tex.image );
					}
				}
				else
				{
					if( eTarget == GL_TEXTURE_1D )
					{
						gluBuild1DMipmaps( eTarget, tex.nInternalFormat, tex.nWidth, tex.nFormat,
												tex.nTextureType, tex.image );
					}
					else if( eTarget == GL_TEXTURE_2D )
					{
						gluBuild2DMipmaps( eTarget, tex.nInternalFormat, tex.nWidth, tex.nHeight, tex.nFormat,
												tex.nTextureType, tex.image );
					}
				}
			}
			else
			{
				// no mipmaps, just base level
				if( eTarget == GL_TEXTURE_1D )
				{
					glTexImage1D( eTarget, 0, tex.nInternalFormat, tex.nWidth, 0, tex.nFormat,
											tex.nTextureType, tex.image );
				}
				else if( eTarget == GL_TEXTURE_2D )
				{
					glTexImage2D( eTarget, 0, tex.nInternalFormat, tex.nWidth, tex.nHeight, 0, tex.nFormat,
											tex.nTextureType, tex.image );
				}
			}

			if( bCompress && renderer::bExtTextureCompression )
			{
				// Check if it was effectively compressed
				bool bWasCompressed = false;
				int nNewTextureSize = 0;
				int nCompressedInternalFormat;
				glGetTexLevelParameteriv( eTarget, 0, GL_TEXTURE_COMPRESSED, (GLint *)&bWasCompressed );
				glGetTexLevelParameteriv( eTarget, 0, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, (GLint *)&nNewTextureSize );
				glGetTexLevelParameteriv( eTarget, 0, GL_TEXTURE_INTERNAL_FORMAT, (GLint *)&nCompressedInternalFormat );

				DBGLOG( "TEXMANAGER: Compressed: %s Format: %d\n", bWasCompressed?"true":"false", nCompressedInternalFormat );

				if( bWasCompressed )
				{
					DBGLOG( "TEXMANAGER: Texture '%s' has been compressed to size:%d\n", szFilename, nNewTextureSize );
					tex.nCompressedTextureSize = nNewTextureSize;
				}
			}
		}

		tex.nID = nTextureID;

		if( szFilename )
		{
			TexStore addTex;
			addTex.refCount = 1;
			addTex.texId = nTextureID;
			addTex.minFilter = minFilter;
			addTex.magFilter = magFilter;
			addTex.texUnit = 0;
			addTex.texFormat = texFormat;
			addTex.imageWidth = tex.nWidth;
			addTex.imageHeight = tex.nHeight;
			addTex.imageBPP = tex.nBPP;
			addTex.resized = wasResized;
			addTex.originalWidth = tex.nOriginalWidth;
			addTex.originalHeight = tex.nOriginalHeight;
			snprintf( addTex.fullFilename, core::MAX_PATH+core::MAX_PATH, "%s", szFilename );

			TextureMap.insert( TNameIntPair( std::string(fileData.szFile), addTex ) );
		}

		// always delete the memory
		//tex.Free();

		// texture loaded 0
		return(nTextureID);
	}

	return( renderer::INVALID_OBJECT );
}

/////////////////////////////////////////////////////
/// Function: RemoveTexture
/// Params: 
///
/////////////////////////////////////////////////////
void renderer::RemoveTexture( GLuint texId )
{
	TNameIntMap::iterator it;

	it = TextureMap.begin();
	
	while( it != TextureMap.end() )
	{
		if( it->second.texId == texId )
		{
			it->second.refCount--;

			if( it->second.refCount < 1 )
			{
				if( glIsTexture(it->second.texId) )
					glDeleteTextures( 1, &it->second.texId );
				TextureMap.erase( it );

				return;
			}
			else
				return;
		}

		it++;
	}

	// assume it's a texture used outside the map
	if( glIsTexture(texId) )
		glDeleteTextures( 1, &texId );
}

#endif // BASE_SUPPORT_OPENGL
