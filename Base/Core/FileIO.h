
#ifndef __FILEIO_H__
#define __FILEIO_H__

#ifndef __COREDEFINES_H__
	#include "Core/CoreDefines.h"
#endif // __COREDEFINES_H__

#ifndef __COREFUNCTIONS_H__
	#include "Core/CoreFunctions.h"
#endif // __COREFUNCTIONS_H__

#include <iostream>
#include <fstream>
#include <cstring>

#ifndef _ZZIP_ZZIP_H
	#include <zzip/zzip.h>
#endif // _ZZIP_ZZIP_H

namespace file
{
	static const char COMMENT_ID[]		= "#";
	static const char COMMENT1_ID[]		= ";";
	static const char COMMENT2_ID[]		= "//";

	enum EFileState
	{
		FILESTATE_CLOSED=0,
		FILESTATE_OPEN,
	};

	enum EFileType
	{
		FILETYPE_BINARY_READ = 0,
		FILETYPE_BINARY_WRITE,
		FILETYPE_BINARY_STREAM_READ,
		FILETYPE_BINARY_STREAM_WRITE,

		FILETYPE_TEXT_READ,
		FILETYPE_TEXT_WRITE,
	};

	enum EFileSeekOrigin
	{
		FILESEEK_BEG=0,
		FILESEEK_CUR,
		FILESEEK_END

	};

	struct TFile
	{
		TFile()
		{
			std::memset( szFilename, 0, sizeof(char)*core::MAX_PATH+core::MAX_PATH );
			std::memset( szDrive, 0, sizeof(char)*core::MAX_PATH );
			std::memset( szPath, 0, sizeof(char)*core::MAX_PATH );
			std::memset( szFileAndExt, 0, sizeof(char)*core::MAX_PATH );
			std::memset( szFile, 0, sizeof(char)*core::MAX_PATH );
			std::memset( szFileExt, 0, sizeof(char)*core::MAX_PATH );
		}
		
		TFile( const char *szConstructFile )
		{
			core::SplitPath( szConstructFile, szDrive, szPath, szFile, szFileExt );

			if( !core::IsEmptyString( szConstructFile ) )
				snprintf( szFilename, core::MAX_PATH+core::MAX_PATH, "%s", szConstructFile );

			if( !core::IsEmptyString( szFile ) && !core::IsEmptyString( szFileExt ) )
				snprintf( szFileAndExt, core::MAX_PATH, "%s%s", szFile, szFileExt );
		}

		~TFile()
		{
			std::memset( szFilename, 0, sizeof(char)*core::MAX_PATH+core::MAX_PATH );
			std::memset( szDrive, 0, sizeof(char)*core::MAX_PATH );
			std::memset( szPath, 0, sizeof(char)*core::MAX_PATH );
			std::memset( szFileAndExt, 0, sizeof(char)*core::MAX_PATH );
			std::memset( szFile, 0, sizeof(char)*core::MAX_PATH );
			std::memset( szFileExt, 0, sizeof(char)*core::MAX_PATH );
		}

		TFile &operator = (const TFile &f)
		{
			if( !core::IsEmptyString( f.szFilename ) )
				snprintf( szFilename, core::MAX_PATH+core::MAX_PATH, "%s", f.szFilename );
				snprintf( szDrive, core::MAX_PATH, "%s", f.szDrive );
			if( !core::IsEmptyString( f.szPath ) )
				snprintf( szPath, core::MAX_PATH, "%s", f.szPath );
			if( !core::IsEmptyString( f.szFile ) )
				snprintf( szFile, core::MAX_PATH, "%s", f.szFile );
			if( !core::IsEmptyString( f.szFileExt ) )
				snprintf( szFileExt, core::MAX_PATH, "%s", f.szFileExt );

			if( !core::IsEmptyString( f.szFileAndExt ) )
			{
				snprintf( szFileAndExt, core::MAX_PATH, "%s", f.szFileAndExt );
			}
			else
			{
				if( !core::IsEmptyString( f.szFile ) && !core::IsEmptyString( f.szFileExt ) )
					snprintf( szFileAndExt, core::MAX_PATH, "%s%s", f.szFile, f.szFileExt );
			}

			return(*this);
		}

