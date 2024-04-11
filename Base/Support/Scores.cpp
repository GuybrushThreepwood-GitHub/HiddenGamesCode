
/*===================================================================
	File: Scores.cpp
	Library: Support

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "ScriptBase.h"
#include "Scores.h"

using namespace support;

Scores* Scores::ms_Instance = 0;

/////////////////////////////////////////////////////
/// Method: Create
/// Params: None
///
/////////////////////////////////////////////////////
void Scores::Create( support::ScoresCallback* callback )
{
	DBG_ASSERT( (ms_Instance == 0) );
    
	DBG_MEMTRY
        ms_Instance = new Scores( callback );
	DBG_MEMCATCH
}

/////////////////////////////////////////////////////
/// Method: Destroy
/// Params: None
///
/////////////////////////////////////////////////////
void Scores::Destroy( )
{
	if( ms_Instance )
	{
		delete ms_Instance;
		ms_Instance = 0;
	}
}

