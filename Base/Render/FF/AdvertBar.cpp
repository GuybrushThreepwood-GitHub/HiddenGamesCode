

/*===================================================================
	File: AdvertBar.cpp
	Library: Render

	(C)Hidden Games
=====================================================================*/

#ifdef BASE_SUPPORT_ADBAR
#if defined(BASE_SUPPORT_OPENGL) || defined(BASE_SUPPORT_OPENGLES)

#include "CoreBase.h"
#include "MathBase.h"
#include "Render/OpenGLCommon.h"
#include "Render/FF/OpenGL/OpenGL.h"
#include "Render/FF/OpenGLES/OpenGLES.h"
#include "Render/TextureShared.h"
#include "Render/Texture.h"
#include "Render/FF/OpenGL/TextureLoadAndUpload.h"
#include "Render/FF/OpenGLES/TextureLoadAndUploadOES.h"

#include "Render/FF/AdvertBar.h"
#include "InputBase.h"

#include "tinyxml2.h"

using renderer::AdvertBar;

namespace
{
	const float ADSWAPTIME_MIN = 10.0f;
	const float ADSWAPTIME_MAX = 30.0f;

	static core::app::Mutex advertbarLockMutex;
	static TDownloadRequest downloadFiles;

	const double REDOWNLOAD_XML_UPDATE_TIME = 259200.0; // secs (3 days)

	const char* DEFAULT_AD_FILTER = "000";
	const char* LOCAL_AD_TIMER_FILE = "ads.dat";
	const char* LOCAL_ADS_PATH = "assets/adverts/hidden_ads.xml";
	const char* EXTERNAL_ADS_URL = "http://www.hiddengames/ads/hidden_extads.xml";
}

bool AdvertBar::AdBarProcessingThread = true;

/////////////////////////////////////////////////////
/// Default constructor
///
///
/////////////////////////////////////////////////////
AdvertBar::AdvertBar()
{
	m_ActiveAdData = 0;
	
	m_DownloadRequestIndex = 0;

	std::memset( &m_LocalDataStore, 0, sizeof(TAdvertData) );
	std::memset( &m_RemoteDataStore, 0, sizeof(TAdvertData) );
	std::memset( &m_TempDataStore, 0, sizeof(TAdvertData) );

	m_AdFilterId = 0;
	m_LocalPath = 0;
	m_ExternalPath = 0;
}
	
/////////////////////////////////////////////////////
/// Default destructor
///
///
/////////////////////////////////////////////////////
AdvertBar::~AdvertBar()
{
	Release();
}