		/// filename
		char szFilename[core::MAX_PATH+core::MAX_PATH];
		/// drive only
		char szDrive[core::MAX_PATH];
		/// path only
		char szPath[core::MAX_PATH];
		/// filename and extension only
		char szFileAndExt[core::MAX_PATH];
		/// file only
		char szFile[core::MAX_PATH];
		/// file extension only
		char szFileExt[core::MAX_PATH];
	};

	struct TFileStoreInformation
	{
		/// file start offset 
		long nFileStartPos;
		/// file end offset
		long nFileEndPos;
		/// file length
		std::size_t nFileLength;
	};

	struct TFileHandle
	{
		/// file state
		EFileState nState;
		/// file type
		EFileType nFileType;
		/// file length
		std::size_t nFileLength;
		/// normal file pointer
		FILE* fp;
		/// c++ input stream
		std::ifstream input;
		/// c++ output stream
		std::ofstream output;
		/// reading zip file flag
		bool zipped;
		/// zip file pointer
		ZZIP_FILE* zfp;

		/// a file within a file
		TFileStoreInformation subFile;
		
		TFileHandle()
		{
			/// file state
			nState = FILESTATE_CLOSED;
			/// file type
			nFileType = FILETYPE_BINARY_READ;
			/// file length
			nFileLength = 0;
			/// normal file pointer
			fp = 0;
			
			subFile.nFileStartPos = 0;
			subFile.nFileEndPos = 0;
			subFile.nFileLength = 0;			
		}
		~TFileHandle()
		{
			/// file state
			nState = FILESTATE_CLOSED;
			/// file type
			nFileType = FILETYPE_BINARY_READ;
			/// file length
			nFileLength = 0;
			/// normal file pointer
			fp = 0;
			
			subFile.nFileStartPos = 0;
			subFile.nFileEndPos = 0;
			subFile.nFileLength = 0;			
		}		
	};

	/// GetZipExtensions - Gets the current extensions to files for zip file operations
	/// \return zzip_strings_t - gets the extension list
	zzip_strings_t* GetZipExtensions();

	/// ReportFileCounts - Reports the total files opened and closed
	void ReportFileCounts();

	/// CreateFileStructure - Create a file structure from a file name
	/// \param szFilename - file to create from
	/// \param pFileStruct - pointer to file structure to fill
	void CreateFileStructure( const char *szFilename, TFile *pFileStruct );

	/// FileIsOpen - Checks if a file handle is open
	/// \param fileHandle - file structure
	/// \return boolean - (SUCCESS:true or FAIL:false)
	bool FileIsOpen( TFileHandle* fileHandle );

	/// FileOpen - Open a new file
	/// \param szFilename - file to open
	/// \param nType - type to open
	/// \param fileHandle - structure to hold file
	/// \return boolean - (SUCCESS:true or FAIL:false)
	bool FileOpen( const char *szFilename, EFileType nType, TFileHandle* fileHandle );

	/// IsEOF - Checks to see if the handle is at the end of the file
	/// \param fileHandle - file structure
	/// \return boolean - (SUCCESS:true or FAIL:false)
	bool IsEOF( TFileHandle* fileHandle );

	/// FileSeek - Seeks through a file
	/// \param nOffset - file offset
	/// \param eOrigin - seek origin
	/// \param fileHandle - file structure
	/// \return integer - (SUCCESS:OK or FAIL:FAIL)
	int FileSeek( long nOffset, EFileSeekOrigin eOrigin, TFileHandle* fileHandle );

	/// FileTell - Get the current file position
	/// \param fileHandle - file structure
	/// \return std::size_t - position in the stream
	std::size_t FileTell( TFileHandle* fileHandle );

	/// FilePosition - Get the current file position
	/// \param fileHandle - structure to hold file
	/// \return std::size_t - position of current stream
	std::size_t FilePosition( TFileHandle* fileHandle );

	/// FileSize - Seeks through a file to get the size
	/// \param fileHandle - structure to hold file
	/// \return std::size_t - size of the file
	std::size_t FileSize( TFileHandle* fileHandle );

	/// FileSize - Gets the file size using stat
	/// \param szFilename - name of the file
	/// \return long - size of the file
	std::size_t FileSize( const char* szFilename );

	/// FileClose - Closes a file
	/// \param fileHandle - file structure
	/// \return boolean - (SUCCESS:true or FAIL:false)
	bool FileClose( TFileHandle* fileHandle );

