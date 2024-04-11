
#ifndef __CORECONSTS_H__
#define __CORECONSTS_H__

#ifdef MAX_PATH 
	#undef MAX_PATH
#endif // MAX_PATH

namespace core
{
	const int MAX_PATH	= 256;
	const int MAX_DRIVE = 3;
	const int MAX_DIR	= 256;
	const int MAX_FNAME = 256;
	const int MAX_EXT	= 256;

	const float FPS60 = 1.0f/60.0f;
	const float FPS30 = 1.0f/30.0f;

	const int LANGUAGE_ENGLISH	= 0;
	const int LANGUAGE_FRENCH	= 1;
	const int LANGUAGE_ITALIAN	= 2;
	const int LANGUAGE_GERMAN	= 3;
	const int LANGUAGE_SPANISH	= 4;

	// ad data
	const int ADBAR_PHONE_320x50	= 0;
	const int ADBAR_PHONE_300x250	= 1;

	const int ADBAR_TABLET_300x250	= 0;
	const int ADBAR_TABLET_728x90	= 1;
	const int ADBAR_TABLET_120x600	= 2;
	const int ADBAR_TABLET_468x60	= 3;

} // namespace core

#endif // __CORECONSTS_H__

