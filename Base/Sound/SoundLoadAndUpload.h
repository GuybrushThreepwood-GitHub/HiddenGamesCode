

#ifndef __SOUNDLOADANDUPLOAD_H__
#define __SOUNDLOADANDUPLOAD_H__

#ifdef BASE_SUPPORT_OPENAL

namespace snd
{
	/// ClearSoundMap - clears the sound name map
	void ClearSoundMap();
	/// SoundLoad - Will load a sound and upload to AL 
	ALuint SoundLoad( const char *szFilename, Sound& sound );
	/// RemoveSound - removes a sound from the map
	/// \param bufferId - sound buffer id
	void RemoveSound( ALuint bufferId );

} // namespace snd

#endif // BASE_SUPPORT_OPENAL

#endif // __SOUNDLOADANDUPLOAD_H__


