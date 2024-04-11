
/*===================================================================
	File: OpenAL.cpp
	Library: Sound

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"

#ifdef BASE_SUPPORT_OPENAL

#include "DebugBase.h"

#include "Core/CoreFunctions.h"
#include "Sound/OpenAL.h"

using snd::OpenAL;

namespace snd
{
	const int VENDOR_VENDOR_SIZE = 64;
	const int VENDOR_VERSION_SIZE = 64;
	const int VENDOR_RENDERER_SIZE = 64;
	const int VENDOR_EXTENSIONS_SIZE = 4096;
}

OpenAL* OpenAL::ms_Instance = 0;

/////////////////////////////////////////////////////
/// Method: Initialise
/// Params: None
///
/////////////////////////////////////////////////////
void OpenAL::Initialise( void )
{
	DBG_ASSERT( (ms_Instance == 0) );

	DBG_MEMTRY
		ms_Instance = new OpenAL;
	DBG_MEMCATCH
}

/////////////////////////////////////////////////////
/// Method: Shutdown
/// Params: None
///
/////////////////////////////////////////////////////
void OpenAL::Shutdown( void )
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
OpenAL::OpenAL()
{
	m_pContext	= 0;
	m_pDevice	= 0;

	m_NumDevices = 0;
	m_IsEAXSupported = false;

	Init();
}

/////////////////////////////////////////////////////
/// Default destructor
/// 
///
/////////////////////////////////////////////////////
OpenAL::~OpenAL()
{
	// call release just in case
	Release();

	int i = 0;
	for( i = 0; i < MAX_AL_DEVICES; i++ )
		std::memset( &m_szALDevices[i], 0, sizeof(char) ) ;

	m_IsEAXSupported = false;
	m_HasALVersion1_0 = false;
	m_HasALVersion1_1 = false;
	m_NumDevices = 0;
}

/////////////////////////////////////////////////////
/// Method: Init
/// Params: None
///
/////////////////////////////////////////////////////
int OpenAL::Init( void )
{
	DBGLOG( "\n----- OpenAL Setup -----\n" );

	m_pContext	= 0;
	m_pDevice	= 0;

#if defined(_DEBUG) && !defined(BASE_PLATFORM_iOS) && !defined(BASE_PLATFORM_ANDROID) && !defined(BASE_PLATFORM_RASPBERRYPI) && !defined(BASE_PLATFORM_MAC)
	const int DEVICE_NAME_SIZE = 256;

	char szDeviceName[DEVICE_NAME_SIZE];
	const char *szDefaultDevice = 0;
	const char *szDeviceList = 0;
	int nDefaultDeviceNum;

	std::strcpy( szDeviceName, "" );
	if( alcIsExtensionPresent( 0, reinterpret_cast<const ALCchar *>("ALC_ENUMERATION_EXT") ) == AL_TRUE )
	{
		szDefaultDevice = const_cast<char *>( alcGetString( 0, ALC_DEFAULT_DEVICE_SPECIFIER ) );
		szDeviceList = const_cast<char *>( alcGetString( 0, ALC_DEVICE_SPECIFIER ) );

		// initialise device names
		for( m_NumDevices = 0; m_NumDevices < MAX_AL_DEVICES; m_NumDevices++ )
			m_szALDevices[m_NumDevices] = 0;

		for( m_NumDevices = 0; m_NumDevices < MAX_AL_DEVICES; m_NumDevices++ )
		{
			m_szALDevices[m_NumDevices] = const_cast<char *>(szDeviceList);

			if( std::strcmp( m_szALDevices[m_NumDevices], szDefaultDevice ) == 0 )
				nDefaultDeviceNum = m_NumDevices;

			szDeviceList += std::strlen( szDeviceList );

			if( szDeviceList[0] == 0 )
			{
				if( szDeviceList[1] == 0 )
					break;
				else
				{
					szDeviceList += 1;
				}
			}
		}
		
		if( m_szALDevices[m_NumDevices] != 0 )
		{
			m_NumDevices++;

			DBGLOG( "Enumeration of OpenAL devices\n" );
			DBGLOG( "0. 0 Device (Default)\n" );

			int i;
			for( i = 0; i < m_NumDevices; i++ )
			{
				//core::WideString deviceName( m_szALDevices[i] );
				DBGLOG( "%d. %s\n", i+1, m_szALDevices[i] );
			}
		}
			
		DBGLOG( "\n" );
	}

#endif // _DEBUG && !BASE_PLATFORM_iOS && !BASE_PLATFORM_ANDROID && !BASE_PLATFORM_RASPBERRYPI && !BASE_PLATFORM_MAC

	m_pDevice = alcOpenDevice( 0 );

	if(m_pDevice == 0)
	{
		DBGLOG( "OPENAL: *ERROR* failed to Initialise OpenAL\n" );
		return(1);
	}

	// create context(s)
#if defined(BASE_PLATFORM_ANDROID) || defined(BASE_PLATFORM_RASPBERRYPI) || defined(BASE_PLATFORM_MAC)
	const ALint context_attribs[] = { ALC_FREQUENCY, 22050, 0 };
	m_pContext = alcCreateContext( m_pDevice, context_attribs );
#else
	m_pContext = alcCreateContext( m_pDevice, 0 );
#endif // 

	// set active context
	alcMakeContextCurrent( m_pContext );

	char szVendor[VENDOR_VENDOR_SIZE];
	char szVersion[VENDOR_VERSION_SIZE];
	char szRenderer[VENDOR_RENDERER_SIZE];
	char szALext[VENDOR_EXTENSIONS_SIZE];

	//core::WideString vendorName( reinterpret_cast<const char*>( alGetString(AL_VENDOR) ) );
	snprintf( szVendor, VENDOR_VENDOR_SIZE, "Vendor:  %s\n", alGetString(AL_VENDOR) );

	//core::WideString versionName( reinterpret_cast<const char*>( alGetString(AL_VERSION) ) );
	snprintf( szVersion, VENDOR_VERSION_SIZE, "Version: %s\n", alGetString(AL_VERSION) );

	//core::WideString rendererName( reinterpret_cast<const char*>( alGetString(AL_RENDERER) ) );
	snprintf( szRenderer, VENDOR_RENDERER_SIZE, "Renderer: %s\n", alGetString(AL_RENDERER) );

	//core::WideString extensionsName( reinterpret_cast<const char*>( alGetString(AL_EXTENSIONS) ) );
	snprintf( szALext, VENDOR_EXTENSIONS_SIZE, "extensions: %s\n", alGetString(AL_EXTENSIONS) );

	DBGLOG( "\n" );
	DBGLOG( "OpenAL driver info\n" );
	DBGLOG( "--------------------------------------\n" );
	DBGLOG( "%s", szVendor );
	DBGLOG( "--------------------------------------\n" );
	DBGLOG( "%s", szVersion );
	DBGLOG( "--------------------------------------\n" );
	DBGLOG( "%s", szRenderer );
	DBGLOG( "--------------------------------------\n" );
	DBGLOG( "%s", szALext );
	DBGLOG( "--------------------------------------\n" );
	DBGLOG( "\n" );

/*	m_IsEAXSupported = alIsExtensionPresent( (unsigned char*)"EAX2.0" );
	
	if( m_IsEAXSupported == AL_TRUE )
	{
		unsigned char szFunctionName[128];
#ifdef BASE_PLATFORM_WINDOWS
		std::sprintf( (char *)szFunctionName, "EAXSet" );
		eaxSet = (EAXSet)alGetProcAddress(szFunctionName);

		std::sprintf( (char *)szFunctionName, "EAXGet" );
		eaxGet = (EAXGet)alGetProcAddress(szFunctionName);

		if( eaxSet == 0 || eaxGet == 0 )
			DBGLOG( "OPENAL: *WARNING* EAX is supported but failed to get function pointers\n" );
#endif // BASE_PLATFORM_WINDOWS
	}
*/
	m_HasALVersion1_0 = SupportsALVersion( 1, 0 );
	m_HasALVersion1_1 = SupportsALVersion( 1, 1 );

	DBGLOG( "----- OpenAL Setup Complete -----\n" );

	return(0);
}

