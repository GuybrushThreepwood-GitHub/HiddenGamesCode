
/*===================================================================
	File: FreetypeFont.cpp
	Library: Render

	(C)Hidden Games
=====================================================================*/

#ifdef BASE_SUPPORT_FREETYPE
#if defined(BASE_SUPPORT_OPENGL) || defined(BASE_SUPPORT_OPENGLES)

#include "CoreBase.h"
#include "MathBase.h"
#include "Render/OpenGLCommon.h"
#include "Render/FF/OpenGL/OpenGL.h"
#include "Render/FF/OpenGLES/OpenGLES.h"
#include "Render/TextureShared.h"
#include "Render/Texture.h"
#include "Render/FF/OpenGL/TextureLoadAndUpload.h"
#include "Render/FF/OpenGLES/TextureLoadAndUploadOES.h"

#include "Render/FreetypeCommon.h"
#include "Render/FF/FreetypeFont.h"

using renderer::FreetypeFont;

namespace
{
	const int COMPONENTS = 2;
	const int MAX_WIDTH = 1024;
	const int MAX_HEIGHT = 1024;
	const int ITALIC_EXTRA_W = 6;

	const int ATLAS_DEPTH = 1;

	const int ATLAS_STARTWIDTH = 256;
	const int ATLAS_STARTHEIGHT = 256;

	const int ATLAS_MAXWIDTH = 1024;
	const int ATLAS_MAXHEIGHT = 1024;

	const float MAX_GLYPH_SIZE_IN_ATLAS = 100.0f;

	GLuint freeTypeUnderlineTexture = renderer::INVALID_OBJECT;

	// work area
	//GLubyte expanded_data[COMPONENTS*MAX_WIDTH*MAX_HEIGHT];

	const int UNDERLINE_TEX_WIDTH = 4;
	const int UNDERLINE_TEX_HEIGHT = 4;
	unsigned char underlineTex[UNDERLINE_TEX_WIDTH * UNDERLINE_TEX_HEIGHT * 1] = {
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
	};

	unsigned short FontStyleCombinations[FONT_STYLE_TOTAL] =
	{
		0,
		FONT_FLAG_ITALIC,
		FONT_FLAG_BOLD,
		(FONT_FLAG_ITALIC | FONT_FLAG_BOLD),
	};
}

//=============================================================================
void spf(std::string &s, const std::string fmt, ...)
{
	int n, size=100;
	bool b=false;
	va_list marker;

	if( fmt.length() == 0 )
		return;

	while (!b)
	{
		s.resize(size);
		va_start(marker, fmt);
			n = vsnprintf((char*)s.c_str(), size, fmt.c_str(), marker);
		va_end(marker);
		if ((n>0) && ((b=(n<size))==true)) 
			s.resize(n); 
		else 
			size*=2;
    }
}

//=============================================================================
void spfa(std::string &s, const std::string fmt, ...)
{
	std::string ss;
	int n, size=100;
	bool b=false;
	va_list marker;

	if( fmt.length() == 0 )
		return;

	while (!b)
	{
		ss.resize(size);
		va_start(marker, fmt);
			n = vsnprintf((char*)ss.c_str(), size, fmt.c_str(), marker);
		va_end(marker);
		if ((n>0) && ((b=(n<size))==true)) 
			ss.resize(n); 
		else 
			size*=2;
    }
    s += ss;
}

/////////////////////////////////////////////////////
/// Method: InitialiseFreetype
/// Params: None
///
/////////////////////////////////////////////////////
void renderer::InitialiseFreetype( renderer::OpenGL* openGLContext )
{
	if( openGLContext != 0 )
	{
		ShutdownFreetype();
	
		glGenTextures( 1, &freeTypeUnderlineTexture );
	}
}

