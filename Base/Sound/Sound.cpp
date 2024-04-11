
/*===================================================================
	File: Sound.cpp
	Library: Sound

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"

#ifdef BASE_SUPPORT_OPENAL

#ifdef BASE_SUPPORT_OGG
	#include <ogg/ogg.h>
	#include <vorbis/vorbisfile.h>
#endif // BASE_SUPPORT_OGG

#include "Math/Vectors.h"

#include "Sound/SoundCommon.h"
#include "Sound/Sound.h"

static snd::MusicPauseCall* s_MusicPauseCall;

void snd::SetMusicPauseCall( snd::MusicPauseCall* call )
{
	s_MusicPauseCall = call;
}

snd::MusicPauseCall* snd::GetMusicPauseCall()
{
	return s_MusicPauseCall;
}

#endif // BASE_SUPPORT_OPENAL