/////////////////////////////////////////////////////
/// Method: Release
/// Params: None
///
/////////////////////////////////////////////////////
int OpenAL::Release( void )
{
	if( m_pContext &&  m_pDevice )
	{
		DBGLOG( "----- OpenAL Shutdown -----\n" );

#if defined( BASE_PLATFORM_WINDOWS )
		// get active context
		m_pContext = alcGetCurrentContext();
	
		// get device for active context
		m_pDevice = alcGetContextsDevice(m_pContext);
		
//		// disable context
		alcMakeContextCurrent(0);
#endif // WIN32

		// release context(s)
		alcDestroyContext(m_pContext);

		// close device
		alcCloseDevice(m_pDevice);

		m_pContext = 0;
		m_pDevice = 0;

		m_HasALVersion1_0 = false;
		m_HasALVersion1_1 = false;
		m_IsEAXSupported = false;

		DBGLOG( "----- OpenAL Shutdown Complete -----\n" );

		return(0);
	}

	return(1);
}

/////////////////////////////////////////////////////
/// Method: ChangeALDevice
/// Params: [in]nDeviceNum
///
/////////////////////////////////////////////////////
int OpenAL::ChangeALDevice( int nDeviceNum )
{
	// release current device
	if( Release() == 0 )
	{
#ifdef BASE_PLATFORM_WINDOWS
		if( !core::IsEmptyString( m_szALDevices[nDeviceNum] ) )
			m_pDevice = alcOpenDevice(reinterpret_cast<ALCchar*>(m_szALDevices[nDeviceNum]));
#endif // BASE_PLATFORM_WINDOWS	

		// create context(s)
		m_pContext = alcCreateContext(m_pDevice,0);
	
		// set active context
		alcMakeContextCurrent(m_pContext);

		char szVendor[VENDOR_VENDOR_SIZE];
		char szVersion[VENDOR_VERSION_SIZE];
		char szRenderer[VENDOR_RENDERER_SIZE];
		char szALext[VENDOR_EXTENSIONS_SIZE];

		//core::WideString vendorName( reinterpret_cast<const char*>( alGetString(AL_VENDOR) ) );
		snprintf( szVendor, VENDOR_VENDOR_SIZE, "Vendor:  %s\n", alGetString(AL_VENDOR) );

		//core::WideString versionName( reinterpret_cast<const char*>( alGetString(AL_VERSION) ) );
		snprintf( szVersion, VENDOR_VERSION_SIZE, "Version: %s\n", alGetString(AL_VERSION) );

		//core::WideString rendererName( reinterpret_cast<const char*>( alGetString(AL_RENDERER) ) );
		snprintf( szRenderer, VENDOR_RENDERER_SIZE, "Renderer: %s\n", alGetString(AL_RENDERER) );

		//core::WideString extensionsName( reinterpret_cast<const char*>( alGetString(AL_EXTENSIONS) ) );
		snprintf( szALext, VENDOR_EXTENSIONS_SIZE, "extensions: %s\n", alGetString(AL_EXTENSIONS) );

		DBGLOG( "\n" );
		DBGLOG( "OpenAL driver info\n" );
		DBGLOG( "--------------------------------------\n" );
		DBGLOG( "%s", szVendor );
		DBGLOG( "--------------------------------------\n" );
		DBGLOG( "%s", szVersion );
		DBGLOG( "--------------------------------------\n" );
		DBGLOG( "%s", szRenderer );
		DBGLOG( "--------------------------------------\n" );
		DBGLOG( "%s", szALext );
		DBGLOG( "--------------------------------------\n" );
		DBGLOG( "\n" );

/*		m_IsEAXSupported = alIsExtensionPresent( (ALchar*)"EAX2.0" );
	
		if( m_IsEAXSupported == AL_TRUE )
		{
			unsigned char szFunctionName[128];
#ifdef BASE_PLATFORM_WINDOWS
			std::sprintf( (char *)szFunctionName, "EAXSet" );
			eaxSet = (EAXSet)alGetProcAddress(szFunctionName);

			std::sprintf( (char *)szFunctionName, "EAXGet" );
			eaxGet = (EAXGet)alGetProcAddress(szFunctionName);

			if( eaxSet == 0 || eaxGet == 0 )
				DBGLOG( "OPENAL: *WARNING* EAX is supported but failed to get function pointers\n" );
#endif // BASE_PLATFORM_WINDOWS

		}
*/		
		m_HasALVersion1_0 = SupportsALVersion( 1, 0 );
		m_HasALVersion1_1 = SupportsALVersion( 1, 1 );

		return(0);
	}

	DBGLOG( "OPENAL: *ERROR* OpenAL device failed to change to %s\n", m_szALDevices[nDeviceNum] );

	return(1);
}

