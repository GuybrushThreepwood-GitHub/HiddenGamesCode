

#ifndef __MICROGAMECRIMINALDOCS_H__
#define __MICROGAMECRIMINALDOCS_H__

#include "MicroGame.h"
#include "GameStates/UI/UIFileLoader.h"

class MicroGameCriminalDocs : public MicroGame
{
	public:
		MicroGameCriminalDocs( int whichDoc );
		virtual ~MicroGameCriminalDocs();

		void Initialise();
		void Release();

		void OnEnter();
		void OnExit();

		void Update( float deltaTime );
		void Draw();

		bool IsComplete()		{ return m_Complete; }
		bool ReturnCode()		{ return m_ReturnCode; }

	private:
		mdl::ModelHGM* m_DocsModels;
		UIFileLoader m_DocsUI;

		ALuint m_PageFlip;

		bool m_NextEnabled;
		bool m_PreviousEnabled;

		int m_WhichDoc;
		int m_CurrentPage;
		int m_NumPages;
};

#endif // __MICROGAMECRIMINALDOCS_H__