	/// FileExists - Checks to see if a file exists
	/// \param szFilename - file to check
	/// \return boolean - (true or false) 
	bool FileExists( const char *szFilename );

	/// FileCopy - Reads a file and copies it to another location
	/// \param szInFilename - input file to copy
	/// \param szOutFilename - output file
	/// \param bFailIfExists - flag if output file already exists, don't overwrite it
	/// \return integer - ( SUCCESS:OK or FAIL:FAIL )
	int FileCopy( const char *szInFilename, const char *szOutFilename, bool bFailIfExists );
	/// FileDelete - Deletes a file
	/// \param szFilename - file to delete
	/// \return integer - ( SUCCESS:OK or FAIL:FAIL )
	int FileDelete( const char *szFilename );

	/// FileGetC - Reads a single character from the stream
	/// \param fileHandle - pointer to the file handler
	/// \return integer - returns the character that was read
	int FileGetC( TFileHandle* fileHandle );
	/// FileGetPos - Gets the position of the file stream
	/// \param pos - variable for storing position
	/// \param fileHandle - pointer to the file handler
	/// \return integer - returns the character that was read
	int FileGetPos( fpos_t* pos, TFileHandle* fileHandle );

	/// FileRead - Reads a buffer from the stream
	/// \param buffer - buffer to receive 
	/// \param size - item size in bytes
	/// \param fileHandle - pointer to the file handler
	/// \return size_t - returns the number of bytes read
	std::size_t FileRead( void *buffer, std::size_t size,  TFileHandle* fileHandle );
	/// FileRead - Reads a buffer from the stream
	/// \param buffer - buffer to receive 
	/// \param size - item size in bytes
	/// \param count - number of items
	/// \param fileHandle - pointer to the file handler
	/// \return size_t - returns the number of bytes read
	std::size_t FileRead( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle );

	/// FileReadUChar - Reads an unsigned char from the stream
	/// \param buffer - buffer to receive 
	/// \param size - item size in bytes
	/// \param count - maximum number of items to be read
	/// \param fileHandle - pointer to the file handler
	/// \return size_t - returns the number of bytes read
	std::size_t FileReadUChar( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle );
	/// FileReadChar - Reads a char from the stream
	/// \param buffer - buffer to receive 
	/// \param size - item size in bytes
	/// \param count - maximum number of items to be read
	/// \param fileHandle - pointer to the file handler
	/// \return size_t - returns the number of bytes read
	std::size_t FileReadChar( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle );
	/// FileReadBool - Reads a bool from the stream
	/// \param buffer - buffer to receive 
	/// \param size - item size in bytes
	/// \param count - maximum number of items to be read
	/// \param fileHandle - pointer to the file handler
	/// \return size_t - returns the number of bytes read
	std::size_t FileReadBool( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle );
	/// FileReadShort - Reads an short from the stream
	/// \param buffer - buffer to receive 
	/// \param size - item size in bytes
	/// \param count - maximum number of items to be read
	/// \param fileHandle - pointer to the file handler
	/// \return size_t - returns the number of bytes read
	std::size_t FileReadShort( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle );
	/// FileReadUShort - Reads an unsigned short from the stream
	/// \param buffer - buffer to receive 
	/// \param size - item size in bytes
	/// \param count - maximum number of items to be read
	/// \param fileHandle - pointer to the file handler
	/// \return size_t - returns the number of bytes read
	std::size_t FileReadUShort( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle );
	/// FileReadInt - Reads an int from the stream
	/// \param buffer - buffer to receive 
	/// \param size - item size in bytes
	/// \param count - maximum number of items to be read
	/// \param fileHandle - pointer to the file handler
	/// \return size_t - returns the number of bytes read
	std::size_t FileReadInt( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle );
	/// FileReadUInt - Reads an unsigned int from the stream
	/// \param buffer - buffer to receive 
	/// \param size - item size in bytes
	/// \param count - maximum number of items to be read
	/// \param fileHandle - pointer to the file handler
	/// \return size_t - returns the number of bytes read
	std::size_t FileReadUInt( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle );
	/// FileReadFloat - Reads a float from the stream
	/// \param buffer - buffer to receive 
	/// \param size - item size in bytes
	/// \param count - maximum number of items to be read
	/// \param fileHandle - pointer to the file handler
	/// \return size_t - returns the number of bytes read
	std::size_t FileReadFloat( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle);
	/// FileReadDouble - Reads a double from the stream
	/// \param buffer - buffer to receive 
	/// \param size - item size in bytes
	/// \param count - maximum number of items to be read
	/// \param fileHandle - pointer to the file handler
	/// \return size_t - returns the number of bytes read
	std::size_t FileReadDouble( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle);

