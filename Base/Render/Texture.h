
#ifndef __TEXTURE_H__
#define __TEXTURE_H__

// forward declare
namespace file { struct TFile; }
namespace renderer { enum ETextureFormat; }

#define TGA_IMAGE_TYPE_NONE          0 /* no image data */
#define TGA_IMAGE_TYPE_COLORMAP      1 /* uncompressed, color-mapped */
#define TGA_IMAGE_TYPE_BGR           2 /* uncompressed, true-color */
#define TGA_IMAGE_TYPE_MONO          3 /* uncompressed, black and white */
#define TGA_IMAGE_TYPE_COLORMAP_RLE  9 /* run-length, color-mapped */
#define TGA_IMAGE_TYPE_BGR_RLE      10 /* run-length, true-color */
#define TGA_IMAGE_TYPE_MONO_RLE     11 /* run-length, black and white */

namespace renderer
{
	class Texture
	{
		public:
			/// default constructor
			Texture();
			/// default destructor
			~Texture();

			/// OPERATOR 'ASSIGN' - Assigned the data of parameters texture to 'this' texture
			/// \param t - texture to assign
			/// \return Texture ( SUCCESS: 'this' texture )
			Texture			&operator =  (const Texture &t);

			/// Load - Loads a texture, will try and load correctly based of its file extension
			/// \param szFilename - name of file to load
			/// \param format - format to load
			/// \param pFile - pointer to file if it's already open
			/// \return unsigned integer - (SUCCESS: GL texture id, FAIL: INVALID_OBJECT)
			int Load( const char *szFilename, renderer::ETextureFormat texFormat=renderer::TEXTURE_TGA, file::TFileHandle* pFile=0 );

#ifdef BASE_SUPPORT_BMP
			/// LoadBMP - Loads a bitmap file as a texture
			/// \param szFilename - name of file to load
			/// \param pFile - pointer to file if it's already open
			/// \return integer - (SUCCESS: OK, FAIL: FAIL)
			int LoadBMP( const char *szFilename, file::TFileHandle* pFile=0 );
			/// SaveBMP - Saves a bmp file from a texture
			/// \param szFilename - name of file to save
			/// \param pFile - pointer to file if it's already open
			/// \return integer - (SUCCESS: OK, FAIL: FAIL)
			int SaveBMP( const char *szFilename, file::TFileHandle* pFile=0 );
#endif // BASE_SUPPORT_BMP

