
/*===================================================================
	File: CoreFunctions.cpp
	Library: Core

	(C)Hidden Games
=====================================================================*/


#include "Core/CoreDefines.h"

#include <ctime>
#include <cstring>
#include <algorithm>

#include "Core/CoreConsts.h"
#include "Core/CoreFunctions.h"
#include "DebugBase.h"

/////////////////////////////////////////////////////
/// Function: SplitPath
/// Params: [in]path, [out]drive, [out]dir, [out]fname, [out]ext
///
/////////////////////////////////////////////////////
void core::SplitPath( const char *path, char *drive, char *dir, char *fname, char *ext )
{
	char *p;
	char *last_slash = 0, *dot = 0;
	unsigned int len;

	if( IsEmptyString( path ) )
		return;

	// we assume that the path argument has the following form, where any
	// or all of the components may be missing.
	//
	// <drive><dir><fname><ext>
	//
	// and each of the components has the following expected form(s)
	//
	//  drive:
	//  0 to MAX_DRIVE-1 characters, the last of which, if any, is a
	//  ':'
	//  dir:
	//  0 to MAX_DIR-1 characters in the form of an absolute path
	//  (leading '/' or '\') or relative path, the last of which, if
	//  any, must be a '/' or '\'.  E.g -
	//  absolute path:
	//      \top\next\last\     ; or
	//      /top/next/last/
	//  relative path:
	//      top\next\last\  ; or
	//      top/next/last/
	//  Mixed use of '/' and '\' within a path is also tolerated
	//  fname:
	//  0 to MAX_FNAME-1 characters not including the '.' character
	// ext:
	//  0 to MAX_EXT-1 characters where, if any, the first must be a
	//  '.'
	//
	//

	// extract drive letter and :, if any 

	if ((std::strlen(path) >= (MAX_DRIVE - 2)) && (*(path + MAX_DRIVE - 2) == ':')) 
	{
		if (drive) 
		{
			std::strncpy(drive, path, MAX_DRIVE - 1);
			*(drive + MAX_DRIVE-1) = '\0';
		}

		path += MAX_DRIVE - 1;
	}
	else if (drive) 
	{
		*drive = '\0';
	}

	// extract path string, if any.  Path now points to the first character
	// of the path, if any, or the filename or extension, if no path was
	// specified.  Scan ahead for the last occurence, if any, of a '/' or
	// '\' path separator character.  If none is found, there is no path.
	// We will also note the last '.' character found, if any, to aid in
	// handling the extension.
	//

	for (last_slash = 0, p = const_cast<char *>(path); *p; p++) 
	{
		if (*p == '/' || *p == '\\')
			// point to one beyond for later copy
			last_slash = p + 1;
		else if (*p == '.')
			dot = p;
	}

	if (last_slash) 
	{

		// found a path - copy up through last_slash or max. characters
		// allowed, whichever is smaller
		//

		if (dir) 
		{
			len = std::min( (int)((reinterpret_cast<char *>(last_slash) - const_cast<char *>(path)) / sizeof(char)), (MAX_DIR - 1));
			std::strncpy(dir, path, len);
			*(dir + len) = '\0';
		}

		path = last_slash;
	}
	else if (dir) 
	{
		// no path found
		*dir = '\0';
	}

	// extract file name and extension, if any.  Path now points to the
	// first character of the file name, if any, or the extension if no
	// file name was given.  Dot points to the '.' beginning the extension,
	// if any.
	//

	if (dot && (dot >= path)) 
	{
		// found the marker for an extension - copy the file name up to
		// the '.'.
		//
		if (fname) 
		{
			len = std::min((int)((reinterpret_cast<char *>(dot) - const_cast<char *>(path)) / sizeof(char)), (MAX_FNAME - 1));
			std::strncpy(fname, path, len);
			*(fname + len) = '\0';
		}
		// now we can get the extension - remember that p still points
		// to the terminating nul character of path.
		//
		if (ext) 
		{
			len = std::min((int)((reinterpret_cast<char *>(p) - reinterpret_cast<char *>(dot)) / sizeof(char)), (MAX_EXT - 1));
			std::strncpy(ext, dot, len);
			*(ext + len) = '\0';
		}
	}
	else 
	{
		// found no extension, give empty extension and copy rest of
		// string into fname.
		//
		if (fname) 
		{
			len = std::min((int)(((char *)p - const_cast<char *>(path)) / sizeof(char)), (MAX_FNAME - 1));
			std::strncpy(fname, path, len);
			*(fname + len) = '\0';
		}
		if (ext) 
		{
			*ext = '\0';
		}
	}
}

/////////////////////////////////////////////////////
/// Function: GetDateAndTime
/// Params: None
///
/////////////////////////////////////////////////////
char *core::GetDateAndTime( void )
{
	struct tm *newtime;
	time_t aclock;

	time( &aclock );   // Get time in seconds
	newtime = localtime( &aclock );   // Convert time to struct tm form

	return( asctime( newtime ) );
}

/////////////////////////////////////////////////////
/// Function: IsEmptyString
/// Params: [in]szString
///
/////////////////////////////////////////////////////
bool core::IsEmptyString( const char *szString )
{
	if( szString == 0 )
		return(true);

	if( szString[0] == '\0' )
		return(true);

	return(false);
}

