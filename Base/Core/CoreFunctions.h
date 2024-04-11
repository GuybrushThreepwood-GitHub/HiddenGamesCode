
#ifndef __COREFUNCTIONS_H__
#define __COREFUNCTIONS_H__

namespace core
{
	/// SplitPath - Splits a path into its drive/directory/filename/extension
	/// \param path - The path you want split
	/// \param drive - pointer to store the returned drive string
	/// \param dir - pointer to store the returned directory string
	/// \param fname - pointer to store the returned filename string
	/// \param ext - pointer to store the returned file extension string
	void SplitPath( const char *path, char *drive, char *dir, char *fname, char *ext );
	/// GetDateAndTime - Returns the date and time as a string
	/// \return char * - string containing time and date
	char *GetDateAndTime( void );
	/// IsEmptyString - Checks to see is a string pointer is 0
	/// \param szString - string to test
	/// \return boolean - true or false
	bool IsEmptyString( const char *szString );

#ifdef BASE_SUPPORT_WCHAR
	/// SplitPath - Splits a path into its drive/directory/filename/extension (wide support)
	/// \param path - The path you want split
	/// \param drive - pointer to store the returned drive string
	/// \param dir - pointer to store the returned directory string
	/// \param fname - pointer to store the returned filename string
	/// \param ext - pointer to store the returned file extension string
	void SplitPath( const wchar_t *path, wchar_t *drive, wchar_t *dir, wchar_t *fname, wchar_t *ext );
	/// IsEmptyString - Checks to see is a string pointer is 0 (unicode support)
	/// \param szString - wide string to test
	/// \return boolean - true or false
	bool IsEmptyString( const wchar_t *szString );
	/// WideString - class to manage conversions from multibyte to wide strings
	class WideString
	{
		public:
			WideString( const char* szString );

			~WideString()
			{
				if( m_WideString )
				{
					delete[] m_WideString;
					m_WideString = 0;
				}
			}

			wchar_t* WideString::GetString()
			{
				return( m_WideString );
			}

		private:
			wchar_t* m_WideString;
	};
#endif // BASE_SUPPORT_WCHAR

	/// GetScreenCenter - Gets the center x, y position of the screen
	/// \param pScreenX - holds the returned x position of the screen center
	/// \param pScreenY - holds the returned y position of the screen center
	void GetScreenCenter( int *pScreenX, int *pScreenY );
	/// SetCursorPosition - Sets the mouse cursor position
	/// \param nPosX - X position to set the mouse
	/// \param nPosY - Y position to set the mouse
	void SetCursorPosition( int nPosX, int nPosY );

} // namespace core

#endif // __COREFUNCTIONS_H__