	/// FileReadTerminatedString - Reads a null terminated string
	/// \param buffer - buffer to receive 
	/// \param maxCount - maximum size of the buffer
	/// \param fileHandle - pointer to the file handler
	/// \return size_t - returns the number of bytes read
	std::size_t FileReadTerminatedString( char *buffer, unsigned int maxCount, TFileHandle* fileHandle );
	/// FileReadVec2D - Reads a Vec2 from the stream
	/// \param buffer - buffer to receive 
	/// \param size - item size in bytes
	/// \param count - maximum number of items to be read
	/// \param fileHandle - pointer to the file handler
	/// \return size_t - returns the number of bytes read
	std::size_t FileReadVec2D( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle );
	/// FileReadVec3D - Reads an Vec3 from the stream
	/// \param buffer - buffer to receive 
	/// \param size - item size in bytes
	/// \param count - maximum number of items to be read
	/// \param fileHandle - pointer to the file handler
	/// \return size_t - returns the number of bytes read
	std::size_t FileReadVec3D( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle );
	/// FileReadVec4D - Reads an Vec4 from the stream
	/// \param buffer - buffer to receive 
	/// \param size - item size in bytes
	/// \param count - maximum number of items to be read
	/// \param fileHandle - pointer to the file handler
	/// \return size_t - returns the number of bytes read
	std::size_t FileReadVec4D( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle );
	/// FileWriteLambertBlock - Read a mdl::mat::LambertShaderBlock from the stream
	/// \param buffer - buffer to receive 
	/// \param size - item size in bytes
	/// \param count - maximum number of items to be read
	/// \param fileHandle - pointer to the file handler
	/// \return size_t - returns the number of bytes read 
	std::size_t FileReadLambertBlock( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle );
	/// FileWriteReflectBlock - Read a mdl::mat::ReflectShaderBlock from the stream
	/// \param buffer - buffer to receive 
	/// \param size - item size in bytes
	/// \param count - maximum number of items to be read
	/// \param fileHandle - pointer to the file handler
	/// \return size_t - returns the number of bytes read 
	std::size_t FileReadReflectBlock( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle );
	/// FileReadPhongBlock - Read a mdl::mat::PhongShaderBlock from the stream
	/// \param buffer - buffer to receive 
	/// \param size - item size in bytes
	/// \param count - maximum number of items to be read
	/// \param fileHandle - pointer to the file handler
	/// \return size_t - returns the number of bytes read 
	std::size_t FileReadPhongBlock( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle );
	/// FileReadBlinnBlock - Read a mdl::mat::BlinnShaderBlock from the stream
	/// \param buffer - buffer to receive 
	/// \param size - item size in bytes
	/// \param count - maximum number of items to be read
	/// \param fileHandle - pointer to the file handler
	/// \return size_t - returns the number of bytes read 
	std::size_t FileReadBlinnBlock( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle );

	/// FileReadFileStructure - Reads a TFile structure from the stream
	/// \param buffer - buffer to receive 
	/// \param size - item size in bytes
	/// \param count - maximum number of items to be read
	/// \param fileHandle - pointer to the file handler
	/// \return size_t - returns the number of bytes read
	std::size_t FileReadFileStructure( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle );

