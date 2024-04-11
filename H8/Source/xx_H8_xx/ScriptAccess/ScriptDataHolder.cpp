
/*===================================================================
	File: ScriptDataHolder.cpp
	Game: H8

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "CollisionBase.h"
#include "Model/ModelCommon.h"
#include "ModelBase.h"

#include "ScriptAccess/ScriptDataHolder.h"

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
ScriptDataHolder::ScriptDataHolder()
{
	std::memset( &m_DevData, 0, sizeof(DevScriptData) );
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
ScriptDataHolder::~ScriptDataHolder()
{
	Release();
}

/////////////////////////////////////////////////////
/// Method: Release
/// Params: None
///
/////////////////////////////////////////////////////
void ScriptDataHolder::Release()
{

}