			/// LoadTGA - Loads a targa file as a texture
			/// \param szFilename - name of file to load
			/// \param pFile - pointer to file if it's already open
			/// \return integer - (SUCCESS: OK, FAIL: FAIL)
			int LoadTGA( const char *szFilename, file::TFileHandle* pFile=0 );
			/// SaveTGA - Saves a targa file from a texture
			/// \param szFilename - name of file to load
			/// \param pFile - pointer to file if it's already open
			/// \return integer - (SUCCESS: OK, FAIL: FAIL)
			int SaveTGA( const char *szFilename, file::TFileHandle* pFile=0 );

#ifdef BASE_SUPPORT_PNG
			/// LoadPNG - Loads a png file as a texture
			/// \param szFilename - name of file to load
			/// \param pFile - pointer to file if it's already open
			/// \return integer - (SUCCESS: OK, FAIL: FAIL)
			int LoadPNG( const char *szFilename, file::TFileHandle* pFile=0 );
			/// SavePNG - Saves a png from a texture
			/// \param szFilename - name of file to save
			/// \param pFile - pointer to file if it's already open
			/// \return integer - (SUCCESS: OK, FAIL: FAIL)
			int SavePNG( const char *szFilename, file::TFileHandle* pFile=0 );
#endif // BASE_SUPPORT_PNG

#ifdef BASE_SUPPORT_DDS
			/// LoadDDS - Loads a dds file as a texture
			/// \param szFilename - name of file to load
			/// \param pFile - pointer to file if it's already open
			/// \return integer - (SUCCESS: OK, FAIL: FAIL)
			int LoadDDS( const char *szFilename, file::TFileHandle* pFile=0 );
#endif // BASE_SUPPORT_DDS

#ifdef BASE_SUPPORT_JPEG
			/// LoadJPEG - Loads a jpg/jpeg file as a texture
			/// \param szFilename - name of file to load
			/// \param pFile - pointer to file if it's already open
			/// \return integer - (SUCCESS: OK, FAIL: FAIL)
			int LoadJPEG( const char *szFilename, file::TFileHandle* pFile=0 );
			/// SaveJPEG - Saves a jpeg from a texture
			/// \param szFilename - name of file to save
			/// \param pFile - pointer to file if it's already open
			/// \return integer - (SUCCESS: OK, FAIL: FAIL)
			int SaveJPEG( const char *szFilename, file::TFileHandle* pFile=0 );
#endif // BASE_SUPPORT_JPEG

#ifdef BASE_SUPPORT_PCX
			/// LoadPCX - Loads a pcx file as a texture
			/// \param szFilename - name of file to load
			/// \param pFile - pointer to file if it's already open
			/// \return integer - (SUCCESS: OK, FAIL: FAIL)
			int LoadPCX( const char *szFilename, file::TFileHandle* pFile=0 );
#endif // BASE_SUPPORT_PCX

#ifdef BASE_SUPPORT_PVR
			/// LoadPVR - Loads a pvr file as a texture
			/// \param szFilename - name of file to load
			/// \param pFile - pointer to file if it's already open
			/// \return integer - (SUCCESS: OK, FAIL: FAIL)
			int LoadPVR( const char *szFilename, file::TFileHandle* pFile=0 );
			/// SavePVR - Saves a pvr from a texture
			/// \param szFilename - name of file to save
			/// \param pFile - pointer to file if it's already open
			/// \return integer - (SUCCESS: OK, FAIL: FAIL)
			int SavePVR( const char *szFilename, file::TFileHandle* pFile=0 );
#endif // BASE_SUPPORT_PVR

			/// Resize - Resize the texture
			/// \param bStretch - stretch image or put place normally in an extended image
			void Resize( bool bStretch );
			/// Flip - Flips the texture image vertically
			void Flip( void );
			/// Free - Deletes a textures information 
			/// \return integer - (SUCCESS: OK, FAIL: FAIL)
			int Free( void );

			/// file size
			std::size_t nFileSize;
			/// file format enum
			renderer::ETextureFormat eFileFormat;
			/// texture filename
			file::TFile TextureFile;
			/// GL texture object id
			GLuint nID;
			/// file size of the texture
			GLuint nTextureSize;
			/// new file size of the texture if it has been compressed
			GLuint nCompressedTextureSize;
			/// width of the texture
			int nWidth;
			/// height of the texture
			int nHeight;
			/// resized flag
			bool bResized;
			/// original width of the texture
			int nOriginalWidth;
			/// original height of the texture
			int nOriginalHeight;
			/// bits-per-pixel of the texture
			int nBPP;
			/// number of channels
			int nNumChannels;
			/// flags whether the texture contains alpha information
			bool bHasAlpha;
			/// flags whether the texture is compressed
			bool bCompressed;
			/// flags whether the texture uses a palette
			bool bPaletted;
			/// flags whether the texture is DXT compressed (DDS only)
			bool bDXTCompressed;
			/// depth of this texture (DDS only)
			int nDepth;
			/// flags whether this texture is volume (DDS only)
			bool bIsVolume;
			/// flags whether this texture is a cubemap (DDS only)
			bool bIsCubemap;
			/// Used to count the number of cubemap faces (DDS only)
			int nCubemapFaceCount;
			/// flags whether this texture contains any mipmaps (DDS only)
			bool bHasMipMaps;
			/// number of mipmaps contained in the file (DDS only)
			int nMipMapCount;
			/// bytes per element (DDS only)
			int nBytesPerElement;
			/// GL internal format
			int nInternalFormat;
			/// GL external format
			int nFormat;
			/// GL type
			int nTextureType;
			/// pointer to the actual image data
			unsigned char *image;
			/// special image pointer, for mipmapped and cubemaps
			unsigned char **imageList;
		
		private:

	};

} // namespace renderer

#endif // __TEXTURE_H__


