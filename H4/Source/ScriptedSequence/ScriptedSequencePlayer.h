
/*===================================================================
	File: ScriptedSequencePlayer.h
	Game: H4

	(C)Hidden Games
=====================================================================*/

#ifndef __SCRIPTEDSEQUENCEPLAYER_H__
#define __SCRIPTEDSEQUENCEPLAYER_H__

#include "Player/Scripted/ScriptedPlayer.h"
#include "Level/Level.h"
#include "Player/Player.h"
#include "NavigationMesh/NavigationMesh.h"

class ScriptedSequencePlayer
{
	public:
		ScriptedSequencePlayer();
		~ScriptedSequencePlayer();

		void ClearLevelData();
		void SetupLevelData( Level& activeLevel, NavigationMesh* pNavMesh );

		void Update( float deltaTime );
		void Draw();

	private:
		ScriptedPlayer* m_Player;

		Level* m_Level;
		NavigationMesh* m_NavMesh; 
};

#endif  // __SCRIPTEDSEQUENCEPLAYER_H__
