
#ifndef __TEXTUREATLAS_H__
#define __TEXTUREATLAS_H__

#include <vector>

// forward declare
namespace renderer { class Texture; class OpenGL; }

namespace renderer
{
	class TextureAtlas
	{
		public:
			/// default constructor
			TextureAtlas();
			/// default destructor
			~TextureAtlas();

			/**
			* Creates a new empty texture atlas.
			*
			* @param   width   width of the atlas
			* @param   height  height of the atlas
			* @param   depth   bit depth of the atlas
			*
			*/
			void Create(renderer::OpenGL* openGLContext, int width, int height, int depth);


			/**
			*  Deletes a texture atlas.
			*
			*/
			void Destroy();


			/**
			*  Upload atlas to video memory.
			*
			*/
			void Bind(GLenum filter = GL_LINEAR);

			/**
			*  Allocate a new region in the atlas.
			*  @param width  width of the region to allocate
			*  @param height height of the region to allocate
			*  @return       Coordinates of the allocated region
			*
			*/
			math::Vec4Int GetRegion(int width, int height);


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
			void SetRegion(const int x, int y, int width, int height, const unsigned char *data, int stride);

			/**
			*  Remove all allocated regions from the atlas.
			*/
			void Clear();

            int GetWidth()  { return m_Width; }
            int GetHeight() { return m_Height; }
            int GetDepth()  { return m_Depth; }
        
			void WriteToFile(const char* filename);
        
            void SetTexture( const char* szFilename, renderer::OpenGL* openGLContext=0 );
        

		private:
			int AtlasFit(int index, int width, int height);

			void AtlasMerge();

		private:
			renderer::OpenGL* m_OpenGLContext;

			std::vector<math::Vec3Lite> m_Nodes;

			int m_Width;
			int m_Height;
			int m_Depth;
			int m_Used;
			GLuint m_TexID;
			unsigned char* m_Data;
	};

} // namespace renderer

#endif // __TEXTUREATLAS_H__

