
/*===================================================================
	File: TextureAtlas.cpp
	Library: Render

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"

#include <cstring>
#include <cmath>

#include "Math/Vectors.h"
#include "Render/OpenGLCommon.h"
#include "Render/FF/OpenGL/OpenGL.h"
#include "Render/GLSL/OpenGLSL.h"
#include "Render/FF/OpenGL/Extensions.h"
#include "Render/FF/OpenGLES/OpenGLES.h"
#include "Render/FF/OpenGLES/ExtensionsOES.h"
#include "Render/TextureShared.h"
#include "Render/Texture.h"
#include "Render/GLSL/TextureLoadAndUploadGLSL.h"
#include "Render/FF/OpenGL/TextureLoadAndUpload.h"
#include "Render/FF/OpenGLES/TextureLoadAndUploadOES.h"

#include "Render/TextureAtlas.h"

using renderer::TextureAtlas;

/////////////////////////////////////////////////////
/// Default constructor
/// 
///
/////////////////////////////////////////////////////
TextureAtlas::TextureAtlas()
{
	m_Nodes.clear();

	m_Used = 0;
	m_Width = 0;
	m_Height = 0;
	m_Depth = 0;
	m_TexID = renderer::INVALID_OBJECT;

	m_Data = 0;
}

/////////////////////////////////////////////////////
/// Default destructor
/// 
///
/////////////////////////////////////////////////////
TextureAtlas::~TextureAtlas()
{
	Destroy();
}

/**
* Creates a new empty texture atlas.
*
* @param   width   width of the atlas
* @param   height  height of the atlas
* @param   depth   bit depth of the atlas
*
*/
void TextureAtlas::Create(renderer::OpenGL* openGLContext, int width, int height, int depth)
{
	if (openGLContext == 0)
		m_OpenGLContext = renderer::OpenGL::GetInstance();
	else
		m_OpenGLContext = openGLContext;

	// We want a one pixel border around the whole atlas to avoid any artefact when
	// sampling texture
	math::Vec3Lite node;
	node.X = 1;
	node.Y = 1;
	node.Z = width - 2;

	DBG_ASSERT((depth == 1) || (depth == 3) || (depth == 4));

	m_Nodes.clear();

	m_Used = 0;
	m_Width = width;
	m_Height = height;
	m_Depth = depth;
	m_TexID = renderer::INVALID_OBJECT;

	m_Nodes.push_back(node);

	m_Data = new unsigned char[width*height*depth];
	DBG_ASSERT(m_Data != 0);

	std::memset(m_Data, 0, m_Width*m_Height*m_Depth);
}


/**
*  Deletes a texture atlas.
*
*/
void TextureAtlas::Destroy()
{
	m_Nodes.clear();

	if (m_Data != 0 )
	{
		delete[] m_Data;
		m_Data = 0;
	}
	if (m_TexID != renderer::INVALID_OBJECT)
    {
        renderer::RemoveTexture( m_TexID );
        m_TexID = renderer::INVALID_OBJECT;
    }
}

/**
*  Upload atlas to video memory.
*
*/
void TextureAtlas::Bind(GLenum filter)
{
	if (m_TexID == renderer::INVALID_OBJECT)
	{
        if( m_Data == 0 )
            return;

		glGenTextures(1, &m_TexID);

		m_OpenGLContext->BindTexture(m_TexID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);

		if (m_Depth == 4)
		{
#ifdef GL_UNSIGNED_INT_8_8_8_8_REV
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height,
				0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, m_Data);
#else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height,
				0, GL_RGBA, GL_UNSIGNED_BYTE, m_Data);
#endif
		}
		else if (m_Depth == 3)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_Width, m_Height,
				0, GL_RGB, GL_UNSIGNED_BYTE, m_Data);
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, m_Width, m_Height,
				0, GL_ALPHA, GL_UNSIGNED_BYTE, m_Data);
		}

		// now on the GPU don't need the data or nodes
		m_Nodes.clear();

		if (m_Data != 0)
		{
			delete[] m_Data;
			m_Data = 0;
		}
	}
	else
	{
		m_OpenGLContext->BindTexture(m_TexID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	}
}

