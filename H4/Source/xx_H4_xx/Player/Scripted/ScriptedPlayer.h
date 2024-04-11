
/*===================================================================
	File: ScriptedPlayer.h
	Game: H4

	(C)Hidden Games
=====================================================================*/

#ifndef __SCRIPTEDPLAYER_H__
#define __SCRIPTEDPLAYER_H__

class ScriptedPlayer
{
	public:
		ScriptedPlayer();
		~ScriptedPlayer();

		void Setup();

		void Update( float deltatime );
		void Draw();

	private:
		mdl::ModelHGA* m_pModelData;
};

#endif // __SCRIPTEDPLAYER_H__

