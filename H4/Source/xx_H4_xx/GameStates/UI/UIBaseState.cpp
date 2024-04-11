
/*===================================================================
	File: UIBaseState.cpp
	Game: H4

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "SoundBase.h"

#include "Audio/AudioSystem.h"
#include "GameStates/UI/UIBaseState.h"

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
UIBaseState::UIBaseState( int id )
{
	m_Id = id;

	m_SoundSource = snd::INVALID_SOUNDSOURCE;
	m_SoundBuffer = snd::INVALID_SOUNDBUFFER;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
UIBaseState::~UIBaseState()
{

}

/////////////////////////////////////////////////////
/// Method: PlaySound
/// Params: [in]id
///
/////////////////////////////////////////////////////
void UIBaseState::PlaySound( unsigned int id )
{
	// play the tick
	if( m_SoundSource == snd::INVALID_SOUNDSOURCE )
	{
		// grab a free source
		m_SoundSource = snd::SoundManager::GetInstance()->GetFreeSoundSource();

		// get the buffer id
		m_SoundBuffer = snd::INVALID_SOUNDBUFFER;

		if( m_SoundSource != snd::INVALID_SOUNDSOURCE && m_SoundBuffer != snd::INVALID_SOUNDBUFFER )
		{
			AudioSystem::GetInstance()->PlayAudio( m_SoundSource, m_SoundBuffer, math::Vec3(0.0f,0.0f,0.0f), AL_TRUE );

			m_SoundSource = snd::INVALID_SOUNDSOURCE;
		}
	}
}