/////////////////////////////////////////////////////
/// Method: Init
/// Params: None
///
/////////////////////////////////////////////////////
void AdvertBar::Init()
{
	AdvertBar::AdBarProcessingThread = true;
	
	std::memset( &downloadFiles, 0, sizeof(TDownloadRequest) );

	CreateDirectories();

	m_AdFilterId = DEFAULT_AD_FILTER;
	m_LocalPath = LOCAL_ADS_PATH;
	m_ExternalPath = EXTERNAL_ADS_URL;

	if( core::app::GetAdvertBarAppFilter() != 0 )
		m_AdFilterId = core::app::GetAdvertBarAppFilter();

	if( core::app::GetAdvertBarLocalXMLPath() != 0 )
		m_LocalPath = core::app::GetAdvertBarLocalXMLPath();

	if( core::app::GetAdvertBarExternalXMLPath() != 0 )
		m_ExternalPath = core::app::GetAdvertBarExternalXMLPath();

	bool doXMLDownload = true;
	bool saveTime = false;

	file::TFileHandle adTimerFile;
	snprintf( m_AdTimerPath, core::MAX_PATH+core::MAX_PATH, "%s/%s", core::app::GetSavePath(), LOCAL_AD_TIMER_FILE );

	time_t storedTime, currentTime;
	time (&currentTime);

	if( file::FileExists( m_AdTimerPath ) )
	{
		if( file::FileOpen( m_AdTimerPath, file::FILETYPE_BINARY_READ, &adTimerFile ) )
		{
			file::FileRead( &storedTime, sizeof(time_t), 1, &adTimerFile );

			file::FileClose( &adTimerFile );
			
			double secondsSinceLastSave = difftime(currentTime, storedTime);

			DBGLOG( "ADBAR: Time since last save %.2f\n", secondsSinceLastSave );
			
			if( secondsSinceLastSave < REDOWNLOAD_XML_UPDATE_TIME )
				doXMLDownload = false;
			else
				saveTime = true;
		}
	}
	else
	{
		// no file, new install/first play
		saveTime = true;

		doXMLDownload = true;
	}

	if( saveTime )
	{
		// write last accumulated time
		if( file::FileOpen( m_AdTimerPath, file::FILETYPE_BINARY_WRITE, &adTimerFile ) )
		{
			file::FileWrite( &currentTime, sizeof(time_t), 1, &adTimerFile );

			file::FileClose( &adTimerFile );
		}
	}

	EParseReturn externalCheck = DoesRemoteDataExist();

	if( (!doXMLDownload) &&
		(externalCheck == PARSERETURN_OK) )
	{
		// got everything, we can use it
		m_ActiveAdData = &m_RemoteDataStore;

		if( m_ActiveAdData &&
			m_ActiveAdData->totalDefinitions <= 0 )
		{
			// delete the downloaded file as it's pretty much empty
			file::DeleteDirectory( m_CompleteFolder );

			m_ActiveAdData = 0;
		}
	}
	else
	{
		if( doXMLDownload ||
			(externalCheck == PARSERETURN_XML_LOAD_ERROR) )
		{
			// clear
			file::DeleteDirectory( m_CompleteFolder );
			// recreate
			file::CreateDirectory( m_CompleteFolder );

			// clean any image loads from the parse above as they are now invalid
			std::memset( &downloadFiles, 0, sizeof(TDownloadRequest) );
			m_DownloadRequestIndex = 0;

			// need to download the xml
			if( m_DownloadRequestIndex < MAX_DOWNLOAD_REQUESTS )
			{
				file::TFile fileStruct;
				file::CreateFileStructure( m_ExternalPath, &fileStruct );
				
				snprintf( downloadFiles.allRequests[m_DownloadRequestIndex].urlAddr, core::MAX_PATH+core::MAX_PATH, "%s", m_ExternalPath );
				snprintf( downloadFiles.allRequests[m_DownloadRequestIndex].localPath, core::MAX_PATH+core::MAX_PATH, "%s/%s", m_IncomingFolder, fileStruct.szFileAndExt );
				downloadFiles.allRequests[m_DownloadRequestIndex].fileType = file::FILETYPE_TEXT_WRITE;
				downloadFiles.allRequests[m_DownloadRequestIndex].callback = this;
				downloadFiles.allRequests[m_DownloadRequestIndex].state = DOWNLOADSTATE_READY;

				m_DownloadRequestIndex++;
				if( m_DownloadRequestIndex > MAX_DOWNLOAD_REQUESTS )
				{
					if( downloadFiles.allRequests[0].state == DOWNLOADSTATE_IDLE )
						m_DownloadRequestIndex = 0;
				}

				downloadFiles.numTotalRequests++;
			}
			else
				DBGLOG( "ADVERTBAR: *ERROR* exceeded download requests\n" );
		}

		// start the download of the external ads
		m_DownloadThread.Initialise( AdvertBarDownloadCallback, &downloadFiles );

		// just load the local
		if( ParseFile( m_LocalPath, &m_LocalDataStore ) == PARSERETURN_OK )
			m_ActiveAdData = &m_LocalDataStore;
	}

	if( m_ActiveAdData &&
		m_ActiveAdData->totalDefinitions > 0 )
	{
		m_CurrentAdIndex = math::RandomNumber( 0, m_ActiveAdData->totalDefinitions-1 );
		m_AdSwapTime = m_ActiveAdData->adList[m_CurrentAdIndex].showTime;//math::RandomNumber( ADSWAPTIME_MIN, ADSWAPTIME_MAX );

		math::Vec2 scale( 1.0f, 1.0f );

		if( core::app::CanAdBarScale() )
			scale = math::Vec2( core::app::GetWidthScale(), core::app::GetHeightScale() );

		core::app::GetAdBarPosition( &m_BarPosition.X, &m_BarPosition.Y );

		if( core::app::IsTablet() )
		{
			// tablet
			int adBarId = core::app::GetAdvertBarSizeTablet();

			if( core::app::IsRetinaDisplay() )
				adBarId = core::app::GetAdvertBarSizeTabletRetina();

			switch( adBarId )
			{
				case core::ADBAR_TABLET_300x250:
				{
					m_BarDims.Width = 300.0f * scale.X;
					m_BarDims.Height = 250.0f * scale.Y;

				}break;
				case core::ADBAR_TABLET_728x90:
				{
					m_BarDims.Width = 728.0f * scale.X;
					m_BarDims.Height = 90.0f * scale.Y;

				}break;
				case core::ADBAR_TABLET_120x600:
				{
					m_BarDims.Width = 120.0f * scale.X;
					m_BarDims.Height = 600.0f * scale.Y;
				}break;
				case core::ADBAR_TABLET_468x60:
				{
					m_BarDims.Width = 468.0f * scale.X;
					m_BarDims.Height = 60.0f * scale.Y;
				}break;

				default:
					DBG_ASSERT(0);
				break;
			}

			if( m_ActiveAdData->adList[m_CurrentAdIndex].imageDef != 0 )
			{
				renderer::SetTextureUVOffset( m_UVPoints, static_cast<int>(m_ActiveAdData->adList[m_CurrentAdIndex].tabletPos.X), static_cast<int>(m_ActiveAdData->adList[m_CurrentAdIndex].tabletPos.Y),
										static_cast<int>(m_ActiveAdData->adList[m_CurrentAdIndex].tabletDims.Width), static_cast<int>(m_ActiveAdData->adList[m_CurrentAdIndex].tabletDims.Height), 
										static_cast<int>(m_ActiveAdData->adList[m_CurrentAdIndex].imageDef->imageDims.Width), static_cast<int>(m_ActiveAdData->adList[m_CurrentAdIndex].imageDef->imageDims.Height), renderer::POINTORIGIN_TOPLEFT );
			}
		}
		else
		{
			// phone
			int adBarId = core::app::GetAdvertBarSizePhone();

			if( core::app::IsRetinaDisplay() )
				adBarId = core::app::GetAdvertBarSizePhoneRetina();

			switch( adBarId )
			{
				case core::ADBAR_PHONE_320x50:
				{
					m_BarDims.Width = 320.0f * scale.X;
					m_BarDims.Height = 50.0f * scale.Y;
				}break;
				case core::ADBAR_PHONE_300x250:
				{
					m_BarDims.Width = 300.0f * scale.X;
					m_BarDims.Height = 250.0f * scale.Y;
				}break;

				default:
					DBG_ASSERT(0);
				break;
			}

			if( m_ActiveAdData->adList[m_CurrentAdIndex].imageDef != 0 )
			{
				renderer::SetTextureUVOffset( m_UVPoints, static_cast<int>(m_ActiveAdData->adList[m_CurrentAdIndex].mobilePos.X), static_cast<int>(m_ActiveAdData->adList[m_CurrentAdIndex].mobilePos.Y),
											static_cast<int>(m_ActiveAdData->adList[m_CurrentAdIndex].mobileDims.Width), static_cast<int>(m_ActiveAdData->adList[m_CurrentAdIndex].mobileDims.Height), 
											static_cast<int>(m_ActiveAdData->adList[m_CurrentAdIndex].imageDef->imageDims.Width), static_cast<int>(m_ActiveAdData->adList[m_CurrentAdIndex].imageDef->imageDims.Height), renderer::POINTORIGIN_TOPLEFT );
			}
		}

		m_VAPoints[0] = math::Vec3( -(m_BarDims.Width*0.5f), -(m_BarDims.Height*0.5f), 0.0f ); 
		m_VAPoints[1] = math::Vec3( (m_BarDims.Width*0.5f), -(m_BarDims.Height*0.5f), 0.0f );
		m_VAPoints[2] = math::Vec3( -(m_BarDims.Width*0.5f), (m_BarDims.Height*0.5f), 0.0f ); 
		m_VAPoints[3] = math::Vec3( (m_BarDims.Width*0.5f), (m_BarDims.Height*0.5f), 0.0f );
	}
}
		
