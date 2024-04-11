
/*===================================================================
	File: ScriptedSequencePlayer.cpp
	Game: H4

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "SoundBase.h"
#include "CollisionBase.h"
#include "ModelBase.h"
#include "InputBase.h"
#include "ScriptBase.h"

#include "ScriptedSequence/ScriptedSequencePlayer.h"

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
ScriptedSequencePlayer::ScriptedSequencePlayer()
{
	m_Player = new ScriptedPlayer();
	DBG_ASSERT( (m_Player != 0) );

	m_Player->Setup();

	m_Level = 0;
	m_NavMesh = 0;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
ScriptedSequencePlayer::~ScriptedSequencePlayer()
{
	if( m_Player != 0 )
	{
		delete m_Player;
		m_Player = 0;
	}
}

/////////////////////////////////////////////////////
/// Method: ClearLevelData
/// Params: None
///
/////////////////////////////////////////////////////
void ScriptedSequencePlayer::ClearLevelData()
{
	m_Level = 0;
	m_NavMesh = 0; 
}

/////////////////////////////////////////////////////
/// Method: SetupLevelData
/// Params: [in]activeLevel
///
/////////////////////////////////////////////////////
void ScriptedSequencePlayer::SetupLevelData( Level& activeLevel, NavigationMesh* pNavMesh  )
{
	m_Level = &activeLevel;
	m_NavMesh = pNavMesh;
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void ScriptedSequencePlayer::Update( float deltaTime )
{
	m_Player->Update( deltaTime );
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void ScriptedSequencePlayer::Draw()
{
	m_Player->Draw();
}