/////////////////////////////////////////////////////
/// Method: ShutdownFreetype
/// Params: None
///
/////////////////////////////////////////////////////
void renderer::ShutdownFreetype()
{
	if( freeTypeUnderlineTexture != renderer::INVALID_OBJECT )
	{
		freeTypeUnderlineTexture = renderer::INVALID_OBJECT;
	}
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
FreetypeFont::FreetypeFont( renderer::OpenGL* openGLContext )
{
	if( openGLContext == 0 )
		m_OpenGLContext = renderer::OpenGL::GetInstance();
	else
		m_OpenGLContext = openGLContext;

	if(  freeTypeUnderlineTexture == renderer::INVALID_OBJECT )
		renderer::InitialiseFreetype( m_OpenGLContext );

	Initialise();
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
FreetypeFont::~FreetypeFont()
{
	Release();
}

/////////////////////////////////////////////////////
/// Method: Initialise
/// Params: None
///
/////////////////////////////////////////////////////
void FreetypeFont::Initialise( void )
{
	std::memset( &m_FontFile, 0, sizeof(file::TFile) );
	m_FixedWidth = false;
	m_vDimensions = math::Vec2( 10.0f, 10.0f );

	m_UseBlockColour = true;
	m_vBlockColour = math::Vec4Lite( 255,255,255,255 );

	m_UseBackgroundColour = false;
	m_vBGColour = math::Vec4Lite( 0,0,0,255 );
    
	m_vTopColour	= math::Vec4Lite( 255,255,255,255 );
	m_vBottomColour = math::Vec4Lite( 255,255,255,255 );
	m_vScale		= math::Vec2( 1.0f, 1.0f );
    
    m_BatchExists = false;
    //m_RenderBatches = 0;
    m_TotalRenderBatches = 0;
}

/////////////////////////////////////////////////////
/// Method: Release
/// Params: None
///
/////////////////////////////////////////////////////
void FreetypeFont::Release( void )
{
	m_AtlasTexture.Destroy();
}

/////////////////////////////////////////////////////
/// Method: Load
/// Params: [in]szFilename, [in]vDims, [in]bDropShadow, [in]vTopColour, [in]vBottomColour
///
/////////////////////////////////////////////////////
int FreetypeFont::Load(const char *szFilename, void *pData, std::size_t dataSize, const math::Vec2 &vDims, const math::Vec4Lite &vTopColour, const math::Vec4Lite &vBottomColour, bool fixedWidth, unsigned short fontStyleSupport, int maxAtlasSize)
{
	int i=0, a = 0, b = 0;
    bool missedGlyph = false;
    
	m_vDimensions = vDims;
	m_vBlockColour = vTopColour;
	m_FixedWidth = fixedWidth;

	// colour store
	m_vTopColour = vTopColour;
	m_vBottomColour = vBottomColour;

	// Create and initilize a freetype font library.
	FT_Library library;
	if( FT_Init_FreeType( &library ) ) 
	{
		DBGLOG( "FREETYPEFONT: *ERROR* FT_Init_FreeType failed" );
		return( 1 );
	}

	// The object in which Freetype holds information on a given font is called a "face".
	FT_Face face;

	CreateFileStructure( szFilename, &m_FontFile );

	// This is where we load in the font information from the file. Of all the places where the code might die, this is the most likely,
	// as FT_New_Face will die if the font file does not exist or is somehow broken.
	
	if (szFilename)
	{
		if (FT_New_Face(library, szFilename, 0, &face))
		{
			DBGLOG("FREETYPEFONT: *ERROR* FT_New_Face failed\n");
			FT_Done_FreeType(library);
			return 1;
		}
	}
	else
	{
		if (FT_New_Memory_Face(library, static_cast<const FT_Byte*>(pData), static_cast<FT_Long>(dataSize), 0, &face))
		{
			DBGLOG("FREETYPEFONT: *ERROR* FT_New_Face failed\n");
			FT_Done_FreeType(library);
			return 1;
		}
	}

	if (FT_Select_Charmap(face, FT_ENCODING_UNICODE))
	{
		DBGLOG("FREETYPEFONT: *ERROR* FT_Select_Charmap failed\n");

		FT_Done_Face(face);
		FT_Done_FreeType(library);

		return 1;
	}

	// For some twisted reason, Freetype measures font size in terms of 1/64ths of pixels.  Thus, to make a font
	// nFontHeight pixels high, we need to request a size of nFontHeight*64. (nFontHeight << 6 is just a prettier way of writting nFontHeight*64)
	//FT_Set_Char_Size( face, (int)m_vDimensions.fWidth << 6, (int)m_vDimensions.fHeight << 6, FREETYPEFONT_DPI, FREETYPEFONT_DPI ); // 72dpi
    math::Vec2 modifiedFontSize = m_vDimensions;
    
	if (m_vDimensions.Width > MAX_GLYPH_SIZE_IN_ATLAS ||
		m_vDimensions.Height > MAX_GLYPH_SIZE_IN_ATLAS)
    {
		m_vDimensions.Width = MAX_GLYPH_SIZE_IN_ATLAS;
		m_vDimensions.Height = MAX_GLYPH_SIZE_IN_ATLAS;
    }
    
    FT_Set_Pixel_Sizes( face, static_cast<int>(m_vDimensions.Width), static_cast<int>(m_vDimensions.Height) );

    m_MaxAtlasSize = maxAtlasSize;
    
	int atlasWidth = ATLAS_STARTWIDTH;
	int atlasHeight = ATLAS_STARTHEIGHT;
	int result = FindBestAtlasSize(library, face, fontStyleSupport, &atlasWidth, &atlasHeight);

	if (result == -1)
		DBGLOG("FREETYPEFONT: *WARNING* Atlas is too big some characters may not appear for %s\n", szFilename );

	// now the real atlas size can be created
	m_AtlasTexture.Create(m_OpenGLContext, atlasWidth, atlasHeight, ATLAS_DEPTH);

	math::Vec4Int underLineRegion = m_AtlasTexture.GetRegion(UNDERLINE_TEX_WIDTH + 1, UNDERLINE_TEX_HEIGHT + 1);
	if (underLineRegion.X < 0)
		DBGLOG("Texture atlas is full\n");
	else
		m_AtlasTexture.SetRegion(underLineRegion.X, underLineRegion.Y, UNDERLINE_TEX_WIDTH, UNDERLINE_TEX_HEIGHT, underlineTex, UNDERLINE_TEX_WIDTH);
	math::Vec4 underlineData = math::Vec4(static_cast<float>(underLineRegion.X + 1), static_cast<float>(underLineRegion.Y + 1), static_cast<float>(UNDERLINE_TEX_WIDTH - 2), static_cast<float>(UNDERLINE_TEX_HEIGHT - 2));

    m_vDimensions = modifiedFontSize;
    
	// Here we ask opengl to allocate resources for all the textures and displays lists which we
	// are about to create.  
	for( i = 0; i < FONT_STYLE_TOTAL; ++i )
	{
		unsigned short flags = FontStyleCombinations[i];
		
		if( fontStyleSupport < i )
			continue;

		// This is where we actually create each of the fonts display lists.
		for(unsigned char j = 0; j < FREETYPEFONT_CHARACTERS; j++ )
		{
			// The first thing we do is get FreeType to render our character
			// into a bitmap.  This actually requires a couple of FreeType commands:

            math::Vec2 modifiedFontSize = m_vDimensions;
            
            if( m_vDimensions.Width > MAX_GLYPH_SIZE_IN_ATLAS ||
               m_vDimensions.Height > MAX_GLYPH_SIZE_IN_ATLAS )
            {
                m_vDimensions.Width = MAX_GLYPH_SIZE_IN_ATLAS;
                m_vDimensions.Height = MAX_GLYPH_SIZE_IN_ATLAS;
            }
            
            FT_Set_Pixel_Sizes( face, static_cast<int>(m_vDimensions.Width), static_cast<int>(m_vDimensions.Height) );

			FT_Int32 filterFlags = 0;
			filterFlags = FT_LOAD_DEFAULT | FT_LOAD_IGNORE_GLOBAL_ADVANCE_WIDTH | FT_LOAD_LINEAR_DESIGN;

			if (ATLAS_DEPTH == 3)
			{
				unsigned char lcd_weights[5];

				// FT_LCD_FILTER_LIGHT   is (0x00, 0x55, 0x56, 0x55, 0x00)
				// FT_LCD_FILTER_DEFAULT is (0x10, 0x40, 0x70, 0x40, 0x10)
				lcd_weights[0] = 0x10;
				lcd_weights[1] = 0x40;
				lcd_weights[2] = 0x70;
				lcd_weights[3] = 0x40;
				lcd_weights[4] = 0x10;

				FT_Library_SetLcdFilter(library, FT_LCD_FILTER_LIGHT);
				filterFlags |= FT_LOAD_TARGET_LCD;
				if (1)
				{
					FT_Library_SetLcdFilterWeights(library, lcd_weights);
				}
			}
			else
				filterFlags |= FT_LOAD_TARGET_MONO;

			// Load the Glyph for our character.
			if (FT_Load_Glyph(face, FT_Get_Char_Index(face, j), filterFlags))
			{
				DBGLOG("FREETYPEFONT: *ERROR* FT_Load_Glyph failed");
				return(1);
			}

			// Move the face's glyph into a Glyph object.
			FT_Glyph tmpGlyph;
			FT_GlyphSlot tmpSlot = face->glyph;

			if( flags & FONT_FLAG_ITALIC )
				FT_GlyphSlot_Oblique(tmpSlot);

			if( flags & FONT_FLAG_BOLD )
				FT_GlyphSlot_Embolden(tmpSlot);

			if (FT_Get_Glyph(face->glyph, &tmpGlyph))
			{
				DBGLOG( "FREETYPEFONT: *ERROR* FT_Get_Glyph failed\n" );
				return( 1 );
			}

			// Convert the glyph to a bitmap.
			if (ATLAS_DEPTH == 1)
				FT_Glyph_To_Bitmap(&tmpGlyph, FT_RENDER_MODE_NORMAL, 0, 1);
			else
				FT_Glyph_To_Bitmap(&tmpGlyph, FT_RENDER_MODE_LCD, 0, 1);

			FT_BitmapGlyph tmpBitmapGlyph = (FT_BitmapGlyph)tmpGlyph;

			// This reference will make accessing the bitmap easier
			FT_Bitmap &tmpBitmap = tmpBitmapGlyph->bitmap;

			// We want each glyph to be separated by at least one black pixel
			int bmp_w = tmpBitmap.width / ATLAS_DEPTH + 1;
			int bmp_h = tmpBitmap.rows + 1;
			int bmp_x = 0;
			int bmp_y = 0;

			if (bmp_w > 1 &&
				bmp_h > 1)
			{
				math::Vec4Int region = m_AtlasTexture.GetRegion(bmp_w, bmp_h);
				if (region.X < 0)
				{
					//missed++;
                    if(!missedGlyph)
                    {
                        DBGLOG("Texture atlas is full\n");
                        missedGlyph = true;
                    }
					// clear the glyph memory
					FT_Done_Glyph(tmpGlyph);

					continue;
				}
				bmp_w = bmp_w - 1;
				bmp_h = bmp_h - 1;
				bmp_x = region.X;
				bmp_y = region.Y;

				m_AtlasTexture.SetRegion(bmp_x, bmp_y, bmp_w, bmp_h, tmpBitmap.buffer, tmpBitmap.pitch);
			}
            
			math::Vec4 charDataUV = math::Vec4(static_cast<float>(bmp_x), static_cast<float>(bmp_y), static_cast<float>(tmpBitmap.width), static_cast<float>(tmpBitmap.rows));

            m_vDimensions = modifiedFontSize;
            
            FT_Set_Pixel_Sizes( face, static_cast<int>(m_vDimensions.Width), static_cast<int>(m_vDimensions.Height) );

			// clear the glyph memory
			FT_Done_Glyph(tmpGlyph);

			//////////////// ****************

            // once image is in the atlas
            // Load the Glyph for our character.
            if (FT_Load_Glyph(face, FT_Get_Char_Index(face, j), filterFlags))
            {
                DBGLOG("FREETYPEFONT: *ERROR* FT_Load_Glyph failed");
                return(1);
            }
            
            // Move the face's glyph into a Glyph object.
			FT_Glyph glyph;
			FT_GlyphSlot slot = face->glyph;
            
            if( flags & FONT_FLAG_ITALIC )
                FT_GlyphSlot_Oblique( slot );
            
            if( flags & FONT_FLAG_BOLD )
                FT_GlyphSlot_Embolden( slot );
            
            if( FT_Get_Glyph( face->glyph, &glyph ) )
            {
                DBGLOG( "FREETYPEFONT: *ERROR* FT_Get_Glyph failed\n" );
                return( 1 );
            }
            
            // Convert the glyph to a bitmap.
            if (ATLAS_DEPTH == 1)
                FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, 0, 1);
            else
                FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_LCD, 0, 1);
            
			FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;
            
            // This reference will make accessing the bitmap easier
			FT_Bitmap bitmap = bitmap_glyph->bitmap;

            //////////////// ****************
            
			// save the glyph width, to use for space counting
			if( flags & FONT_FLAG_ITALIC )
				m_GlyphWidths[i][j] = (face->glyph->advance.x >> 6) + (ITALIC_EXTRA_W);
			else
				m_GlyphWidths[i][j] = face->glyph->advance.x >> 6;

			// create triangle strip
			// 2 ---- 3
			// |\     |
			// |  \   |
			// |    \ |
			// 0 ---- 1
			if( m_FixedWidth )
				GlyphQuad[i][j].Offsets[0] = math::Vec3( static_cast<float>(bitmap_glyph->left)+m_vDimensions.Width*0.25f, 0.0f, 0.0f );
			else
				GlyphQuad[i][j].Offsets[0] = math::Vec3( static_cast<float>(bitmap_glyph->left), 0.0f, 0.0f );
			GlyphQuad[i][j].Offsets[1] = math::Vec3( 0.0f, static_cast<float>(bitmap_glyph->top) - static_cast<float>(bitmap.rows), 0.0f );

			GlyphQuad[i][j].ShadowOffset = math::Vec3(m_vDimensions.Width / FONT_DROP_SHADOW_SHIFT, -m_vDimensions.Height / FONT_DROP_SHADOW_SHIFT, 0.0f);

			float UNDERLINE_HEIGHT = m_vDimensions.Height*0.075f;
			float UNDERLINE_INC_W = m_vDimensions.Width*0.055f;

			float BG_INC_H = m_vDimensions.Height*0.025f;
			float BG_INC_W = m_vDimensions.Width*0.015f;

			math::Vec4 charDataSize = math::Vec4(static_cast<float>(bmp_x), static_cast<float>(bmp_y), static_cast<float>(bitmap.width), static_cast<float>(bitmap.rows));
			float glw = static_cast<float>(m_GlyphWidths[i][j]);

			GlyphQuad[i][j].textUV[0] = math::Vec2(charDataUV.X / static_cast<float>(atlasWidth), (charDataUV.Y + charDataUV.Height) / static_cast<float>(atlasHeight));
			GlyphQuad[i][j].textPoint[0] = math::Vec2(0.0f, 0.0f);
			GlyphQuad[i][j].textColour[0] = vBottomColour;
			GlyphQuad[i][j].textShadowColour[0] = math::Vec4Lite(0, 0, 0, 255);
			GlyphQuad[i][j].textUnderline[0] = math::Vec2(-UNDERLINE_INC_W, 0.0f);
			GlyphQuad[i][j].textUnderlineUV[0] = math::Vec2(underlineData.X / static_cast<float>(atlasWidth), (underlineData.Y + underlineData.Height) / static_cast<float>(atlasHeight));
			GlyphQuad[i][j].textBGBox[0] = math::Vec2(-(BG_INC_W), -BG_INC_H);
			GlyphQuad[i][j].fillUV[0] = math::Vec2(0.0f, 0.0f);

			GlyphQuad[i][j].textUV[1] = math::Vec2((charDataUV.X + charDataUV.Width) / static_cast<float>(atlasWidth), (charDataUV.Y + charDataUV.Height) / static_cast<float>(atlasHeight));
			GlyphQuad[i][j].textPoint[1] = math::Vec2(charDataSize.Width, 0.0f);
			GlyphQuad[i][j].textColour[1] = vBottomColour;
			GlyphQuad[i][j].textShadowColour[1] = math::Vec4Lite(0, 0, 0, 255);
			GlyphQuad[i][j].textUnderline[1] = math::Vec2(glw + UNDERLINE_INC_W, 0.0f);
			GlyphQuad[i][j].textUnderlineUV[1] = math::Vec2((underlineData.X + underlineData.Width) / static_cast<float>(atlasWidth), (underlineData.Y + underlineData.Height) / static_cast<float>(atlasHeight));
			GlyphQuad[i][j].textBGBox[1] = math::Vec2(glw + BG_INC_W, -BG_INC_H);
			GlyphQuad[i][j].fillUV[1] = math::Vec2(1.0f, 0.0f);

			GlyphQuad[i][j].textUV[2] = math::Vec2(charDataUV.X / static_cast<float>(atlasWidth), charDataUV.Y / static_cast<float>(atlasHeight));
			GlyphQuad[i][j].textPoint[2] = math::Vec2(0.0f, charDataSize.Height);
			GlyphQuad[i][j].textColour[2] = vTopColour;
			GlyphQuad[i][j].textShadowColour[2] = math::Vec4Lite(0, 0, 0, 255);
			GlyphQuad[i][j].textUnderline[2] = math::Vec2(-UNDERLINE_INC_W, UNDERLINE_HEIGHT);
			GlyphQuad[i][j].textUnderlineUV[2] = math::Vec2(underlineData.X / static_cast<float>(atlasWidth), underlineData.Y / static_cast<float>(atlasHeight));
			GlyphQuad[i][j].textBGBox[2] = math::Vec2(-(BG_INC_W), (m_vDimensions.Height*0.95f));
			GlyphQuad[i][j].fillUV[2] = math::Vec2(0.0f, 1.0f);

			GlyphQuad[i][j].textUV[3] = math::Vec2((charDataUV.X + charDataUV.Width) / static_cast<float>(atlasWidth), charDataUV.Y / static_cast<float>(atlasHeight));
			GlyphQuad[i][j].textPoint[3] = math::Vec2(charDataSize.Width, charDataSize.Height);
			GlyphQuad[i][j].textColour[3] = vTopColour;
			GlyphQuad[i][j].textShadowColour[3] = math::Vec4Lite(0, 0, 0, 255);
			GlyphQuad[i][j].textUnderline[3] = math::Vec2(glw + UNDERLINE_INC_W, UNDERLINE_HEIGHT);
			GlyphQuad[i][j].textUnderlineUV[3] = math::Vec2((underlineData.X + underlineData.Width) / static_cast<float>(atlasWidth), underlineData.Y / static_cast<float>(atlasHeight));
			GlyphQuad[i][j].textBGBox[3] = math::Vec2(glw + BG_INC_W, (m_vDimensions.Height*0.95f));
			GlyphQuad[i][j].fillUV[3] = math::Vec2(1.0f, 1.0f);

			if( m_FixedWidth )
				GlyphQuad[i][j].Offsets[2] = math::Vec3( m_vDimensions.Width*0.75f, 0.0f, 0.0f );
			else
				GlyphQuad[i][j].Offsets[2] = math::Vec3( glw, 0.0f, 0.0f );

			// clear the glyph memory
			FT_Done_Glyph(glyph);
		}
	}

	//char imageFileCheck[core::MAX_PATH + core::MAX_PATH];
	//file::TFile fileStruct;
	//file::CreateFileStructure(szFilename, &fileStruct);
	//snprintf(imageFileCheck, core::MAX_PATH + core::MAX_PATH, "%s%d.png", fileStruct.szFile, (int)m_vDimensions.Width );
	//m_AtlasTexture.WriteToFile(imageFileCheck);

	// We don't need the face information now that the display lists have been created, so we free the associated resources.
	FT_Done_Face(face);

	// Ditto for the library.
	FT_Done_FreeType(library);

	return(0);
}

