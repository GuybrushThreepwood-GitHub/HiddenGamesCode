
#ifndef __FREETYPEFONTUTF8GLSL_H__
#define __FREETYPEFONTUTF8GLSL_H__

#ifdef BASE_SUPPORT_FREETYPE_UTF8
#ifdef BASE_SUPPORT_OPENGL_GLSL

#include <vector>
#include <array>

#include <ft2build.h>
#include FT_LCD_FILTER_H
#include <freetype.h>
#include <ftglyph.h>
#include <ftoutln.h>
#include <fttrigon.h>
#include <ftsynth.h>

#define UTF8_FOR_CPP_CHECKED_H_2675DCD0_9480_4c0c_B92A_CC14C027B731
#include "utf8.h"
#include "Render/TextureAtlas.h"

// 9 == \t
// 10 == \n
// 13 == \r
// 32 == space
const int ASCII_VAL_TAB = 9;
const int ASCII_VAL_NEWLINE = 10;
const int ASCII_VAL_CR = 13;
const int ASCII_VAL_SPACE = 32;

//#define UTFCPP utf8
#define UTFCPP utf8::unchecked

namespace renderer
{
	const int MAX_FREETYPE_STRINGLEN	= 1024;
	const float FONT_DROP_SHADOW_SHIFT	= 16.0f;

	const int FREETYPEFONT_DPI			= 72;
	const int FREETYPEFONT_CHARACTERS	= 255;
    
    const float MAX_GLYPH_SIZE_IN_ATLAS = 100.0f;

	/// InitialiseFreetype - any initialisation for freetype
	void InitialiseFreetype( renderer::OpenGL* openGLContext=0 );
	/// ShutdownFreetype - any clear up for freetype
	void ShutdownFreetype();

	class FreetypeFont
	{
		public:
			/// default constructor
			FreetypeFont( renderer::OpenGL* openGLContext=0 );
			/// default destructor
			~FreetypeFont();

			/// Initialise - Setup the class variables and memory
			void Initialise( void );
			/// Release - Clear up class variables and release any memory
			void Release( void );

			int Load(const char *szFilename);
        
			void BuildCharacterMapsAndAtlas(std::string& fmtStr, const std::string& atlasFile, const float fontSize);

			void UpdateWithSize(float fontSize);
        
			/// Load - Loads a true type font builds GL data
			/// \param szFilename - true type font file to load
			/// \param pData - memory to load
			/// \param dataSize - size of the data
			/// \param vDims - font dimensions
			/// \param vTopColour - colour of top vertices in each character
			/// \param vBottomColour - colour of bottom vertices in each character
			/// \param bBlockFill - flag to autofill the full font
			/// \param vBlockColour - colour of all vertices
			/// \return integer - ( SUCCESS: OK or FAIL: FAIL )
			/*int Load(const char *szFilename, void *pData, std::size_t dataSize, const math::Vec2 &vDims, const math::Vec4Lite &vTopColour = math::Vec4Lite(255, 255, 255, 255), const math::Vec4Lite &vBottomColour = math::Vec4Lite(255, 255, 255, 255), bool fixedWidth = false, unsigned short fontStyleSupport = FONT_STYLE_NORMAL, int maxAtlasSize = 1024, int maxGlyphSize=MAX_GLYPH_SIZE_IN_ATLAS);
        
            int Load(const char *szFilename, const math::Vec2 &vDims, const math::Vec4Lite &vTopColour = math::Vec4Lite(255, 255, 255, 255), const math::Vec4Lite &vBottomColour = math::Vec4Lite(255, 255, 255, 255), bool fixedWidth = false, int maxGlyphSize=MAX_GLYPH_SIZE_IN_ATLAS );
        
            void WriteAtlasAndData( const char* szOutputAtlasName, const char* szOutputDataName, int fontSize, int maxGlyphSize=MAX_GLYPH_SIZE_IN_ATLAS );
            void ReadAtlasAndData( const char* szOutputAtlasName, const char* szOutputDataName );
        
			/// BuildBufferCache - Builds a buffered render list of text
			/// \param x - x window position of string
			/// \param y - y window position of string
			/// \param dropShadow - draw with drop shadow
			/// \param fmtStr - formatted string data
			/// \param ... - variable string options
			void BuildBufferCache(int x, int y, bool dropShadow, std::vector<TFormatChangeBlock>& formatChanges, std::string fmtStr, ...);*/

