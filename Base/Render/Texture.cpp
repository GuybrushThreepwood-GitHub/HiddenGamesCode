
/*===================================================================
	File: Texture.cpp
	Library: Render

	(C)Hidden Games
=====================================================================*/


#include "CoreBase.h"

#include <cmath>

#ifdef BASE_SUPPORT_JPEG
	#include "setjmp.h"
	#include "jpeglib.h"
#endif // BASE_SUPPORT_JPEG

#ifdef BASE_SUPPORT_PNG
	#include "zlib.h"
	#include "png.h"
#endif // BASE_SUPPORT_PNG

#ifdef BASE_SUPPORT_PVR
/*	#include "PVRTexLib.h"
	#include "PVRTexLibGlobals.h"
	#include "PVRTGlobal.h"
	#include "PVRException.h"
	#include "Pixel.h"
	#include "CPVRTextureHeader.h"
	#include "CPVRTextureData.h"
	#include "CPVRTexture.h"
*/
#endif // BASE_SUPPORT_PVR

#include "Math/Vectors.h"

#include "Collision/AABB.h"

#include "Render/OpenGLCommon.h"
#include "Render/FF/OpenGL/OpenGL.h"
#include "Render/FF/OpenGL/Extensions.h"
#include "Render/FF/OpenGLES/OpenGLES.h"
#include "Render/FF/OpenGLES/ExtensionsOES.h"
#include "Render/TextureShared.h"
#include "Render/Texture.h"

using renderer::Texture;

#ifdef BASE_SUPPORT_PNG
    void png_zip_read(png_structp png_ptr, png_bytep data, png_uint_32 length );

	void png_zip_read(png_structp png_ptr, png_bytep data, png_uint_32 length ) 
	{
		ZZIP_FILE *zfp = reinterpret_cast<ZZIP_FILE *>(png_get_io_ptr(png_ptr));

		zzip_read(zfp, data, length);
	}
#endif // BASE_SUPPORT_PNG

/////////////////////////////////////////////////////
/// Default constructor
/// 
///
/////////////////////////////////////////////////////
Texture::Texture()
{
	nFileSize = 0;
	eFileFormat = TEXTURE_UNKNOWN;
	std::memset( &TextureFile, 0, sizeof(file::TFile) );
	nID = INVALID_OBJECT;
	nTextureSize = 0;
	nCompressedTextureSize = 0;
	nWidth = 0;
	nHeight = 0;
	bResized = false;
	nOriginalWidth = 0;
	nOriginalHeight = 0;
	nBPP = 0;
	nNumChannels = 0;
	bHasAlpha = false;
	bCompressed = false;
	bPaletted = false;
	bDXTCompressed = false;
	nDepth = 0;
	bIsVolume = false;
	bIsCubemap = false;
	nCubemapFaceCount = 0;
	bHasMipMaps = false;
	nMipMapCount = 0;
	nBytesPerElement = 0;
	nInternalFormat = GL_RGB;
	nFormat = GL_RGB;
	nTextureType = GL_UNSIGNED_BYTE;
	image = 0;
	imageList = 0;
}

/////////////////////////////////////////////////////
/// Default destructor
/// 
///
/////////////////////////////////////////////////////
Texture::~Texture()
{
	int i = 0;

	int nMipCount = 0;

	if( bIsCubemap )
		nMipCount = nMipMapCount*nCubemapFaceCount;
	else
		nMipCount = nMipMapCount;

	if( imageList )
	{
		for( i = 0; i < nMipCount; i++ )
		{
			if( imageList[i] )
			{
				delete[] imageList[i];
				imageList[i] = 0;
			}
		}

		// clear the list
		delete[] imageList;
		imageList = 0;
	}

	if( image )
	{
		delete[] image;
		image = 0;
	}
}

/////////////////////////////////////////////////////
/// Operator: ASSIGN
/// Params: [in]t
///
/////////////////////////////////////////////////////
Texture &Texture::operator = (const Texture &t)
{
	nFileSize			= 0;
	eFileFormat			= TEXTURE_UNKNOWN;
	TextureFile			= t.TextureFile;
	nID					= t.nID;
	nTextureSize		= t.nTextureSize;
	nCompressedTextureSize = t.nCompressedTextureSize;
	nWidth				= t.nWidth;
	nHeight				= t.nHeight;
	bResized			= t.bResized;
	nOriginalWidth		= t.nOriginalWidth;
	nOriginalHeight		= t.nOriginalHeight;
	nBPP				= t.nBPP;
	nNumChannels		= t.nNumChannels;
	bHasAlpha			= t.bHasAlpha;
	bCompressed			= t.bCompressed;
	bPaletted			= t.bPaletted;
	bDXTCompressed		= t.bDXTCompressed;
	nDepth				= t.nDepth;
	bIsVolume			= t.bIsVolume;
	bIsCubemap			= t.bIsCubemap;
	nCubemapFaceCount	= t.nCubemapFaceCount;
	bHasMipMaps			= t.bHasMipMaps;
	nMipMapCount		= t.nMipMapCount;
	nBytesPerElement	= t.nBytesPerElement;
	nInternalFormat		= t.nInternalFormat;
	nFormat				= t.nFormat;
	nTextureType		= t.nTextureType;

	image				= t.image;

	if( t.imageList )
	{
		if( t.bIsCubemap )
			imageList = new unsigned char*[t.nMipMapCount*t.nCubemapFaceCount];
		else
			imageList = new unsigned char*[t.nMipMapCount];

		for( int face = 0; face < ((t.nCubemapFaceCount) ? t.nCubemapFaceCount : 1); face++ ) 
		{
			int w = t.nWidth;
			int h = t.nHeight;
			int d = (t.nDepth) ? t.nDepth : 1;
				
			for( int level = 0; level < t.nMipMapCount; level++ ) 
			{
				int bw = (t.bDXTCompressed) ? (w+3)/4 : w;
				int bh = (t.bDXTCompressed) ? (h+3)/4 : h;
				int size = bw*bh*d*t.nBytesPerElement;

				imageList[ level + (face*t.nMipMapCount) ] = new unsigned char[size];

				std::memcpy( imageList[ level + (face*t.nMipMapCount) ], t.imageList[ level + (face*t.nMipMapCount) ], size );

				// reduce mip sizes
				w = ( w > 1 ) ? w >> 1 : 1;
				h = ( h > 1 ) ? h >> 1 : 1;
				d = ( d > 1 ) ? d >> 1 : 1;
			}
		}
	}

	return(*this);
}

/////////////////////////////////////////////////////
/// Method: Load
/// Params: [in]szFilename
///
/////////////////////////////////////////////////////
int Texture::Load( const char *szFilename, renderer::ETextureFormat texFormat, file::TFileHandle* pFile )
{
	char extStr[core::MAX_EXT];
	std::memset( extStr, 0, sizeof( char )*core::MAX_EXT );

	// get extension from filename
	//extStr = strchr( szFilename, '.' );
	core::SplitPath( szFilename, 0, 0, 0, extStr );

	nInternalFormat = GL_RGB; 
	nFormat = GL_RGB;

	// set the texture type based on extension of filename

	if( !core::IsEmptyString( szFilename ) )
	{
		if( ( std::strcmp( extStr, ".TGA" ) == 0 ) || ( std::strcmp( extStr, ".tga" ) == 0 ) )
		{
			return( LoadTGA( szFilename ) );
		}
#ifdef BASE_SUPPORT_BMP
		else if( ( std::strcmp( extStr, ".BMP" ) == 0 ) || ( std::strcmp( extStr, ".bmp" ) == 0 ) )
		{
			return( LoadBMP( szFilename ) );
		}
#endif // BASE_SUPPORT_BMP
#ifdef BASE_SUPPORT_PNG
		else if( ( std::strcmp( extStr, ".PNG" ) == 0 ) || ( std::strcmp( extStr, ".png" ) == 0 ) )
		{
			return( LoadPNG( szFilename ) );
		}
#endif // BASE_SUPPORT_PNG
#ifdef BASE_SUPPORT_DDS
		else if( ( std::strcmp( extStr, ".DDS" ) == 0 ) || ( std::strcmp( extStr, ".dds" ) == 0 ) )
		{
			return( LoadDDS( szFilename ) );
		}
#endif // BASE_SUPPORT_DDS
#ifdef BASE_SUPPORT_JPEG
		else if( ( std::strcmp( extStr, ".JPEG" ) == 0 ) || ( std::strcmp( extStr, ".jpeg" ) == 0 ) || ( std::strcmp( extStr, ".JPG" ) == 0 ) || ( std::strcmp( extStr, ".jpg" ) == 0 ) )
		{
			return( LoadJPEG( szFilename ) );
		}
#endif // BASE_SUPPORT_JPEG
#ifdef BASE_SUPPORT_PCX
		else if( ( std::strcmp( extStr, ".PCX" ) == 0 ) || ( std::strcmp( extStr, ".pcx" ) == 0 ) )
		{
			return( LoadPCX( szFilename ) );
		}
#endif // BASE_SUPPORT_PCX
#ifdef BASE_SUPPORT_PVR
		else if( ( std::strcmp( extStr, ".PVR" ) == 0 ) || ( std::strcmp( extStr, ".pvr" ) == 0 ) )
		{
			return( LoadPVR( szFilename ) );
		}
#endif // BASE_SUPPORT_PVR
	}
	else
	{
		if( texFormat == TEXTURE_TGA && pFile )
		{
			return( LoadTGA( szFilename, pFile ) );
		}
#ifdef BASE_SUPPORT_BMP
		else if( texFormat == TEXTURE_BMP && pFile )
		{
			return( LoadBMP( szFilename, pFile ) );
		}
#endif // BASE_SUPPORT_BMP
#ifdef BASE_SUPPORT_PNG
		else if( texFormat == TEXTURE_PNG && pFile )
		{
			return( LoadPNG( szFilename, pFile ) );
		}
#endif // BASE_SUPPORT_PNG
#ifdef BASE_SUPPORT_DDS
		else if( texFormat == TEXTURE_DDS && pFile )
		{
			return( LoadDDS( szFilename, pFile ) );
		}
#endif // BASE_SUPPORT_DDS
#ifdef BASE_SUPPORT_JPEG
		else if( texFormat == TEXTURE_JPEG && pFile )
		{
			return( LoadJPEG( szFilename, pFile ) );
		}
#endif // BASE_SUPPORT_JPEG
#ifdef BASE_SUPPORT_PCX
		else if( texFormat == TEXTURE_PCX && pFile )
		{
			return( LoadPCX( szFilename, pFile ) );
		}
#endif // BASE_SUPPORT_PCX
#ifdef BASE_SUPPORT_PVR
		else if( texFormat == TEXTURE_PVR && pFile )
		{
			return( LoadPVR( szFilename, pFile ) );
		}
#endif // BASE_SUPPORT_PVR
	}

	DBGLOG( "TEXTURE: *ERROR* file '%s' is an unsupported file format\n", szFilename );
	return(1);
}

