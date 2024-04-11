
#ifndef __MICROGAMECARDSWIPE_H__
#define __MICROGAMECARDSWIPE_H__

#include "MicroGame.h"
#include "GameStates/UI/UIFileLoader.h"

class MicroGameCardSwipe : public MicroGame
{
	public:
		MicroGameCardSwipe( int cardIndex );
		virtual ~MicroGameCardSwipe();

		void Initialise();
		void Release();

		void OnEnter();
		void OnExit();

		void Update( float deltaTime );
		void Draw();

		bool IsComplete()		{ return m_Complete; }
		bool ReturnCode()		{ return m_ReturnCode; }

	private:
		mdl::ModelHGM* m_SwipeUIModel;

		UIFileLoader m_SwipeUI;

		ALuint m_UnlockAudio;
		ALuint m_ErrorAudio;
		ALuint m_ComputerLoopAudio;

		ALuint m_ComputerLoopSource;

		float m_ErrorAudioTimer;

		int m_CardIndex;
		math::Vec3 m_CardStartPos;

		bool m_SwipeActive;

		math::Vec3 m_ScrollTextPos;
		math::Vec3 m_ScrollText;

		bool m_SuccessMessage;
		float m_ShowMessageTime;
};

#endif // __MICROGAMECARDSWIPE_H__
