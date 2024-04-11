
/*===================================================================
	File: FileIO.cpp
	Library: File

	(C)Hidden Games
=====================================================================*/

//FIXME: check size and count params for 0, then give a warning

#include "Core/CoreDefines.h"

#include <iostream>
#include <fstream>
#include <iterator>
#include <istream>
#include <sys/stat.h>

#ifdef BASE_PLATFORM_WINDOWS
	#include <direct.h>
#endif // BASE_PLATFORM_WINDOWS

#include "CoreBase.h"
#include "DebugBase.h"

#include "Math/Vectors.h"
#include "Math/Quaternion.h"

#include "Render/OpenGLCommon.h"
#include "Render/TextureShared.h"
#include "Model/ModelCommon.h"

#include "Core/FileIO.h"

// some trackers to make sure files are correctly opened/closed
static unsigned int nFilesOpened = 0;
static unsigned int nFilesClosed = 0;

namespace
{
	// zzip file opening
	static zzip_strings_t zipFileExt[] = { ".zip", ".ZIP", "", 0 };
	static char szConvertedFilename[core::MAX_PATH+core::MAX_PATH];
	static int o_flags = O_RDONLY;	// rb
	static int o_modes = 0664;
}

/////////////////////////////////////////////////////
/// Function: GetZipExtensions
/// Params: None
///
/////////////////////////////////////////////////////
zzip_strings_t* file::GetZipExtensions()
{
	return zipFileExt;
}

/////////////////////////////////////////////////////
/// Function: ReportFileCounts
/// Params: None
///
/////////////////////////////////////////////////////
void file::ReportFileCounts()
{
	DBGLOG( "FILEIO: Files opened %d\n", nFilesOpened );
	DBGLOG( "FILEIO: Files closed %d\n", nFilesClosed );

	if( nFilesClosed != nFilesOpened )
	{
		DBGLOG( "FILEIO: *WARNING* Not all file handles were closed\n" );
	}
}

/////////////////////////////////////////////////////
/// Function: CreateFileStructure
/// Params: [in]szFilename, [in/out]pFileStruct
///
/////////////////////////////////////////////////////
void file::CreateFileStructure( const char *szFilename, TFile *pFileStruct )
{
	if( !core::IsEmptyString( szFilename ) && pFileStruct )
	{
		std::memset( pFileStruct, 0, sizeof(TFile) );

		core::SplitPath( szFilename, pFileStruct->szDrive, pFileStruct->szPath, pFileStruct->szFile, pFileStruct->szFileExt );

		if( !core::IsEmptyString( szFilename ) )
			snprintf( pFileStruct->szFilename, core::MAX_PATH+core::MAX_PATH, "%s", szFilename );
		if( !core::IsEmptyString( pFileStruct->szFile ) && !core::IsEmptyString( pFileStruct->szFileExt ) )
			snprintf( pFileStruct->szFileAndExt, core::MAX_PATH, "%s%s", pFileStruct->szFile, pFileStruct->szFileExt );
	}
	else
	{
		DBGLOG( "FILEIO: Could not create file structure for '%s'\n", szFilename );
	}

/*	if( !core::IsEmptyString( szFilename ) && pFileStruct )
	{
		std::memset( pFileStruct, 0, sizeof(TFile) );

		wchar_t* pWideString = 0;

		// convert to wide
		std::size_t len = std::strlen( szFilename );
		if( len > 0 )
		{
			pWideString = new wchar_t[(len+1)];

			std::size_t size = std::mbstowcs( pWideString, szFilename, len+1 );

			if( size != len )
			{
				DBGLOG( L"FILEIO: Incorrect converted size of wide character in CreateFileStructure\n" );
			}
		
			CreateFileStructure( pWideString, pFileStruct );

			if( pWideString )
			{
				delete[] pWideString;
				pWideString = 0;
			}
		}
	}
	else
	{
		DBGLOG( L"FILEIO: Could not create file structure\n" );
	}
*/
}

/////////////////////////////////////////////////////
/// Function: FileIsOpen
/// Params: [in]fileHandle
///
/////////////////////////////////////////////////////
bool file::FileIsOpen( TFileHandle* fileHandle )
{
	if( fileHandle->nState == FILESTATE_OPEN )
		return(true);

	return(false);
}

/////////////////////////////////////////////////////
/// Function: FileOpen
/// Params: [in]szFilename, [in]nType, [in]fileHandle
///
/////////////////////////////////////////////////////
bool file::FileOpen( const char *szFilename, EFileType nType, TFileHandle* fileHandle )
{
	if( core::IsEmptyString( szFilename ) )
		return(false);

	if( fileHandle->nState == FILESTATE_OPEN )
	{
		DBGLOG( "FILEIO: *WARNING* trying to open an already open file\n" );
		return(true);
	}

	std::streamoff nFileStart = 0;
	std::streamoff nFileEnd = 0;

	if( nType == FILETYPE_BINARY_READ )
	{
		fileHandle->zipped = false;

		if( core::app::GetLoadFilesFromZip() )
		{
			// normal file doesn't exist
			fileHandle->zfp = zzip_fopen (szFilename, "rb");
			if( fileHandle->zfp == 0 )
			{
				snprintf( szConvertedFilename, core::MAX_PATH+core::MAX_PATH, "%s/%s", core::app::GetRootZipFile(), szFilename );
			
				// try and load from the zip
				fileHandle->zfp = zzip_open_shared_io( 0, szConvertedFilename, o_flags, o_modes, file::GetZipExtensions(), 0 );

				if( !fileHandle->zfp )
					return( false );	

				fileHandle->zipped = true;
			}
			else
				fileHandle->zipped = true;
		
			zzip_seek( fileHandle->zfp, 0, SEEK_END );
			fileHandle->nFileLength = static_cast<std::size_t>( zzip_tell( fileHandle->zfp ) );
			zzip_seek( fileHandle->zfp, 0, SEEK_SET );
		}
		else
		{
			fileHandle->fp = fopen( szFilename, "rb" );

			if( !fileHandle->fp )
				return(false);

			fseek( fileHandle->fp, 0, SEEK_END );
			fileHandle->nFileLength = ftell( fileHandle->fp );
			fseek( fileHandle->fp, 0, SEEK_SET );
		}
	}
	else if( nType == FILETYPE_BINARY_WRITE )
	{
		fileHandle->fp = fopen( szFilename, "wb" );

		if( !fileHandle->fp )
			return(false);

		fileHandle->zipped = false;
		fseek( fileHandle->fp, 0, SEEK_END );
		fileHandle->nFileLength = ftell( fileHandle->fp );
		fseek( fileHandle->fp, 0, SEEK_SET );
	}
	else if( nType == FILETYPE_BINARY_STREAM_READ )
	{
		fileHandle->input.open( szFilename, std::ios::in | std::ios::binary );

		if( !fileHandle->input.is_open() || fileHandle->input.fail() )
			return(false);

		nFileStart = fileHandle->input.tellg();
		fileHandle->input.seekg( 0, std::ios::end );
		nFileEnd = fileHandle->input.tellg();
		fileHandle->input.seekg( 0, std::ios::beg );

		fileHandle->zipped = false;
		fileHandle->nFileLength = static_cast<std::size_t>(nFileEnd - nFileStart);
	}
	else if( nType == FILETYPE_BINARY_STREAM_WRITE )
	{
		fileHandle->output.open( szFilename, std::ios::out | std::ios::binary );

		if( !fileHandle->output.is_open() || fileHandle->output.fail() )
			return(false);

		nFileStart = fileHandle->output.tellp();
		fileHandle->output.seekp( 0, std::ios::end );
		nFileEnd = fileHandle->output.tellp();
		fileHandle->output.seekp( 0, std::ios::beg );

		fileHandle->zipped = false;
		fileHandle->nFileLength = static_cast<std::size_t>(nFileEnd - nFileStart);
	}
	else if( nType == FILETYPE_TEXT_READ )
	{
		if( core::app::GetLoadFilesFromZip() )
		{
			// normal file doesn't exist
			fileHandle->zfp = zzip_fopen (szFilename, "rt");
			if( fileHandle->zfp == 0 )
			{
				snprintf( szConvertedFilename, core::MAX_PATH+core::MAX_PATH, "%s/%s", core::app::GetRootZipFile(), szFilename );
			
				// try and load from the zip
				fileHandle->zfp = zzip_open_shared_io( 0, szConvertedFilename, o_flags, o_modes, file::GetZipExtensions(), 0 );

				if( !fileHandle->zfp )
					return( false );	

				fileHandle->zipped = true;
			}
			else
				fileHandle->zipped = true;
		
			zzip_seek( fileHandle->zfp, 0, SEEK_END );
			fileHandle->nFileLength = static_cast<std::size_t>( zzip_tell( fileHandle->zfp ) );
			zzip_seek( fileHandle->zfp, 0, SEEK_SET );
		}
		else
		{
			fileHandle->fp = fopen( szFilename, "rt" );

			if( !fileHandle->fp )
				return(false);

			fileHandle->zipped = false;
			fseek( fileHandle->fp, 0, SEEK_END );
			fileHandle->nFileLength = ftell( fileHandle->fp );
			fseek( fileHandle->fp, 0, SEEK_SET );
		}
	}
	else if( nType == FILETYPE_TEXT_WRITE )
	{
		fileHandle->fp = fopen( szFilename, "wt" );

		if( !fileHandle->fp )
			return(false);

		fileHandle->zipped = false;
		fseek( fileHandle->fp, 0, SEEK_END );
		fileHandle->nFileLength = ftell( fileHandle->fp );
		fseek( fileHandle->fp, 0, SEEK_SET );
	}
	else
	{
		// unknown type
		DBGLOG( "FILEIO: *ERROR* unrecognised file type in FileOpen\n" );
		return(false);
	}

	fileHandle->nState = FILESTATE_OPEN;
	fileHandle->nFileType = nType;

	// track it
	nFilesOpened++;

	return(true);
}

/////////////////////////////////////////////////////
/// Function: IsEOF
/// Params: [in]fileHandle
///
/////////////////////////////////////////////////////
bool file::IsEOF( TFileHandle* fileHandle )
{
	if( fileHandle->nState == FILESTATE_CLOSED )
		return(true);

	if( fileHandle->nFileType == FILETYPE_BINARY_READ ||
		fileHandle->nFileType == FILETYPE_BINARY_WRITE )
	{
		if( fileHandle->zipped )
		{
			std::size_t curPos = static_cast<std::size_t>( zzip_tell( fileHandle->zfp ) );			

			if( curPos < fileHandle->nFileLength  )
				return( false );
			else
				return( true );
		}
		else
		{
			std::size_t curPos = static_cast<std::size_t>( ftell( fileHandle->fp ) );

			if( curPos < fileHandle->nFileLength  )
				return( false );
			else
				return( true );
		}
	}
	else if( fileHandle->nFileType == FILETYPE_BINARY_STREAM_READ )
	{
		if( !fileHandle->input.is_open() )
			return(true);	

		return( fileHandle->input.eof() );
	}
	else if( fileHandle->nFileType == FILETYPE_BINARY_STREAM_WRITE )
	{
		if( !fileHandle->output.is_open() )
			return(true);	

		return( fileHandle->output.eof() );
	}

	return(true);
}

/////////////////////////////////////////////////////
/// Function: FileSeek
/// Params: [in]nOffset, [in]eOrigin, [in]fileHandle
///
/////////////////////////////////////////////////////
int file::FileSeek( long nOffset, EFileSeekOrigin eOrigin, TFileHandle* fileHandle )
{
	if( fileHandle->nState == FILESTATE_CLOSED )
		return(1);

	//std::ios::seekdir dir = std::ios::beg;

	if( fileHandle->nFileType == FILETYPE_BINARY_READ )
	{
		if( fileHandle->zipped )
			zzip_seek( fileHandle->zfp, nOffset, static_cast<int>(eOrigin) );
		else
			fseek( fileHandle->fp, nOffset, static_cast<int>(eOrigin) );
	}
	else if( fileHandle->nFileType == FILETYPE_BINARY_WRITE )
	{
		fseek( fileHandle->fp, nOffset, static_cast<int>(eOrigin) );
	}
	else if( fileHandle->nFileType == FILETYPE_BINARY_STREAM_READ )
	{
		if( !fileHandle->input.is_open() )
			return(1);	

		fileHandle->input.seekg( static_cast<std::streamoff>(nOffset), static_cast<std::ios::seekdir>(eOrigin) );
	}
	else if( fileHandle->nFileType == FILETYPE_BINARY_STREAM_WRITE )
	{
		if( !fileHandle->output.is_open() )
			return(1);	

		fileHandle->output.seekp( static_cast<std::streamoff>(nOffset), static_cast<std::ios::seekdir>(eOrigin) );
	}

	return(0);
}

/////////////////////////////////////////////////////
/// Function: FileTell
/// Params: [in]fileHandle
///
/////////////////////////////////////////////////////
std::size_t file::FileTell( TFileHandle* fileHandle )
{
	std::size_t result = 0;

	if( fileHandle->nState == FILESTATE_CLOSED )
		return(0);

	if( fileHandle->nFileType == FILETYPE_BINARY_READ )
	{
		if( fileHandle->zipped )
			result = static_cast<std::size_t>(zzip_tell( fileHandle->zfp ));
		else
			result = static_cast<std::size_t>(ftell( fileHandle->fp ));
	}
	else if( fileHandle->nFileType == FILETYPE_BINARY_WRITE )
	{
		result = static_cast<std::size_t>(ftell( fileHandle->fp ));
	}
	else if( fileHandle->nFileType == FILETYPE_BINARY_STREAM_READ )
	{
		if( !fileHandle->input.is_open() )
			return(1);	

		result = static_cast<std::size_t>(fileHandle->input.tellg());
	}
	else if( fileHandle->nFileType == FILETYPE_BINARY_STREAM_WRITE )
	{
		if( !fileHandle->output.is_open() )
			return(1);	

		result = static_cast<std::size_t>(fileHandle->output.tellp());
	}

	return(result);
}

/////////////////////////////////////////////////////
/// Function: FilePosition
/// Params: [in]fileHandle
///
/////////////////////////////////////////////////////
std::size_t file::FilePosition( TFileHandle* fileHandle )
{
	std::size_t result = 0;
	fpos_t pos;

#ifdef BASE_PLATFORM_RASPBERRYPI
	#define POS_STRUCT(x) x.__pos
#else
	#define POS_STRUCT(x) x
#endif

	if( fileHandle->nState == FILESTATE_CLOSED )
		return(0);

	if( fileHandle->nFileType == FILETYPE_BINARY_READ )
	{
		if( fileHandle->zipped )
		{
			result = static_cast<std::size_t>(zzip_tell( fileHandle->zfp ));
		}
		else
		{
			fgetpos( fileHandle->fp, &pos );
			result = static_cast<std::size_t>(POS_STRUCT(pos));
		}
	}
	else if( fileHandle->nFileType == FILETYPE_BINARY_WRITE )
	{
		fgetpos( fileHandle->fp, &pos );
		result = static_cast<std::size_t>(POS_STRUCT(pos));
	}
	else if( fileHandle->nFileType == FILETYPE_BINARY_STREAM_READ )
	{
		if( !fileHandle->input.is_open() )
			return(1);	

		result = static_cast<std::size_t>(fileHandle->input.tellg());
	}
	else if( fileHandle->nFileType == FILETYPE_BINARY_STREAM_WRITE )
	{
		if( !fileHandle->output.is_open() )
			return(1);	

		result = static_cast<std::size_t>(fileHandle->output.tellp());
	}

	return(result);
}

/////////////////////////////////////////////////////
/// Function: FileSize
/// Params: [in]fileHandle
///
/////////////////////////////////////////////////////
std::size_t file::FileSize( TFileHandle* fileHandle )
{
	return( fileHandle->nFileLength );
}

/////////////////////////////////////////////////////
/// Function: FileSize
/// Params: [in]szFilename
///
/////////////////////////////////////////////////////
std::size_t file::FileSize( const char* szFilename )
{
	std::size_t fileSize = 0;
	struct stat st;
	if( !stat(szFilename, &st) )
		fileSize = st.st_size;

	return fileSize;
}

/////////////////////////////////////////////////////
/// Function: FileClose
/// Params: [in]fileHandle
///
/////////////////////////////////////////////////////
bool file::FileClose( TFileHandle* fileHandle )
{
	if( fileHandle->nFileType == FILETYPE_BINARY_READ && fileHandle->nState == FILESTATE_OPEN )
	{
		if( fileHandle->zipped )
		{
			if( fileHandle->zfp )
			{
				zzip_close( fileHandle->zfp );
				fileHandle->zfp = 0;
			}
		}
		else
		{
			if( fileHandle->fp )
			{
				fclose( fileHandle->fp );
				fileHandle->fp = 0;
			}
		}

		fileHandle->nState = FILESTATE_CLOSED;
		nFilesClosed++;
		
		return(true);
		
	}
	else if( fileHandle->nFileType == FILETYPE_BINARY_WRITE && fileHandle->nState == FILESTATE_OPEN )
	{
		if( fileHandle->fp )
		{
			fclose( fileHandle->fp );
			fileHandle->fp = 0;
			fileHandle->nState = FILESTATE_CLOSED;

			nFilesClosed++;
			return(true);
		}
	}
	else if( fileHandle->nFileType == FILETYPE_BINARY_STREAM_READ && fileHandle->nState == FILESTATE_OPEN )
	{
		if( fileHandle->input.is_open() )
		{
			fileHandle->input.close();
			fileHandle->nState = FILESTATE_CLOSED;

			nFilesClosed++;
			return(true);
		}
	}
	else if( fileHandle->nFileType == FILETYPE_BINARY_STREAM_WRITE && fileHandle->nState == FILESTATE_OPEN )
	{
		if( fileHandle->output.is_open() )
		{
			fileHandle->output.close();
			fileHandle->nState = FILESTATE_CLOSED;

			nFilesClosed++;
			return(true);
		}
	}
	else if( fileHandle->nFileType == FILETYPE_TEXT_READ && fileHandle->nState == FILESTATE_OPEN )
	{
		if( fileHandle->fp )
		{
			fclose( fileHandle->fp );
			fileHandle->fp = 0;
			fileHandle->nState = FILESTATE_CLOSED;

			nFilesClosed++;
			return(true);
		}
	}
	else if( fileHandle->nFileType == FILETYPE_TEXT_WRITE && fileHandle->nState == FILESTATE_OPEN )
	{
		if( fileHandle->fp )
		{
			fclose( fileHandle->fp );
			fileHandle->fp = 0;
			fileHandle->nState = FILESTATE_CLOSED;

			nFilesClosed++;
			return(true);
		}
	}
	return(false);
}