/**
*  Allocate a new region in the atlas.
*  @param width  width of the region to allocate
*  @param height height of the region to allocate
*  @return       Coordinates of the allocated region
*
*/
math::Vec4Int TextureAtlas::GetRegion(int width, int height)
{
	int y, best_height, best_width, best_index;
	math::Vec3Lite *node, *prev;
	math::Vec4Int region = math::Vec4Int(0, 0, width, height);
	int i = 0;

	best_height = INT_MAX;
	best_index = -1;
	best_width = INT_MAX;

	int count = static_cast<int>(m_Nodes.size());
	for (i = 0; i<count; ++i)
	{
		y = AtlasFit(i, width, height);
		if (y >= 0)
		{
			node = &m_Nodes[i];
			if (((y + height) < best_height) ||
				(((y + height) == best_height) && (node->Z < best_width)))
			{
				best_height = y + height;
				best_index = i;
				best_width = node->Z;
				region.X = node->X;
				region.Y = y;
			}
		}
	}

	if (best_index == -1)
	{
		region.X = -1;
		region.Y = -1;
		region.Z = 0;
		region.W = 0;
		return region;
	}

	math::Vec3Lite newNode;
	newNode.X = region.X;
	newNode.Y = region.Y + height;
	newNode.Z = width;

	std::vector<math::Vec3Lite>::iterator it = m_Nodes.begin();
	std::advance(it, best_index);
	m_Nodes.insert(it, newNode);
	//vector_insert(self->nodes, best_index, node);

	count = static_cast<int>(m_Nodes.size());
	i = best_index + 1;

	for (i = best_index + 1; i < count; ++i)
	{
		node = &m_Nodes[i];
		prev = &m_Nodes[i - 1];

		if (node->X < (prev->X + prev->Z))
		{
			int shrink = prev->X + prev->Z - node->X;
			node->X += shrink;
			node->Z -= shrink;
			if (node->Z <= 0)
			{
				it = m_Nodes.begin();
				std::advance(it, i);
				m_Nodes.erase(it);
				//vector_erase(self->nodes, i);
				count = static_cast<int>(m_Nodes.size());
				--i;
			}
			else
			{
				break;
			}
		}
		else
		{
			break;
		}
	}
	AtlasMerge();
	m_Used += width * height;
	return region;
}

/**
*  Upload data to the specified atlas region.
*  @param x      x coordinate the region
*  @param y      y coordinate the region
*  @param width  width of the region
*  @param height height of the region
*  @param data   data to be uploaded into the specified region
*  @param stride stride of the data
*
*/
void TextureAtlas::SetRegion(int x, int y, int width, int height, const unsigned char *data, int stride)
{
	int i;
	int depth;
	int charsize;

	DBG_ASSERT(x > 0);
	DBG_ASSERT(y > 0);
	DBG_ASSERT(x < (m_Width - 1));
	DBG_ASSERT((x + width) <= (m_Width - 1));
	DBG_ASSERT(y < (m_Height - 1));
	DBG_ASSERT((y + height) <= (m_Height - 1));

	depth = m_Depth;
	charsize = sizeof(char);
	for (i = 0; i<height; ++i)
	{
		std::memcpy(m_Data + ((y + i)*m_Width + x) * charsize * depth,
			data + (i*stride) * charsize, width * charsize * depth);
	}
}

/**
*  Remove all allocated regions from the atlas.
*/
void TextureAtlas::Clear()
{
	math::Vec3Lite node;
	node.X = 1;
	node.Y = 1;
	node.Z = 1;

	DBG_ASSERT(m_Data);

	m_Nodes.clear();
	m_Used = 0;

	// We want a one pixel border around the whole atlas to avoid any artefact when
	// sampling texture
	node.Z = m_Width - 2;

	m_Nodes.push_back(node);
	std::memset(m_Data, 0, m_Width*m_Height*m_Depth);
}

