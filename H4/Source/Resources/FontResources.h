
#ifndef __FONTRESOURCES_H__
#define __FONTRESOURCES_H__

namespace res
{
	// font info store
	struct FontResourceStore
	{
		const char* resName;
		int fontId;
		renderer::FreetypeFont* fontRender;
	};

	void CreateFontResourceMap();

	void ClearFontResources();

	const FontResourceStore* GetFontResource( int index );

	const FontResourceStore* GetFontResource( int index, int fontSize, bool dropShadow, const math::Vec4Lite& colour, bool fixedWidth );
}

#endif // __FONTRESOURCES_H__