/////////////////////////////////////////////////////
/// Function: FileExists
/// Params: [in]szFilename
///
/////////////////////////////////////////////////////
bool file::FileExists( const char *szFilename )
{
	if( core::IsEmptyString(szFilename) )
		return(false);

	file::TFileHandle fileHandle;

	struct stat   buffer;   
	return(stat (szFilename, &buffer) == 0);

	/*if( FileOpen( szFilename, file::FILETYPE_BINARY_READ, &fileHandle ) )
	{
		file::FileClose(&fileHandle);

		// does exist
		return(true);
	}

	// doesn't exist
	return(false);*/
}

/////////////////////////////////////////////////////
/// Function: FileCopy
/// Params: [in]szInFilename, [in]szOutFilename, [in]bFailIfExists
///
/////////////////////////////////////////////////////
int file::FileCopy( const char *szInFilename, const char *szOutFilename, bool bFailIfExists )
{
/*	FILE *pInFile = 0;
	FILE *pOutFile = 0;

	long inLen = 0;

	char* c = 0;
	//int c = 0;

	if( std::strcmp( szInFilename, szOutFilename ) == 0 )
	{
		DBGLOG( "FILEIO: *WARNING* FileCopy source and destination names were the same\n" );
		return(1);
	}

	if( core::IsEmptyString( szInFilename ) )
		return(1);

	pInFile = fopen( szInFilename, "rb" );

	if( pInFile == 0 )
		return(1);

	// track it
	nFilesOpened++;

	fseek( pInFile, 0, SEEK_END );
	inLen = ftell( pInFile );
	fseek( pInFile, 0, SEEK_SET );

	if( inLen > 0 )
	{
		c = new char[inLen];

		if( c == 0 )
			return(1);
	}
	else
		return(1);

	if( core::IsEmptyString( szOutFilename ) )
		return(1);

	pOutFile = fopen( szOutFilename, "wb" );

	if( pOutFile == 0 )
		return(1);

	// track it
	nFilesOpened++;

	while( !feof(pInFile) )
	{
		// read it
		fread( c, sizeof( char ), inLen, pInFile );

		// write it
		fwrite( c, sizeof( char ), inLen, pOutFile );
	}

	if( c )
	{
		delete[] c;
		c = 0;
	}

	fclose( pInFile );
	// track it
	nFilesClosed++;

	fclose( pOutFile );
	// track it
	nFilesClosed++;
*/

	std::fstream f(szInFilename, std::fstream::in | std::fstream::binary);
	f << std::noskipws;
	std::istream_iterator<unsigned char> begin(f);
	std::istream_iterator<unsigned char> end;
 
	std::fstream f2(szOutFilename, std::fstream::out|std::fstream::trunc|std::fstream::binary);
	std::ostream_iterator<char> begin2(f2);
 
	std::copy(begin, end, begin2);

	return(0);
}

/////////////////////////////////////////////////////
/// Function: FileDelete
/// Params: [in]szFilename
///
/////////////////////////////////////////////////////
int file::FileDelete( const char *szFilename )
{
	int err = 0;

	err = remove( szFilename );

	if( err )
		return( 1 );

	return( 0 );
}

/////////////////////////////////////////////////////
/// Function: FileGetC
/// Params: [in]fileHandle
///
/////////////////////////////////////////////////////
int file::FileGetC( TFileHandle* fileHandle )
{
	int result = 0;

	if( fileHandle->nFileType == FILETYPE_BINARY_READ && fileHandle->nState == FILESTATE_OPEN )
	{
		if( fileHandle->zipped )
		{
			DBG_ASSERT(0);
			return 0;
		}
		else
		{
			if( fileHandle->fp )
			{
				result = fgetc( fileHandle->fp );
				return(result);
			}
		}
	}
	else if( fileHandle->nFileType == FILETYPE_BINARY_STREAM_READ && fileHandle->nState == FILESTATE_OPEN )
	{
		if( fileHandle->input.is_open() )
		{
			result = static_cast<int>( fileHandle->input.get() );

			return(result);
		}
	}
	else
	{
		DBGLOG( "FILEIO: *ERROR* unrecognised read type in FileGetC\n" );
		return(0);
	}

	return(result);
}

/////////////////////////////////////////////////////
/// Function: FileGetPos
/// Params: [in/out]pos, [in]fileHandle
///
/////////////////////////////////////////////////////
int file::FileGetPos( fpos_t* pos, TFileHandle* fileHandle )
{
	int result = 0;

	if( fileHandle->nFileType == FILETYPE_BINARY_READ && fileHandle->nState == FILESTATE_OPEN )
	{
		if( fileHandle->zipped )
		{
			DBG_ASSERT(0);
			return 0;
		}
		else
		{
			if( fileHandle->fp )
			{
				result = fgetpos( fileHandle->fp, pos );
				return(result);
			}
		}
	}
	else
	{
		DBGLOG( "FILEIO: *ERROR* unrecognised read type in FileGetPos\n" );
		return(0);
	}

	return(result);
}

/////////////////////////////////////////////////////
/// Function: FileRead
/// Params: [in]buffer, [in]size, [in]fileHandle
///
/////////////////////////////////////////////////////
std::size_t file::FileRead( void *buffer, std::size_t size, TFileHandle* fileHandle )
{
	std::size_t result = 0;

	// char reads are endian safe

	if( fileHandle->nFileType == FILETYPE_BINARY_READ && 
		fileHandle->nState == FILESTATE_OPEN )
	{
		if( fileHandle->zipped )
		{
			if( fileHandle->zfp )
			{
				result += zzip_fread( buffer, 1, size, fileHandle->zfp );

				return(result);
			}
		}
		else
		{
			if( fileHandle->fp )
			{

#ifdef _DEBUG
				// always check that the file is not read past EOF
				if( feof(fileHandle->fp ) )
				{
					DBGLOG( "FILEIO: *ERROR* reading past the EOF in FileRead\n" );
					DBG_ASSERT_MSG( 0, "FILEIO: *ERROR* reading past the EOF in FileRead" );
				}
#endif // _DEBUG

				result += fread( buffer, 1, size, fileHandle->fp );
				return(result);
			}
		}
	}
	else if( fileHandle->nFileType == FILETYPE_BINARY_STREAM_READ && 
		fileHandle->nState == FILESTATE_OPEN )
	{
		if( fileHandle->input.is_open() )
		{
			fileHandle->input.read( reinterpret_cast<char *>(buffer), static_cast<std::streamsize>(size) );
			result += static_cast<std::size_t>(fileHandle->input.gcount());

			return(result);
		}
	}
	else
	{
		DBGLOG( "FILEIO: *ERROR* unrecognised read type in FileRead\n" );
		return(0);
	}

	return(result);
}

/////////////////////////////////////////////////////
/// Function: FileRead
/// Params: [in]buffer, [in]size, [in]count, [in]fileHandle
///
/////////////////////////////////////////////////////
std::size_t file::FileRead( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle )
{
	std::size_t result = 0;

	// char reads are endian safe

	if( fileHandle->nFileType == FILETYPE_BINARY_READ && 
		fileHandle->nState == FILESTATE_OPEN )
	{
		if( fileHandle->zipped )
		{
			if( fileHandle->zfp )
			{
				result += zzip_fread( buffer, size, count, fileHandle->zfp );

				return(result);
			}
		}
		else
		{
			if( fileHandle->fp )
			{
#ifdef _DEBUG
				// always check that the file is not read past EOF
				if( feof(fileHandle->fp ) )
				{
					DBGLOG( "FILEIO: *ERROR* reading past the EOF in FileRead\n" );
					DBG_ASSERT_MSG( 0, "FILEIO: *ERROR* reading past the EOF in FileRead" );
				}
#endif // _DEBUG

				result += fread( buffer, size, count, fileHandle->fp );

				return(result);
			}
		}
	}
	else if( fileHandle->nFileType == FILETYPE_BINARY_STREAM_READ && 
		fileHandle->nState == FILESTATE_OPEN )
	{
		if( fileHandle->input.is_open() )
		{
			fileHandle->input.read( reinterpret_cast<char *>(buffer), static_cast<std::streamsize>(size*count) );
			result += static_cast<std::size_t>(fileHandle->input.gcount());

			return(result);
		}
	}
	else if( fileHandle->nFileType == FILETYPE_TEXT_READ  && 
		fileHandle->nState == FILESTATE_OPEN )
	{
		if( fileHandle->zipped )
		{
			if( fileHandle->zfp )
			{
				result += zzip_fread( buffer, size, count, fileHandle->zfp );

				return(result);
			}
		}
		else
		{
			if( fileHandle->fp )
			{
#ifdef _DEBUG
				// always check that the file is not read past EOF
				if( feof(fileHandle->fp ) )
				{
					DBGLOG( "FILEIO: *ERROR* reading past the EOF in FileRead\n" );
					DBG_ASSERT_MSG( 0, "FILEIO: *ERROR* reading past the EOF in FileRead" );
				}
#endif // _DEBUG

				result += fread( buffer, size, count, fileHandle->fp );

				return(result);
			}
		}
	}
	else
	{
		DBGLOG( "FILEIO: *ERROR* unrecognised read type in FileRead\n" );
		return(0);
	}

	return(result);
}

/////////////////////////////////////////////////////
/// Function: FileReadUChar
/// Params: [in]buffer, [in]size, [in]count, [in]fileHandle
///
/////////////////////////////////////////////////////
std::size_t file::FileReadUChar( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle )
{
	std::size_t result = 0;

	// char reads are endian safe

	if( fileHandle->nFileType == FILETYPE_BINARY_READ && fileHandle->nState == FILESTATE_OPEN )
	{
		if( fileHandle->zipped )
		{
			if( fileHandle->zfp )
			{
				result += zzip_fread( buffer, size, count, fileHandle->zfp );

				return(result);
			}
		}
		else
		{
			if( fileHandle->fp )
			{
#ifdef _DEBUG
				// always check that the file is not read past EOF
				if( feof(fileHandle->fp ) )
				{
					DBGLOG( "FILEIO: *ERROR* reading past the EOF in FileReadUChar\n" );
					DBG_ASSERT_MSG( 0, "FILEIO: *ERROR* reading past the EOF in FileReadUChar" );
				}
#endif // _DEBUG

				result += fread( buffer, size, count, fileHandle->fp );
				return(result);
			}
		}
	}
	else if( fileHandle->nFileType == FILETYPE_BINARY_STREAM_READ && fileHandle->nState == FILESTATE_OPEN )
	{
		if( fileHandle->input.is_open() )
		{
			fileHandle->input.read( reinterpret_cast<char *>(buffer), static_cast<std::streamsize>(size*count) );
			result += static_cast<std::size_t>(fileHandle->input.gcount());

			return(result);
		}
	}
	else
	{
		DBGLOG( "FILEIO: *ERROR* unrecognised read type in FileReadUChar\n" );
		return(0);
	}

	return(result);
}

/////////////////////////////////////////////////////
/// Function: FileReadChar
/// Params: [in]buffer, [in]size, [in]count, [in]fileHandle
///
/////////////////////////////////////////////////////
std::size_t file::FileReadChar( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle )
{
	std::size_t result = 0;

	// char reads are endian safe

	if( fileHandle->nFileType == FILETYPE_BINARY_READ && fileHandle->nState == FILESTATE_OPEN )
	{
		if( fileHandle->zipped )
		{
			if( fileHandle->zfp )
			{
				result += zzip_fread( buffer, size, count, fileHandle->zfp );

				return(result);
			}
		}
		else
		if( fileHandle->fp )
		{
#ifdef _DEBUG
			// always check that the file is not read past EOF
			if( feof(fileHandle->fp ) )
			{
				DBGLOG( "FILEIO: *ERROR* reading past the EOF in FileReadChar\n" );
				DBG_ASSERT_MSG( 0, "FILEIO: *ERROR* reading past the EOF in FileReadChar" );
			}
#endif // _DEBUG

			result += fread( buffer, size, count, fileHandle->fp );
			return(result);
		}
	}
	else if( fileHandle->nFileType == FILETYPE_BINARY_STREAM_READ && fileHandle->nState == FILESTATE_OPEN )
	{
		if( fileHandle->input.is_open() )
		{
			fileHandle->input.read( reinterpret_cast<char *>(buffer), static_cast<std::streamsize>(size*count) );
			result += static_cast<std::size_t>(fileHandle->input.gcount());
			return(result);
		}
	}
	else
	{
		DBGLOG( "FILEIO: *ERROR* unrecognised read type in FileReadChar\n" );
		return(0);
	}

	return(result);
}

/////////////////////////////////////////////////////
/// Function: FileReadBool
/// Params: [in]buffer, [in]size, [in]count, [in]fileHandle
///
/////////////////////////////////////////////////////
std::size_t file::FileReadBool( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle )
{
	std::size_t result = 0;

	if( fileHandle->nFileType == FILETYPE_BINARY_READ && fileHandle->nState == FILESTATE_OPEN )
	{
		if( fileHandle->zipped )
		{
			if( fileHandle->zfp )
			{
				result += zzip_fread( buffer, size, count, fileHandle->zfp );

				return(result);
			}
		}
		else
		if( fileHandle->fp )
		{
#ifdef _DEBUG
			// always check that the file is not read past EOF
			if( feof(fileHandle->fp ) )
			{
				DBGLOG( "FILEIO: *ERROR* reading past the EOF in FileReadBool\n" );
				DBG_ASSERT_MSG( 0, "FILEIO: *ERROR* reading past the EOF in FileReadBool" );
			}
#endif // _DEBUG

			result += fread( buffer, size, count, fileHandle->fp );
			return(result);
		}
	}
	else if( fileHandle->nFileType == FILETYPE_BINARY_STREAM_READ && fileHandle->nState == FILESTATE_OPEN )
	{
		if( fileHandle->input.is_open() )
		{
			fileHandle->input.read( reinterpret_cast<char *>(buffer), static_cast<std::streamsize>(size*count) );
			result += static_cast<std::size_t>(fileHandle->input.gcount());
			return(result);
		}
	}
	else
	{
		DBGLOG( "FILEIO: *ERROR* unrecognised read type in FileReadBool\n" );
		return(0);
	}

	return(result);
}

/////////////////////////////////////////////////////
/// Function: FileReadShort
/// Params: [in]buffer, [in]size, [in]count, [in]fileHandle
///
/////////////////////////////////////////////////////
std::size_t file::FileReadShort( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle )
{
	unsigned int i = 0;
	std::size_t result = 0;
	short nTempShort = 0;

	if( count > 1 )
	{
		for( i = 0; i < count; i++ )
		{
			if( fileHandle->nFileType == FILETYPE_BINARY_READ && fileHandle->nState == FILESTATE_OPEN )
			{
				if( fileHandle->zipped )
				{
					if( fileHandle->zfp )
					{
						result += zzip_fread( &nTempShort, size, 1, fileHandle->zfp );
					}
				}
				else
				if( fileHandle->fp )
				{
#ifdef _DEBUG
					// always check that the file is not read past EOF
					if( feof(fileHandle->fp ) )
					{
						DBGLOG( "FILEIO: *ERROR* reading past the EOF in FileReadShort\n" );
						DBG_ASSERT_MSG( 0, "FILEIO: *ERROR* reading past the EOF in FileReadShort" );
					}
#endif // _DEBUG
					result += fread( &nTempShort, size, 1, fileHandle->fp );
				}
			}
			else if( fileHandle->nFileType == FILETYPE_BINARY_STREAM_READ && fileHandle->nState == FILESTATE_OPEN )
			{
				if( fileHandle->input.is_open() )
				{
					fileHandle->input.read( reinterpret_cast<char *>(&nTempShort), static_cast<std::streamsize>(size) );
					result += static_cast<std::size_t>(fileHandle->input.gcount());
				}
			}
			else
			{
				DBGLOG( "FILEIO: *ERROR* unrecognised read type in FileReadShort\n" );
				return(0);
			}

			short *pTmpbuffer = reinterpret_cast<short *>(buffer);

			pTmpbuffer[i] = core::EndianSwapShort( nTempShort, core::MACHINE_LITTLE_ENDIAN );
		}
	}
	else
	{
		if( fileHandle->nFileType == FILETYPE_BINARY_READ && fileHandle->nState == FILESTATE_OPEN )
		{
			if( fileHandle->zipped )
			{
				if( fileHandle->zfp )
				{
					result += zzip_fread( &nTempShort, size, 1, fileHandle->zfp );
				}
			}
			else
			if( fileHandle->fp )
			{
#ifdef _DEBUG
				// always check that the file is not read past EOF
				if( feof(fileHandle->fp ) )
				{
					DBGLOG( "FILEIO: *ERROR* reading past the EOF in FileReadShort\n" );
					DBG_ASSERT_MSG( 0, "FILEIO: *ERROR* reading past the EOF in FileReadShort" );
				}
#endif // _DEBUG
				result += fread( &nTempShort, size, 1, fileHandle->fp );
			}
		}
		else if( fileHandle->nFileType == FILETYPE_BINARY_STREAM_READ && fileHandle->nState == FILESTATE_OPEN )
		{
			if( fileHandle->input.is_open() )
			{
				fileHandle->input.read( reinterpret_cast<char *>(&nTempShort), static_cast<std::streamsize>(size) );
				result += static_cast<std::size_t>(fileHandle->input.gcount());
			}
		}
		else
		{
			DBGLOG( "FILEIO: *ERROR* unrecognised read type in FileReadShort\n" );
			return(0);
		}

		short *pTmpbuffer = reinterpret_cast<short *>(buffer);

		*pTmpbuffer = core::EndianSwapShort( nTempShort, core::MACHINE_LITTLE_ENDIAN );	

	}
	
	return(result);
}