	/// FileWrite - Writes a buffer to the stream
	/// \param buffer - buffer to write 
	/// \param size - item size in bytes
	/// \param fileHandle - pointer to the file handler
	/// \return size_t - returns the number of bytes written 
	std::size_t FileWrite( void *buffer, std::size_t size, TFileHandle* fileHandle );
	/// FileWrite - Writes a buffer to the stream
	/// \param buffer - buffer to write 
	/// \param size - item size in bytes
	/// \param count - number of items
	/// \param fileHandle - pointer to the file handler
	/// \return size_t - returns the number of bytes written 
	std::size_t FileWrite( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle );
	/// FileWriteUChar - Writes an unsigned char to the stream
	/// \param buffer - buffer to write 
	/// \param size - item size in bytes
	/// \param count - maximum number of items to be written
	/// \param fileHandle - pointer to the file handler
	/// \return size_t - returns the number of bytes written 
	std::size_t FileWriteUChar( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle );
	/// FileWriteChar - Writes a char to the stream
	/// \param buffer - buffer to write 
	/// \param size - item size in bytes
	/// \param count - maximum number of items to be written
	/// \param fileHandle - pointer to the file handler
	/// \return size_t - returns the number of bytes written 
	std::size_t FileWriteChar( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle );
	/// FileWriteBool - Writes bool to the stream
	/// \param buffer - buffer to write 
	/// \param size - item size in bytes
	/// \param count - maximum number of items to be written
	/// \param fileHandle - pointer to the file handler
	/// \return size_t - returns the number of bytes written 
	std::size_t FileWriteBool( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle );
	/// FileWriteShort - Writes an short to the stream
	/// \param buffer - buffer to write 
	/// \param size - item size in bytes
	/// \param count - maximum number of items to be written
	/// \param fileHandle - pointer to the file handler
	/// \return size_t - returns the number of bytes written 
	std::size_t FileWriteShort( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle );
	/// FileWriteUShort - Writes an unsigned short to the stream
	/// \param buffer - buffer to write 
	/// \param size - item size in bytes
	/// \param count - maximum number of items to be written
	/// \param fileHandle - pointer to the file handler
	/// \return size_t - returns the number of bytes written 
	std::size_t FileWriteUShort( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle );
	/// FileWriteInt - Writes an int to the stream
	/// \param buffer - buffer to write 
	/// \param size - item size in bytes
	/// \param count - maximum number fo items to be written
	/// \param fileHandle - pointer to the file handler
	/// \return size_t - returns the number of bytes written 
	std::size_t FileWriteInt( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle );
	/// FileWriteUInt - Writes an unsigned int to the stream
	/// \param buffer - buffer to write 
	/// \param size - item size in bytes
	/// \param count - maximum number fo items to be written
	/// \param fileHandle - pointer to the file handler
	/// \return size_t - returns the number of bytes written 
	std::size_t FileWriteUInt( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle );
	/// FileWriteFloat - Writes a float to the stream
	/// \param buffer - buffer to write 
	/// \param size - item size in bytes
	/// \param count - maximum number fo items to be written
	/// \param fileHandle - pointer to the file handler
	/// \return size_t - returns the number of bytes written 
	std::size_t FileWriteFloat( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle );

	/// FileWriteVec2D - Writes a Vec2 to the stream
	/// \param buffer - buffer to write 
	/// \param size - item size in bytes
	/// \param count - maximum number of items to be written
	/// \param fileHandle - pointer to the file handler
	/// \return size_t - returns the number of bytes written 
	std::size_t FileWriteVec2D( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle );
	/// FileWriteVec3D - Writes a Vec3 to the stream
	/// \param buffer - buffer to write 
	/// \param size - item size in bytes
	/// \param count - maximum number of items to be written
	/// \param fileHandle - pointer to the file handler
	/// \return size_t - returns the number of bytes written 
	std::size_t FileWriteVec3D( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle );
	/// FileWriteVec4D - Writes a Vec4 to the stream
	/// \param buffer - buffer to write 
	/// \param size - item size in bytes
	/// \param count - maximum number of items to be written
	/// \param fileHandle - pointer to the file handler
	/// \return size_t - returns the number of bytes written 
	std::size_t FileWriteVec4D( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle );
	/// FileWriteLambertBlock - Writes a mdl::mat::LambertShaderBlock to the stream
	/// \param buffer - buffer to write 
	/// \param size - item size in bytes
	/// \param count - maximum number of items to be written
	/// \param fileHandle - pointer to the file handler
	/// \return size_t - returns the number of bytes written 
	std::size_t FileWriteLambertBlock( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle );
	/// FileWriteReflectBlock - Writes a mdl::mat::ReflectShaderBlock to the stream
	/// \param buffer - buffer to write 
	/// \param size - item size in bytes
	/// \param count - maximum number of items to be written
	/// \param fileHandle - pointer to the file handler
	/// \return size_t - returns the number of bytes written 
	std::size_t FileWriteReflectBlock( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle );
	/// FileWritePhongBlock - Writes a mdl::mat::PhongShaderBlock to the stream
	/// \param buffer - buffer to write 
	/// \param size - item size in bytes
	/// \param count - maximum number of items to be written
	/// \param fileHandle - pointer to the file handler
	/// \return size_t - returns the number of bytes written 
	std::size_t FileWritePhongBlock( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle );
	/// FileWriteBlinnBlock - Writes a mdl::mat::BlinnShaderBlock to the stream
	/// \param buffer - buffer to write 
	/// \param size - item size in bytes
	/// \param count - maximum number of items to be written
	/// \param fileHandle - pointer to the file handler
	/// \return size_t - returns the number of bytes written 
	std::size_t FileWriteBlinnBlock( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle );

