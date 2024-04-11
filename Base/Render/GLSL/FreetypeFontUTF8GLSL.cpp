
/*===================================================================
	File: FreetypeFontUTF8GLSL.cpp
	Library: Render

	(C)Hidden Games
=====================================================================*/

#ifdef BASE_SUPPORT_FREETYPE_UTF8
#ifdef BASE_SUPPORT_OPENGL_GLSL

#include <string>
#include "CoreBase.h"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "MathBase.h"
#include "Render/OpenGLCommon.h"
#include "Render/GLSL/glewES.h"
#include "Render/GLSL/OpenGLSL.h"
#include "Render/TextureShared.h"
#include "Render/Texture.h"
#include "Render/GLSL/TextureLoadAndUploadGLSL.h"
#include "Render/GLSL/ShaderShared.h"
#include "Render/GLSL/PrimitivesGLSL.h"

#include "Render/FreetypeCommon.h"
#include "Render/GLSL/FreetypeFontUTF8GLSL.h"

using renderer::FreetypeFont;

namespace
{
#ifdef BASE_PLATFORM_iOS
	const int ITALIC_EXTRA_W = 0;
#else
    const int ITALIC_EXTRA_W = 6;
#endif
    
	const int ATLAS_DEPTH = 1;

	const int ATLAS_STARTWIDTH = 512;
	const int ATLAS_STARTHEIGHT = 512;

	//const int ATLAS_MAXWIDTH = 1024;

    const float DEFAULT_UTF8_FONT_SIZE_SMALL = 72.0f;
	const float DEFAULT_UTF8_FONT_SIZE_BIG = 150.0f;

	const int NUM_CHARACTERS_PER_BATCH = 150;

	GLuint freeTypeRenderProgram = renderer::INVALID_OBJECT;
	GLuint freeTypeRenderBatchProgram = renderer::INVALID_OBJECT;
	//GLuint freeTypeUnderlineTexture = renderer::INVALID_OBJECT;
	
	const char FreetypeVertexShader[] = 
	"	//_FREETYPE_ precision highp float;\n\
		attribute vec2 base_v;\n\
		attribute vec2 base_uv0;\n\
		varying vec2 tu0;\n\
		varying vec4 colour0;\n\
		uniform mat4 ogl_ModelViewProjectionMatrix;\n\
		uniform vec4 ogl_VertexColour;\n\
		void main()\n\
		{\n\
			tu0 = base_uv0;\n\
			colour0 = ogl_VertexColour;\n\
			vec4 vInVertex = ogl_ModelViewProjectionMatrix * vec4(vec2(base_v.xy), 0.0, 1.0);\n\
			gl_Position = vInVertex;\n\
		}\n\
	";
	const char FreetypeFragmentShader[] = 
	"	//_FREETYPE_ precision highp float;\n\
		uniform sampler2D texUnit0;\n\
		varying vec2 tu0;\n\
		varying vec4 colour0;\n\
		void main()\n\
		{\n\
			vec4 color = colour0 * texture2D(texUnit0, tu0.xy).a;\n\
			gl_FragColor = color;\n\
		}\n\
	";
	
	const char FreetypeBatchVertexShader[] = 
	"	//_FREETYPEBATCH_ precision highp float;\n\
		attribute vec2 base_v;\n\
		attribute vec2 base_uv0;\n\
		attribute vec4 base_col;\n\
		varying vec2 tu0;\n\
		varying vec4 colour0;\n\
		uniform mat4 ogl_ModelViewProjectionMatrix;\n\
		uniform vec4 ogl_VertexColour;\n\
		void main()\n\
		{\n\
			tu0 = base_uv0;\n\
			colour0 = base_col;\n\
			vec4 vInVertex = ogl_ModelViewProjectionMatrix * vec4(vec2(base_v.xy), 0.0, 1.0);\n\
			gl_Position = vInVertex;\n\
		}\n\
	";
	const char FreetypeBatchFragmentShader[] = 
	"	//_FREETYPEBATCH_ precision highp float;\n\
		uniform sampler2D texUnit0;\n\
		varying vec2 tu0;\n\
		varying vec4 colour0;\n\
		void main()\n\
		{\n\
			vec4 color = colour0 * texture2D(texUnit0, tu0.xy).a;\n\
			gl_FragColor = color;\n\
		}\n\
	";
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

		freeTypeRenderProgram = renderer::LoadShaderStringsForProgram( FreetypeVertexShader, FreetypeFragmentShader );
		freeTypeRenderBatchProgram = renderer::LoadShaderStringsForProgram(FreetypeBatchVertexShader, FreetypeBatchFragmentShader);
	}
}

