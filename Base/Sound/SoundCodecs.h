
#ifndef __SOUNDCODECS_H__
#define __SOUNDCODECS_H__

#ifdef BASE_SUPPORT_OPENAL

#ifdef BASE_SUPPORT_OGG
	#include <ogg/ogg.h>
	#include <vorbis/vorbisfile.h>
#endif // BASE_SUPPORT_OGG

signed short ima2linear (unsigned char nibble, signed short *val, unsigned char *idx);

#ifdef BASE_SUPPORT_OGG
    const ov_callbacks GetOGGCallbacks();
#endif // BASE_SUPPORT_OGG

#endif // BASE_SUPPORT_OPENAL

#endif // __SOUNDCODECS_H__