void TextureAtlas::WriteToFile(const char* szFilename )
{
#ifdef BASE_SUPPORT_PNG
	renderer::Texture texFile;

	texFile.nWidth = m_Width;
	texFile.nHeight = m_Height;
	texFile.bResized = false;
	texFile.nOriginalWidth = m_Width;
	texFile.nOriginalHeight = m_Height;
	texFile.nBPP = m_Depth*8;
	texFile.nNumChannels = m_Depth;
	
	texFile.bCompressed = false;
	texFile.bPaletted = false;
	texFile.bDXTCompressed = false;
	texFile.nDepth = 0;
	texFile.bIsVolume = false;
	texFile.bIsCubemap = false;
	texFile.nCubemapFaceCount = 0;
	texFile.bHasMipMaps = false;
	texFile.nMipMapCount = 0;
	texFile.nBytesPerElement = 0;

	texFile.nInternalFormat = GL_RGB;
	texFile.nFormat = GL_RGB;
	texFile.nTextureType = GL_UNSIGNED_BYTE;

	if (m_Depth == 4)
	{
		texFile.nFormat = GL_RGBA;
		texFile.bHasAlpha = true;
	}
	else if (m_Depth == 3)
	{
		texFile.nFormat = GL_RGB;
		texFile.bHasAlpha = false;
	}
	else
	{
		texFile.nFormat = GL_ALPHA;
		texFile.bHasAlpha = false;
	}

	texFile.image = new unsigned char[m_Width*m_Height*m_Depth];
	std::memcpy( texFile.image, m_Data, sizeof(unsigned char)*(m_Width*m_Height*m_Depth));
	//texFile.Flip();

	texFile.SavePNG(szFilename);
	texFile.Free();

	/*int i = 0;
	for (i = 0; i < static_cast<int>(m_Nodes.size()); ++i)
	{
		math::Vec3Lite node = m_Nodes[i];

		DBGLOG("x:%d y:%d z:%d\n", (int)node.X, (int)node.Y, (int)node.Z );
	}*/

#endif // BASE_SUPPORT_PNG
}

void TextureAtlas::SetTexture( const char* szFilename, renderer::OpenGL* openGLContext )
{
    if (openGLContext == 0)
        m_OpenGLContext = renderer::OpenGL::GetInstance();
    else
        m_OpenGLContext = openGLContext;
    
    renderer::Texture texFile;
    m_TexID = renderer::TextureLoad( szFilename, texFile, TEXTURE_PNG, 0, false, GL_LINEAR, GL_LINEAR, 0, false, renderer::INVALID_OBJECT, m_OpenGLContext );
    
    if( m_TexID != renderer::INVALID_OBJECT  )
    {
        m_Width = texFile.nWidth;
        m_Height = texFile.nHeight;
        m_Depth = texFile.nNumChannels;
        
        m_Data = 0;
        //std::memcpy( m_Data, texFile.image, sizeof(unsigned char)*(m_Width*m_Height*m_Depth));
    }
    texFile.Free();
}

int TextureAtlas::AtlasFit(int index, int width, int height)
{
	math::Vec3Lite *node;
	int x, y, width_left;
	int i = 0;

	node = &m_Nodes[index];
	x = node->X;
	y = node->Y;
	width_left = width;
	i = index;

	if ((x + width) > (m_Width - 1))
	{
		return -1;
	}
	y = node->Y;
	while (width_left > 0)
	{
		node = &m_Nodes[i];
		if (node->Y > y)
		{
			y = node->Y;
		}
		if ((y + height) > (m_Height - 1))
		{
			return -1;
		}
		width_left -= node->Z;
		++i;
	}
	return y;
}

void TextureAtlas::AtlasMerge()
{
	math::Vec3Lite *node,* next;
	int i = 0;

	std::vector<math::Vec3Lite>::iterator it = m_Nodes.begin();

	int count = static_cast<int>(m_Nodes.size()) - 1;

	for (i = 0; i<count; ++i)
	{
		node = &m_Nodes[i];
		next = &m_Nodes[i+1];

		if (node->Y == next->Y)
		{
			node->Z += next->Z;

			it = m_Nodes.begin();
			std::advance(it, i + 1);

			it = m_Nodes.erase(it);
			//vector_erase(self->nodes, i + 1);
			count = static_cast<int>(m_Nodes.size()) - 1;

			--i;
		}
	}
}