/////////////////////////////////////////////////////
/// Function: FileReadUShort
/// Params: [in]buffer, [in]size, [in]count, [in]fileHandle
///
/////////////////////////////////////////////////////
std::size_t file::FileReadUShort( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle )
{
	unsigned int i = 0;
	std::size_t result = 0;
	unsigned short nTempShort = 0;

	if( count > 1 )
	{
		for( i = 0; i < count; i++ )
		{
			if( fileHandle->nFileType == FILETYPE_BINARY_READ && fileHandle->nState == FILESTATE_OPEN )
			{
				if( fileHandle->zipped )
				{
					if( fileHandle->zfp )
					{
						result += zzip_fread( &nTempShort, size, 1, fileHandle->zfp );
					}
				}
				else
				if( fileHandle->fp )
				{
#ifdef _DEBUG
					// always check that the file is not read past EOF
					if( feof(fileHandle->fp ) )
					{
						DBGLOG( "FILEIO: *ERROR* reading past the EOF in FileReadUShort\n" );
						DBG_ASSERT_MSG( 0, "FILEIO: *ERROR* reading past the EOF in FileReadUShort" );
					}
#endif // _DEBUG
					result += fread( &nTempShort, size, 1, fileHandle->fp );
				}
			}
			else if( fileHandle->nFileType == FILETYPE_BINARY_STREAM_READ && fileHandle->nState == FILESTATE_OPEN )
			{
				if( fileHandle->input.is_open() )
				{
					fileHandle->input.read( reinterpret_cast<char *>(&nTempShort), static_cast<std::streamsize>(size) );
					result += static_cast<std::size_t>(fileHandle->input.gcount());
				}
			}
			else
			{
				DBGLOG( "FILEIO: *ERROR* unrecognised read type in FileReadUShort\n" );
				return(0);
			}

			unsigned short *pTmpbuffer = reinterpret_cast<unsigned short *>(buffer);

			pTmpbuffer[i] = core::EndianSwapShort( nTempShort, core::MACHINE_LITTLE_ENDIAN );
		}
	}
	else
	{
		if( fileHandle->nFileType == FILETYPE_BINARY_READ && fileHandle->nState == FILESTATE_OPEN )
		{
			if( fileHandle->zipped )
			{
				if( fileHandle->zfp )
				{
					result += zzip_fread( &nTempShort, size, 1, fileHandle->zfp );
				}
			}
			else
			if( fileHandle->fp )
			{
#ifdef _DEBUG
				// always check that the file is not read past EOF
				if( feof(fileHandle->fp ) )
				{
					DBGLOG( "FILEIO: *ERROR* reading past the EOF in FileReadUShort\n" );
					DBG_ASSERT_MSG( 0, "FILEIO: *ERROR* reading past the EOF in FileReadUShort" );
				}
#endif // _DEBUG
				result += fread( &nTempShort, size, 1, fileHandle->fp );
			}
		}
		else if( fileHandle->nFileType == FILETYPE_BINARY_STREAM_READ && fileHandle->nState == FILESTATE_OPEN )
		{
			if( fileHandle->input.is_open() )
			{
				fileHandle->input.read( reinterpret_cast<char *>(&nTempShort), static_cast<std::streamsize>(size) );
				result += static_cast<std::size_t>(fileHandle->input.gcount());
			}
		}
		else
		{
			DBGLOG( "FILEIO: *ERROR* unrecognised read type in FileReadUShort\n" );
			return(0);
		}

		unsigned short *pTmpbuffer = reinterpret_cast<unsigned short *>(buffer);

		*pTmpbuffer = core::EndianSwapShort( nTempShort, core::MACHINE_LITTLE_ENDIAN );	

	}
	
	return(result);
}

/////////////////////////////////////////////////////
/// Function: FileReadInt
/// Params: [in]buffer, [in]size, [in]count, [in]fileHandle
///
/////////////////////////////////////////////////////
std::size_t file::FileReadInt( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle )
{
	unsigned int i = 0;
	std::size_t result = 0;
	int nTempInt = 0;

	if( count > 1 )
	{
		for( i = 0; i < count; i++ )
		{
			if( fileHandle->nFileType == FILETYPE_BINARY_READ && fileHandle->nState == FILESTATE_OPEN )
			{
				if( fileHandle->zipped )
				{
					if( fileHandle->zfp )
					{
						result += zzip_fread( &nTempInt, size, 1, fileHandle->zfp );
					}
				}
				else
				if( fileHandle->fp )
				{
#ifdef _DEBUG
					// always check that the file is not read past EOF
					if( feof(fileHandle->fp ) )
					{
						DBGLOG( "FILEIO: *ERROR* reading past the EOF in FileReadInt\n" );
						DBG_ASSERT_MSG( 0, "FILEIO: *ERROR* reading past the EOF in FileReadInt" );
					}
#endif // _DEBUG
					result += fread( &nTempInt, size, 1, fileHandle->fp );
				}
			}
			else if( fileHandle->nFileType == FILETYPE_BINARY_STREAM_READ && fileHandle->nState == FILESTATE_OPEN )
			{
				if( fileHandle->input.is_open() )
				{
					fileHandle->input.read( reinterpret_cast<char *>(&nTempInt), static_cast<std::streamsize>(size) );
					result += static_cast<std::size_t>(fileHandle->input.gcount());
				}
			}
			else
			{
				DBGLOG( "FILEIO: *ERROR* unrecognised read type in FileReadInt\n" );
				return(0);
			}

			int *pTmpbuffer = reinterpret_cast<int *>(buffer);

			pTmpbuffer[i] = core::EndianSwapInt( nTempInt, core::MACHINE_LITTLE_ENDIAN );
		}
	}
	else
	{
		if( fileHandle->nFileType == FILETYPE_BINARY_READ && fileHandle->nState == FILESTATE_OPEN )
		{
			if( fileHandle->zipped )
			{
				if( fileHandle->zfp )
				{
					result += zzip_fread( &nTempInt, size, 1, fileHandle->zfp );
				}
			}
			else
			if( fileHandle->fp )
			{
#ifdef _DEBUG
				// always check that the file is not read past EOF
				if( feof(fileHandle->fp ) )
				{
					DBGLOG( "FILEIO: *ERROR* reading past the EOF in FileReadInt\n" );
					DBG_ASSERT_MSG( 0, "FILEIO: *ERROR* reading past the EOF in FileReadInt" );
				}
#endif // _DEBUG
				result += fread( &nTempInt, size, 1, fileHandle->fp );
			}
		}
		else if( fileHandle->nFileType == FILETYPE_BINARY_STREAM_READ && fileHandle->nState == FILESTATE_OPEN )
		{
			if( fileHandle->input.is_open() )
			{
				fileHandle->input.read( reinterpret_cast<char *>(&nTempInt), static_cast<std::streamsize>(size) );
				result += static_cast<std::size_t>(fileHandle->input.gcount());
			}
		}
		else
		{
			DBGLOG( "FILEIO: *ERROR* unrecognised read type in FileReadInt\n" );
			return(0);
		}

		int *pTmpbuffer = reinterpret_cast<int *>(buffer);

		*pTmpbuffer = core::EndianSwapInt( nTempInt, core::MACHINE_LITTLE_ENDIAN );
	}
	
	return(result);
}

/////////////////////////////////////////////////////
/// Function: FileReadUInt
/// Params: [in]buffer, [in]size, [in]count, [in]fileHandle
///
/////////////////////////////////////////////////////
std::size_t file::FileReadUInt( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle )
{
	unsigned int i = 0;
	std::size_t result = 0;
	unsigned int nTempInt = 0;

	if( count > 1 )
	{
		for( i = 0; i < count; i++ )
		{
			if( fileHandle->nFileType == FILETYPE_BINARY_READ && fileHandle->nState == FILESTATE_OPEN )
			{
				if( fileHandle->zipped )
				{
					if( fileHandle->zfp )
					{
						result += zzip_fread( &nTempInt, size, 1, fileHandle->zfp );
					}
				}
				else
				if( fileHandle->fp )
				{
#ifdef _DEBUG
					// always check that the file is not read past EOF
					if( feof(fileHandle->fp ) )
					{
						DBGLOG( "FILEIO: *ERROR* reading past the EOF in FileReadUInt\n" );
						DBG_ASSERT_MSG( 0, "FILEIO: *ERROR* reading past the EOF in FileReadUInt" );
					}
#endif // _DEBUG
					result += fread( &nTempInt, size, 1, fileHandle->fp );
				}
			}
			else if( fileHandle->nFileType == FILETYPE_BINARY_STREAM_READ && fileHandle->nState == FILESTATE_OPEN )
			{
				if( fileHandle->input.is_open() )
				{
					fileHandle->input.read( reinterpret_cast<char *>(&nTempInt), static_cast<std::streamsize>(size) );
					result += static_cast<std::size_t>(fileHandle->input.gcount());
				}
			}
			else
			{
				DBGLOG( "FILEIO: *ERROR* unrecognised read type in FileReadUInt\n" );
				return(0);
			}

			unsigned int *pTmpbuffer = reinterpret_cast<unsigned int *>(buffer);

			pTmpbuffer[i] = core::EndianSwapInt( nTempInt, core::MACHINE_LITTLE_ENDIAN );
		}
	}
	else
	{
		if( fileHandle->nFileType == FILETYPE_BINARY_READ && fileHandle->nState == FILESTATE_OPEN )
		{
			if( fileHandle->zipped )
			{
				if( fileHandle->zfp )
				{
					result += zzip_fread( &nTempInt, size, 1, fileHandle->zfp );
				}
			}
			else
			if( fileHandle->fp )
			{
#ifdef _DEBUG
				// always check that the file is not read past EOF
				if( feof(fileHandle->fp ) )
				{
					DBGLOG( "FILEIO: *ERROR* reading past the EOF in FileReadUInt\n" );
					DBG_ASSERT_MSG( 0, "FILEIO: *ERROR* reading past the EOF in FileReadUInt" );
				}
#endif // _DEBUG
				result += fread( &nTempInt, size, 1, fileHandle->fp );
			}
		}
		else if( fileHandle->nFileType == FILETYPE_BINARY_STREAM_READ && fileHandle->nState == FILESTATE_OPEN )
		{
			if( fileHandle->input.is_open() )
			{
				fileHandle->input.read( reinterpret_cast<char *>(&nTempInt), static_cast<std::streamsize>(size) );
				result += static_cast<std::size_t>(fileHandle->input.gcount());
			}
		}
		else
		{
			DBGLOG( "FILEIO: *ERROR* unrecognised read type in FileReadUInt\n" );
			return(0);
		}

		unsigned int *pTmpbuffer = reinterpret_cast<unsigned int *>(buffer);

		*pTmpbuffer = core::EndianSwapInt( nTempInt, core::MACHINE_LITTLE_ENDIAN );
	}
	
	return(result);
}

/////////////////////////////////////////////////////
/// Function: FileReadFloat
/// Params: [in]buffer, [in]size, [in]count, [in]fileHandle
///
/////////////////////////////////////////////////////
std::size_t file::FileReadFloat( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle )
{
	unsigned int i = 0;
	std::size_t result = 0;
	float fTempFloat = 0.0f;

	if( count > 1 )
	{
		for( i = 0; i < count; i++ )
		{
			if( fileHandle->nFileType == FILETYPE_BINARY_READ && fileHandle->nState == FILESTATE_OPEN )
			{
				if( fileHandle->zipped )
				{
					if( fileHandle->zfp )
					{
						result += zzip_fread( &fTempFloat, size, 1, fileHandle->zfp );
					}
				}
				else
				if( fileHandle->fp )
				{
#ifdef _DEBUG
					// always check that the file is not read past EOF
					if( feof(fileHandle->fp ) )
					{
						DBGLOG( "FILEIO: *ERROR* reading past the EOF in FileReadFloat\n" );
						DBG_ASSERT_MSG( 0, "FILEIO: *ERROR* reading past the EOF in FileReadFloat" );
					}
#endif // _DEBUG
					result += fread( &fTempFloat, size, 1, fileHandle->fp );
				}
			}
			else if( fileHandle->nFileType == FILETYPE_BINARY_STREAM_READ && fileHandle->nState == FILESTATE_OPEN )
			{
				if( fileHandle->input.is_open() )
				{
					fileHandle->input.read( reinterpret_cast<char *>(&fTempFloat), static_cast<std::streamsize>(size) );
					result += static_cast<std::size_t>(fileHandle->input.gcount());
				}
			}
			else
			{
				DBGLOG( "FILEIO: *ERROR* unrecognised read type in FileReadFloat\n" );
				return(0);
			}

			float *pTmpbuffer = reinterpret_cast<float *>(buffer);

			pTmpbuffer[i] = core::EndianSwapFloat( fTempFloat, core::MACHINE_LITTLE_ENDIAN );
		}
	}
	else
	{
		if( fileHandle->nFileType == FILETYPE_BINARY_READ && fileHandle->nState == FILESTATE_OPEN )
		{
			if( fileHandle->zipped )
			{
				if( fileHandle->zfp )
				{
					result += zzip_fread( &fTempFloat, size, 1, fileHandle->zfp );
				}
			}
			else
			if( fileHandle->fp )
			{
#ifdef _DEBUG
				// always check that the file is not read past EOF
				if( feof(fileHandle->fp ) )
				{
					DBGLOG( "FILEIO: *ERROR* reading past the EOF in FileReadFloat\n" );
					DBG_ASSERT_MSG( 0, "FILEIO: *ERROR* reading past the EOF in FileReadFloat" );
				}
#endif // _DEBUG
				result += fread( &fTempFloat, size, 1, fileHandle->fp );
			}
		}
		else if( fileHandle->nFileType == FILETYPE_BINARY_STREAM_READ && fileHandle->nState == FILESTATE_OPEN )
		{
			if( fileHandle->input.is_open() )
			{
				fileHandle->input.read( reinterpret_cast<char *>(&fTempFloat), static_cast<std::streamsize>(size) );
				result += static_cast<std::size_t>(fileHandle->input.gcount());
			}
		}
		else
		{
			DBGLOG( "FILEIO: *ERROR* unrecognised read type in FileReadFloat\n" );
			return(0);
		}

		float *pTmpbuffer = reinterpret_cast<float *>(buffer);

		*pTmpbuffer = core::EndianSwapFloat( fTempFloat, core::MACHINE_LITTLE_ENDIAN );
	}
	
	return(result);
}

/////////////////////////////////////////////////////
/// Function: FileReadDouble
/// Params: [in]buffer, [in]size, [in]count, [in]fileHandle
///
/////////////////////////////////////////////////////
std::size_t file::FileReadDouble( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle )
{
	unsigned int i = 0;
	std::size_t result = 0;
	double dTempDouble = 0.0f;

	if( count > 1 )
	{
		for( i = 0; i < count; i++ )
		{
			if( fileHandle->nFileType == FILETYPE_BINARY_READ && fileHandle->nState == FILESTATE_OPEN )
			{
				if( fileHandle->zipped )
				{
					if( fileHandle->zfp )
					{
						result += zzip_fread( &dTempDouble, size, 1, fileHandle->zfp );
					}
				}
				else
				if( fileHandle->fp )
				{
#ifdef _DEBUG
					// always check that the file is not read past EOF
					if( feof(fileHandle->fp ) )
					{
						DBGLOG( "FILEIO: *ERROR* reading past the EOF in FileReadFloat\n" );
						DBG_ASSERT_MSG( 0, "FILEIO: *ERROR* reading past the EOF in FileReadFloat" );
					}
#endif // _DEBUG
					result += fread( &dTempDouble, size, 1, fileHandle->fp );
				}
			}
			else if( fileHandle->nFileType == FILETYPE_BINARY_STREAM_READ && fileHandle->nState == FILESTATE_OPEN )
			{
				if( fileHandle->input.is_open() )
				{
					fileHandle->input.read( reinterpret_cast<char *>(&dTempDouble), static_cast<std::streamsize>(size) );
					result += static_cast<std::size_t>(fileHandle->input.gcount());
				}
			}
			else
			{
				DBGLOG( "FILEIO: *ERROR* unrecognised read type in FileReadFloat\n" );
				return(0);
			}

			float *pTmpbuffer = reinterpret_cast<float *>(buffer);

			pTmpbuffer[i] = static_cast<float>(dTempDouble);//core::EndianSwapFloat( dTempDouble, core::MACHINE_LITTLE_ENDIAN );
		}
	}
	else
	{
		if( fileHandle->nFileType == FILETYPE_BINARY_READ && fileHandle->nState == FILESTATE_OPEN )
		{
			if( fileHandle->zipped )
			{
				if( fileHandle->zfp )
				{
					result += zzip_fread( &dTempDouble, size, 1, fileHandle->zfp );
				}
			}
			else
			if( fileHandle->fp )
			{
#ifdef _DEBUG
				// always check that the file is not read past EOF
				if( feof(fileHandle->fp ) )
				{
					DBGLOG( "FILEIO: *ERROR* reading past the EOF in FileReadFloat\n" );
					DBG_ASSERT_MSG( 0, "FILEIO: *ERROR* reading past the EOF in FileReadFloat" );
				}
#endif // _DEBUG
				result += fread( &dTempDouble, size, 1, fileHandle->fp );
			}
		}
		else if( fileHandle->nFileType == FILETYPE_BINARY_STREAM_READ && fileHandle->nState == FILESTATE_OPEN )
		{
			if( fileHandle->input.is_open() )
			{
				fileHandle->input.read( reinterpret_cast<char *>(&dTempDouble), static_cast<std::streamsize>(size) );
				result += static_cast<std::size_t>(fileHandle->input.gcount());
			}
		}
		else
		{
			DBGLOG( "FILEIO: *ERROR* unrecognised read type in FileReadFloat\n" );
			return(0);
		}

		float *pTmpbuffer = reinterpret_cast<float *>(buffer);

		*pTmpbuffer = static_cast<float>(dTempDouble);//core::EndianSwapFloat( dTempDouble, core::MACHINE_LITTLE_ENDIAN );
	}
	
	return(result);
}

