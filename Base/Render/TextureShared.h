
#ifndef __TEXTURESHARED_H__
#define __TEXTURESHARED_H__

// forward declare
namespace renderer { class Texture; }

namespace renderer
{
	///////////////////////////// T G A /////////////////////////////

	#pragma pack(push, 1)
		/// tga header
		struct TTGAHeader
		{
			unsigned char	IDLength;			// size of image id
			unsigned char	ColourmapType;		// 1 is has a colourmap
			unsigned char	ImageType;			// compression type

			short			Cm_first_entry;		// colourmap origin
			short			Cm_length;			// colourmap length
			unsigned char	Cm_size;			// colourmap size

			short			nXOrigin;			// bottom left x coord origin
			short			nYOrigin;			// bottom left y coord origin

			short			nWidth;				// picture width (in pixels)
			short			nHeight;			// picture height (in pixels)

			unsigned char  PixelDepth;			// bits per pixel: 8, 16, 24 or 32
			unsigned char  ImageDescriptor;		// 24 bits = 0x00; 32 bits = 0x80

		};
	#pragma pack(pop)

	/////////////////////////////////////////////////////////////////

	///////////////////////////// B M P /////////////////////////////

#ifdef BASE_SUPPORT_BMP
	#pragma pack(push, 2)
		/// bitmap file header
		struct TBMPFileHeader
		{
			unsigned char bfType[2];      // magic number "BM" (0x4D42)
			unsigned int bfSize;          // file size
			unsigned short bfReserved1;
			unsigned short bfReserved2;
			unsigned int bfOffBits;       // offset to image data

		};
	#pragma pack(pop)

	/// bitmap info header
	struct TBMPInfoHeader
	{
		unsigned int biSize;          // size of bitmap info header
		int biWidth;                  // image with
		int biHeight;                 // image height
		unsigned short biPlanes;      // must be equal to 1
		unsigned short biBitCount;    // bits per pixels
		unsigned int biCompression;   // compression type
		unsigned int biSizeImage;     // size of pixel data
		int biXPelsPerMeter;          // pixels per meter on x-axis
		int biYPelsPerMeter;          // pixels per meter on y-axis
		unsigned int biClrUsed;       // number of used colours
		unsigned int biClrImportant;  // number of important colours

	};

	/// bitmap core header
	struct TBMPCoreHeader
	{
		unsigned int bcSize;           // size of bitmap core header
		unsigned short bcWidth;        // image with
		unsigned short bcHeight;       // image height
		unsigned short bcPlanes;       // must be equal to 1
		unsigned short bcBitCount;     // bits per pixel

	};

	/// bitmap core info
	struct TBMPCoreInfo
	{
		TBMPCoreHeader bmciHeader;
		unsigned char bmciColours[3];
	};

	/// OS-style type
	enum EBMPOSType
	{
		BMP_WIN,
		BMP_OS2
	};

	enum EBMPRLEType
	{
		/// RLE byte type constants
		RLE_COMMAND     = 0,
		RLE_ENDOFLINE   = 0,
		RLE_ENDOFBITMAP = 1,
		RLE_DELTA       = 2
	};
#endif // BASE_SUPPORT_BMP

	/////////////////////////////////////////////////////////////////

	///////////////////////////// P C X /////////////////////////////

#ifdef BASE_SUPPORT_PCX

	#pragma pack(push, 1)
		/// pcx header 
		struct TPCXHeader
		{
			unsigned char manufacturer;
			unsigned char version;
			unsigned char encoding;
			unsigned char bitsPerPixel;

			unsigned short xmin, ymin;
			unsigned short xmax, ymax;
			unsigned short horzRes, vertRes;

			unsigned char palette[48];
			unsigned char reserved;
			unsigned char numColourPlanes;

			unsigned short bytesPerScanLine;
			unsigned short paletteType;
			unsigned short horzSize, vertSize;

			unsigned char padding[54];

		};
	#pragma pack(pop)

#endif // BASE_SUPPORT_PCX

	/////////////////////////////////////////////////////////////////

	///////////////////////////// D D S /////////////////////////////
	
#ifdef BASE_SUPPORT_DDS

	// surface description flags
	const unsigned long DDSF_CAPS			= 0x00000001l;
	const unsigned long DDSF_HEIGHT			= 0x00000002l;
	const unsigned long DDSF_WIDTH			= 0x00000004l;
	const unsigned long DDSF_PITCH			= 0x00000008l;
	const unsigned long DDSF_PIXELFORMAT	= 0x00001000l;
	const unsigned long DDSF_MIPMAPCOUNT	= 0x00020000l;
	const unsigned long DDSF_LINEARSIZE		= 0x00080000l;
	const unsigned long DDSF_DEPTH			= 0x00800000l;