#ifdef BASE_SUPPORT_WCHAR
	/////////////////////////////////////////////////////
	/// Function: SplitPath
	/// Params: [in]path, [out]drive, [out]dir, [out]fname, [out]ext
	///
	/////////////////////////////////////////////////////
	void core::SplitPath( const wchar_t *path, wchar_t *drive, wchar_t *dir, wchar_t *fname, wchar_t *ext )
	{
		wchar_t *p;
		wchar_t *last_slash = 0, *dot = 0;
		unsigned int len;

		if( IsEmptyString( path ) )
			return;

		// we assume that the path argument has the following form, where any
		// or all of the components may be missing.
		//
		// <drive><dir><fname><ext>
		//
		// and each of the components has the following expected form(s)
		//
		//  drive:
		//  0 to MAX_DRIVE-1 characters, the last of which, if any, is a
		//  ':'
		//  dir:
		//  0 to MAX_DIR-1 characters in the form of an absolute path
		//  (leading '/' or '\') or relative path, the last of which, if
		//  any, must be a '/' or '\'.  E.g -
		//  absolute path:
		//      \top\next\last\     ; or
		//      /top/next/last/
		//  relative path:
		//      top\next\last\  ; or
		//      top/next/last/
		//  Mixed use of '/' and '\' within a path is also tolerated
		//  fname:
		//  0 to MAX_FNAME-1 characters not including the '.' character
		// ext:
		//  0 to MAX_EXT-1 characters where, if any, the first must be a
		//  '.'
		//
		//

		// extract drive letter and :, if any 

		if ((std::wcslen(path) >= (MAX_DRIVE - 2)) && (*(path + MAX_DRIVE - 2) == ':')) 
		{
			if (drive) 
			{
				std::wcsncpy(drive, path, MAX_DRIVE - 1);
				*(drive + MAX_DRIVE-1) = '\0';
			}

			path += MAX_DRIVE - 1;
		}
		else if (drive) 
		{
			*drive = '\0';
		}

		// extract path string, if any.  Path now points to the first character
		// of the path, if any, or the filename or extension, if no path was
		// specified.  Scan ahead for the last occurence, if any, of a '/' or
		// '\' path separator character.  If none is found, there is no path.
		// We will also note the last '.' character found, if any, to aid in
		// handling the extension.
		//

		for (last_slash = 0, p = const_cast<wchar_t *>(path); *p; p++) 
		{
			if (*p == L'/' || *p == L'\\')
				// point to one beyond for later copy
				last_slash = p + 1;
			else if (*p == L'.')
				dot = p;
		}

		if (last_slash) 
		{

			// found a path - copy up through last_slash or max. characters
			// allowed, whichever is smaller
			//

			if (dir) 
			{
				len = std::min( (int)((reinterpret_cast<wchar_t *>(last_slash) - const_cast<wchar_t *>(path))), (MAX_DIR - 1));
				std::wcsncpy(dir, path, len);
				*(dir + len) = '\0';
			}

			path = last_slash;
		}
		else if (dir) 
		{
			// no path found
			*dir = '\0';
		}

		// extract file name and extension, if any.  Path now points to the
		// first character of the file name, if any, or the extension if no
		// file name was given.  Dot points to the '.' beginning the extension,
		// if any.
		//

		if (dot && (dot >= path)) 
		{
			// found the marker for an extension - copy the file name up to
			// the '.'.
			//
			if (fname) 
			{
				len = std::min((int)((reinterpret_cast<wchar_t *>(dot) - const_cast<wchar_t *>(path)) ), (MAX_FNAME - 1));
				std::wcsncpy(fname, path, len);
				*(fname + len) = '\0';
			}
			// now we can get the extension - remember that p still points
			// to the terminating nul character of path.
			//
			if (ext) 
			{
				len = std::min((int)((reinterpret_cast<wchar_t *>(p) - reinterpret_cast<wchar_t *>(dot)) ), (MAX_EXT - 1));
				std::wcsncpy(ext, dot, len);
				*(ext + len) = '\0';
			}
		}
		else 
		{
			// found no extension, give empty extension and copy rest of
			// string into fname.
			//
			if (fname) 
			{
				len = std::min((int)(((wchar_t *)p - const_cast<wchar_t *>(path)) ), (MAX_FNAME - 1));
				std::wcsncpy(fname, path, len);
				*(fname + len) = '\0';
			}
			if (ext) 
			{
				*ext = '\0';
			}
		}
	}

	/////////////////////////////////////////////////////
	/// Function: IsEmptyString
	/// Params: [in]szString
	///
	/////////////////////////////////////////////////////
	bool core::IsEmptyString( const wchar_t *szString )
	{
		if( szString == 0 )
			return(true);

		if( szString[0] == '\0' )
			return(true);

		return(false);
	}


	/////////////////////////////////////////////////////
	/// Constructor
	/// Params: [in]szString
	///
	/////////////////////////////////////////////////////
	core::WideString::WideString( const char* szString )
	{
		m_WideString = 0;

		if( szString )
		{
			// convert to wide
			size_t len = std::strlen( szString );
			if( len > 0 )
			{
				m_WideString = new wchar_t[(len+1)];

				size_t size = std::mbstowcs( m_WideString, szString, len+1 );

				if( size != len )
				{
					DBGLOG( "CORE: *ERROR* Incorrect converted size of wide character in WideString\n" );
				}		
			}
		}
	}
#endif // BASE_SUPPORT_WCHAR

#ifdef BASE_DUMMY_APP
/////////////////////////////////////////////////////
/// Function: SetCursorPosition
/// Params: [in]nPosX, [in]nPosY
///
/////////////////////////////////////////////////////
void core::SetCursorPosition( int nPosX, int nPosY )
{

}
#endif // BASE_DUMMY_APP