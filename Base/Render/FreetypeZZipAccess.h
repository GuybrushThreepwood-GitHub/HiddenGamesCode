
#ifndef __FREETYPEZZIPACCESS_H__
#define __FREETYPEZZIPACCESS_H__

#ifdef __cplusplus
extern "C" {
#endif 
    
/////////////////////////////////////////////////////
/// Function: SetFreeTypeZipFile
/// Params: [in]
///
/////////////////////////////////////////////////////
void SetFreeTypeZipFile( const char* filename );

/////////////////////////////////////////////////////
/// Function: GetFreeTypeZipFile
/// Params: None
///
/////////////////////////////////////////////////////
const char* GetFreeTypeZipFile( void );

#ifdef __cplusplus
}
#endif 

#endif // __FREETYPEZZIPACCESS_H__