#ifdef BASE_SUPPORT_BMP
/////////////////////////////////////////////////////
/// Method: LoadBMP
/// Params: [in]szFilename
///
/////////////////////////////////////////////////////
int Texture::LoadBMP( const char *szFilename, file::TFileHandle* pFile )
{
	file::TFileHandle* fileHandle = 0;
	int i = 0, j = 0;
	TBMPFileHeader bmfh;
	TBMPInfoHeader bmih;
	TBMPCoreHeader bmch;
	EBMPOSType os_type;
	unsigned int nCompression;
	unsigned int nBitCount;
	fpos_t bmhPos;
	unsigned char *pColourmap = 0;
	int nColourmapSize;

	// clear any old data
	this->Free();

	if( pFile == 0 )
	{
		fileHandle = new file::TFileHandle;

		if( core::IsEmptyString( szFilename ) )
		{
			delete fileHandle;
			return(1);
		}

		if( !file::FileOpen( szFilename, file::FILETYPE_BINARY_READ, fileHandle ) )
		{
			DBGLOG( "TEXTURE: *ERROR* Failed loading bmp: %s\n", szFilename );
			delete fileHandle;
			return(1);
		}

		// find length of file
		nFileSize = file::FileSize( fileHandle );

		file::CreateFileStructure( szFilename, &TextureFile );
	}
	else
	{
		fileHandle = pFile;
	}

	// read bitmap file header
	std::memset( &bmfh, 0, sizeof(TBMPFileHeader) );

	file::FileRead( &bmfh, sizeof (TBMPFileHeader), fileHandle );
	bmhPos = file::FilePosition( fileHandle );

	//unsigned char bfType[2];      // magic number "BM" (0x4D42)
	bmfh.bfSize = core::LittleToBigEndianInt( bmfh.bfSize );
	bmfh.bfReserved1 = core::LittleToBigEndianShort( bmfh.bfReserved1 );
	bmfh.bfReserved2 = core::LittleToBigEndianShort( bmfh.bfReserved2 );
	bmfh.bfOffBits = core::LittleToBigEndianInt( bmfh.bfOffBits );

	if( std::strncmp( reinterpret_cast<char *>(bmfh.bfType), "BM", 2 ) != 0 )
	{
		DBGLOG( "TEXTURE: *ERROR* %s is not a valid BMP file!\n", szFilename );
	
		// close the file
		if( pFile == 0 )
		{
			file::FileClose( fileHandle );
			delete fileHandle;
		}

		return( 1 );
	}

	nFormat = GL_RGB;
	nInternalFormat = GL_RGB8;
	nTextureType = GL_UNSIGNED_BYTE;

	// read bitmap info header
	std::memset( &bmih, 0, sizeof(TBMPInfoHeader) );
	file::FileRead( &bmih, sizeof(TBMPInfoHeader), fileHandle );

	bmih.biSize = static_cast<unsigned int>(core::LittleToBigEndianInt( bmih.biSize ));          
	bmih.biWidth = static_cast<int>(core::LittleToBigEndianInt( bmih.biWidth ));                 
	bmih.biHeight = static_cast<int>(core::LittleToBigEndianInt( bmih.biHeight ));                
	bmih.biPlanes = static_cast<unsigned short>(core::LittleToBigEndianShort( bmih.biPlanes ));      
	bmih.biBitCount = static_cast<unsigned short>(core::LittleToBigEndianShort( bmih.biBitCount ));   
	bmih.biCompression = static_cast<unsigned int>(core::LittleToBigEndianInt( bmih.biCompression ));  
	bmih.biSizeImage = static_cast<unsigned int>(core::LittleToBigEndianInt( bmih.biSizeImage ));     
	bmih.biXPelsPerMeter = static_cast<int>(core::LittleToBigEndianInt( bmih.biXPelsPerMeter ));          
	bmih.biYPelsPerMeter = static_cast<int>(core::LittleToBigEndianInt( bmih.biYPelsPerMeter ));          
	bmih.biClrUsed = static_cast<unsigned int>(core::LittleToBigEndianInt( bmih.biClrUsed ));       
	bmih.biClrImportant = static_cast<unsigned int>(core::LittleToBigEndianInt( bmih.biClrImportant ));  

	if( bmih.biCompression > 3 )
	{
		// this is an OS/2 bitmap file, we don't use
		// bitmap info header but bitmap core header instead

		// we must go back to read bitmap core header
		fsetpos( fileHandle->fp, &bmhPos );
		std::memset( &bmch, 0, sizeof(TBMPCoreHeader) );

		file::FileRead( &bmch, sizeof(TBMPCoreHeader), fileHandle );

		bmch.bcSize = static_cast<unsigned int>(core::LittleToBigEndianInt( bmch.bcSize ));          
		bmch.bcWidth = static_cast<unsigned short>(core::LittleToBigEndianShort( bmch.bcWidth ));        
		bmch.bcHeight = static_cast<unsigned short>(core::LittleToBigEndianShort( bmch.bcHeight ));       
		bmch.bcPlanes = static_cast<unsigned short>(core::LittleToBigEndianShort( bmch.bcPlanes ));   
		bmch.bcBitCount = static_cast<unsigned short>(core::LittleToBigEndianShort( bmch.bcBitCount )); 

		os_type = BMP_OS2;
		nCompression = BI_RGB;
		nBitCount = bmch.bcBitCount;

		nWidth = bmch.bcWidth;
		nHeight = bmch.bcHeight;
	}
	else
	{
		// Windows style
		os_type = BMP_WIN;
		nCompression = bmih.biCompression;
		nBitCount = bmih.biBitCount;

		nWidth = bmih.biWidth;
		nHeight = bmih.biHeight;
	}

	// look for palette data if present
	if( nBitCount <= 8 )
	{
		nColourmapSize = (1 << nBitCount) * ((os_type == BMP_OS2) ? 3 : 4);
		pColourmap = new unsigned char[nColourmapSize];

		file::FileRead( pColourmap, sizeof(unsigned char), nColourmapSize, fileHandle );
	}

	eFileFormat		= TEXTURE_BMP;		
	nNumChannels	= 3;
	nBPP			= nNumChannels*8;										
	nOriginalWidth	= nWidth;
	nOriginalHeight	= nHeight;
	bHasAlpha		= false;
	nMipMapCount	= 1;
	bHasMipMaps		= false;
	bIsCubemap		= false;
	bIsVolume		= false;
	
	if( (nWidth <= 0) || (nHeight <= 0) )	
	{
		DBGLOG( "TEXTURE: *ERROR* invalid bmp information\n" );	

		if( pFile == 0 )
		{
			file::FileClose( fileHandle );
			delete fileHandle;
		}											

		return(1);													
	}

	// memory allocation for pixel data
	nTextureSize	= (nWidth * nHeight) * nNumChannels;
	image = new unsigned char[nTextureSize];

	// go to begining of pixel data
	file::FileSeek( bmfh.bfOffBits, file::FILESEEK_BEG, fileHandle );

	// read image data
	switch( nCompression )
	{
		case BI_RGB:
		{
			bCompressed = false;

			switch( nBitCount )
			{
				case 1:
				{
					int cmPixSize;
					unsigned char colour, clrIndex;

					cmPixSize = (os_type == BMP_OS2) ? 3 : 4;

					for( i = 0; i < (nWidth * nHeight); )
					{
						// read index colour byte
						colour = static_cast<unsigned char>(file::FileGetC( fileHandle ));

						// convert 8-by-8 pixels to RGB 24 bits
						for( j = 7; j >= 0; --j, ++i )
						{
							clrIndex = ((colour & (1 << j)) > 0);
							image[(i * 3) + 2] = pColourmap[(clrIndex * cmPixSize) + 0];
							image[(i * 3) + 1] = pColourmap[(clrIndex * cmPixSize) + 1];
							image[(i * 3) + 0] = pColourmap[(clrIndex * cmPixSize) + 2];
						}
					}
				}break;

				case 4:
				{
					int cmPixSize;
					unsigned char colour, clrIndex;

					cmPixSize = (os_type == BMP_OS2) ? 3 : 4;

					for( i = 0; i < (nWidth * nHeight); i += 2 )
					{
						// read index colour byte
						colour = static_cast<unsigned char>(file::FileGetC( fileHandle ));

						// convert 2-by-2 pixels to RGB 24 bits

						// first pixel
						clrIndex = (colour >> 4);
						image[(i * 3) + 2] = pColourmap[(clrIndex * cmPixSize) + 0];
						image[(i * 3) + 1] = pColourmap[(clrIndex * cmPixSize) + 1];
						image[(i * 3) + 0] = pColourmap[(clrIndex * cmPixSize) + 2];

						// second pixel
						clrIndex = (colour & 0x0F);
						image[(i * 3) + 5] = pColourmap[(clrIndex * cmPixSize) + 0];
						image[(i * 3) + 4] = pColourmap[(clrIndex * cmPixSize) + 1];
						image[(i * 3) + 3] = pColourmap[(clrIndex * cmPixSize) + 2];
					}
				}break;

				case 8:
				{
					int cmPixSize;
					unsigned char colour;

					cmPixSize = (os_type == BMP_OS2) ? 3 : 4;

					for( i = 0; i < (nWidth * nHeight); ++i )
					{
						// read index colour byte
						colour = static_cast<unsigned char>(file::FileGetC( fileHandle ));

						// convert to RGB 24 bits
						image[(i * 3) + 2] = pColourmap[(colour * cmPixSize) + 0];
						image[(i * 3) + 1] = pColourmap[(colour * cmPixSize) + 1];
						image[(i * 3) + 0] = pColourmap[(colour * cmPixSize) + 2];
					}
				}break;

				case 24:
				{
					for( i = 0; i < (nWidth * nHeight); ++i )
					{
						// read and convert BGR to RGB
						image[(i * 3) + 2] = static_cast<unsigned char>(file::FileGetC( fileHandle ));
						image[(i * 3) + 1] = static_cast<unsigned char>(file::FileGetC( fileHandle ));
						image[(i * 3) + 0] = static_cast<unsigned char>(file::FileGetC( fileHandle ));
					}
				}break;

				case 32:
				{
					unsigned char skip;

					for( i = 0; i < (nWidth * nHeight); ++i )
					{
						// read and convert BGRA to RGB
						image[(i * 3) + 2] = static_cast<unsigned char>(file::FileGetC( fileHandle ));
						image[(i * 3) + 1] = static_cast<unsigned char>(file::FileGetC( fileHandle ));
						image[(i * 3) + 0] = static_cast<unsigned char>(file::FileGetC( fileHandle ));

						// skip last byte
						skip = static_cast<unsigned char>(file::FileGetC( fileHandle ));
					}
				}break;
			}
		}break;

		case BI_RLE8:
		{
			unsigned char colour, skip;
			unsigned char byte1, byte2;
			unsigned char *ptr = image;

			bCompressed = true;

			while( ptr < image + (nWidth * nHeight) * 3 )
			{
				// read first two byte
				byte1 = static_cast<unsigned char>(file::FileGetC( fileHandle ));
				byte2 = static_cast<unsigned char>(file::FileGetC( fileHandle ));

				if( byte1 == RLE_COMMAND )
				{
					for( i = 0; i < byte2; ++i, ptr += 3 )
					{
						colour = (unsigned char)file::FileGetC( fileHandle );

						ptr[0] = pColourmap[(colour * 4) + 2];
						ptr[1] = pColourmap[(colour * 4) + 1];
						ptr[2] = pColourmap[(colour * 4) + 0];
					}

					if( byte2 % 2 )
					{
						// skip one byte if number of pixels is odd
						skip = static_cast<unsigned char>(file::FileGetC( fileHandle ));
					}
				}
				else
				{
					for( i = 0; i < byte1; ++i, ptr += 3 )
					{
						ptr[0] = pColourmap[(byte2 * 4) + 2];
						ptr[1] = pColourmap[(byte2 * 4) + 1];
						ptr[2] = pColourmap[(byte2 * 4) + 0];
					}
				}
			}
		}break;

		case BI_RLE4:
		{
			int bytesRead = 0;
			unsigned char colour, databyte, skip;
			unsigned char byte1, byte2;
			unsigned char *ptr = image;

			bCompressed = true;

			while( ptr < image + (nWidth * nHeight) * 3 )
			{
				// read first two byte
				byte1 = static_cast<unsigned char>(file::FileGetC( fileHandle ));
				byte2 = static_cast<unsigned char>(file::FileGetC( fileHandle ));
				bytesRead += 2;

				if( byte1 == RLE_COMMAND )
				{
					databyte = 0;

					for( i = 0; i < byte2; ++i, ptr += 3 )
					{
						if (i % 2)
						{
							// four less significant bits
							colour = (databyte & 0x0F);
						}
						else
						{
							databyte = static_cast<unsigned char>(file::FileGetC( fileHandle ));
							++bytesRead;

							// four most significant bits
							colour = (databyte >> 4);
						}

						// convert from index colour to RGB 24 bits
						ptr[0] = pColourmap[(colour * 4) + 2];
						ptr[1] = pColourmap[(colour * 4) + 1];
						ptr[2] = pColourmap[(colour * 4) + 0];
					}

					if( bytesRead % 2 )
					{
						// skip one byte if number of read bytes is odd
						skip = static_cast<unsigned char>(file::FileGetC( fileHandle ));
						++bytesRead;
					}
				}
				else
				{
					for( i = 0; i < byte1; ++i, ptr += 3 )
					{
						if(i % 2)
							colour = (byte2 & 0x0F);
						else
							colour = (byte2 >> 4);

						// convert from index colour to RGB 24 bits
						ptr[0] = pColourmap[(colour * 4) + 2];
						ptr[1] = pColourmap[(colour * 4) + 1];
						ptr[2] = pColourmap[(colour * 4) + 0];
					}
				}
			}
		}break;

		case BI_BITFIELDS:
		default:
		{
			// image type is not correct
			DBGLOG( "TEXTURE: *ERROR* unknown BMP image type\n" );

			if( image )
			{
				delete[] image;
				image = 0;
			}

			if( pColourmap )
			{
				delete[] pColourmap;
				pColourmap = 0;
			}
			
			// close the file
			if( pFile == 0 )
			{
				file::FileClose( fileHandle );
				delete fileHandle;
			}

			return(1);
		}break;
	}

	// no longer need colourmap data
	if( pColourmap )
	{
		delete[] pColourmap;
		pColourmap = 0;
	}

	// close the file
	if( pFile == 0 )
	{
		file::FileClose( fileHandle );
		delete fileHandle;
	}

	//DBGLOG( "TEXTURE: IMAGE %s Loaded\t\t(w:%d, h:%d, bpp:%d, ch:%d, file_size:%d, image_size:%d, alpha:%s, compressed:%s)\n", 
	//	szFilename, nWidth, nHeight, nBPP, nNumChannels, nFileSize, 
	//	nTextureSize, bHasAlpha?"YES":"NO", bCompressed?"YES":"NO" );

	return(0);
}

/////////////////////////////////////////////////////
/// Function: SaveBMP
/// Params: [in]szFilename
///
/////////////////////////////////////////////////////
int Texture::SaveBMP( const char *szFilename, file::TFileHandle* pFile )
{
	file::TFileHandle* fileHandle = 0;
	TBMPFileHeader	bitmapFileHeader;
	TBMPInfoHeader	bitmapInfoHeader;	
	unsigned int i;			
	unsigned char tempRGB;			


	// FIXME: add support for at least greyscale and maybe index in the end
	if( nNumChannels < 3 )
	{
		DBGLOG( "TEXTURE: *ERROR* cannot currently save greyscale or index bmp textures\n" );
		return(1);
	}

	if( pFile == 0 )
	{
		fileHandle = new file::TFileHandle;

		if( core::IsEmptyString( szFilename ) )
		{
			delete fileHandle;
			return(1);
		}
		if( !file::FileOpen( szFilename, file::FILETYPE_BINARY_WRITE, fileHandle ) )
		{
			DBGLOG( "TEXTURE: *ERROR* Failed saving bmp: %s\n", szFilename );
			delete fileHandle;
			return(1);
		}
	}
	else
	{
		fileHandle = pFile;
		
		// get the start point of this save file
		fileHandle->subFile.nFileStartPos = static_cast<long>(file::FilePosition(fileHandle));
	}

	std::memset( &bitmapFileHeader, 0, sizeof(TBMPFileHeader) );
	std::memset( &bitmapInfoHeader, 0, sizeof(TBMPInfoHeader) );

	// define the bitmap file header
	bitmapFileHeader.bfSize				= core::EndianSwapInt( sizeof(TBMPFileHeader), core::MACHINE_LITTLE_ENDIAN );
	std::sprintf( reinterpret_cast<char *>(bitmapFileHeader.bfType), "BM" );
	bitmapFileHeader.bfReserved1		= core::EndianSwapShort( 0, core::MACHINE_LITTLE_ENDIAN );
	bitmapFileHeader.bfReserved2		= core::EndianSwapShort( 0, core::MACHINE_LITTLE_ENDIAN );
	bitmapFileHeader.bfOffBits			= core::EndianSwapInt( sizeof(TBMPFileHeader) + sizeof(TBMPInfoHeader), core::MACHINE_LITTLE_ENDIAN );
	
	// define the bitmap information header
	bitmapInfoHeader.biSize				= core::EndianSwapInt( sizeof(TBMPInfoHeader), core::MACHINE_LITTLE_ENDIAN );
	bitmapInfoHeader.biPlanes			= core::EndianSwapShort( 1, core::MACHINE_LITTLE_ENDIAN );
	bitmapInfoHeader.biBitCount			= core::EndianSwapShort( nBPP, core::MACHINE_LITTLE_ENDIAN );
	bitmapInfoHeader.biCompression		= core::EndianSwapInt( BI_RGB, core::MACHINE_LITTLE_ENDIAN );				
	bitmapInfoHeader.biSizeImage		= core::EndianSwapInt( nTextureSize, core::MACHINE_LITTLE_ENDIAN );
	bitmapInfoHeader.biXPelsPerMeter	= core::EndianSwapInt( 0, core::MACHINE_LITTLE_ENDIAN );
	bitmapInfoHeader.biYPelsPerMeter	= core::EndianSwapInt( 0, core::MACHINE_LITTLE_ENDIAN );
	bitmapInfoHeader.biClrUsed			= core::EndianSwapInt( 0, core::MACHINE_LITTLE_ENDIAN );
	bitmapInfoHeader.biClrImportant		= core::EndianSwapInt( 0, core::MACHINE_LITTLE_ENDIAN );
	bitmapInfoHeader.biWidth			= core::EndianSwapInt( nWidth, core::MACHINE_LITTLE_ENDIAN );
	bitmapInfoHeader.biHeight			= core::EndianSwapInt( nHeight, core::MACHINE_LITTLE_ENDIAN );

	// switch the image data from RGB to BGR
	if( nNumChannels == 3 || nNumChannels == 4 )
	{
		for( i = 0; i < nTextureSize;  )
		{
			tempRGB = image[i];
			image[i] = image[i + 2];
			image[i + 2] = tempRGB;
				
			i += nNumChannels;
		}
	}

	// write the bitmap file header
	file::FileWrite( &bitmapFileHeader, sizeof(TBMPFileHeader), fileHandle );

	// write the bitmap info header
	file::FileWrite( &bitmapInfoHeader, sizeof(TBMPInfoHeader), fileHandle );

	// write the image data
	file::FileWrite( image, nTextureSize, fileHandle );
	
	if( pFile == 0 )
	{
		file::FileClose( fileHandle );
		delete fileHandle;
	}
	else
	{
		// get the end point of this save file
		fileHandle->subFile.nFileEndPos = static_cast<long>(file::FilePosition(fileHandle));

		fileHandle->subFile.nFileLength = fileHandle->subFile.nFileEndPos - fileHandle->subFile.nFileStartPos;
	}

	DBGLOG( "TEXTURE: IMAGE %s Saved\t\t(w:%d, h:%d, alpha:%s)\n", szFilename, nWidth, nHeight,
			bHasAlpha?"YES":"NO" );

	return(0);
}

#endif // BASE_SUPPORT_BMP

