
#ifndef __TEXTURERESOURCES_H__
#define __TEXTURERESOURCES_H__

namespace res
{
	// texture info store
	struct TextureResourceStore
	{
		int resId;
		const char* resourceFilename;
		GLenum magFilter;
		GLenum minFilter;
		bool preLoad;
		GLuint texId;
	};

	void CreateTextureResourceMap();

	void ClearTextureResources();

	const res::TextureResourceStore* GetTextureResource( int index );

	const std::map< int, res::TextureResourceStore >& GetTextureResourceMap();

	int LoadTexture( int index );

	void RemoveTexture( GLuint texId );

	void RemoveTexture( GLuint* pTexId=0 );
}

#endif // __TEXTURERESOURCES_H__

