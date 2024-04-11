
#ifndef __SPRITERESOURCES_H__
#define __SPRITERESOURCES_H__

namespace res
{
	// sprite info store
	struct SpriteResourceStore
	{
		int textureResId;
		GLfloat uOffset,vOffset;
		GLfloat w, h;
	};

	void CreateSpriteResourceMap();

	void ClearSpriteResources();

	const SpriteResourceStore* GetSpriteResource( int index );
}

#endif // __SPRITERESOURCES_H__
