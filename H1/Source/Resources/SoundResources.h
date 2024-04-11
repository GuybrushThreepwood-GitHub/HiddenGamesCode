
/*===================================================================
	File: SoundResources.h
	Game: AirCadets

	(C)Hidden Games
=====================================================================*/

#ifndef __SOUNDRESOURCES_H__
#define __SOUNDRESOURCES_H__

namespace res
{
	// sound info store
	struct SoundResourceStore
	{
		const char* sndFile;
		//ALuint bufferId;
	};

	void CreateSoundResourceMap();

	void ClearSoundResources();

	const char* GetSoundResource( int index );
}

#endif // __SOUNDRESOURCES_H__
