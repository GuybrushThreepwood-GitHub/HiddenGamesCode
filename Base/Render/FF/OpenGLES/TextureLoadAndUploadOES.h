
#ifndef __TEXTURELOADANDUPLOADOES_H__
#define __TEXTURELOADANDUPLOADOES_H__

#ifdef BASE_SUPPORT_OPENGLES

namespace renderer
{
	/// GetTextureFormat - Gets the texture enumerated format from a filename
	/// \param szFilename - the filename to test
	/// \return ETextureFormat - enumerated texture format
	renderer::ETextureFormat GetTextureFormat( const char* szFilename );
	/// ClearTextureMap - clears the texture name map
	void ClearTextureMap();
	/// TextureLoad - Will load a texture and upload to GL 
	GLuint TextureLoad(const char *szFilename, Texture& tex, ETextureFormat texFormat = TEXTURE_TGA, file::TFileHandle* pFile = 0, bool genMipMaps = false, GLenum magFilter = GL_LINEAR, GLenum minFilter = GL_LINEAR, int texUnit = 0, bool reload = false, GLuint texObjectId = renderer::INVALID_OBJECT, renderer::OpenGL* openGLContext = 0);
	/// RemoveTexture - removes a texture from the map
	/// \param texId - texture id
	void RemoveTexture( GLuint texId );

} // namespace renderer

#endif // BASE_SUPPORT_OPENGLES

#endif // __TEXTURELOADANDUPLOADOES_H__