/////////////////////////////////////////////////////
/// Method: SupportsALVersion
/// Params: [in]nLeastMajor, [in]nLeastMinor
///
/////////////////////////////////////////////////////
bool OpenAL::SupportsALVersion( int nLeastMajor, int nLeastMinor )
{
	const char *szVersion = 0;
	int nMajor, nMinor;

	szVersion = reinterpret_cast<const char *>(alGetString(AL_VERSION));

	if( !core::IsEmptyString( szVersion ) )
	{
		if( sscanf( szVersion, "%d.%d", &nMajor, &nMinor ) == 2 )
		{
			// found a version higher than needed
			if( nMajor > nLeastMajor )
				return(true);
			// 0 need a major version and greater or equal minor
			if( nMajor == nLeastMajor && nMinor >= nLeastMinor )
				return(true);
		}
		else
		{
			DBGLOG( "OPENAL: *ERROR* AL string version is malformed\n" );
			return(false);
		}
	}

	return(false);
}

/////////////////////////////////////////////////////
/// Method: SuspendContext
/// Params: None
///
/////////////////////////////////////////////////////
int OpenAL::SuspendContext( void )
{
	if( m_pContext )
	{
		alcSuspendContext( m_pContext );
		return(0);
	}

	return(1);
}

/////////////////////////////////////////////////////
/// Method: ProcessContext
/// Params: None
///
/////////////////////////////////////////////////////
int OpenAL::ProcessContext( void )
{
	if( m_pContext )
	{
		alcProcessContext( m_pContext );
		return(0);
	}

	return(1);
}