/////////////////////////////////////////////////////
/// Method: Release
/// Params: None
///
/////////////////////////////////////////////////////
void AdvertBar::Release()
{
	std::memset( &downloadFiles, 0, sizeof(TDownloadRequest) );

	m_DownloadThread.Release();

	// local store
	if( m_LocalDataStore.imageList != 0 )
	{
		delete[] m_LocalDataStore.imageList;
		m_LocalDataStore.imageList = 0;
	}

	if( m_LocalDataStore.adList != 0 )
	{
		delete[] m_LocalDataStore.adList;
		m_LocalDataStore.adList = 0;
	}

	m_LocalDataStore.totalDefinitions = 0;
	m_LocalDataStore.totalImageFiles = 0;

	// remote store
	if( m_RemoteDataStore.imageList != 0 )
	{
		delete[] m_RemoteDataStore.imageList;
		m_RemoteDataStore.imageList = 0;
	}

	if( m_RemoteDataStore.adList != 0 )
	{
		delete[] m_RemoteDataStore.adList;
		m_RemoteDataStore.adList = 0;
	}

	m_RemoteDataStore.totalDefinitions = 0;
	m_RemoteDataStore.totalImageFiles = 0;

	// temp store
	if( m_TempDataStore.imageList != 0 )
	{
		delete[] m_TempDataStore.imageList;
		m_TempDataStore.imageList = 0;
	}

	if( m_TempDataStore.adList != 0 )
	{
		delete[] m_TempDataStore.adList;
		m_TempDataStore.adList = 0;
	}

	m_TempDataStore.totalDefinitions = 0;
	m_TempDataStore.totalImageFiles = 0;

	m_ActiveAdData = 0;
	
	AdvertBar::AdBarProcessingThread = false;
}

