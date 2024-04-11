
#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "CollisionBase.h"
#include "ModelBase.h"
#include "ScriptBase.h"

#include "Resources/ResourceHelper.h"

namespace
{
	static renderer::Texture texLoader;
}

/////////////////////////////////////////////////////
/// Method: LoadTextureName
/// Params: None
///
/////////////////////////////////////////////////////
GLuint res::LoadTextureName( const char* name, bool genMipMaps, GLenum magFilter, GLenum minFilter, bool lowPower )
{
	GLuint texId = renderer::INVALID_OBJECT;

	renderer::ETextureFormat texFormat = renderer::TEXTURE_UNKNOWN;

	char texturePath[core::MAX_PATH+core::MAX_PATH];
	std::memset( &texturePath, 0, sizeof(char)*core::MAX_PATH+core::MAX_PATH );

#if defined(BASE_PLATFORM_iOS)
	if( lowPower )
	{
		// test pvr
		snprintf( texturePath, core::MAX_PATH+core::MAX_PATH, "%s.pvr", name );
		texFormat = renderer::TEXTURE_PVR;
		
		// test png
		if( !file::FileExists(texturePath) )
		{
			snprintf( texturePath, core::MAX_PATH+core::MAX_PATH, "%s.png", name );
			texFormat = renderer::TEXTURE_PNG;
		}
	}
	else
	{
		snprintf( texturePath, core::MAX_PATH+core::MAX_PATH, "%s.png", name );
		texFormat = renderer::TEXTURE_PNG;
	}
#else
	// test tga
	snprintf( texturePath, core::MAX_PATH+core::MAX_PATH, "%s.tga", name );
	texFormat = renderer::TEXTURE_TGA;
	
	// test png
	if( !file::FileExists(texturePath) )
	{
		snprintf( texturePath, core::MAX_PATH+core::MAX_PATH, "%s.png", name );
		texFormat = renderer::TEXTURE_PNG;
	}
#endif // BASE_PLATFORM_iOS

	texId = renderer::TextureLoad( texturePath, texLoader, texFormat, 0, genMipMaps, magFilter, minFilter );
	texLoader.Free();

	return texId;
}

