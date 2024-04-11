
#ifndef __SOUNDMANAGER_H__
#define __SOUNDMANAGER_H__

#ifdef BASE_SUPPORT_OPENAL

namespace snd
{
	const int SOUND_MAXSOURCES			= 32;

	enum ESoundManagerState
	{
		SOUNDMANAGER_STATE_PLAYING=0,
		SOUNDMANAGER_STATE_PAUSED,
		SOUNDMANAGER_STATE_STOPPED
	};

	class SoundBuffer
	{
		public:
			/// sound filename
			file::TFile SoundBufferFile;
			/// sound AL buffer id
			ALuint nBufferID;

			/// sound format
			int nFormat;
			/// sound bitrate
			int nBitRate;
			/// sound frequency
			int nFrequency;
			/// number of channels
			int nChannels;
			/// bytes per second
			int nBytesPerSec;
			/// sound file size
			int nSoundSize;
	};

	class SoundManager
	{
		public:
			/// default constructor
			SoundManager();
			/// default destructor
			~SoundManager();

			/// Update - Manages all looping and active sounds
			/// \param fDelta - time since last frame
			/// \param bFullUpdate - whether to do the full sound management, or just track the sources
			void Update( float fDelta, bool bFullUpdate );

			/// Pause - Pause all sound sources
			void Pause();
			/// UnPause - UnPause all sound sources
			void UnPause();
			/// StopAll - Stop all sound sources
			void StopAll();
			/// Stop - Stop all sounds with a specific bufferId
			void Stop( ALuint bufferId );

			/// SetState - Sets the managers state
			/// \param eState - state to set
			void SetState( ESoundManagerState eState )					{ m_eState = eState; }
			/// GetState - Gets the managers state
			/// \return ESoundManagerState - current state
			ESoundManagerState GetState( void )							{ return(m_eState); }

			/// SetListenerPosition - sets the position of the listener
			/// \param fPosX - X Position of the listener 
			/// \param fPosY - Y Position of the listener 
			/// \param fPosZ - Z Position of the listener 
			void SetListenerPosition( float fPosX, float fPosY, float fPosZ );
			/// SetListenerPosition - sets the position of the listener
			/// \param vPos - vector position for listener
			void SetListenerPosition( math::Vec3 &vPos );
			/// SetListenerOrientation - sets the orientation of the listener
			/// \param fAngleX - X rotation of the listener 
			/// \param fAngleY - Y rotation of the listener 
			/// \param fAngleZ - Z rotation of the listener 
			/// \param fUpX - X up of the listener 
			/// \param fUpY - Y up of the listener 
			/// \param fUpZ - Z up of the listener 
			void SetListenerOrientation( float fAngleX, float fAngleY, float fAngleZ, float fUpX, float fUpY, float fUpZ );
			/// SetListenerOrientation - sets the orientation of the listener
			/// \param vAngle - vector rotation of the listener 
			/// \param vUp - up vector of the listener 
			void SetListenerOrientation( math::Vec3 &vAngle, math::Vec3 &vUp );
			/// GetListenerPosition - gets the position of the listener
			/// \param fPosX - X Position of the listener 
			/// \param fPosY - Y Position of the listener 
			/// \param fPosZ - Z Position of the listener 
			void GetListenerPosition( float *fPosX, float *fPosY, float *fPosZ );
			/// GetListenerPosition - gets the position of the listener
			/// \param vPos - vector position for listener
			void GetListenerPosition( math::Vec3 &vPos );
			/// GetListenerOrientation - sets the position on the listener
			/// \param fAngleX - X rotation of the listener 
			/// \param fAngleY - Y rotation of the listener 
			/// \param fAngleZ - Z rotation of the listener 
			/// \param fUpX - X up of the listener 
			/// \param fUpY - Y up of the listener 
			/// \param fUpZ - Z up of the listener 
			void GetListenerOrientation( float *fAngleX, float *fAngleY, float *fAngleZ, float *fUpX, float *fUpY, float *fUpZ );

			/// GetSoundState - trys to get a sound state in the list
			/// \param nSoundSource - sound id to return the sounds state
			/// \return unsigned integer - (AL_STATE_???)
			unsigned int GetSoundState( unsigned int nSoundSource );
			/// GetFreeSoundSource - trys to get a free sound source
			/// \return unsigned integer - ( SUCCESS: Source object ID on success or FAIL: INVALID_SOUNDSOURCE )
			unsigned int GetFreeSoundSource( void );

			/// GetNumSources - get the number of sources supported
			/// \return unsigned integer - number of sources
			unsigned int GetNumSources( void )			{ return m_NumberOfSources; }
			/// GetSourcePtrs - get the source id pointer
			/// \return ALuint * - pointer to list
			ALuint *GetSourcePtrs()						{ return m_pSourceIDs; }

			/// GetSoundCount - trys to find the sound in the list via its name
			/// \return integer - ( SUCCESS: number of sounds loaded )
			int GetSoundCount( void )					{	return m_TotalSoundCount; }
			/// GetSoundMemory - gets the current amount of memory loaded by the manager
			/// \return unsigned long - ( SUCCESS: memory count )
			unsigned long GetSoundMemory( void )		{	return m_TotalSoundMemory; }
			/// GetActiveSources - gets the number of sound sources currently playing in the manager
			/// \return integer - ( SUCCESS: active sound sources )
			int GetActiveSources( void )				{	return m_NumActiveSources; }

			static void Initialise( void );
			static void Shutdown( void );

			static SoundManager *GetInstance( void ) 
			{
				DBG_ASSERT( (ms_Instance != 0) );

				return( ms_Instance );
			}

			static bool IsInitialised( void ) 
			{
				return( ms_Instance != 0 );
			}

		private:
			static SoundManager* ms_Instance;

			/// current sound manager state
			ESoundManagerState m_eState;
			/// total number of sounds
			unsigned int m_TotalSoundCount;
			/// total sound memory loaded
			unsigned long m_TotalSoundMemory;
			/// number of sources supported by the driver
			unsigned int m_NumberOfSources;
			/// current number of active sources
			unsigned int m_NumActiveSources;
			/// pointer to list of AL source ids
			ALuint *m_pSourceIDs;
			/// listener position
			float m_ListenerPos[3];
			/// listener orientation
			float m_ListenerOrientation[6];
	};

} // namespace snd

#endif // BASE_SUPPORT_OPENAL
#endif // __SOUNDMANAGER_H__

