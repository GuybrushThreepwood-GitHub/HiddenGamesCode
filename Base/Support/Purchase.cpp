
/*===================================================================
	File: Purchase.cpp
	Library: Support

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "Purchase.h"

using namespace support;

Purchase* Purchase::ms_Instance = 0;

/////////////////////////////////////////////////////
/// Method: Create
/// Params: None
///
/////////////////////////////////////////////////////
void Purchase::Create( )
{
	DBG_ASSERT( (ms_Instance == 0) );
    
	DBG_MEMTRY
		ms_Instance = new Purchase;
	DBG_MEMCATCH
}

/////////////////////////////////////////////////////
/// Method: Destroy
/// Params: None
///
/////////////////////////////////////////////////////
void Purchase::Destroy( )
{
	if( ms_Instance )
	{
		delete ms_Instance;
		ms_Instance = 0;
	}
}