/////////////////////////////////////////////////////
/// Method: Render
/// Params: None
///
/////////////////////////////////////////////////////
void AdvertBar::Render()
{
	if( m_ActiveAdData == 0 ||
		m_ActiveAdData->totalDefinitions <= 0 )
		return;

	// can draw my own bar just to fill the space
	renderer::OpenGL::GetInstance()->SetNearFarClip( -1.0f, 1.0f );
	renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );

	renderer::OpenGL::GetInstance()->EnableTexturing();
	renderer::OpenGL::GetInstance()->DisableLighting();

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_ALWAYS );
	renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	if( m_ActiveAdData->adList[m_CurrentAdIndex].imageDef &&
		m_ActiveAdData->adList[m_CurrentAdIndex].imageDef->textureId != renderer::INVALID_OBJECT )
		renderer::OpenGL::GetInstance()->BindTexture( m_ActiveAdData->adList[m_CurrentAdIndex].imageDef->textureId );

	renderer::OpenGL::GetInstance()->SetColour4ub( 255, 255, 255, 255 );	
		
	glClientActiveTexture( GL_TEXTURE0 );
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );

	glPushMatrix();
		glTranslatef( m_BarPosition.X, m_BarPosition.Y, 0.0f );	

		glVertexPointer( 3, GL_FLOAT, sizeof(math::Vec3), m_VAPoints );
		glTexCoordPointer( 2, GL_FLOAT, sizeof(math::Vec2), m_UVPoints );

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4 );
	glPopMatrix();

	glClientActiveTexture( GL_TEXTURE0 );
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_LESS );

	GL_CHECK;
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void AdvertBar::Update( float deltaTime )
{
	if( m_ActiveAdData == 0 ||
		m_ActiveAdData->totalDefinitions <= 0 )
		return;

	m_AdSwapTime -= deltaTime;

	if( m_AdSwapTime < 0.0f )
	{
		m_CurrentAdIndex++;

		if( m_CurrentAdIndex >= m_ActiveAdData->totalDefinitions )
			m_CurrentAdIndex = 0;
		
		m_AdSwapTime = m_ActiveAdData->adList[m_CurrentAdIndex].showTime;//math::RandomNumber( ADSWAPTIME_MIN, ADSWAPTIME_MAX );

		if( core::app::IsTablet() )
		{
			renderer::SetTextureUVOffset( m_UVPoints, static_cast<int>(m_ActiveAdData->adList[m_CurrentAdIndex].tabletPos.X), static_cast<int>(m_ActiveAdData->adList[m_CurrentAdIndex].tabletPos.Y), 
										static_cast<int>(m_ActiveAdData->adList[m_CurrentAdIndex].tabletDims.Width), static_cast<int>(m_ActiveAdData->adList[m_CurrentAdIndex].tabletDims.Height), 
										static_cast<int>(m_ActiveAdData->adList[m_CurrentAdIndex].imageDef->imageDims.Width), static_cast<int>(m_ActiveAdData->adList[m_CurrentAdIndex].imageDef->imageDims.Height), renderer::POINTORIGIN_TOPLEFT );
		}
		else
		{
			renderer::SetTextureUVOffset( m_UVPoints, static_cast<int>(m_ActiveAdData->adList[m_CurrentAdIndex].mobilePos.X), static_cast<int>(m_ActiveAdData->adList[m_CurrentAdIndex].mobilePos.Y), 
											static_cast<int>(m_ActiveAdData->adList[m_CurrentAdIndex].mobileDims.Width), static_cast<int>(m_ActiveAdData->adList[m_CurrentAdIndex].mobileDims.Height), 
											static_cast<int>(m_ActiveAdData->adList[m_CurrentAdIndex].imageDef->imageDims.Width), static_cast<int>(m_ActiveAdData->adList[m_CurrentAdIndex].imageDef->imageDims.Height), renderer::POINTORIGIN_TOPLEFT );
		}
	}

	// check for click
	if( input::gInputState.TouchesData[input::FIRST_TOUCH].bRelease )
	{
		float x = static_cast<float>( input::gInputState.TouchesData[input::FIRST_TOUCH].nTouchX );
		float y = static_cast<float>( input::gInputState.TouchesData[input::FIRST_TOUCH].nTouchY );

		float halfWidth = m_BarDims.Width*0.5f;
		float halfHeight = m_BarDims.Height*0.5f;

		if( (x < (m_BarPosition.X+halfWidth) ) &&
			(x > (m_BarPosition.X-halfWidth) ) &&
			(y < (m_BarPosition.Y+halfHeight) ) &&
			(y > (m_BarPosition.Y-halfHeight) ) )
		{
            std::size_t len = std::strlen(m_ActiveAdData->adList[m_CurrentAdIndex].url);
			if( len > 0 )
				core::app::OpenWebLink( m_ActiveAdData->adList[m_CurrentAdIndex].url );
		}
	}
}

/////////////////////////////////////////////////////
/// Method: CreateDirectories
/// Params: None
///
/////////////////////////////////////////////////////
void AdvertBar::CreateDirectories()
{
	const char* savePath = core::app::GetSavePath();

	if( savePath )
	{
		char rootFolder[core::MAX_PATH+core::MAX_PATH];
		snprintf( rootFolder, core::MAX_PATH+core::MAX_PATH, "%s/ads", savePath );
		file::CreateDirectory( rootFolder );

		snprintf( m_IncomingFolder, core::MAX_PATH+core::MAX_PATH, "%s/incoming", rootFolder );
		file::DeleteDirectory( m_IncomingFolder );
		file::CreateDirectory( m_IncomingFolder );

		snprintf( m_CompleteFolder, core::MAX_PATH+core::MAX_PATH, "%s/complete", rootFolder );
		file::CreateDirectory( m_CompleteFolder );
	}
}