/////////////////////////////////////////////////////
/// Function: FileReadVec2D
/// Params: [in]buffer, [in]size, [in]count, [in]fileHandle
///
/////////////////////////////////////////////////////
std::size_t file::FileReadTerminatedString( char *buffer, unsigned int maxCount, TFileHandle* fileHandle )
{
	std::size_t result = 0;

	// char reads are endian safe

	unsigned int bufferIndex = 0;
	bool completedRead = false;

	if( fileHandle->nFileType == FILETYPE_BINARY_READ && fileHandle->nState == FILESTATE_OPEN )
	{
		if( fileHandle->zipped )
		{
			if( fileHandle->zfp )
			{
				while( !completedRead )
				{
					result += zzip_fread( &buffer[bufferIndex], sizeof(char), 1, fileHandle->zfp );

					if( buffer[bufferIndex] == '\0' )
						return(result);
					else
					{
						bufferIndex++;

						if( bufferIndex >= maxCount )
							return(result);
					}
				}
			}
		}
		else
		if( fileHandle->fp )
		{
			while( !completedRead )
			{
#ifdef _DEBUG
				// always check that the file is not read past EOF
				if( feof(fileHandle->fp ) )
				{
					DBGLOG( "FILEIO: *ERROR* reading past the EOF in FileReadChar\n" );
					DBG_ASSERT_MSG( 0, "FILEIO: *ERROR* reading past the EOF in FileReadChar" );
				}
#endif // _DEBUG

				result += fread( &buffer[bufferIndex], sizeof(char), 1, fileHandle->fp );

				if( buffer[bufferIndex] == '\0' )
					return(result);
				else
				{
					bufferIndex++;

					if( bufferIndex >= maxCount )
						return(result);
				}
			}
		}
	}
	else if( fileHandle->nFileType == FILETYPE_BINARY_STREAM_READ && fileHandle->nState == FILESTATE_OPEN )
	{
		if( fileHandle->input.is_open() )
		{
			while( !completedRead )
			{
				fileHandle->input.read( &buffer[bufferIndex], static_cast<std::streamsize>(sizeof(char)) );
				result += static_cast<std::size_t>(fileHandle->input.gcount());

				if( buffer[bufferIndex] == '\0' )
					return(result);
				else
				{
					bufferIndex++;

					if( bufferIndex >= maxCount )
						return(result);
				}
			}
		}
	}
	else
	{
		DBGLOG( "FILEIO: *ERROR* unrecognised read type in FileReadChar\n" );
		return(0);
	}

	return(result);
}

/////////////////////////////////////////////////////
/// Function: FileReadVec2D
/// Params: [in]buffer, [in]size, [in]count, [in]fileHandle
///
/////////////////////////////////////////////////////
std::size_t file::FileReadVec2D( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle )
{
	unsigned int i = 0;
	std::size_t result = 0;

	math::Vec2 vTempVec( 0.0f, 0.0f );

	if( count > 1 )
	{
		for( i = 0; i < count; i++ )
		{
			if( fileHandle->nFileType == FILETYPE_BINARY_READ && fileHandle->nState == FILESTATE_OPEN )
			{
				if( fileHandle->zipped )
				{
					if( fileHandle->zfp )
					{
						result += zzip_fread( &vTempVec, size, 1, fileHandle->zfp );
					}
				}
				else
				if( fileHandle->fp )
				{
#ifdef _DEBUG
					// always check that the file is not read past EOF
					if( feof(fileHandle->fp ) )
					{
						DBGLOG( "FILEIO: *ERROR* reading past the EOF in FileReadVec2D\n" );
						DBG_ASSERT_MSG( 0, "FILEIO: *ERROR* reading past the EOF in FileReadVec2D" );
					}
#endif // _DEBUG
					result += fread( &vTempVec, size, 1, fileHandle->fp );
				}
			}
			else if( fileHandle->nFileType == FILETYPE_BINARY_STREAM_READ && fileHandle->nState == FILESTATE_OPEN )
			{
				if( fileHandle->input.is_open() )
				{
					fileHandle->input.read( reinterpret_cast<char *>(&vTempVec), static_cast<std::streamsize>(size) );
					result += static_cast<std::size_t>(fileHandle->input.gcount());
				}
			}
			else
			{
				DBGLOG( "FILEIO: *ERROR* unrecognised read type in FileReadVec2D\n" );
				return(0);
			}

			math::Vec2 *pTmpbuffer = reinterpret_cast<math::Vec2 *>(buffer);

			pTmpbuffer[i].X = core::EndianSwapFloat( vTempVec.X, core::MACHINE_LITTLE_ENDIAN );
			pTmpbuffer[i].Y = core::EndianSwapFloat( vTempVec.Y, core::MACHINE_LITTLE_ENDIAN );
		}
	}
	else
	{
		
		if( fileHandle->nFileType == FILETYPE_BINARY_READ && fileHandle->nState == FILESTATE_OPEN )
		{
			if( fileHandle->zipped )
			{
				if( fileHandle->zfp )
				{
					result += zzip_fread( &vTempVec, size, 1, fileHandle->zfp );
				}
			}
			else
			if( fileHandle->fp )
			{
#ifdef _DEBUG
				// always check that the file is not read past EOF
				if( feof(fileHandle->fp ) )
				{
					DBGLOG( "FILEIO: *ERROR* reading past the EOF in FileReadVec2D\n" );
					DBG_ASSERT_MSG( 0, "FILEIO: *ERROR* reading past the EOF in FileReadVec2D" );
				}
#endif // _DEBUG
				result += fread( &vTempVec, size, 1, fileHandle->fp );
			}
		}
		else if( fileHandle->nFileType == FILETYPE_BINARY_STREAM_READ && fileHandle->nState == FILESTATE_OPEN )
		{
			if( fileHandle->input.is_open() )
			{
				fileHandle->input.read( reinterpret_cast<char *>(&vTempVec), static_cast<std::streamsize>(size) );
				result += static_cast<std::size_t>(fileHandle->input.gcount());
			}
		}
		else
		{
			DBGLOG( "FILEIO: *ERROR* unrecognised read type in FileReadVec2D\n" );
			return(0);
		}

		math::Vec2 *pTmpbuffer = reinterpret_cast<math::Vec2 *>(buffer);

		pTmpbuffer->X = core::EndianSwapFloat( vTempVec.X, core::MACHINE_LITTLE_ENDIAN );
		pTmpbuffer->Y = core::EndianSwapFloat( vTempVec.Y, core::MACHINE_LITTLE_ENDIAN );

	}
	
	return(result);
}

/////////////////////////////////////////////////////
/// Function: FileReadVec3D
/// Params: [in]buffer, [in]size, [in]count, [in]fileHandle
///
/////////////////////////////////////////////////////
std::size_t file::FileReadVec3D( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle )
{
	unsigned int i = 0;
	std::size_t result = 0;
	math::Vec3 vTempVec( 0.0f, 0.0f, 0.0f );

	if( count > 1 )
	{
		for( i = 0; i < count; i++ )
		{
			if( fileHandle->nFileType == FILETYPE_BINARY_READ && fileHandle->nState == FILESTATE_OPEN )
			{
				if( fileHandle->zipped )
				{
					if( fileHandle->zfp )
					{
						result += zzip_fread( &vTempVec, size, 1, fileHandle->zfp );
					}
				}
				else
				if( fileHandle->fp )
				{
#ifdef _DEBUG
					// always check that the file is not read past EOF
					if( feof(fileHandle->fp ) )
					{
						DBGLOG( "FILEIO: *ERROR* reading past the EOF in FileReadVec3D\n" );
						DBG_ASSERT_MSG( 0, "FILEIO: *ERROR* reading past the EOF in FileReadVec3D" );
					}
#endif // _DEBUG
					result += fread( &vTempVec, size, 1, fileHandle->fp );
				}
			}
			else if( fileHandle->nFileType == FILETYPE_BINARY_STREAM_READ && fileHandle->nState == FILESTATE_OPEN )
			{
				if( fileHandle->input.is_open() )
				{
					fileHandle->input.read( reinterpret_cast<char *>(&vTempVec), static_cast<std::streamsize>(size) );
					result += static_cast<std::size_t>(fileHandle->input.gcount());
				}
			}
			else
			{
				DBGLOG( "FILEIO: *ERROR* unrecognised read type in FileReadVec3D\n" );
				return(0);
			}

			math::Vec3 *pTmpbuffer = reinterpret_cast<math::Vec3 *>(buffer);

			pTmpbuffer[i].X = core::EndianSwapFloat( vTempVec.X, core::MACHINE_LITTLE_ENDIAN );
			pTmpbuffer[i].Y = core::EndianSwapFloat( vTempVec.Y, core::MACHINE_LITTLE_ENDIAN );
			pTmpbuffer[i].Z = core::EndianSwapFloat( vTempVec.Z, core::MACHINE_LITTLE_ENDIAN );
		}
	}
	else
	{		
		if( fileHandle->nFileType == FILETYPE_BINARY_READ && fileHandle->nState == FILESTATE_OPEN )
		{
			if( fileHandle->zipped )
			{
				if( fileHandle->zfp )
				{
					result += zzip_fread( &vTempVec, size, 1, fileHandle->zfp );
				}
			}
			else
			if( fileHandle->fp )
			{
#ifdef _DEBUG
				// always check that the file is not read past EOF
				if( feof(fileHandle->fp ) )
				{
					DBGLOG( "FILEIO: *ERROR* reading past the EOF in FileReadVec3D\n" );
					DBG_ASSERT_MSG( 0, "FILEIO: *ERROR* reading past the EOF in FileReadVec3D" );
				}
#endif // _DEBUG
				result += fread( &vTempVec, size, 1, fileHandle->fp );
			}
		}
		else if( fileHandle->nFileType == FILETYPE_BINARY_STREAM_READ && fileHandle->nState == FILESTATE_OPEN )
		{
			if( fileHandle->input.is_open() )
			{
				fileHandle->input.read( reinterpret_cast<char *>(&vTempVec), static_cast<std::streamsize>(size) );
				result += static_cast<std::size_t>(fileHandle->input.gcount());
			}
		}
		else
		{
			DBGLOG( "FILEIO: *ERROR* unrecognised read type in FileReadVec3D\n" );
			return(0);
		}

		math::Vec3 *pTmpbuffer = reinterpret_cast<math::Vec3 *>(buffer);

		pTmpbuffer->X = core::EndianSwapFloat( vTempVec.X, core::MACHINE_LITTLE_ENDIAN );
		pTmpbuffer->Y = core::EndianSwapFloat( vTempVec.Y, core::MACHINE_LITTLE_ENDIAN );
		pTmpbuffer->Z = core::EndianSwapFloat( vTempVec.Z, core::MACHINE_LITTLE_ENDIAN );
	}
	
	return(result);
}

/////////////////////////////////////////////////////
/// Function: FileReadVec4D
/// Params: [in]buffer, [in]size, [in]count, [in]fileHandle
///
/////////////////////////////////////////////////////
std::size_t file::FileReadVec4D( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle )
{
	unsigned int i = 0;
	std::size_t result = 0;
	math::Vec4 vTempVec( 0.0f, 0.0f, 0.0f, 0.0f );

	if( count > 1 )
	{
		for( i = 0; i < count; i++ )
		{
			if( fileHandle->nFileType == FILETYPE_BINARY_READ && fileHandle->nState == FILESTATE_OPEN )
			{
				if( fileHandle->zipped )
				{
					if( fileHandle->zfp )
					{
						result += zzip_fread( &vTempVec, size, 1, fileHandle->zfp );
					}
				}
				else
				if( fileHandle->fp )
				{
#ifdef _DEBUG
					// always check that the file is not read past EOF
					if( feof(fileHandle->fp ) )
					{
						DBGLOG( "FILEIO: *ERROR* reading past the EOF in FileReadVec4D\n" );
						DBG_ASSERT_MSG( 0, "FILEIO: *ERROR* reading past the EOF in FileReadVec4D" );
					}
#endif // _DEBUG
					result += fread( &vTempVec, size, 1, fileHandle->fp );
				}
			}
			else if( fileHandle->nFileType == FILETYPE_BINARY_STREAM_READ && fileHandle->nState == FILESTATE_OPEN )
			{
				if( fileHandle->input.is_open() )
				{
					fileHandle->input.read( reinterpret_cast<char *>(&vTempVec), static_cast<std::streamsize>(size) );
					result += static_cast<std::size_t>(fileHandle->input.gcount());
				}
			}
			else
			{
				DBGLOG( "FILEIO: *ERROR* unrecognised read type in FileReadVec4D\n" );
				return(0);
			}

			math::Vec4 *pTmpbuffer = reinterpret_cast<math::Vec4 *>(buffer);

			pTmpbuffer[i].X = core::EndianSwapFloat( vTempVec.X, core::MACHINE_LITTLE_ENDIAN );
			pTmpbuffer[i].Y = core::EndianSwapFloat( vTempVec.Y, core::MACHINE_LITTLE_ENDIAN );
			pTmpbuffer[i].Z = core::EndianSwapFloat( vTempVec.Z, core::MACHINE_LITTLE_ENDIAN );
			pTmpbuffer[i].W = core::EndianSwapFloat( vTempVec.W, core::MACHINE_LITTLE_ENDIAN );
		}
	}
	else
	{	
	
		if( fileHandle->nFileType == FILETYPE_BINARY_READ && fileHandle->nState == FILESTATE_OPEN )
		{
			if( fileHandle->zipped )
			{
				if( fileHandle->zfp )
				{
					result += zzip_fread( &vTempVec, size, 1, fileHandle->zfp );
				}
			}
			else
			if( fileHandle->fp )
			{
#ifdef _DEBUG
				// always check that the file is not read past EOF
				if( feof(fileHandle->fp ) )
				{
					DBGLOG( "FILEIO: *ERROR* reading past the EOF in FileReadVec4D\n" );
					DBG_ASSERT_MSG( 0, "FILEIO: *ERROR* reading past the EOF in FileReadVec4D" );
				}
#endif // _DEBUG
				result += fread( &vTempVec, size, 1, fileHandle->fp );
			}
		}
		else if( fileHandle->nFileType == FILETYPE_BINARY_STREAM_READ && fileHandle->nState == FILESTATE_OPEN )
		{
			if( fileHandle->input.is_open() )
			{
				fileHandle->input.read( reinterpret_cast<char *>(&vTempVec), static_cast<std::streamsize>(size) );
				result += static_cast<std::size_t>(fileHandle->input.gcount());
			}
		}
		else
		{
			DBGLOG( "FILEIO: *ERROR* unrecognised read type in FileReadVec4D\n" );
			return(0);
		}

		math::Vec4 *pTmpbuffer = reinterpret_cast<math::Vec4 *>(buffer);

		pTmpbuffer->X = core::EndianSwapFloat( vTempVec.X, core::MACHINE_LITTLE_ENDIAN );
		pTmpbuffer->Y = core::EndianSwapFloat( vTempVec.Y, core::MACHINE_LITTLE_ENDIAN );
		pTmpbuffer->Z = core::EndianSwapFloat( vTempVec.Z, core::MACHINE_LITTLE_ENDIAN );
		pTmpbuffer->W = core::EndianSwapFloat( vTempVec.W, core::MACHINE_LITTLE_ENDIAN );
	}
	
	return(result);
}

