
/*===================================================================
	File: AndroidAudioSystem.cpp
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#ifdef BASE_PLATFORM_ANDROID

#include "CoreBase.h"
#include <map>
#include <vector>
#include "MathBase.h"
#include "SoundBase.h"

#include "Resources/SoundResources.h"
#include "Audio/aos/AndroidAudioSystemBase.h"

#ifdef CABBY_LITE
	#define ANDROID_CLASS_NAME		"uk/co/hiddengames/cabbylite/MusicMediaPlayer"
	#define MAKE_FN_NAME(x)			JNIEXPORT void JNICALL Java_uk_co_hiddengames_cabbylite_MusicMediaPlayer_ ## x
#elif  CABBY_FREE
	#define ANDROID_CLASS_NAME		"uk/co/hiddengames/cabbyfree/MusicMediaPlayer"
	#define MAKE_FN_NAME(x)			JNIEXPORT void JNICALL Java_uk_co_hiddengames_cabbyfree_MusicMediaPlayer_ ## x
#else
	#define ANDROID_CLASS_NAME	"uk/co/hiddengames/cabby/MusicMediaPlayer"
	#define MAKE_FN_NAME(x)			JNIEXPORT void JNICALL Java_uk_co_hiddengames_cabby_MusicMediaPlayer_ ## x
#endif // CABBY_LITE

	#define FUNCTION_NAME(caller)	MAKE_FN_NAME(caller)

namespace
{
	snd::MusicCallback* pMusicCallback = 0;
	char CurrentPlaylistName[core::MAX_PATH];
	char CurrentSongName[core::MAX_PATH];
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
AudioSystemBase::AudioSystemBase(snd::MusicMode mode)
{
	m_MusicMode = mode;
	m_MusicWasPlaying = false;
	
	m_IsPlaying = false;
	m_IsPaused = false;

	m_CurrentBGMTrack = -1;

	pMusicCallback = 0;

	std::memset( CurrentPlaylistName, 0, sizeof(char)*core::MAX_PATH );
	std::memset( CurrentSongName, 0, sizeof(char)*core::MAX_PATH );

	m_UIBufferId = snd::INVALID_SOUNDBUFFER;

	m_SystemPaused = false;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
AudioSystemBase::~AudioSystemBase()
{
	pMusicCallback = 0;
	
	m_CurrentBGMTrack = -1;
	m_SFXState = true;

	m_BGMStream.Release();

	m_UIBufferId = snd::INVALID_SOUNDBUFFER;
}

/////////////////////////////////////////////////////
/// Method: AddSfx
/// Params: [in]pSFX
///
/////////////////////////////////////////////////////
void AudioSystemBase::Update( float deltaTime )
{
	snd::SoundManager::GetInstance()->Update( deltaTime, false );

	if( !m_BGMStream.IsPlaying() )
		m_IsPlaying = false;

	m_BGMStream.Update();
}

/////////////////////////////////////////////////////
/// Method: UpdatePlaylists
/// Params: 
///
/////////////////////////////////////////////////////
void AudioSystemBase::UpdatePlaylists()
{	
	// Get jmethod UpdatePlaylists from MusicMediaPlayer class
	JNIEnv* env = 0;
	core::aos::GetVM()->GetEnv( (void **) &env, JNI_VERSION_1_6);
	jclass javaMusicMediaPlayer = env->FindClass(ANDROID_CLASS_NAME);
	if( javaMusicMediaPlayer != 0 )
	{
		jmethodID jmethodUpdatePlaylists = env->GetStaticMethodID(javaMusicMediaPlayer, "UpdatePlaylists", "()V");

		env->CallStaticVoidMethod( javaMusicMediaPlayer, jmethodUpdatePlaylists );
	}

}

/////////////////////////////////////////////////////
/// Method: PlayMusicTrack
/// Params: filename
///
/////////////////////////////////////////////////////
void AudioSystemBase::PlayBGMTrack( const char* filename, bool loop )
{
	// convert an mp3 to ogg lookup
	file::TFile fileSplit;
	char newPath[core::MAX_PATH+core::MAX_PATH];
	std::memset( newPath, 0, sizeof(char)*core::MAX_PATH+core::MAX_PATH );
		
	file::CreateFileStructure( filename, &fileSplit );

	if( std::strcmp( fileSplit.szFileExt, ".mp3" ) == 0 ||
		std::strcmp( fileSplit.szFileExt, ".MP3" ) == 0 )
	{
		snprintf( newPath, core::MAX_PATH+core::MAX_PATH, "%sogg/%s.ogg", fileSplit.szDrive, fileSplit.szFile );
		m_BGMStream.StreamOpen( newPath );
		m_BGMStream.StartPlayback(loop);
	}
	else
		m_BGMStream.StreamOpen( filename );

	m_IsPlaying = true;
}

/////////////////////////////////////////////////////
/// Method: StopMusicTrack
/// Params: None
///
/////////////////////////////////////////////////////
void AudioSystemBase::StopBGMTrack()
{
	m_BGMStream.Stop();	

	m_IsPlaying = false;	
}

/////////////////////////////////////////////////////
/// Method: ClearMusicTrack
/// Params: None
///
/////////////////////////////////////////////////////
void AudioSystemBase::ClearBGMTrack()
{
	m_BGMStream.Release();

	m_IsPlaying = false;
	m_IsPaused = false;
}

/////////////////////////////////////////////////////
/// Method: SetMusicTrackVolume
/// Params: None
///
/////////////////////////////////////////////////////
void AudioSystemBase::SetBGMTrackVolume( float vol )
{
	math::Clamp( &vol, 0.0f, 1.0f );

	m_BGMStream.SetVolume( vol );	
}

/////////////////////////////////////////////////////
/// Method: PauseMusicTrack
/// Params: None
///
/////////////////////////////////////////////////////
void AudioSystemBase::PauseBGMTrack()
{
	m_BGMStream.Pause();
	m_IsPaused = true;	
}

/////////////////////////////////////////////////////
/// Method: ResumeMusicTrack
/// Params: None
///
/////////////////////////////////////////////////////
void AudioSystemBase::ResumeBGMTrack()
{
	m_BGMStream.Play();
	m_IsPaused = false;	
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////
/// Method: SetShuffleMode
/// Params: [in]mode
///
/////////////////////////////////////////////////////
void AudioSystemBase::SetShuffleMode( int mode )
{

}

/////////////////////////////////////////////////////
/// Method: SetRepeatFlag
/// Params: [in]state
///
/////////////////////////////////////////////////////
void AudioSystemBase::SetRepeatFlag( bool state )
{

}

/////////////////////////////////////////////////////
/// Method: IsPlaying
/// Params: None
///
/////////////////////////////////////////////////////
bool AudioSystemBase::IsPlaying()
{
	if( m_MusicMode == snd::MusicMode_SingleFilePlayer )
	{
		return m_IsPlaying;
	}
	else if( m_MusicMode == snd::MusicMode_PlaylistPlayer )
	{
		// Get jmethod IsPlaying from MusicMediaPlayer class
		JNIEnv* env = 0;
		core::aos::GetVM()->GetEnv( (void **) &env, JNI_VERSION_1_6);
		jclass javaMusicMediaPlayer = env->FindClass(ANDROID_CLASS_NAME);
		if( javaMusicMediaPlayer != 0 )
		{
			jmethodID jmethodIsPlaying = env->GetStaticMethodID(javaMusicMediaPlayer, "IsPlaying", "()Z");

			jboolean val = env->CallStaticBooleanMethod( javaMusicMediaPlayer, jmethodIsPlaying );

			return(static_cast<bool>(val));
		}
	}
	return false;
}

/////////////////////////////////////////////////////
/// Method: IsPaused
/// Params: None
///
/////////////////////////////////////////////////////
bool AudioSystemBase::IsPaused()
{
	if( m_MusicMode == snd::MusicMode_SingleFilePlayer )
	{
		return m_IsPaused;
	}
	else if( m_MusicMode == snd::MusicMode_PlaylistPlayer )
	{
		// Get jmethod IsPaused from MusicMediaPlayer class
		JNIEnv* env = 0;
		core::aos::GetVM()->GetEnv( (void **) &env, JNI_VERSION_1_6);
		jclass javaMusicMediaPlayer = env->FindClass(ANDROID_CLASS_NAME);
		if( javaMusicMediaPlayer != 0 )
		{
			jmethodID jmethodIsPaused = env->GetStaticMethodID(javaMusicMediaPlayer, "IsPaused", "()Z");

			jboolean val = env->CallStaticBooleanMethod( javaMusicMediaPlayer, jmethodIsPaused );

			return(static_cast<bool>(val));
		}
	}

	return false;
}

/////////////////////////////////////////////////////
/// Method: IsUsingPlaylists
/// Params: None
///
/////////////////////////////////////////////////////
bool AudioSystemBase::IsUsingPlaylists()
{
	// Get jmethod IsUsingPlaylists from MusicMediaPlayer class
	JNIEnv* env = 0;
	core::aos::GetVM()->GetEnv( (void **) &env, JNI_VERSION_1_6);
	jclass javaMusicMediaPlayer = env->FindClass(ANDROID_CLASS_NAME);
	if( javaMusicMediaPlayer != 0 )
	{
		jmethodID jmethodIsUsingPlaylists = env->GetStaticMethodID(javaMusicMediaPlayer, "IsUsingPlaylists", "()Z");

		jboolean val = env->CallStaticBooleanMethod( javaMusicMediaPlayer, jmethodIsUsingPlaylists );

		return(static_cast<bool>(val));
	}

	return false;
}

/////////////////////////////////////////////////////
/// Method: GetCurrentSong
/// Params: None
///
/////////////////////////////////////////////////////
const char* AudioSystemBase::GetCurrentSong()
{
	// Get jmethod GetCurrentSong from MusicMediaPlayer class
	JNIEnv* env = 0;
	core::aos::GetVM()->GetEnv( (void **) &env, JNI_VERSION_1_6);
	jclass javaMusicMediaPlayer = env->FindClass(ANDROID_CLASS_NAME);
	if( javaMusicMediaPlayer != 0 )
	{
		jmethodID jmethodGetCurrentSong = env->GetStaticMethodID(javaMusicMediaPlayer, "GetCurrentSong", "()Ljava/lang/String;");

		jstring val = (jstring)env->CallStaticObjectMethod( javaMusicMediaPlayer, jmethodGetCurrentSong );

		const char *buffer = env->GetStringUTFChars( val, JNI_FALSE );
		if( buffer != 0 )
		{
			// android will have full sdcard path so let's remove it
			file::TFile filePath;
			file::CreateFileStructure( buffer, &filePath );

			snprintf( CurrentSongName, core::MAX_PATH, "%s", filePath.szFile );
		}
		env->ReleaseStringUTFChars( val, buffer );

		return CurrentSongName;
	}

	return 0;
}

/////////////////////////////////////////////////////
/// Method: GetPlaylistName
/// Params: [in]playlistIndex
///
/////////////////////////////////////////////////////
const char* AudioSystemBase::GetPlaylistName( int playlistIndex )
{
	// Get jmethod GetCurrentSong from MusicMediaPlayer class
	JNIEnv* env = 0;
	core::aos::GetVM()->GetEnv( (void **) &env, JNI_VERSION_1_6);
	jclass javaMusicMediaPlayer = env->FindClass(ANDROID_CLASS_NAME);
	if( javaMusicMediaPlayer != 0 )
	{
		jmethodID jmethodGetPlaylistName = env->GetStaticMethodID(javaMusicMediaPlayer, "GetPlaylistName", "(I)Ljava/lang/String;");

		jint playlist = static_cast<jint>(playlistIndex);

		jstring val = (jstring)env->CallStaticObjectMethod( javaMusicMediaPlayer, jmethodGetPlaylistName, playlist );

		const char *buffer = env->GetStringUTFChars( val, JNI_FALSE );
		if( buffer != 0 )
			snprintf( CurrentPlaylistName, core::MAX_PATH, "%s", buffer );
		env->ReleaseStringUTFChars( val, buffer );

		return CurrentPlaylistName;
	}

	return 0;	
}

/////////////////////////////////////////////////////
/// Method: GetSongName
/// Params: [in]playlistIndex, [in]songIndex
///
/////////////////////////////////////////////////////
const char* AudioSystemBase::GetSongName( int playlistIndex, int songIndex )
{
	// Get jmethod GetCurrentSong from MusicMediaPlayer class
	JNIEnv* env = 0;
	core::aos::GetVM()->GetEnv( (void **) &env, JNI_VERSION_1_6);
	jclass javaMusicMediaPlayer = env->FindClass(ANDROID_CLASS_NAME);
	if( javaMusicMediaPlayer != 0 )
	{
		jmethodID jmethodGetSongName = env->GetStaticMethodID(javaMusicMediaPlayer, "GetSongName", "(II)Ljava/lang/String;");

		jint playlist = static_cast<jint>(playlistIndex);
		jint song = static_cast<jint>(songIndex);

		jstring val = (jstring)env->CallStaticObjectMethod( javaMusicMediaPlayer, jmethodGetSongName, playlist, song );

		const char *buffer = env->GetStringUTFChars( val, JNI_FALSE );
		if( buffer != 0 )
		{
			// android will have full sdcard path so let's remove it
			file::TFile filePath;
			file::CreateFileStructure( buffer, &filePath );

			snprintf( CurrentSongName, core::MAX_PATH, "%s", filePath.szFile );
		}
		env->ReleaseStringUTFChars( val, buffer );

		return CurrentSongName;
	}

	return 0;		
}

/////////////////////////////////////////////////////
/// Method: GetTotalNumberOfPlaylists
/// Params: None
///
/////////////////////////////////////////////////////
int AudioSystemBase::GetTotalNumberOfPlaylists()
{
	// Get jmethod GetTotalNumberOfPlaylists from MusicMediaPlayer class
	JNIEnv* env = 0;
	core::aos::GetVM()->GetEnv( (void **) &env, JNI_VERSION_1_6);
	jclass javaMusicMediaPlayer = env->FindClass(ANDROID_CLASS_NAME);
	if( javaMusicMediaPlayer != 0 )
	{
		jmethodID jmethodGetTotalNumberOfPlaylists = env->GetStaticMethodID(javaMusicMediaPlayer, "GetTotalNumberOfPlaylists", "()I");

		jint val = env->CallStaticIntMethod( javaMusicMediaPlayer, jmethodGetTotalNumberOfPlaylists );

		return(static_cast<int>(val));
	}

	return 0;		
}

/////////////////////////////////////////////////////
/// Method: GetTotalNumberOfSongsInPlaylist
/// Params: [in]playlistIndex
///
/////////////////////////////////////////////////////
int AudioSystemBase::GetTotalNumberOfSongsInPlaylist( int playlistIndex )
{
	// Get jmethod GetTotalNumberOfPlaylists from MusicMediaPlayer class
	JNIEnv* env = 0;
	core::aos::GetVM()->GetEnv( (void **) &env, JNI_VERSION_1_6);
	jclass javaMusicMediaPlayer = env->FindClass(ANDROID_CLASS_NAME);
	jmethodID jmethodGetTotalNumberOfSongsInPlaylist = env->GetStaticMethodID(javaMusicMediaPlayer, "GetTotalNumberOfSongsInPlaylist", "(I)I");

	jint playlist = static_cast<int>(playlistIndex);

	jint val = env->CallStaticIntMethod( javaMusicMediaPlayer, jmethodGetTotalNumberOfSongsInPlaylist, playlist );

	return(static_cast<int>(val));

	return 0;		
}

/////////////////////////////////////////////////////
/// Method: GetCurrentPlaylistIndex
/// Params: None
///
/////////////////////////////////////////////////////
int AudioSystemBase::GetCurrentPlaylistIndex()
{
	// Get jmethod GetCurrentPlaylistIndex from MusicMediaPlayer class
	JNIEnv* env = 0;
	core::aos::GetVM()->GetEnv( (void **) &env, JNI_VERSION_1_6);
	jclass javaMusicMediaPlayer = env->FindClass(ANDROID_CLASS_NAME);
	if( javaMusicMediaPlayer != 0 )
	{
		jmethodID jmethodGetCurrentPlaylistIndex = env->GetStaticMethodID(javaMusicMediaPlayer, "GetCurrentPlaylistIndex", "()I");

		jint val = env->CallStaticIntMethod( javaMusicMediaPlayer, jmethodGetCurrentPlaylistIndex );

		return(static_cast<int>(val));
	}

	return -1;		
}

/////////////////////////////////////////////////////
/// Method: GetCurrentSongIndex
/// Params: None
///
/////////////////////////////////////////////////////
int AudioSystemBase::GetCurrentSongIndex()
{
	// Get jmethod GetCurrentSongIndex from MusicMediaPlayer class
	JNIEnv* env = 0;
	core::aos::GetVM()->GetEnv( (void **) &env, JNI_VERSION_1_6);
	jclass javaMusicMediaPlayer = env->FindClass(ANDROID_CLASS_NAME);
	if( javaMusicMediaPlayer != 0 )
	{
		jmethodID jmethodGetCurrentSongIndex = env->GetStaticMethodID(javaMusicMediaPlayer, "GetCurrentSongIndex", "()I");

		jint val = env->CallStaticIntMethod( javaMusicMediaPlayer, jmethodGetCurrentSongIndex );

		return(static_cast<int>(val));
	}

	return -1;	
}

/////////////////////////////////////////////////////
/// Method: PlaySong
/// Params: None
///
/////////////////////////////////////////////////////
void AudioSystemBase::PlaySong( int playlistIndex, int songIndex )
{
	// Get jmethod PlaySong from MusicMediaPlayer class
	JNIEnv* env = 0;
	core::aos::GetVM()->GetEnv( (void **) &env, JNI_VERSION_1_6);
	jclass javaMusicMediaPlayer = env->FindClass(ANDROID_CLASS_NAME);
	if( javaMusicMediaPlayer != 0 )
	{
		jmethodID jmethodPlaySong = env->GetStaticMethodID(javaMusicMediaPlayer, "PlaySong", "(II)V");

		jint playlist = static_cast<jint>(playlistIndex);
		jint song = static_cast<jint>(songIndex);

		env->CallStaticVoidMethod( javaMusicMediaPlayer, jmethodPlaySong, playlist, song );
	}

}

/////////////////////////////////////////////////////
/// Method: PlaySong
/// Params: None
///
/////////////////////////////////////////////////////
void AudioSystemBase::PlaySong()
{
	// Get jmethod PlaySong from MusicMediaPlayer class
	JNIEnv* env = 0;
	core::aos::GetVM()->GetEnv( (void **) &env, JNI_VERSION_1_6);
	jclass javaMusicMediaPlayer = env->FindClass(ANDROID_CLASS_NAME);
	if( javaMusicMediaPlayer != 0 )
	{
		jmethodID jmethodPlaySong = env->GetStaticMethodID(javaMusicMediaPlayer, "PlaySong", "()V");

		env->CallStaticVoidMethod( javaMusicMediaPlayer, jmethodPlaySong );
	}
}

/////////////////////////////////////////////////////
/// Method: PlaySong
/// Params: None
///
/////////////////////////////////////////////////////
void AudioSystemBase::PauseSong()
{
	// Get jmethod PauseSong from MusicMediaPlayer class
	JNIEnv* env = 0;
	core::aos::GetVM()->GetEnv( (void **) &env, JNI_VERSION_1_6);
	jclass javaMusicMediaPlayer = env->FindClass(ANDROID_CLASS_NAME);
	if( javaMusicMediaPlayer != 0 )
	{
		jmethodID jmethodPauseSong = env->GetStaticMethodID(javaMusicMediaPlayer, "PauseSong", "()V");

		env->CallStaticVoidMethod( javaMusicMediaPlayer, jmethodPauseSong );
	}

}

/////////////////////////////////////////////////////
/// Method: ResumeSong
/// Params: None
///
/////////////////////////////////////////////////////
void AudioSystemBase::ResumeSong()
{
	// Get jmethod ResumeSong from MusicMediaPlayer class
	JNIEnv* env = 0;
	core::aos::GetVM()->GetEnv( (void **) &env, JNI_VERSION_1_6);
	jclass javaMusicMediaPlayer = env->FindClass(ANDROID_CLASS_NAME);
	if( javaMusicMediaPlayer != 0 )
	{
		jmethodID jmethodResumeSong = env->GetStaticMethodID(javaMusicMediaPlayer, "ResumeSong", "()V");

		env->CallStaticVoidMethod( javaMusicMediaPlayer, jmethodResumeSong );
	}

}

/////////////////////////////////////////////////////
/// Method: StopSong
/// Params: None
///
/////////////////////////////////////////////////////
void AudioSystemBase::StopSong()
{
	// Get jmethod StopSong from MusicMediaPlayer class
	JNIEnv* env = 0;
	core::aos::GetVM()->GetEnv( (void **) &env, JNI_VERSION_1_6);
	jclass javaMusicMediaPlayer = env->FindClass(ANDROID_CLASS_NAME);
	if( javaMusicMediaPlayer != 0 )
	{
		jmethodID jmethodStopSong = env->GetStaticMethodID(javaMusicMediaPlayer, "StopSong", "()V");

		env->CallStaticVoidMethod( javaMusicMediaPlayer, jmethodStopSong );
	}	
}

/////////////////////////////////////////////////////
/// Method: SkipToPreviousSong
/// Params: None
///
/////////////////////////////////////////////////////
void AudioSystemBase::SkipToPreviousSong()
{
	// Get jmethod SkipToPreviousSong from MusicMediaPlayer class
	JNIEnv* env = 0;
	core::aos::GetVM()->GetEnv( (void **) &env, JNI_VERSION_1_6);
	jclass javaMusicMediaPlayer = env->FindClass(ANDROID_CLASS_NAME);
	if( javaMusicMediaPlayer != 0 )
	{
		jmethodID jmethodSkipToPreviousSong = env->GetStaticMethodID(javaMusicMediaPlayer, "SkipToPreviousSong", "()V");

		env->CallStaticVoidMethod( javaMusicMediaPlayer, jmethodSkipToPreviousSong );
	}
}

/////////////////////////////////////////////////////
/// Method: SkipToNextSong
/// Params: None
///
/////////////////////////////////////////////////////
void AudioSystemBase::SkipToNextSong()
{
	// Get jmethod SkipToNextSong from MusicMediaPlayer class
	JNIEnv* env = 0;
	core::aos::GetVM()->GetEnv( (void **) &env, JNI_VERSION_1_6);
	jclass javaMusicMediaPlayer = env->FindClass(ANDROID_CLASS_NAME);
	if( javaMusicMediaPlayer != 0 )
	{
		jmethodID jmethodSkipToNextSong = env->GetStaticMethodID(javaMusicMediaPlayer, "SkipToNextSong", "()V");

		env->CallStaticVoidMethod( javaMusicMediaPlayer, jmethodSkipToNextSong );
	}
}

/////////////////////////////////////////////////////
/// Method: SetMusicCallback
/// Params: [in]pCallback
///
/////////////////////////////////////////////////////
void AudioSystemBase::SetMusicCallback( snd::MusicCallback* pCallback )
{
	pMusicCallback = pCallback;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////
/// Method: PlayUIAudio
/// Params: None
///
/////////////////////////////////////////////////////
void AudioSystemBase::PlayUIAudio()
{
	float soundMultiply = 1.0f;
	if(!m_SFXState)
		soundMultiply = 0.0f;

	ALuint sourceId = snd::INVALID_SOUNDSOURCE;
	sourceId = snd::SoundManager::GetInstance()->GetFreeSoundSource();

	if( sourceId != snd::INVALID_SOUNDSOURCE &&
		m_UIBufferId != snd::INVALID_SOUNDBUFFER )
	{
		CHECK_OPENAL_ERROR( alSourcei( sourceId, AL_BUFFER, m_UIBufferId ) )
		CHECK_OPENAL_ERROR( alSourcei( sourceId, AL_SOURCE_RELATIVE, AL_TRUE ) )
		CHECK_OPENAL_ERROR( alSourcei( sourceId, AL_LOOPING, AL_FALSE ) )
		CHECK_OPENAL_ERROR( alSourcef( sourceId, AL_PITCH, 1.0f ) )
		CHECK_OPENAL_ERROR( alSourcef( sourceId, AL_GAIN, 1.0f*soundMultiply ) )
		CHECK_OPENAL_ERROR( alSource3f( sourceId, AL_POSITION, 0.0f, 0.0f, 0.0f ) )
		
		CHECK_OPENAL_ERROR( alSourcef( sourceId, AL_MAX_DISTANCE, MAX_AUDIO_DISTANCE ) )
		CHECK_OPENAL_ERROR( alSourcef( sourceId, AL_ROLLOFF_FACTOR, ROLL_OFF ) )
		CHECK_OPENAL_ERROR( alSourcef( sourceId, AL_REFERENCE_DISTANCE, REF_DISTANCE ) )
		
		CHECK_OPENAL_ERROR( alSourcePlay( sourceId ) )
	}
}

/////////////////////////////////////////////////////
/// Method: PlayAudio
/// Params: [in]sourceId, [in]bufferId, [in]pos, [in]sourceRel
///
/////////////////////////////////////////////////////
void AudioSystemBase::PlayAudio( ALuint sourceId, ALuint bufferId, math::Vec3 pos, bool sourceRel, bool loop, float pitch, float gain )
{
	float soundMultiply = 1.0f;
	if(!m_SFXState)
		soundMultiply = 0.0f;

	if( sourceId == snd::INVALID_SOUNDSOURCE ||
		bufferId == snd::INVALID_SOUNDBUFFER )
		return;

	CHECK_OPENAL_ERROR( alSourcei( sourceId, AL_BUFFER, bufferId ) )
	CHECK_OPENAL_ERROR( alSourcei( sourceId, AL_SOURCE_RELATIVE, sourceRel ) )
	CHECK_OPENAL_ERROR( alSourcei( sourceId, AL_LOOPING, loop ) )
	CHECK_OPENAL_ERROR( alSourcef( sourceId, AL_PITCH, pitch ) )
	CHECK_OPENAL_ERROR( alSourcef( sourceId, AL_GAIN, gain*soundMultiply ) )
	CHECK_OPENAL_ERROR( alSource3f( sourceId, AL_POSITION, pos.X, pos.Y, pos.Z ) )
	
	CHECK_OPENAL_ERROR( alSourcef( sourceId, AL_MAX_DISTANCE, MAX_AUDIO_DISTANCE ) )
	CHECK_OPENAL_ERROR( alSourcef( sourceId, AL_ROLLOFF_FACTOR, ROLL_OFF ) )
	CHECK_OPENAL_ERROR( alSourcef( sourceId, AL_REFERENCE_DISTANCE, REF_DISTANCE ) )
	
	CHECK_OPENAL_ERROR( alSourcePlay( sourceId ) )
}

/////////////////////////////////////////////////////
/// Method: PlayAudio
/// Params: [in]bufferId, [in]pos, [in]sourceRel
///
/////////////////////////////////////////////////////
ALuint AudioSystemBase::PlayAudio( ALuint bufferId, math::Vec3 pos, bool sourceRel, bool loop, float pitch, float gain )
{
	float soundMultiply = 1.0f;
	gain = 2.0f;
	if(!m_SFXState)
		soundMultiply = 0.0f;

	ALuint sourceId = snd::SoundManager::GetInstance()->GetFreeSoundSource();

	if( sourceId != snd::INVALID_SOUNDSOURCE &&
		bufferId != snd::INVALID_SOUNDBUFFER )
	{
		CHECK_OPENAL_ERROR( alSourcei( sourceId, AL_BUFFER, bufferId ) )
		CHECK_OPENAL_ERROR( alSourcei( sourceId, AL_SOURCE_RELATIVE, sourceRel ) )
		CHECK_OPENAL_ERROR( alSourcei( sourceId, AL_LOOPING, loop ) )
		CHECK_OPENAL_ERROR( alSourcef( sourceId, AL_PITCH, pitch ) )
		CHECK_OPENAL_ERROR( alSourcef( sourceId, AL_GAIN, gain*soundMultiply ) )
		CHECK_OPENAL_ERROR( alSource3f( sourceId, AL_POSITION, pos.X, pos.Y, pos.Z ) )

		CHECK_OPENAL_ERROR( alSourcef( sourceId, AL_MAX_DISTANCE, MAX_AUDIO_DISTANCE ) )
		CHECK_OPENAL_ERROR( alSourcef( sourceId, AL_ROLLOFF_FACTOR, ROLL_OFF ) )
		CHECK_OPENAL_ERROR( alSourcef( sourceId, AL_REFERENCE_DISTANCE, REF_DISTANCE ) )

		CHECK_OPENAL_ERROR( alSourcePlay( sourceId ) )
	}

	if( loop )
		return(sourceId);

	return(snd::INVALID_SOUNDSOURCE);
}

/////////////////////////////////////////////////////
/// Method: StopAudio
/// Params: [in]sourceId
///
/////////////////////////////////////////////////////
void AudioSystemBase::StopAudio( ALuint sourceId )
{
	CHECK_OPENAL_ERROR( alSourceStop( sourceId ) )
}

/////////////////////////////////////////////////////
/// Method: StopAll
/// Params: None
///
/////////////////////////////////////////////////////
void AudioSystemBase::StopAll()
{
	snd::SoundManager::GetInstance()->StopAll();
}

/////////////////////////////////////////////////////
/// Method: Pause
/// Params: None
///
/////////////////////////////////////////////////////
void AudioSystemBase::Pause()
{
	snd::SoundManager::GetInstance()->Pause();

	m_SystemPaused = true;
}

/////////////////////////////////////////////////////
/// Method: UnPause
/// Params: None
///
/////////////////////////////////////////////////////
void AudioSystemBase::UnPause()
{
	snd::SoundManager::GetInstance()->UnPause();

	m_SystemPaused = false;
}

/////////////////////////////////////////////////////
/// Method: PauseMusic
/// Params: None
///
/////////////////////////////////////////////////////
void AudioSystemBase::PauseMusic()
{
	// Get jmethod IsPlaying from MusicMediaPlayer class
	JNIEnv* env = 0;
	core::aos::GetVM()->GetEnv( (void **) &env, JNI_VERSION_1_6);
	jclass javaMusicMediaPlayer = env->FindClass(ANDROID_CLASS_NAME);
	if( javaMusicMediaPlayer != 0 )
	{	
		jmethodID jmethodIsPlaying = env->GetStaticMethodID(javaMusicMediaPlayer, "IsPlaying", "()Z");
		jmethodID jmethodIsPaused = env->GetStaticMethodID(javaMusicMediaPlayer, "IsPaused", "()Z");
		jmethodID jmethodPauseSong = env->GetStaticMethodID(javaMusicMediaPlayer, "PauseSong", "()V");

		jboolean isPlaying = env->CallStaticBooleanMethod( javaMusicMediaPlayer, jmethodIsPlaying );
		jboolean isPaused = env->CallStaticBooleanMethod( javaMusicMediaPlayer, jmethodIsPaused );

		if( !m_MusicWasPlaying )
		{
			if( isPlaying && !isPaused)
			{
				env->CallStaticVoidMethod( javaMusicMediaPlayer, jmethodPauseSong );
				m_MusicWasPaused = true;
			}
			else 
			{
				m_MusicWasPaused = false;
			}
		}
	}
}

/////////////////////////////////////////////////////
/// Method: UnPauseMusic
/// Params: None
///
/////////////////////////////////////////////////////
void AudioSystemBase::UnPauseMusic()
{
	if( !m_MusicWasPlaying && m_MusicWasPaused)
	{
		// Get jmethod PlaySong from MusicMediaPlayer class
		JNIEnv* env = 0;
		core::aos::GetVM()->GetEnv( (void **) &env, JNI_VERSION_1_6);
		jclass javaMusicMediaPlayer = env->FindClass(ANDROID_CLASS_NAME);
		if( javaMusicMediaPlayer != 0 )
		{
			jmethodID jmethodPlaySong = env->GetStaticMethodID(javaMusicMediaPlayer, "PlaySong", "()V");

			env->CallStaticVoidMethod( javaMusicMediaPlayer, jmethodPlaySong );
		}
	}
}

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

	/////////////////////////////////////////////////////
	/// Function: songChanged
	/// Params: [in]env, [in]thiz, [in]songName
	///
	/////////////////////////////////////////////////////
	FUNCTION_NAME(songChanged)( JNIEnv* env, jobject thiz, jstring songName )
	{
		DBGLOG( "AUDIOSYSTEM: songChanged" );

		const char *buffer = env->GetStringUTFChars( songName, JNI_FALSE );
		
		if( buffer != 0 )
		{
			// android will have full sdcard path so let's remove it
			file::TFile filePath;
			file::CreateFileStructure( buffer, &filePath );

			snprintf( CurrentSongName, core::MAX_PATH, "%s", filePath.szFile );

			if( pMusicCallback != 0 )
				pMusicCallback->PlayingSongChanged();
		}		

		env->ReleaseStringUTFChars( songName, buffer );
	}

#ifdef  __cplusplus
}
#endif // __cplusplus

#endif // BASE_PLATFORM_ANDROID