/////////////////////////////////////////////////////
/// Method: DoesRemoteDataExist
/// Params: None
///
/////////////////////////////////////////////////////
AdvertBar::EParseReturn AdvertBar::DoesRemoteDataExist()
{
	char workingPath[core::MAX_PATH+core::MAX_PATH];
	std::memset( workingPath, 0, sizeof(char)*(core::MAX_PATH+core::MAX_PATH) );

	file::TFile fileStruct;
	file::CreateFileStructure( m_ExternalPath, &fileStruct );

	// does the xml exist
	snprintf( workingPath, core::MAX_PATH+core::MAX_PATH, "%s/%s", m_CompleteFolder, fileStruct.szFileAndExt );
	if( file::FileExists( workingPath ) )
	{
		EParseReturn ret = ParseFile( workingPath, &m_RemoteDataStore );
		
		if( ret == PARSERETURN_OK )
			return PARSERETURN_OK;
		else if( ret == PARSERETURN_XML_LOAD_ERROR )
		{
			// bad xml, delete the file so it can be redownloaded again
			file::FileDelete( workingPath );
		}
		
		return ret;
	}

	return PARSERETURN_XML_LOAD_ERROR;
}

/////////////////////////////////////////////////////
/// Method: ParseFile
/// Params: [in]xmlFile, [in]dataStore
///
/////////////////////////////////////////////////////
AdvertBar::EParseReturn AdvertBar::ParseFile( const char* xmlFile, TAdvertData* dataStore )
{
	bool dataComplete = true;

	tinyxml2::XMLDocument xmlDoc;

	file::TFileHandle fileHandle;

	if( !file::FileOpen( xmlFile, file::FILETYPE_TEXT_READ, &fileHandle ) )
		return PARSERETURN_XML_LOAD_ERROR;

	char* szBuffer = new char[fileHandle.nFileLength+1];
	std::memset( szBuffer, 0, sizeof(char)*fileHandle.nFileLength );

	file::FileRead( szBuffer, sizeof(char), fileHandle.nFileLength, &fileHandle );
	szBuffer[fileHandle.nFileLength] = '\0';

	file::FileClose( &fileHandle );

	if( xmlDoc.Parse( szBuffer ) != tinyxml2::XML_NO_ERROR )
	{
		xmlDoc.Clear();

		delete[] szBuffer;
		szBuffer = 0;
		return PARSERETURN_XML_LOAD_ERROR;
	}

	delete[] szBuffer;
	szBuffer = 0;

	tinyxml2::XMLElement* child = 0;

	dataStore->totalDefinitions = 0;
	dataStore->totalImageFiles = 0;

	tinyxml2::XMLElement* advertElement = xmlDoc.FirstChildElement( "adverts" );
	if( advertElement )
	{
		tinyxml2::XMLNode* advertNode = advertElement->FirstChild();
		
		for( child = advertElement->FirstChildElement("advert"); child; child = child->NextSiblingElement("advert") )
		{
			tinyxml2::XMLElement *childElmt = child->ToElement();

			if( strcmp( "advert", childElmt->Name() ) == 0 )
			{
				const char* ident = childElmt->Attribute( "id" );

				if( m_AdFilterId &&
					strcmp( m_AdFilterId, ident) == 0 )
					continue;
				else
					dataStore->totalDefinitions++;
			}
		}

		dataStore->adList = new TAdvertDefinition[dataStore->totalDefinitions];
		DBG_ASSERT( dataStore->adList != 0 );

		tinyxml2::XMLElement *imagesElmt = advertElement->FirstChildElement( "images" );

		for( child = imagesElmt->FirstChildElement("imgloc"); child; child = child->NextSiblingElement("imgloc") )
			dataStore->totalImageFiles++;

		dataStore->imageList = new TImageDefinition[dataStore->totalImageFiles];
		DBG_ASSERT( dataStore->imageList != 0 );

		int imageIndex = 0;
		if( imagesElmt != 0 )
		{
			tinyxml2::XMLElement* imgElem = imagesElmt->FirstChildElement("imgloc");
			while( imgElem )
			{
				//DBGLOG( "imgloc  = %s\n", imgElem->GetText() );

				snprintf( dataStore->imageList[imageIndex].path, core::MAX_PATH+core::MAX_PATH, "%s", imgElem->GetText() );

				renderer::Texture texLoader;

				if( strncmp( dataStore->imageList[imageIndex].path, "http", 4 ) == 0 )
				{
					dataStore->imageList[imageIndex].isUrl = true;

					// check if files are locally stored already
					file::TFile fileStruct;
					file::CreateFileStructure( dataStore->imageList[imageIndex].path, &fileStruct );

					char workingPath[core::MAX_PATH+core::MAX_PATH];
					snprintf( workingPath, core::MAX_PATH+core::MAX_PATH, "%s/%s", m_CompleteFolder, fileStruct.szFileAndExt );

					if( file::FileExists( workingPath ) )
					{
						dataStore->imageList[imageIndex].isLocal = true;

						snprintf( dataStore->imageList[imageIndex].path, core::MAX_PATH+core::MAX_PATH, "%s/%s", m_CompleteFolder, fileStruct.szFileAndExt );

						// load it now
						dataStore->imageList[imageIndex].textureId = renderer::TextureLoad(dataStore->imageList[imageIndex].path, texLoader, renderer::GetTextureFormat(dataStore->imageList[imageIndex].path), 0, false, GL_NEAREST, GL_NEAREST );
						
						if( dataStore->imageList[imageIndex].textureId != renderer::INVALID_OBJECT )
						{
							dataStore->imageList[imageIndex].imageDims.Width = static_cast<float>( texLoader.nWidth );
							dataStore->imageList[imageIndex].imageDims.Height = static_cast<float>( texLoader.nHeight );
						}
						else
							dataComplete = false;

						texLoader.Free();
					}
					else
					{
						// add to download list
						dataStore->imageList[imageIndex].isLocal = false;

						if( m_DownloadRequestIndex < MAX_DOWNLOAD_REQUESTS )
						{
							snprintf( downloadFiles.allRequests[m_DownloadRequestIndex].urlAddr, core::MAX_PATH+core::MAX_PATH, "%s", dataStore->imageList[imageIndex].path );
							snprintf( downloadFiles.allRequests[m_DownloadRequestIndex].localPath, core::MAX_PATH+core::MAX_PATH, "%s/%s", m_IncomingFolder, fileStruct.szFileAndExt );
							downloadFiles.allRequests[m_DownloadRequestIndex].fileType = file::FILETYPE_BINARY_WRITE;
							downloadFiles.allRequests[m_DownloadRequestIndex].callback = this;
							downloadFiles.allRequests[m_DownloadRequestIndex].state = DOWNLOADSTATE_READY;

							m_DownloadRequestIndex++;

							if( m_DownloadRequestIndex > MAX_DOWNLOAD_REQUESTS )
							{
								if( downloadFiles.allRequests[0].state == DOWNLOADSTATE_IDLE )
									m_DownloadRequestIndex = 0;
							}
						}
						else
							DBGLOG( "ADVERTBAR: *ERROR* exceeded download requests\n" );

						dataComplete = false;
					}
				}
				else
				{
					dataStore->imageList[imageIndex].isUrl = false;

					// load it now
					dataStore->imageList[imageIndex].textureId = renderer::TextureLoad(dataStore->imageList[imageIndex].path, texLoader, renderer::GetTextureFormat(dataStore->imageList[imageIndex].path), 0, false, GL_NEAREST, GL_NEAREST );
					dataStore->imageList[imageIndex].imageDims.Width = static_cast<float>( texLoader.nWidth );
					dataStore->imageList[imageIndex].imageDims.Height = static_cast<float>( texLoader.nHeight );

					texLoader.Free();
				}

				imageIndex++;

				imgElem = imgElem->NextSiblingElement("imgloc");
			}
		}

		int adIndex = 0;
		advertNode = advertElement->FirstChild();
		while( advertNode != 0 )
		{
			tinyxml2::XMLElement *childElmt = advertNode->ToElement();

			if( strcmp( "advert", childElmt->Name() ) == 0 )
			{
				const char* ident = childElmt->Attribute( "id" );

				if( m_AdFilterId &&
					strcmp( m_AdFilterId, ident) == 0 )
				{
					// next ad node
					advertNode = advertNode->NextSibling();
					continue;
				}
				else
				{
					//dataStore->adList[adIndex].identifier = ident;
					dataStore->adList[adIndex].imageDef = 0;

					tinyxml2::XMLElement* titleElem = childElmt->FirstChildElement("title");
					if( titleElem )
					{
						//DBGLOG( "title  = %s\n", titleElem->GetText() );

						snprintf( dataStore->adList[adIndex].title, core::MAX_PATH, "%s", titleElem->GetText() );
						//m_AllAds[adIndex].title = titleElem->GetText();
					}

					tinyxml2::XMLElement* urlElem = childElmt->FirstChildElement("url");
					if( urlElem )
					{
						//DBGLOG( "url  = %s\n", urlElem->GetText() );
						snprintf( dataStore->adList[adIndex].url, core::MAX_PATH+core::MAX_PATH, "%s", urlElem->GetText() );
						//m_AllAds[adIndex].url = urlElem->GetText();
					}

					tinyxml2::XMLElement* imgElem = childElmt->FirstChildElement("img");
					if( imgElem )
					{
						//DBGLOG( "img  = %s\n", imgElem->GetText() );
						snprintf( dataStore->adList[adIndex].img, core::MAX_PATH, "%s", imgElem->GetText() );

						int i=0;
						for( i=0; i < dataStore->totalImageFiles; ++i )
						{
							file::TFile fullPath;
							file::CreateFileStructure( dataStore->imageList[i].path, &fullPath );
							if( strcmp(dataStore->adList[adIndex].img, fullPath.szFileAndExt) == 0 )
							{
								dataStore->adList[adIndex].imageDef = &dataStore->imageList[i];
								break;
							}
						}
					}

					tinyxml2::XMLElement* timeElem = childElmt->FirstChildElement("showtime");
					if( timeElem )
					{
						//DBGLOG( "showTime  = %s\n", timeElem->GetText() );
						timeElem->QueryFloatText( &dataStore->adList[adIndex].showTime );
					}
					else
						dataStore->adList[adIndex].showTime = math::RandomNumber( ADSWAPTIME_MIN, ADSWAPTIME_MAX );

					tinyxml2::XMLNode* tabletNode = childElmt->FirstChildElement( "tablet" );
					if( tabletNode )
					{
						tinyxml2::XMLElement *tabletElmt = tabletNode->ToElement();
						if( tabletElmt )
						{
							tinyxml2::XMLElement* wElem = tabletElmt->FirstChildElement("tablet_w");
							if( wElem )
							{
								//DBGLOG( "tablet_w  = %s\n", wElem->GetText() );
								wElem->QueryFloatText( &dataStore->adList[adIndex].tabletDims.Width );
							}

							tinyxml2::XMLElement* hElem = tabletElmt->FirstChildElement("tablet_h");
							if( hElem )
							{
								//DBGLOG( "tablet_h  = %s\n", hElem->GetText() );
								hElem->QueryFloatText(&dataStore->adList[adIndex].tabletDims.Height);
							}

							tinyxml2::XMLElement* xElem = tabletElmt->FirstChildElement("tablet_x");
							if( xElem )
							{
								//DBGLOG( "tablet_x  = %s\n", xElem->GetText() );
								xElem->QueryFloatText(&dataStore->adList[adIndex].tabletPos.X);
							}

							tinyxml2::XMLElement* yElem = tabletElmt->FirstChildElement("tablet_y");
							if( yElem )
							{
								//DBGLOG( "tablet_y  = %s\n", yElem->GetText() );
								yElem->QueryFloatText(&dataStore->adList[adIndex].tabletPos.Y);
							}
						}
					}

					tinyxml2::XMLNode* mobileNode = childElmt->FirstChildElement( "mobile" );
					if( mobileNode )
					{
						tinyxml2::XMLElement *mobileElmt = mobileNode->ToElement();
						if( mobileElmt )
						{
							tinyxml2::XMLElement* wElem = mobileElmt->FirstChildElement("mobile_w");
							if( wElem )
							{
								//DBGLOG( "mobile_w  = %s\n", wElem->GetText() );
								wElem->QueryFloatText(&dataStore->adList[adIndex].mobileDims.Width);
							}

							tinyxml2::XMLElement* hElem = mobileElmt->FirstChildElement("mobile_h");
							if( hElem )
							{
								//DBGLOG( "mobile_h  = %s\n", hElem->GetText() );
								hElem->QueryFloatText(&dataStore->adList[adIndex].mobileDims.Height);
							}

							tinyxml2::XMLElement* xElem = mobileElmt->FirstChildElement("mobile_x");
							if( xElem )
							{
								//DBGLOG( "mobile_x  = %s\n", xElem->GetText() );
								xElem->QueryFloatText(&dataStore->adList[adIndex].mobilePos.X);
							}

							tinyxml2::XMLElement* yElem = mobileElmt->FirstChildElement("mobile_y");
							if( yElem )
							{
								//DBGLOG( "mobile_y  = %s\n", yElem->GetText() );
								yElem->QueryFloatText(&dataStore->adList[adIndex].mobilePos.Y);
							}
						}
					}
				}
				adIndex++;
			}

			// next ad node
			advertNode = advertNode->NextSibling();
		}
	}

	xmlDoc.Clear();

	if( !dataComplete )
		return PARSERETURN_IMAGES_TO_DOWNLOAD;

	return PARSERETURN_OK;
}