/////////////////////////////////////////////////////
/// Function: FileReadLambertBlock
/// Params: [in]buffer, [in]size, [in]count, [in]fileHandle
///
/////////////////////////////////////////////////////
std::size_t file::FileReadLambertBlock( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle )
{
	unsigned int i = 0;
	std::size_t result = 0;

	if( count > 1 )
	{
		for( i = 0; i < count; i++ )
		{
			mdl::mat::LambertShaderBlock *pTmpbuffer = reinterpret_cast<mdl::mat::LambertShaderBlock *>(buffer);

			result += file::FileReadFloat( &pTmpbuffer->diffuseCoeff,		sizeof(float),				1, fileHandle );
			result += file::FileReadVec4D( &pTmpbuffer->colour,				sizeof(math::Vec4),			1, fileHandle );
			result += file::FileReadUInt( &pTmpbuffer->colourIdx,			sizeof(int),				1, fileHandle );
			result += file::FileReadVec4D( &pTmpbuffer->transparency,		sizeof(math::Vec4),			1, fileHandle ); 
			result += file::FileReadUInt( &pTmpbuffer->transparencyIdx,		sizeof(int),				1, fileHandle );
			result += file::FileReadVec4D( &pTmpbuffer->ambientColour,		sizeof(math::Vec4),			1, fileHandle );
			result += file::FileReadUInt( &pTmpbuffer->ambientColourIdx,	sizeof(int),				1, fileHandle );
			result += file::FileReadVec4D( &pTmpbuffer->incandescence,		sizeof(math::Vec4),			1, fileHandle );
			result += file::FileReadUInt( &pTmpbuffer->incandescenceIdx,	sizeof(int),				1, fileHandle );
			result += file::FileReadFloat( &pTmpbuffer->translucenceCoeff,	sizeof(float),				1, fileHandle );
			result += file::FileReadFloat( &pTmpbuffer->glowIntensity,		sizeof(float),				1, fileHandle );

			result += file::FileReadUInt( &pTmpbuffer->bump.hasBump,		sizeof(int),				1, fileHandle );
			result += file::FileReadFloat( &pTmpbuffer->bump.bumpDepth,		sizeof(float),				1, fileHandle );
			result += file::FileReadUInt( &pTmpbuffer->bump.bumpIdx,		sizeof(int),				1, fileHandle );			
		}
	}
	else
	{	
		mdl::mat::LambertShaderBlock *pTmpbuffer = reinterpret_cast<mdl::mat::LambertShaderBlock *>(buffer);

		result += file::FileReadFloat( &pTmpbuffer->diffuseCoeff,		sizeof(float),				1, fileHandle );
		result += file::FileReadVec4D( &pTmpbuffer->colour,				sizeof(math::Vec4),			1, fileHandle );
		result += file::FileReadUInt( &pTmpbuffer->colourIdx,			sizeof(int),				1, fileHandle );
		result += file::FileReadVec4D( &pTmpbuffer->transparency,		sizeof(math::Vec4),			1, fileHandle ); 
		result += file::FileReadUInt( &pTmpbuffer->transparencyIdx,		sizeof(int),				1, fileHandle );
		result += file::FileReadVec4D( &pTmpbuffer->ambientColour,		sizeof(math::Vec4),			1, fileHandle );
		result += file::FileReadUInt( &pTmpbuffer->ambientColourIdx,	sizeof(int),				1, fileHandle );
		result += file::FileReadVec4D( &pTmpbuffer->incandescence,		sizeof(math::Vec4),			1, fileHandle );
		result += file::FileReadUInt( &pTmpbuffer->incandescenceIdx,	sizeof(int),				1, fileHandle );
		result += file::FileReadFloat( &pTmpbuffer->translucenceCoeff,	sizeof(float),				1, fileHandle );
		result += file::FileReadFloat( &pTmpbuffer->glowIntensity,		sizeof(float),				1, fileHandle );

		result += file::FileReadUInt( &pTmpbuffer->bump.hasBump,		sizeof(int),				1, fileHandle );
		result += file::FileReadFloat( &pTmpbuffer->bump.bumpDepth,		sizeof(float),				1, fileHandle );
		result += file::FileReadUInt( &pTmpbuffer->bump.bumpIdx,		sizeof(int),				1, fileHandle );
	}
	
	return(result);
}

/////////////////////////////////////////////////////
/// Function: FileReadReflectBlock
/// Params: [in]buffer, [in]size, [in]count, [in]fileHandle
///
/////////////////////////////////////////////////////
std::size_t file::FileReadReflectBlock( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle )
{
	unsigned int i = 0;
	std::size_t result = 0;

	if( count > 1 )
	{
		for( i = 0; i < count; i++ )
		{
			mdl::mat::ReflectShaderBlock *pTmpbuffer = reinterpret_cast<mdl::mat::ReflectShaderBlock *>(buffer);

			result += file::FileReadUInt( &pTmpbuffer->envmapBlock.hasEnvMap, sizeof(int),	1, fileHandle );
			result += file::FileReadUInt( &pTmpbuffer->envmapBlock.envType, sizeof(int),	1, fileHandle );
			result += file::FileReadUInt( &pTmpbuffer->envmapBlock.envIdx[0], sizeof(int),	1, fileHandle );
			result += file::FileReadUInt( &pTmpbuffer->envmapBlock.envIdx[1], sizeof(int),	1, fileHandle );
			result += file::FileReadUInt( &pTmpbuffer->envmapBlock.envIdx[2], sizeof(int),	1, fileHandle );
			result += file::FileReadUInt( &pTmpbuffer->envmapBlock.envIdx[3], sizeof(int),	1, fileHandle );
			result += file::FileReadUInt( &pTmpbuffer->envmapBlock.envIdx[4], sizeof(int),	1, fileHandle );
			result += file::FileReadUInt( &pTmpbuffer->envmapBlock.envIdx[5], sizeof(int),	1, fileHandle );	
			result += file::FileReadVec4D( &pTmpbuffer->specularColour,		sizeof(math::Vec4),	1, fileHandle ); 
			result += file::FileReadUInt( &pTmpbuffer->specularColourIdx,	sizeof(int),				1, fileHandle ); 
			result += file::FileReadFloat( &pTmpbuffer->reflectivity,		sizeof(float),				1, fileHandle );
			result += file::FileReadVec4D( &pTmpbuffer->reflectedColour,	sizeof(math::Vec4),	1, fileHandle );
		}
	}
	else
	{	
		mdl::mat::ReflectShaderBlock *pTmpbuffer = reinterpret_cast<mdl::mat::ReflectShaderBlock *>(buffer);

		result += file::FileReadUInt( &pTmpbuffer->envmapBlock.hasEnvMap, sizeof(int),	1, fileHandle );
		result += file::FileReadUInt( &pTmpbuffer->envmapBlock.envType, sizeof(int),	1, fileHandle );
		result += file::FileReadUInt( &pTmpbuffer->envmapBlock.envIdx[0], sizeof(int),	1, fileHandle );
		result += file::FileReadUInt( &pTmpbuffer->envmapBlock.envIdx[1], sizeof(int),	1, fileHandle );
		result += file::FileReadUInt( &pTmpbuffer->envmapBlock.envIdx[2], sizeof(int),	1, fileHandle );
		result += file::FileReadUInt( &pTmpbuffer->envmapBlock.envIdx[3], sizeof(int),	1, fileHandle );
		result += file::FileReadUInt( &pTmpbuffer->envmapBlock.envIdx[4], sizeof(int),	1, fileHandle );
		result += file::FileReadUInt( &pTmpbuffer->envmapBlock.envIdx[5], sizeof(int),	1, fileHandle );
		result += file::FileReadVec4D( &pTmpbuffer->specularColour,		sizeof(math::Vec4),	1, fileHandle ); 
		result += file::FileReadUInt( &pTmpbuffer->specularColourIdx,	sizeof(int),				1, fileHandle ); 
		result += file::FileReadFloat( &pTmpbuffer->reflectivity,		sizeof(float),				1, fileHandle );
		result += file::FileReadVec4D( &pTmpbuffer->reflectedColour,	sizeof(math::Vec4),	1, fileHandle );
	}
	
	return(result);
}

/////////////////////////////////////////////////////
/// Function: FileReadPhongBlock
/// Params: [in]buffer, [in]size, [in]count, [in]fileHandle
///
/////////////////////////////////////////////////////
std::size_t file::FileReadPhongBlock( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle )
{
	unsigned int i = 0;
	std::size_t result = 0;

	if( count > 1 )
	{
		for( i = 0; i < count; i++ )
		{
			mdl::mat::PhongShaderBlock *pTmpbuffer = reinterpret_cast<mdl::mat::PhongShaderBlock *>(buffer);

			result += file::FileReadLambertBlock( &pTmpbuffer->lambert, sizeof(mdl::mat::LambertShaderBlock), 1, fileHandle );
			result += file::FileReadReflectBlock( &pTmpbuffer->reflect, sizeof(mdl::mat::ReflectShaderBlock), 1, fileHandle );
			result += file::FileReadFloat( &pTmpbuffer->cosPower, sizeof(float), 1, fileHandle );
		}
	}
	else
	{	
		mdl::mat::PhongShaderBlock *pTmpbuffer = reinterpret_cast<mdl::mat::PhongShaderBlock *>(buffer);

		result += file::FileReadLambertBlock( &pTmpbuffer->lambert, sizeof(mdl::mat::LambertShaderBlock), 1, fileHandle );
		result += file::FileReadReflectBlock( &pTmpbuffer->reflect, sizeof(mdl::mat::ReflectShaderBlock), 1, fileHandle );
		result += file::FileReadFloat( &pTmpbuffer->cosPower, sizeof(float), 1, fileHandle );
	}
	
	return(result);
}

/////////////////////////////////////////////////////
/// Function: FileReadBlinnBlock
/// Params: [in]buffer, [in]size, [in]count, [in]fileHandle
///
/////////////////////////////////////////////////////
std::size_t file::FileReadBlinnBlock( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle )
{
	unsigned int i = 0;
	std::size_t result = 0;

	if( count > 1 )
	{
		for( i = 0; i < count; i++ )
		{
			mdl::mat::BlinnShaderBlock *pTmpbuffer = reinterpret_cast<mdl::mat::BlinnShaderBlock *>(buffer);

			result += file::FileReadLambertBlock( &pTmpbuffer->lambert, sizeof(mdl::mat::LambertShaderBlock), 1, fileHandle );
			result += file::FileReadReflectBlock( &pTmpbuffer->reflect, sizeof(mdl::mat::ReflectShaderBlock), 1, fileHandle );
			result += file::FileReadFloat( &pTmpbuffer->eccentricity, sizeof(float), 1, fileHandle );
			result += file::FileReadFloat( &pTmpbuffer->specularRollOff, sizeof(float), 1, fileHandle );
		}
	}
	else
	{	
		mdl::mat::BlinnShaderBlock *pTmpbuffer = reinterpret_cast<mdl::mat::BlinnShaderBlock *>(buffer);

		result += file::FileReadLambertBlock( &pTmpbuffer->lambert, sizeof(mdl::mat::LambertShaderBlock), 1, fileHandle );
		result += file::FileReadReflectBlock( &pTmpbuffer->reflect, sizeof(mdl::mat::ReflectShaderBlock), 1, fileHandle );
		result += file::FileReadFloat( &pTmpbuffer->eccentricity, sizeof(float), 1, fileHandle );
		result += file::FileReadFloat( &pTmpbuffer->specularRollOff, sizeof(float), 1, fileHandle );
	}
	
	return(result);
}