/////////////////////////////////////////////////////
/// Method: GetALDevices
/// Params: [out]nCount, [out]deviceNames
///
/////////////////////////////////////////////////////
int OpenAL::GetALDevices( int *nCount, char szDeviceNames[snd::MAX_AL_DEVICES][snd::DEVICE_NAME_SIZE] )
{
	*nCount = m_NumDevices;

	std::sprintf( szDeviceNames[0], "0 Device (Default)" );

	for( int i = 0; i < m_NumDevices; i++ )
	{
		if( !core::IsEmptyString( m_szALDevices[i] ) )
			std::sprintf( szDeviceNames[i+1], "%s", m_szALDevices[i] );
	}
	return(0);
}

/////////////////////////////////////////////////////
/// Method: CheckALErrors
/// Params: None
///
/////////////////////////////////////////////////////
int OpenAL::CheckALErrors( void )
{

#ifdef _DEBUG
	ALenum error = 0;
	while( ( error = alGetError() ) != AL_NO_ERROR ) 
	{
		char errorString[256];
		switch( error )
		{
			case AL_INVALID_NAME:
				std::sprintf( errorString, "AL_INVALID_NAME" );
				break;
			case AL_INVALID_ENUM:
				std::sprintf( errorString, "AL_ILLEGAL_ENUM or AL_INVALID_ENUM" );
				break;
			case AL_INVALID_VALUE:
				std::sprintf( errorString, "AL_INVALID_VALUE" );
				break;
			case AL_INVALID_OPERATION:
				std::sprintf( errorString, "AL_ILLEGAL_COMMAND or AL_INVALID_OPERATION" );
				break;
			case AL_OUT_OF_MEMORY:
				std::sprintf( errorString, "AL_OUT_OF_MEMORY" );
				break;
		}
		DBGLOG( "OpenAL failed with error %s (%d)\n", errorString, error ); 
		DBG_ASSERT(0);
	} 
#endif // _DEBUG

	return(0);
}

#if !defined(BASE_PLATFORM_iOS) && !defined(BASE_PLATFORM_MAC)

