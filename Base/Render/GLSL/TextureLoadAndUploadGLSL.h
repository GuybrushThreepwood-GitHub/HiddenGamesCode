
#ifndef __TEXTURELOADANDUPLOADGLSL_H__
#define __TEXTURELOADANDUPLOADGLSL_H__

#ifdef BASE_SUPPORT_OPENGL_GLSL

namespace renderer
{
	/// GetTextureFormat - Gets the texture enumerated format from a filename
	/// \param szFilename - the filename to test
	/// \return ETextureFormat - enumerated texture format
	renderer::ETextureFormat GetTextureFormat( const char* szFilename );
	/// attempts to reload all textures into their designated slots
	void ReloadTextureMap();
	/// ClearTextureMap - clears the texture name map
	void ClearTextureMap();
	/// TextureLoad - Will load a texture and upload to GL 
	/// \param szFilename - file to load
	/// \param tex - texture loader object
	/// \param texFormat - the format of the texture to load
	/// \param pFile - optional file handle if this texture is inside another file format
	/// \param genMipMaps - boolean set if you want the hardware to generate mipmaps
	/// \param magFilter - mipmap magnification filter
	/// \param minFilter - mipmap minification filter
	/// \param texUnit - which texture unit to bind to
	/// \param reload - boolean flag is this texture is being reloaded and therefore there is no need to glGen a new texture object id
	/// \param texObjectId - on a reload this object will be used to populate the texture data
	GLuint TextureLoad( const char *szFilename, Texture& tex, ETextureFormat texFormat=TEXTURE_TGA, file::TFileHandle* pFile=0, bool genMipMaps=false, GLenum magFilter=GL_LINEAR, GLenum minFilter=GL_LINEAR, int texUnit=0, bool reload=false, GLuint texObjectId=renderer::INVALID_OBJECT, renderer::OpenGL* openGLContext=0 );
	/// RemoveTexture - removes a texture from the map
	/// \param texId - texture id
	void RemoveTexture( GLuint texId );

} // namespace renderer

#endif // BASE_SUPPORT_OPENGL_GLSL

#endif // __TEXTURELOADANDUPLOADGLSL_H__