/////////////////////////////////////////////////////
/// Method: LoadTGA
/// Params: [in]szFilename
///
/////////////////////////////////////////////////////
int Texture::LoadTGA( const char *szFilename, file::TFileHandle* pFile )
{
	int i = 0;
	file::TFileHandle* fileHandle = 0;
	TTGAHeader TGAHeader;
	unsigned char *pColourmap = 0;

	// clear any old data
	this->Free();

	if( pFile == 0 )
	{
		fileHandle = new file::TFileHandle;

		if( core::IsEmptyString( szFilename ) )
		{
			delete fileHandle;
			return(1);
		}
		if( !file::FileOpen( szFilename, file::FILETYPE_BINARY_READ, fileHandle ) )
		{
			DBGLOG( "TEXTURE: *ERROR* failed loading tga: %s\n", szFilename );
			delete fileHandle;
			return(1);
		}

		// find length of file
		nFileSize = file::FileSize( fileHandle );

		file::CreateFileStructure( szFilename, &TextureFile );
	}
	else
	{
		fileHandle = pFile;
	}

	// clear and read the header
	std::memset( &TGAHeader, 0, sizeof(TTGAHeader) );
	if( file::FileRead( &TGAHeader, sizeof(TTGAHeader), fileHandle ) == 0 )				
	{
		DBGLOG( "TEXTURE: *ERROR* could not read tga file header\n" );	
		if( pFile == 0 )
		{
			file::FileClose( fileHandle );
			delete fileHandle;
		}											
		return(1);												
	}

	//unsigned char	IDLength;			
	//unsigned char	ColourmapType;		
	//unsigned char	ImageType;			

	TGAHeader.Cm_first_entry = static_cast<short>(core::LittleToBigEndianShort( TGAHeader.Cm_first_entry ));		
	TGAHeader.Cm_length = static_cast<short>(core::LittleToBigEndianShort( TGAHeader.Cm_length ));			
	//unsigned char	Cm_size;			

	TGAHeader.nXOrigin = static_cast<short>(core::LittleToBigEndianShort( TGAHeader.nXOrigin ));			
	TGAHeader.nYOrigin = static_cast<short>(core::LittleToBigEndianShort( TGAHeader.nYOrigin ));			

	TGAHeader.nWidth = static_cast<short>(core::LittleToBigEndianShort( TGAHeader.nWidth ));				
	TGAHeader.nHeight = static_cast<short>(core::LittleToBigEndianShort( TGAHeader.nHeight ));			

	//unsigned char  PixelDepth;			
	//unsigned char  ImageDescriptor;
		
	switch( TGAHeader.ImageType )
    {
		case 3:  // grayscale 8 bits
		case 11: // grayscale 8 bits (RLE)
		{
			if( TGAHeader.PixelDepth == 8 )
			{
				nFormat = GL_LUMINANCE;
				nInternalFormat = GL_LUMINANCE;
				nNumChannels = 1;
				nTextureType = GL_UNSIGNED_BYTE;
				//nInternalFormat = 1;

				bHasAlpha = false;
			}
			else // 16 bits
			{
				nFormat = GL_LUMINANCE_ALPHA;
				nInternalFormat = GL_LUMINANCE_ALPHA;
				nNumChannels = 2;
				nTextureType = GL_UNSIGNED_BYTE;
				//nInternalFormat = 2;

				bHasAlpha = true;
			}

		}break;

		case 1:  // 8 bits colour index 
		case 2:  // BGR 16-24-32 bits 
		case 9:  // 8 bits colour index (RLE) 
		case 10: // BGR 16-24-32 bits (RLE) 
		{
			// 8 bits and 16 bits images will be converted to 24 bits 
			if( TGAHeader.PixelDepth <= 24 )
			{
				nFormat = GL_RGB;
				nInternalFormat = GL_RGB;
				nNumChannels = 3;
				nTextureType = GL_UNSIGNED_BYTE;
				//nInternalFormat = 3;

				bHasAlpha = false;
			}
			else // 32 bits 
			{
				nFormat = GL_RGBA;
				nInternalFormat = GL_RGBA;
				nNumChannels = 4;
				nTextureType = GL_UNSIGNED_BYTE;
				//nInternalFormat = 4;

				bHasAlpha = true;
			}	

		}break;
    }
	
	// move to the image data
	file::FileSeek( TGAHeader.IDLength, file::FILESEEK_CUR, fileHandle );

	eFileFormat	= TEXTURE_TGA;
	nWidth		= TGAHeader.nWidth;			
	nHeight		= TGAHeader.nHeight;			
	nBPP		= nNumChannels*8;										
	nOriginalWidth = nWidth;
	nOriginalHeight = nHeight;
	nMipMapCount	= 1;
	bHasMipMaps		= false;
	bIsCubemap		= false;
	bIsVolume		= false;

	if( (nWidth <= 0) || (nHeight <= 0) )	
	{
		DBGLOG( "TEXTURE: *ERROR* invalid tga information\n" );
		if( pFile == 0 )
		{
			file::FileClose( fileHandle );
			delete fileHandle;
		}												
		return(1);													
	}

	// allocate memory							
	nTextureSize	= (nWidth * nHeight) * nNumChannels;	
	image			= new unsigned char[nTextureSize];

	if( TGAHeader.ColourmapType )
    {
		// NOTE: colour map is stored in BGR format
		pColourmap = new unsigned char[ TGAHeader.Cm_length * (TGAHeader.Cm_size >> 3) ];
		file::FileRead( pColourmap, sizeof(unsigned char), TGAHeader.Cm_length * (TGAHeader.Cm_size >> 3), fileHandle );
    }

	std::memset( image, 0, sizeof(unsigned char)*nTextureSize );

	// read image data
	switch( TGAHeader.ImageType )
	{
		case 0:
		{
			// no data
		}break;

		case 1:
		{
			bCompressed = false;

			// uncompressed 8 bits colour index
			unsigned char colour;

			for( i = 0; i < (nWidth * nHeight); ++i )
			{
				// read index colour byte
				colour = static_cast<unsigned char>(file::FileGetC(fileHandle));

				// convert to RGB 24 bits
				image[(i * 3) + 2] = pColourmap[(colour * 3) + 0];
				image[(i * 3) + 1] = pColourmap[(colour * 3) + 1];
				image[(i * 3) + 0] = pColourmap[(colour * 3) + 2];
			}
		}break;

		case 2:
		{
			bCompressed = false;

			// uncompressed 16-24-32 bits
			switch( TGAHeader.PixelDepth )
			{
				case 16:
				{
					unsigned short colour = 0;

					for( i = 0; i < (nWidth * nHeight); ++i )
					{
						// read colour word
						colour = static_cast<unsigned short>( file::FileGetC(fileHandle) + ( file::FileGetC(fileHandle) << 8 ) );

						// convert BGR to RGB
						image[(i * 3) + 0] = static_cast<unsigned char>((((colour & 0x7C00) >> 10) << 3));
						image[(i * 3) + 1] = static_cast<unsigned char>((((colour & 0x03E0) >>  5) << 3));
						image[(i * 3) + 2] = static_cast<unsigned char>((((colour & 0x001F) >>  0) << 3));
					}
				}break;

				case 24:
				{
					for( i = 0; i < (nWidth * nHeight); ++i )
					{
						// read and convert BGR to RGB
						image[(i * 3) + 2] = static_cast<unsigned char>(file::FileGetC(fileHandle));
						image[(i * 3) + 1] = static_cast<unsigned char>(file::FileGetC(fileHandle));
						image[(i * 3) + 0] = static_cast<unsigned char>(file::FileGetC(fileHandle));
					}
				}break;

				case 32:
				{
					for( i = 0; i < (nWidth * nHeight); ++i )
					{
						// read and convert BGRA to RGBA
						image[(i * 4) + 2] = static_cast<unsigned char>(file::FileGetC(fileHandle));
						image[(i * 4) + 1] = static_cast<unsigned char>(file::FileGetC(fileHandle));
						image[(i * 4) + 0] = static_cast<unsigned char>(file::FileGetC(fileHandle));
						image[(i * 4) + 3] = static_cast<unsigned char>(file::FileGetC(fileHandle));
					}
				}break;
			}
		}break;

		case 3:
		{
			bCompressed = false;

			// uncompressed 8 or 16 bits grayscale
			if( TGAHeader.PixelDepth == 8 )
			{
				for( i = 0; i < (nWidth * nHeight); ++i )
				{
					// read grayscale colour byte
					image[i] = static_cast<unsigned char>(file::FileGetC(fileHandle));
				}
			}
			else // 16
			{
				for( i = 0; i < (nWidth * nHeight); ++i )
				{
					// read grayscale colour + alpha channel bytes
					image[(i * 2) + 0] = static_cast<unsigned char>(file::FileGetC(fileHandle));
					image[(i * 2) + 1] = static_cast<unsigned char>(file::FileGetC(fileHandle));
				}
			}
		}break;

		case 9:
		{
			// RLE compressed 8 bits colour index
			bCompressed = true;

			int size;
			unsigned char colour;
			unsigned char packet_header;
			unsigned char *ptr = image;

			while( ptr < image + (nWidth * nHeight) * 3 )
			{
				// read first byte
				packet_header = static_cast<unsigned char>(file::FileGetC(fileHandle));
				size = 1 + (packet_header & 0x7f);

				if( packet_header & 0x80 )
				{
					// run-length packet
					colour = static_cast<unsigned char>(file::FileGetC(fileHandle));

					for( i = 0; i < size; ++i, ptr += 3 )
					{
						ptr[0] = pColourmap[(colour * 3) + 2];
						ptr[1] = pColourmap[(colour * 3) + 1];
						ptr[2] = pColourmap[(colour * 3) + 0];
					}
				}
				else
				{
					// non run-length packet
					for( i = 0; i < size; ++i, ptr += 3 )
					{
						colour = static_cast<unsigned char>(file::FileGetC(fileHandle));

						ptr[0] = pColourmap[(colour * 3) + 2];
						ptr[1] = pColourmap[(colour * 3) + 1];
						ptr[2] = pColourmap[(colour * 3) + 0];
					}
				}
			}
		}break;

		case 10:
		{
			bCompressed = true;

			// RLE compressed 16-24-32 bits
			switch( TGAHeader.PixelDepth )
			{
				case 16:
				{
					int size;
					unsigned short colour;
					unsigned char packet_header;
					unsigned char *ptr = image;

					while( ptr < image + (nWidth * nHeight) * 3 )
					{
						// read first byte
						packet_header = static_cast<unsigned char>(file::FileGetC(fileHandle));
						size = 1 + (packet_header & 0x7f);

						if( packet_header & 0x80 )
						{
							// run-length packet
							colour = static_cast<unsigned short>(file::FileGetC(fileHandle) + ( file::FileGetC(fileHandle) << 8 ));

							for( i = 0; i < size; ++i, ptr += 3 )
							{
								ptr[0] = static_cast<unsigned char>((((colour & 0x7C00) >> 10) << 3));
								ptr[1] = static_cast<unsigned char>((((colour & 0x03E0) >>  5) << 3));
								ptr[2] = static_cast<unsigned char>((((colour & 0x001F) >>  0) << 3));
							}
						}
						else
						{
							// non run-length packet
							for( i = 0; i < size; ++i, ptr += 3 )
							{
								colour = static_cast<unsigned short>(file::FileGetC(fileHandle) + ( file::FileGetC(fileHandle) << 8 ));

								ptr[0] = static_cast<unsigned char>((((colour & 0x7C00) >> 10) << 3));
								ptr[1] = static_cast<unsigned char>((((colour & 0x03E0) >>  5) << 3));
								ptr[2] = static_cast<unsigned char>((((colour & 0x001F) >>  0) << 3));
							}
						}
					}
				}break;

				case 24:
				{
					int size;
					unsigned char rgb[3];
					unsigned char packet_header;
					unsigned char *ptr = image;

					while( ptr < image + (nWidth * nHeight) * 3 )
					{
						// read first byte
						packet_header = static_cast<unsigned char>(file::FileGetC(fileHandle));
						size = 1 + (packet_header & 0x7f);

						if (packet_header & 0x80)
						{
							// run-length packet
							file::FileRead( rgb, sizeof(unsigned char), 3, fileHandle );

							for( i = 0; i < size; ++i, ptr += 3 )
							{
								ptr[0] = rgb[2];
								ptr[1] = rgb[1];
								ptr[2] = rgb[0];
							}
						}
						else
						{
							// non run-length packet
							for( i = 0; i < size; ++i, ptr += 3 )
							{
								ptr[2] = static_cast<unsigned char>(file::FileGetC(fileHandle));
								ptr[1] = static_cast<unsigned char>(file::FileGetC(fileHandle));
								ptr[0] = static_cast<unsigned char>(file::FileGetC(fileHandle));
							}
						}
					}
				}break;

				case 32:
				{
					int size;
					unsigned char rgba[4];
					unsigned char packet_header;
					unsigned char *ptr = image;

					while( ptr < image + (nWidth * nHeight) * 4 )
					{
						// read first byte
						packet_header = static_cast<unsigned char>(file::FileGetC(fileHandle));
						size = 1 + (packet_header & 0x7f);

						if( packet_header & 0x80 )
						{
							// run-length packet
							file::FileRead( rgba, sizeof(unsigned char), 4, fileHandle );

							for( i = 0; i < size; ++i, ptr += 4 )
							{
								ptr[0] = rgba[2];
								ptr[1] = rgba[1];
								ptr[2] = rgba[0];
								ptr[3] = rgba[3];
							}
						}
						else
						{
							// non run-length packet
							for( i = 0; i < size; ++i, ptr += 4 )
							{
								ptr[2] = static_cast<unsigned char>(file::FileGetC(fileHandle));
								ptr[1] = static_cast<unsigned char>(file::FileGetC(fileHandle));
								ptr[0] = static_cast<unsigned char>(file::FileGetC(fileHandle));
								ptr[3] = static_cast<unsigned char>(file::FileGetC(fileHandle));
							}
						}
					}
				}break;
			}
		}break;

		case 11:
		{
			// RLE compressed 8 or 16 bits grayscale
			bCompressed = true;

			if( TGAHeader.PixelDepth == 8 )
			{
				int size;
				unsigned char colour;
				unsigned char packet_header;
				unsigned char *ptr = image;

				while( ptr < image + (nWidth * nHeight) )
				{
					// read first byte
					packet_header = static_cast<unsigned char>(file::FileGetC(fileHandle));
					size = 1 + (packet_header & 0x7f);

					if( packet_header & 0x80 )
					{
						// run-length packet
						colour = static_cast<unsigned char>(file::FileGetC(fileHandle));

						for( i = 0; i < size; ++i, ptr++ )
							*ptr = colour;
					}
					else
					{
						// non run-length packet
						for( i = 0; i < size; ++i, ptr++ )
							*ptr = static_cast<unsigned char>(file::FileGetC(fileHandle));
					}
				}
			}
			else // 16
			{
				int size;
				unsigned char colour, alpha;
				unsigned char packet_header;
				unsigned char *ptr = image;

				while( ptr < image + (nWidth * nHeight) * 2 )
				{
					// read first byte
					packet_header = static_cast<unsigned char>(file::FileGetC(fileHandle));
					size = 1 + (packet_header & 0x7f);

					if( packet_header & 0x80 )
					{
						// run-length packet
						colour = static_cast<unsigned char>(file::FileGetC(fileHandle));
						alpha = static_cast<unsigned char>(file::FileGetC(fileHandle));

						for( i = 0; i < size; ++i, ptr += 2 )
						{
							ptr[0] = colour;
							ptr[1] = alpha;
						}
					}
					else
					{
						// non run-length packet
						for( i = 0; i < size; ++i, ptr += 2 )
						{
							ptr[0] = static_cast<unsigned char>(file::FileGetC(fileHandle));
							ptr[1] = static_cast<unsigned char>(file::FileGetC(fileHandle));
						}
					}
				}
			}
		}break;

		default:
		{
			// image type is not correct
			DBGLOG( "TEXTURE: *ERROR* unknown TGA image type %i\n", TGAHeader.ImageType );

			if( image )
			{
				delete[] image;
				image = 0;
			}

			if( pColourmap )
			{
				delete[] pColourmap;
				pColourmap = 0;
			}

			// close the file
			if( pFile == 0 )
			{
				file::FileClose( fileHandle );
				delete fileHandle;
			}

			return(1);
		}break;
	}

	// no longer need colourmap data 
	if( pColourmap )
	{
	    delete[] pColourmap;
		pColourmap = 0;
	}

	// tga needs flipping
	if( TGAHeader.ImageDescriptor & (1<<5) )
		Flip();

	// close the file
	if( pFile == 0 )
	{
		file::FileClose( fileHandle );
		delete fileHandle;
	}

	//DBGLOG( "TEXTURE: IMAGE %s Loaded\t\t(w:%d, h:%d, bpp:%d, ch:%d, file_size:%d, image_size:%d, alpha:%s, compressed:%s)\n", 
	//	szFilename, nWidth, nHeight, nBPP, nNumChannels, nFileSize, 
	//	nTextureSize, bHasAlpha?"YES":"NO", bCompressed?"YES":"NO" );

	return(0);
}

