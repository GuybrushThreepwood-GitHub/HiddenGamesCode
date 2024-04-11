
#include "CoreBase.h"

#ifdef BASE_SUPPORT_OPENAL

#include <zzip/zzip.h>
#include "Sound/SoundCodecs.h"

namespace
{
	static const signed short index_table[16] =
	{
		-1, -1, -1, -1, 2, 4, 6, 8,
		-1, -1, -1, -1, 2, 4, 6, 8
	};
	static const signed short step_table[89] =
	{
		7, 8, 9, 10, 11, 12, 13, 14, 16, 17,
		19, 21, 23, 25, 28, 31, 34, 37, 41, 45,
		50, 55, 60, 66, 73, 80, 88, 97, 107, 118,
		130, 143, 157, 173, 190, 209, 230, 253, 279, 307,
		337, 371, 408, 449, 494, 544, 598, 658, 724, 796,
		876, 963, 1060, 1166, 1282, 1411, 1552, 1707, 1878, 2066,
		2272, 2499, 2749, 3024, 3327, 3660, 4026, 4428, 4871, 5358,
		5894, 6484, 7132, 7845, 8630, 9493, 10442, 11487, 12635, 13899,
		15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794, 32767
	};
}

/*
 * From: http://www.multimedia.cx/simpleaudio.html#tth_sEc4.2
 */
signed short ima2linear (unsigned char nibble, signed short *val, unsigned char *idx)
{
	signed short predictor, diff, step;
	signed char delta, sign;
	signed char index;

	index = *idx;
	step = step_table[index];
	predictor = *val;

	index += index_table[nibble];
	if (index < 0) 
	  index = 0;
	if (index > 88) 
	  index = 88;

	sign = nibble & 0x8;
	delta = nibble & 0x7;

	diff = step >> 3;
	if (delta & 4) 
		diff += step;
	if (delta & 2) 
		diff += (step >> 1);
	if (delta & 1) 
		diff += (step >> 2);

	if (sign) 
		predictor -= diff;
	else 
		predictor += diff;

	*val = predictor;
	*idx = index;

	return predictor;
}

#ifdef BASE_SUPPORT_OGG

/////////////////////////////////////////////////////
/// Function: read_ogg
/// Params: [in]ptr, [in]nmemb, [in]datasource
///
/////////////////////////////////////////////////////
size_t read_ogg(void *ptr, size_t size, size_t nmemb, void *datasource)
{
	file::TFileHandle* fileHandle = reinterpret_cast<file::TFileHandle*>(datasource);
	DBG_ASSERT( fileHandle != 0 );

	return file::FileRead(ptr, size, nmemb, fileHandle );
}

/////////////////////////////////////////////////////
/// Function: seek_ogg
/// Params: [in]datasource, [in]offset, [in]whence
///
/////////////////////////////////////////////////////
int seek_ogg(void *datasource, ogg_int64_t offset, int whence)
{
	file::TFileHandle* fileHandle = reinterpret_cast<file::TFileHandle*>(datasource);
	DBG_ASSERT( fileHandle != 0 );

	long offsetIdx = static_cast<long>(offset);

	int returnVal = -1;

	switch(whence)
	{
		case SEEK_SET:
			returnVal = file::FileSeek( offsetIdx, file::FILESEEK_BEG, fileHandle);
		break;
		case SEEK_CUR:
			returnVal = file::FileSeek( offsetIdx, file::FILESEEK_CUR, fileHandle);
		break;
		case SEEK_END:
			returnVal = file::FileSeek( offsetIdx, file::FILESEEK_END, fileHandle);
		break;
		break;

		default:		//Bad value
			return -1;

	}

	if(returnVal==0)
		return 0;
	else
		return -1;		//Could not do a seek. Device not capable of seeking. (Should never encounter this case)

}

/////////////////////////////////////////////////////
/// Function: close_ogg
/// Params: [in]datasource
///
/////////////////////////////////////////////////////
int close_ogg(void *datasource)
{
	file::TFileHandle* fileHandle = reinterpret_cast<file::TFileHandle*>(datasource);
	DBG_ASSERT( fileHandle != 0 );

	file::FileClose(fileHandle);

	return 0;
}

/////////////////////////////////////////////////////
/// Function: tell_ogg
/// Params: [in]datasource
///
/////////////////////////////////////////////////////
long tell_ogg(void *datasource)
{
	file::TFileHandle* fileHandle = reinterpret_cast<file::TFileHandle*>(datasource);
	DBG_ASSERT( fileHandle != 0 );

	return static_cast<long>( file::FileTell(fileHandle) );
}

/////////////////////////////////////////////////////

const ov_callbacks zzip_ogg_callbacks =
{
	read_ogg,

	seek_ogg,

	close_ogg,

	tell_ogg
};

/////////////////////////////////////////////////////
/// Function: GetOGGCallbacks
/// Params: [in]datasource
///
/////////////////////////////////////////////////////
const ov_callbacks GetOGGCallbacks()
{
	return zzip_ogg_callbacks;
}

#endif // BASE_SUPPORT_OGG

#endif // BASE_SUPPORT_OPENAL
