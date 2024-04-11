
#ifndef __FREETYPECOMMON_H__
#define __FREETYPECOMMON_H__

#if defined(BASE_SUPPORT_FREETYPE) || defined(BASE_SUPPORT_FREETYPE_UTF8)

#include "Collision/AABB.h"

enum EFontFlag
{
	FONT_FLAG_NORMAL=0x0001,
	FONT_FLAG_ITALIC=0x0002,
	FONT_FLAG_BOLD=0x0004,
	FONT_FLAG_UNDERLINE=0x0008,
};

enum EFontStyle
{
	FONT_STYLE_NORMAL=0,
	FONT_STYLE_ITALIC,
	FONT_STYLE_BOLD,
	FONT_STYLE_ITALIC_BOLD,

	FONT_STYLE_TOTAL
};

struct TFormatChangeBlock
{
	EFontStyle style;
	bool underline;
	math::Vec4Lite textColour;
	int numCharsInBlockText;
    bool bgColour;
    math::Vec4Lite textBGColour;
    
    collision::AABB bgColourArea;
    collision::AABB blockArea;
};

#endif // BASE_SUPPORT_FREETYPE || BASE_SUPPORT_FREETYPE_UTF8

#endif // __FREETYPECOMMON_H__