/////////////////////////////////////////////////////
/// Function: SaveTGA
/// Params: [in]szFilename
///
/////////////////////////////////////////////////////
int Texture::SaveTGA( const char *szFilename, file::TFileHandle* pFile )
{
	file::TFileHandle* fileHandle = 0;
	TTGAHeader TGAHeader;
	unsigned char tempRGB;
	int			  i = 0;

	// FIXME: add support for at least greyscale and maybe index in the end
	//if( nNumChannels < 3 )
	//{
	//	DBGLOG( "TEXTURE: *ERROR* cannot currently save greyscale or index tga textures\n" );
	//	return(1);
	//}

	if( pFile == 0 )
	{
		fileHandle = new file::TFileHandle;

		if( core::IsEmptyString( szFilename ) )
		{
			delete fileHandle;
			return(1);
		}
		if( !file::FileOpen( szFilename, file::FILETYPE_BINARY_WRITE, fileHandle ) )
		{
			DBGLOG( "TEXTURE: *ERROR* Failed saving tga: %s\n", szFilename );
			delete fileHandle;
			return(1);
		}
	}
	else
	{
		fileHandle = pFile;

		// get the start point of this save file
		fileHandle->subFile.nFileStartPos = static_cast<long>(file::FilePosition(fileHandle));
	}

	std::memset( &TGAHeader, 0, sizeof(TTGAHeader) );

	TGAHeader.IDLength			= 0;													// size of image id
	TGAHeader.ColourmapType		= 0;													// 1 is has a colourmap
	if (nNumChannels == 1)
		TGAHeader.ImageType = TGA_IMAGE_TYPE_MONO;													// compression type
	else
		TGAHeader.ImageType = TGA_IMAGE_TYPE_BGR;
	TGAHeader.Cm_first_entry	= core::EndianSwapShort( 0, core::MACHINE_LITTLE_ENDIAN );			// colourmap origin
	TGAHeader.Cm_length			= core::EndianSwapShort( 0, core::MACHINE_LITTLE_ENDIAN );			// colourmap length
	TGAHeader.Cm_size			= 0;													// colourmap size
	TGAHeader.nXOrigin			= core::EndianSwapShort( 0, core::MACHINE_LITTLE_ENDIAN );			// bottom left x coord origin
	TGAHeader.nYOrigin			= core::EndianSwapShort( 0, core::MACHINE_LITTLE_ENDIAN );			// bottom left y coord origin
	TGAHeader.nWidth			= core::EndianSwapShort( static_cast<unsigned short>(nWidth), core::MACHINE_LITTLE_ENDIAN );		// picture width (in pixels)
	TGAHeader.nHeight			= core::EndianSwapShort( static_cast<unsigned short>(nHeight), core::MACHINE_LITTLE_ENDIAN );	// picture height (in pixels)
	TGAHeader.PixelDepth		= static_cast<unsigned char>(nBPP);						// bits per pixel: 8, 16, 24 or 32
	TGAHeader.ImageDescriptor	= 0;													// 24 bits = 0x00; 32 bits = 0x80

	// write the header
	file::FileWrite( &TGAHeader, sizeof(TTGAHeader), fileHandle );

	// change image data from RGB(A) to BGR(A)
	if( nNumChannels == 3 || nNumChannels == 4 )
	{
		for( i = 0; i < (int)nTextureSize; i += nNumChannels ) 
		{
			tempRGB = image[i];
			image[i] = image[i + 2];
			image[i + 2] = tempRGB;
		}
	}

	// write the image data
	file::FileWrite( image, sizeof(unsigned char), nTextureSize, fileHandle );

	// close the file
	if( pFile == 0 )
	{
		file::FileClose( fileHandle );
		delete fileHandle;
	}
	else
	{
		// get the end point of this save file
		fileHandle->subFile.nFileEndPos = static_cast<long>(file::FilePosition(fileHandle));

		fileHandle->subFile.nFileLength = fileHandle->subFile.nFileEndPos - fileHandle->subFile.nFileStartPos;
	}

	DBGLOG( "TEXTURE: IMAGE %s Saved\t\t(w:%d, h:%d, alpha:%s)\n", szFilename, nWidth, nHeight,
			bHasAlpha?"YES":"NO" );

	return(0);
}

#ifdef BASE_SUPPORT_PNG
/////////////////////////////////////////////////////
/// Method: LoadPNG
/// Params: [in]szFilename
///
/////////////////////////////////////////////////////
int Texture::LoadPNG( const char *szFilename, file::TFileHandle* pFile ) 
{
	file::TFileHandle* fileHandle = 0;
	int i;
	int bit_depth, colour_type;
	png_byte magic[8];
	png_structp png_ptr;
	png_infop info_ptr;
	 
	png_bytep *row_pointers = 0;

	// clear any old data
	this->Free();

	if( pFile == 0 )
	{
		fileHandle = new file::TFileHandle;

		if( core::IsEmptyString( szFilename ) )
		{
			delete fileHandle;
			return(1);
		}

		if( !file::FileOpen( szFilename, file::FILETYPE_BINARY_READ, fileHandle ) )
		{
			DBGLOG( "TEXTURE: *ERROR* Failed loading png: %s\n", szFilename );
			delete fileHandle;
			return(1);
		}
		
		// find length of file
		nFileSize = file::FileSize( fileHandle );

		file::CreateFileStructure( szFilename, &TextureFile );
	}
	else
	{
		fileHandle = pFile;
	}

	// read magic number
	file::FileRead( magic, sizeof(magic), fileHandle );

	// check for valid magic number
	if( png_sig_cmp( magic, 0, sizeof(magic) ) )
	{
		DBGLOG( "TEXTURE: *ERROR* %s is not a valid PNG image\n", szFilename );
		if( pFile == 0 )
		{
			file::FileClose( fileHandle );
			delete fileHandle;
		}
		return(1);
	}

	// create a png read struct
	png_ptr = png_create_read_struct( PNG_LIBPNG_VER_STRING, 0, 0, 0 );
	if( !png_ptr )
	{
		if( pFile == 0 )
		{
			file::FileClose( fileHandle );
			delete fileHandle;
		}
		return(1);
	}

	// create a png info struct
	info_ptr = png_create_info_struct( png_ptr );
	if( !info_ptr )
	{
		if( pFile == 0 )
		{
			file::FileClose( fileHandle );
			delete fileHandle;
		}
		png_destroy_read_struct( &png_ptr, 0, 0 );
		return(1);
	}

	// initialize the setjmp for returning properly after a libpng
	//	error occured 
	if( setjmp( (*png_set_longjmp_fn((png_ptr), (png_longjmp_ptr)longjmp, sizeof (jmp_buf)))) )
	{
		if( pFile == 0 )
		{
			file::FileClose( fileHandle );
			delete fileHandle;
		}
		png_destroy_read_struct (&png_ptr, &info_ptr, 0);

		if(row_pointers)
		{
			delete[] row_pointers;
			row_pointers = 0;
		}

		if(image)
		{
			delete[] image;
			image = 0;
		}

		return(1);
	}

	// setup libpng for using standard C fread() function
	// with our FILE pointer 
	if( fileHandle->zipped )
	{
		png_set_read_fn( png_ptr, fileHandle->zfp, (png_rw_ptr)png_zip_read );
	}
	else
		png_init_io( png_ptr, fileHandle->fp );

	// tell libpng that we have already read the magic number
	png_set_sig_bytes( png_ptr, sizeof(magic) );

	// read png info */
	png_read_info( png_ptr, info_ptr );

	// get some usefull information from header
	bit_depth = png_get_bit_depth( png_ptr, info_ptr );
	colour_type = png_get_color_type( png_ptr, info_ptr );

	// convert index colour images to RGB images
	if( colour_type == PNG_COLOR_TYPE_PALETTE )
		png_set_palette_to_rgb( png_ptr );

	// convert 1-2-4 bits grayscale images to 8 bits
	// grayscale.
	if( colour_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8 )
		png_set_expand_gray_1_2_4_to_8( png_ptr );

	if( png_get_valid( png_ptr, info_ptr, PNG_INFO_tRNS ) )
		png_set_tRNS_to_alpha( png_ptr );

	if( bit_depth == 16 )
		png_set_strip_16( png_ptr );
	else if( bit_depth < 8 )
		png_set_packing( png_ptr );

	// update info structure to apply transformations
	png_read_update_info( png_ptr, info_ptr );

	// retrieve updated information
	png_get_IHDR (png_ptr, info_ptr, 
			(png_uint_32*)(&nWidth),
			(png_uint_32*)(&nHeight),
			&bit_depth, &colour_type,
			0, 0, 0);

	switch( colour_type )
	{
		case PNG_COLOR_TYPE_GRAY:
		{
			nFormat = GL_ALPHA;
			nInternalFormat = GL_ALPHA;
			nNumChannels = 1;
			nTextureType = GL_UNSIGNED_BYTE;

			bHasAlpha = false;
		}break;

		case PNG_COLOR_TYPE_GRAY_ALPHA:
		{
			nFormat = GL_LUMINANCE_ALPHA;
			nInternalFormat = GL_LUMINANCE_ALPHA;
			nNumChannels = 2;
			nTextureType = GL_UNSIGNED_BYTE;

			bHasAlpha = false;
		}break;

		case PNG_COLOR_TYPE_RGB:
		{
			nFormat = GL_RGB;
            nInternalFormat = GL_RGB;
			nNumChannels = 3;
			nTextureType = GL_UNSIGNED_BYTE;

			bHasAlpha = false;
		}break;

		case PNG_COLOR_TYPE_RGB_ALPHA:
		{
			nFormat = GL_RGBA;
            nInternalFormat = GL_RGBA;
			nNumChannels = 4;
			nTextureType = GL_UNSIGNED_BYTE;

			bHasAlpha = true;
		}break;

		default:
		// Badness
		break;
	}

	eFileFormat		= TEXTURE_PNG;		
	nBPP			= nNumChannels*8;										
	nOriginalWidth	= nWidth;
	nOriginalHeight = nHeight;
	nMipMapCount	= 1;
	bHasMipMaps		= false;
	bIsCubemap		= false;
	bIsVolume		= false;

	if( (nWidth <= 0) || (nHeight <= 0) )	
	{
		DBGLOG( "TEXTURE: *ERROR* invalid tga information\n" );	
		if( pFile == 0 )
		{
			file::FileClose( fileHandle );
			delete fileHandle;
		}												
		return(1);													
	}

	// allocate memory							
	nTextureSize	= (nWidth * nHeight) * nNumChannels;	
	image			= new unsigned char[nTextureSize];

	// setup a pointer array.  Each one points at the begening of a row.
	row_pointers = new png_bytep[nHeight];

	for( i = 0; i < nHeight; ++i )
	{
		row_pointers[i] = (png_bytep)( image +
			(( nHeight - (i + 1)) * nWidth * nNumChannels ));
	}

	// read pixel data using row pointers
	png_read_image( png_ptr, row_pointers );

	// finish decompression and release memory
	png_read_end( png_ptr, 0 );
	png_destroy_read_struct( &png_ptr, &info_ptr, 0 );

	// we don't need row pointers anymore
	if( row_pointers )
	{
		delete[] row_pointers;
		row_pointers = 0;
	}

	// close the file
	if( pFile == 0 )
	{
		file::FileClose( fileHandle );
		delete fileHandle;
	}
    
	//DBGLOG( "TEXTURE: IMAGE %s Loaded\t\t(w:%d, h:%d, bpp:%d, ch:%d, file_size:%d, image_size:%d, alpha:%s, compressed:%s)\n", 
	//	szFilename, nWidth, nHeight, nBPP, nNumChannels, nFileSize, 
	//	nTextureSize, bHasAlpha?"YES":"NO", bCompressed?"YES":"NO" );

	return(0);
}