/////////////////////////////////////////////////////
/// Function: FileReadFileStructure
/// Params: [in]buffer, [in]size, [in]count, [in]fileHandle
///
/////////////////////////////////////////////////////
std::size_t file::FileReadFileStructure( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle )
{
	unsigned int i = 0;
	std::size_t result = 0;

	if( count > 1 )
	{
		for( i = 0; i < count; i++ )
		{
			if( fileHandle->nFileType == FILETYPE_BINARY_READ && fileHandle->nState == FILESTATE_OPEN )
			{
				if( fileHandle->zipped )
				{
					if( fileHandle->zfp )
					{
						result += zzip_fread( buffer, size, 1, fileHandle->zfp );
					}
				}
				else
				if( fileHandle->fp )
				{
#ifdef _DEBUG
					// always check that the file is not read past EOF
					if( feof(fileHandle->fp ) )
					{
						DBGLOG( "FILEIO: *ERROR* reading past the EOF in FileReadFileStructure\n" );
						DBG_ASSERT_MSG( 0, "FILEIO: *ERROR* reading past the EOF in FileReadFileStructure" );
					}
#endif // _DEBUG
					result += fread( buffer, size, 1, fileHandle->fp );
				}
			}
			else if( fileHandle->nFileType == FILETYPE_BINARY_STREAM_READ && fileHandle->nState == FILESTATE_OPEN )
			{
				if( fileHandle->input.is_open() )
				{
					fileHandle->input.read( reinterpret_cast<char *>(buffer), static_cast<std::streamsize>(size) );
					result += static_cast<std::size_t>(fileHandle->input.gcount());
				}
			}
			else
			{
				DBGLOG( "FILEIO: *ERROR* unrecognised read type in FileReadFileStructure\n" );
				return(0);
			}
		}
	}
	else
	{	
	
		if( fileHandle->nFileType == FILETYPE_BINARY_READ && fileHandle->nState == FILESTATE_OPEN )
		{
			if( fileHandle->zipped )
			{
				if( fileHandle->zfp )
				{
					result += zzip_fread( buffer, size, 1, fileHandle->zfp );
				}
			}
			else
			if( fileHandle->fp )
			{
#ifdef _DEBUG
				// always check that the file is not read past EOF
				if( feof(fileHandle->fp ) )
				{
					DBGLOG( "FILEIO: *ERROR* reading past the EOF in FileReadFileStructure\n" );
					DBG_ASSERT_MSG( 0, "FILEIO: *ERROR* reading past the EOF in FileReadFileStructure" );
				}
#endif // _DEBUG
				result += fread( buffer, size, 1, fileHandle->fp );
			}
		}
		else if( fileHandle->nFileType == FILETYPE_BINARY_STREAM_READ && fileHandle->nState == FILESTATE_OPEN )
		{
			if( fileHandle->input.is_open() )
			{
				fileHandle->input.read( reinterpret_cast<char *>(buffer), static_cast<std::streamsize>(size) );
				result += static_cast<std::size_t>(fileHandle->input.gcount());
			}
		}
		else
		{
			DBGLOG( "FILEIO: *ERROR* unrecognised read type in FileReadFileStructure\n" );
			return(0);
		}
	}
	
	return(result);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////
/// Function: FileWrite
/// Params: [in]buffer, [in]size, [in]fileHandle
///
/////////////////////////////////////////////////////
std::size_t file::FileWrite( void *buffer, std::size_t size, TFileHandle* fileHandle )
{
	std::size_t result = 0;

	// char writes are endian safe
	if( fileHandle->nFileType == FILETYPE_BINARY_WRITE && fileHandle->nState == FILESTATE_OPEN )
	{
		if( fileHandle->fp )
		{
			result += fwrite( buffer, size, 1, fileHandle->fp );
			return(result);
		}
	}
	else if( fileHandle->nFileType == FILETYPE_BINARY_STREAM_WRITE && fileHandle->nState == FILESTATE_OPEN )
	{
		if( fileHandle->output.is_open() )
		{
			fileHandle->output.write( reinterpret_cast<char *>(buffer), static_cast<std::streamsize>(size) );
			return(result);
		}
	}
	else
	{
		DBGLOG( "FILEIO: *ERROR* unrecognised write type in FileWrite\n" );
		return(0);
	}

	return(result);
}

/////////////////////////////////////////////////////
/// Function: FileWrite
/// Params: [in]buffer, [in]size, [in]fileHandle
///
/////////////////////////////////////////////////////
std::size_t file::FileWrite( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle )
{
	std::size_t result = 0;

	// char writes are endian safe
	if( (fileHandle->nFileType == FILETYPE_BINARY_WRITE && fileHandle->nState == FILESTATE_OPEN) )
	{
		if( fileHandle->fp )
		{
			result += fwrite( buffer, size, count, fileHandle->fp );
			return(result);
		}
	}
	else
	if( (fileHandle->nFileType == FILETYPE_TEXT_WRITE && fileHandle->nState == FILESTATE_OPEN) )
	{
		if( fileHandle->fp )
		{
			result += fwrite( buffer, size, count, fileHandle->fp );
			return(result);
		}
	}
	else if( fileHandle->nFileType == FILETYPE_BINARY_STREAM_WRITE && fileHandle->nState == FILESTATE_OPEN )
	{
		if( fileHandle->output.is_open() )
		{
			fileHandle->output.write( reinterpret_cast<char *>(buffer), static_cast<std::streamsize>(size*count) );
			return(result);
		}
	}
	else
	{
		DBGLOG( "FILEIO: *ERROR* unrecognised write type in FileWrite\n" );
		return(0);
	}

	return(result);
}

/////////////////////////////////////////////////////
/// Function: FileWriteUChar
/// Params: [in]buffer, [in]size, [in]count, [in]fileHandle
///
/////////////////////////////////////////////////////
std::size_t file::FileWriteUChar( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle )
{
	std::size_t result = 0;

	// char writes are endian safe
	if( fileHandle->nFileType == FILETYPE_BINARY_WRITE && fileHandle->nState == FILESTATE_OPEN )
	{
		if( fileHandle->fp )
		{
			result += fwrite( buffer, size, count, fileHandle->fp );
			return(result);
		}
	}
	else if( fileHandle->nFileType == FILETYPE_BINARY_STREAM_WRITE && fileHandle->nState == FILESTATE_OPEN )
	{
		if( fileHandle->output.is_open() )
		{
			fileHandle->output.write( reinterpret_cast<char *>(buffer), static_cast<std::streamsize>(size*count) );
			return(result);
		}
	}
	else
	{
		DBGLOG( "FILEIO: *ERROR* unrecognised write type in FileWriteUChar\n" );
		return(0);
	}

	return(result);
}

/////////////////////////////////////////////////////
/// Function: FileWriteChar
/// Params: [in]buffer, [in]size, [in]count, [in]fileHandle
///
/////////////////////////////////////////////////////
std::size_t file::FileWriteChar( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle )
{
	std::size_t result = 0;

	// char writes are endian safe
	if( fileHandle->nFileType == FILETYPE_BINARY_WRITE && fileHandle->nState == FILESTATE_OPEN )
	{
		if( fileHandle->fp )
		{
			result += fwrite( buffer, size, count, fileHandle->fp );
			return(result);
		}
	}
	else if( fileHandle->nFileType == FILETYPE_BINARY_STREAM_WRITE && fileHandle->nState == FILESTATE_OPEN )
	{
		if( fileHandle->output.is_open() )
		{
			fileHandle->output.write( reinterpret_cast<char *>(buffer), static_cast<std::streamsize>(size*count) );
			return(result);
		}
	}
	else
	{
		DBGLOG( "FILEIO: *ERROR* unrecognised write type in FileWriteChar\n" );
		return(0);
	}

	return(result);
}

/////////////////////////////////////////////////////
/// Function: FileWriteBool
/// Params: [in]buffer, [in]size, [in]count, [in]fileHandle
///
/////////////////////////////////////////////////////
std::size_t file::FileWriteBool( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle  )
{
	std::size_t result = 0;

	// bool writes are endian safe
	if( fileHandle->nFileType == FILETYPE_BINARY_WRITE && fileHandle->nState == FILESTATE_OPEN )
	{
		if( fileHandle->fp )
		{
			result += fwrite( buffer, size, count, fileHandle->fp );
			return(result);
		}
	}
	else if( fileHandle->nFileType == FILETYPE_BINARY_STREAM_WRITE && fileHandle->nState == FILESTATE_OPEN )
	{
		if( fileHandle->output.is_open() )
		{
			fileHandle->output.write( reinterpret_cast<char *>(buffer), static_cast<std::streamsize>(size*count) );
			return(result);
		}
	}
	else
	{
		DBGLOG( "FILEIO: *ERROR* unrecognised write type in FileWriteBool\n" );
		return(0);
	}

	return(result);
}

/////////////////////////////////////////////////////
/// Function: FileWriteShort
/// Params: [in]buffer, [in]size, [in]count, [in]fileHandle
///
/////////////////////////////////////////////////////
std::size_t file::FileWriteShort( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle  )
{
	unsigned int i = 0;
	std::size_t result = 0;

	short nTempShort = 0;
	
	if( count > 1 )
	{
		for( i = 0; i < count; i++ )
		{
			short *pTmpbuffer = reinterpret_cast<short *>(buffer);

			nTempShort = pTmpbuffer[i];

			nTempShort = core::EndianSwapShort( nTempShort, core::MACHINE_LITTLE_ENDIAN );

			if( fileHandle->nFileType == FILETYPE_BINARY_WRITE && fileHandle->nState == FILESTATE_OPEN )
			{
				if( fileHandle->fp )
					result += fwrite( &nTempShort, size, 1, fileHandle->fp );
			}
			else if( fileHandle->nFileType == FILETYPE_BINARY_STREAM_WRITE && fileHandle->nState == FILESTATE_OPEN )
			{
				if( fileHandle->output.is_open() )
					fileHandle->output.write( reinterpret_cast<char *>(&nTempShort), static_cast<std::streamsize>(size) );
			}
			else
			{
				DBGLOG( "FILEIO: *ERROR* unrecognised write type in FileWriteShort\n" );
				return(0);
			}
		}
	}
	else
	{
		short *pTmpbuffer = reinterpret_cast<short *>(buffer);

		nTempShort = *pTmpbuffer;

		nTempShort = core::EndianSwapShort( nTempShort, core::MACHINE_LITTLE_ENDIAN );
		
		if( fileHandle->nFileType == FILETYPE_BINARY_WRITE && fileHandle->nState == FILESTATE_OPEN )
		{
			if( fileHandle->fp )
				result += fwrite( &nTempShort, size, 1, fileHandle->fp );
		}
		else if( fileHandle->nFileType == FILETYPE_BINARY_STREAM_WRITE && fileHandle->nState == FILESTATE_OPEN )
		{
			if( fileHandle->output.is_open() )
				fileHandle->output.write( reinterpret_cast<char *>(&nTempShort), static_cast<std::streamsize>(size) );
		}
		else
		{
			DBGLOG( "FILEIO: *ERROR* unrecognised write type in FileWriteShort\n" );
			return(0);
		}

	}
	
	return(result);
}

/////////////////////////////////////////////////////
/// Function: FileWriteUShort
/// Params: [in]buffer, [in]size, [in]count, [in]fileHandle
///
/////////////////////////////////////////////////////
std::size_t file::FileWriteUShort( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle  )
{
	unsigned int i = 0;
	std::size_t result = 0;

	unsigned short nTempShort = 0;
	
	if( count > 1 )
	{
		for( i = 0; i < count; i++ )
		{
			unsigned short *pTmpbuffer = reinterpret_cast<unsigned short *>(buffer);

			nTempShort = pTmpbuffer[i];

			nTempShort = core::EndianSwapShort( nTempShort, core::MACHINE_LITTLE_ENDIAN );

			if( fileHandle->nFileType == FILETYPE_BINARY_WRITE && fileHandle->nState == FILESTATE_OPEN )
			{
				if( fileHandle->fp )
					result += fwrite( &nTempShort, size, 1, fileHandle->fp );
			}
			else if( fileHandle->nFileType == FILETYPE_BINARY_STREAM_WRITE && fileHandle->nState == FILESTATE_OPEN )
			{
				if( fileHandle->output.is_open() )
					fileHandle->output.write( reinterpret_cast<char *>(&nTempShort), static_cast<std::streamsize>(size) );
			}
			else
			{
				DBGLOG( "FILEIO: *ERROR* unrecognised write type in FileWriteUShort\n" );
				return(0);
			}
		}
	}
	else
	{
		unsigned short *pTmpbuffer = reinterpret_cast<unsigned short *>(buffer);

		nTempShort = *pTmpbuffer;

		nTempShort = core::EndianSwapShort( nTempShort, core::MACHINE_LITTLE_ENDIAN );
		
		if( fileHandle->nFileType == FILETYPE_BINARY_WRITE && fileHandle->nState == FILESTATE_OPEN )
		{
			if( fileHandle->fp )
				result += fwrite( &nTempShort, size, 1, fileHandle->fp );
		}
		else if( fileHandle->nFileType == FILETYPE_BINARY_STREAM_WRITE && fileHandle->nState == FILESTATE_OPEN )
		{
			if( fileHandle->output.is_open() )
				fileHandle->output.write( reinterpret_cast<char *>(&nTempShort), static_cast<std::streamsize>(size) );
		}
		else
		{
			DBGLOG( "FILEIO: *ERROR* unrecognised write type in FileWriteUShort\n" );
			return(0);
		}

	}
	
	return(result);
}

/////////////////////////////////////////////////////
/// Function: FileWriteInt
/// Params: [in]buffer, [in]size, [in]count, [in]fileHandle
///
/////////////////////////////////////////////////////
std::size_t file::FileWriteInt( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle )
{
	unsigned int i = 0;
	std::size_t result = 0;

	int nTempInt = 0;

	if( count > 1 )
	{
		for( i = 0; i < count; i++ )
		{
			int *pTmpbuffer = reinterpret_cast<int *>(buffer);

			nTempInt = pTmpbuffer[i];

			nTempInt = core::EndianSwapInt( nTempInt, core::MACHINE_LITTLE_ENDIAN );

			if( fileHandle->nFileType == FILETYPE_BINARY_WRITE && fileHandle->nState == FILESTATE_OPEN )
			{
				if( fileHandle->fp )
					result += fwrite( &nTempInt, size, 1, fileHandle->fp );
			}
			else if( fileHandle->nFileType == FILETYPE_BINARY_STREAM_WRITE && fileHandle->nState == FILESTATE_OPEN )
			{
				if( fileHandle->output.is_open() )
					fileHandle->output.write( reinterpret_cast<char *>(&nTempInt), static_cast<std::streamsize>(size) );
			}
			else
			{
				DBGLOG( "FILEIO: *ERROR* unrecognised write type in FileWriteInt\n" );
				return(0);
			}
		}
	}
	else
	{
		int *pTmpbuffer = reinterpret_cast<int *>(buffer);

		nTempInt = *pTmpbuffer;

		nTempInt = core::EndianSwapInt( nTempInt, core::MACHINE_LITTLE_ENDIAN );

		if( fileHandle->nFileType == FILETYPE_BINARY_WRITE && fileHandle->nState == FILESTATE_OPEN )
		{
			if( fileHandle->fp )
				result += fwrite( &nTempInt, size, 1, fileHandle->fp );
		}
		else if( fileHandle->nFileType == FILETYPE_BINARY_STREAM_WRITE && fileHandle->nState == FILESTATE_OPEN )
		{
			if( fileHandle->output.is_open() )
				fileHandle->output.write( reinterpret_cast<char *>(&nTempInt), static_cast<std::streamsize>(size) );
		}
		else
		{
			DBGLOG( "FILEIO: *ERROR* unrecognised write type in FileWriteInt\n" );
			return(0);
		}
	}
	
	return(result);
}

/////////////////////////////////////////////////////
/// Function: FileWriteUInt
/// Params: [in]buffer, [in]size, [in]count, [in]fileHandle
///
/////////////////////////////////////////////////////
std::size_t file::FileWriteUInt( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle  )
{
	unsigned int i = 0;
	std::size_t result = 0;

	unsigned int nTempInt = 0;

	if( count > 1 )
	{
		for( i = 0; i < count; i++ )
		{
			unsigned int *pTmpbuffer = reinterpret_cast<unsigned int *>(buffer);

			nTempInt = pTmpbuffer[i];

			nTempInt = core::EndianSwapInt( nTempInt, core::MACHINE_LITTLE_ENDIAN );

			if( fileHandle->nFileType == FILETYPE_BINARY_WRITE && fileHandle->nState == FILESTATE_OPEN )
			{
				if( fileHandle->fp )
					result += fwrite( &nTempInt, size, 1, fileHandle->fp );
			}
			else if( fileHandle->nFileType == FILETYPE_BINARY_STREAM_WRITE && fileHandle->nState == FILESTATE_OPEN )
			{
				if( fileHandle->output.is_open() )
					fileHandle->output.write( reinterpret_cast<char *>(&nTempInt), static_cast<std::streamsize>(size) );
			}
			else
			{
				DBGLOG( "FILEIO: *ERROR* unrecognised write type in FileWriteUInt\n" );
				return(0);
			}
		}
	}
	else
	{
		unsigned int *pTmpbuffer = reinterpret_cast<unsigned int *>(buffer);

		nTempInt = *pTmpbuffer;

		nTempInt = core::EndianSwapInt( nTempInt, core::MACHINE_LITTLE_ENDIAN );

		if( fileHandle->nFileType == FILETYPE_BINARY_WRITE && fileHandle->nState == FILESTATE_OPEN )
		{
			if( fileHandle->fp )
				result += fwrite( &nTempInt, size, 1, fileHandle->fp );
		}
		else if( fileHandle->nFileType == FILETYPE_BINARY_STREAM_WRITE && fileHandle->nState == FILESTATE_OPEN )
		{
			if( fileHandle->output.is_open() )
				fileHandle->output.write( reinterpret_cast<char *>(&nTempInt), static_cast<std::streamsize>(size) );
		}
		else
		{
			DBGLOG( "FILEIO: *ERROR* unrecognised write type in FileWriteUInt\n" );
			return(0);
		}
	}
	
	return(result);
}

/////////////////////////////////////////////////////
/// Function: FileWriteFloat
/// Params: [in]buffer, [in]size, [in]count, [in]fileHandle
///
/////////////////////////////////////////////////////
std::size_t file::FileWriteFloat( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle )
{
	unsigned int i = 0;
	std::size_t result = 0;

	float fTempFloat = 0.0f;

	if( count > 1 )
	{
		for( i = 0; i < count; i++ )
		{
			float *pTmpbuffer = reinterpret_cast<float *>(buffer);

			fTempFloat = pTmpbuffer[i];

			fTempFloat = core::EndianSwapFloat( fTempFloat, core::MACHINE_LITTLE_ENDIAN );

			if( fileHandle->nFileType == FILETYPE_BINARY_WRITE && fileHandle->nState == FILESTATE_OPEN )
			{
				if( fileHandle->fp )
					result += fwrite( &fTempFloat, size, 1, fileHandle->fp );
			}
			else if( fileHandle->nFileType == FILETYPE_BINARY_STREAM_WRITE && fileHandle->nState == FILESTATE_OPEN )
			{
				if( fileHandle->output.is_open() )
					fileHandle->output.write( reinterpret_cast<char *>(&fTempFloat), static_cast<std::streamsize>(size) );
			}
			else
			{
				DBGLOG( "FILEIO: *ERROR* unrecognised write type in FileWriteFloat\n" );
				return(0);
			}
		}
	}
	else
	{
		float *pTmpbuffer = reinterpret_cast<float *>(buffer);

		fTempFloat = *pTmpbuffer;

		fTempFloat = core::EndianSwapFloat( fTempFloat, core::MACHINE_LITTLE_ENDIAN );

		if( fileHandle->nFileType == FILETYPE_BINARY_WRITE && fileHandle->nState == FILESTATE_OPEN )
		{
			if( fileHandle->fp )
				result += fwrite( &fTempFloat, size, 1, fileHandle->fp );
		}
		else if( fileHandle->nFileType == FILETYPE_BINARY_STREAM_WRITE && fileHandle->nState == FILESTATE_OPEN )
		{
			if( fileHandle->output.is_open() )
				fileHandle->output.write( reinterpret_cast<char *>(&fTempFloat), static_cast<std::streamsize>(size) );
		}
		else
		{
			DBGLOG( "FILEIO: *ERROR* unrecognised write type in FileWriteFloat\n" );
			return(0);
		}
	}
	
	return(result);
}

/////////////////////////////////////////////////////
/// Function: FileWriteVec2D
/// Params: [in]buffer, [in]size, [in]count, [in]fileHandle
///
/////////////////////////////////////////////////////
std::size_t file::FileWriteVec2D( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle )
{
	unsigned int i = 0;
	std::size_t result = 0;

	math::Vec2 vTempVec( 0.0f, 0.0f );

	if( count > 1 )
	{
		for( i = 0; i < count; i++ )
		{
			
			math::Vec2 *pTmpbuffer = reinterpret_cast<math::Vec2 *>(buffer);

			vTempVec.X = pTmpbuffer[i].X;
			vTempVec.Y = pTmpbuffer[i].Y;

			vTempVec.X = core::EndianSwapFloat( vTempVec.X, core::MACHINE_LITTLE_ENDIAN );
			vTempVec.Y = core::EndianSwapFloat( vTempVec.Y, core::MACHINE_LITTLE_ENDIAN );

			if( fileHandle->nFileType == FILETYPE_BINARY_WRITE && fileHandle->nState == FILESTATE_OPEN )
			{
				if( fileHandle->fp )
					result += fwrite( &vTempVec, size, 1, fileHandle->fp );
			}
			else if( fileHandle->nFileType == FILETYPE_BINARY_STREAM_WRITE && fileHandle->nState == FILESTATE_OPEN )
			{
				if( fileHandle->output.is_open() )
					fileHandle->output.write( reinterpret_cast<char *>(&vTempVec), static_cast<std::streamsize>(size) );
			}
			else
			{
				DBGLOG( "FILEIO: *ERROR* unrecognised write type in FileWriteVec2D\n" );
				return(0);
			}
		}
	}
	else
	{
		math::Vec2 *pTmpbuffer = reinterpret_cast<math::Vec2 *>(buffer);

		vTempVec.X = pTmpbuffer->X;
		vTempVec.Y = pTmpbuffer->Y;

		vTempVec.X = core::EndianSwapFloat( vTempVec.X, core::MACHINE_LITTLE_ENDIAN );
		vTempVec.Y = core::EndianSwapFloat( vTempVec.Y, core::MACHINE_LITTLE_ENDIAN );

		if( fileHandle->nFileType == FILETYPE_BINARY_WRITE && fileHandle->nState == FILESTATE_OPEN )
		{
			if( fileHandle->fp )
				result += fwrite( &vTempVec, size, 1, fileHandle->fp );
		}
		else if( fileHandle->nFileType == FILETYPE_BINARY_STREAM_WRITE && fileHandle->nState == FILESTATE_OPEN )
		{
			if( fileHandle->output.is_open() )
				fileHandle->output.write( reinterpret_cast<char *>(&vTempVec), static_cast<std::streamsize>(size) );
		}
		else
		{
			DBGLOG( "FILEIO: *ERROR* unrecognised write type in FileWriteVec2D\n" );
			return(0);
		}
	}
	
	return(result);
}

/////////////////////////////////////////////////////
/// Function: FileWriteVec3D
/// Params: [in]buffer, [in]size, [in]count, [in]fileHandle
///
/////////////////////////////////////////////////////
std::size_t file::FileWriteVec3D( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle )
{
	unsigned int i = 0;
	std::size_t result = 0;
			
	math::Vec3 vTempVec( 0.0f, 0.0f, 0.0f );

	if( count > 1 )
	{
		for( i = 0; i < count; i++ )
		{

			math::Vec3 *pTmpbuffer = reinterpret_cast<math::Vec3 *>(buffer);

			vTempVec.X = pTmpbuffer[i].X;
			vTempVec.Y = pTmpbuffer[i].Y;
			vTempVec.Z = pTmpbuffer[i].Z;

			vTempVec.X = core::EndianSwapFloat( vTempVec.X, core::MACHINE_LITTLE_ENDIAN );
			vTempVec.Y = core::EndianSwapFloat( vTempVec.Y, core::MACHINE_LITTLE_ENDIAN );
			vTempVec.Z = core::EndianSwapFloat( vTempVec.Z, core::MACHINE_LITTLE_ENDIAN );

			if( fileHandle->nFileType == FILETYPE_BINARY_WRITE && fileHandle->nState == FILESTATE_OPEN )
			{
				if( fileHandle->fp )
					result += fwrite( &vTempVec, size, 1, fileHandle->fp );
			}
			else if( fileHandle->nFileType == FILETYPE_BINARY_STREAM_WRITE && fileHandle->nState == FILESTATE_OPEN )
			{
				if( fileHandle->output.is_open() )
					fileHandle->output.write( reinterpret_cast<char *>(&vTempVec), static_cast<std::streamsize>(size) );
			}
			else
			{
				DBGLOG( "FILEIO: *ERROR* unrecognised write type in FileWriteVec3D\n" );
				return(0);
			}		
		}
	}
	else
	{	
		math::Vec3 *pTmpbuffer = reinterpret_cast<math::Vec3 *>(buffer);

		vTempVec.X = pTmpbuffer->X;
		vTempVec.Y = pTmpbuffer->Y;
		vTempVec.Z = pTmpbuffer->Z;

		vTempVec.X = core::EndianSwapFloat( vTempVec.X, core::MACHINE_LITTLE_ENDIAN );
		vTempVec.Y = core::EndianSwapFloat( vTempVec.Y, core::MACHINE_LITTLE_ENDIAN );
		vTempVec.Z = core::EndianSwapFloat( vTempVec.Z, core::MACHINE_LITTLE_ENDIAN );	

		if( fileHandle->nFileType == FILETYPE_BINARY_WRITE && fileHandle->nState == FILESTATE_OPEN )
		{
			if( fileHandle->fp )
				result += fwrite( &vTempVec, size, 1, fileHandle->fp );
		}
		else if( fileHandle->nFileType == FILETYPE_BINARY_STREAM_WRITE && fileHandle->nState == FILESTATE_OPEN )
		{
			if( fileHandle->output.is_open() )
				fileHandle->output.write( reinterpret_cast<char *>(&vTempVec), static_cast<std::streamsize>(size) );
		}
		else
		{
			DBGLOG( "FILEIO: *ERROR* unrecognised write type in FileWriteVec3D\n" );
			return(0);
		}
	}
	
	return(result);
}

/////////////////////////////////////////////////////
/// Function: FileWriteVec4D
/// Params: [in]buffer, [in]size, [in]count, [in]fileHandle
///
/////////////////////////////////////////////////////
std::size_t file::FileWriteVec4D( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle )
{
	unsigned int i = 0;
	std::size_t result = 0;

	math::Vec4 vTempVec( 0.0f, 0.0f, 0.0f, 0.0f );

	if( count > 1 )
	{
		for( i = 0; i < count; i++ )
		{
			math::Vec4 *pTmpbuffer = reinterpret_cast<math::Vec4 *>(buffer);

			vTempVec.X = pTmpbuffer[i].X;
			vTempVec.Y = pTmpbuffer[i].Y;
			vTempVec.Z = pTmpbuffer[i].Z;
			vTempVec.W = pTmpbuffer[i].W;

			vTempVec.X = core::EndianSwapFloat( vTempVec.X, core::MACHINE_LITTLE_ENDIAN );
			vTempVec.Y = core::EndianSwapFloat( vTempVec.Y, core::MACHINE_LITTLE_ENDIAN );
			vTempVec.Z = core::EndianSwapFloat( vTempVec.Z, core::MACHINE_LITTLE_ENDIAN );
			vTempVec.W = core::EndianSwapFloat( vTempVec.W, core::MACHINE_LITTLE_ENDIAN );

			if( fileHandle->nFileType == FILETYPE_BINARY_WRITE && fileHandle->nState == FILESTATE_OPEN )
			{
				if( fileHandle->fp )
					result += fwrite( &vTempVec, size, 1, fileHandle->fp );
			}
			else if( fileHandle->nFileType == FILETYPE_BINARY_STREAM_WRITE && fileHandle->nState == FILESTATE_OPEN )
			{
				if( fileHandle->output.is_open() )
					fileHandle->output.write( reinterpret_cast<char *>(&vTempVec), static_cast<std::streamsize>(size) );
			}
			else
			{
				DBGLOG( "FILEIO: *ERROR* unrecognised write type in FileWriteVec4D\n" );
				return(0);
			}		
		}
	}
	else
	{	
		math::Vec4 *pTmpbuffer = reinterpret_cast<math::Vec4 *>(buffer);

		vTempVec.X = pTmpbuffer->X;
		vTempVec.Y = pTmpbuffer->Y;
		vTempVec.Z = pTmpbuffer->Z;
		vTempVec.W = pTmpbuffer->W;

		vTempVec.X = core::EndianSwapFloat( vTempVec.X, core::MACHINE_LITTLE_ENDIAN );
		vTempVec.Y = core::EndianSwapFloat( vTempVec.Y, core::MACHINE_LITTLE_ENDIAN );
		vTempVec.Z = core::EndianSwapFloat( vTempVec.Z, core::MACHINE_LITTLE_ENDIAN );
		vTempVec.W = core::EndianSwapFloat( vTempVec.W, core::MACHINE_LITTLE_ENDIAN );

		if( fileHandle->nFileType == FILETYPE_BINARY_WRITE && fileHandle->nState == FILESTATE_OPEN )
		{
			if( fileHandle->fp )
				result += fwrite( &vTempVec, size, 1, fileHandle->fp );
		}
		else if( fileHandle->nFileType == FILETYPE_BINARY_STREAM_WRITE && fileHandle->nState == FILESTATE_OPEN )
		{
			if( fileHandle->output.is_open() )
				fileHandle->output.write( reinterpret_cast<char *>(&vTempVec), static_cast<std::streamsize>(size) );
		}
		else
		{
			DBGLOG( "FILEIO: *ERROR* unrecognised write type in FileWriteVec4D\n" );
			return(0);
		}
	}
	
	return(result);
}

/////////////////////////////////////////////////////
/// Function: FileWriteLambertBlock
/// Params: [in]buffer, [in]size, [in]count, [in]fileHandle
///
/////////////////////////////////////////////////////
std::size_t file::FileWriteLambertBlock( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle )
{
	unsigned int i = 0;
	std::size_t result = 0;

	if( count > 1 )
	{
		for( i = 0; i < count; i++ )
		{
			mdl::mat::LambertShaderBlock *pTmpbuffer = reinterpret_cast<mdl::mat::LambertShaderBlock *>(buffer);

			result += file::FileWriteFloat( &pTmpbuffer->diffuseCoeff,		sizeof(float),				1, fileHandle );
			result += file::FileWriteVec4D( &pTmpbuffer->colour,			sizeof(math::Vec4),			1, fileHandle );
			result += file::FileWriteUInt( &pTmpbuffer->colourIdx,			sizeof(int),				1, fileHandle );
			result += file::FileWriteVec4D( &pTmpbuffer->transparency,		sizeof(math::Vec4),			1, fileHandle ); 
			result += file::FileWriteUInt( &pTmpbuffer->transparencyIdx,	sizeof(int),				1, fileHandle );
			result += file::FileWriteVec4D( &pTmpbuffer->ambientColour,		sizeof(math::Vec4),			1, fileHandle );
			result += file::FileWriteUInt( &pTmpbuffer->ambientColourIdx,	sizeof(int),				1, fileHandle );
			result += file::FileWriteVec4D( &pTmpbuffer->incandescence,		sizeof(math::Vec4),			1, fileHandle );
			result += file::FileWriteUInt( &pTmpbuffer->incandescenceIdx,	sizeof(int),				1, fileHandle );
			result += file::FileWriteFloat( &pTmpbuffer->translucenceCoeff, sizeof(float),				1, fileHandle );
			result += file::FileWriteFloat( &pTmpbuffer->glowIntensity,		sizeof(float),				1, fileHandle );

			result += file::FileWriteUInt( &pTmpbuffer->bump.hasBump,		sizeof(int),				1, fileHandle );
			result += file::FileWriteFloat( &pTmpbuffer->bump.bumpDepth,	sizeof(float),				1, fileHandle );
			result += file::FileWriteUInt( &pTmpbuffer->bump.bumpIdx,		sizeof(int),				1, fileHandle );		
		}
	}
	else
	{	
		mdl::mat::LambertShaderBlock *pTmpbuffer = reinterpret_cast<mdl::mat::LambertShaderBlock *>(buffer);

		result += file::FileWriteFloat( &pTmpbuffer->diffuseCoeff,		sizeof(float),				1, fileHandle );
		result += file::FileWriteVec4D( &pTmpbuffer->colour,			sizeof(math::Vec4),			1, fileHandle );
		result += file::FileWriteUInt( &pTmpbuffer->colourIdx,			sizeof(int),				1, fileHandle );
		result += file::FileWriteVec4D( &pTmpbuffer->transparency,		sizeof(math::Vec4),			1, fileHandle ); 
		result += file::FileWriteUInt( &pTmpbuffer->transparencyIdx,	sizeof(int),				1, fileHandle );
		result += file::FileWriteVec4D( &pTmpbuffer->ambientColour,		sizeof(math::Vec4),			1, fileHandle );
		result += file::FileWriteUInt( &pTmpbuffer->ambientColourIdx,	sizeof(int),				1, fileHandle );
		result += file::FileWriteVec4D( &pTmpbuffer->incandescence,		sizeof(math::Vec4),			1, fileHandle );
		result += file::FileWriteUInt( &pTmpbuffer->incandescenceIdx,	sizeof(int),				1, fileHandle );
		result += file::FileWriteFloat( &pTmpbuffer->translucenceCoeff, sizeof(float),				1, fileHandle );
		result += file::FileWriteFloat( &pTmpbuffer->glowIntensity,		sizeof(float),				1, fileHandle );

		result += file::FileWriteUInt( &pTmpbuffer->bump.hasBump,		sizeof(int),				1, fileHandle );
		result += file::FileWriteFloat( &pTmpbuffer->bump.bumpDepth,	sizeof(float),				1, fileHandle );
		result += file::FileWriteUInt( &pTmpbuffer->bump.bumpIdx,		sizeof(int),				1, fileHandle );	
	}
	
	return(result);
}

/////////////////////////////////////////////////////
/// Function: FileWriteReflectBlock
/// Params: [in]buffer, [in]size, [in]count, [in]fileHandle
///
/////////////////////////////////////////////////////
std::size_t file::FileWriteReflectBlock( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle )
{
	unsigned int i = 0;
	std::size_t result = 0;

	if( count > 1 )
	{
		for( i = 0; i < count; i++ )
		{
			mdl::mat::ReflectShaderBlock *pTmpbuffer = reinterpret_cast<mdl::mat::ReflectShaderBlock *>(buffer);

			result += file::FileWriteUInt( &pTmpbuffer->envmapBlock.hasEnvMap, sizeof(int),				1, fileHandle );
			result += file::FileWriteUInt( &pTmpbuffer->envmapBlock.envType, sizeof(int),				1, fileHandle );
			result += file::FileWriteUInt( &pTmpbuffer->envmapBlock.envIdx[0], sizeof(int),				1, fileHandle );
			result += file::FileWriteUInt( &pTmpbuffer->envmapBlock.envIdx[1], sizeof(int),				1, fileHandle );
			result += file::FileWriteUInt( &pTmpbuffer->envmapBlock.envIdx[2], sizeof(int),				1, fileHandle );
			result += file::FileWriteUInt( &pTmpbuffer->envmapBlock.envIdx[3], sizeof(int),				1, fileHandle );
			result += file::FileWriteUInt( &pTmpbuffer->envmapBlock.envIdx[4], sizeof(int),				1, fileHandle );
			result += file::FileWriteUInt( &pTmpbuffer->envmapBlock.envIdx[5], sizeof(int),				1, fileHandle );

			result += file::FileWriteVec4D( &pTmpbuffer->specularColour,	sizeof(math::Vec4),			1, fileHandle ); 
			result += file::FileWriteUInt( &pTmpbuffer->specularColourIdx,	sizeof(int),				1, fileHandle ); 
			result += file::FileWriteFloat( &pTmpbuffer->reflectivity,		sizeof(float),				1, fileHandle );
			result += file::FileWriteVec4D( &pTmpbuffer->reflectedColour,	sizeof(math::Vec4),			1, fileHandle ); 		
		}
	}
	else
	{	
		mdl::mat::ReflectShaderBlock *pTmpbuffer = reinterpret_cast<mdl::mat::ReflectShaderBlock *>(buffer);

		result += file::FileWriteUInt( &pTmpbuffer->envmapBlock.hasEnvMap, sizeof(int),				1, fileHandle );
		result += file::FileWriteUInt( &pTmpbuffer->envmapBlock.envType, sizeof(int),				1, fileHandle );
		result += file::FileWriteUInt( &pTmpbuffer->envmapBlock.envIdx[0], sizeof(int),				1, fileHandle );
		result += file::FileWriteUInt( &pTmpbuffer->envmapBlock.envIdx[1], sizeof(int),				1, fileHandle );
		result += file::FileWriteUInt( &pTmpbuffer->envmapBlock.envIdx[2], sizeof(int),				1, fileHandle );
		result += file::FileWriteUInt( &pTmpbuffer->envmapBlock.envIdx[3], sizeof(int),				1, fileHandle );
		result += file::FileWriteUInt( &pTmpbuffer->envmapBlock.envIdx[4], sizeof(int),				1, fileHandle );
		result += file::FileWriteUInt( &pTmpbuffer->envmapBlock.envIdx[5], sizeof(int),				1, fileHandle );

		result += file::FileWriteVec4D( &pTmpbuffer->specularColour,	sizeof(math::Vec4),			1, fileHandle ); 
		result += file::FileWriteUInt( &pTmpbuffer->specularColourIdx,	sizeof(int),				1, fileHandle ); 
		result += file::FileWriteFloat( &pTmpbuffer->reflectivity,		sizeof(float),				1, fileHandle );
		result += file::FileWriteVec4D( &pTmpbuffer->reflectedColour,	sizeof(math::Vec4),			1, fileHandle ); 	
	}
	
	return(result);
}

/////////////////////////////////////////////////////
/// Function: FileWritePhongBlock
/// Params: [in]buffer, [in]size, [in]count, [in]fileHandle
///
/////////////////////////////////////////////////////
std::size_t file::FileWritePhongBlock( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle )
{
	unsigned int i = 0;
	std::size_t result = 0;

	if( count > 1 )
	{
		for( i = 0; i < count; i++ )
		{
			mdl::mat::PhongShaderBlock *pTmpbuffer = reinterpret_cast<mdl::mat::PhongShaderBlock *>(buffer);

			result += file::FileWriteLambertBlock( &pTmpbuffer->lambert, sizeof(mdl::mat::LambertShaderBlock), 1, fileHandle );
			result += file::FileWriteReflectBlock( &pTmpbuffer->reflect, sizeof(mdl::mat::ReflectShaderBlock), 1, fileHandle );
			result += file::FileWriteFloat( &pTmpbuffer->cosPower, sizeof(float), 1, fileHandle );
		}
	}
	else
	{	
		mdl::mat::PhongShaderBlock *pTmpbuffer = reinterpret_cast<mdl::mat::PhongShaderBlock *>(buffer);

		result += file::FileWriteLambertBlock( &pTmpbuffer->lambert, sizeof(mdl::mat::LambertShaderBlock), 1, fileHandle );
		result += file::FileWriteReflectBlock( &pTmpbuffer->reflect, sizeof(mdl::mat::ReflectShaderBlock), 1, fileHandle );
		result += file::FileWriteFloat( &pTmpbuffer->cosPower, sizeof(float), 1, fileHandle );	
	}
	
	return(result);
}

/////////////////////////////////////////////////////
/// Function: FileWriteBlinnBlock
/// Params: [in]buffer, [in]size, [in]count, [in]fileHandle
///
/////////////////////////////////////////////////////
std::size_t file::FileWriteBlinnBlock( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle )
{
	unsigned int i = 0;
	std::size_t result = 0;

	if( count > 1 )
	{
		for( i = 0; i < count; i++ )
		{
			mdl::mat::BlinnShaderBlock *pTmpbuffer = reinterpret_cast<mdl::mat::BlinnShaderBlock *>(buffer);

			result += file::FileWriteLambertBlock( &pTmpbuffer->lambert, sizeof(mdl::mat::LambertShaderBlock), 1, fileHandle );
			result += file::FileWriteReflectBlock( &pTmpbuffer->reflect, sizeof(mdl::mat::ReflectShaderBlock), 1, fileHandle );
			result += file::FileWriteFloat( &pTmpbuffer->eccentricity, sizeof(float), 1, fileHandle );
			result += file::FileWriteFloat( &pTmpbuffer->specularRollOff, sizeof(float), 1, fileHandle );
		}
	}
	else
	{	
		mdl::mat::BlinnShaderBlock *pTmpbuffer = reinterpret_cast<mdl::mat::BlinnShaderBlock *>(buffer);

		result += file::FileWriteLambertBlock( &pTmpbuffer->lambert, sizeof(mdl::mat::LambertShaderBlock), 1, fileHandle );
		result += file::FileWriteReflectBlock( &pTmpbuffer->reflect, sizeof(mdl::mat::ReflectShaderBlock), 1, fileHandle );
		result += file::FileWriteFloat( &pTmpbuffer->eccentricity, sizeof(float), 1, fileHandle );
		result += file::FileWriteFloat( &pTmpbuffer->specularRollOff, sizeof(float), 1, fileHandle );
	}
	
	return(result);
}

/////////////////////////////////////////////////////
/// Function: FileWriteFileStructure
/// Params: [in]buffer, [in]size, [in]count, [in]fileHandle
///
/////////////////////////////////////////////////////
std::size_t file::FileWriteFileStructure( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle )
{
	unsigned int i = 0;
	std::size_t result = 0;

	if( count > 1 )
	{
		for( i = 0; i < count; i++ )
		{
			if( fileHandle->nFileType == FILETYPE_BINARY_WRITE && fileHandle->nState == FILESTATE_OPEN )
			{
				if( fileHandle->fp )
					result += fwrite( buffer, size, 1, fileHandle->fp );
			}
			else if( fileHandle->nFileType == FILETYPE_BINARY_STREAM_WRITE && fileHandle->nState == FILESTATE_OPEN )
			{
				if( fileHandle->output.is_open() )
					fileHandle->output.write( reinterpret_cast<char *>(buffer), static_cast<std::streamsize>(size) );
			}
			else
			{
				DBGLOG( "FILEIO: *ERROR* unrecognised write type in FileWriteFileStructure\n" );
				return(0);
			}		
		}
	}
	else
	{	
		if( fileHandle->nFileType == FILETYPE_BINARY_WRITE && fileHandle->nState == FILESTATE_OPEN )
		{
			if( fileHandle->fp )
				result += fwrite( buffer, size, 1, fileHandle->fp );
		}
		else if( fileHandle->nFileType == FILETYPE_BINARY_STREAM_WRITE && fileHandle->nState == FILESTATE_OPEN )
		{
			if( fileHandle->output.is_open() )
				fileHandle->output.write( reinterpret_cast<char *>(buffer), static_cast<std::streamsize>(size) );
		}
		else
		{
			DBGLOG( "FILEIO: *ERROR* unrecognised write type in FileWriteFileStructure\n" );
			return(0);
		}
	}
	
	return(result);
}

/////////////////////////////////////////////////////
/// Function: FileWriteFileStore
/// Params: [in]buffer, [in]size, [in]count, [in]fileHandle
///
/////////////////////////////////////////////////////
std::size_t file::FileWriteFileStore( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle )
{
	unsigned int i = 0;
	std::size_t result = 0;

	file::TFileStoreInformation tmpFileStore;
	std::memset( &tmpFileStore, 0, sizeof(file::TFileStoreInformation) );

	if( count > 1 )
	{
		for( i = 0; i < count; i++ )
		{
			file::TFileStoreInformation *pTmpbuffer = reinterpret_cast<file::TFileStoreInformation *>(buffer);

			tmpFileStore.nFileStartPos = pTmpbuffer[i].nFileStartPos;
			tmpFileStore.nFileEndPos = pTmpbuffer[i].nFileEndPos;
			tmpFileStore.nFileLength = pTmpbuffer[i].nFileLength;

			tmpFileStore.nFileStartPos = core::EndianSwapInt( tmpFileStore.nFileStartPos, core::MACHINE_LITTLE_ENDIAN );
			tmpFileStore.nFileEndPos = core::EndianSwapInt( tmpFileStore.nFileEndPos, core::MACHINE_LITTLE_ENDIAN );
			tmpFileStore.nFileLength = static_cast<std::size_t>( core::EndianSwapInt( static_cast<unsigned int>(tmpFileStore.nFileLength), core::MACHINE_LITTLE_ENDIAN ) );

			if( fileHandle->nFileType == FILETYPE_BINARY_WRITE && fileHandle->nState == FILESTATE_OPEN )
			{
				if( fileHandle->fp )
					result += fwrite( &tmpFileStore, size, 1, fileHandle->fp );
			}
			else if( fileHandle->nFileType == FILETYPE_BINARY_STREAM_WRITE && fileHandle->nState == FILESTATE_OPEN )
			{
				if( fileHandle->output.is_open() )
					fileHandle->output.write( reinterpret_cast<char *>(&tmpFileStore), static_cast<std::streamsize>(size) );
			}
			else
			{
				DBGLOG( "FILEIO: *ERROR* unrecognised write type in FileWriteFileStore\n" );
				return(0);
			}		
		}
	}
	else
	{	
		file::TFileStoreInformation *pTmpbuffer = reinterpret_cast<file::TFileStoreInformation *>(buffer);

		tmpFileStore.nFileStartPos = pTmpbuffer->nFileStartPos;
		tmpFileStore.nFileEndPos = pTmpbuffer->nFileEndPos;
		tmpFileStore.nFileLength = pTmpbuffer->nFileLength;

		tmpFileStore.nFileStartPos = core::EndianSwapInt( tmpFileStore.nFileStartPos, core::MACHINE_LITTLE_ENDIAN );
		tmpFileStore.nFileEndPos = core::EndianSwapInt( tmpFileStore.nFileEndPos, core::MACHINE_LITTLE_ENDIAN );
		tmpFileStore.nFileLength = core::EndianSwapInt( static_cast<unsigned int>(tmpFileStore.nFileLength), core::MACHINE_LITTLE_ENDIAN );

		if( fileHandle->nFileType == FILETYPE_BINARY_WRITE && fileHandle->nState == FILESTATE_OPEN )
		{
			if( fileHandle->fp )
				result += fwrite( &tmpFileStore, size, 1, fileHandle->fp );
		}
		else if( fileHandle->nFileType == FILETYPE_BINARY_STREAM_WRITE && fileHandle->nState == FILESTATE_OPEN )
		{
			if( fileHandle->output.is_open() )
				fileHandle->output.write( reinterpret_cast<char *>(&tmpFileStore), static_cast<std::streamsize>(size) );
		}
		else
		{
			DBGLOG( "FILEIO: *ERROR* unrecognised write type in FileWriteFileStore\n" );
			return(0);
		}
	}
	
	return(result);
}

/////////////////////////////////////////////////////
/// Function: FileWriteCURL
/// Params: [in]buffer, [in]size, [in]count, [in]fileHandle
///
/////////////////////////////////////////////////////
std::size_t file::FileWriteCURL( char *buffer, std::size_t size, std::size_t count, file::TFileHandle* fileHandle )
{
	if( fileHandle != 0 )
	{
		if( file::FileIsOpen( fileHandle ) )
			return file::FileWrite( buffer, size, count, fileHandle );
	}

	return 0;
}

/////////////////////////////////////////////////////
/// Function: ReadString
/// Params: [in]szString, [in]fp
///
/////////////////////////////////////////////////////
void file::ReadString( char *szString, FILE *fp )
{
	//int nScanReturn = 0;	// Return value of fscanf

	if( szString == 0 )
		return;

	// Skip all strings that contain comments
	std::memset( szString, 0, sizeof(char) );

	// Read new string
	//nScanReturn = fscanf( fp, "%s", szString );
		
	// Is rest of the line a comment ?
	if( !std::strncmp(szString, COMMENT_ID, sizeof(COMMENT_ID) ) ||
		!std::strncmp(szString, COMMENT1_ID, sizeof(COMMENT1_ID) ) ||
		!std::strncmp(szString, COMMENT2_ID, sizeof(COMMENT2_ID) ) )
	{
		// Skip the rest of the line
		while( !feof( fp ) )
		{
			int c = 0;
			c = fgetc( fp );

			if( c == '\n' || c == EOF )
				break;
		}
	}
}

/////////////////////////////////////////////////////
/// Function: CreateDirectory
/// Params: [in]szDirectory
///
/////////////////////////////////////////////////////
int file::CreateDirectory( const char *szDirectory )
{
#ifdef BASE_PLATFORM_WINDOWS
	#ifdef BASE_SUPPORT_64BIT
		return 1;
		//return( _wmkdir( szDirectory ) );
	#else
		return( _mkdir( szDirectory ) );
	#endif
#elif defined(BASE_PLATFORM_MAC) || defined(BASE_PLATFORM_iOS)
	#ifdef BASE_DUMMY_APP // running QT so has to be C++ only
	
	#else
		NSError* error = nil;
		NSString* convertedPath = [[NSString alloc] initWithCString:szDirectory encoding:NSUTF8StringEncoding];
		[[NSFileManager defaultManager] createDirectoryAtPath:convertedPath withIntermediateDirectories:NO attributes:nil error:&error];
		if( error != nil )
			return 1;
		else
			return 0;
	#endif
#else
	return( mkdir( szDirectory, 0777 ) );
#endif 
	
	return 1;
}

/////////////////////////////////////////////////////
/// Function: DeleteDirectory
/// Params: [in]szDirectory
///
/////////////////////////////////////////////////////
int file::DeleteDirectory( const char *szDirectory, bool deleteSubDirectories )
{
#ifdef BASE_PLATFORM_WINDOWS
	#ifdef BASE_SUPPORT_64BIT
		return 1;
	#else
		char doubleNullDir[core::MAX_PATH+core::MAX_PATH];
		std::memset( doubleNullDir, 0, sizeof(char)*core::MAX_PATH+core::MAX_PATH );
		snprintf( doubleNullDir, core::MAX_PATH+core::MAX_PATH, "%s\0", szDirectory );

		SHFILEOPSTRUCTA file_op = {
			0,
			FO_DELETE,
			doubleNullDir,
			"\0",
			FOF_NO_UI,
			//FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT,
			false,
			0,
			"" };
		SHFileOperationA(&file_op);
	#endif //
#elif defined(BASE_PLATFORM_MAC) || defined(BASE_PLATFORM_iOS)
	#ifdef BASE_DUMMY_APP // running QT so has to be C++ only
	
	#else
		NSError* error = nil;
		NSString* convertedPath = [[NSString alloc] initWithCString:szDirectory encoding:NSUTF8StringEncoding];
		[[NSFileManager defaultManager] removeItemAtPath:convertedPath error:&error];
		if( error != nil )
			return 1;
		else
			return 0;
	#endif
#else
	return( rmdir( szDirectory ) );
#endif // BASE_PLATFORM_MA

	return 1;
}

#ifdef BASE_SUPPORT_WCHAR
	/////////////////////////////////////////////////////
	/// Function: CreateDirectory
	/// Params: [in]szDirectory
	///
	/////////////////////////////////////////////////////
	int file::CreateDirectory( const wchar_t *szDirectory )
	{

	#ifdef BASE_PLATFORM_WINDOWS
		return( _wmkdir( szDirectory ) );
	#endif // BASE_PLATFORM_WINDOWS

		return(1);
	}


	/////////////////////////////////////////////////////
	/// Function: CreateFileStructure (wide support)
	/// Params: [in]szFilename, [in/out]pFileStruct
	///
	/////////////////////////////////////////////////////
	void file::CreateFileStructure( const wchar_t *szFilename, TFile *pFileStruct )
	{
		if( !core::IsEmptyString( szFilename ) && pFileStruct )
		{
			std::memset( pFileStruct, 0, sizeof(TFile) );

			core::SplitPath( szFilename, pFileStruct->szDrive, pFileStruct->szPath, pFileStruct->szFile, pFileStruct->szFileExt );

			if( !core::IsEmptyString( szFilename ) )
				std::swprintf( pFileStruct->szFilename, core::MAX_PATH+core::MAX_PATH, L"%s", szFilename );
			if( !core::IsEmptyString( pFileStruct->szFile ) && !core::IsEmptyString( pFileStruct->szFileExt ) )
				std::swprintf( pFileStruct->szFileAndExt, core::MAX_PATH, L"%s%s", pFileStruct->szFile, pFileStruct->szFileExt );
		}
		else
		{
			DBGLOG( L"FILEIO: Could not create file structure\n" );
		}
	}

	/////////////////////////////////////////////////////
	/// Function: FileOpen (wide support)
	/// Params: [in]szFilename, [in]nType, [in]fileHandle
	///
	/////////////////////////////////////////////////////
	bool file::FileOpen( const wchar_t *szFilename, EFileType nType, TFileHandle* fileHandle )
	{
		if( core::IsEmptyString( szFilename ) )
			return(false);

		if( fileHandle->nState == FILESTATE_OPEN )
		{
			DBGLOG( L"FILEIO: *WARNING* trying to open an already open file\n" );
			return(true);
		}

		long nFileStart = 0;
		long nFileEnd = 0;

		if( nType == FILETYPE_BINARY_READ )
		{
			fileHandle->fp = _wfopen( szFilename, L"rb" );

			if( !fileHandle->fp )
				return(false);

			fseek( fileHandle->fp, 0, SEEK_END );
			fileHandle->nFileLength = ftell( fileHandle->fp );
			fseek( fileHandle->fp, 0, SEEK_SET );
		}
		else if( nType == FILETYPE_BINARY_WRITE )
		{
			fileHandle->fp = _wfopen( szFilename, L"wb" );

			if( !fileHandle->fp )
				return(false);

			fseek( fileHandle->fp, 0, SEEK_END );
			fileHandle->nFileLength = ftell( fileHandle->fp );
			fseek( fileHandle->fp, 0, SEEK_SET );
		}
		else if( nType == FILETYPE_BINARY_STREAM_READ )
		{
			fileHandle->input.open( szFilename, std::ios::in | std::ios::binary );

			if( !fileHandle->input.is_open() || fileHandle->input.fail() )
				return(false);

			nFileStart = fileHandle->input.tellg();
			fileHandle->input.seekg( 0, std::ios::end );
			nFileEnd = fileHandle->input.tellg();
			fileHandle->input.seekg( 0, std::ios::beg );

			fileHandle->nFileLength = nFileEnd - nFileStart;
		}
		else if( nType == FILETYPE_BINARY_STREAM_WRITE )
		{
			fileHandle->output.open( szFilename, std::ios::out | std::ios::binary );

			if( !fileHandle->output.is_open() || fileHandle->output.fail() )
				return(false);

			nFileStart = fileHandle->output.tellp();
			fileHandle->output.seekp( 0, std::ios::end );
			nFileEnd = fileHandle->output.tellp();
			fileHandle->output.seekp( 0, std::ios::beg );

			fileHandle->nFileLength = nFileEnd - nFileStart;
		}
		else if( nType == FILETYPE_TEXT_READ )
		{
			fileHandle->fp = _wfopen( szFilename, L"rt" );

			if( !fileHandle->fp )
				return(false);

			fseek( fileHandle->fp, 0, SEEK_END );
			fileHandle->nFileLength = ftell( fileHandle->fp );
			fseek( fileHandle->fp, 0, SEEK_SET );
		}
		else if( nType == FILETYPE_TEXT_WRITE )
		{
			fileHandle->fp = _wfopen( szFilename, L"wt" );

			if( !fileHandle->fp )
				return(false);

			fseek( fileHandle->fp, 0, SEEK_END );
			fileHandle->nFileLength = ftell( fileHandle->fp );
			fseek( fileHandle->fp, 0, SEEK_SET );
		}
		else
		{
			// unknown type
			DBGLOG( L"FILEIO: *ERROR* unrecognised file type in FileOpen\n" );
			return(false);
		}

		fileHandle->nState = FILESTATE_OPEN;
		fileHandle->nFileType = nType;

		// track it
		nFilesOpened++;

		return(true);
	}

	/////////////////////////////////////////////////////
	/// Function: FileExists (wide support)
	/// Params: [in]szFilename
	///
	/////////////////////////////////////////////////////
	bool file::FileExists( const wchar_t *szFilename )
	{
		if( core::IsEmptyString(szFilename) )
			return(false);

		file::TFileHandle fileHandle;

		if( FileOpen( szFilename, file::FILETYPE_BINARY_STREAM_READ, fileHandle ) )
		{
			file::FileClose(fileHandle);

			// does exist
			return(true);
		}

		// doesn't exist
		return(false);
	}

	/////////////////////////////////////////////////////
	/// Function: FileCopy
	/// Params: [in]szInFilename, [in]szOutFilename, [in]bFailIfExists
	///
	/////////////////////////////////////////////////////
	int file::FileCopy( const wchar_t *szInFilename, const wchar_t *szOutFilename, bool bFailIfExists )
	{
		FILE *pInFile = 0;
		FILE *pOutFile = 0;

		long inLen = 0;

		char* c = 0;
		//int c = 0;

		if( std::wcscmp( szInFilename, szOutFilename ) == 0 )
		{
			DBGLOG( L"FILEIO: *WARNING* FileCopy source and destination names were the same\n" );
			return(1);
		}

		if( core::IsEmptyString( szInFilename ) )
			return(1);

		pInFile = _wfopen( szInFilename, L"rb" );

		if( pInFile == 0 )
			return(1);

		// track it
		nFilesOpened++;

		fseek( pInFile, 0, SEEK_END );
		inLen = ftell( pInFile );
		fseek( pInFile, 0, SEEK_SET );

		if( inLen > 0 )
		{
			c = new char[inLen];

			if( c == 0 )
				return(1);
		}
		else
			return(1);

		if( core::IsEmptyString( szOutFilename ) )
			return(1);

		pOutFile = _wfopen( szOutFilename, L"wb" );

		if( pOutFile == 0 )
			return(1);

		// track it
		nFilesOpened++;

		while( !feof(pInFile) )
		{
			// read it
			fread( c, sizeof( char ), inLen, pInFile );

			// write it
			fwrite( c, sizeof( char ), inLen, pOutFile );
		}

		if( c )
		{
			delete[] c;
			c = 0;
		}

		fclose( pInFile );
		// track it
		nFilesClosed++;

		fclose( pOutFile );
		// track it
		nFilesClosed++;

		return(0);

	}

	/////////////////////////////////////////////////////
	/// Function: FileWriteString
	/// Params: [in]buffer, [in]size, [in]fileHandle
	///
	/////////////////////////////////////////////////////
	std::size_t file::FileWriteString( wchar_t *buffer, std::size_t size,  TFileHandle* fileHandle )
	{
		std::size_t result = 0;

		if( fileHandle->nFileType == FILETYPE_TEXT_WRITE && fileHandle->nState == FILESTATE_OPEN )
		{
			if( fileHandle->fp )
				result += fputws( buffer, fileHandle->fp );
		}
	//	else if( fileHandle->nFileType == FILETYPE_BINARY_STREAM_WRITE && fileHandle->nState == FILESTATE_OPEN )
	//	{
	//		if( fileHandle->output.is_open() )
	//			fileHandle->output.write( reinterpret_cast<char *>(&fTempFloat), static_cast<std::streamsize>(size) );
	//	}
		else
		{
			DBGLOG( L"FILEIO: *ERROR* unrecognised read type in FileWriteString\n" );
			return(0);
		}

		return(result);
	}
#endif // BASE_SUPPORT_WCHAR