			/// BuildBufferCacheUTF8 - Builds a buffered render list of text
			/// \param x - x window position of string
			/// \param y - y window position of string
			/// \param dropShadow - draw with drop shadow
			/// \param fmtStr - formatted string data
			/// \param ... - variable string options
			void BuildBufferCacheUTF8(int x, int y, bool dropShadow, std::vector<TFormatChangeBlock>& formatChanges, std::string fmtStr, ...);

			/// RenderBatches - Renders the cached text
			/// \param filter - GL filter type (GL_NEAREST or GL_LINEAR)
			void RenderBatches(GLenum filter=GL_LINEAR);

			/// Print - Displays a string buffer of text in Orthographic mode
			/// \param x - x window position of string
			/// \param y - y window position of string
			/// \param dropShadow - draw with drop shadow
			/// \param fmtStr - formatted string data
			/// \param ... - variable string options
			void Print(int x, int y, bool dropShadow, GLenum filter, std::vector<TFormatChangeBlock>& formatChanges, const std::string& fmtStr, ...);

			/// Print - Displays a string buffer of text in Orthographic mode
			/// \param x - x window position of string
			/// \param y - y window position of string
			/// \param dropShadow - draw with drop shadow
			/// \param szString - string data
			/// \param ... - variable string options
			void Print(int x, int y, bool dropShadow, GLenum filter, const std::string& fmtStr, ...);
			/// Print - Displays a string buffer of text in Orthographic mode
			/// \param x - x window position of string
			/// \param y - y window position of string
			/// \param vColour - colour of the text
			/// \param szString - string data
			/// \param ... - variable string options
			//void Print(int x, int y, const math::Vec4Lite &vColour, GLenum filter, const char *szString, ...);

			/// GetCharacterWidth - gets the width of the character
			/// \param c - character to get width of
			/// \param nCharacterWidth - variable to receive font width
			/// \param style - feed character style (normal, bold, italic)
			//void GetCharacterWidth( unsigned char c, int *nCharacterWidth, int style=FONT_STYLE_NORMAL );
			/// GetCharacterWidthUTF8 - gets the width of the character
			/// \param c - character to get width of
			/// \param nCharacterWidth - variable to receive font width
			/// \param style - feed character style (normal, bold, italic)
			void GetCharacterWidthUTF8(utf8::uint32_t c, int *nCharacterWidth, int style = FONT_STYLE_NORMAL);

            /// GetStringLength - Gets the current string length
            /// \param szString - string data
            /// \param ... - variable string options
            float GetStringLength(const std::string& str, ...);
        
			/// SetDimensions - Sets the dimensions of a font
			/// \param vDims - dimension data 
			void SetDimensions( const math::Vec2 &vDims )			{ m_vDimensions = vDims; }
			/// GetDimensions - Gets the scale of the entire output of a font
			/// \return Vec2 - dimension data 
			math::Vec2 &GetDimensions( void )					{ return(m_vDimensions); }

			/// SetScale - Sets the scale of the entire output of a font
			/// \param vScale - scale data 
			void SetScale( const math::Vec2 &vScale )			{ m_vScale = vScale; }
			/// GetScale - Gets the scale of the entire output of a font
			/// \return Vec2 - scale data 
			math::Vec2 &GetScale( void )						{ return(m_vScale); }

			/// SetBlockFillFlag - Sets flag to determine whether to draw the font verts using a single colour
			/// \param flag - fill flag
			void SetBlockFillFlag( bool flag )									{ m_UseBlockColour = flag; }
			/// SetBlockFillColour - Sets colour of the font verts
			/// \param vBlockColour - fill colour
			void SetBlockFillColour( const math::Vec4Lite &vBlockColour )		{ m_vBlockColour = vBlockColour; }

            /// SetBackgroundFillFlag - Sets flag to determine whether to draw a background filled block
            /// \param flag - fill flag
            void SetBackgroundFillFlag( bool flag )								{ m_UseBackgroundColour = flag; }
            /// SetBackgroundColour - Sets colour of the background
            /// \param vBlockColour - fill colour
            void SetBackgroundColour( const math::Vec4Lite &vColour )           { m_vBGColour = vColour; }
        