/////////////////////////////////////////////////////
/// Method: SavePNG
/// Params: [in]szFilename
///
/////////////////////////////////////////////////////
int Texture::SavePNG( const char *szFilename, file::TFileHandle* pFile )
{
	file::TFileHandle* fileHandle = 0;
	int i = 0;

	// FIXME: add support for at least greyscale and maybe index in the end
	/*if( nNumChannels < 3 )
	{
		DBGLOG( "TEXTURE: *ERROR* cannot currently save greyscale or index png textures\n" );
		return(1);
	}*/

	if( pFile == 0 )
	{
		fileHandle = new file::TFileHandle;

		if( core::IsEmptyString( szFilename ) )
		{
			delete fileHandle;
			return(1);
		}
		if( !file::FileOpen( szFilename, file::FILETYPE_BINARY_WRITE, fileHandle ) )
		{
			DBGLOG( "TEXTURE: *ERROR* Failed saving png: %s\n", szFilename );
			delete fileHandle;
			return(1);
		}
	}
	else
	{
		fileHandle = pFile;
		// get the start point of this save file
		fileHandle->subFile.nFileStartPos = static_cast<long>(file::FilePosition(fileHandle));
	}

    // create write struct
    png_structp png_ptr = png_create_write_struct( PNG_LIBPNG_VER_STRING, 0, 0, 0 );
	if( !png_ptr ) 
	{
		if( pFile == 0 )
		{
			file::FileClose( fileHandle );
			delete fileHandle;
		}
		return(1);
    }

    // error handling!
    if( setjmp( (*png_set_longjmp_fn((png_ptr), (png_longjmp_ptr)longjmp, sizeof (jmp_buf)))) )
	{
		png_destroy_write_struct(&png_ptr, 0);

		if( pFile == 0 )
		{
			file::FileClose( fileHandle );
			delete fileHandle;
		}
		return(1);
	}

    // create info struct
    png_infop info_ptr = png_create_info_struct( png_ptr );
	if( !info_ptr ) 
	{
		png_destroy_write_struct(&png_ptr, 0);
		if( pFile == 0 )
		{
			file::FileClose( fileHandle );
			delete fileHandle;
		}
		return(1);
	}

	int width  = nWidth;
	int height = nHeight;

	// set image characteristics
	if( fileHandle->nFileType != file::FILETYPE_BINARY_WRITE )
	{
		if( pFile == 0 )
		{
			file::FileClose( fileHandle );
			delete fileHandle;
		}
		DBGLOG( "TEXTURE: *ERROR* png saving only works with FILETYPE_BINARY_WRITE\n" );
		return(1);		
	}

	png_init_io( png_ptr, fileHandle->fp );

	int colour_format = 0; // png output format
	int colour_format_bpp = 0; // png bytes per pixel
	bool colour_format_paletted = false; // png palette needed flag

	// figure out output format
	switch( nFormat ) 
	{
		case GL_RGBA:
		{
			colour_format = PNG_COLOR_TYPE_RGB_ALPHA;
			colour_format_bpp = 4;
			colour_format_paletted = false;
		}break;
		case GL_RGB:
		{
			colour_format = PNG_COLOR_TYPE_RGB;
			colour_format_bpp = 3;
			colour_format_paletted = false;
		}break;
        case GL_ALPHA:
        {
            colour_format = PNG_COLOR_TYPE_GRAY; //PNG_COLOR_TYPE_GRAY_ALPHA
            colour_format_bpp = 1;
            colour_format_paletted = false;
        }break;
//		case GL_:
//		{
//			colour_format = PNG_COLOR_TYPE_PALETTE;
//			colour_format_bpp = 1;
//			colour_format_paletted = true;
//		}break;
		default:
		{
			// Unsupported format.  This should already be taken care of
			// by the test at the beginning of this function.
			png_destroy_write_struct(&png_ptr, &info_ptr);
			return false;
		}break;
    }

	png_set_IHDR( png_ptr, info_ptr, width, height, 8, colour_format, PNG_INTERLACE_NONE,
					PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT );

    png_color* png_palette = 0;
    if( colour_format_paletted ) 
	{
/*		DBGLOG( "TEXTURE: Saving palettized image..." );

		int image_palette_format = image->getPaletteFormat(); // palette format
		int image_palette_size = image->getPaletteSize(); // palette size

		// allocate png palette and get pointer to image palette
		png_palette = (png_colour*)png_malloc( png_ptr, sizeof(png_colour) * image_palette_size );
		byte* image_palette = (byte*)image->getPalette();

		if( image_palette_format == GL_RGBA ) 
		{
			// 24 bit source palette
			for (int i = 0; i < image_palette_size; i++) 
			{
				// copy entry directly
				png_palette[i].red   = *image_palette++;
				png_palette[i].green = *image_palette++;
				png_palette[i].blue  = *image_palette++;
			}
		} 
		else if( image_palette_format == GL_RGBA ) 
		{
			// 32 bit source palette
			for (int i = 0; i < image_palette_size; i++) 
			{
				// copy entry, skip alpha
				png_palette[i].red   = *image_palette++;
				png_palette[i].green = *image_palette++;
				png_palette[i].blue  = *image_palette++;
				image_palette++;
			}
		}
		// write palette
		png_set_PLTE(png_ptr, info_ptr, png_palette, image_palette_size);
*/
		return(1);
	}

	// image needs to be flipped vertically, so do it here 	
	Flip();

	// pass the image
	unsigned char* pixels = image;

	// build rows
	void** rows = (void**)png_malloc( png_ptr, sizeof(void*) * height );
	for( i = 0; i < height; ++i ) 
	{
		rows[i] = png_malloc(png_ptr, colour_format_bpp * width);
		std::memcpy(rows[i], pixels, colour_format_bpp * width);
		pixels += width * colour_format_bpp;      
	}
	png_set_rows( png_ptr, info_ptr, (png_bytepp)rows );
	//info_ptr->valid |= PNG_INFO_IDAT;

	// actually write the image
	png_write_png( png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, 0 );

	// clean up memory
	for( i = 0; i < height; ++i ) 
	{
		png_free( png_ptr, rows[i] );
	}
	png_free(png_ptr, rows);

	if( png_palette ) 
	{
		png_free( png_ptr, png_palette );
	}

	png_destroy_write_struct(&png_ptr, &info_ptr);
	
	if( pFile == 0 )
	{
		file::FileClose( fileHandle );
		delete fileHandle;
	}
	else
	{
		// get the end point of this save file
		fileHandle->subFile.nFileEndPos = static_cast<long>(file::FilePosition(fileHandle));

		fileHandle->subFile.nFileLength = fileHandle->subFile.nFileEndPos - fileHandle->subFile.nFileStartPos;
	}

	DBGLOG( "TEXTURE: IMAGE %s Saved\t\t(w:%d, h:%d, alpha:%s)\n", szFilename, nWidth, nHeight,
			bHasAlpha?"YES":"NO" );

    return(0);
}

#endif // BASE_SUPPORT_PNG

#ifdef BASE_SUPPORT_DDS
/////////////////////////////////////////////////////
/// Method: LoadDDS
/// Params: [in]szFilename
///
/////////////////////////////////////////////////////
int Texture::LoadDDS( const char *szFilename, file::TFileHandle* pFile )
{
	file::TFileHandle* fileHandle = 0;
	char magic[4];
	TDDSHeader DDSHeader;

	// clear any old data
	this->Free();

	if( pFile == 0 )
	{
		fileHandle = new file::TFileHandle;

		if( core::IsEmptyString( szFilename ) )
		{
			delete fileHandle;
			return(1);
		}
		if( !file::FileOpen( szFilename, file::FILETYPE_BINARY_READ, fileHandle ) )
		{
			DBGLOG( "TEXTURE: *ERROR* Failed loading dds: %s\n", szFilename );
			delete fileHandle;
			return(1);
		}
	
		// find length of file
		nFileSize = file::FileSize( fileHandle );

		file::CreateFileStructure( szFilename, &TextureFile );
	}
	else
	{
		fileHandle = pFile;
	}

	// read magic number and check if valid .dds file 
	file::FileRead( &magic, sizeof(char), 4, fileHandle );

	if( std::strncmp( magic, "DDS ", 4 ) != 0 )
	{
		DBGLOG( "TEXTURE: *ERROR* Failed loading dds %s\n", szFilename );
		if( pFile == 0 )
		{
			file::FileClose( fileHandle );
			delete fileHandle;
		}
		return(1);
	}

	// get the surface descriptor
	file::FileRead( &DDSHeader, sizeof(TDDSHeader), fileHandle );
	
	DDSHeader.dwSize = static_cast<unsigned long>(core::LittleToBigEndianInt( DDSHeader.dwSize ));
	DDSHeader.dwFlags = static_cast<unsigned long>(core::LittleToBigEndianInt( DDSHeader.dwFlags ));
	DDSHeader.dwHeight = static_cast<unsigned long>(core::LittleToBigEndianInt( DDSHeader.dwHeight ));
	DDSHeader.dwWidth = static_cast<unsigned long>(core::LittleToBigEndianInt( DDSHeader.dwWidth )); 
	DDSHeader.dwPitchOrLinearSize = static_cast<unsigned long>(core::LittleToBigEndianInt( DDSHeader.dwPitchOrLinearSize ));
	DDSHeader.dwDepth = static_cast<unsigned long>(core::LittleToBigEndianInt( DDSHeader.dwDepth ));
	DDSHeader.dwMipMapCount = static_cast<unsigned long>(core::LittleToBigEndianInt( DDSHeader.dwMipMapCount ));
	//unsigned long dwReserved1[11];
	DDSHeader.ddspf.dwSize = static_cast<unsigned long>(core::LittleToBigEndianInt( DDSHeader.ddspf.dwSize ));
	DDSHeader.ddspf.dwFlags = static_cast<unsigned long>(core::LittleToBigEndianInt( DDSHeader.ddspf.dwFlags ));
	DDSHeader.ddspf.dwFourCC = static_cast<unsigned long>(core::LittleToBigEndianInt( DDSHeader.ddspf.dwFourCC ));
	DDSHeader.ddspf.dwRGBBitCount = static_cast<unsigned long>(core::LittleToBigEndianInt( DDSHeader.ddspf.dwRGBBitCount ));
	DDSHeader.ddspf.dwRBitMask = static_cast<unsigned long>(core::LittleToBigEndianInt( DDSHeader.ddspf.dwRBitMask ));
	DDSHeader.ddspf.dwGBitMask = static_cast<unsigned long>(core::LittleToBigEndianInt( DDSHeader.ddspf.dwGBitMask ));
	DDSHeader.ddspf.dwBBitMask = static_cast<unsigned long>(core::LittleToBigEndianInt( DDSHeader.ddspf.dwBBitMask ));
	DDSHeader.ddspf.dwABitMask = static_cast<unsigned long>(core::LittleToBigEndianInt( DDSHeader.ddspf.dwABitMask ));
	DDSHeader.dwCaps1 = static_cast<unsigned long>(core::LittleToBigEndianInt( DDSHeader.dwCaps1 ));
	DDSHeader.dwCaps2 = static_cast<unsigned long>(core::LittleToBigEndianInt( DDSHeader.dwCaps2 ));
	//unsigned long dwReserved2[3];
	
	eFileFormat	= TEXTURE_DDS;
	bCompressed	= true;
	nWidth		= DDSHeader.dwWidth;
	nHeight		= DDSHeader.dwHeight;
	nOriginalWidth	= DDSHeader.dwWidth;
	nOriginalHeight	= DDSHeader.dwHeight;

	// check if image is a volume texture
	if( (DDSHeader.dwCaps2 & DDSF_VOLUME) && (DDSHeader.dwDepth > 0) )
	{
		bIsVolume = true;
		nDepth = DDSHeader.dwDepth;
	}
	else
	{
		bIsVolume = false;
		nDepth = 0;
	}

	// check for mipmaps
	if( DDSHeader.dwFlags & DDSF_MIPMAPCOUNT ) 
	{
		bHasMipMaps = true;
		nMipMapCount = DDSHeader.dwMipMapCount;
	}
	else
	{
		bHasMipMaps = false;
		nMipMapCount = 1;
	}

	// check for a cubemap texture
	if( DDSHeader.dwCaps2 & DDSF_CUBEMAP ) 
	{
		// this is a cubemap, count the faces
		nCubemapFaceCount = 0;
		nCubemapFaceCount += (DDSHeader.dwCaps2 & DDSF_CUBEMAP_POSITIVEX) ? 1 : 0;
		nCubemapFaceCount += (DDSHeader.dwCaps2 & DDSF_CUBEMAP_NEGATIVEX) ? 1 : 0;
		nCubemapFaceCount += (DDSHeader.dwCaps2 & DDSF_CUBEMAP_POSITIVEY) ? 1 : 0;
		nCubemapFaceCount += (DDSHeader.dwCaps2 & DDSF_CUBEMAP_NEGATIVEY) ? 1 : 0;
		nCubemapFaceCount += (DDSHeader.dwCaps2 & DDSF_CUBEMAP_POSITIVEZ) ? 1 : 0;
		nCubemapFaceCount += (DDSHeader.dwCaps2 & DDSF_CUBEMAP_NEGATIVEZ) ? 1 : 0;

		bIsCubemap = true;

		// check for a complete cubemap
		if ( (nCubemapFaceCount != 6) || (nWidth != nHeight) ) 
		{
			if( pFile == 0 )
			{
				file::FileClose( fileHandle );
				delete fileHandle;
			}
			return(1);
        }
    }
    else 
	{
        // not a cubemap
		bIsCubemap = false;
        nCubemapFaceCount = 0;
    }

	nBytesPerElement = 0;

	// figure out what the image format is
	if( DDSHeader.ddspf.dwFlags & DDSF_FOURCC ) 
	{
		switch( DDSHeader.ddspf.dwFourCC )
		{
			case FOURCC_DXT1:
			{
				nFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
				nInternalFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
				nTextureType = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
				nBytesPerElement = 8;
				bDXTCompressed = true;
			}break;

			case FOURCC_DXT2:
			case FOURCC_DXT3:
			{
				nFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
				nInternalFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
				nTextureType = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
				nBytesPerElement = 16;
				bDXTCompressed = true;
			}break;

			case FOURCC_DXT4:
			case FOURCC_DXT5:
			{
				nFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
				nInternalFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
				nTextureType = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
				nBytesPerElement = 16;
				bDXTCompressed = true;
			}break;

			case FOURCC_R8G8B8:
			{
				nFormat = GL_BGR;
				nInternalFormat = GL_RGB;
				nTextureType = GL_UNSIGNED_BYTE;
				nBytesPerElement = 3;
				bDXTCompressed = false;
			}break;

			case FOURCC_A8R8G8B8:
			{
				nFormat = GL_BGRA;
				nInternalFormat = GL_RGBA;
				nTextureType = GL_UNSIGNED_BYTE;
				nBytesPerElement = 4;
				bDXTCompressed = false;
			}break;

			case FOURCC_X8R8G8B8:
			{
				nFormat = GL_BGRA;
				nInternalFormat = GL_RGB;
				nTextureType = GL_UNSIGNED_INT_8_8_8_8;
				nBytesPerElement = 4;
				bDXTCompressed = false;
			}break;

			case FOURCC_R5G6B5:
			{
				nFormat = GL_BGR;
				nInternalFormat = GL_RGB5;
				nTextureType = GL_UNSIGNED_SHORT_5_6_5;
				nBytesPerElement = 2;
				bDXTCompressed = false;
			}break;

			case FOURCC_A8:
			{
				nFormat = GL_ALPHA;
				nInternalFormat = GL_ALPHA;
				nTextureType = GL_UNSIGNED_BYTE;
				nBytesPerElement = 1;
				bDXTCompressed = false;
			}break;

			case FOURCC_A2B10G10R10:
			{
				nFormat = GL_RGBA;
				nInternalFormat = GL_RGB10_A2;
				nTextureType = GL_UNSIGNED_INT_10_10_10_2;
				nBytesPerElement = 4;
				bDXTCompressed = false;
			}break;

			case FOURCC_A8B8G8R8:
			{
				nFormat = GL_RGBA;
				nInternalFormat = GL_RGBA;
				nTextureType = GL_UNSIGNED_BYTE;
				nBytesPerElement = 4;
				bDXTCompressed = false;
			}break;

			case FOURCC_X8B8G8R8:
			{
				nFormat = GL_RGBA;
				nInternalFormat = GL_RGB;
				nTextureType = GL_UNSIGNED_INT_8_8_8_8;
				nBytesPerElement = 4;
				bDXTCompressed = false;
			}break;

			case FOURCC_A2R10G10B10:
			{
				nFormat = GL_BGRA;
				nInternalFormat = GL_RGB10_A2;
				nTextureType = GL_UNSIGNED_INT_10_10_10_2;
				nBytesPerElement = 4;
				bDXTCompressed = false;
			}break;

			case FOURCC_A16B16G16R16:
			{
				nFormat = GL_RGBA;
				nInternalFormat = GL_RGBA16;
				nTextureType = GL_UNSIGNED_SHORT;
				nBytesPerElement = 8;
				bDXTCompressed = false;
			}break;

			case FOURCC_L8:
			{
				nFormat = GL_LUMINANCE;
				nInternalFormat = GL_LUMINANCE;
				nTextureType = GL_UNSIGNED_BYTE;
				nBytesPerElement = 1;
				bDXTCompressed = false;
			}break;

			case FOURCC_A8L8:
			{
				nFormat = GL_LUMINANCE_ALPHA;
				nInternalFormat = GL_LUMINANCE_ALPHA;
				nTextureType = GL_UNSIGNED_BYTE;
				nBytesPerElement = 2;
				bDXTCompressed = false;
			}break;

			case FOURCC_L16:
			{
				nFormat = GL_LUMINANCE;
				nInternalFormat = GL_LUMINANCE16;
				nTextureType = GL_UNSIGNED_SHORT;
				nBytesPerElement = 2;
				bDXTCompressed = false;
			}break;

			case FOURCC_R16F:
			{
				nFormat = GL_LUMINANCE; // should use red, once it is available
				nInternalFormat = GL_LUMINANCE16F_ARB; 
				nTextureType = GL_HALF_FLOAT_ARB;
				nBytesPerElement = 2;
				bDXTCompressed = false;
			}break;

			case FOURCC_A16B16G16R16F:
			{
				nFormat = GL_RGBA;
				nInternalFormat = GL_RGBA16F_ARB;
				nTextureType = GL_HALF_FLOAT_ARB;
				nBytesPerElement = 8;
				bDXTCompressed = false;
			}break;

			case FOURCC_R32F:
			{
				nFormat = GL_LUMINANCE; // should use red, once it is available
				nInternalFormat = GL_LUMINANCE32F_ARB; 
				nTextureType = GL_FLOAT;
				nBytesPerElement = 4;
				bDXTCompressed = false;
			}break;

			case FOURCC_A32B32G32R32F:
			{
				nFormat = GL_RGBA;
				nInternalFormat = GL_RGBA32F_ARB;
				nTextureType = GL_FLOAT;
				nBytesPerElement = 16;
				bDXTCompressed = false;
			}break;

			case FOURCC_UNKNOWN:
			case FOURCC_X1R5G5B5:
			case FOURCC_A1R5G5B5:
			case FOURCC_A4R4G4B4:
			case FOURCC_R3G3B2:
			case FOURCC_A8R3G3B2:
			case FOURCC_X4R4G4B4:
			case FOURCC_A4L4:
			case FOURCC_D16_LOCKABLE:
			case FOURCC_D32:
			case FOURCC_D24X8:
			case FOURCC_D16:
			case FOURCC_D32F_LOCKABLE:
			case FOURCC_G16R16:
			case FOURCC_G16R16F:
			case FOURCC_G32R32F:
				// these are unsupported for now
			default:
				{
					if( pFile == 0 )
					{
						file::FileClose( fileHandle );
						delete fileHandle;
					}
					return(1);
				}break;
		}
	}
	else if( DDSHeader.ddspf.dwFlags == DDSF_RGBA && DDSHeader.ddspf.dwRGBBitCount == 32 )
	{
		nFormat = GL_BGRA;
		nInternalFormat = GL_RGBA;
		nTextureType = GL_UNSIGNED_BYTE;
		nBytesPerElement = 4;
	}
	else if( DDSHeader.ddspf.dwFlags == DDSF_RGB  && DDSHeader.ddspf.dwRGBBitCount == 32 )
	{
		nFormat = GL_BGR;
		nInternalFormat = GL_RGBA;
		nTextureType = GL_UNSIGNED_BYTE;
		nBytesPerElement = 4;
	}
	else if( DDSHeader.ddspf.dwFlags == DDSF_RGB  && DDSHeader.ddspf.dwRGBBitCount == 24 )
	{
		nFormat = GL_BGR;
		nInternalFormat = GL_RGB;
		nTextureType = GL_UNSIGNED_BYTE;
		nBytesPerElement = 3;
	}
	else if( DDSHeader.ddspf.dwRGBBitCount == 8 )
	{
		nFormat = GL_LUMINANCE; 
		nInternalFormat = GL_LUMINANCE; 
		nTextureType = GL_UNSIGNED_BYTE;
		nBytesPerElement = 1;
	}
	else 
	{
		if( pFile == 0 )
		{
			file::FileClose( fileHandle );
			delete fileHandle;
		}
		return(1);
	}

	nNumChannels = nBytesPerElement;
	nBPP = nNumChannels*8;

//    i._elementSize = bytesPerElement;

	// allocate an image list for various base and mipmap textures
	if( bIsCubemap )
		imageList = new unsigned char*[nMipMapCount*nCubemapFaceCount];
	else
		imageList = new unsigned char*[nMipMapCount];

	for( int face = 0; face < ((nCubemapFaceCount) ? nCubemapFaceCount : 1); face++ ) 
	{
		int w = nWidth;
		int h = nHeight;
		int d = (nDepth) ? nDepth : 1;
			
		for( int level = 0; level < nMipMapCount; level++ ) 
		{
			int bw = (bDXTCompressed) ? (w+3)/4 : w;
			int bh = (bDXTCompressed) ? (h+3)/4 : h;
			int size = bw*bh*d*nBytesPerElement;

			imageList[ level + (face*nMipMapCount) ] = new unsigned char[size];

			file::FileRead( imageList[ level + (face*nMipMapCount) ], size, fileHandle );

			// reduce mip sizes
			w = ( w > 1 ) ? w >> 1 : 1;
			h = ( h > 1 ) ? h >> 1 : 1;
			d = ( d > 1 ) ? d >> 1 : 1;
		}
	}

	// call a flip for DDS
	Flip( );

/*
	switch( ddsd.format.fourCC )
	{
		case DDS_FOURCC_DXT1:
		{
			// DXT1's compression ratio is 8:1
			nFormat = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
			nInternalFormat = GL_RGB8;
			bHasAlpha = false;
			nNumChannels = 3;
			nBPP = nNumChannels*8;
			mipmapFactor = 2;
		}break;

		case DDS_FOURCC_DXT3:
		{
			// DXT3's compression ratio is 4:1 
			nFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
			nInternalFormat = GL_RGBA8;
			bHasAlpha = true;
			nNumChannels = 4;
			nBPP = nNumChannels*8;
			mipmapFactor = 4;
		}break;

		case DDS_FOURCC_DXT5:
		{
			// DXT5's compression ratio is 4:1
			nFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			nInternalFormat = GL_RGBA8;
			bHasAlpha = true;
			nNumChannels = 4;
			nBPP = nNumChannels*8;
			mipmapFactor = 4;
		}break;

		default:
			// bad fourCC, unsupported or bad format
			DBGLOG( "TEXTURE: *ERROR* Unsupported DXT\n" );
			fclose (fp);
		return 0;
	}

	// calculate pixel data size
	curr = ftell(fp);
	fseek( fp, 0, SEEK_END );
	end = ftell(fp);
	fseek( fp, curr, SEEK_SET );
	nTextureSize = end - curr;

	// read pixel data with mipmaps
	image = new unsigned char[nTextureSize];
	fread( image, sizeof(unsigned char), nTextureSize, fp );
*/

	// close the file
	if( pFile == 0 )
	{
		file::FileClose( fileHandle );
		delete fileHandle;
	}

	//DBGLOG( "TEXTURE: IMAGE %s Loaded\t\t(w:%d, h:%d, bpp:%d, ch:%d, cubemap:%d, mipmaps:%d, file_size:%d, image_size:%d, alpha:%s, compressed:%s)\n", 
	//	szFilename, nWidth, nHeight, nBPP, nNumChannels, bIsCubemap, nMipMapCount, nFileSize, 
	//	nTextureSize, bHasAlpha?"YES":"NO", bCompressed?"YES":"NO" );

	return(0);
}

