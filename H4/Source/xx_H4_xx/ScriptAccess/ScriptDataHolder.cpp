
/*===================================================================
	File: ScriptDataHolder.cpp
	Game: H4

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "CollisionBase.h"
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

	std::memset( &m_CameraSetup, 0, sizeof(CameraSetup) );

	std::memset( &m_ActiveStage, 0, sizeof(StageDefinition) );
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
	unsigned int i=0;

	for( i=0; i < m_StageSetups.size(); ++i )
	{
		delete m_StageSetups[i];
	}

	m_StageSetups.clear();
}