	// pixel format flags
	const unsigned long DDSF_ALPHAPIXELS	= 0x00000001l;
	const unsigned long DDSF_FOURCC			= 0x00000004l;
	const unsigned long DDSF_RGB			= 0x00000040l;
	const unsigned long DDSF_RGBA			= 0x00000041l;

	// dwCaps1 flags
	const unsigned long DDSF_COMPLEX		= 0x00000008l;
	const unsigned long DDSF_TEXTURE		= 0x00001000l;
	const unsigned long DDSF_MIPMAP			= 0x00400000l;

	// dwCaps2 flags
	const unsigned long DDSF_CUBEMAP			= 0x00000200l;
	const unsigned long DDSF_CUBEMAP_POSITIVEX	= 0x00000400l;
	const unsigned long DDSF_CUBEMAP_NEGATIVEX	= 0x00000800l;
	const unsigned long DDSF_CUBEMAP_POSITIVEY	= 0x00001000l;
	const unsigned long DDSF_CUBEMAP_NEGATIVEY	= 0x00002000l;
	const unsigned long DDSF_CUBEMAP_POSITIVEZ	= 0x00004000l;
	const unsigned long DDSF_CUBEMAP_NEGATIVEZ	= 0x00008000l;
	const unsigned long DDSF_CUBEMAP_ALL_FACES	= 0x0000FC00l;
	const unsigned long DDSF_VOLUME				= 0x00200000l;

	// compressed texture types
	const unsigned long FOURCC_UNKNOWN		= 0;

	const unsigned long FOURCC_R8G8B8		= 20;
	const unsigned long FOURCC_A8R8G8B8		= 21;
	const unsigned long FOURCC_X8R8G8B8		= 22;
	const unsigned long FOURCC_R5G6B5		= 23;
	const unsigned long FOURCC_X1R5G5B5		= 24;
	const unsigned long FOURCC_A1R5G5B5		= 25;
	const unsigned long FOURCC_A4R4G4B4		= 26;
	const unsigned long FOURCC_R3G3B2		= 27;
	const unsigned long FOURCC_A8			= 28;
	const unsigned long FOURCC_A8R3G3B2		= 29;
	const unsigned long FOURCC_X4R4G4B4		= 30;
	const unsigned long FOURCC_A2B10G10R10	= 31;
	const unsigned long FOURCC_A8B8G8R8		= 32;
	const unsigned long FOURCC_X8B8G8R8		= 33;
	const unsigned long FOURCC_G16R16		= 34;
	const unsigned long FOURCC_A2R10G10B10	= 35;
	const unsigned long FOURCC_A16B16G16R16	= 36;

	const unsigned long FOURCC_L8			= 50;
	const unsigned long FOURCC_A8L8			= 51;
	const unsigned long FOURCC_A4L4			= 52;
	const unsigned long FOURCC_DXT1			= 0x31545844l; //(MAKEFOURCC('D','X','T','1'))
	const unsigned long FOURCC_DXT2			= 0x32545844l; //(MAKEFOURCC('D','X','T','1'))
	const unsigned long FOURCC_DXT3			= 0x33545844l; //(MAKEFOURCC('D','X','T','3'))
	const unsigned long FOURCC_DXT4			= 0x34545844l; //(MAKEFOURCC('D','X','T','3'))
	const unsigned long FOURCC_DXT5			= 0x35545844l; //(MAKEFOURCC('D','X','T','5'))

	const unsigned long FOURCC_D16_LOCKABLE	= 70;
	const unsigned long FOURCC_D32			= 71;
	const unsigned long FOURCC_D24X8		= 77;
	const unsigned long FOURCC_D16			= 80;

	const unsigned long FOURCC_D32F_LOCKABLE	= 82;

	const unsigned long FOURCC_L16			= 81;

	// Floating point surface formats

	// s10e5 formats (16-bits per channel)
	const unsigned long FOURCC_R16F				= 111;
	const unsigned long FOURCC_G16R16F			= 112;
	const unsigned long FOURCC_A16B16G16R16F	= 113;

	// IEEE s23e8 formats (32-bits per channel)
	const unsigned long FOURCC_R32F				= 114;
	const unsigned long FOURCC_G32R32F			= 115;
	const unsigned long FOURCC_A32B32G32R32F	= 116;

	struct TDXTColBlock
	{
		unsigned short col0;
		unsigned short col1;