#endif // BASE_SUPPORT_DDS

#ifdef BASE_SUPPORT_JPEG

/////////////////////////////////////////////////////
/// Method: LoadJPEG
/// Params: [in]szFilename
///
/////////////////////////////////////////////////////
int Texture::LoadJPEG( const char *szFilename, file::TFileHandle* pFile )
{
	file::TFileHandle* fileHandle = 0;
	int i = 0;
	struct jpeg_decompress_struct jpegInfo;

	// clear any old data
	this->Free();

	if( pFile == 0 )
	{
		fileHandle = new file::TFileHandle;

		if( core::IsEmptyString( szFilename ) )
		{
			delete fileHandle;
			return(1);
		}
		if( !file::FileOpen( szFilename, file::FILETYPE_BINARY_READ, fileHandle ) )
		{
			DBGLOG( "TEXTURE: *ERROR* Failed loading jpeg: %s\n", szFilename );
			delete fileHandle;
			return(1);
		}

		// find length of file
		nFileSize = file::FileSize( fileHandle );

		file::CreateFileStructure( szFilename, &TextureFile );
	}
	else
	{
		fileHandle = pFile;
	}

	// if the jpeg is in a zipped file, read it into a buffer first
	unsigned char* tempBuffer = 0;
	if( fileHandle->zipped )
	{
		tempBuffer = new unsigned char[nFileSize];
		file::FileRead( tempBuffer, nFileSize, 1, fileHandle );
	}

	jpeg_error_mgr jerr;

	// attach error checker
	jpegInfo.err = jpeg_std_error( &jerr );

	// get ready to decompress the data
	jpeg_create_decompress( &jpegInfo );

	if( fileHandle->zipped )
	{
		jpeg_mem_src( &jpegInfo, tempBuffer, nFileSize );
	}
	else
		jpeg_stdio_src( &jpegInfo, fileHandle->fp );

	// decompress 
	jpeg_read_header( &jpegInfo, true );
	
	jpeg_start_decompress( &jpegInfo );

	// get the number of colour channels
	eFileFormat	= TEXTURE_JPEG;
	nWidth			= static_cast<int>(core::LittleToBigEndianInt( jpegInfo.image_width ));
	nHeight			= static_cast<int>(core::LittleToBigEndianInt( jpegInfo.image_height ));
	nBPP			= static_cast<int>(core::LittleToBigEndianInt( jpegInfo.num_components*8 ));
	nNumChannels	= static_cast<int>(core::LittleToBigEndianInt( jpegInfo.num_components ));
	nOriginalWidth	= nWidth;
	nOriginalHeight = nHeight;
	bHasAlpha		= false;
	bCompressed		= true;
	nMipMapCount	= 1;
	bHasMipMaps		= false;
	bIsCubemap		= false;
	bIsVolume		= false;

	if( jpegInfo.num_components == 1 )
	{
		nInternalFormat = GL_LUMINANCE;
		nFormat = GL_LUMINANCE;
		nTextureType = GL_UNSIGNED_BYTE;
	}
	else
	{
		nInternalFormat = GL_RGB;
		nFormat = GL_RGB;
		nTextureType = GL_UNSIGNED_BYTE;
	}

	nTextureSize = (nWidth * nHeight) * nNumChannels;
	image = new unsigned char[nTextureSize];
	
	unsigned char** rowPtr = new unsigned char*[nHeight];
	for( i = 0; i < nHeight; i++ )
		rowPtr[i] = &(image[ (i*nWidth) * nNumChannels ]);
	
	int rowsRead =0 ;
	while( jpegInfo.output_scanline < jpegInfo.output_height ) 
	{
		rowsRead += jpeg_read_scanlines( &jpegInfo, &rowPtr[rowsRead], jpegInfo.output_height - rowsRead );
	}

	delete [] rowPtr;
	rowPtr = 0;
	
	jpeg_finish_decompress( &jpegInfo );

	// finished, release memory
	jpeg_destroy_decompress( &jpegInfo );

	// jpegs need to be flipped vertically, so do it here 	
	Flip( );

	// close the file
	if( pFile == 0 )
	{
		file::FileClose( fileHandle );
		delete fileHandle;
	}

	if( tempBuffer != 0 )
	{
		delete[] tempBuffer;
		tempBuffer = 0;
	}

	//DBGLOG( "TEXTURE: IMAGE %s Loaded\t\t(w:%d, h:%d, bpp:%d, ch:%d, file_size:%d, image_size:%d, alpha:%s, compressed:%s)\n", 
	//	szFilename, nWidth, nHeight, nBPP, nNumChannels, nFileSize, 
	//	nTextureSize, bHasAlpha?"YES":"NO", bCompressed?"YES":"NO" );

	return(0);
}

/////////////////////////////////////////////////////
/// Function: SaveJPEG
/// Params: [in]szFilename
///
/////////////////////////////////////////////////////
int Texture::SaveJPEG( const char *szFilename, file::TFileHandle* pFile )
{
	file::TFileHandle* fileHandle = 0;
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr pub;
	jmp_buf setjmp_buffer;
	JSAMPROW row_pointer[1];
	int row_stride;

	int nQuality = 80;

	// FIXME: add support for at least greyscale
	if( nNumChannels < 3 )
	{
		DBGLOG( "TEXTURE: *ERROR* cannot currently save greyscale or index jpeg textures\n" );
		return(1);
	}

	if( pFile == 0 )
	{
		fileHandle = new file::TFileHandle;

		if( core::IsEmptyString( szFilename ) )
		{
			delete fileHandle;
			return(1);
		}
		if( !file::FileOpen( szFilename, file::FILETYPE_BINARY_WRITE, fileHandle ) )
		{
			DBGLOG( "TEXTURE: *ERROR* Failed saving jpeg: %s\n", szFilename );
			delete fileHandle;
			return(1);
		}
	}
	else
	{
		fileHandle = pFile;

		// get the start point of this save file
		fileHandle->subFile.nFileStartPos = static_cast<long>(file::FilePosition(fileHandle));
	}

	// image needs to be flipped vertically, so do it here 	
	Flip( );

	// attach error checker
	cinfo.err = jpeg_std_error(&pub);

	if( setjmp( setjmp_buffer ) ) 
	{
		jpeg_destroy_compress(&cinfo);
		if( pFile == 0 )
		{
			file::FileClose( fileHandle );
			delete fileHandle;
		}
		return(1);
	}

	jpeg_create_compress( &cinfo );
	jpeg_stdio_dest( &cinfo, fileHandle->fp );

	unsigned char *rgb_image = 0;
	
	// jpeg does not support alpha
	if( nBPP/8 == 4 )
	{
		// original image contains alpha, need to remove it
		rgb_image = new unsigned char[nWidth * nHeight * 3];

		for(unsigned int src = 0, dst = 0; src < nTextureSize; src +=4, dst +=3 )
		{
			// copy pixel data over
			rgb_image[dst]		= image[src];
			rgb_image[dst+1]	= image[src+1];
			rgb_image[dst+2]	= image[src+2];
		}

		// copy should be alright, free the old data and recreate
		delete[] image;
		image = 0;

		// recreate
		image = new unsigned char[nWidth * nHeight * 3];
		// copy new RGB image over
		std::memcpy( image, rgb_image, sizeof( unsigned char )*nWidth * nHeight * 3 );

		// delete the temp image
		delete[] rgb_image;
		rgb_image = 0;
	}

	// should have a valid saveable buffer now
	cinfo.image_width = nWidth; 	
	cinfo.image_height = nHeight;
	cinfo.input_components = 3;	
	cinfo.in_color_space = JCS_RGB; 	

	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, nQuality, true);

	jpeg_start_compress(&cinfo, true);

	// row stride (RGB)
	row_stride = nWidth * 3;

	// go through the image and save out
	while( cinfo.next_scanline < cinfo.image_height ) 
	{
		row_pointer[0] = & image[cinfo.next_scanline * row_stride];
		jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}

	// finish the compress structure
	jpeg_finish_compress(&cinfo);

	// close the file
	if( pFile == 0 )
	{
		file::FileClose( fileHandle );
		delete fileHandle;
	}
	else
	{
		// get the end point of this save file
		fileHandle->subFile.nFileEndPos = static_cast<long>(file::FilePosition(fileHandle));

		fileHandle->subFile.nFileLength = fileHandle->subFile.nFileEndPos - fileHandle->subFile.nFileStartPos;
	}

	jpeg_destroy_compress(&cinfo);

	DBGLOG( "TEXTURE: IMAGE %s Saved\t\t(w:%d, h:%d, alpha:%s)\n", szFilename, nWidth, nHeight,
			bHasAlpha?"YES":"NO" );

	return(0);
}

