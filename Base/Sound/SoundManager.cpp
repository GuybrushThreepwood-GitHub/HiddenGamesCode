
/*===================================================================
	File: SoundManager.cpp
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
#include "Sound/OpenAL.h"
#include "Sound/Sound.h"
#include "Sound/SoundManager.h"

using snd::SoundManager;

namespace
{
	const int _X_ = 0;
	const int _Y_ = 1;
	const int _Z_ = 2;
}

SoundManager* SoundManager::ms_Instance = 0;

/////////////////////////////////////////////////////
/// Static Method: Initialise
/// Params: None
///
/////////////////////////////////////////////////////
void SoundManager::Initialise( void )
{
	DBG_ASSERT( (ms_Instance == 0) );

	DBG_MEMTRY
		ms_Instance = new SoundManager;
	DBG_MEMCATCH
}

/////////////////////////////////////////////////////
/// Static Method: Shutdown
/// Params: None
///
/////////////////////////////////////////////////////
void SoundManager::Shutdown( void )
{
	if( ms_Instance )
	{
		delete ms_Instance;
		ms_Instance = 0;
	}
}

/////////////////////////////////////////////////////
/// Default constructor
/// 
///
/////////////////////////////////////////////////////
SoundManager::SoundManager()
{
	DBGLOG( "\n----- Starting Sound Manager -----\n" );

	unsigned int i;

	m_eState = SOUNDMANAGER_STATE_PLAYING;
	m_TotalSoundCount = 0;
	m_TotalSoundMemory = 0;

	m_NumberOfSources = SOUND_MAXSOURCES;

	ALuint nTempSourceID[SOUND_MAXSOURCES];
	for( i = 0; i < SOUND_MAXSOURCES; i++ )
		nTempSourceID[i] = snd::INVALID_SOUNDSOURCE;
	
	ALint distanceModel = alGetInteger( AL_DISTANCE_MODEL );
	switch( distanceModel )
	{
		case AL_NONE:
			DBGLOG( "Distance Model - AL_NONE\n" );
		break;
		case AL_INVERSE_DISTANCE:
			DBGLOG( "Distance Model - AL_INVERSE_DISTANCE\n" );
		break;
		case AL_INVERSE_DISTANCE_CLAMPED:
			DBGLOG( "Distance Model - AL_INVERSE_DISTANCE_CLAMPED\n" );
		break;
		case AL_LINEAR_DISTANCE:
			DBGLOG( "Distance Model - AL_LINEAR_DISTANCE\n" );
		break;
		case AL_LINEAR_DISTANCE_CLAMPED:
			DBGLOG( "Distance Model - AL_LINEAR_DISTANCE_CLAMPED\n" );
		break;
		case AL_EXPONENT_DISTANCE:
			DBGLOG( "Distance Model - AL_EXPONENT_DISTANCE\n" );
		break;			
		case AL_EXPONENT_DISTANCE_CLAMPED:
			DBGLOG( "Distance Model - AL_EXPONENT_DISTANCE_CLAMPED\n" );
		break;			
	}
	
	// find out how many sound sources this AL device can have ( if infinite, just get 32 )
	for( i = 0; i < SOUND_MAXSOURCES; i++ )
	{
		nTempSourceID[i] = snd::INVALID_SOUNDSOURCE;
		alGenSources( 1, &nTempSourceID[i] );

		// was there an error (should be INVALID_VALUE when source list is full)
        if( alGetError() == AL_NO_ERROR )
			continue;
		else
		{
			// ok max sources have been reached, check the index and save it
			// -1 to reserve a source for music
			m_NumberOfSources = i-1;
			
			// break out if the i loop
			break;
		}
	}

    
	// delete the created ones
	for( i = 0; i < SOUND_MAXSOURCES; i++ )
	{
		if( nTempSourceID[i] != snd::INVALID_SOUNDSOURCE )
			CHECK_OPENAL_ERROR( alDeleteSources( 1, &nTempSourceID[i] ) )
	}
	DBGLOG( "MAX SOUND SOURCES: %d\n", m_NumberOfSources );

	// reserve for music
	if( m_NumberOfSources <= SOUND_MAXSOURCES )
		m_NumberOfSources = m_NumberOfSources-1;

	// now create the proper manager sources
	m_pSourceIDs = 0;

	DBG_MEMTRY
		m_pSourceIDs = new ALuint[m_NumberOfSources];
	DBG_MEMCATCH

	for( i = 0; i < m_NumberOfSources; i++ )
	{
		// initialise 
		m_pSourceIDs[i] = INVALID_SOUNDSOURCE;
		CHECK_OPENAL_ERROR( alGenSources( 1, &m_pSourceIDs[i] ) )

		if( i == 0 )
		{
			ALint boolFlag;
			ALfloat floatVal;

			DBGLOG( "Default Source Settings\n" );

			alGetSourcef( m_pSourceIDs[i], AL_GAIN, &floatVal );
			DBGLOG( "AL_GAIN = %f\n", floatVal );
			alGetSourcef( m_pSourceIDs[i], AL_MIN_GAIN, &floatVal );
			DBGLOG( "AL_MIN_GAIN = %f\n", floatVal );
			alGetSourcef( m_pSourceIDs[i], AL_MAX_GAIN, &floatVal );
			DBGLOG( "AL_MAX_GAIN = %f\n", floatVal );
			alGetSourcei( m_pSourceIDs[i], AL_SOURCE_RELATIVE, &boolFlag );
			DBGLOG( "AL_SOURCE_RELATIVE = %d\n", boolFlag );
			alGetSourcef( m_pSourceIDs[i], AL_REFERENCE_DISTANCE, &floatVal );
			DBGLOG( "AL_REFERENCE_DISTANCE = %f\n", floatVal );
			alGetSourcef( m_pSourceIDs[i], AL_MAX_DISTANCE, &floatVal );
			DBGLOG( "AL_MAX_DISTANCE = %f\n", floatVal );
			alGetSourcef( m_pSourceIDs[i], AL_ROLLOFF_FACTOR, &floatVal );
			DBGLOG( "AL_ROLLOFF_FACTOR = %f\n", floatVal );
			alGetSourcef( m_pSourceIDs[i], AL_CONE_INNER_ANGLE, &floatVal );
			DBGLOG( "AL_CONE_INNER_ANGLE = %f\n", floatVal );
			alGetSourcef( m_pSourceIDs[i], AL_CONE_OUTER_ANGLE, &floatVal );
			DBGLOG( "AL_CONE_OUTER_ANGLE = %f\n", floatVal );
			alGetSourcef( m_pSourceIDs[i], AL_CONE_OUTER_GAIN, &floatVal );
			DBGLOG( "AL_CONE_OUTER_GAIN = %f\n", floatVal );
			alGetSourcef( m_pSourceIDs[i], AL_PITCH, &floatVal );
			DBGLOG( "AL_PITCH = %f\n", floatVal );
			alGetSourcei( m_pSourceIDs[i], AL_LOOPING, &boolFlag );
			DBGLOG( "AL_LOOPING = %d\n", boolFlag );
		}

		// make all sources default to no distance attenuation
		CHECK_OPENAL_ERROR( alSourcef( m_pSourceIDs[i], AL_MAX_DISTANCE, 0.01f ) )
	}

	// default listener positions
	m_ListenerPos[_X_] = 0.0f;
	m_ListenerPos[_Y_] = 0.0f;
	m_ListenerPos[_Z_] = 0.0f;

	m_ListenerOrientation[0] = 0.0f;
	m_ListenerOrientation[1] = 0.0f;
	m_ListenerOrientation[2] = 0.0f;
	m_ListenerOrientation[3] = 0.0f;
	m_ListenerOrientation[4] = 1.0f;
	m_ListenerOrientation[5] = 0.0f;

	CHECK_OPENAL_ERROR( alListenerfv( AL_POSITION, (float *)m_ListenerPos) )
	CHECK_OPENAL_ERROR( alListenerfv( AL_ORIENTATION, (float *)m_ListenerOrientation) )

	m_NumActiveSources = 0;
}

/////////////////////////////////////////////////////
/// Default destructor
/// 
///
/////////////////////////////////////////////////////
SoundManager::~SoundManager()
{
	DBGLOG( "\n----- Closing Sound Manager -----\n" );

	unsigned int i = 0;

	// delete the list of sound sources that were created
	for( i = 0; i < m_NumberOfSources; i++ )
	{
		// initialise 
		if( m_pSourceIDs[i] != INVALID_SOUNDSOURCE )
		{
			CHECK_OPENAL_ERROR( alSourceStop( m_pSourceIDs[i] ) )
			CHECK_OPENAL_ERROR( alDeleteSources( 1, &m_pSourceIDs[i] ) )
			m_pSourceIDs[i] = INVALID_SOUNDSOURCE;
		}
	}
	
	// delete the source id memory
	if( m_pSourceIDs )
	{
		delete m_pSourceIDs;
		m_pSourceIDs = 0;
	}
	
	m_TotalSoundCount = 0;
	m_TotalSoundMemory = 0;
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]fDelta, [in]bFullUpdate
///
/////////////////////////////////////////////////////
void SoundManager::Update( float fDelta, bool bFullUpdate )
{
	// are sounds paused or off...don't do an update
	if( m_eState == SOUNDMANAGER_STATE_PAUSED || m_eState == SOUNDMANAGER_STATE_STOPPED )
		return;

	unsigned int i = 0;

	// reset all the sources
	m_NumActiveSources = 0;

	// get the source states
	for( i = 0; i < m_NumberOfSources; i++ )
	{
		// initialise 
		if( m_pSourceIDs[i] != INVALID_SOUNDSOURCE )
		{
			ALenum state;
			CHECK_OPENAL_ERROR( alGetSourcei( m_pSourceIDs[i], AL_SOURCE_STATE, &state ) )

			if( state == AL_PLAYING )
				m_NumActiveSources++;
		}
	}
}

/////////////////////////////////////////////////////
/// Method: Pause
/// Params: None
///
/////////////////////////////////////////////////////
void SoundManager::Pause()
{
	unsigned int i = 0;

	// get the source states
	for( i = 0; i < m_NumberOfSources; i++ )
	{
		// pause 
		if( m_pSourceIDs[i] != INVALID_SOUNDSOURCE )
		{
			ALenum state;
			CHECK_OPENAL_ERROR( alGetSourcei( m_pSourceIDs[i], AL_SOURCE_STATE, &state ) )

			if( state == AL_PLAYING )
			{
				CHECK_OPENAL_ERROR( alSourcePause( m_pSourceIDs[i] ) )
			}
		}
	}

	m_eState = SOUNDMANAGER_STATE_PAUSED;
}

/////////////////////////////////////////////////////
/// Method: UnPause
/// Params: None
///
/////////////////////////////////////////////////////
void SoundManager::UnPause()
{
	unsigned int i = 0;

	// get the source states
	for( i = 0; i < m_NumberOfSources; i++ )
	{
		// unpause 
		if( m_pSourceIDs[i] != INVALID_SOUNDSOURCE )
		{
			ALenum state;
			CHECK_OPENAL_ERROR( alGetSourcei( m_pSourceIDs[i], AL_SOURCE_STATE, &state ) )

			if( state == AL_PAUSED )
			{
				CHECK_OPENAL_ERROR( alSourcePlay( m_pSourceIDs[i] ) )
			}
		}
	}

	m_eState = SOUNDMANAGER_STATE_PLAYING;
}

/////////////////////////////////////////////////////
/// Method: StopAll
/// Params: None
///
/////////////////////////////////////////////////////
void SoundManager::StopAll()
{
	unsigned int i = 0;

	// get the source states
	for( i = 0; i < m_NumberOfSources; i++ )
	{
		// stop all 
		if( m_pSourceIDs[i] != INVALID_SOUNDSOURCE )
		{
			CHECK_OPENAL_ERROR( alSourceStop( m_pSourceIDs[i] ) )
		}
	}
}

/////////////////////////////////////////////////////
/// Method: Stop
/// Params: None
///
/////////////////////////////////////////////////////
void SoundManager::Stop( ALuint bufferId )
{
	unsigned int i = 0;

	// get the source states
	for( i = 0; i < m_NumberOfSources; i++ )
	{
		// stop if buffer id matches 
		if( m_pSourceIDs[i] != INVALID_SOUNDSOURCE )
		{
			ALint sourceBuffer = -1;

			CHECK_OPENAL_ERROR( alGetSourcei( m_pSourceIDs[i], AL_BUFFER, &sourceBuffer ) )

			if( sourceBuffer == bufferId )
			{
				CHECK_OPENAL_ERROR( alSourcei( m_pSourceIDs[i], AL_LOOPING, false ) )
				CHECK_OPENAL_ERROR( alSourceStop( m_pSourceIDs[i] ) )

				// iphone runs in a thread, do no delete the buffer until it has stopped
				ALenum state;
				state = AL_PLAYING;
				while( (state != AL_STOPPED) ) 
				{
					alGetSourcei( m_pSourceIDs[i], AL_SOURCE_STATE, &state );
				}

				CHECK_OPENAL_ERROR( alSourcei( m_pSourceIDs[i], AL_BUFFER, 0 ) )
			}
		}
	}
}

/////////////////////////////////////////////////////
/// Method: GetSoundState
/// Params: [in]nSoundSource
///
/////////////////////////////////////////////////////
ALuint SoundManager::GetSoundState( unsigned int nSoundSource )
{
	if( nSoundSource == INVALID_SOUNDSOURCE )
		return(INVALID_SOUNDSOURCE);

	// find the state of requested sound
	unsigned int i = 0;
	for( i = 0; i < m_NumberOfSources; i++ )
	{
		if( nSoundSource == m_pSourceIDs[i] )
		{
			ALenum state;
			CHECK_OPENAL_ERROR( alGetSourcei( m_pSourceIDs[i], AL_SOURCE_STATE, &state) )
			return(state);
		}
	}

	return(INVALID_SOUNDSOURCE);
}

/////////////////////////////////////////////////////
/// Method: GetFreeSoundSource
/// Params: None
///
/////////////////////////////////////////////////////
ALuint SoundManager::GetFreeSoundSource( void )
{
	unsigned int i = 0;

	// find a source that is either in the stopped or initial state
	for( i = 0; i < m_NumberOfSources; i++ )
	{
		ALenum state;
		CHECK_OPENAL_ERROR( alGetSourcei( m_pSourceIDs[i], AL_SOURCE_STATE, &state) )

		if( state == AL_STOPPED || state == AL_INITIAL )
			return(m_pSourceIDs[i]);
	}

	return(INVALID_SOUNDSOURCE);
}

/////////////////////////////////////////////////////
/// Method: SetListenerPosition
/// Params: [in]fPosX, [in]fPosY, [in]fPosZ
///
/////////////////////////////////////////////////////
void SoundManager::SetListenerPosition( float fPosX, float fPosY, float fPosZ )
{
	// new listener position
	m_ListenerPos[_X_] = fPosX;
	m_ListenerPos[_Y_] = fPosY;
	m_ListenerPos[_Z_] = fPosZ;

	CHECK_OPENAL_ERROR( alListenerfv( AL_POSITION, (float *)m_ListenerPos) )
}

/////////////////////////////////////////////////////
/// Method: SetListenerPosition
/// Params: [in]vPos
///
/////////////////////////////////////////////////////
void SoundManager::SetListenerPosition( math::Vec3 &vPos )
{
	// new listener position
	m_ListenerPos[_X_] = vPos.X;
	m_ListenerPos[_Y_] = vPos.Y;
	m_ListenerPos[_Z_] = vPos.Z;

	CHECK_OPENAL_ERROR( alListenerfv( AL_POSITION, (float *)m_ListenerPos) )
}

/////////////////////////////////////////////////////
/// Method: SetListenerOrientation
/// Params: [in]fAngleX, [in]fAngleY, [in]fAngleZ, [in]fUpX, [in]fUpY, [in]fUpZ
///
/////////////////////////////////////////////////////
void SoundManager::SetListenerOrientation( float fAngleX, float fAngleY, float fAngleZ, float fUpX, float fUpY, float fUpZ )
{
	// new listener orientation
	m_ListenerOrientation[0] = fAngleX;
	m_ListenerOrientation[1] = fAngleY;
	m_ListenerOrientation[2] = fAngleZ;
	m_ListenerOrientation[3] = fUpX;
	m_ListenerOrientation[4] = fUpY;
	m_ListenerOrientation[5] = fUpZ;

	CHECK_OPENAL_ERROR( alListenerfv( AL_ORIENTATION, (float *)m_ListenerOrientation) )
}

/////////////////////////////////////////////////////
/// Method: SetListenerOrientation
/// Params: [in]vAngle, [in]vUp
///
/////////////////////////////////////////////////////
void SoundManager::SetListenerOrientation( math::Vec3 &vAngle, math::Vec3 &vUp )
{
	// new listener orientation
	m_ListenerOrientation[0] = vAngle.X;
	m_ListenerOrientation[1] = vAngle.Y;
	m_ListenerOrientation[2] = vAngle.Z;
	m_ListenerOrientation[3] = vUp.X;
	m_ListenerOrientation[4] = vUp.Y;
	m_ListenerOrientation[5] = vUp.Z;

	CHECK_OPENAL_ERROR( alListenerfv( AL_ORIENTATION, (float *)m_ListenerOrientation) )
}

/////////////////////////////////////////////////////
/// Method: GetListenerPosition
/// Params: [out]fPosX, [out]fPosY, [out]fPosZ
///
/////////////////////////////////////////////////////
void SoundManager::GetListenerPosition( float *fPosX, float *fPosY, float *fPosZ )
{
	if( fPosX )
	{
		*fPosX = m_ListenerPos[_X_];
	}
	if( fPosY )
	{
		*fPosY = m_ListenerPos[_Y_];
	}
	if( fPosZ )
	{
		*fPosZ = m_ListenerPos[_Z_];
	}
}

/////////////////////////////////////////////////////
/// Method: GetListenerPosition
/// Params: [out]vPos
///
/////////////////////////////////////////////////////
void SoundManager::GetListenerPosition( math::Vec3 &vPos )
{
	vPos.X = m_ListenerPos[_X_];
	vPos.Y = m_ListenerPos[_Y_];
	vPos.Z = m_ListenerPos[_Z_];
}

/////////////////////////////////////////////////////
/// Method: GetListenerOrientation
/// Params: [out]fAngleX, [out]fAngleY, [out]fAngleZ, [out]fUpX, [out]fUpY, [out]fUpZ 
///
/////////////////////////////////////////////////////
void SoundManager::GetListenerOrientation( float *fAngleX, float *fAngleY, float *fAngleZ, float *fUpX, float *fUpY, float *fUpZ )
{
	if( fAngleX )
	{
		*fAngleX = m_ListenerOrientation[0];
	}
	if( fAngleY )
	{
		*fAngleY = m_ListenerOrientation[1];
	}
	if( fAngleZ )
	{
		*fAngleZ = m_ListenerOrientation[2];
	}
	if( fUpX )
	{
		*fUpX = m_ListenerOrientation[3];
	}
	if( fUpY )
	{
		*fUpY = m_ListenerOrientation[4];
	}
	if( fUpZ )
	{
		*fUpZ = m_ListenerOrientation[5];
	}
}

#endif // BASE_SUPPORT_OPENAL
