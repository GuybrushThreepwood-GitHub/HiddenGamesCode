
/*===================================================================
	File: FreeTypeZipAccess.c
	Library: Core

	(C)Hidden Games
=====================================================================*/

#include "FreetypeZZipAccess.h"

const char* currentFreeTypeZipFile = 0;

/////////////////////////////////////////////////////
/// Function: SetFreeTypeZipFile
/// Params: [in]filename
///
/////////////////////////////////////////////////////
void SetFreeTypeZipFile( const char* filename )
{
	currentFreeTypeZipFile = filename;
}

/////////////////////////////////////////////////////
/// Function: GetFreeTypeZipFile
/// Params: None
///
/////////////////////////////////////////////////////
const char* GetFreeTypeZipFile( void )
{
	return currentFreeTypeZipFile;
}