/////////////////////////////////////////////////////
/// Method: ClearBatches
/// Params: None
///
/////////////////////////////////////////////////////
void FreetypeFont::ClearBatches()
{

}

/////////////////////////////////////////////////////
/// Method: BuildBufferCache
/// Params: [in]x, [in]y, [in]dropShadow, [in]formatChanges, [in]fmtStr
///
/////////////////////////////////////////////////////
void FreetypeFont::BuildBufferCache(int x, int y, bool dropShadow, std::vector<TFormatChangeBlock>& formatChanges, std::string& fmtStr, ...)
{

}

/////////////////////////////////////////////////////
/// Method: RenderBatches
/// Params: [in]filter
///
/////////////////////////////////////////////////////
void FreetypeFont::RenderBatches(GLenum filter)
{

}

/////////////////////////////////////////////////////
/// Method: Print
/// Params: [in]x, [in]y, [in]dropShadow, [in]szString, [in]optional
///
/////////////////////////////////////////////////////
void FreetypeFont::Print(int x, int y, bool dropShadow, GLenum filter, std::vector<TFormatChangeBlock>& formatChanges, std::string fmtStr, ...)
{
	int style = FONT_STYLE_NORMAL;
	unsigned int i=0;
	std::string s, ss;
	int n, size=100;
	bool b=false;
	va_list marker;

	if( fmtStr.length() == 0 )
		return;

	while (!b)
	{
		ss.resize(size);
		va_start(marker, fmtStr);
			n = vsnprintf((char*)ss.c_str(), size, fmtStr.c_str(), marker);
		va_end(marker);
		if ((n>0) && ((b=(n<size))==true)) 
			ss.resize(n); 
		else 
			size*=2;
    }
    s += ss;

	m_OpenGLContext->DisableVBO();
	
	glClientActiveTexture( GL_TEXTURE0 );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );

	bool textureState = m_OpenGLContext->GetTextureState();

	if( !m_UseBlockColour )
		m_OpenGLContext->EnableColourArray();

	float xOffsetTotal = 0.0f;

	// initial block
	TFormatChangeBlock block;
	block.style = FONT_STYLE_NORMAL;
	block.numCharsInBlockText = static_cast<int>(s.length());
	block.textColour = m_vBlockColour;
	block.underline = false;
	block.bgColour = false;
    block.textBGColour = math::Vec4Lite(0,0,0,255);
    
	bool hasChanges = false;
	bool doUnderline = false;
	int currentChangeIndex = 0;
    bool doBackground = false;
    
	if( formatChanges.size() != 0 )
	{
		hasChanges = true;

		style = formatChanges[currentChangeIndex].style;
		block.numCharsInBlockText = formatChanges[currentChangeIndex].numCharsInBlockText;
		m_vBlockColour = formatChanges[currentChangeIndex].textColour;
		doUnderline = formatChanges[currentChangeIndex].underline;
		m_vBGColour = formatChanges[currentChangeIndex].textBGColour;
		doBackground = formatChanges[currentChangeIndex].bgColour;
	}

	int currentCharChangeOffset = 0;

	m_AtlasTexture.Bind(filter);

	glPushMatrix();	
		glTranslatef( static_cast<GLfloat>(x), static_cast<GLfloat>(y), 0.0f );

		for( i=0; i < s.length(); ++i )
		{
			if( hasChanges )
			{
				if( currentCharChangeOffset >= block.numCharsInBlockText )
				{
					currentChangeIndex++;

					if( currentChangeIndex < static_cast<int>( formatChanges.size() ) )
					{
						style = formatChanges[currentChangeIndex].style;
						block.numCharsInBlockText = formatChanges[currentChangeIndex].numCharsInBlockText;
						m_vBlockColour = formatChanges[currentChangeIndex].textColour;
						doUnderline = formatChanges[currentChangeIndex].underline;
						m_vBGColour = formatChanges[currentChangeIndex].textBGColour;
						doBackground = formatChanges[currentChangeIndex].bgColour;

						currentCharChangeOffset = 0;
					}
				}
			}

			if( s[i] == '\n' )
			{
				glTranslatef( -xOffsetTotal, -m_vDimensions.Height, 0.0f );
				xOffsetTotal = 0.0f;
				continue;
			}

			currentCharChangeOffset++;
			unsigned char index = s[i];
			
			if( doBackground )
			{
				m_OpenGLContext->SetColour4ub( m_vBGColour.R, m_vBGColour.G, m_vBGColour.B, m_vBGColour.A );

				glPushMatrix();
					glTranslatef( 0.0f, -m_vDimensions.Y*0.15f, 0.0f );
					glVertexPointer( 2, GL_FLOAT, 0, &GlyphQuad[style][index].textBGBox[0] );
					glTexCoordPointer( 2, GL_FLOAT, 0, &GlyphQuad[style][index].textUnderlineUV[0] );

					glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
				glPopMatrix();
			}

			if( dropShadow )
			{
				glPushMatrix();
					glTranslatef( GlyphQuad[style][index].Offsets[0].X, GlyphQuad[style][index].Offsets[0].Y, GlyphQuad[style][index].Offsets[0].Z );

					glTranslatef( GlyphQuad[style][index].Offsets[1].X, GlyphQuad[style][index].Offsets[1].Y, GlyphQuad[style][index].Offsets[1].Z );

					glPushMatrix();
						glTranslatef( GlyphQuad[style][index].ShadowOffset.X, GlyphQuad[style][index].ShadowOffset.Y, GlyphQuad[style][index].ShadowOffset.Z );

						glVertexPointer( 2, GL_FLOAT, 0, &GlyphQuad[style][index].textPoint[0] );
						glTexCoordPointer( 2, GL_FLOAT, 0, &GlyphQuad[style][index].textUV[0] );

						if( !m_UseBlockColour )
							glColorPointer( 4, GL_UNSIGNED_BYTE, 0, &GlyphQuad[style][index].textShadowColour[0] );
						else
							m_OpenGLContext->SetColour4ub( 0, 0, 0, m_vBlockColour.A );

						glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
					glPopMatrix();
				glPopMatrix();
			}

			glPushMatrix();
				glTranslatef( GlyphQuad[style][index].Offsets[0].X, GlyphQuad[style][index].Offsets[0].Y, GlyphQuad[style][index].Offsets[0].Z );

				glTranslatef( GlyphQuad[style][index].Offsets[1].X, GlyphQuad[style][index].Offsets[1].Y, GlyphQuad[style][index].Offsets[1].Z );

				glVertexPointer( 2, GL_FLOAT, 0, &GlyphQuad[style][index].textPoint[0] );
				glTexCoordPointer( 2, GL_FLOAT, 0, &GlyphQuad[style][index].textUV[0] );

				if( !m_UseBlockColour )
					glColorPointer( 4, GL_UNSIGNED_BYTE, 0, &GlyphQuad[style][index].textColour[0] );
				else
					m_OpenGLContext->SetColour4ub( m_vBlockColour.R, m_vBlockColour.G, m_vBlockColour.B, m_vBlockColour.A );

				glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
			glPopMatrix();

			if( doUnderline )
			{
				glPushMatrix();
					glTranslatef( 0.0f, -m_vDimensions.Y*0.15f, 0.0f );
					glVertexPointer( 2, GL_FLOAT, 0, &GlyphQuad[style][index].textUnderline[0] );
					glTexCoordPointer( 2, GL_FLOAT, 0, &GlyphQuad[style][index].textUnderlineUV[0] );

					if( !m_UseBlockColour )
						glColorPointer( 4, GL_UNSIGNED_BYTE, 0, &GlyphQuad[style][index].textColour[0] );
					else
						m_OpenGLContext->SetColour4ub( m_vBlockColour.R, m_vBlockColour.G, m_vBlockColour.B, m_vBlockColour.A );

					glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
				glPopMatrix();
			}

			glTranslatef( GlyphQuad[style][index].Offsets[2].X, GlyphQuad[style][index].Offsets[2].Y, GlyphQuad[style][index].Offsets[2].Z );

			// track how far word has gone
			xOffsetTotal += GlyphQuad[style][index].Offsets[2].X;
		}
	glPopMatrix();

	glDisableClientState( GL_TEXTURE_COORD_ARRAY );

	if( !m_UseBlockColour )
		m_OpenGLContext->DisableColourArray();

	s.clear();
	ss.clear();
}