#endif // BASE_SUPPORT_JPEG

#ifdef BASE_SUPPORT_PCX

/////////////////////////////////////////////////////
/// Method: LoadPCX
/// Params: [in]szFilename
///
/////////////////////////////////////////////////////
int Texture::LoadPCX( const char *szFilename, file::TFileHandle* pFile )
{
	file::TFileHandle* fileHandle = 0;
	int i = 0;
	TPCXHeader PCXHeader;
	int nBitcount;

	// clear any old data
	this->Free();

	if( pFile == 0 )
	{
		fileHandle = new file::TFileHandle;

		if( core::IsEmptyString( szFilename ) )
		{
			delete fileHandle;
			return(1);
		}
		if( !file::FileOpen( szFilename, file::FILETYPE_BINARY_READ, fileHandle ) )
		{
			DBGLOG( "TEXTURE: *ERROR* Failed loading pcx: %s\n", szFilename );
			delete fileHandle;
			return(1);
		}

		// find length of file
		nFileSize = file::FileSize( fileHandle );

		file::CreateFileStructure( szFilename, &TextureFile );
	}
	else
	{
		fileHandle = pFile;
	}

	// read header file
	file::FileRead( &PCXHeader, sizeof(TPCXHeader), fileHandle );
	if( PCXHeader.manufacturer != 0x0a )
	{
		DBGLOG( "TEXTURE: *ERROR* bad version number (%i)\n", PCXHeader.manufacturer);
		// close the file
		if( pFile == 0 )
		{
			file::FileClose( fileHandle );
			delete fileHandle;
		}
		return(1);
	}

	//unsigned char manufacturer;
	//unsigned char version;
	//unsigned char encoding;
	//unsigned char bitsPerPixel;

	PCXHeader.xmin = static_cast<unsigned short>(core::LittleToBigEndianShort( PCXHeader.xmin ));
	PCXHeader.ymin = static_cast<unsigned short>(core::LittleToBigEndianShort( PCXHeader.ymin ));
	PCXHeader.xmax = static_cast<unsigned short>(core::LittleToBigEndianShort( PCXHeader.xmax ));
	PCXHeader.ymax = static_cast<unsigned short>(core::LittleToBigEndianShort( PCXHeader.ymax ));
	PCXHeader.horzRes = static_cast<unsigned short>(core::LittleToBigEndianShort( PCXHeader.horzRes ));
	PCXHeader.vertRes = static_cast<unsigned short>(core::LittleToBigEndianShort( PCXHeader.vertRes ));

	//unsigned char palette[48];
	//unsigned char reserved;
	//unsigned char numColourPlanes;

	PCXHeader.bytesPerScanLine = static_cast<unsigned short>(core::LittleToBigEndianShort( PCXHeader.bytesPerScanLine ));
	PCXHeader.paletteType = static_cast<unsigned short>(core::LittleToBigEndianShort( PCXHeader.paletteType ));
	PCXHeader.horzSize = static_cast<unsigned short>(core::LittleToBigEndianShort( PCXHeader.horzSize ));
	PCXHeader.vertSize = static_cast<unsigned short>(core::LittleToBigEndianShort( PCXHeader.vertSize ));

	//unsigned char padding[54];
		
	// initialize texture parameters
	eFileFormat	= TEXTURE_PCX;
	nWidth		= PCXHeader.xmax - PCXHeader.xmin + 1;
	nHeight		= PCXHeader.ymax - PCXHeader.ymin + 1;
	nFormat		= GL_RGB;
	nInternalFormat = GL_RGB8;
	nTextureType	= GL_UNSIGNED_BYTE;
	nBPP			= 24;
	nNumChannels	= 3;
	nOriginalWidth	= nWidth;
	nOriginalHeight	= nHeight;
	nMipMapCount	= 1;
	bHasMipMaps		= false;
	bIsCubemap		= false;
	bIsVolume		= false;

	// allocate memory							
	nTextureSize	= (nWidth * nHeight) * nNumChannels;	
	image			= new unsigned char[nTextureSize];

	nBitcount = PCXHeader.bitsPerPixel * PCXHeader.numColourPlanes;

	// read image data
	switch( nBitcount )
	{
		case 1:
		{
			// 1 bit colour index
			int y, bytes;
			int colourIndex;
			int rle_count = 0, rle_value = 0;
			unsigned char *ptr = image;

			for( y = 0; y < nHeight; ++y )
			{
				ptr = &image[(nHeight - (y + 1)) * nWidth * 3];
				bytes = PCXHeader.bytesPerScanLine;

				// decode line number y
				while( bytes-- )
				{
					if( rle_count == 0 )
					{
						if( ( rle_value = file::FileGetC(fileHandle) ) < 0xc0)
						{
							rle_count = 1;
						}
						else
						{
							rle_count = rle_value - 0xc0;
							rle_value = file::FileGetC(fileHandle);
						}
					}

					rle_count--;

					for( i = 7; i >= 0; --i, ptr += 3 )
					{
						colourIndex = ((rle_value & (1 << i)) > 0);

						ptr[0] = PCXHeader.palette[colourIndex * 3 + 0];
						ptr[1] = PCXHeader.palette[colourIndex * 3 + 1];
						ptr[2] = PCXHeader.palette[colourIndex * 3 + 2];
					}
				}
			}
		}break;

		case 4:
		{
			// 4 bits colour index
			unsigned char *colourIndex, *line;
			unsigned char *pLine, *ptr;
			int rle_count = 0, rle_value = 0;
			int x, y, c;
			int bytes;

			colourIndex = new unsigned char[nWidth];
			line = new unsigned char[PCXHeader.bytesPerScanLine];

			for (y = 0; y < nHeight; ++y)
			{
				ptr = &image[(nHeight - (y + 1)) * nWidth * 3];

				std::memset( colourIndex, 0, sizeof(unsigned char) * nWidth );

				for( c = 0; c < 4; ++c )
				{
					pLine = line;
					bytes = PCXHeader.bytesPerScanLine;

					// decode line number y 
					while( bytes-- )
					{
						if( rle_count == 0 )
						{
							if ( (rle_value = file::FileGetC(fileHandle)) < 0xc0)
							{
								rle_count = 1;
							}
							else
							{
								rle_count = rle_value - 0xc0;
								rle_value = file::FileGetC(fileHandle);
							}
						}

						rle_count--;
						*(pLine++) = rle_value;
					}

					// compute line's colour indexes
					for( x = 0; x < nWidth; ++x )
					{
						if (line[x / 8] & (128 >> (x % 8)))
						colourIndex[x] += (1 << c);
					}
				}

				// decode scan line.  colour index => rgb 
				for( x = 0; x < nWidth; ++x, ptr += 3 )
				{
					ptr[0] = PCXHeader.palette[colourIndex[x] * 3 + 0];
					ptr[1] = PCXHeader.palette[colourIndex[x] * 3 + 1];
					ptr[2] = PCXHeader.palette[colourIndex[x] * 3 + 2];
				}
			}

			// release memory
			if( colourIndex )
			{
				delete[] colourIndex;
				colourIndex = 0;
			}

			if( line )
			{
				delete[] line;
				line = 0;
			}
		}break;

		case 8:
		{
			// 8 bits colour index
			int rle_count = 0, rle_value = 0;
			unsigned char palette[768];
			unsigned char magic;
			unsigned char *ptr;
			fpos_t curpos;
			int y, bytes;

			// the palette is contained in the last 769 bytes of the file
			curpos = file::FilePosition( fileHandle );
			file::FileSeek( -769, file::FILESEEK_END, fileHandle );
			magic = file::FileGetC(fileHandle);

			// first byte must be equal to 0x0c (12)
			if( magic != 0x0c )
			{
				DBGLOG( "TEXTURE: *ERROR* colourmap's first byte must be 0x0c! (%#x)\n", magic );

				if( image )
				{
					delete[] image;
					image = 0;
				}

				// close the file
				if( pFile == 0 )
				{
					file::FileClose( fileHandle );
					delete fileHandle;
				}

				return(1);
			}

			// read palette
			file::FileRead( palette, sizeof(unsigned char), 768, fileHandle );
			fsetpos( fileHandle->fp, &curpos );

			// read pixel data
			for( y = 0; y < nHeight; ++y )
			{
				ptr = &image[(nHeight - (y + 1)) * nWidth * 3];
				bytes = PCXHeader.bytesPerScanLine;

				// decode line number y
				while( bytes-- )
				{
					if( rle_count == 0 )
					{
						if( (rle_value = file::FileGetC(fileHandle)) < 0xc0)
						{
							rle_count = 1;
						}
						else
						{
							rle_count = rle_value - 0xc0;
							rle_value = file::FileGetC(fileHandle);
						}
					}

					rle_count--;

					ptr[0] = palette[rle_value * 3 + 0];
					ptr[1] = palette[rle_value * 3 + 1];
					ptr[2] = palette[rle_value * 3 + 2];
					ptr += 3;
				}
			}
		}break;

		case 24:
		{
			// 24 bits
			unsigned char *ptr = image;
			int rle_count = 0, rle_value = 0;
			int y, c;
			int bytes;

			for (y = 0; y < nHeight; ++y)
			{
				// for each colour plane
				for( c = 0; c < 3; ++c )
				{
					ptr = &image[(nHeight - (y + 1)) * nWidth * 3];
					bytes = PCXHeader.bytesPerScanLine;

					// decode line number y
					while( bytes-- )
					{
						if( rle_count == 0 )
						{
							if( (rle_value = file::FileGetC(fileHandle)) < 0xc0)
							{
								rle_count = 1;
							}
							else
							{
								rle_count = rle_value - 0xc0;
								rle_value = file::FileGetC(fileHandle);
							}
						}

						rle_count--;
						ptr[c] = (unsigned char)rle_value;
						ptr += 3;
					}
				}
			}
		}break;

		default:
			// unsupported
			DBGLOG( "TEXTURE: *ERROR* unknown %i bitcount pcx files\n", nBitcount );

			if(image)
			{
				delete[] image;
				image = 0;
			}

			// close the file
			if( pFile == 0 )
			{
				file::FileClose( fileHandle );
				delete fileHandle;
			}

			return(1);
		break;
	}

	// close the file
	if( pFile == 0 )
	{
		file::FileClose( fileHandle );
		delete fileHandle;
	}

	//DBGLOG( "TEXTURE: IMAGE %s Loaded\t\t(w:%d, h:%d, bpp:%d, ch:%d, file_size:%d, image_size:%d, alpha:%s, compressed:%s)\n", 
	//	szFilename, nWidth, nHeight, nBPP, nNumChannels, nFileSize, 
	//	nTextureSize, bHasAlpha?"YES":"NO", bCompressed?"YES":"NO" );

	return(0);
}

#endif // BASE_SUPPORT_PCX

#ifdef BASE_SUPPORT_PVR
	/////////////////////////////////////////////////////
	/// Method: LoadPVR
	/// Params: [in]szFilename, [in]pFile
	/// requires the PowerVR PVRTexLib library
	/////////////////////////////////////////////////////
	int Texture::LoadPVR( const char *szFilename, file::TFileHandle* pFile )
	{
		// clear any old data
		this->Free();

/*		if( pFile == 0 )
		{
			pvrtexlib::PVRTextureUtilities* pvrTexUtil = pvrtexlib::PVRTextureUtilities::getPointer();

			pvrtexlib::CPVRTexture origTexture(szFilename);

			pvrtexlib::CPVRTexture decompressedTexture;

			pvrTexUtil->DecompressPVR( origTexture, decompressedTexture );

			eFileFormat		= TEXTURE_PVR;

			nWidth			= decompressedTexture.getWidth();
			nHeight			= decompressedTexture.getHeight();

			nMipMapCount	= decompressedTexture.getMipMapCount();
			bHasMipMaps		= decompressedTexture.hasMips();

			bIsCubemap		= decompressedTexture.isCubeMap();
			bIsVolume		= decompressedTexture.isVolume();

			return(0);
		}
*/
		static char PVRTexIdentifier[5]			= "PVR!";
		file::TFileHandle* fileHandle = 0;
		TPVRHeader PVRHeader;
		unsigned int flags, pvrTag;
		unsigned int formatFlags;
	
		unsigned int dataLength = 0, dataOffset = 0, dataSize = 0;
		unsigned int blockSize = 0, widthBlocks = 0, heightBlocks = 0;
		unsigned int width = 0, height = 0;

		if( pFile == 0 )
		{
			fileHandle = new file::TFileHandle;

			if( core::IsEmptyString( szFilename ) )
			{
				delete fileHandle;
				return(1);
			}
			if( !file::FileOpen( szFilename, file::FILETYPE_BINARY_READ, fileHandle ) )
			{
				DBGLOG( "TEXTURE: *ERROR* failed loading tga: %s\n", szFilename );
				delete fileHandle;
				return(1);
			}

			// find length of file
			nFileSize = file::FileSize( fileHandle );

			file::CreateFileStructure( szFilename, &TextureFile );
		}
		else
		{
			fileHandle = pFile;
		}

		// clear and read the header
		std::memset( &PVRHeader, 0, sizeof(TPVRHeader) );
		if( file::FileRead( &PVRHeader, sizeof(TPVRHeader), fileHandle ) == 0 )				
		{
			DBGLOG( "TEXTURE: *ERROR* could not read pvr file header\n" );	
			if( pFile == 0 )
			{
				file::FileClose( fileHandle );
				delete fileHandle;
			}											
			return(1);												
		}

		// is it a valid pvr file
		pvrTag = core::EndianSwapInt(PVRHeader.pvrTag, core::MACHINE_LITTLE_ENDIAN);

		if (PVRTexIdentifier[0] != ((pvrTag >>  0) & 0xff) ||
			PVRTexIdentifier[1] != ((pvrTag >>  8) & 0xff) ||
			PVRTexIdentifier[2] != ((pvrTag >> 16) & 0xff) ||
			PVRTexIdentifier[3] != ((pvrTag >> 24) & 0xff))
		{
            
			DBGLOG( "TEXTURE: *ERROR* not a valid pvr file\n" );
			return(1);
		}

		flags = core::EndianSwapInt(PVRHeader.flags, core::MACHINE_LITTLE_ENDIAN);
		formatFlags = flags & PVR_TEXTURE_FLAG_TYPE_MASK;

		if( formatFlags != PVRTextureFlagTypePVRTC_2 && 
			formatFlags != PVRTextureFlagTypePVRTC_4 )
		{
			DBGLOG( "TEXTURE: *ERROR* unsupported pvr bit depth\n" );
			return(1);
		}
		
		if( PVRHeader.width != PVRHeader.height )
		{
			DBGLOG( "TEXTURE: *ERROR* non-square pvr texture %s\n", szFilename );
			return(1);			
		}
		
		// grab the data
		eFileFormat	= TEXTURE_PVR;
		bCompressed = true;

		
		nWidth = core::EndianSwapInt(PVRHeader.width, core::MACHINE_LITTLE_ENDIAN);
		nHeight = core::EndianSwapInt(PVRHeader.height, core::MACHINE_LITTLE_ENDIAN);

		if( core::EndianSwapInt(PVRHeader.bitmaskAlpha, core::MACHINE_LITTLE_ENDIAN) )
			bHasAlpha = true;
		else
			bHasAlpha = false;

		if (formatFlags == PVRTextureFlagTypePVRTC_2)
		{
			nBPP = 2;
			if(bHasAlpha)
				nInternalFormat = GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;
			else
				nInternalFormat = GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG;
		}
		else if (formatFlags == PVRTextureFlagTypePVRTC_4)
		{
			nBPP = 4;
			
			if(bHasAlpha)
				nInternalFormat = GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
			else	
				nInternalFormat = GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG;
		}
		
		bHasMipMaps = false;
		nMipMapCount = core::EndianSwapInt(PVRHeader.numMipmaps, core::MACHINE_LITTLE_ENDIAN);
		if( nMipMapCount )
			bHasMipMaps = true;

		nMipMapCount+=1;

		// allocate the image list
		imageList = new unsigned char*[nMipMapCount];

		// how big is the data
		dataLength = core::EndianSwapInt(PVRHeader.dataLength, core::MACHINE_LITTLE_ENDIAN);
	
		// get the data start point
		width = nWidth;
		height = nHeight;
		int mipIndex = 0;

		while (dataOffset < dataLength)
		{
			if (formatFlags == PVRTextureFlagTypePVRTC_2)
			{
				blockSize = 8 * 4; // Pixel by pixel block size for 2bpp
				widthBlocks = width / 8;
				heightBlocks = height / 4;
				nBPP = 2;
			}
			else if (formatFlags == PVRTextureFlagTypePVRTC_4)
			{
				blockSize = 4 * 4; // Pixel by pixel block size for 4bpp
				widthBlocks = width / 4;
				heightBlocks = height / 4;
				nBPP = 4;
			}
			
			// Clamp to minimum number of blocks
			if (widthBlocks < 2)
				widthBlocks = 2;
			if (heightBlocks < 2)
				heightBlocks = 2;

			dataSize = widthBlocks * heightBlocks * ((blockSize  * nBPP) / 8);
			
			imageList[mipIndex] = new unsigned char[dataSize];
			file::FileRead( imageList[mipIndex], dataSize, fileHandle );

			dataOffset += dataSize;
			
			width = std::max( (unsigned int)width >> 1, (unsigned int)1 );
			height = std::max( (unsigned int)height >> 1, (unsigned int)1 );

			mipIndex++;
		}

		// close the file
		if( pFile == 0 )
		{
			file::FileClose( fileHandle );
			delete fileHandle;
		}

		return(0);
	}

