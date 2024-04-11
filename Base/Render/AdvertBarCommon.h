
#ifndef __ADVERTBARCOMMON_H__
#define __ADVERTBARCOMMON_H__

#ifdef BASE_SUPPORT_ADBAR

#ifndef __CURL_CURL_H
	#include "curl/curl.h"
#endif // __CURL_CURL_H

// forward declare
class DownloadRequestCallback;

const int MAX_DOWNLOAD_REQUESTS = 20;

struct TImageDefinition
{
	char path[core::MAX_PATH+core::MAX_PATH];
	bool isUrl;
	bool isLocal;
	bool isValidImage;
	GLuint textureId;
	math::Vec2 imageDims;
};

struct TAdvertDefinition
{
	int identifier;
	char title[core::MAX_PATH];
	char url[core::MAX_PATH+core::MAX_PATH];
	char img[core::MAX_PATH];
	float showTime;

	TImageDefinition* imageDef;

	math::Vec2 tabletPos;
	math::Vec2 tabletDims;

	math::Vec2 mobilePos;
	math::Vec2 mobileDims;
};

struct TAdvertData
{
	int totalDefinitions;
	int totalImageFiles;

	TImageDefinition* imageList;
	TAdvertDefinition* adList;
};

enum EDownloadState
{
	DOWNLOADSTATE_IDLE=0,
	DOWNLOADSTATE_READY
};

struct TDownloadRequest
{
	struct TSingleRequest
	{
		char urlAddr[core::MAX_PATH+core::MAX_PATH];
		char localPath[core::MAX_PATH+core::MAX_PATH];
		file::EFileType fileType;
		EDownloadState state;

		DownloadRequestCallback* callback;
	};

	int numTotalRequests;
	TSingleRequest allRequests[MAX_DOWNLOAD_REQUESTS];
};

class DownloadRequestCallback
{
	public:
		DownloadRequestCallback() {} 
		virtual ~DownloadRequestCallback() {}

		virtual void DownloadComplete( CURL* context, CURLcode returnCode, TDownloadRequest::TSingleRequest* request ) = 0;

	private:
};

#endif // BASE_SUPPORT_ADBAR

#endif // __ADVERTBARCOMMON_H__