/////////////////////////////////////////////////////
/// Method: Print
/// Params: [in]x, [in]y, [in]dropShadow, [in]szString, [in]optional
///
/////////////////////////////////////////////////////
void FreetypeFont::Print(int x, int y, bool dropShadow, GLenum filter, const char *szString, ...)
{
	int style = FONT_STYLE_NORMAL;
	char szText[MAX_FREETYPE_STRINGLEN];
	std::va_list ap;				
	unsigned int i=0;

	if( szString == 0 ||
		std::strlen(szString) >= MAX_FREETYPE_STRINGLEN-1)		
		return;				

	va_start( ap, szString );					
		std::vsprintf( szText, szString, ap );			
	va_end( ap );

	m_OpenGLContext->DisableVBO();
	
	glClientActiveTexture( GL_TEXTURE0 );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );

	if( !m_UseBlockColour )
		m_OpenGLContext->EnableColourArray();

	float xOffsetTotal = 0.0f;

	//math::Vec4Lite globalColour = m_OpenGLContext->GetColour4ub();
	m_AtlasTexture.Bind(filter);

	glPushMatrix();	
		glTranslatef( static_cast<GLfloat>(x), static_cast<GLfloat>(y), 0.0f );
		for( i=0; i < (int)std::strlen(szText); ++i )
		{
			if( szText[i] == '\n' )
			{
				glTranslatef( -xOffsetTotal, -m_vDimensions.Height, 0.0f );
				xOffsetTotal = 0.0f;
				continue;
			}

			unsigned char index = szText[i];

			if( dropShadow )
			{
				glPushMatrix();
					glTranslatef( GlyphQuad[style][index].Offsets[0].X, GlyphQuad[style][index].Offsets[0].Y, GlyphQuad[style][index].Offsets[0].Z );

					glTranslatef( GlyphQuad[style][index].Offsets[1].X, GlyphQuad[style][index].Offsets[1].Y, GlyphQuad[style][index].Offsets[1].Z );

					glPushMatrix();
						glTranslatef( GlyphQuad[style][index].ShadowOffset.X, GlyphQuad[style][index].ShadowOffset.Y, GlyphQuad[style][index].ShadowOffset.Z );

						glVertexPointer( 2, GL_FLOAT, 0, &GlyphQuad[style][index].textPoint[0] );
						glTexCoordPointer( 2, GL_FLOAT, 0, &GlyphQuad[style][index].textUV[0] );

						if( !m_UseBlockColour )
							glColorPointer( 4, GL_UNSIGNED_BYTE, 0, &GlyphQuad[style][index].textShadowColour[0] );
						else
							m_OpenGLContext->SetColour4ub( 0, 0, 0, m_vBlockColour.A );

						glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
					glPopMatrix();
				glPopMatrix();
			}

			glPushMatrix();
				glTranslatef( GlyphQuad[style][index].Offsets[0].X, GlyphQuad[style][index].Offsets[0].Y, GlyphQuad[style][index].Offsets[0].Z );

				glTranslatef( GlyphQuad[style][index].Offsets[1].X, GlyphQuad[style][index].Offsets[1].Y, GlyphQuad[style][index].Offsets[1].Z );

				glVertexPointer( 2, GL_FLOAT, 0, &GlyphQuad[style][index].textPoint[0] );
				glTexCoordPointer( 2, GL_FLOAT, 0, &GlyphQuad[style][index].textUV[0] );

				if( !m_UseBlockColour )
					glColorPointer( 4, GL_UNSIGNED_BYTE, 0, &GlyphQuad[style][index].textColour[0] );
				else
					m_OpenGLContext->SetColour4ub( m_vBlockColour.R, m_vBlockColour.G, m_vBlockColour.B, m_vBlockColour.A );

				glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
			glPopMatrix();
			
			glTranslatef( GlyphQuad[style][index].Offsets[2].X, GlyphQuad[style][index].Offsets[2].Y, GlyphQuad[style][index].Offsets[2].Z );

			// track how far word has gone
			xOffsetTotal += GlyphQuad[style][index].Offsets[2].X;
		}
	glPopMatrix();

	glDisableClientState( GL_TEXTURE_COORD_ARRAY );

	if( !m_UseBlockColour )
		m_OpenGLContext->DisableColourArray();
}

