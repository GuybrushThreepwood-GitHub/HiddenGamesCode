
/*===================================================================
	File: TextureLoadAndUploadOES.cpp
	Library: Render

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"

#ifdef BASE_SUPPORT_OPENGLES

#include <cstring>
#include <cmath>

#include "Math/Vectors.h"

#include "Render/OpenGLCommon.h"
#include "Render/FF/OpenGLES/OpenGLES.h"
#include "Render/FF/OpenGLES/ExtensionsOES.h"
#include "Render/TextureShared.h"
#include "Render/Texture.h"

#include "Render/FF/OpenGLES/TextureLoadAndUploadOES.h"

// OpenGL ES specific quick texture loader
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
GLuint renderer::TextureLoad( const char *szFilename, Texture& tex, ETextureFormat texFormat, file::TFileHandle* pFile, bool genMipMaps, GLenum magFilter, GLenum minFilter, int texUnit, bool reload, GLuint texObjectId, renderer::OpenGL* openGLContext )
{
	GLuint nTextureID = renderer::INVALID_OBJECT;

	GLenum eTarget = GL_TEXTURE_2D;
	GLenum eMagFilter = magFilter;
	GLenum eMinFilter = minFilter;

	//float fAnisotropyLevel = 1.0f;
	bool bGenMipmaps = genMipMaps;
	//bool bCompress = false;

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
		glTexParameterf( eTarget, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameterf( eTarget, GL_TEXTURE_WRAP_T, GL_REPEAT );
	
		if( (!nIsHeightPOW2 || !nIsWidthPOW2) )
		{
			if( renderer::bExtTextureNonPowerOfTwo )
			{
				glTexParameterf( eTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
				glTexParameterf( eTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
				
				if( eMinFilter == GL_NEAREST_MIPMAP_NEAREST ||
				   eMinFilter == GL_NEAREST_MIPMAP_LINEAR )
					eMinFilter = GL_NEAREST;
				else
				if( eMinFilter == GL_LINEAR_MIPMAP_NEAREST ||
				   eMinFilter == GL_LINEAR_MIPMAP_LINEAR )
					eMinFilter = GL_LINEAR;				
			}
			else 
			{
				if( tex.eFileFormat != TEXTURE_DDS &&
				   tex.eFileFormat != TEXTURE_PVR )
				{
					tex.Flip();
					tex.Resize( false );
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
		}		

		// set correct filter state
		glTexParameterf( eTarget, GL_TEXTURE_MAG_FILTER, eMagFilter );
		glTexParameterf( eTarget, GL_TEXTURE_MIN_FILTER, eMinFilter );

		//if( renderer::bExtAnisotropicFiltering )
		//	glTexParameterf( eTarget, GL_TEXTURE_MAX_ANISOTROPY_EXT, fAnisotropyLevel );

		if( tex.eFileFormat == TEXTURE_PVR && renderer::bExtTextureCompression && renderer::bExtTextureCompressionPVRtc )
		{
			int w = tex.nWidth;
			int h = tex.nHeight;

			// turn on auto mipmap if there aren't any in the file
			if( bGenMipmaps && tex.nMipMapCount <= 1 )
			{
				// hardware mipmap creation
				glTexParameteri( eTarget, GL_GENERATE_MIPMAP, GL_TRUE );
			}

			unsigned int dataSize = 0;
			unsigned int blockSize = 0, widthBlocks = 0, heightBlocks = 0;

			for( int level = 0; level < tex.nMipMapCount; level++ ) 
			{
				if (tex.nInternalFormat == GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG ||
					tex.nInternalFormat == GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG )
				{
					blockSize = 8 * 4; // Pixel by pixel block size for 2bpp
					widthBlocks = w / 8;
					heightBlocks = h / 4;
				}
				else 
				if (tex.nInternalFormat == GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG ||
					tex.nInternalFormat == GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG )
				{
					blockSize = 4 * 4; // Pixel by pixel block size for 4bpp
					widthBlocks = w / 4;
					heightBlocks = h / 4;
				}
				
				// Clamp to minimum number of blocks
				if (widthBlocks < 2)
					widthBlocks = 2;
				if (heightBlocks < 2)
					heightBlocks = 2;

				dataSize = widthBlocks * heightBlocks * ((blockSize  * tex.nBPP) / 8);

				// no mipmaps in the PVR, but are needed
				if( bGenMipmaps && tex.nMipMapCount == 1 )
				{
					// hardware mipmap creation
					glTexParameteri( eTarget, GL_GENERATE_MIPMAP, GL_TRUE );
				}

				// upload
				if( tex.bCompressed )
					glCompressedTexImage2D( eTarget, level, tex.nInternalFormat, w, h, 0, dataSize, tex.imageList[level] );

				// next mip
				w = ( w > 1 ) ? w >> 1 : 1;
				h = ( h > 1 ) ? h >> 1 : 1;
			}
		}
		else
		{
			if( bGenMipmaps )
				glTexParameteri( eTarget, GL_GENERATE_MIPMAP, GL_TRUE );

			if( eTarget == GL_TEXTURE_2D )
			{
				glTexImage2D( eTarget, 0, tex.nInternalFormat, tex.nWidth, tex.nHeight, 0, tex.nFormat,
										tex.nTextureType, tex.image );
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
		tex.Free();

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

				it = TextureMap.begin();

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

#endif // BASE_SUPPORT_OPENGLES
