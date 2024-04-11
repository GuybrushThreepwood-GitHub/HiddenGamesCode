
/*===================================================================
	File: SoundStream.cpp
	Library: SoundLib

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
#include "Sound/SoundCodecs.h"
#include "Sound/OpenAL.h"
#include "Sound/Sound.h"
#include "Sound/SoundStream.h"

#endif // BASE_SUPPORT_OPENAL