			const math::Vec4Lite& GetBlockColour()								{ return m_vBlockColour; }
			const math::Vec4Lite& GetTopColour()								{ return m_vTopColour; }
			const math::Vec4Lite& GetBottomColour()								{ return m_vBottomColour; }
			bool IsFixedWidth()													{ return m_FixedWidth; }
            bool DoesBatchExist()                                               { return m_BatchExists; }
            void ClearBatches();
        
		private:
			/// FindBestAtlasSize - Recursive function that attempts to find the best size of atlas to stiore the font
			int FindBestAtlasSize(FT_Library& library, FT_Face& face, unsigned short fontStyleSupport, int *atlasWidth, int *atlasHeight);

		public:
			struct GlyphQuads
			{
				GLuint glyphWidth;

				math::Vec4 charDataUV;
				math::Vec2 charDataSize;

				math::Vec3 Offsets[3];

				math::Vec3 ShadowOffset;

				math::Vec2 textUV[4];
				math::Vec2 textPoint[4];
				math::Vec4Lite textColour[4];
				math::Vec4Lite textShadowColour[4];

				math::Vec2 textUnderline[4];
				math::Vec2 textUnderlineUV[4];

				math::Vec2 textBGBox[4];
				math::Vec4Lite textBGColour[4];

				math::Vec2 fillUV[4];

				// values to scale
				math::Vec2 atlasGlyphDims;

				float faceGlyphAdvance;

				float bitmapGlyphLeft;
				float bitmapGlyphTop;
				float bitmapRows;
				float bitmapWidth;
			};

		private:
			struct GlyphVert
			{
				math::Vec2 v;
				math::Vec2 uv;
				math::Vec4Lite col;
			};

			struct FreetypeRenderBatch
			{
				int numberOfCharacters;

				collision::AABB bufferAABB;

                GlyphVert* batchBackground;
				GlyphVert* batchForeground;
			};

			FT_Library m_Library;
			FT_Face m_Face;

			renderer::OpenGL* m_OpenGLContext;
			renderer::TextureAtlas m_AtlasTexture;
            int m_MaxAtlasSize;
        
			//GlyphQuads GlyphQuad[FONT_STYLE_TOTAL][FREETYPEFONT_CHARACTERS];
			math::Vec4 m_UnderlineData;

			/// file structure
			file::TFile m_FontFile;
			/// width list for each character
			//GLuint m_GlyphWidths[FONT_STYLE_TOTAL][FREETYPEFONT_CHARACTERS];
			/// stored colour of top vertices in each character
			math::Vec4Lite m_vTopColour;
			/// stored colour of bottom vertices in each character
			math::Vec4Lite m_vBottomColour;
			/// block colour flag
			bool m_UseBlockColour;
			/// block colour of the font
			math::Vec4Lite m_vBlockColour;
            /// background colour flag
            bool m_UseBackgroundColour;
            /// background colour of the font
            math::Vec4Lite m_vBGColour;
			/// font dimensions
			math::Vec2 m_vDimensions;
			/// font scale
			math::Vec2 m_vScale;
			// fixed width flag
			bool m_FixedWidth;

			/// storage for the GL state
			renderer::TGLDepthBlendAlphaState m_DBAState;

            bool m_BatchExists;
			FreetypeRenderBatch *m_RenderBatches;
			int m_TotalRenderBatches;

			struct GlyphInfo
			{
				utf8::uint32_t rawValue;
				wchar_t wideVal;
			};

			typedef std::map< utf8::uint32_t, GlyphInfo > TValGlyphMap;
			typedef std::pair< utf8::uint32_t, GlyphInfo > TValGlyphPair;

			TValGlyphMap m_GlyphMap;

			typedef std::array<GlyphQuads, FONT_STYLE_TOTAL> QuadArray;
			typedef std::map<utf8::uint32_t, QuadArray> TValQuadMap;
			typedef std::pair<utf8::uint32_t, QuadArray> TValQuadPair;
			TValQuadMap m_QuadMap;
	};

} // namespace renderer

#endif // BASE_SUPPORT_OPENGL_GLSL
#endif // BASE_SUPPORT_FREETYPE_UTF8

#endif // __FREETYPEFONTUTF8GLSL_H__