	/// FileWriteFileStructure - Writes a TFile structure from the stream
	/// \param buffer - buffer to receive 
	/// \param size - item size in bytes
	/// \param count - maximum number of items to be written
	/// \param fileHandle - pointer to the file handler
	/// \return size_t - returns the number of bytes written
	std::size_t FileWriteFileStructure( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle );
	/// FileWriteFileStore - Writes a TFile structure from the stream
	/// \param buffer - buffer to receive 
	/// \param size - item size in bytes
	/// \param count - maximum number of items to be written
	/// \param fileHandle - pointer to the file handler
	/// \return size_t - returns the number of bytes written
	std::size_t FileWriteFileStore( void *buffer, std::size_t size, std::size_t count, TFileHandle* fileHandle );

	/// FileWriteCURL - file writer function for libCURL
	/// \param buffer - buffer to receive 
	/// \param size - item size in bytes
	/// \param count - maximum number of items to be written
	/// \param fileHandle - pointer to the file handler
	/// \return size_t - returns the number of bytes written
	std::size_t FileWriteCURL( char *buffer, std::size_t size, std::size_t count, file::TFileHandle *fileHandle );

	/// ReadString - Reads a complete line in a file and skips comment characters
	/// \param szString - string to read from the file
	/// \param fp - file pointer
	void ReadString( char *szString, FILE *fp );

	/// CreateDirectory - Creates a directory
	/// \param szDirectory - directory to create
	/// \return integer - ( SUCCESS:0 or FAIL:FAIL )
	int CreateDirectory( const char *szDirectory );
	/// DeleteDirectory - Removed a directory
	/// \param szDirectory - directory to create
	/// \return integer - ( SUCCESS:0 or FAIL:FAIL )
	int DeleteDirectory( const char *szDirectory, bool deleteSubDirectories=true );

#ifdef BASE_SUPPORT_WCHAR
	/// CreateDirectory - Creates a directory
	/// \param szDirectory - directory to create
	/// \return integer - ( SUCCESS:OK or FAIL:FAIL )
	int CreateDirectory( const wchar_t *szDirectory );

	/// CreateFileStructure - Create a file structure from a file name
	/// \param szFilename - file to create from
	/// \param pFileStruct - pointer to file structure to fill
	void CreateFileStructure( const wchar_t *szFilename, TFile *pFileStruct );
	/// FileOpen - Open a new file (wide support)
	/// \param szFilename - file to open
	/// \param nType - type to open
	/// \param fileHandle - structure to hold file
	/// \return boolean - (SUCCESS:true or FAIL:false)
	bool FileOpen( const wchar_t *szFilename, EFileType nType, TFileHandle* fileHandle );
	/// FileExists - Checks to see if a file exists
	/// \param szFilename - file to check
	/// \return boolean - (true or false) 
	bool FileExists( const wchar_t *szFilename );
	/// FileWriteString - Writes a wide character string to the stream
	/// \param buffer - wide character string
	/// \param size - buffer size in bytes
	/// \param fileHandle - pointer to the file handler
	/// \return size_t - returns the number of bytes written 
	std::size_t FileWriteString( wchar_t *buffer, std::size_t size,  TFileHandle* fileHandle );
	/// FileCopy - Reads a file and copies it to another location
	/// \param szInFilename - input file to copy
	/// \param szOutFilename - output file
	/// \param bFailIfExists - flag if output file already exists, don't overwrite it
	/// \return integer - ( SUCCESS:OK or FAIL:FAIL )
	int FileCopy( const wchar_t *szInFilename, const wchar_t *szOutFilename, bool bFailIfExists );
#endif // BASE_SUPPORT_WCHAR

} // namespace file

#endif // __FILEIO_H__