/////////////////////////////////////////////////////
/// Method: SavePVR
/// Params: [in]szFilename
///
/////////////////////////////////////////////////////
int Texture::SavePVR( const char *szFilename, file::TFileHandle* pFile )
{
/*	file::TFileHandle* fileHandle = 0;
	int i = 0;

	if( pFile == 0 )
	{
		fileHandle = new file::TFileHandle;

		if( core::IsEmptyString( szFilename ) )
		{
			delete fileHandle;
			return(1);
		}
		if( !file::FileOpen( szFilename, file::FILETYPE_BINARY_WRITE, fileHandle ) )
		{
			DBGLOG( "TEXTURE: *ERROR* Failed saving png: %s\n", szFilename );
			delete fileHandle;
			return(1);
		}
	}
	else
	{
		fileHandle = pFile;
		// get the start point of this save file
		fileHandle->subFile.nFileStartPos = static_cast<long>(file::FilePosition(fileHandle));
	}

    // create the pvr
	pvrtexlib::PVRTextureUtilities* pvrTexUtil = pvrtexlib::PVRTextureUtilities::getPointer();

	pvrtexlib::PixelType pixelType = pvrtexlib::OGL_RGB_888;
	switch( nInternalFormat )
	{
		case GL_RGB8:
		{
			pixelType = pvrtexlib::OGL_RGB_888;
		}break;
		case GL_RGBA8:
		{
			pixelType = pvrtexlib::OGL_RGBA_8888;
		}break;
	}

	pvrtexlib::CPVRTexture origTexture(	nWidth,		// const unsigned int u32Width,	// create header info for raw data
								nHeight,	// const unsigned int	u32Height,
								0,			// const unsigned int	u32MipMapCount,
								1,			// const unsigned int	u32NumSurfaces,
								false,		// const bool			bBorder,
								false,		// const bool			bTwiddled,
								false,		// const bool			bCubeMap,
								false,		// const bool			bVolume,
								false,		// const bool			bFalseMips,
								bHasAlpha,	// const bool			bAlpha,
								false,		// const bool			bFlipped,
								pixelType,	// const PixelType		ePixelType,
								0.0f,		// const float			fNormalMap,
								image		// uint8				*pPixelData
								);

	// create a header
	pvrtexlib::CPVRTextureHeader processHeader( origTexture.getHeader() );

	// setup some preferences for the compressed file
	processHeader.setMipMapCount(8);
	processHeader.setFalseMips(true);

	pvrTexUtil->ProcessRawPVR( origTexture, processHeader );

	pvrtexlib::CPVRTexture compressedTexture( origTexture.getHeader() );
	compressedTexture.setPixelType( pvrtexlib::OGL_PVRTC4 ); // 4bit

	// compress to PVR
	pvrTexUtil->CompressPVR( origTexture, compressedTexture );

	// write to the file
	compressedTexture.getHeader().writeToFile(fileHandle->fp);
	compressedTexture.getData().writeToFile(fileHandle->fp);

	///// TEST /////
	static int j=0;
	char theFile[256];
	snprintf( theFile, 256, "F:\\test_%d.pvr", j );
	compressedTexture.writeToFile( theFile );
	j++;
	////////////////

	if( pFile == 0 )
	{
		file::FileClose( fileHandle );
		delete fileHandle;
	}
	else
	{
		// get the end point of this save file
		fileHandle->subFile.nFileEndPos = static_cast<long>(file::FilePosition(fileHandle));

		fileHandle->subFile.nFileLength = fileHandle->subFile.nFileEndPos - fileHandle->subFile.nFileStartPos;
	}

	DBGLOG( "TEXTURE: IMAGE %s Saved\t\t(w:%d, h:%d, alpha:%s)\n", szFilename, nWidth, nHeight,
			bHasAlpha?"YES":"NO" );
*/
    return(0);
}
#endif // BASE_SUPPORT_PVR

/////////////////////////////////////////////////////
/// Method: Resize
/// Params: [in]bStretch
///
/////////////////////////////////////////////////////
void Texture::Resize( bool bStretch )
{
	int nSrc = 0, nDst = 0;
	int nOffset = 0;

	int nIsHeightPOW2 = IsPowerOfTwo( nHeight );
	int nIsWidthPOW2 = IsPowerOfTwo( nWidth );

	bResized = true;

	int maxTextureSize = 1024;
	glGetIntegerv( GL_MAX_TEXTURE_SIZE, &maxTextureSize );

	// get new size
	if( !nIsHeightPOW2 )
		nHeight = GetNextPowerOfTwo( nHeight, /*renderer::OpenGL::GetInstance()->GetMaxTextureSize()*/maxTextureSize );
	if( !nIsWidthPOW2 )
		nWidth = GetNextPowerOfTwo( nWidth, /*renderer::OpenGL::GetInstance()->GetMaxTextureSize()*/maxTextureSize );

	unsigned char *newImage = 0;
	newImage = new unsigned char[(nWidth*nHeight)*nNumChannels];

	std::memset( newImage, 0, sizeof(unsigned char)*((nWidth*nHeight)*nNumChannels) );
					
	// normal model texture, allow it to stretch and maintain any texture coords
	/*if( bStretch )
	{						
		// try a resize
		gluScaleImage( nFormat, nOriginalWidth, nOriginalHeight, nTextureType, image,
										nWidth, nHeight, GL_UNSIGNED_BYTE, newImage );
	}
	else*/
	{
		// 2D images do not get stretched
						
		// manually copy each row of pixels over
		for( nSrc = 0, nDst = 0; nSrc < ((nOriginalWidth*nOriginalHeight) * nNumChannels); nSrc += nNumChannels )
		{
			// end of original row
			if( nOffset == (nOriginalWidth*nNumChannels) )
			{
				nDst += (nWidth - nOriginalWidth) * nNumChannels;
				nOffset = 0;
			}

			// copy data based on the channel counts
			if( nNumChannels == 1 )
			{
				newImage[nDst] = image[nSrc];
			}
			else if( nNumChannels == 2 )
			{
				newImage[nDst] = image[nSrc];
				newImage[nDst+1] = image[nSrc+1];
			}
			else if( nNumChannels == 3 )
			{
				newImage[nDst] = image[nSrc];
				newImage[nDst+1] = image[nSrc+1];
				newImage[nDst+2] = image[nSrc+2];
			}
			else if( nNumChannels == 4 )
			{
				newImage[nDst] = image[nSrc];
				newImage[nDst+1] = image[nSrc+1];
				newImage[nDst+2] = image[nSrc+2];
				newImage[nDst+3] = image[nSrc+3];
			}

			nOffset += nNumChannels;
			nDst += nNumChannels;
		}
	}

	// delete old image
	delete[] image;
	image = 0;

	// copy the new one
	image = new unsigned char[ (nWidth*nHeight) * nNumChannels];
	std::memcpy( image, newImage, (nWidth*nHeight) * nNumChannels );

	// delete temp holding image
	delete[] newImage;
	newImage = 0;

	// new size update
	nTextureSize = (nWidth*nHeight) * nNumChannels;
}

/////////////////////////////////////////////////////
/// Method: Flip
/// Params: None
///
/////////////////////////////////////////////////////
void Texture::Flip( void )
{
	int i = 0;

	// DDS file may have compression and mipmaps, so need to do the flip in a different way
#ifdef BASE_SUPPORT_DDS	
	unsigned int blockSize = 0;
	unsigned int lineSize = 0;
	
	void (*flipblocks)( unsigned char*, unsigned int );

	if( eFileFormat == TEXTURE_DDS )
	{
		if( !imageList )
			return;

		if( nFormat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT ||
			nFormat == GL_COMPRESSED_RGBA_S3TC_DXT3_EXT || 
			nFormat == GL_COMPRESSED_RGBA_S3TC_DXT5_EXT )
		{
			for( int face = 0; face < ((nCubemapFaceCount) ? nCubemapFaceCount : 1); face++ ) 
			{
				int w = nWidth;
				int h = nHeight;
				int d = (nDepth) ? nDepth : 1;
					
				for( int level = 0; level < nMipMapCount; level++ ) 
				{
					int bw = (bDXTCompressed) ? (w+3)/4 : w;
					int bh = (bDXTCompressed) ? (h+3)/4 : h;

	//				imageList[ level + (face*nMipMapCount) ] = new unsigned char[size];
	//				fread( imageList[ level + (face*nMipMapCount) ], size, 1, fp );

					switch( nFormat )
					{
						case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT: 
						{
							blockSize = 8;
							flipblocks = &FlipTexture_DXTC1; 
						}break;
						case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT: 
						{
							blockSize = 16;
							flipblocks = &FlipTexture_DXTC3; 
						}break;
						case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
						{
							blockSize = 16;
							flipblocks = &FlipTexture_DXTC5; 
						}break;
						default:
							return;
					}

					lineSize = bw * blockSize;
					unsigned char *tempBuf = new unsigned char[lineSize];

					unsigned char *top = imageList[ level + (face*nMipMapCount) ];
					unsigned char *bottom = imageList[ level + (face*nMipMapCount) ] + (bh-1) * lineSize;

					for( unsigned int j = 0; j < std::max( (unsigned int)bh >> 1, (unsigned int)1); j++ )
					{
						if (top == bottom)
						{
							flipblocks(top, bw);
							break;
						}

						flipblocks(top, bw);
						flipblocks(bottom, bw);

						std::memcpy( tempBuf, top, lineSize);
						std::memcpy( top, bottom, lineSize);
						std::memcpy( bottom, tempBuf, lineSize);

						top += lineSize;
						bottom -= lineSize;
					}

					delete []tempBuf;
					tempBuf = 0;

					// reduce mip sizes
					w = ( w > 1 ) ? w >> 1 : 1;
					h = ( h > 1 ) ? h >> 1 : 1;
					d = ( d > 1 ) ? d >> 1 : 1;
				}
			}
		}
		else	
		{
			// normal non-compressed
			for( int face = 0; face < ((nCubemapFaceCount) ? nCubemapFaceCount : 1); face++ ) 
			{
				int w = nWidth;
				int h = nHeight;
				int d = (nDepth) ? nDepth : 1;
					
				for( int level = 0; level < nMipMapCount; level++ ) 
				{
					int bw = (bDXTCompressed) ? (w+3)/4 : w;
					int bh = (bDXTCompressed) ? (h+3)/4 : h;

					lineSize = nBytesPerElement * bw;
					unsigned int sliceSize = lineSize * bh;

					unsigned char *tempBuf = new unsigned char[lineSize];

					for ( int ii = 0; ii < d; ii++) 
					{
						unsigned char *top = imageList[ level + (face*nMipMapCount) ] + ii*sliceSize;
						unsigned char *bottom = top + (sliceSize - lineSize);
			    
						for ( int jj = 0; jj < (bh >> 1); jj++) 
						{
							std::memcpy( tempBuf, top, lineSize );
							std::memcpy( top, bottom, lineSize );
							std::memcpy( bottom, tempBuf, lineSize );

							top += lineSize;
							bottom -= lineSize;
						}
					}

					delete[] tempBuf;
					tempBuf = 0;

					// reduce mip sizes
					w = ( w > 1 ) ? w >> 1 : 1;
					h = ( h > 1 ) ? h >> 1 : 1;
					d = ( d > 1 ) ? d >> 1 : 1;
				}
			}
		}
		
	}
	else
#endif // BASE_SUPPORT_DDS		
	{
		if( !image )
			return;

		// image needs to be flipped vertically, so do it here 	
		// dont flip zero or 1 height images
		if( nHeight != 0 || nHeight != 1 )
		{
			int rowsToSwap = 0;
			//see how many rows to swap
			if(nHeight%2 == 1)
				rowsToSwap = (nHeight-1)/2;
			else
				rowsToSwap = nHeight/2;

			// create space for a temporary row
			unsigned char *tempRow = new unsigned char[nWidth*(nBPP/8)];
			if( tempRow == 0 )
			{
				DBGLOG( "TEXTURE: Error, unable to flip image, unable to create space for temporary row\n" );
				return;
			}

			// loop through rows to swap
			for( i = 0; i < rowsToSwap; ++i )
			{
				//copy row i into temp
				std::memcpy(tempRow, &image[i*nWidth*(nBPP/8)], nWidth*(nBPP/8));
				//copy row height-i-1 to row i
				std::memcpy(&image[i*nWidth*(nBPP/8)], &image[(nHeight-i-1)*nWidth*(nBPP/8)], nWidth*(nBPP/8));
				//copy temp into row height-i-1
				std::memcpy(&image[(nHeight-i-1)*nWidth*(nBPP/8)], tempRow, nWidth*(nBPP/8));
			}

			// free tempRow
			if(tempRow)
			{
				delete[] tempRow;
				tempRow = 0;
			}
		}
	}
}

/////////////////////////////////////////////////////
/// Method: Free
/// Params: None
///
/////////////////////////////////////////////////////
int Texture::Free( void )
{
	int i = 0;
	int nMipCount = 0;

	if( bIsCubemap )
		nMipCount = nMipMapCount*nCubemapFaceCount;
	else
		nMipCount = nMipMapCount;

	if( imageList )
	{
		for( i = 0; i < nMipCount; i++ )
		{
			if( imageList[i] )
			{
				delete[] imageList[i];
				imageList[i] = 0;
			}
		}

		// clear the list
		delete[] imageList;
		imageList = 0;
	}

	if( image )
	{
		delete[] image;
		image = 0;
	}
	
	return(0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



