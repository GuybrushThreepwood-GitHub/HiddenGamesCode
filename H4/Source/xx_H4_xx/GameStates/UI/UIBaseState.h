
/*===================================================================
	File: UIBaseState.h
	Game: H4

	(C)Hidden Games
=====================================================================*/

#ifndef __UIBASESTATE_H__
#define __UIBASESTATE_H__

namespace ui { class CUIStaticText; }

class UIBaseState
{
	public:
		UIBaseState( int ident );
		virtual ~UIBaseState();

		void PlaySound( unsigned int id );
		
		int GetId()							{ return m_Id; }

	protected:
		/// storage for the GL state
		renderer::TGLDepthBlendAlphaState m_DBAState;
		int m_Id;

		ALuint m_SoundSource;
		ALuint m_SoundBuffer;
};

#endif // __UIBASESTATE_H__