/////////////////////////////////////////////////////
/// Method: Print
/// Params: [in]x, [in]y, [in]vColour, [in]szString, [in]optional
///
/////////////////////////////////////////////////////
void FreetypeFont::Print(int x, int y, const math::Vec4Lite &vColour, GLenum filter, const char *szString, ...)
{
	int style = FONT_STYLE_NORMAL;
	char szText[MAX_FREETYPE_STRINGLEN];
	std::va_list ap;				
	unsigned int i=0;

	if( szString == 0 ||
		std::strlen(szString) >= MAX_FREETYPE_STRINGLEN-1)		
		return;				

	va_start( ap, szString );					
		std::vsprintf( szText, szString, ap );			
	va_end( ap );

	math::Vec4Lite globalColour = m_OpenGLContext->GetColour4ub();

		glClientActiveTexture( GL_TEXTURE0 );
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );

	if( !m_UseBlockColour )
		m_OpenGLContext->EnableColourArray();

	m_AtlasTexture.Bind(filter);

	glPushMatrix();	
		glTranslatef( static_cast<GLfloat>(x), static_cast<GLfloat>(y), 0.0f );
		for( i=0; i < (int)std::strlen(szText); ++i )
		{
			char index = szText[i];

			for( i=0; i < 4; ++i )
				GlyphQuad[style][index].textColour[i] = vColour;

			glPushMatrix();
				glTranslatef( GlyphQuad[style][index].Offsets[0].X, GlyphQuad[style][index].Offsets[0].Y, GlyphQuad[style][index].Offsets[0].Z );

				glTranslatef( GlyphQuad[style][index].Offsets[1].X, GlyphQuad[style][index].Offsets[1].Y, GlyphQuad[style][index].Offsets[1].Z );

				glVertexPointer( 2, GL_FLOAT, 0, &GlyphQuad[style][index].textPoint[0] );
				glTexCoordPointer( 2, GL_FLOAT, 0, &GlyphQuad[style][index].textUV[0] );
				if( !m_UseBlockColour )
					glColorPointer( 4, GL_UNSIGNED_BYTE, 0, &GlyphQuad[style][index].textColour[0] );
				else
					m_OpenGLContext->SetColour4ub( m_vBlockColour.R, m_vBlockColour.G, m_vBlockColour.B, globalColour.A );

				glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
			glPopMatrix();
			
			glTranslatef( GlyphQuad[style][index].Offsets[2].X, GlyphQuad[style][index].Offsets[2].Y, GlyphQuad[style][index].Offsets[2].Z );
		}
	glPopMatrix();

	glClientActiveTexture( GL_TEXTURE0 );
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );

	if( !m_UseBlockColour )
		m_OpenGLContext->DisableColourArray();
}