void snd::GetWaveALBufferFormat( WAVEFileInfo* pWaveInfo, PFNALGETENUMVALUE pfnGetEnumValue, unsigned long *pulFormat )
{
	if (pWaveInfo && pfnGetEnumValue && pulFormat)
	{
		*pulFormat = 0;

		if (pWaveInfo->wfType == WF_EX)
		{
			if (pWaveInfo->wfEXT.Format.nChannels == 1)
				*pulFormat = pWaveInfo->wfEXT.Format.wBitsPerSample == 16 ? pfnGetEnumValue("AL_FORMAT_MONO16") : pfnGetEnumValue("AL_FORMAT_MONO8");
			else if (pWaveInfo->wfEXT.Format.nChannels == 2)
				*pulFormat = pWaveInfo->wfEXT.Format.wBitsPerSample == 16 ? pfnGetEnumValue("AL_FORMAT_STEREO16") : pfnGetEnumValue("AL_FORMAT_STEREO8");
			else if ((pWaveInfo->wfEXT.Format.nChannels == 4) && (pWaveInfo->wfEXT.Format.wBitsPerSample == 16))
				*pulFormat = pfnGetEnumValue("AL_FORMAT_QUAD16");
		}
		else if (pWaveInfo->wfType == WF_EXT)
		{
			if ((pWaveInfo->wfEXT.Format.nChannels == 1) && (pWaveInfo->wfEXT.dwChannelMask == snd::SPEAKER_FRONT_CENTER))
				*pulFormat = pWaveInfo->wfEXT.Format.wBitsPerSample == 16 ? pfnGetEnumValue("AL_FORMAT_MONO16") : pfnGetEnumValue("AL_FORMAT_MONO8");
			else if ((pWaveInfo->wfEXT.Format.nChannels == 2) && (pWaveInfo->wfEXT.dwChannelMask == (snd::SPEAKER_FRONT_LEFT|snd::SPEAKER_FRONT_RIGHT)))
				*pulFormat = pWaveInfo->wfEXT.Format.wBitsPerSample == 16 ? pfnGetEnumValue("AL_FORMAT_STEREO16") : pfnGetEnumValue("AL_FORMAT_STEREO8");
			else if ((pWaveInfo->wfEXT.Format.nChannels == 2) && (pWaveInfo->wfEXT.Format.wBitsPerSample == 16) && (pWaveInfo->wfEXT.dwChannelMask == (snd::SPEAKER_BACK_LEFT|snd::SPEAKER_BACK_RIGHT)))
				*pulFormat =  pfnGetEnumValue("AL_FORMAT_REAR16");
			else if ((pWaveInfo->wfEXT.Format.nChannels == 4) && (pWaveInfo->wfEXT.Format.wBitsPerSample == 16) && (pWaveInfo->wfEXT.dwChannelMask == (snd::SPEAKER_FRONT_LEFT|snd::SPEAKER_FRONT_RIGHT|snd::SPEAKER_BACK_LEFT|snd::SPEAKER_BACK_RIGHT)))
				*pulFormat = pfnGetEnumValue("AL_FORMAT_QUAD16");
			else if ((pWaveInfo->wfEXT.Format.nChannels == 6) && (pWaveInfo->wfEXT.Format.wBitsPerSample == 16) && (pWaveInfo->wfEXT.dwChannelMask == (snd::SPEAKER_FRONT_LEFT|snd::SPEAKER_FRONT_RIGHT|snd::SPEAKER_FRONT_CENTER|snd::SPEAKER_LOW_FREQUENCY|snd::SPEAKER_BACK_LEFT|snd::SPEAKER_BACK_RIGHT)))
				*pulFormat = pfnGetEnumValue("AL_FORMAT_51CHN16");
			else if ((pWaveInfo->wfEXT.Format.nChannels == 7) && (pWaveInfo->wfEXT.Format.wBitsPerSample == 16) && (pWaveInfo->wfEXT.dwChannelMask == (snd::SPEAKER_FRONT_LEFT|snd::SPEAKER_FRONT_RIGHT|snd::SPEAKER_FRONT_CENTER|snd::SPEAKER_LOW_FREQUENCY|snd::SPEAKER_BACK_LEFT|snd::SPEAKER_BACK_RIGHT|snd::SPEAKER_BACK_CENTER)))
				*pulFormat = pfnGetEnumValue("AL_FORMAT_61CHN16");
			else if ((pWaveInfo->wfEXT.Format.nChannels == 8) && (pWaveInfo->wfEXT.Format.wBitsPerSample == 16) && (pWaveInfo->wfEXT.dwChannelMask == (snd::SPEAKER_FRONT_LEFT|snd::SPEAKER_FRONT_RIGHT|snd::SPEAKER_FRONT_CENTER|snd::SPEAKER_LOW_FREQUENCY|snd::SPEAKER_BACK_LEFT|snd::SPEAKER_BACK_RIGHT|snd::SPEAKER_SIDE_LEFT|snd::SPEAKER_SIDE_RIGHT)))
				*pulFormat = pfnGetEnumValue("AL_FORMAT_71CHN16");
		}
		else if (pWaveInfo->wfType == WF_IMA4_ADPCM)
		{
			if (pWaveInfo->wfEXT.Format.nChannels == 1)
				*pulFormat = pWaveInfo->wfEXT.Format.wBitsPerSample == 16 ? pfnGetEnumValue("AL_FORMAT_MONO16") : pfnGetEnumValue("AL_FORMAT_MONO8");
			else if (pWaveInfo->wfEXT.Format.nChannels == 2)
				*pulFormat = pWaveInfo->wfEXT.Format.wBitsPerSample == 16 ? pfnGetEnumValue("AL_FORMAT_STEREO16") : pfnGetEnumValue("AL_FORMAT_STEREO8");
			else if ((pWaveInfo->wfEXT.Format.nChannels == 4) && (pWaveInfo->wfEXT.Format.wBitsPerSample == 16))
				*pulFormat = pfnGetEnumValue("AL_FORMAT_QUAD16");			
		}

		if (*pulFormat == 0)
		{
			DBGLOG( "OPENAL: *ERROR* Invalid wave file type\n" );
		}
	}
}
#else
void snd::GetWaveALBufferFormat( WAVEFileInfo* pWaveInfo, LPALGETENUMVALUE pfnGetEnumValue, unsigned long *pulFormat )
{
	if (pWaveInfo && pfnGetEnumValue && pulFormat)
	{
		*pulFormat = 0;
		
		if (pWaveInfo->wfType == WF_EX)
		{
			if (pWaveInfo->wfEXT.Format.nChannels == 1)
				*pulFormat = pWaveInfo->wfEXT.Format.wBitsPerSample == 16 ? pfnGetEnumValue("AL_FORMAT_MONO16") : pfnGetEnumValue("AL_FORMAT_MONO8");
			else if (pWaveInfo->wfEXT.Format.nChannels == 2)
				*pulFormat = pWaveInfo->wfEXT.Format.wBitsPerSample == 16 ? pfnGetEnumValue("AL_FORMAT_STEREO16") : pfnGetEnumValue("AL_FORMAT_STEREO8");
			else if ((pWaveInfo->wfEXT.Format.nChannels == 4) && (pWaveInfo->wfEXT.Format.wBitsPerSample == 16))
				*pulFormat = pfnGetEnumValue("AL_FORMAT_QUAD16");
		}
		else if (pWaveInfo->wfType == WF_EXT)
		{
			if ((pWaveInfo->wfEXT.Format.nChannels == 1) && (pWaveInfo->wfEXT.dwChannelMask == snd::SPEAKER_FRONT_CENTER))
				*pulFormat = pWaveInfo->wfEXT.Format.wBitsPerSample == 16 ? pfnGetEnumValue("AL_FORMAT_MONO16") : pfnGetEnumValue("AL_FORMAT_MONO8");
			else if ((pWaveInfo->wfEXT.Format.nChannels == 2) && (pWaveInfo->wfEXT.dwChannelMask == (snd::SPEAKER_FRONT_LEFT|snd::SPEAKER_FRONT_RIGHT)))
				*pulFormat = pWaveInfo->wfEXT.Format.wBitsPerSample == 16 ? pfnGetEnumValue("AL_FORMAT_STEREO16") : pfnGetEnumValue("AL_FORMAT_STEREO8");
			else if ((pWaveInfo->wfEXT.Format.nChannels == 2) && (pWaveInfo->wfEXT.Format.wBitsPerSample == 16) && (pWaveInfo->wfEXT.dwChannelMask == (snd::SPEAKER_BACK_LEFT|snd::SPEAKER_BACK_RIGHT)))
				*pulFormat =  pfnGetEnumValue("AL_FORMAT_REAR16");
			else if ((pWaveInfo->wfEXT.Format.nChannels == 4) && (pWaveInfo->wfEXT.Format.wBitsPerSample == 16) && (pWaveInfo->wfEXT.dwChannelMask == (snd::SPEAKER_FRONT_LEFT|snd::SPEAKER_FRONT_RIGHT|snd::SPEAKER_BACK_LEFT|snd::SPEAKER_BACK_RIGHT)))
				*pulFormat = pfnGetEnumValue("AL_FORMAT_QUAD16");
			else if ((pWaveInfo->wfEXT.Format.nChannels == 6) && (pWaveInfo->wfEXT.Format.wBitsPerSample == 16) && (pWaveInfo->wfEXT.dwChannelMask == (snd::SPEAKER_FRONT_LEFT|snd::SPEAKER_FRONT_RIGHT|snd::SPEAKER_FRONT_CENTER|snd::SPEAKER_LOW_FREQUENCY|snd::SPEAKER_BACK_LEFT|snd::SPEAKER_BACK_RIGHT)))
				*pulFormat = pfnGetEnumValue("AL_FORMAT_51CHN16");
			else if ((pWaveInfo->wfEXT.Format.nChannels == 7) && (pWaveInfo->wfEXT.Format.wBitsPerSample == 16) && (pWaveInfo->wfEXT.dwChannelMask == (snd::SPEAKER_FRONT_LEFT|snd::SPEAKER_FRONT_RIGHT|snd::SPEAKER_FRONT_CENTER|snd::SPEAKER_LOW_FREQUENCY|snd::SPEAKER_BACK_LEFT|snd::SPEAKER_BACK_RIGHT|snd::SPEAKER_BACK_CENTER)))
				*pulFormat = pfnGetEnumValue("AL_FORMAT_61CHN16");
			else if ((pWaveInfo->wfEXT.Format.nChannels == 8) && (pWaveInfo->wfEXT.Format.wBitsPerSample == 16) && (pWaveInfo->wfEXT.dwChannelMask == (snd::SPEAKER_FRONT_LEFT|snd::SPEAKER_FRONT_RIGHT|snd::SPEAKER_FRONT_CENTER|snd::SPEAKER_LOW_FREQUENCY|snd::SPEAKER_BACK_LEFT|snd::SPEAKER_BACK_RIGHT|snd::SPEAKER_SIDE_LEFT|snd::SPEAKER_SIDE_RIGHT)))
				*pulFormat = pfnGetEnumValue("AL_FORMAT_71CHN16");
		}
		else if (pWaveInfo->wfType == WF_IMA4_ADPCM)
		{
			if (pWaveInfo->wfEXT.Format.nChannels == 1)
				*pulFormat = pWaveInfo->wfEXT.Format.wBitsPerSample == 16 ? pfnGetEnumValue("AL_FORMAT_MONO16") : pfnGetEnumValue("AL_FORMAT_MONO8");
			else if (pWaveInfo->wfEXT.Format.nChannels == 2)
				*pulFormat = pWaveInfo->wfEXT.Format.wBitsPerSample == 16 ? pfnGetEnumValue("AL_FORMAT_STEREO16") : pfnGetEnumValue("AL_FORMAT_STEREO8");
			else if ((pWaveInfo->wfEXT.Format.nChannels == 4) && (pWaveInfo->wfEXT.Format.wBitsPerSample == 16))
				*pulFormat = pfnGetEnumValue("AL_FORMAT_QUAD16");			
		}
		
		if (*pulFormat == 0)
		{
			DBGLOG( "OPENAL: *ERROR* Invalid wave file type\n" );
		}
	}
}

#endif 

#endif // BASE_SUPPORT_OPENAL