		unsigned char row[4];
	};

	struct TDXT3AlphaBlock
	{
		unsigned short row[4];
	};

	struct TDXT5AlphaBlock
	{
		unsigned char alpha0;
		unsigned char alpha1;
		    
		unsigned char row[6];
	};

	struct TDDSPixelFormat
	{
		unsigned long dwSize;
		unsigned long dwFlags;
		unsigned long dwFourCC;
		unsigned long dwRGBBitCount;
		unsigned long dwRBitMask;
		unsigned long dwGBitMask;
		unsigned long dwBBitMask;
		unsigned long dwABitMask;
	};

	struct TDDSHeader
	{
		unsigned long dwSize;
		unsigned long dwFlags;
		unsigned long dwHeight;
		unsigned long dwWidth;
		unsigned long dwPitchOrLinearSize;
		unsigned long dwDepth;
		unsigned long dwMipMapCount;
		unsigned long dwReserved1[11];
		TDDSPixelFormat ddspf;
		unsigned long dwCaps1;
		unsigned long dwCaps2;
		unsigned long dwReserved2[3];
	};

#endif // BASE_SUPPORT_DDS

#ifdef BASE_SUPPORT_PVR

	const unsigned int PVR_TEXTURE_FLAG_TYPE_MASK	= 0xff;
	const int PVRTextureFlagTypePVRTC_2		= 24;
	const int PVRTextureFlagTypePVRTC_4		= 25;

	struct TPVRHeader
	{
		unsigned int headerLength;
		unsigned int height;
		unsigned int width;
		unsigned int numMipmaps;
		unsigned int flags;
		unsigned int dataLength;
		unsigned int bpp;
		unsigned int bitmaskRed;
		unsigned int bitmaskGreen;
		unsigned int bitmaskBlue;
		unsigned int bitmaskAlpha;
		unsigned int pvrTag;
		unsigned int numSurfs;
	};
#endif // BASE_SUPPORT_PVR

	/////////////////////////////////////////////////////////////////

	enum ETextureFormat
	{
		TEXTURE_UNKNOWN=0,
		
		TEXTURE_BMP=1,
		TEXTURE_TGA=2,
		TEXTURE_PNG=3,
		TEXTURE_DDS=4,
		TEXTURE_JPEG=5,
		TEXTURE_PCX=6,
		TEXTURE_PVR=7,

		TEXTURE_MISSING_UNSUPPORTED=9999,
	};

	/// IsPowerOfTwo - Checks to see if the value passed is a valid power of two number
	/// \param nValue - value to check
	/// \return boolean - ( SUCCESS: true or FAIL: false )
	bool IsPowerOfTwo( int nValue );
	/// GetPreviousPowerOfTwo - Gets the previous valid power of two number from the value passed to the function
	/// \param nValue - value to get previous value from
	/// \param nMaxSize - max value allowed (should be a driver limit)
	/// \return integer - Previous power of two number
	int GetPreviousPowerOfTwo( int nValue, int nMaxSize );
	/// GetNextPowerOfTwo - Gets the next valid power of two number from the value passed to the function
	/// \param nValue - value to get next value from
	/// \param nMaxSize - max value allowed (should be a driver limit)
	/// \return integer - Next power of two number
	int GetNextPowerOfTwo( int nValue, int nMaxSize );

	/// GetUVCoord - Gets a UV coord from a pixel position in the texture
	/// \param nPixelCoord - pixel point in the texture
	/// \param nTextureSize - the size of the texture
	/// \return float - the uv coord
	float GetUVCoord( int nPixelCoord, int nTextureSize );

#ifdef BASE_SUPPORT_DDS
	/// FlipTexture_DXTC1 - 
	/// \param ptr - image pointer
	/// \param numBlocks - block size
	void FlipTexture_DXTC1( unsigned char *ptr, unsigned int numBlocks );
	/// FlipTexture_DXTC3 - 
	/// \param ptr - image pointer
	/// \param numBlocks - block size
	void FlipTexture_DXTC3( unsigned char *ptr, unsigned int numBlocks );
	/// Flip_DXT5_Alpha - 
	/// \param block - alpha block pointer
	void FlipTexture_DXT5_Alpha( TDXT5AlphaBlock *block );
	/// FlipTexture_DXTC5 - 
	/// \param ptr - image pointer
	/// \param numBlocks - block size
	void FlipTexture_DXTC5( unsigned char *ptr, unsigned int numBlocks );
#endif // BASE_SUPPORT_DDS

} // namespace renderer

#endif // __TEXTURESHARED_H__