/////////////////////////////////////////////////////
/// Method: GetFontWidthHeight
/// Params: [in]c, [out]nCharacterWidth
///
/////////////////////////////////////////////////////
void FreetypeFont::GetCharacterWidth( unsigned char c, int *nCharacterWidth, int style )
{
	int nIndex = static_cast<int>(c);

	if( nCharacterWidth )
	{
		if( m_FixedWidth )
			*nCharacterWidth = static_cast<int>(m_vDimensions.Width*0.75f);
		else
		{
			*nCharacterWidth = m_GlyphWidths[style][nIndex];
		}
	}
}

/////////////////////////////////////////////////////
/// Method: GetStringLength
/// Params: [in]szString
///
/////////////////////////////////////////////////////
float FreetypeFont::GetStringLength(const char *szString, ...)
{
    int style = FONT_STYLE_NORMAL;
    char szText[MAX_FREETYPE_STRINGLEN];
    std::va_list ap;
    unsigned int i=0;
    
    if( szString == 0 ||
       std::strlen(szString) >= MAX_FREETYPE_STRINGLEN-1)
        return 0.0f;
    
    va_start( ap, szString );
        std::vsprintf( szText, szString, ap );
    va_end( ap );
    
    float xOffsetTotal = 0.0f;
    
    for( i=0; i < (int)std::strlen(szText); ++i )
    {
        if( szText[i] == '\n' )
        {
            return xOffsetTotal;
        }
        
        unsigned char index = szText[i];
        
        // track how far word has gone
        xOffsetTotal += GlyphQuad[style][index].Offsets[2].X;
    }
    
    return xOffsetTotal;
}

