
#ifndef __FREETYPEFONTZZIP_H__
#define __FREETYPEFONTZZIP_H__

#ifdef __cplusplus
extern "C" {
#endif 
    
#include <ft2build.h>
#include FT_CONFIG_CONFIG_H
    //#include <freetype/internal/ftdebug.h>
    //#include <freetype/internal/ftstream.h>
#include FT_SYSTEM_H
#include FT_TYPES_H
#include FT_ERRORS_H

/////////////////////////////////////////////////////
/// Function: SetFreeTypeZipFile
/// Params: [in]
///
/////////////////////////////////////////////////////
FT_BASE_DEF( FT_Error )
FT_Stream_Open( FT_Stream    stream,
                const char*  filepathname );

/////////////////////////////////////////////////////
/// Function: GetFreeTypeZipFile
/// Params: None
///
/////////////////////////////////////////////////////
FT_BASE_DEF( FT_Memory )
FT_New_Memory( void );

/////////////////////////////////////////////////////
/// Function: GetFreeTypeZipFile
/// Params: None
///
/////////////////////////////////////////////////////
FT_BASE_DEF( void )
    FT_Done_Memory( FT_Memory  memory );
    
#ifdef __cplusplus
}
#endif 

#endif // __FREETYPEFONTZZIP_H__

