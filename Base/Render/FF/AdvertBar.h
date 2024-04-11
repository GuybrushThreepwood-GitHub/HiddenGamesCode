
#ifndef __ADVERTBAR_H__
#define __ADVERTBAR_H__

#ifdef BASE_SUPPORT_ADBAR
#if defined(BASE_SUPPORT_OPENGL) || defined(BASE_SUPPORT_OPENGLES)

#include "Render/AdvertBarCommon.h"

namespace renderer
{
	class AdvertBar : public DownloadRequestCallback
	{
		public:
			AdvertBar();
			virtual ~AdvertBar();

			void Init();
			void Release();

			void Render();
			void Update( float deltaTime );

			const char* GetIncomingFolderPath()		{ return m_IncomingFolder; }
			const char* GetCompleteFolderPath()		{ return m_CompleteFolder; }
			
		private:
			enum EParseReturn
			{
				PARSERETURN_OK=0,
				PARSERETURN_XML_LOAD_ERROR,
				PARSERETURN_IMAGES_TO_DOWNLOAD,
			};

			void CreateDirectories();
			EParseReturn DoesRemoteDataExist();
			EParseReturn ParseFile( const char* xmlFile, TAdvertData* dataStore );
			virtual void DownloadComplete( CURL* context, CURLcode returnCode, TDownloadRequest::TSingleRequest* request );

			static void* AdvertBarDownloadCallback(void *requestBlock);
			static bool AdBarProcessingThread;
		
		private:
			TAdvertData	m_LocalDataStore;
			TAdvertData m_RemoteDataStore;
			TAdvertData m_TempDataStore;

			TAdvertData* m_ActiveAdData;

			char m_AdTimerPath[core::MAX_PATH+core::MAX_PATH];

			const char* m_AdFilterId;
			const char* m_LocalPath;
			const char* m_ExternalPath;

			char m_IncomingFolder[core::MAX_PATH+core::MAX_PATH];
			char m_CompleteFolder[core::MAX_PATH+core::MAX_PATH];

			int m_CurrentAdIndex;
			float m_AdSwapTime;

			math::Vec2 m_UVPoints[4];
			math::Vec3 m_VAPoints[4];

			math::Vec2 m_BarPosition;
			math::Vec2 m_BarDims;

			core::app::Thread m_DownloadThread;

			int m_DownloadRequestIndex;
	};

} // namespace renderer

#endif // (BASE_SUPPORT_OPENGL) || defined(BASE_SUPPORT_OPENGLES)
#endif // BASE_SUPPORT_ADBAR

#endif // __ADVERTBAR_H__