/////////////////////////////////////////////////////
/// Method: FindBestAtlasSize
/// Params: [in]library, [in]face, [in]fontStyleSupport, [in/out]atlasWidth, [in/out]atlasHeight
///
/////////////////////////////////////////////////////
int FreetypeFont::FindBestAtlasSize(FT_Library& library, FT_Face& face, unsigned short fontStyleSupport, int *atlasWidth, int *atlasHeight)
{
	// preprocess all characters to try and get the smallest atlas
	renderer::TextureAtlas tmpAtlas;
	tmpAtlas.Create(m_OpenGLContext, *atlasWidth, *atlasHeight, ATLAS_DEPTH);

	int i = 0;
	for (i = 0; i < FONT_STYLE_TOTAL; ++i)
	{
		unsigned short flags = FontStyleCombinations[i];

		if (fontStyleSupport < i)
			continue;

		// This is where we actually create each of the fonts display lists.
		for (unsigned char j = 0; j < FREETYPEFONT_CHARACTERS; j++)
		{
			// The first thing we do is get FreeType to render our character
			// into a bitmap.  This actually requires a couple of FreeType commands:

			FT_Int32 filterFlags = 0;
			filterFlags = FT_LOAD_DEFAULT | FT_LOAD_IGNORE_GLOBAL_ADVANCE_WIDTH | FT_LOAD_LINEAR_DESIGN;

			if (ATLAS_DEPTH == 3)
			{
				unsigned char lcd_weights[5];

				// FT_LCD_FILTER_LIGHT   is (0x00, 0x55, 0x56, 0x55, 0x00)
				// FT_LCD_FILTER_DEFAULT is (0x10, 0x40, 0x70, 0x40, 0x10)
				lcd_weights[0] = 0x10;
				lcd_weights[1] = 0x40;
				lcd_weights[2] = 0x70;
				lcd_weights[3] = 0x40;
				lcd_weights[4] = 0x10;

				FT_Library_SetLcdFilter(library, FT_LCD_FILTER_LIGHT);
				filterFlags |= FT_LOAD_TARGET_LCD;
				if (1)
				{
					FT_Library_SetLcdFilterWeights(library, lcd_weights);
				}
			}
			else
				filterFlags |= FT_LOAD_TARGET_MONO;

			// Load the Glyph for our character.
			if (FT_Load_Glyph(face, FT_Get_Char_Index(face, j), filterFlags))
			{
				DBGLOG("FREETYPEFONT: *ERROR* FT_Load_Glyph failed");
				return 1;
			}

			// Move the face's glyph into a Glyph object.
			FT_Glyph glyph;
			FT_GlyphSlot slot = face->glyph;

			if (flags & FONT_FLAG_ITALIC)
				FT_GlyphSlot_Oblique(slot);

			if (flags & FONT_FLAG_BOLD)
				FT_GlyphSlot_Embolden(slot);

			if (FT_Get_Glyph(face->glyph, &glyph))
			{
				DBGLOG("FREETYPEFONT: *ERROR* FT_Get_Glyph failed\n");

				FT_Done_Face(face);
				FT_Done_FreeType(library);

				return 1;
			}

			// Convert the glyph to a bitmap.
			if (ATLAS_DEPTH == 1)
				FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, 0, 1);
			else
				FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_LCD, 0, 1);

			FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;

			// This reference will make accessing the bitmap easier
			FT_Bitmap &bitmap = bitmap_glyph->bitmap;

			// We want each glyph to be separated by at least one black pixel
			int bmp_w = bitmap.width / ATLAS_DEPTH + 1;
			int bmp_h = bitmap.rows + 1;
			int bmp_x = 0;
			int bmp_y = 0;

			if (bmp_w > 2 &&
				bmp_h > 2)
			{
				math::Vec4Int region = tmpAtlas.GetRegion(bmp_w, bmp_h);
				if (region.X < 0)
				{
					DBGLOG("Texture atlas (%d x %d) is full\n", *atlasWidth, *atlasHeight);

					// clear the glyph memory
					FT_Done_Glyph(glyph);

					tmpAtlas.Destroy();

                    int maxSize = m_OpenGLContext->GetMaxTextureSize();
                    
                    *atlasWidth = renderer::GetNextPowerOfTwo(*atlasWidth, maxSize);
                    *atlasHeight = renderer::GetNextPowerOfTwo(*atlasHeight, maxSize);
                    
                    if (*atlasWidth < m_MaxAtlasSize)
						return FindBestAtlasSize(library, face, fontStyleSupport, atlasWidth, atlasHeight);
					else
						return -1;
				}
				bmp_w = bmp_w - 1;
				bmp_h = bmp_h - 1;
				bmp_x = region.X;
				bmp_y = region.Y;

				tmpAtlas.SetRegion(bmp_x, bmp_y, bmp_w, bmp_h, bitmap.buffer, bitmap.pitch);
			}

			// clear the glyph memory
			FT_Done_Glyph(glyph);
		}
	}

	tmpAtlas.Destroy();

	return 0;
}

#endif // (BASE_SUPPORT_OPENGL) || defined(BASE_SUPPORT_OPENGLES)
#endif // BASE_SUPPORT_FREETYPE