/////////////////////////////////////////////////////
/// Method: ShutdownFreetype
/// Params: None
///
/////////////////////////////////////////////////////
void renderer::ShutdownFreetype()
{
	if( freeTypeRenderProgram != renderer::INVALID_OBJECT )
	{
		renderer::RemoveShaderProgram( freeTypeRenderProgram );
		freeTypeRenderProgram = renderer::INVALID_OBJECT;
	}

	if (freeTypeRenderBatchProgram != renderer::INVALID_OBJECT)
	{
		renderer::RemoveShaderProgram(freeTypeRenderBatchProgram);
		freeTypeRenderBatchProgram = renderer::INVALID_OBJECT;
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

	if(  freeTypeRenderProgram == renderer::INVALID_OBJECT ||
		freeTypeRenderBatchProgram == renderer::INVALID_OBJECT )
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
	m_Library = 0;
	m_Face = 0;

	std::memset( &m_FontFile, 0, sizeof(file::TFile) );
	m_FixedWidth = false;
	m_vDimensions = math::Vec2( 10.0f, 10.0f );

	m_UseBlockColour = true;
	m_vBlockColour = math::Vec4Lite( 255,255,255,255 );

	m_UseBackgroundColour = false;
	m_vBGColour = math::Vec4Lite( 0,0,0,0 );
 
	m_vTopColour	= math::Vec4Lite( 255,255,255,255 );
	m_vBottomColour = math::Vec4Lite( 255,255,255,255 );
	m_vScale		= math::Vec2( 1.0f, 1.0f );

    m_BatchExists = false;
	m_RenderBatches = 0;
	m_TotalRenderBatches = 0;
}

/////////////////////////////////////////////////////
/// Method: Release
/// Params: None
///
/////////////////////////////////////////////////////
void FreetypeFont::Release( void )
{
	int i=0;
	
	if (m_Face != 0)
	{
		FT_Done_Face(m_Face);
		m_Face = 0;
	}

	if (m_Library != 0)
	{
		FT_Done_FreeType(m_Library);
		m_Library = 0;
	}

	if (m_RenderBatches != 0)
	{
		for (i = 0; i < m_TotalRenderBatches; ++i)
		{
            if (m_RenderBatches[i].batchBackground != 0)
            {
                delete[] m_RenderBatches[i].batchBackground;
                m_RenderBatches[i].batchBackground = 0;
            }
            
            if (m_RenderBatches[i].batchForeground != 0)
            {
                delete[] m_RenderBatches[i].batchForeground;
                m_RenderBatches[i].batchForeground = 0;
            }
		}

		delete[] m_RenderBatches;
		m_RenderBatches = 0;
	}
	m_TotalRenderBatches = 0;
    m_BatchExists = false;
    
	m_QuadMap.clear();
	m_GlyphMap.clear();

	m_AtlasTexture.Destroy();
}

/////////////////////////////////////////////////////
/// Method: Load
/// Params: [in]szFilename
///
/////////////////////////////////////////////////////
int FreetypeFont::Load(const char *szFilename)
{
	// Create and initilize a freetype font library.
	if (FT_Init_FreeType(&m_Library))
	{
		DBGLOG("FREETYPEFONT: *ERROR* FT_Init_FreeType failed");
		return(1);
	}

	// The object in which Freetype holds information on a given font is called a "face".

	CreateFileStructure(szFilename, &m_FontFile);

	// This is where we load in the font information from the file. Of all the places where the code might die, this is the most likely,
	// as FT_New_Face will die if the font file does not exist or is somehow broken.
	if (FT_New_Face(m_Library, szFilename, 0, &m_Face))
	{
		DBGLOG("FREETYPEFONT: *ERROR* FT_New_Face failed\n");
		FT_Done_FreeType(m_Library);
		return 1;
	}

	if (FT_Select_Charmap(m_Face, FT_ENCODING_UNICODE))
	{
		DBGLOG("FREETYPEFONT: *ERROR* FT_Select_Charmap failed\n");

		FT_Done_Face(m_Face);
		FT_Done_FreeType(m_Library);

		return 1;
	}

	return 0;
}

/////////////////////////////////////////////////////
/// Method: BuildCharacterMapsAndAtlas
/// Params: [in]fmtStr, [in]atlasFilename, [in]fontSize
///
/////////////////////////////////////////////////////
void FreetypeFont::BuildCharacterMapsAndAtlas(std::string& fmtStr, const std::string& atlasFilename, const float fontSize)
{
	int i = 0;
	bool missedGlyph = false;

	if (m_Library == 0 ||
		m_Face == 0)
	{
		DBGLOG("FREETYPEFONT: *ERROR* Cannot build maps and atlas\n");
		return;
	}

    m_AtlasTexture.Destroy();
    
    m_GlyphMap.clear();
    m_QuadMap.clear();
	
    std::string allChars = " !\"#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~'";
    
    allChars += fmtStr;
    
	// valid?
	/*bool validString = UTFCPP::is_valid(allChars.begin(), allChars.end());

	if (!validString)
	{
		// replace chars that are not valid
		std::string temp;
		UTFCPP::replace_invalid(allChars.begin(), allChars.end(), back_inserter(temp));
		allChars = temp;
	}*/

	auto iter = allChars.begin();
	while (iter != allChars.end())
	{
		utf8::uint32_t val = UTFCPP::next(iter/*, allChars.end()*/);
		wchar_t wideVal = (wchar_t)val;

		// search in the current list and add it if not in the map
		auto search = m_GlyphMap.find(val);
		if (search == m_GlyphMap.end()) 
		{
			// 9 == \t
			// 10 == \n
			// 13 == \r
			// 32 == space
			if (val != ASCII_VAL_TAB &&
				val != ASCII_VAL_NEWLINE &&
				val != ASCII_VAL_CR)
			{
				GlyphInfo newGlyph;
				newGlyph.rawValue = val;
				newGlyph.wideVal = wideVal;

				m_GlyphMap.insert(TValGlyphPair(val, newGlyph));
			}
		}
	}

	// now build the atlas, render at the default size
	if (m_OpenGLContext->GetMaxTextureSize() > 2048+1 )
		m_MaxAtlasSize = 4096;
	else
		m_MaxAtlasSize = 2048;

	int atlasWidth = m_MaxAtlasSize;
	int atlasHeight = m_MaxAtlasSize;

	// now the real atlas size can be created
	m_AtlasTexture.Create(m_OpenGLContext, atlasWidth, atlasHeight, ATLAS_DEPTH);

	math::Vec4Int underLineRegion = m_AtlasTexture.GetRegion(UNDERLINE_TEX_WIDTH + 1, UNDERLINE_TEX_HEIGHT + 1);
	if (underLineRegion.X < 0)
		DBGLOG("Texture atlas is full\n");
	else
		m_AtlasTexture.SetRegion(underLineRegion.X, underLineRegion.Y, UNDERLINE_TEX_WIDTH, UNDERLINE_TEX_HEIGHT, underlineTex, UNDERLINE_TEX_WIDTH);

	m_UnderlineData = math::Vec4(static_cast<float>(underLineRegion.X + 1), static_cast<float>(underLineRegion.Y + 1), static_cast<float>(UNDERLINE_TEX_WIDTH - 2), static_cast<float>(UNDERLINE_TEX_HEIGHT - 2));


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

		FT_Library_SetLcdFilter(m_Library, FT_LCD_FILTER_LIGHT);
		filterFlags |= FT_LOAD_TARGET_LCD;
		if (1)
		{
			FT_Library_SetLcdFilterWeights(m_Library, lcd_weights);
		}
	}
	else
		filterFlags |= FT_LOAD_TARGET_MONO;

	if( m_MaxAtlasSize <= 2048)
	{
		m_vDimensions.Width = DEFAULT_UTF8_FONT_SIZE_SMALL;
		m_vDimensions.Height = DEFAULT_UTF8_FONT_SIZE_SMALL;
	}
	else
	{
		m_vDimensions.Width = DEFAULT_UTF8_FONT_SIZE_BIG;
		m_vDimensions.Height = DEFAULT_UTF8_FONT_SIZE_BIG;
	}

	FT_Set_Pixel_Sizes(m_Face, static_cast<int>(m_vDimensions.Width), static_cast<int>(m_vDimensions.Height));

	auto glyphIt = m_GlyphMap.begin();
	while (glyphIt != m_GlyphMap.end())
	{
		// build a new array each time per quad
		QuadArray newArray;

		// Load the Glyph for our character.
		FT_UInt glyphIndex = FT_Get_Char_Index(m_Face, glyphIt->second.wideVal);
		//DBG_ASSERT(glyphIndex != 0);

		if (FT_Load_Glyph(m_Face, glyphIndex, filterFlags))
		{
			DBGLOG("FREETYPEFONT: *ERROR* FT_Load_Glyph failed");
			return;
		}

		// Move the face's glyph into a Glyph object.
		FT_Glyph tmpGlyph;
		FT_GlyphSlot tmpSlot = m_Face->glyph;

		for (i = 0; i < FONT_STYLE_TOTAL; ++i)
		{
			unsigned short flags = FontStyleCombinations[i];

			if (m_MaxAtlasSize <= 2048)
			{
				m_vDimensions.Width = DEFAULT_UTF8_FONT_SIZE_SMALL;
				m_vDimensions.Height = DEFAULT_UTF8_FONT_SIZE_SMALL;
			}
			else
			{
				m_vDimensions.Width = DEFAULT_UTF8_FONT_SIZE_BIG;
				m_vDimensions.Height = DEFAULT_UTF8_FONT_SIZE_BIG;
			}

			if (flags & FONT_FLAG_ITALIC)
				FT_GlyphSlot_Oblique(tmpSlot);

			if (flags & FONT_FLAG_BOLD)
				FT_GlyphSlot_Embolden(tmpSlot);

			if (FT_Get_Glyph(m_Face->glyph, &tmpGlyph))
			{
				DBGLOG("FREETYPEFONT: *ERROR* FT_Get_Glyph failed\n");
				return;
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
					if (!missedGlyph)
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

			newArray[i].charDataUV = math::Vec4(static_cast<float>(bmp_x), static_cast<float>(bmp_y), static_cast<float>(tmpBitmap.width), static_cast<float>(tmpBitmap.rows));

			// set current glyph values to scale
			newArray[i].atlasGlyphDims = m_vDimensions;

			newArray[i].faceGlyphAdvance = (float)(m_Face->glyph->advance.x >> 6);

			newArray[i].bitmapGlyphLeft = (float)tmpBitmapGlyph->left;
			newArray[i].bitmapGlyphTop = (float)tmpBitmapGlyph->top;
			newArray[i].bitmapRows = (float)tmpBitmap.rows;
			newArray[i].bitmapWidth = (float)tmpBitmap.width;

			//////////////// ****************

			// now scale
			float scaleValue = fontSize / (float)newArray[i].atlasGlyphDims.Width;

			float faceGlyphAdvance = newArray[i].faceGlyphAdvance * scaleValue;

			float bitmapGlyphLeft = newArray[i].bitmapGlyphLeft * scaleValue;
			float bitmapGlyphTop = newArray[i].bitmapGlyphTop * scaleValue;
			float bitmapRows = newArray[i].bitmapRows * scaleValue;
			float bitmapWidth = newArray[i].bitmapWidth * scaleValue;

			// reset the font size to what it should be
			m_vDimensions.Width = fontSize;
			m_vDimensions.Height = fontSize;

			// save the glyph width, to use for space counting
			if (flags & FONT_FLAG_ITALIC)
				newArray[i].glyphWidth = (faceGlyphAdvance) + (ITALIC_EXTRA_W);
			else
				newArray[i].glyphWidth = faceGlyphAdvance;

			// create triangle strip
			// 2 ---- 3
			// |\     |
			// |  \   |
			// |    \ |
			// 0 ---- 1
			if (m_FixedWidth)
				newArray[i].Offsets[0] = math::Vec3(static_cast<float>(bitmapGlyphLeft)+m_vDimensions.Width*0.25f, 0.0f, 0.0f);
			else
				newArray[i].Offsets[0] = math::Vec3(static_cast<float>(bitmapGlyphLeft), 0.0f, 0.0f);
			newArray[i].Offsets[1] = math::Vec3(0.0f, static_cast<float>(bitmapGlyphTop)-static_cast<float>(bitmapRows), 0.0f);

			newArray[i].ShadowOffset = math::Vec3(m_vDimensions.Width / FONT_DROP_SHADOW_SHIFT, -m_vDimensions.Height / FONT_DROP_SHADOW_SHIFT, 0.0f);

			float UNDERLINE_HEIGHT = m_vDimensions.Height*0.075f;
			float UNDERLINE_INC_W = m_vDimensions.Width*0.055f;

			float BG_INC_H = m_vDimensions.Height*0.025f;
			float BG_INC_W = m_vDimensions.Width*0.015f;

			math::Vec4Lite vBottomColour(255, 255, 255, 255);
			math::Vec4Lite vTopColour(255, 255, 255, 255);

			newArray[i].charDataSize = math::Vec2(static_cast<float>(bitmapWidth), static_cast<float>(bitmapRows));
			float glw = static_cast<float>(newArray[i].glyphWidth);

			newArray[i].textUV[0] = math::Vec2(newArray[i].charDataUV.X / static_cast<float>(atlasWidth), (newArray[i].charDataUV.Y + newArray[i].charDataUV.Height) / static_cast<float>(atlasHeight));
			newArray[i].textPoint[0] = math::Vec2(0.0f, 0.0f);
			newArray[i].textColour[0] = vBottomColour;
			newArray[i].textShadowColour[0] = math::Vec4Lite(0, 0, 0, 255);
			newArray[i].textUnderline[0] = math::Vec2(-UNDERLINE_INC_W, 0.0f);
			newArray[i].textUnderlineUV[0] = math::Vec2(m_UnderlineData.X / static_cast<float>(atlasWidth), (m_UnderlineData.Y + m_UnderlineData.Height) / static_cast<float>(atlasHeight));
			newArray[i].textBGBox[0] = math::Vec2(-(BG_INC_W), -BG_INC_H);
			newArray[i].fillUV[0] = math::Vec2(0.0f, 0.0f);

			newArray[i].textUV[1] = math::Vec2((newArray[i].charDataUV.X + newArray[i].charDataUV.Width) / static_cast<float>(atlasWidth), (newArray[i].charDataUV.Y + newArray[i].charDataUV.Height) / static_cast<float>(atlasHeight));
			newArray[i].textPoint[1] = math::Vec2(newArray[i].charDataSize.Width, 0.0f);
			newArray[i].textColour[1] = vBottomColour;
			newArray[i].textShadowColour[1] = math::Vec4Lite(0, 0, 0, 255);
			newArray[i].textUnderline[1] = math::Vec2(glw + UNDERLINE_INC_W, 0.0f);
			newArray[i].textUnderlineUV[1] = math::Vec2((m_UnderlineData.X + m_UnderlineData.Width) / static_cast<float>(atlasWidth), (m_UnderlineData.Y + m_UnderlineData.Height) / static_cast<float>(atlasHeight));
			newArray[i].textBGBox[1] = math::Vec2(glw + BG_INC_W, -BG_INC_H);
			newArray[i].fillUV[1] = math::Vec2(1.0f, 0.0f);

			newArray[i].textUV[2] = math::Vec2(newArray[i].charDataUV.X / static_cast<float>(atlasWidth), newArray[i].charDataUV.Y / static_cast<float>(atlasHeight));
			newArray[i].textPoint[2] = math::Vec2(0.0f, newArray[i].charDataSize.Height);
			newArray[i].textColour[2] = vTopColour;
			newArray[i].textShadowColour[2] = math::Vec4Lite(0, 0, 0, 255);
			newArray[i].textUnderline[2] = math::Vec2(-UNDERLINE_INC_W, UNDERLINE_HEIGHT);
			newArray[i].textUnderlineUV[2] = math::Vec2(m_UnderlineData.X / static_cast<float>(atlasWidth), m_UnderlineData.Y / static_cast<float>(atlasHeight));
			newArray[i].textBGBox[2] = math::Vec2(-(BG_INC_W), (m_vDimensions.Height*0.95f));
			newArray[i].fillUV[2] = math::Vec2(0.0f, 1.0f);

			newArray[i].textUV[3] = math::Vec2((newArray[i].charDataUV.X + newArray[i].charDataUV.Width) / static_cast<float>(atlasWidth), newArray[i].charDataUV.Y / static_cast<float>(atlasHeight));
			newArray[i].textPoint[3] = math::Vec2(newArray[i].charDataSize.Width, newArray[i].charDataSize.Height);
			newArray[i].textColour[3] = vTopColour;
			newArray[i].textShadowColour[3] = math::Vec4Lite(0, 0, 0, 255);
			newArray[i].textUnderline[3] = math::Vec2(glw + UNDERLINE_INC_W, UNDERLINE_HEIGHT);
			newArray[i].textUnderlineUV[3] = math::Vec2((m_UnderlineData.X + m_UnderlineData.Width) / static_cast<float>(atlasWidth), m_UnderlineData.Y / static_cast<float>(atlasHeight));
			newArray[i].textBGBox[3] = math::Vec2(glw + BG_INC_W, (m_vDimensions.Height*0.95f));
			newArray[i].fillUV[3] = math::Vec2(1.0f, 1.0f);

			if (m_FixedWidth)
				newArray[i].Offsets[2] = math::Vec3(m_vDimensions.Width*0.75f, 0.0f, 0.0f);
			else
				newArray[i].Offsets[2] = math::Vec3(glw, 0.0f, 0.0f);
		}

		// clear the glyph memory
		FT_Done_Glyph(tmpGlyph);

		// add the final array to the map
		m_QuadMap.insert(TValQuadPair(glyphIt->first, newArray));

		// next glyph
		glyphIt++;
	}

	// save the atals to a file
	//file::TFile fileAtlasStruct;
	//file::CreateFileStructure(atlasFilename.c_str(), &fileAtlasStruct);
	//m_AtlasTexture.WriteToFile(atlasFilename.c_str());
}

void FreetypeFont::UpdateWithSize( float fontSize )
{
	int i = 0;

	// reset the font size to what it should be
	m_vDimensions.Width = fontSize;
	m_vDimensions.Height = fontSize;

	auto glyphIt = m_GlyphMap.begin();
	while (glyphIt != m_GlyphMap.end())
	{
		auto quadMapIter = m_QuadMap.find(glyphIt->first);
		if (quadMapIter == m_QuadMap.end())
			DBG_ASSERT(0);

		auto& currQuad = quadMapIter->second;

		for (i = 0; i < FONT_STYLE_TOTAL; ++i)
		{
			unsigned short flags = FontStyleCombinations[i];

			float scaleValue = fontSize / (float)currQuad[i].atlasGlyphDims.Width;

			float faceGlyphAdvance = currQuad[i].faceGlyphAdvance * scaleValue;

			float bitmapGlyphLeft = currQuad[i].bitmapGlyphLeft * scaleValue;
			float bitmapGlyphTop = currQuad[i].bitmapGlyphTop * scaleValue;
			float bitmapRows = currQuad[i].bitmapRows * scaleValue;
			float bitmapWidth = currQuad[i].bitmapWidth * scaleValue;

			// save the glyph width, to use for space counting
			if (flags & FONT_FLAG_ITALIC)
				currQuad[i].glyphWidth = (faceGlyphAdvance) + (ITALIC_EXTRA_W);
			else
				currQuad[i].glyphWidth = faceGlyphAdvance;

			// create triangle strip
			// 2 ---- 3
			// |\     |
			// |  \   |
			// |    \ |
			// 0 ---- 1
			if (m_FixedWidth)
				currQuad[i].Offsets[0] = math::Vec3(static_cast<float>(bitmapGlyphLeft)+m_vDimensions.Width*0.25f, 0.0f, 0.0f);
			else
				currQuad[i].Offsets[0] = math::Vec3(static_cast<float>(bitmapGlyphLeft), 0.0f, 0.0f);
			currQuad[i].Offsets[1] = math::Vec3(0.0f, static_cast<float>(bitmapGlyphTop)-static_cast<float>(bitmapRows), 0.0f);

			currQuad[i].ShadowOffset = math::Vec3(m_vDimensions.Width / FONT_DROP_SHADOW_SHIFT, -m_vDimensions.Height / FONT_DROP_SHADOW_SHIFT, 0.0f);

			float UNDERLINE_HEIGHT = m_vDimensions.Height*0.075f;
			float UNDERLINE_INC_W = m_vDimensions.Width*0.055f;

			float BG_INC_H = m_vDimensions.Height*0.025f;
			float BG_INC_W = m_vDimensions.Width*0.015f;

			math::Vec4Lite vBottomColour(255, 255, 255, 255);
			math::Vec4Lite vTopColour(255, 255, 255, 255);

			currQuad[i].charDataSize = math::Vec2(static_cast<float>(bitmapWidth), static_cast<float>(bitmapRows));
			float glw = static_cast<float>(currQuad[i].glyphWidth);

			currQuad[i].textUV[0] = math::Vec2(currQuad[i].charDataUV.X / static_cast<float>(m_MaxAtlasSize), (currQuad[i].charDataUV.Y + currQuad[i].charDataUV.Height) / static_cast<float>(m_MaxAtlasSize));
			currQuad[i].textPoint[0] = math::Vec2(0.0f, 0.0f);
			currQuad[i].textColour[0] = vBottomColour;
			currQuad[i].textShadowColour[0] = math::Vec4Lite(0, 0, 0, 255);
			currQuad[i].textUnderline[0] = math::Vec2(-UNDERLINE_INC_W, 0.0f);
			currQuad[i].textUnderlineUV[0] = math::Vec2(m_UnderlineData.X / static_cast<float>(m_MaxAtlasSize), (m_UnderlineData.Y + m_UnderlineData.Height) / static_cast<float>(m_MaxAtlasSize));
			currQuad[i].textBGBox[0] = math::Vec2(-(BG_INC_W), -BG_INC_H);
			currQuad[i].fillUV[0] = math::Vec2(0.0f, 0.0f);

			currQuad[i].textUV[1] = math::Vec2((currQuad[i].charDataUV.X + currQuad[i].charDataUV.Width) / static_cast<float>(m_MaxAtlasSize), (currQuad[i].charDataUV.Y + currQuad[i].charDataUV.Height) / static_cast<float>(m_MaxAtlasSize));
			currQuad[i].textPoint[1] = math::Vec2(currQuad[i].charDataSize.Width, 0.0f);
			currQuad[i].textColour[1] = vBottomColour;
			currQuad[i].textShadowColour[1] = math::Vec4Lite(0, 0, 0, 255);
			currQuad[i].textUnderline[1] = math::Vec2(glw + UNDERLINE_INC_W, 0.0f);
			currQuad[i].textUnderlineUV[1] = math::Vec2((m_UnderlineData.X + m_UnderlineData.Width) / static_cast<float>(m_MaxAtlasSize), (m_UnderlineData.Y + m_UnderlineData.Height) / static_cast<float>(m_MaxAtlasSize));
			currQuad[i].textBGBox[1] = math::Vec2(glw + BG_INC_W, -BG_INC_H);
			currQuad[i].fillUV[1] = math::Vec2(1.0f, 0.0f);

			currQuad[i].textUV[2] = math::Vec2(currQuad[i].charDataUV.X / static_cast<float>(m_MaxAtlasSize), currQuad[i].charDataUV.Y / static_cast<float>(m_MaxAtlasSize));
			currQuad[i].textPoint[2] = math::Vec2(0.0f, currQuad[i].charDataSize.Height);
			currQuad[i].textColour[2] = vTopColour;
			currQuad[i].textShadowColour[2] = math::Vec4Lite(0, 0, 0, 255);
			currQuad[i].textUnderline[2] = math::Vec2(-UNDERLINE_INC_W, UNDERLINE_HEIGHT);
			currQuad[i].textUnderlineUV[2] = math::Vec2(m_UnderlineData.X / static_cast<float>(m_MaxAtlasSize), m_UnderlineData.Y / static_cast<float>(m_MaxAtlasSize));
			currQuad[i].textBGBox[2] = math::Vec2(-(BG_INC_W), (m_vDimensions.Height*0.95f));
			currQuad[i].fillUV[2] = math::Vec2(0.0f, 1.0f);

			currQuad[i].textUV[3] = math::Vec2((currQuad[i].charDataUV.X + currQuad[i].charDataUV.Width) / static_cast<float>(m_MaxAtlasSize), currQuad[i].charDataUV.Y / static_cast<float>(m_MaxAtlasSize));
			currQuad[i].textPoint[3] = math::Vec2(currQuad[i].charDataSize.Width, currQuad[i].charDataSize.Height);
			currQuad[i].textColour[3] = vTopColour;
			currQuad[i].textShadowColour[3] = math::Vec4Lite(0, 0, 0, 255);
			currQuad[i].textUnderline[3] = math::Vec2(glw + UNDERLINE_INC_W, UNDERLINE_HEIGHT);
			currQuad[i].textUnderlineUV[3] = math::Vec2((m_UnderlineData.X + m_UnderlineData.Width) / static_cast<float>(m_MaxAtlasSize), m_UnderlineData.Y / static_cast<float>(m_MaxAtlasSize));
			currQuad[i].textBGBox[3] = math::Vec2(glw + BG_INC_W, (m_vDimensions.Height*0.95f));
			currQuad[i].fillUV[3] = math::Vec2(1.0f, 1.0f);

			if (m_FixedWidth)
				currQuad[i].Offsets[2] = math::Vec3(m_vDimensions.Width*0.75f, 0.0f, 0.0f);
			else
				currQuad[i].Offsets[2] = math::Vec3(glw, 0.0f, 0.0f);
		}

		// next glyph
		glyphIt++;
	}
}

/////////////////////////////////////////////////////
/// Method: Load
/// Params: [in]szFilename, [in]pData, [in]dataSize [in]vDims, [in]bDropShadow, [in]vTopColour, [in]vBottomColour
///
/////////////////////////////////////////////////////
/*int FreetypeFont::Load(const char *szFilename, void *pData, std::size_t dataSize, const math::Vec2 &vDims, const math::Vec4Lite &vTopColour, const math::Vec4Lite &vBottomColour, bool fixedWidth, unsigned short fontStyleSupport, int maxAtlasSize, int maxGlyphSize )
{
	int i=0;
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
	
	if (szFilename != 0)
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
    
    if (m_vDimensions.Width > maxGlyphSize ||
        m_vDimensions.Height > maxGlyphSize)
    {
        m_vDimensions.Width = maxGlyphSize;
        m_vDimensions.Height = maxGlyphSize;
    }
    
	FT_Set_Pixel_Sizes( face, static_cast<int>(m_vDimensions.Width), static_cast<int>(m_vDimensions.Height) );

    m_MaxAtlasSize = maxAtlasSize;
    
	int atlasWidth = ATLAS_STARTWIDTH;
	int atlasHeight = ATLAS_STARTHEIGHT;
	int result = FindBestAtlasSize(library, face, fontStyleSupport, &atlasWidth, &atlasHeight);

	if (result == -1)
		DBGLOG("FREETYPEFONT: *WARNING* Atlas is too big some characters may not appear for %s\n", szFilename );

	// now the real atlas size can be created
	m_AtlasTexture.Create( m_OpenGLContext, atlasWidth, atlasHeight, ATLAS_DEPTH);

	math::Vec4Int underLineRegion = m_AtlasTexture.GetRegion(UNDERLINE_TEX_WIDTH + 1, UNDERLINE_TEX_HEIGHT + 1);
	if (underLineRegion.X < 0)
		DBGLOG("Texture atlas is full\n");
	else
		m_AtlasTexture.SetRegion(underLineRegion.X, underLineRegion.Y, UNDERLINE_TEX_WIDTH, UNDERLINE_TEX_HEIGHT, underlineTex, UNDERLINE_TEX_WIDTH);
	math::Vec4 underlineData = math::Vec4(static_cast<float>(underLineRegion.X+1), static_cast<float>(underLineRegion.Y+1), static_cast<float>(UNDERLINE_TEX_WIDTH-2), static_cast<float>(UNDERLINE_TEX_HEIGHT-2));

    m_vDimensions = modifiedFontSize;
    
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
            
            if( m_vDimensions.Width > maxGlyphSize ||
               m_vDimensions.Height > maxGlyphSize )
            {
                m_vDimensions.Width = maxGlyphSize;
                m_vDimensions.Height = maxGlyphSize;
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
				DBGLOG( "FREETYPEFONT: *ERROR* FT_Load_Glyph failed" );
				return( 1 );
			}
			
			// Move the face's glyph into a Glyph object.
            FT_Glyph tmpGlyph;
            FT_GlyphSlot tmpSlot = face->glyph;
			
			if( flags & FONT_FLAG_ITALIC )
				FT_GlyphSlot_Oblique( tmpSlot );
			
			if( flags & FONT_FLAG_BOLD )
				FT_GlyphSlot_Embolden( tmpSlot );
			
			if( FT_Get_Glyph( face->glyph, &tmpGlyph ) )
			{
				DBGLOG( "FREETYPEFONT: *ERROR* FT_Get_Glyph failed\n" );
				return( 1 );
			}
			
			// Convert the glyph to a bitmap.
			if (ATLAS_DEPTH == 1)
				FT_Glyph_To_Bitmap( &tmpGlyph, FT_RENDER_MODE_NORMAL, 0, 1 );
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
				m_GlyphWidths[i][j] = (face->glyph->advance.x >> 6) + (ITALIC_EXTRA_W );
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
			FT_Done_Glyph( glyph );
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

int FreetypeFont::Load(const char *szFilename, const math::Vec2 &vDims, const math::Vec4Lite &vTopColour, const math::Vec4Lite &vBottomColour, bool fixedWidth, int maxGlyphSize )
{
    m_vDimensions = vDims;
    
    int glphSize = (int)vDims.Height;
    
    if (m_vDimensions.Width > maxGlyphSize ||
        m_vDimensions.Height > maxGlyphSize)
    {
        glphSize = maxGlyphSize;
    }
    
    m_vBlockColour = vTopColour;
    m_FixedWidth = fixedWidth;
    
    // colour store
    m_vTopColour = vTopColour;
    m_vBottomColour = vBottomColour;
    
    char szInputAtlasName[core::MAX_PATH+core::MAX_PATH];
    std::memset( szInputAtlasName, 0, sizeof(char)*core::MAX_PATH+core::MAX_PATH );
    
    snprintf(szInputAtlasName, core::MAX_PATH+core::MAX_PATH, "%s%d.png", szFilename, glphSize );
    
    char szInputDataName[core::MAX_PATH+core::MAX_PATH];
    std::memset( szInputDataName, 0, sizeof(char)*core::MAX_PATH+core::MAX_PATH );
    
    snprintf(szInputDataName, core::MAX_PATH+core::MAX_PATH, "%s%d.uv", szFilename, (int)vDims.Height );
    
    ReadAtlasAndData( szInputAtlasName, szInputDataName );
    
    return 0;
}

/////////////////////////////////////////////////////
/// Method: WriteAtlasAndData
/// Params: None
///
/////////////////////////////////////////////////////
void FreetypeFont::WriteAtlasAndData( const char* szOutputAtlasName, const char* szOutputDataName, int fontSize, int maxGlyphSize )
{
    file::TFile fileAtlasStruct;
    file::CreateFileStructure( szOutputAtlasName, &fileAtlasStruct);
    
    if( fontSize <= maxGlyphSize )
        m_AtlasTexture.WriteToFile(szOutputAtlasName);
    
    file::TFile fileDataStruct;
    file::CreateFileStructure( szOutputDataName, &fileDataStruct);
    
    file::TFileHandle fileHandle;
    if( file::FileOpen( szOutputDataName, file::FILETYPE_BINARY_WRITE, &fileHandle ) )
    {
        int i=0, j=0, k=0;
        for( i=0; i < FONT_STYLE_TOTAL; ++i )
        {
            for( j=0; j < FREETYPEFONT_CHARACTERS; ++j )
            {
                file::FileWriteUInt(&m_GlyphWidths[i][j], sizeof(int), 1, &fileHandle);
                
                for( k=0; k < 3; ++k )
                    file::FileWriteVec3D(&GlyphQuad[i][j].Offsets[k], sizeof(math::Vec3), 1, &fileHandle);
                
                file::FileWriteVec3D(&GlyphQuad[i][j].ShadowOffset, sizeof(math::Vec3), 1, &fileHandle);
                
                for( k=0; k < 4; ++k )
                {
                    file::FileWriteVec2D(&GlyphQuad[i][j].textUV[k], sizeof(math::Vec2), 1, &fileHandle);
                    file::FileWriteVec2D(&GlyphQuad[i][j].textPoint[k], sizeof(math::Vec2), 1, &fileHandle);
                    
                    file::FileWriteUInt(&GlyphQuad[i][j].textColour[k], sizeof(int), 1, &fileHandle);
                    file::FileWriteUInt(&GlyphQuad[i][j].textShadowColour[k], sizeof(int), 1, &fileHandle);
                    
                    file::FileWriteVec2D(&GlyphQuad[i][j].textUnderline[k], sizeof(math::Vec2), 1, &fileHandle);
                    file::FileWriteVec2D(&GlyphQuad[i][j].textUnderlineUV[k], sizeof(math::Vec2), 1, &fileHandle);
                    
                    file::FileWriteVec2D(&GlyphQuad[i][j].textBGBox[k], sizeof(math::Vec2), 1, &fileHandle);
                    
                    file::FileWriteUInt(&GlyphQuad[i][j].textBGColour[k], sizeof(int), 1, &fileHandle);
                    
                    file::FileWriteVec2D(&GlyphQuad[i][j].fillUV[k], sizeof(math::Vec2), 1, &fileHandle);
                }
            }
        }
        
        file::FileClose(&fileHandle);
    }
}


/////////////////////////////////////////////////////
/// Method: ReadAtlasAndData
/// Params: None
///
/////////////////////////////////////////////////////
void FreetypeFont::ReadAtlasAndData( const char* szOutputAtlasName, const char* szOutputDataName )
{
    file::TFile fileAtlasStruct;
    file::CreateFileStructure( szOutputAtlasName, &fileAtlasStruct);
    
    m_AtlasTexture.SetTexture(szOutputAtlasName, m_OpenGLContext);
    
    file::TFile fileDataStruct;
    file::CreateFileStructure( szOutputDataName, &fileDataStruct);
    
    file::TFileHandle fileHandle;
    if( file::FileOpen( szOutputDataName, file::FILETYPE_BINARY_READ, &fileHandle ) )
    {
        int i=0, j=0, k=0;
        for( i=0; i < FONT_STYLE_TOTAL; ++i )
        {
            for( j=0; j < FREETYPEFONT_CHARACTERS; ++j )
            {
                file::FileReadUInt(&m_GlyphWidths[i][j], sizeof(int), 1, &fileHandle);
                
                for( k=0; k < 3; ++k )
                    file::FileReadVec3D(&GlyphQuad[i][j].Offsets[k], sizeof(math::Vec3), 1, &fileHandle);
                
                file::FileReadVec3D(&GlyphQuad[i][j].ShadowOffset, sizeof(math::Vec3), 1, &fileHandle);
                
                for( k=0; k < 4; ++k )
                {
                    file::FileReadVec2D(&GlyphQuad[i][j].textUV[k], sizeof(math::Vec2), 1, &fileHandle);
                    file::FileReadVec2D(&GlyphQuad[i][j].textPoint[k], sizeof(math::Vec2), 1, &fileHandle);
                    
                    file::FileReadUInt(&GlyphQuad[i][j].textColour[k], sizeof(int), 1, &fileHandle);
                    file::FileReadUInt(&GlyphQuad[i][j].textShadowColour[k], sizeof(int), 1, &fileHandle);
                    
                    file::FileReadVec2D(&GlyphQuad[i][j].textUnderline[k], sizeof(math::Vec2), 1, &fileHandle);
                    file::FileReadVec2D(&GlyphQuad[i][j].textUnderlineUV[k], sizeof(math::Vec2), 1, &fileHandle);
                    
                    file::FileReadVec2D(&GlyphQuad[i][j].textBGBox[k], sizeof(math::Vec2), 1, &fileHandle);
                    
                    file::FileReadUInt(&GlyphQuad[i][j].textBGColour[k], sizeof(int), 1, &fileHandle);
                    file::FileReadVec2D(&GlyphQuad[i][j].fillUV[k], sizeof(math::Vec2), 1, &fileHandle);
                }
            }
        }
        
        file::FileClose(&fileHandle);
    }
}*/

/////////////////////////////////////////////////////
/// Method: ClearBatches
/// Params: None
///
/////////////////////////////////////////////////////
void FreetypeFont::ClearBatches()
{
    unsigned int i = 0;
    
    if (m_RenderBatches != 0)
    {
        for (i = 0; i < static_cast<unsigned int>(m_TotalRenderBatches); ++i)
        {
            if (m_RenderBatches[i].batchBackground != 0)
            {
                delete[] m_RenderBatches[i].batchBackground;
                m_RenderBatches[i].batchBackground = 0;
            }
            
            if (m_RenderBatches[i].batchForeground != 0)
            {
                delete[] m_RenderBatches[i].batchForeground;
                m_RenderBatches[i].batchForeground = 0;
            }
        }
        
        delete[] m_RenderBatches;
        m_RenderBatches = 0;
    }
    m_TotalRenderBatches = 0;
    
    m_BatchExists = false;
}

/////////////////////////////////////////////////////
/// Method: BuildBufferCache
/// Params: [in]szFilename, [in]pData, [in]dataSize [in]vDims, [in]bDropShadow, [in]vTopColour, [in]vBottomColour
///
/////////////////////////////////////////////////////
/*void FreetypeFont::BuildBufferCache(int x, int y, bool dropShadow, std::vector<TFormatChangeBlock>& formatChanges, std::string fmtStr, ...)
{
	int style = FONT_STYLE_NORMAL;
	unsigned int i = 0, j = 0, k = 0;
	std::string s, ss;
	int n, size = 100;
	bool b = false;
	va_list marker;

	if (freeTypeRenderBatchProgram == renderer::INVALID_OBJECT ||
		freeTypeRenderBatchProgram == 0)
		return;

	if (fmtStr.length() == 0)
		return;

#if !defined(BASE_PLATFORM_ANDROID)	
	while (!b)
	{
		ss.resize(size);
		va_start(marker, fmtStr);
		n = vsnprintf((char*)ss.c_str(), size, fmtStr.c_str(), marker);
		va_end(marker);
		if ((n>0) && ((b = (n<size)) == true))
			ss.resize(n);
		else
			size *= 2;
	}
	s += ss;
#else
	s = fmtStr;
#endif
	glm::mat4 modelMat = m_OpenGLContext->GetModelMatrix();

	float xOffsetTotal = 0.0f;

	// initial block
	TFormatChangeBlock block;
	block.style = FONT_STYLE_NORMAL;
	block.numCharsInBlockText = static_cast<int>(s.length());
	block.textColour = m_vBlockColour;
	block.underline = false;
	block.bgColour = false;
	block.textBGColour = math::Vec4Lite(0, 0, 0, 0);

	bool hasChanges = false;
	bool doUnderline = false;
	bool doBackground = false;

	int currentChangeIndex = 0;

	if (formatChanges.size() != 0)
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

	// set the start point
	glm::mat4 offsetMatrix = glm::translate(modelMat, glm::vec3(static_cast<GLfloat>(x), static_cast<GLfloat>(y), 0.0f));

	if (s.length() == 0)
		return;

	if (m_RenderBatches != 0)
	{
		for (i = 0; i < static_cast<unsigned int>(m_TotalRenderBatches); ++i)
		{
            if (m_RenderBatches[i].batchBackground != 0)
            {
                delete[] m_RenderBatches[i].batchBackground;
                m_RenderBatches[i].batchBackground = 0;
            }
            
			if (m_RenderBatches[i].batchForeground != 0)
			{
				delete[] m_RenderBatches[i].batchForeground;
				m_RenderBatches[i].batchForeground = 0;
			}
		}

		delete[] m_RenderBatches;
		m_RenderBatches = 0;
	}
	m_TotalRenderBatches = 0;

	m_TotalRenderBatches = ((int)s.length() / NUM_CHARACTERS_PER_BATCH) + 1;

	m_RenderBatches = new FreetypeRenderBatch[m_TotalRenderBatches];
	DBG_ASSERT(m_RenderBatches != 0);

	// init all data inside
	for (i = 0; i < static_cast<unsigned int>(m_TotalRenderBatches); ++i)
	{
		FreetypeRenderBatch* pCurrentBatch = &m_RenderBatches[i];
		
		pCurrentBatch->numberOfCharacters = 0;
		pCurrentBatch->bufferAABB.Reset();
		pCurrentBatch->bufferAABB.vBoxMax.Z = 0.0f;
		pCurrentBatch->bufferAABB.vBoxMin.Z = 0.0f;

        pCurrentBatch->batchBackground = new GlyphVert[(2*6)*NUM_CHARACTERS_PER_BATCH]; // 2 triangles with 6 vertices and 2 versions for bg, shadow
        DBG_ASSERT(pCurrentBatch->batchBackground != 0);

		pCurrentBatch->batchForeground = new GlyphVert[(2*6)*NUM_CHARACTERS_PER_BATCH]; // 2 triangles with 6 vertices and 2 versions for glyph and underline
		DBG_ASSERT(pCurrentBatch->batchForeground != 0);

		int triOffset = 0;

		for (j = 0; j < NUM_CHARACTERS_PER_BATCH; ++j)
		{
			for (k = 0; k < 2; ++k) // 2 versions for bg, shadow
			{
				// vert init
				// glyph quad = 2 tris = 6 verts
				pCurrentBatch->batchBackground[triOffset].v = math::Vec2(0.0f, 0.0f);
				pCurrentBatch->batchBackground[triOffset + 1].v = math::Vec2(0.0f, 0.0f);
				pCurrentBatch->batchBackground[triOffset + 2].v = math::Vec2(0.0f, 0.0f);

				pCurrentBatch->batchBackground[triOffset + 3].v = math::Vec2(0.0f, 0.0f);
				pCurrentBatch->batchBackground[triOffset + 4].v = math::Vec2(0.0f, 0.0f);
				pCurrentBatch->batchBackground[triOffset + 5].v = math::Vec2(0.0f, 0.0f);
				///

				// texture coords
				pCurrentBatch->batchBackground[triOffset].uv = math::Vec2(0.0f, 0.0f);
				pCurrentBatch->batchBackground[triOffset + 1].uv = math::Vec2(0.0f, 0.0f);
				pCurrentBatch->batchBackground[triOffset + 2].uv = math::Vec2(0.0f, 0.0f);

				pCurrentBatch->batchBackground[triOffset + 3].uv = math::Vec2(0.0f, 0.0f);
				pCurrentBatch->batchBackground[triOffset + 4].uv = math::Vec2(0.0f, 0.0f);
				pCurrentBatch->batchBackground[triOffset + 5].uv = math::Vec2(0.0f, 0.0f);

				// colour init
				pCurrentBatch->batchBackground[triOffset].col = math::Vec4Lite(0, 0, 0, 0);
				pCurrentBatch->batchBackground[triOffset + 1].col = math::Vec4Lite(0, 0, 0, 0);
				pCurrentBatch->batchBackground[triOffset + 2].col = math::Vec4Lite(0, 0, 0, 0);

				pCurrentBatch->batchBackground[triOffset + 3].col = math::Vec4Lite(0, 0, 0, 0);
				pCurrentBatch->batchBackground[triOffset + 4].col = math::Vec4Lite(0, 0, 0, 0);
				pCurrentBatch->batchBackground[triOffset + 5].col = math::Vec4Lite(0, 0, 0, 0);
                
                
                // glyph quad = 2 tris = 6 verts
                pCurrentBatch->batchForeground[triOffset].v = math::Vec2(0.0f, 0.0f);
                pCurrentBatch->batchForeground[triOffset + 1].v = math::Vec2(0.0f, 0.0f);
                pCurrentBatch->batchForeground[triOffset + 2].v = math::Vec2(0.0f, 0.0f);
                
                pCurrentBatch->batchForeground[triOffset + 3].v = math::Vec2(0.0f, 0.0f);
                pCurrentBatch->batchForeground[triOffset + 4].v = math::Vec2(0.0f, 0.0f);
                pCurrentBatch->batchForeground[triOffset + 5].v = math::Vec2(0.0f, 0.0f);
                ///
                
                // texture coords
                pCurrentBatch->batchForeground[triOffset].uv = math::Vec2(0.0f, 0.0f);
                pCurrentBatch->batchForeground[triOffset + 1].uv = math::Vec2(0.0f, 0.0f);
                pCurrentBatch->batchForeground[triOffset + 2].uv = math::Vec2(0.0f, 0.0f);
                
                pCurrentBatch->batchForeground[triOffset + 3].uv = math::Vec2(0.0f, 0.0f);
                pCurrentBatch->batchForeground[triOffset + 4].uv = math::Vec2(0.0f, 0.0f);
                pCurrentBatch->batchForeground[triOffset + 5].uv = math::Vec2(0.0f, 0.0f);
                
                // colour init
                pCurrentBatch->batchForeground[triOffset].col = math::Vec4Lite(0, 0, 0, 0);
                pCurrentBatch->batchForeground[triOffset + 1].col = math::Vec4Lite(0, 0, 0, 0);
                pCurrentBatch->batchForeground[triOffset + 2].col = math::Vec4Lite(0, 0, 0, 0);
                
                pCurrentBatch->batchForeground[triOffset + 3].col = math::Vec4Lite(0, 0, 0, 0);
                pCurrentBatch->batchForeground[triOffset + 4].col = math::Vec4Lite(0, 0, 0, 0);
                pCurrentBatch->batchForeground[triOffset + 5].col = math::Vec4Lite(0, 0, 0, 0);

				triOffset += 6;
			}
		}

		//DBGLOG("Size = %d\n", triOffset);
	}

	// set to first batch
	int currentBatchIndex = 0;
	int currentBatchCharacter = 0;
	FreetypeRenderBatch* pCurrentBatch = &m_RenderBatches[currentBatchIndex];

	int bgOffset = 0;
    int fgOffset = 0;
    
	for (i = 0; i < s.length(); ++i)
    {
		if (currentBatchCharacter >= NUM_CHARACTERS_PER_BATCH)
		{
            pCurrentBatch->numberOfCharacters = currentBatchCharacter;
            
			currentBatchCharacter = 0;
			currentBatchIndex++;

			bgOffset = 0;
            fgOffset = 0;
			pCurrentBatch = &m_RenderBatches[currentBatchIndex];
		}

		if (hasChanges)
		{
			if (currentCharChangeOffset >= block.numCharsInBlockText)
			{
				currentChangeIndex++;

				if (currentChangeIndex < static_cast<int>(formatChanges.size()))
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

		if (s[i] == '\n')
		{
			// need to handle this so the AABB keeps the right size

			offsetMatrix = glm::translate(offsetMatrix, glm::vec3(-xOffsetTotal, -m_vDimensions.Height, 0.0f));
			xOffsetTotal = 0.0f;
            
            currentCharChangeOffset++;
            
			continue;
		}

		currentCharChangeOffset++;
		unsigned char index = s[i];

		unsigned char alpha = m_vBGColour.A;
		if (!doBackground)
			alpha = 0;
		else
		{
			alpha = m_vBGColour.A;
		}

		///// background

		glm::mat4 textMatrix = glm::translate(offsetMatrix, glm::vec3(0.0f, -m_vDimensions.Y*0.15f, 0.0f));
		math::Vec2 m = math::Vec2(textMatrix[3][0], textMatrix[3][1]);

		// verts
		// bottom, left, bottom right, top left
		pCurrentBatch->batchBackground[bgOffset].v = math::Vec2(GlyphQuad[style][index].textBGBox[0].X + m.X, GlyphQuad[style][index].textBGBox[0].Y + m.Y);
		pCurrentBatch->bufferAABB.ReCalculate(pCurrentBatch->batchBackground[bgOffset].v);
		pCurrentBatch->batchBackground[bgOffset + 1].v = math::Vec2(GlyphQuad[style][index].textBGBox[1].X + m.X, GlyphQuad[style][index].textBGBox[1].Y + m.Y);
		pCurrentBatch->bufferAABB.ReCalculate(pCurrentBatch->batchBackground[bgOffset+1].v);
		pCurrentBatch->batchBackground[bgOffset + 2].v = math::Vec2(GlyphQuad[style][index].textBGBox[2].X + m.X, GlyphQuad[style][index].textBGBox[2].Y + m.Y);
		pCurrentBatch->bufferAABB.ReCalculate(pCurrentBatch->batchBackground[bgOffset+2].v);

		// bottom right, top right, top left
		pCurrentBatch->batchBackground[bgOffset + 3].v = math::Vec2(GlyphQuad[style][index].textBGBox[1].X + m.X, GlyphQuad[style][index].textBGBox[1].Y + m.Y);
		pCurrentBatch->bufferAABB.ReCalculate(pCurrentBatch->batchBackground[bgOffset + 3].v);
		pCurrentBatch->batchBackground[bgOffset + 4].v = math::Vec2(GlyphQuad[style][index].textBGBox[3].X + m.X, GlyphQuad[style][index].textBGBox[3].Y + m.Y);
		pCurrentBatch->bufferAABB.ReCalculate(pCurrentBatch->batchBackground[bgOffset + 4].v);
		pCurrentBatch->batchBackground[bgOffset + 5].v = math::Vec2(GlyphQuad[style][index].textBGBox[2].X + m.X, GlyphQuad[style][index].textBGBox[2].Y + m.Y);
		pCurrentBatch->bufferAABB.ReCalculate(pCurrentBatch->batchBackground[bgOffset + 5].v);

		// texture coords
		// bottom, left, bottom right, top left
		pCurrentBatch->batchBackground[bgOffset].uv = GlyphQuad[style][index].textUnderlineUV[0];
		pCurrentBatch->batchBackground[bgOffset + 1].uv = GlyphQuad[style][index].textUnderlineUV[1];
		pCurrentBatch->batchBackground[bgOffset + 2].uv = GlyphQuad[style][index].textUnderlineUV[2];

		// bottom right, top right, top left
		pCurrentBatch->batchBackground[bgOffset + 3].uv = GlyphQuad[style][index].textUnderlineUV[1];
		pCurrentBatch->batchBackground[bgOffset + 4].uv = GlyphQuad[style][index].textUnderlineUV[3];
		pCurrentBatch->batchBackground[bgOffset + 5].uv = GlyphQuad[style][index].textUnderlineUV[2];

		// colour init

		// bottom, left, bottom right, top left
		pCurrentBatch->batchBackground[bgOffset].col = math::Vec4Lite(m_vBGColour.R, m_vBGColour.G, m_vBGColour.B, alpha);
		pCurrentBatch->batchBackground[bgOffset + 1].col = math::Vec4Lite(m_vBGColour.R, m_vBGColour.G, m_vBGColour.B, alpha);
		pCurrentBatch->batchBackground[bgOffset + 2].col = math::Vec4Lite(m_vBGColour.R, m_vBGColour.G, m_vBGColour.B, alpha);

		// bottom right, top right, top left
		pCurrentBatch->batchBackground[bgOffset + 3].col = math::Vec4Lite(m_vBGColour.R, m_vBGColour.G, m_vBGColour.B, alpha);
		pCurrentBatch->batchBackground[bgOffset + 4].col = math::Vec4Lite(m_vBGColour.R, m_vBGColour.G, m_vBGColour.B, alpha);
		pCurrentBatch->batchBackground[bgOffset + 5].col = math::Vec4Lite(m_vBGColour.R, m_vBGColour.G, m_vBGColour.B, alpha);

		bgOffset += 6;

		///// drop shadow 

		alpha = m_vBlockColour.A;
		if (!dropShadow)
			alpha = 0;

		glm::mat4 shadowMatrix = glm::mat4(1.0f);
		shadowMatrix = glm::translate(offsetMatrix, glm::vec3(GlyphQuad[style][index].Offsets[0].X, GlyphQuad[style][index].Offsets[0].Y, GlyphQuad[style][index].Offsets[0].Z));
		shadowMatrix = glm::translate(shadowMatrix, glm::vec3(GlyphQuad[style][index].Offsets[1].X, GlyphQuad[style][index].Offsets[1].Y, GlyphQuad[style][index].Offsets[1].Z));
		shadowMatrix = glm::translate(shadowMatrix, glm::vec3(GlyphQuad[style][index].ShadowOffset.X, GlyphQuad[style][index].ShadowOffset.Y, GlyphQuad[style][index].ShadowOffset.Z));
		m = math::Vec2(shadowMatrix[3][0], shadowMatrix[3][1]);

		// verts
		// bottom, left, bottom right, top left
		pCurrentBatch->batchBackground[bgOffset].v = math::Vec2(GlyphQuad[style][index].textPoint[0].X + m.X, GlyphQuad[style][index].textPoint[0].Y + m.Y);
		pCurrentBatch->bufferAABB.ReCalculate(pCurrentBatch->batchBackground[bgOffset].v);
		pCurrentBatch->batchBackground[bgOffset + 1].v = math::Vec2(GlyphQuad[style][index].textPoint[1].X + m.X, GlyphQuad[style][index].textPoint[1].Y + m.Y);
		pCurrentBatch->bufferAABB.ReCalculate(pCurrentBatch->batchBackground[bgOffset+1].v);
		pCurrentBatch->batchBackground[bgOffset + 2].v = math::Vec2(GlyphQuad[style][index].textPoint[2].X + m.X, GlyphQuad[style][index].textPoint[2].Y + m.Y);
		pCurrentBatch->bufferAABB.ReCalculate(pCurrentBatch->batchBackground[bgOffset+2].v);

		// bottom right, top right, top left
		pCurrentBatch->batchBackground[bgOffset + 3].v = math::Vec2(GlyphQuad[style][index].textPoint[1].X + m.X, GlyphQuad[style][index].textPoint[1].Y + m.Y);
		pCurrentBatch->bufferAABB.ReCalculate(pCurrentBatch->batchBackground[bgOffset+3].v);
		pCurrentBatch->batchBackground[bgOffset + 4].v = math::Vec2(GlyphQuad[style][index].textPoint[3].X + m.X, GlyphQuad[style][index].textPoint[3].Y + m.Y);
		pCurrentBatch->bufferAABB.ReCalculate(pCurrentBatch->batchBackground[bgOffset+4].v);
		pCurrentBatch->batchBackground[bgOffset + 5].v = math::Vec2(GlyphQuad[style][index].textPoint[2].X + m.X, GlyphQuad[style][index].textPoint[2].Y + m.Y);
		pCurrentBatch->bufferAABB.ReCalculate(pCurrentBatch->batchBackground[bgOffset+5].v);

		// texture coords
		// bottom, left, bottom right, top left
		pCurrentBatch->batchBackground[bgOffset].uv = GlyphQuad[style][index].textUV[0];
		pCurrentBatch->batchBackground[bgOffset + 1].uv = GlyphQuad[style][index].textUV[1];
		pCurrentBatch->batchBackground[bgOffset + 2].uv = GlyphQuad[style][index].textUV[2];

		// bottom right, top right, top left
		pCurrentBatch->batchBackground[bgOffset + 3].uv = GlyphQuad[style][index].textUV[1];
		pCurrentBatch->batchBackground[bgOffset + 4].uv = GlyphQuad[style][index].textUV[3];
		pCurrentBatch->batchBackground[bgOffset + 5].uv = GlyphQuad[style][index].textUV[2];

		// colour is ok
		// bottom, left, bottom right, top left
		pCurrentBatch->batchBackground[bgOffset].col = math::Vec4Lite(0, 0, 0, static_cast<int>(alpha));
		pCurrentBatch->batchBackground[bgOffset + 1].col = math::Vec4Lite(0, 0, 0, static_cast<int>(alpha));
		pCurrentBatch->batchBackground[bgOffset + 2].col = math::Vec4Lite(0, 0, 0, static_cast<int>(alpha));

		// bottom right, top right, top left
		pCurrentBatch->batchBackground[bgOffset + 3].col = math::Vec4Lite(0, 0, 0, static_cast<int>(alpha));
		pCurrentBatch->batchBackground[bgOffset + 4].col = math::Vec4Lite(0, 0, 0, static_cast<int>(alpha));
		pCurrentBatch->batchBackground[bgOffset + 5].col = math::Vec4Lite(0, 0, 0, static_cast<int>(alpha));

		bgOffset += 6;

		///// glyph 
		
		textMatrix = glm::translate(offsetMatrix, glm::vec3(GlyphQuad[style][index].Offsets[0].X, GlyphQuad[style][index].Offsets[0].Y, GlyphQuad[style][index].Offsets[0].Z));
		textMatrix = glm::translate(textMatrix, glm::vec3(GlyphQuad[style][index].Offsets[1].X, GlyphQuad[style][index].Offsets[1].Y, GlyphQuad[style][index].Offsets[1].Z));
		m = math::Vec2(textMatrix[3][0], textMatrix[3][1]);

		// verts
		// bottom, left, bottom right, top left
		pCurrentBatch->batchForeground[fgOffset].v = math::Vec2(GlyphQuad[style][index].textPoint[0].X + m.X, GlyphQuad[style][index].textPoint[0].Y + m.Y);
		pCurrentBatch->bufferAABB.ReCalculate(pCurrentBatch->batchForeground[fgOffset].v);
		pCurrentBatch->batchForeground[fgOffset + 1].v = math::Vec2(GlyphQuad[style][index].textPoint[1].X + m.X, GlyphQuad[style][index].textPoint[1].Y + m.Y);
		pCurrentBatch->bufferAABB.ReCalculate(pCurrentBatch->batchForeground[fgOffset+1].v);
		pCurrentBatch->batchForeground[fgOffset + 2].v = math::Vec2(GlyphQuad[style][index].textPoint[2].X + m.X, GlyphQuad[style][index].textPoint[2].Y + m.Y);
		pCurrentBatch->bufferAABB.ReCalculate(pCurrentBatch->batchForeground[fgOffset+2].v);

		// bottom right, top right, top left
		pCurrentBatch->batchForeground[fgOffset + 3].v = math::Vec2(GlyphQuad[style][index].textPoint[1].X + m.X, GlyphQuad[style][index].textPoint[1].Y + m.Y);
		pCurrentBatch->bufferAABB.ReCalculate(pCurrentBatch->batchForeground[fgOffset+3].v);
		pCurrentBatch->batchForeground[fgOffset + 4].v = math::Vec2(GlyphQuad[style][index].textPoint[3].X + m.X, GlyphQuad[style][index].textPoint[3].Y + m.Y);
		pCurrentBatch->bufferAABB.ReCalculate(pCurrentBatch->batchForeground[fgOffset+4].v);
		pCurrentBatch->batchForeground[fgOffset + 5].v = math::Vec2(GlyphQuad[style][index].textPoint[2].X + m.X, GlyphQuad[style][index].textPoint[2].Y + m.Y);
		pCurrentBatch->bufferAABB.ReCalculate(pCurrentBatch->batchForeground[fgOffset+5].v);

		// texture coords
		// bottom, left, bottom right, top left
		pCurrentBatch->batchForeground[fgOffset].uv = GlyphQuad[style][index].textUV[0];
		pCurrentBatch->batchForeground[fgOffset + 1].uv = GlyphQuad[style][index].textUV[1];
		pCurrentBatch->batchForeground[fgOffset + 2].uv = GlyphQuad[style][index].textUV[2];

		// bottom right, top right, top left
		pCurrentBatch->batchForeground[fgOffset + 3].uv = GlyphQuad[style][index].textUV[1];
		pCurrentBatch->batchForeground[fgOffset + 4].uv = GlyphQuad[style][index].textUV[3];
		pCurrentBatch->batchForeground[fgOffset + 5].uv = GlyphQuad[style][index].textUV[2];

		// colour init

		// bottom, left, bottom right, top left
		pCurrentBatch->batchForeground[fgOffset].col = math::Vec4Lite(m_vBlockColour.R, m_vBlockColour.G, m_vBlockColour.B, m_vBlockColour.A);
		pCurrentBatch->batchForeground[fgOffset + 1].col = math::Vec4Lite(m_vBlockColour.R, m_vBlockColour.G, m_vBlockColour.B, m_vBlockColour.A);
		pCurrentBatch->batchForeground[fgOffset + 2].col = math::Vec4Lite(m_vBlockColour.R, m_vBlockColour.G, m_vBlockColour.B, m_vBlockColour.A);

		// bottom right, top right, top left
		pCurrentBatch->batchForeground[fgOffset + 3].col = math::Vec4Lite(m_vBlockColour.R, m_vBlockColour.G, m_vBlockColour.B, m_vBlockColour.A);
		pCurrentBatch->batchForeground[fgOffset + 4].col = math::Vec4Lite(m_vBlockColour.R, m_vBlockColour.G, m_vBlockColour.B, m_vBlockColour.A);
		pCurrentBatch->batchForeground[fgOffset + 5].col = math::Vec4Lite(m_vBlockColour.R, m_vBlockColour.G, m_vBlockColour.B, m_vBlockColour.A);

		fgOffset += 6;

		///// underline 

		alpha = m_vBlockColour.A;
		if (!doUnderline)
			alpha = 0;

		textMatrix = glm::translate(offsetMatrix, glm::vec3(0.0f, -m_vDimensions.Y*0.15f, 0.0f));
		m = math::Vec2(textMatrix[3][0], textMatrix[3][1]);

		// verts
		// bottom, left, bottom right, top left
		pCurrentBatch->batchForeground[fgOffset].v = math::Vec2(GlyphQuad[style][index].textUnderline[0].X + m.X, GlyphQuad[style][index].textUnderline[0].Y + m.Y);
		pCurrentBatch->bufferAABB.ReCalculate(pCurrentBatch->batchForeground[fgOffset].v);
		pCurrentBatch->batchForeground[fgOffset + 1].v = math::Vec2(GlyphQuad[style][index].textUnderline[1].X + m.X, GlyphQuad[style][index].textUnderline[1].Y + m.Y);
		pCurrentBatch->bufferAABB.ReCalculate(pCurrentBatch->batchForeground[fgOffset+1].v);
		pCurrentBatch->batchForeground[fgOffset + 2].v = math::Vec2(GlyphQuad[style][index].textUnderline[2].X + m.X, GlyphQuad[style][index].textUnderline[2].Y + m.Y);
		pCurrentBatch->bufferAABB.ReCalculate(pCurrentBatch->batchForeground[fgOffset+2].v);

		// bottom right, top right, top left
		pCurrentBatch->batchForeground[fgOffset + 3].v = math::Vec2(GlyphQuad[style][index].textUnderline[1].X + m.X, GlyphQuad[style][index].textUnderline[1].Y + m.Y);
		pCurrentBatch->bufferAABB.ReCalculate(pCurrentBatch->batchForeground[fgOffset+3].v);
		pCurrentBatch->batchForeground[fgOffset + 4].v = math::Vec2(GlyphQuad[style][index].textUnderline[3].X + m.X, GlyphQuad[style][index].textUnderline[3].Y + m.Y);
		pCurrentBatch->bufferAABB.ReCalculate(pCurrentBatch->batchForeground[fgOffset+4].v);
		pCurrentBatch->batchForeground[fgOffset + 5].v = math::Vec2(GlyphQuad[style][index].textUnderline[2].X + m.X, GlyphQuad[style][index].textUnderline[2].Y + m.Y);
		pCurrentBatch->bufferAABB.ReCalculate(pCurrentBatch->batchForeground[fgOffset+5].v);

		// texture coords
		// bottom, left, bottom right, top left
		pCurrentBatch->batchForeground[fgOffset].uv = GlyphQuad[style][index].textUnderlineUV[0];
		pCurrentBatch->batchForeground[fgOffset + 1].uv = GlyphQuad[style][index].textUnderlineUV[1];
		pCurrentBatch->batchForeground[fgOffset + 2].uv = GlyphQuad[style][index].textUnderlineUV[2];

		// bottom right, top right, top left
		pCurrentBatch->batchForeground[fgOffset + 3].uv = GlyphQuad[style][index].textUnderlineUV[1];
		pCurrentBatch->batchForeground[fgOffset + 4].uv = GlyphQuad[style][index].textUnderlineUV[3];
		pCurrentBatch->batchForeground[fgOffset + 5].uv = GlyphQuad[style][index].textUnderlineUV[2];

		// colour init

		// bottom, left, bottom right, top left
		pCurrentBatch->batchForeground[fgOffset].col = math::Vec4Lite(m_vBlockColour.R, m_vBlockColour.G, m_vBlockColour.B, alpha);
		pCurrentBatch->batchForeground[fgOffset + 1].col = math::Vec4Lite(m_vBlockColour.R, m_vBlockColour.G, m_vBlockColour.B, alpha);
		pCurrentBatch->batchForeground[fgOffset + 2].col = math::Vec4Lite(m_vBlockColour.R, m_vBlockColour.G, m_vBlockColour.B, alpha);

		// bottom right, top right, top left
		pCurrentBatch->batchForeground[fgOffset + 3].col = math::Vec4Lite(m_vBlockColour.R, m_vBlockColour.G, m_vBlockColour.B, alpha);
		pCurrentBatch->batchForeground[fgOffset + 4].col = math::Vec4Lite(m_vBlockColour.R, m_vBlockColour.G, m_vBlockColour.B, alpha);
		pCurrentBatch->batchForeground[fgOffset + 5].col = math::Vec4Lite(m_vBlockColour.R, m_vBlockColour.G, m_vBlockColour.B, alpha);

		fgOffset += 6;

		offsetMatrix = glm::translate(offsetMatrix, glm::vec3(GlyphQuad[style][index].Offsets[2].X, GlyphQuad[style][index].Offsets[2].Y, GlyphQuad[style][index].Offsets[2].Z));

		// track how far word has gone
		xOffsetTotal += GlyphQuad[style][index].Offsets[2].X;

		currentBatchCharacter++;
        
        if( i+1 == s.length() )
            pCurrentBatch->numberOfCharacters = currentBatchCharacter;
	}
    
    //small batches with newlines will not have the character count correctly assigned
    if( pCurrentBatch )
    {
        if( pCurrentBatch->numberOfCharacters == 0 &&
           s.length() > 0 )
        {
            pCurrentBatch->numberOfCharacters = currentBatchCharacter;
        }
    }
    
    m_BatchExists = true;
}*/

/////////////////////////////////////////////////////
/// Method: BuildBufferCacheUTF8
/// Params: [in]szFilename, [in]pData, [in]dataSize [in]vDims, [in]bDropShadow, [in]vTopColour, [in]vBottomColour
///
/////////////////////////////////////////////////////
void FreetypeFont::BuildBufferCacheUTF8(int x, int y, bool dropShadow, std::vector<TFormatChangeBlock>& formatChanges, std::string fmtStr, ...)
{
	int style = FONT_STYLE_NORMAL;
	unsigned int i = 0, j = 0, k = 0;
	std::string s, ss;
	int n, size = 100;
	bool b = false;
	va_list marker;

	if (freeTypeRenderBatchProgram == renderer::INVALID_OBJECT ||
		freeTypeRenderBatchProgram == 0)
		return;

	if (fmtStr.length() == 0)
		return;

#if !defined(BASE_PLATFORM_ANDROID)	
	while (!b)
	{
		ss.resize(size);
		va_start(marker, fmtStr);
		n = vsnprintf((char*)ss.c_str(), size, fmtStr.c_str(), marker);
		va_end(marker);
		if ((n>0) && ((b = (n<size)) == true))
			ss.resize(n);
		else
			size *= 2;
	}
	s += ss;
#else
	s = fmtStr;
#endif
	glm::mat4 modelMat = m_OpenGLContext->GetModelMatrix();

	float xOffsetTotal = 0.0f;

	// initial block
	TFormatChangeBlock block;
	block.style = FONT_STYLE_NORMAL;
	block.numCharsInBlockText = static_cast<int>(UTFCPP::distance(s.begin(), s.end()));
	block.textColour = m_vBlockColour;
	block.underline = false;
	block.bgColour = false;
	block.textBGColour = math::Vec4Lite(0, 0, 0, 0);

	bool hasChanges = false;
	bool doUnderline = false;
	bool doBackground = false;

	int currentChangeIndex = 0;

	if (formatChanges.size() != 0)
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

	// set the start point
	glm::mat4 offsetMatrix = glm::translate(modelMat, glm::vec3(static_cast<GLfloat>(x), static_cast<GLfloat>(y), 0.0f));

	if (s.length() == 0)
		return;

	if (m_RenderBatches != 0)
	{
		for (i = 0; i < static_cast<unsigned int>(m_TotalRenderBatches); ++i)
		{
			if (m_RenderBatches[i].batchBackground != 0)
			{
				delete[] m_RenderBatches[i].batchBackground;
				m_RenderBatches[i].batchBackground = 0;
			}

			if (m_RenderBatches[i].batchForeground != 0)
			{
				delete[] m_RenderBatches[i].batchForeground;
				m_RenderBatches[i].batchForeground = 0;
			}
		}

		delete[] m_RenderBatches;
		m_RenderBatches = 0;
	}
	m_TotalRenderBatches = 0;

	m_TotalRenderBatches = ((int)s.length() / NUM_CHARACTERS_PER_BATCH) + 1;

	m_RenderBatches = new FreetypeRenderBatch[m_TotalRenderBatches];
	DBG_ASSERT(m_RenderBatches != 0);

	// init all data inside
	for (i = 0; i < static_cast<unsigned int>(m_TotalRenderBatches); ++i)
	{
		FreetypeRenderBatch* pCurrentBatch = &m_RenderBatches[i];

		pCurrentBatch->numberOfCharacters = 0;
		pCurrentBatch->bufferAABB.Reset();
		pCurrentBatch->bufferAABB.vBoxMax.Z = 0.0f;
		pCurrentBatch->bufferAABB.vBoxMin.Z = 0.0f;

		pCurrentBatch->batchBackground = new GlyphVert[(2 * 6)*NUM_CHARACTERS_PER_BATCH]; // 2 triangles with 6 vertices and 2 versions for bg, shadow
		DBG_ASSERT(pCurrentBatch->batchBackground != 0);

		pCurrentBatch->batchForeground = new GlyphVert[(2 * 6)*NUM_CHARACTERS_PER_BATCH]; // 2 triangles with 6 vertices and 2 versions for glyph and underline
		DBG_ASSERT(pCurrentBatch->batchForeground != 0);

		int triOffset = 0;

		for (j = 0; j < NUM_CHARACTERS_PER_BATCH; ++j)
		{
			for (k = 0; k < 2; ++k) // 2 versions for bg, shadow
			{
				// vert init
				// glyph quad = 2 tris = 6 verts
				pCurrentBatch->batchBackground[triOffset].v = math::Vec2(0.0f, 0.0f);
				pCurrentBatch->batchBackground[triOffset + 1].v = math::Vec2(0.0f, 0.0f);
				pCurrentBatch->batchBackground[triOffset + 2].v = math::Vec2(0.0f, 0.0f);

				pCurrentBatch->batchBackground[triOffset + 3].v = math::Vec2(0.0f, 0.0f);
				pCurrentBatch->batchBackground[triOffset + 4].v = math::Vec2(0.0f, 0.0f);
				pCurrentBatch->batchBackground[triOffset + 5].v = math::Vec2(0.0f, 0.0f);
				///

				// texture coords
				pCurrentBatch->batchBackground[triOffset].uv = math::Vec2(0.0f, 0.0f);
				pCurrentBatch->batchBackground[triOffset + 1].uv = math::Vec2(0.0f, 0.0f);
				pCurrentBatch->batchBackground[triOffset + 2].uv = math::Vec2(0.0f, 0.0f);

				pCurrentBatch->batchBackground[triOffset + 3].uv = math::Vec2(0.0f, 0.0f);
				pCurrentBatch->batchBackground[triOffset + 4].uv = math::Vec2(0.0f, 0.0f);
				pCurrentBatch->batchBackground[triOffset + 5].uv = math::Vec2(0.0f, 0.0f);

				// colour init
				pCurrentBatch->batchBackground[triOffset].col = math::Vec4Lite(0, 0, 0, 0);
				pCurrentBatch->batchBackground[triOffset + 1].col = math::Vec4Lite(0, 0, 0, 0);
				pCurrentBatch->batchBackground[triOffset + 2].col = math::Vec4Lite(0, 0, 0, 0);

				pCurrentBatch->batchBackground[triOffset + 3].col = math::Vec4Lite(0, 0, 0, 0);
				pCurrentBatch->batchBackground[triOffset + 4].col = math::Vec4Lite(0, 0, 0, 0);
				pCurrentBatch->batchBackground[triOffset + 5].col = math::Vec4Lite(0, 0, 0, 0);


				// glyph quad = 2 tris = 6 verts
				pCurrentBatch->batchForeground[triOffset].v = math::Vec2(0.0f, 0.0f);
				pCurrentBatch->batchForeground[triOffset + 1].v = math::Vec2(0.0f, 0.0f);
				pCurrentBatch->batchForeground[triOffset + 2].v = math::Vec2(0.0f, 0.0f);

				pCurrentBatch->batchForeground[triOffset + 3].v = math::Vec2(0.0f, 0.0f);
				pCurrentBatch->batchForeground[triOffset + 4].v = math::Vec2(0.0f, 0.0f);
				pCurrentBatch->batchForeground[triOffset + 5].v = math::Vec2(0.0f, 0.0f);
				///

				// texture coords
				pCurrentBatch->batchForeground[triOffset].uv = math::Vec2(0.0f, 0.0f);
				pCurrentBatch->batchForeground[triOffset + 1].uv = math::Vec2(0.0f, 0.0f);
				pCurrentBatch->batchForeground[triOffset + 2].uv = math::Vec2(0.0f, 0.0f);

				pCurrentBatch->batchForeground[triOffset + 3].uv = math::Vec2(0.0f, 0.0f);
				pCurrentBatch->batchForeground[triOffset + 4].uv = math::Vec2(0.0f, 0.0f);
				pCurrentBatch->batchForeground[triOffset + 5].uv = math::Vec2(0.0f, 0.0f);

				// colour init
				pCurrentBatch->batchForeground[triOffset].col = math::Vec4Lite(0, 0, 0, 0);
				pCurrentBatch->batchForeground[triOffset + 1].col = math::Vec4Lite(0, 0, 0, 0);
				pCurrentBatch->batchForeground[triOffset + 2].col = math::Vec4Lite(0, 0, 0, 0);

				pCurrentBatch->batchForeground[triOffset + 3].col = math::Vec4Lite(0, 0, 0, 0);
				pCurrentBatch->batchForeground[triOffset + 4].col = math::Vec4Lite(0, 0, 0, 0);
				pCurrentBatch->batchForeground[triOffset + 5].col = math::Vec4Lite(0, 0, 0, 0);

				triOffset += 6;
			}
		}

		//DBGLOG("Size = %d\n", triOffset);
	}

	// set to first batch
	int currentBatchIndex = 0;
	int currentBatchCharacter = 0;
	FreetypeRenderBatch* pCurrentBatch = &m_RenderBatches[currentBatchIndex];

	int bgOffset = 0;
	int fgOffset = 0;

	//for (i = 0; i < s.length(); ++i)
	//{

	auto iter = s.begin();
	while (iter != s.end())
	{
		utf8::uint32_t val = UTFCPP::next(iter/*, s.end()*/);
		wchar_t wideVal = (wchar_t)val;

		if (currentBatchCharacter >= NUM_CHARACTERS_PER_BATCH)
		{
			pCurrentBatch->numberOfCharacters = currentBatchCharacter;

			currentBatchCharacter = 0;
			currentBatchIndex++;

			bgOffset = 0;
			fgOffset = 0;
			pCurrentBatch = &m_RenderBatches[currentBatchIndex];
		}

		if (hasChanges)
		{
			if (currentCharChangeOffset >= block.numCharsInBlockText)
			{
				currentChangeIndex++;

				if (currentChangeIndex < static_cast<int>(formatChanges.size()))
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

		//if (s[i] == '\n')
		if (val == ASCII_VAL_NEWLINE)
		{
			// need to handle this so the AABB keeps the right size

			offsetMatrix = glm::translate(offsetMatrix, glm::vec3(-xOffsetTotal, -m_vDimensions.Height, 0.0f));
			xOffsetTotal = 0.0f;

			currentCharChangeOffset++;

			continue;
		}

		currentCharChangeOffset++;
		//unsigned char index = s[i];
		auto quadList = m_QuadMap.find(val);
		if (quadList == m_QuadMap.end())
        {
            quadList = m_QuadMap.find(ASCII_VAL_SPACE);
        }

		QuadArray quadValue = quadList->second;

		unsigned char alpha = m_vBGColour.A;
		if (!doBackground)
			alpha = 0;
		else
		{
			alpha = m_vBGColour.A;
		}

		///// background

		glm::mat4 textMatrix = glm::translate(offsetMatrix, glm::vec3(0.0f, -m_vDimensions.Y*0.15f, 0.0f));
		math::Vec2 m = math::Vec2(textMatrix[3][0], textMatrix[3][1]);

		// verts
		// bottom, left, bottom right, top left
		pCurrentBatch->batchBackground[bgOffset].v = math::Vec2(quadValue[style].textBGBox[0].X + m.X, quadValue[style].textBGBox[0].Y + m.Y);
		pCurrentBatch->bufferAABB.ReCalculate(pCurrentBatch->batchBackground[bgOffset].v);
		pCurrentBatch->batchBackground[bgOffset + 1].v = math::Vec2(quadValue[style].textBGBox[1].X + m.X, quadValue[style].textBGBox[1].Y + m.Y);
		pCurrentBatch->bufferAABB.ReCalculate(pCurrentBatch->batchBackground[bgOffset + 1].v);
		pCurrentBatch->batchBackground[bgOffset + 2].v = math::Vec2(quadValue[style].textBGBox[2].X + m.X, quadValue[style].textBGBox[2].Y + m.Y);
		pCurrentBatch->bufferAABB.ReCalculate(pCurrentBatch->batchBackground[bgOffset + 2].v);

		// bottom right, top right, top left
		pCurrentBatch->batchBackground[bgOffset + 3].v = math::Vec2(quadValue[style].textBGBox[1].X + m.X, quadValue[style].textBGBox[1].Y + m.Y);
		pCurrentBatch->bufferAABB.ReCalculate(pCurrentBatch->batchBackground[bgOffset + 3].v);
		pCurrentBatch->batchBackground[bgOffset + 4].v = math::Vec2(quadValue[style].textBGBox[3].X + m.X, quadValue[style].textBGBox[3].Y + m.Y);
		pCurrentBatch->bufferAABB.ReCalculate(pCurrentBatch->batchBackground[bgOffset + 4].v);
		pCurrentBatch->batchBackground[bgOffset + 5].v = math::Vec2(quadValue[style].textBGBox[2].X + m.X, quadValue[style].textBGBox[2].Y + m.Y);
		pCurrentBatch->bufferAABB.ReCalculate(pCurrentBatch->batchBackground[bgOffset + 5].v);

		// texture coords
		// bottom, left, bottom right, top left
		pCurrentBatch->batchBackground[bgOffset].uv = quadValue[style].textUnderlineUV[0];
		pCurrentBatch->batchBackground[bgOffset + 1].uv = quadValue[style].textUnderlineUV[1];
		pCurrentBatch->batchBackground[bgOffset + 2].uv = quadValue[style].textUnderlineUV[2];

		// bottom right, top right, top left
		pCurrentBatch->batchBackground[bgOffset + 3].uv = quadValue[style].textUnderlineUV[1];
		pCurrentBatch->batchBackground[bgOffset + 4].uv = quadValue[style].textUnderlineUV[3];
		pCurrentBatch->batchBackground[bgOffset + 5].uv = quadValue[style].textUnderlineUV[2];

		// colour init

		// bottom, left, bottom right, top left
		pCurrentBatch->batchBackground[bgOffset].col = math::Vec4Lite(m_vBGColour.R, m_vBGColour.G, m_vBGColour.B, alpha);
		pCurrentBatch->batchBackground[bgOffset + 1].col = math::Vec4Lite(m_vBGColour.R, m_vBGColour.G, m_vBGColour.B, alpha);
		pCurrentBatch->batchBackground[bgOffset + 2].col = math::Vec4Lite(m_vBGColour.R, m_vBGColour.G, m_vBGColour.B, alpha);

		// bottom right, top right, top left
		pCurrentBatch->batchBackground[bgOffset + 3].col = math::Vec4Lite(m_vBGColour.R, m_vBGColour.G, m_vBGColour.B, alpha);
		pCurrentBatch->batchBackground[bgOffset + 4].col = math::Vec4Lite(m_vBGColour.R, m_vBGColour.G, m_vBGColour.B, alpha);
		pCurrentBatch->batchBackground[bgOffset + 5].col = math::Vec4Lite(m_vBGColour.R, m_vBGColour.G, m_vBGColour.B, alpha);

		bgOffset += 6;

		///// drop shadow 

		alpha = m_vBlockColour.A;
		if (!dropShadow)
			alpha = 0;

		glm::mat4 shadowMatrix = glm::mat4(1.0f);
		shadowMatrix = glm::translate(offsetMatrix, glm::vec3(quadValue[style].Offsets[0].X, quadValue[style].Offsets[0].Y, quadValue[style].Offsets[0].Z));
		shadowMatrix = glm::translate(shadowMatrix, glm::vec3(quadValue[style].Offsets[1].X, quadValue[style].Offsets[1].Y, quadValue[style].Offsets[1].Z));
		shadowMatrix = glm::translate(shadowMatrix, glm::vec3(quadValue[style].ShadowOffset.X, quadValue[style].ShadowOffset.Y, quadValue[style].ShadowOffset.Z));
		m = math::Vec2(shadowMatrix[3][0], shadowMatrix[3][1]);

		// verts
		// bottom, left, bottom right, top left
		pCurrentBatch->batchBackground[bgOffset].v = math::Vec2(quadValue[style].textPoint[0].X + m.X, quadValue[style].textPoint[0].Y + m.Y);
		pCurrentBatch->bufferAABB.ReCalculate(pCurrentBatch->batchBackground[bgOffset].v);
		pCurrentBatch->batchBackground[bgOffset + 1].v = math::Vec2(quadValue[style].textPoint[1].X + m.X, quadValue[style].textPoint[1].Y + m.Y);
		pCurrentBatch->bufferAABB.ReCalculate(pCurrentBatch->batchBackground[bgOffset + 1].v);
		pCurrentBatch->batchBackground[bgOffset + 2].v = math::Vec2(quadValue[style].textPoint[2].X + m.X, quadValue[style].textPoint[2].Y + m.Y);
		pCurrentBatch->bufferAABB.ReCalculate(pCurrentBatch->batchBackground[bgOffset + 2].v);

		// bottom right, top right, top left
		pCurrentBatch->batchBackground[bgOffset + 3].v = math::Vec2(quadValue[style].textPoint[1].X + m.X, quadValue[style].textPoint[1].Y + m.Y);
		pCurrentBatch->bufferAABB.ReCalculate(pCurrentBatch->batchBackground[bgOffset + 3].v);
		pCurrentBatch->batchBackground[bgOffset + 4].v = math::Vec2(quadValue[style].textPoint[3].X + m.X, quadValue[style].textPoint[3].Y + m.Y);
		pCurrentBatch->bufferAABB.ReCalculate(pCurrentBatch->batchBackground[bgOffset + 4].v);
		pCurrentBatch->batchBackground[bgOffset + 5].v = math::Vec2(quadValue[style].textPoint[2].X + m.X, quadValue[style].textPoint[2].Y + m.Y);
		pCurrentBatch->bufferAABB.ReCalculate(pCurrentBatch->batchBackground[bgOffset + 5].v);

		// texture coords
		// bottom, left, bottom right, top left
		pCurrentBatch->batchBackground[bgOffset].uv = quadValue[style].textUV[0];
		pCurrentBatch->batchBackground[bgOffset + 1].uv = quadValue[style].textUV[1];
		pCurrentBatch->batchBackground[bgOffset + 2].uv = quadValue[style].textUV[2];

		// bottom right, top right, top left
		pCurrentBatch->batchBackground[bgOffset + 3].uv = quadValue[style].textUV[1];
		pCurrentBatch->batchBackground[bgOffset + 4].uv = quadValue[style].textUV[3];
		pCurrentBatch->batchBackground[bgOffset + 5].uv = quadValue[style].textUV[2];

		// colour is ok
		// bottom, left, bottom right, top left
		pCurrentBatch->batchBackground[bgOffset].col = math::Vec4Lite(0, 0, 0, static_cast<int>(alpha));
		pCurrentBatch->batchBackground[bgOffset + 1].col = math::Vec4Lite(0, 0, 0, static_cast<int>(alpha));
		pCurrentBatch->batchBackground[bgOffset + 2].col = math::Vec4Lite(0, 0, 0, static_cast<int>(alpha));

		// bottom right, top right, top left
		pCurrentBatch->batchBackground[bgOffset + 3].col = math::Vec4Lite(0, 0, 0, static_cast<int>(alpha));
		pCurrentBatch->batchBackground[bgOffset + 4].col = math::Vec4Lite(0, 0, 0, static_cast<int>(alpha));
		pCurrentBatch->batchBackground[bgOffset + 5].col = math::Vec4Lite(0, 0, 0, static_cast<int>(alpha));

		bgOffset += 6;

		///// glyph 

		textMatrix = glm::translate(offsetMatrix, glm::vec3(quadValue[style].Offsets[0].X, quadValue[style].Offsets[0].Y, quadValue[style].Offsets[0].Z));
		textMatrix = glm::translate(textMatrix, glm::vec3(quadValue[style].Offsets[1].X, quadValue[style].Offsets[1].Y, quadValue[style].Offsets[1].Z));
		m = math::Vec2(textMatrix[3][0], textMatrix[3][1]);

		// verts
		// bottom, left, bottom right, top left
		pCurrentBatch->batchForeground[fgOffset].v = math::Vec2(quadValue[style].textPoint[0].X + m.X, quadValue[style].textPoint[0].Y + m.Y);
		pCurrentBatch->bufferAABB.ReCalculate(pCurrentBatch->batchForeground[fgOffset].v);
		pCurrentBatch->batchForeground[fgOffset + 1].v = math::Vec2(quadValue[style].textPoint[1].X + m.X, quadValue[style].textPoint[1].Y + m.Y);
		pCurrentBatch->bufferAABB.ReCalculate(pCurrentBatch->batchForeground[fgOffset + 1].v);
		pCurrentBatch->batchForeground[fgOffset + 2].v = math::Vec2(quadValue[style].textPoint[2].X + m.X, quadValue[style].textPoint[2].Y + m.Y);
		pCurrentBatch->bufferAABB.ReCalculate(pCurrentBatch->batchForeground[fgOffset + 2].v);

		// bottom right, top right, top left
		pCurrentBatch->batchForeground[fgOffset + 3].v = math::Vec2(quadValue[style].textPoint[1].X + m.X, quadValue[style].textPoint[1].Y + m.Y);
		pCurrentBatch->bufferAABB.ReCalculate(pCurrentBatch->batchForeground[fgOffset + 3].v);
		pCurrentBatch->batchForeground[fgOffset + 4].v = math::Vec2(quadValue[style].textPoint[3].X + m.X, quadValue[style].textPoint[3].Y + m.Y);
		pCurrentBatch->bufferAABB.ReCalculate(pCurrentBatch->batchForeground[fgOffset + 4].v);
		pCurrentBatch->batchForeground[fgOffset + 5].v = math::Vec2(quadValue[style].textPoint[2].X + m.X, quadValue[style].textPoint[2].Y + m.Y);
		pCurrentBatch->bufferAABB.ReCalculate(pCurrentBatch->batchForeground[fgOffset + 5].v);

		// texture coords
		// bottom, left, bottom right, top left
		pCurrentBatch->batchForeground[fgOffset].uv = quadValue[style].textUV[0];
		pCurrentBatch->batchForeground[fgOffset + 1].uv = quadValue[style].textUV[1];
		pCurrentBatch->batchForeground[fgOffset + 2].uv = quadValue[style].textUV[2];

		// bottom right, top right, top left
		pCurrentBatch->batchForeground[fgOffset + 3].uv = quadValue[style].textUV[1];
		pCurrentBatch->batchForeground[fgOffset + 4].uv = quadValue[style].textUV[3];
		pCurrentBatch->batchForeground[fgOffset + 5].uv = quadValue[style].textUV[2];

		// colour init

		// bottom, left, bottom right, top left
		pCurrentBatch->batchForeground[fgOffset].col = math::Vec4Lite(m_vBlockColour.R, m_vBlockColour.G, m_vBlockColour.B, m_vBlockColour.A);
		pCurrentBatch->batchForeground[fgOffset + 1].col = math::Vec4Lite(m_vBlockColour.R, m_vBlockColour.G, m_vBlockColour.B, m_vBlockColour.A);
		pCurrentBatch->batchForeground[fgOffset + 2].col = math::Vec4Lite(m_vBlockColour.R, m_vBlockColour.G, m_vBlockColour.B, m_vBlockColour.A);

		// bottom right, top right, top left
		pCurrentBatch->batchForeground[fgOffset + 3].col = math::Vec4Lite(m_vBlockColour.R, m_vBlockColour.G, m_vBlockColour.B, m_vBlockColour.A);
		pCurrentBatch->batchForeground[fgOffset + 4].col = math::Vec4Lite(m_vBlockColour.R, m_vBlockColour.G, m_vBlockColour.B, m_vBlockColour.A);
		pCurrentBatch->batchForeground[fgOffset + 5].col = math::Vec4Lite(m_vBlockColour.R, m_vBlockColour.G, m_vBlockColour.B, m_vBlockColour.A);

		fgOffset += 6;

		///// underline 

		alpha = m_vBlockColour.A;
		if (!doUnderline)
			alpha = 0;

		textMatrix = glm::translate(offsetMatrix, glm::vec3(0.0f, -m_vDimensions.Y*0.15f, 0.0f));
		m = math::Vec2(textMatrix[3][0], textMatrix[3][1]);

		// verts
		// bottom, left, bottom right, top left
		pCurrentBatch->batchForeground[fgOffset].v = math::Vec2(quadValue[style].textUnderline[0].X + m.X, quadValue[style].textUnderline[0].Y + m.Y);
		pCurrentBatch->bufferAABB.ReCalculate(pCurrentBatch->batchForeground[fgOffset].v);
		pCurrentBatch->batchForeground[fgOffset + 1].v = math::Vec2(quadValue[style].textUnderline[1].X + m.X, quadValue[style].textUnderline[1].Y + m.Y);
		pCurrentBatch->bufferAABB.ReCalculate(pCurrentBatch->batchForeground[fgOffset + 1].v);
		pCurrentBatch->batchForeground[fgOffset + 2].v = math::Vec2(quadValue[style].textUnderline[2].X + m.X, quadValue[style].textUnderline[2].Y + m.Y);
		pCurrentBatch->bufferAABB.ReCalculate(pCurrentBatch->batchForeground[fgOffset + 2].v);

		// bottom right, top right, top left
		pCurrentBatch->batchForeground[fgOffset + 3].v = math::Vec2(quadValue[style].textUnderline[1].X + m.X, quadValue[style].textUnderline[1].Y + m.Y);
		pCurrentBatch->bufferAABB.ReCalculate(pCurrentBatch->batchForeground[fgOffset + 3].v);
		pCurrentBatch->batchForeground[fgOffset + 4].v = math::Vec2(quadValue[style].textUnderline[3].X + m.X, quadValue[style].textUnderline[3].Y + m.Y);
		pCurrentBatch->bufferAABB.ReCalculate(pCurrentBatch->batchForeground[fgOffset + 4].v);
		pCurrentBatch->batchForeground[fgOffset + 5].v = math::Vec2(quadValue[style].textUnderline[2].X + m.X, quadValue[style].textUnderline[2].Y + m.Y);
		pCurrentBatch->bufferAABB.ReCalculate(pCurrentBatch->batchForeground[fgOffset + 5].v);

		// texture coords
		// bottom, left, bottom right, top left
		pCurrentBatch->batchForeground[fgOffset].uv = quadValue[style].textUnderlineUV[0];
		pCurrentBatch->batchForeground[fgOffset + 1].uv = quadValue[style].textUnderlineUV[1];
		pCurrentBatch->batchForeground[fgOffset + 2].uv = quadValue[style].textUnderlineUV[2];

		// bottom right, top right, top left
		pCurrentBatch->batchForeground[fgOffset + 3].uv = quadValue[style].textUnderlineUV[1];
		pCurrentBatch->batchForeground[fgOffset + 4].uv = quadValue[style].textUnderlineUV[3];
		pCurrentBatch->batchForeground[fgOffset + 5].uv = quadValue[style].textUnderlineUV[2];

		// colour init

		// bottom, left, bottom right, top left
		pCurrentBatch->batchForeground[fgOffset].col = math::Vec4Lite(m_vBlockColour.R, m_vBlockColour.G, m_vBlockColour.B, alpha);
		pCurrentBatch->batchForeground[fgOffset + 1].col = math::Vec4Lite(m_vBlockColour.R, m_vBlockColour.G, m_vBlockColour.B, alpha);
		pCurrentBatch->batchForeground[fgOffset + 2].col = math::Vec4Lite(m_vBlockColour.R, m_vBlockColour.G, m_vBlockColour.B, alpha);

		// bottom right, top right, top left
		pCurrentBatch->batchForeground[fgOffset + 3].col = math::Vec4Lite(m_vBlockColour.R, m_vBlockColour.G, m_vBlockColour.B, alpha);
		pCurrentBatch->batchForeground[fgOffset + 4].col = math::Vec4Lite(m_vBlockColour.R, m_vBlockColour.G, m_vBlockColour.B, alpha);
		pCurrentBatch->batchForeground[fgOffset + 5].col = math::Vec4Lite(m_vBlockColour.R, m_vBlockColour.G, m_vBlockColour.B, alpha);

		fgOffset += 6;

		offsetMatrix = glm::translate(offsetMatrix, glm::vec3(quadValue[style].Offsets[2].X, quadValue[style].Offsets[2].Y, quadValue[style].Offsets[2].Z));

		// track how far word has gone
		xOffsetTotal += quadValue[style].Offsets[2].X;

		currentBatchCharacter++;

		//if (i + 1 == s.length())
		if (iter == s.end())
			pCurrentBatch->numberOfCharacters = currentBatchCharacter;
	}

	//small batches with newlines will not have the character count correctly assigned
	if (pCurrentBatch)
	{
		if (pCurrentBatch->numberOfCharacters == 0 &&
			s.length() > 0)
		{
			pCurrentBatch->numberOfCharacters = currentBatchCharacter;
		}
	}

	m_BatchExists = true;
}

/////////////////////////////////////////////////////
/// Method: RenderBatches
/// Params: [in]filter
///
/////////////////////////////////////////////////////
void FreetypeFont::RenderBatches(GLenum filter)
{
    if( !m_BatchExists )
        return;
    
	m_OpenGLContext->DisableVBO();

	GLuint prevProg = m_OpenGLContext->GetCurrentProgram();
	m_OpenGLContext->UseProgram(freeTypeRenderBatchProgram);

	// grab all matrices
	glm::mat4 projMat = m_OpenGLContext->GetProjectionMatrix();
	glm::mat4 modelMat = m_OpenGLContext->GetModelMatrix();

	GLint nVertexAttribLocation = glGetAttribLocation(freeTypeRenderBatchProgram, "base_v");
	GLint nTexCoordsAttribLocation = glGetAttribLocation(freeTypeRenderBatchProgram, "base_uv0");
	GLint nColourAttribLocation = glGetAttribLocation(freeTypeRenderBatchProgram, "base_col");

	GLint ogl_ModelViewProjectionMatrix = glGetUniformLocation(freeTypeRenderBatchProgram, "ogl_ModelViewProjectionMatrix");

	GLint nTexSamplerUniform = glGetUniformLocation(freeTypeRenderBatchProgram, "texUnit0");
	if (nTexSamplerUniform != -1)
		glUniform1i(nTexSamplerUniform, 0);

	glEnableVertexAttribArray(nVertexAttribLocation);
	glEnableVertexAttribArray(nTexCoordsAttribLocation);
	glEnableVertexAttribArray(nColourAttribLocation);

	if (ogl_ModelViewProjectionMatrix != -1)
		glUniformMatrix4fv(ogl_ModelViewProjectionMatrix, 1, GL_FALSE, glm::value_ptr(projMat*modelMat));

	m_AtlasTexture.Bind(filter);

	m_OpenGLContext->ExtractFrustum();

	int i = 0;
	for (i = 0; i < m_TotalRenderBatches; ++i)
	{
		FreetypeRenderBatch* pCurrentBatch = &m_RenderBatches[i];

		if (pCurrentBatch->numberOfCharacters > 0)
		{
			if( m_OpenGLContext->AABBInFrustum(pCurrentBatch->bufferAABB) )
			{
                glVertexAttribPointer(nVertexAttribLocation, 2, GL_FLOAT, GL_FALSE, sizeof(GlyphVert), &pCurrentBatch->batchBackground[0].v);
                glVertexAttribPointer(nTexCoordsAttribLocation, 2, GL_FLOAT, GL_FALSE, sizeof(GlyphVert), &pCurrentBatch->batchBackground[0].uv);
                glVertexAttribPointer(nColourAttribLocation, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(GlyphVert), &pCurrentBatch->batchBackground[0].col);
                
                glDrawArrays(GL_TRIANGLES, 0, pCurrentBatch->numberOfCharacters * 12);
                
				glVertexAttribPointer(nVertexAttribLocation, 2, GL_FLOAT, GL_FALSE, sizeof(GlyphVert), &pCurrentBatch->batchForeground[0].v);
				glVertexAttribPointer(nTexCoordsAttribLocation, 2, GL_FLOAT, GL_FALSE, sizeof(GlyphVert), &pCurrentBatch->batchForeground[0].uv);
				glVertexAttribPointer(nColourAttribLocation, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(GlyphVert), &pCurrentBatch->batchForeground[0].col);

				glDrawArrays(GL_TRIANGLES, 0, pCurrentBatch->numberOfCharacters * 12);
			}
		}
	}

	if (nVertexAttribLocation != -1)
		glDisableVertexAttribArray(nVertexAttribLocation);

	if (nTexCoordsAttribLocation != -1)
		glDisableVertexAttribArray(nTexCoordsAttribLocation);

	if (nColourAttribLocation != -1)
		glDisableVertexAttribArray(nColourAttribLocation);

	m_OpenGLContext->UseProgram(prevProg);

	/*for (i = 0; i < m_TotalRenderBatches; ++i)
	{
		FreetypeRenderBatch* pCurrentBatch = &m_RenderBatches[i];

		if (pCurrentBatch->numberOfCharacters > 0)
		{
			m_OpenGLContext->SetColour4ub(0, 0, 255, 255);
			renderer::DrawAABB(pCurrentBatch->bufferAABB.vBoxMin, pCurrentBatch->bufferAABB.vBoxMax, false);
		}
	}*/
}

/////////////////////////////////////////////////////
/// Method: Print
/// Params: [in]x, [in]y, [in]dropShadow, [in]szString, [in]optional
///
/////////////////////////////////////////////////////
void FreetypeFont::Print(int x, int y, bool dropShadow, GLenum filter, std::vector<TFormatChangeBlock>& formatChanges, const std::string& fmtStr, ...)
{
	int style = FONT_STYLE_NORMAL;
	unsigned int i=0;
	std::string s, ss;
	int n, size=100;
	bool b=false;
	va_list marker;

	if( freeTypeRenderProgram == renderer::INVALID_OBJECT ||
		freeTypeRenderProgram == 0 )
		return;

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

	GLuint prevProg = m_OpenGLContext->GetCurrentProgram();
	m_OpenGLContext->UseProgram(freeTypeRenderProgram);

	// grab all matrices
	glm::mat4 projMat = m_OpenGLContext->GetProjectionMatrix();
	glm::mat4 modelMat = m_OpenGLContext->GetModelMatrix();
	
	GLint nVertexAttribLocation = glGetAttribLocation( freeTypeRenderProgram, "base_v" );
	GLint nTexCoordsAttribLocation = glGetAttribLocation( freeTypeRenderProgram, "base_uv0" );
	GLint ogl_ModelViewProjectionMatrix = glGetUniformLocation(freeTypeRenderProgram, "ogl_ModelViewProjectionMatrix");

	GLint nTexSamplerUniform = glGetUniformLocation( freeTypeRenderProgram, "texUnit0" );
	if( nTexSamplerUniform != -1 )
		glUniform1i( nTexSamplerUniform, 0 );
	
	glEnableVertexAttribArray( nVertexAttribLocation );
	glEnableVertexAttribArray( nTexCoordsAttribLocation );

	float xOffsetTotal = 0.0f;
		
	// initial block
	TFormatChangeBlock block;
	block.style = FONT_STYLE_NORMAL;
	block.numCharsInBlockText = static_cast<int>(s.length());
	block.textColour = m_vBlockColour;
	block.underline = false;
	block.bgColour = false;
    block.textBGColour = math::Vec4Lite(0,0,0,0);
    
	bool hasChanges = false;
	bool doUnderline = false;
    bool doBackground = false;
    
	int currentChangeIndex = 0;
	
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
	
	glm::mat4 offsetMatrix = glm::translate( modelMat, glm::vec3( static_cast<GLfloat>(x), static_cast<GLfloat>(y), 0.0f ) );

	m_AtlasTexture.Bind(filter);
    
    std::vector<int> utf32result;
	UTFCPP::utf8to32(s.begin(), s.end(), back_inserter(utf32result));
    
    for (i = 0; i < utf32result.size(); ++i)
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
		
		if( utf32result[i] == ASCII_VAL_NEWLINE )
		{
			offsetMatrix = glm::translate( offsetMatrix, glm::vec3( -xOffsetTotal, -m_vDimensions.Height, 0.0f ) );
			xOffsetTotal = 0.0f;
			continue;
		}

		currentCharChangeOffset++;
        utf8::uint32_t val = utf32result[i];
        auto quadList = m_QuadMap.find(val);
        if (quadList == m_QuadMap.end())
        {
            quadList = m_QuadMap.find(ASCII_VAL_SPACE);
        }
        
        QuadArray quadValue = quadList->second;
        
		if( doBackground )
		{
			//m_OpenGLContext->BindTexture( freeTypeUnderlineTexture );

			m_OpenGLContext->SetColour4ub( m_vBGColour.R, m_vBGColour.G, m_vBGColour.B, m_vBGColour.A );

			glm::mat4 textMatrix = glm::translate( offsetMatrix, glm::vec3( 0.0f, -m_vDimensions.Y*0.15f, 0.0f ) );
			
			if( ogl_ModelViewProjectionMatrix != -1 )
				glUniformMatrix4fv( ogl_ModelViewProjectionMatrix, 1, GL_FALSE, glm::value_ptr(projMat*textMatrix) );
			
			glVertexAttribPointer( nVertexAttribLocation, 2, GL_FLOAT, GL_FALSE, sizeof(math::Vec2), &quadValue[style].textBGBox[0] );
			glVertexAttribPointer(nTexCoordsAttribLocation, 2, GL_FLOAT, GL_FALSE, sizeof(math::Vec2), &quadValue[style].textUnderlineUV[0]);

			glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
		}

		if( dropShadow )
		{
			glm::mat4 shadowMatrix = glm::mat4(1.0f);

			shadowMatrix = glm::translate( offsetMatrix, glm::vec3( quadValue[style].Offsets[0].X, quadValue[style].Offsets[0].Y, quadValue[style].Offsets[0].Z ) );

			shadowMatrix = glm::translate( shadowMatrix, glm::vec3( quadValue[style].Offsets[1].X, quadValue[style].Offsets[1].Y, quadValue[style].Offsets[1].Z ) );

			shadowMatrix = glm::translate( shadowMatrix, glm::vec3( quadValue[style].ShadowOffset.X, quadValue[style].ShadowOffset.Y, quadValue[style].ShadowOffset.Z ) );

			if( ogl_ModelViewProjectionMatrix != -1 )
				glUniformMatrix4fv( ogl_ModelViewProjectionMatrix, 1, GL_FALSE, glm::value_ptr(projMat*shadowMatrix) );

			glVertexAttribPointer( nVertexAttribLocation, 2, GL_FLOAT, GL_FALSE, sizeof(math::Vec2), &quadValue[style].textPoint[0]  );
			glVertexAttribPointer( nTexCoordsAttribLocation, 2, GL_FLOAT, GL_FALSE, sizeof(math::Vec2), &quadValue[style].textUV[0]  );

			m_OpenGLContext->SetColour4ub( 0, 0, 0, m_vBlockColour.A );

			glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
		}

		glm::mat4 textMatrix = glm::translate( offsetMatrix, glm::vec3( quadValue[style].Offsets[0].X, quadValue[style].Offsets[0].Y, quadValue[style].Offsets[0].Z ) );

		textMatrix = glm::translate( textMatrix, glm::vec3( quadValue[style].Offsets[1].X, quadValue[style].Offsets[1].Y, quadValue[style].Offsets[1].Z ) );

		if( ogl_ModelViewProjectionMatrix != -1 )
			glUniformMatrix4fv( ogl_ModelViewProjectionMatrix, 1, GL_FALSE, glm::value_ptr(projMat*textMatrix) );

		glVertexAttribPointer( nVertexAttribLocation, 2, GL_FLOAT, GL_FALSE, sizeof(math::Vec2), &quadValue[style].textPoint[0]  );
		glVertexAttribPointer( nTexCoordsAttribLocation, 2, GL_FLOAT, GL_FALSE, sizeof(math::Vec2), &quadValue[style].textUV[0]  );

		m_OpenGLContext->SetColour4ub( m_vBlockColour.R, m_vBlockColour.G, m_vBlockColour.B, m_vBlockColour.A );

		glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
		
		if( doUnderline )
		{
			//m_OpenGLContext->BindTexture( freeTypeUnderlineTexture );
			
			textMatrix = glm::translate( offsetMatrix, glm::vec3( 0.0f, -m_vDimensions.Y*0.15f, 0.0f ) );
			
			if( ogl_ModelViewProjectionMatrix != -1 )
				glUniformMatrix4fv( ogl_ModelViewProjectionMatrix, 1, GL_FALSE, glm::value_ptr(projMat*textMatrix) );
			
			glVertexAttribPointer( nVertexAttribLocation, 2, GL_FLOAT, GL_FALSE, sizeof(math::Vec2), &quadValue[style].textUnderline[0] );
			glVertexAttribPointer( nTexCoordsAttribLocation, 2, GL_FLOAT, GL_FALSE, sizeof(math::Vec2), &quadValue[style].textUnderlineUV[0]  );
			
			glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
			
		}
		
		offsetMatrix = glm::translate( offsetMatrix, glm::vec3( quadValue[style].Offsets[2].X, quadValue[style].Offsets[2].Y, quadValue[style].Offsets[2].Z ) );

		// track how far word has gone
		xOffsetTotal += quadValue[style].Offsets[2].X;
	}

	if( nVertexAttribLocation != -1 )
		glDisableVertexAttribArray( nVertexAttribLocation );

	if( nTexCoordsAttribLocation != -1 )
		glDisableVertexAttribArray( nTexCoordsAttribLocation );

	m_OpenGLContext->UseProgram(prevProg);
	
}

/////////////////////////////////////////////////////
/// Method: Print
/// Params: [in]x, [in]y, [in]dropShadow, [in]szString, [in]optional
///
/////////////////////////////////////////////////////
void FreetypeFont::Print(int x, int y, bool dropShadow, GLenum filter, const std::string& fmtStr, ...)
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

	if( freeTypeRenderProgram == renderer::INVALID_OBJECT ||
		freeTypeRenderProgram == 0 )
		return;

	m_OpenGLContext->DisableVBO();

	GLuint prevProg = m_OpenGLContext->GetCurrentProgram();
	m_OpenGLContext->UseProgram(freeTypeRenderProgram);

	// grab all matrices
	glm::mat4 projMat = m_OpenGLContext->GetProjectionMatrix();
	glm::mat4 modelMat = m_OpenGLContext->GetModelMatrix();
	
	GLint nVertexAttribLocation = glGetAttribLocation( freeTypeRenderProgram, "base_v" );
	GLint nTexCoordsAttribLocation = glGetAttribLocation( freeTypeRenderProgram, "base_uv0" );
	GLint ogl_ModelViewProjectionMatrix = glGetUniformLocation(freeTypeRenderProgram, "ogl_ModelViewProjectionMatrix");

	GLint nTexSamplerUniform = glGetUniformLocation( freeTypeRenderProgram, "texUnit0" );
	if( nTexSamplerUniform != -1 )
		glUniform1i( nTexSamplerUniform, 0 );
	
	glEnableVertexAttribArray( nVertexAttribLocation );
	glEnableVertexAttribArray( nTexCoordsAttribLocation );

	float xOffsetTotal = 0.0f;
	
	glm::mat4 offsetMatrix = glm::translate( modelMat, glm::vec3( static_cast<GLfloat>(x), static_cast<GLfloat>(y), 0.0f ) );

	m_AtlasTexture.Bind(filter);

    std::vector<int> utf32result;
	UTFCPP::utf8to32(s.begin(), s.end(), back_inserter(utf32result));
    
    for (i = 0; i < utf32result.size(); ++i)
	{
		if( utf32result[i] == ASCII_VAL_NEWLINE )
		{
			offsetMatrix = glm::translate( offsetMatrix, glm::vec3( -xOffsetTotal, -m_vDimensions.Height, 0.0f ) );
			xOffsetTotal = 0.0f;
			continue;
		}

        utf8::uint32_t val = utf32result[i];
        auto quadList = m_QuadMap.find(val);
        if (quadList == m_QuadMap.end())
        {
            quadList = m_QuadMap.find(ASCII_VAL_SPACE);
        }
        
        QuadArray quadValue = quadList->second;

		if( dropShadow )
		{
			glm::mat4 shadowMatrix = glm::mat4(1.0f);

			shadowMatrix = glm::translate( offsetMatrix, glm::vec3( quadValue[style].Offsets[0].X, quadValue[style].Offsets[0].Y, quadValue[style].Offsets[0].Z ) );

			shadowMatrix = glm::translate( shadowMatrix, glm::vec3( quadValue[style].Offsets[1].X, quadValue[style].Offsets[1].Y, quadValue[style].Offsets[1].Z ) );

			shadowMatrix = glm::translate( shadowMatrix, glm::vec3( quadValue[style].ShadowOffset.X, quadValue[style].ShadowOffset.Y, quadValue[style].ShadowOffset.Z ) );

			if( ogl_ModelViewProjectionMatrix != -1 )
				glUniformMatrix4fv( ogl_ModelViewProjectionMatrix, 1, GL_FALSE, glm::value_ptr(projMat*shadowMatrix) );

			glVertexAttribPointer( nVertexAttribLocation, 2, GL_FLOAT, GL_FALSE, sizeof(math::Vec2), &quadValue[style].textPoint[0]  );
			glVertexAttribPointer( nTexCoordsAttribLocation, 2, GL_FLOAT, GL_FALSE, sizeof(math::Vec2), &quadValue[style].textUV[0]  );

			m_OpenGLContext->SetColour4ub( 0, 0, 0, m_vBlockColour.A );

			glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
		}

		glm::mat4 textMatrix = glm::translate( offsetMatrix, glm::vec3( quadValue[style].Offsets[0].X, quadValue[style].Offsets[0].Y, quadValue[style].Offsets[0].Z ) );

		textMatrix = glm::translate( textMatrix, glm::vec3( quadValue[style].Offsets[1].X, quadValue[style].Offsets[1].Y, quadValue[style].Offsets[1].Z ) );

		if( ogl_ModelViewProjectionMatrix != -1 )
			glUniformMatrix4fv( ogl_ModelViewProjectionMatrix, 1, GL_FALSE, glm::value_ptr(projMat*textMatrix) );

		glVertexAttribPointer( nVertexAttribLocation, 2, GL_FLOAT, GL_FALSE, sizeof(math::Vec2), &quadValue[style].textPoint[0]  );
		glVertexAttribPointer( nTexCoordsAttribLocation, 2, GL_FLOAT, GL_FALSE, sizeof(math::Vec2), &quadValue[style].textUV[0]  );

		m_OpenGLContext->SetColour4ub( m_vBlockColour.R, m_vBlockColour.G, m_vBlockColour.B, m_vBlockColour.A );

		glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
			
		offsetMatrix = glm::translate( offsetMatrix, glm::vec3( quadValue[style].Offsets[2].X, quadValue[style].Offsets[2].Y, quadValue[style].Offsets[2].Z ) );

		// track how far word has gone
		xOffsetTotal += quadValue[style].Offsets[2].X;
	}

	if( nVertexAttribLocation != -1 )
		glDisableVertexAttribArray( nVertexAttribLocation );

	if( nTexCoordsAttribLocation != -1 )
		glDisableVertexAttribArray( nTexCoordsAttribLocation );

	m_OpenGLContext->UseProgram(prevProg);
	
}

/////////////////////////////////////////////////////
/// Method: Print
/// Params: [in]x, [in]y, [in]vColour, [in]szString, [in]optional
///
/////////////////////////////////////////////////////
/*void FreetypeFont::Print(int x, int y, const math::Vec4Lite &vColour, GLenum filter, const char *szString, ...)
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

	m_OpenGLContext->UseProgram(freeTypeRenderProgram);

	// grab all matrices
	glm::mat4 projMat = m_OpenGLContext->GetProjectionMatrix();
	glm::mat4 modelMat = m_OpenGLContext->GetModelMatrix();
	
	GLint nVertexAttribLocation = glGetAttribLocation( freeTypeRenderProgram, "base_v" );
	GLint nTexCoordsAttribLocation = glGetAttribLocation( freeTypeRenderProgram, "base_uv0" );
	GLint ogl_ModelViewProjectionMatrix = glGetUniformLocation(freeTypeRenderProgram, "ogl_ModelViewProjectionMatrix");

	GLint nTexSamplerUniform = glGetUniformLocation( freeTypeRenderProgram, "texUnit0" );
	if( nTexSamplerUniform != -1 )
		glUniform1i( nTexSamplerUniform, 0 );
	
	glEnableVertexAttribArray( nVertexAttribLocation );
	glEnableVertexAttribArray( nTexCoordsAttribLocation );

	math::Vec4Lite globalColour = m_OpenGLContext->GetColour4ub();

	glm::mat4 offsetMatrix = glm::translate( modelMat, glm::vec3( static_cast<GLfloat>(x), static_cast<GLfloat>(y), 0.0f ) );

	m_AtlasTexture.Bind(filter);

	for( i=0; i < (int)std::strlen(szText); ++i )
	{
		char index = szText[i];

		glm::mat4 textMatrix = glm::translate( offsetMatrix, glm::vec3( GlyphQuad[style][index].Offsets[0].X, GlyphQuad[style][index].Offsets[0].Y, GlyphQuad[style][index].Offsets[0].Z ) );

		textMatrix = glm::translate( textMatrix, glm::vec3( GlyphQuad[style][index].Offsets[1].X, GlyphQuad[style][index].Offsets[1].Y, GlyphQuad[style][index].Offsets[1].Z ) );

		if( ogl_ModelViewProjectionMatrix != -1 )
			glUniformMatrix4fv( ogl_ModelViewProjectionMatrix, 1, GL_FALSE, glm::value_ptr(projMat*textMatrix) );

		glVertexAttribPointer( nVertexAttribLocation, 2, GL_FLOAT, GL_FALSE, sizeof(math::Vec2), &GlyphQuad[style][index].textPoint[0]  );
		glVertexAttribPointer( nTexCoordsAttribLocation, 2, GL_FLOAT, GL_FALSE, sizeof(math::Vec2), &GlyphQuad[style][index].textUV[0]  );

		m_OpenGLContext->SetColour4ub( m_vBlockColour.R, m_vBlockColour.G, m_vBlockColour.B, globalColour.A/255 );

		glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
			
		offsetMatrix = glm::translate( offsetMatrix, glm::vec3( GlyphQuad[style][index].Offsets[2].X, GlyphQuad[style][index].Offsets[2].Y, GlyphQuad[style][index].Offsets[2].Z ) );

	}

	if( nVertexAttribLocation != -1 )
		glDisableVertexAttribArray( nVertexAttribLocation );

	if( nTexCoordsAttribLocation != -1 )
		glDisableVertexAttribArray( nTexCoordsAttribLocation );
	
}*/

/////////////////////////////////////////////////////
/// Method: GetFontWidthHeight
/// Params: [in]c, [out]nCharacterWidth
///
/////////////////////////////////////////////////////
/*void FreetypeFont::GetCharacterWidth( unsigned char c, int *nCharacterWidth, int style )
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
}*/

/////////////////////////////////////////////////////
/// Method: GetFontWidthHeight
/// Params: [in]c, [out]nCharacterWidth
///
/////////////////////////////////////////////////////
void FreetypeFont::GetCharacterWidthUTF8(utf8::uint32_t c, int *nCharacterWidth, int style)
{
	auto iter = m_QuadMap.find(c);

	if (iter != m_QuadMap.end() &&
		nCharacterWidth)
	{
		if (m_FixedWidth)
			*nCharacterWidth = static_cast<int>(m_vDimensions.Width*0.75f);
		else
		{
			*nCharacterWidth = iter->second[style].glyphWidth;
		}
	}
    else
    {
        //DBG_ASSERT(0);
        *nCharacterWidth = 0;//static_cast<int>(m_vDimensions.Width);
    }
}

/////////////////////////////////////////////////////
/// Method: GetStringLength
/// Params: [in]szString
///
/////////////////////////////////////////////////////
float FreetypeFont::GetStringLength(const std::string& str, ...)
{
    int style = FONT_STYLE_NORMAL;
    unsigned int i=0;
    std::string s, ss;
    int n, size=100;
    bool b=false;
    va_list marker;
    
    if( str.length() == 0 )
        return 0.0f;
    
    while (!b)
    {
        ss.resize(size);
        va_start(marker, str);
        n = vsnprintf((char*)ss.c_str(), size, str.c_str(), marker);
        va_end(marker);
        if ((n>0) && ((b=(n<size))==true))
            ss.resize(n);
        else
            size*=2;
    }
    s += ss;
    
    float xOffsetTotal = 0.0f;
    
    std::vector<int> utf32result;
	UTFCPP::utf8to32(s.begin(), s.end(), back_inserter(utf32result));
    
    for (i = 0; i < utf32result.size(); ++i)
    {
        if( utf32result[i] == ASCII_VAL_NEWLINE )
        {
            return xOffsetTotal;
        }
        
        utf8::uint32_t val = utf32result[i];
        auto quadList = m_QuadMap.find(val);
        if (quadList == m_QuadMap.end())
        {
            quadList = m_QuadMap.find(ASCII_VAL_SPACE);
        }
        
        QuadArray quadValue = quadList->second;
        
        // track how far word has gone
        xOffsetTotal += quadValue[style].Offsets[2].X;
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
	tmpAtlas.Create( m_OpenGLContext, *atlasWidth, *atlasHeight, ATLAS_DEPTH);

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
                    {
						return -1;
                    }
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

#endif // BASE_SUPPORT_OPENGL_GLSL
#endif // BASE_SUPPORT_FREETYPE_UTF8
