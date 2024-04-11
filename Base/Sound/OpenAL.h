
#ifndef __OPENAL_H__
#define __OPENAL_H__

#ifdef BASE_SUPPORT_OPENAL

#ifndef __PLATFORMSOUNDBASE_H__
	#include "Sound/PlatformSoundBase.h"
#endif //__PLATFORMSOUNDBASE_H__

#ifndef __ASSERTION_H__
	#include "Debug/Assertion.h"
#endif // __ASSERTION_H__

#ifndef __SOUNDCOMMON_H__
	#include "Sound/SoundCommon.h"
#endif // __SOUNDCOMMON_H__

namespace snd
{
	const int MAX_AL_DEVICES					= 12;
	const int DEVICE_NAME_SIZE					= 64;

	#ifdef _DEBUG
		#define CHECK_OPENAL_ERROR( alCmd ) \
				alCmd; \
				{ \
					ALenum error; \
					while( ( error = alGetError() ) != AL_NO_ERROR ) \
					{ \
						char errorString[256]; \
						switch( error ) \
						{ \
							case AL_INVALID_NAME: \
								std::sprintf( errorString, "AL_INVALID_NAME" ); \
							break; \
							case AL_INVALID_ENUM: \
								std::sprintf( errorString, "AL_ILLEGAL_ENUM or AL_INVALID_ENUM" ); \
							break; \
							case AL_INVALID_VALUE: \
								std::sprintf( errorString, "AL_INVALID_VALUE" ); \
							break; \
							case AL_INVALID_OPERATION: \
								std::sprintf( errorString, "AL_ILLEGAL_COMMAND or AL_INVALID_OPERATION" ); \
							break; \
							case AL_OUT_OF_MEMORY: \
								std::sprintf( errorString, "AL_OUT_OF_MEMORY" ); \
							break; \
						} \
						DBGLOG( "[%s:%d] '%s' failed with error %s (%d)\n", __FILE__, __LINE__, #alCmd,  errorString, error ); \
					} \
				}
	#else
		#define CHECK_OPENAL_ERROR( alCmd ) \
				alCmd;
	#endif // _DEBUG

	class OpenAL
	{
		public:
			/// default constructor
			OpenAL();
			/// default destructor
			~OpenAL();

		private:
			/// Init
			/// \return integer - ( SUCCESS: OK or FAIL: FAIL )
			int Init( void );
			/// Release - Clears any memory
			/// \return integer - ( SUCCESS: OK or FAIL: FAIL )
			int Release( void );

		public:
			/// ChangeALDevice - Changes the AL device used for sound output
			/// \param nDeviceNum - Enumerated device number to use
			/// \return integer - ( SUCCESS: OK or FAIL: FAIL )
			int ChangeALDevice( int nDeviceNum );

			/// SupportsALVersion - Finds out what versions of OpenAL are supported
			/// \param nLeastMajor - major version number to check
			/// \param nLeastMinor - minor version number to check
			/// \return boolean ( SUCCESS:true or FAIL:false )
			bool SupportsALVersion( int nLeastMajor, int nLeastMinor );
			/// SuspendContext - Suspends the current AL context from output
			/// \returns integer - ( SUCCESS: OK or FAIL: FAIL )
			int SuspendContext( void );
			/// ResumeContext - Resumes the current AL context
			/// \return integer - ( SUCCESS: OK or FAIL: FAIL )
			int ProcessContext( void );

			/// GetALDevices - Gets a list of the current AL output devices available
			/// \param nCount - returned number of devices
			/// \param szDeviceNames - list to be filled with device names
			/// \return integer - ( SUCCESS: OK or FAIL: FAIL )
			int GetALDevices( int *nCount, char szDeviceNames[MAX_AL_DEVICES][DEVICE_NAME_SIZE] );

			/// CheckALErrors - Checks the AL state for errors and asserts if there are any
			/// \return integer - ( SUCCESS: OK or FAIL: FAIL )
			int CheckALErrors( void );

			static void Initialise( void );
			static void Shutdown( void );

			static OpenAL *GetInstance( void ) 
			{
				DBG_ASSERT( (ms_Instance != 0) );

				return( ms_Instance );
			}
		private:
			static OpenAL* ms_Instance;

			/// main AL context pointer
			ALCcontext *m_pContext;
			/// AL device pointer
			ALCdevice *m_pDevice;
			/// number of devices
			int		m_NumDevices;
			/// device name list
			char	*m_szALDevices[MAX_AL_DEVICES];
			/// AL v1.0 support flag
			int		m_HasALVersion1_0;
			/// AL v1.1 support flag
			int		m_HasALVersion1_1;
			/// EAX support flag
			int		m_IsEAXSupported;
	};
#ifndef BASE_PLATFORM_iOS
	typedef int (*PFNALGETENUMVALUE)( const char *szEnumName );
	void GetWaveALBufferFormat( WAVEFileInfo* pWaveInfo, PFNALGETENUMVALUE pfnGetEnumValue, unsigned long *pulFormat );
#else
	typedef int (*LPALGETENUMVALUE)( const char *szEnumName );
	void GetWaveALBufferFormat( WAVEFileInfo* pWaveInfo, LPALGETENUMVALUE pfnGetEnumValue, unsigned long *pulFormat );
#endif
} // namespace snd

#endif // BASE_SUPPORT_OPENAL

#endif // __OPENAL_H__

