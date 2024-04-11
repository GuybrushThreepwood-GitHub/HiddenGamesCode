
#ifndef __MICROGAMETHUMBSCAN_H__
#define __MICROGAMETHUMBSCAN_H__

#include "MicroGame.h"
#include "GameStates/UI/UIFileLoader.h"

class MicroGameThumbScan : public MicroGame
{
	public:
		MicroGameThumbScan( );
		virtual ~MicroGameThumbScan();

		void Initialise();
		void Release();

		void OnEnter();
		void OnExit();

		void Update( float deltaTime );
		void Draw();

		bool IsComplete()		{ return m_Complete; }
		bool ReturnCode()		{ return m_ReturnCode; }

	private:
		mdl::ModelHGM* m_ThumbUIModel;

		UIFileLoader m_ThumbUI;

		ALuint m_ScanAudio;
		ALuint m_ScanAudioSource;

		ALuint m_ErrorAudio;
		ALuint m_SuccessAudio;

		math::Vec3 m_ScanPos;
		bool m_SwapDir;

		bool m_ScanActive;
		bool m_SuccessMessage;

		float m_ShowMessageTime;
};

#endif // __MICROGAMETHUMBSCAN_H__