/////////////////////////////////////////////////////
/// Method: DownloadComplete
/// Params: None
///
/////////////////////////////////////////////////////
void AdvertBar::DownloadComplete( CURL* context, CURLcode returnCode, TDownloadRequest::TSingleRequest* request )
{
	if( returnCode != CURLE_OK )
	{
		// fail
		if( request != 0 )
		{
			DBGLOG( "ADVERTBAR: Download Failed: %s\n", request->urlAddr );
			
			// if it was the XML, that failed (could be offline)
			if( strcmp( request->urlAddr, m_ExternalPath ) == 0 )
			{
				// delete ad.dat to refresh next launch
				file::FileDelete( m_AdTimerPath );
			}
		}
	}
	else
	{
		// success
		if( request != 0 )
		{
			//DBGLOG( "Download Complete: %s\n", request->urlAddr );

			// get some info about the xfer:
			double statDouble = 0;
			long statLong = 0;
			char* statString = 0;

			// known as CURLINFO_RESPONSE_CODE in later curl versions
			if( curl_easy_getinfo( context, CURLINFO_HTTP_CODE, &statLong ) == CURLE_OK )
			{
				//DBGLOG("\tResponse code:  %lu\n", statLong );
			}

			if( curl_easy_getinfo( context, CURLINFO_CONTENT_TYPE, &statString ) == CURLE_OK )
			{
				//DBGLOG("\tContent type:   %s\n", statString );
			}

			if( curl_easy_getinfo( context, CURLINFO_SIZE_DOWNLOAD, &statDouble ) == CURLE_OK )
			{
				//DBGLOG("\tDownload size:  %g bytes\n", statDouble );
			}

			if( curl_easy_getinfo( context, CURLINFO_SPEED_DOWNLOAD, &statDouble ) == CURLE_OK )
			{
				//DBGLOG( "\tDownload speed: %g bytes/sec\n", statDouble );
			}

			// copy the file to the complete folder
			file::TFile fileStruct;
			file::CreateFileStructure( request->urlAddr, &fileStruct );

			char copyDestinationPath[core::MAX_PATH+core::MAX_PATH];
			snprintf( copyDestinationPath, core::MAX_PATH+core::MAX_PATH, "%s/%s", m_CompleteFolder, fileStruct.szFileAndExt );
			file::FileCopy( request->localPath, copyDestinationPath, false );

			// if it was the XML, may as well start the download for any images
			if( strcmp( request->urlAddr, m_ExternalPath ) == 0 )
			{
				// re parse to start the image downloads
				EParseReturn ret = ParseFile( copyDestinationPath, &m_TempDataStore );

				if( ret == PARSERETURN_XML_LOAD_ERROR )
				{
					file::FileDelete( copyDestinationPath );
				}
			}
		}
	}
}

