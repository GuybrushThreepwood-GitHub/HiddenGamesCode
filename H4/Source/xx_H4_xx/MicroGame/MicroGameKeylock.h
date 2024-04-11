
#ifndef __MICROGAMEKEYLOCK_H__
#define __MICROGAMEKEYLOCK_H__

#include "MicroGame.h"
#include "GameStates/UI/UIFileLoader.h"

class MicroGameKeylock : public MicroGame
{
	public:
		MicroGameKeylock( int keyIndex );
		virtual ~MicroGameKeylock();

		void Initialise();
		void Release();

		void OnEnter();
		void OnExit();

		void Update( float deltaTime );
		void Draw();

		bool IsComplete()		{ return m_Complete; }
		bool ReturnCode()		{ return m_ReturnCode; }

	private:
		mdl::ModelHGM* m_KeylockModel;
		UIFileLoader m_KeylockUI;

		ALuint m_KeyClickAudio;

		math::Vec3 m_KeyStartPos;

		int m_KeyIndex;

		bool m_KeyInLock;
		bool m_HoldingKey;

		float m_Range;

		int m_TurnValue;
		int m_TurnAngle;

};

#endif // __MICROGAMEKEYLOCK_H__
