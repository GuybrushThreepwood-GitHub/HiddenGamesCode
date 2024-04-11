
/*===================================================================
	File: UIBaseState.h
	Game: AirCadets

	(C)Hidden Games
=====================================================================*/

#ifndef __UIBASESTATE_H__
#define __UIBASESTATE_H__

class UIBaseState
{
	public:
		UIBaseState();
		virtual ~UIBaseState();

	protected:
		bool m_FinishedTransitionIn;
		bool m_FinishedTransitionOut;

		int m_FadeTransition;
};

#endif // __UIBASESTATE_H__