/////////////////////////////////////////////////////
/// Method: AdvertBarDownloadCallback
/// Params: None
///
/////////////////////////////////////////////////////
void* AdvertBar::AdvertBarDownloadCallback(void *requestBlock)
{
	CURL *curlContext = 0;
	CURLcode resCode = CURLE_OK;
	TDownloadRequest* toDownload = reinterpret_cast<TDownloadRequest *>(requestBlock);

	if( toDownload != 0 )
	{
		file::TFileHandle fileOut;
 
		while(AdvertBar::AdBarProcessingThread) // keep it running
		{
			if( toDownload->numTotalRequests > 0 )
			{
				int i=0;

				for( i=0; i < MAX_DOWNLOAD_REQUESTS; ++i )
				{
					advertbarLockMutex.Lock();
					if( toDownload->allRequests[i].state == DOWNLOADSTATE_READY )
					{
						//DBGLOG("AdvertBarDownloadCallback idx = %d\n", i);
 
						curlContext = curl_easy_init();
						if(curlContext != 0)
						{
							file::FileOpen( toDownload->allRequests[i].localPath, toDownload->allRequests[i].fileType, &fileOut );

							// Set the URL and transfer type
							curl_easy_setopt(curlContext, CURLOPT_URL, toDownload->allRequests[i].urlAddr);
							curl_easy_setopt(curlContext, CURLOPT_NOPROGRESS, 1 ) ;
							curl_easy_setopt(curlContext, CURLOPT_FOLLOWLOCATION, 1);
							
							// Write to the file
							curl_easy_setopt(curlContext, CURLOPT_WRITEDATA, &fileOut );
							curl_easy_setopt(curlContext, CURLOPT_WRITEFUNCTION, file::FileWriteCURL );
 
							resCode = curl_easy_perform(curlContext);
 
							file::FileClose(&fileOut);
							
							// check the state again
							if(!AdvertBar::AdBarProcessingThread)
							{
								advertbarLockMutex.UnLock();
								curl_easy_cleanup(curlContext);
								return 0;
							}
 
							// call callback with the original request
							if( toDownload->allRequests[i].callback != 0 )
								toDownload->allRequests[i].callback->DownloadComplete( curlContext, resCode, &toDownload->allRequests[i] );

							toDownload->allRequests[i].state = DOWNLOADSTATE_IDLE;

							curl_easy_cleanup(curlContext);
						}
					}
					advertbarLockMutex.UnLock();
				}
			}

#ifdef BASE_PLATFORM_WINDOWS
			Sleep(1000);
#else
			sleep(1);
#endif
		}
	}

	return 0;
}

#endif // (BASE_SUPPORT_OPENGL) || defined(BASE_SUPPORT_OPENGLES)
#endif // BASE_SUPPORT_ADBAR

