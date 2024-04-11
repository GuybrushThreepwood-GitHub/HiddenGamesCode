
/*===================================================================
	File: Event.h
	Game: H4

	(C)Hidden Games
=====================================================================*/

#ifndef __EVENT_H__
#define __EVENT_H__

const unsigned int EVENTID_INVALID = (~0);
const unsigned int EVENTTYPE_UNKNOWN = (~0);

class IEvent
{
	public:
		enum EventState
		{
			EVENTSTATE_IDLE=-1,

			EVENTSTATE_WAIT=0,

			EVENTSTATE_ENTER=1,
			EVENTSTATE_PROCESS,
			EVENTSTATE_COMPLETE,
			EVENTSTATE_EXIT,
		};

	public:
		IEvent();
		virtual ~IEvent();

		virtual int Wait()			{ return(1); }

		virtual int OnEnter()		{ return(1); }
		virtual int Process()		{ return(1); }
		virtual int OnComplete()	{ return(1); }
		virtual int OnExit()		{ return(1); }

		unsigned int GetEventId()			{ return m_EventId; }
		unsigned int GetEventType()			{ return m_EventType; }

		void SetState( EventState state )	{ m_CurrentState = state; }
		EventState GetState()				{ return m_CurrentState; }

		void SetNextState( EventState state )	{ m_NextState = state; }
		EventState GetNextState()				{ return m_NextState; }

	private:
		unsigned int m_EventId;
		unsigned int m_EventType;

		EventState m_CurrentState;
		EventState m_NextState;
};

#endif // __EVENT_H__

