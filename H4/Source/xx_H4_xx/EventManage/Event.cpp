
/*===================================================================
	File: Event.cpp
	Game: H4

	(C)Hidden Games
=====================================================================*/

#include "Event.h"

IEvent::IEvent()
{
	m_EventId = EVENTID_INVALID;
	m_EventType = EVENTTYPE_UNKNOWN;

	m_CurrentState = EVENTSTATE_IDLE;
	m_NextState = EVENTSTATE_IDLE;
}

IEvent::~IEvent()
{

}
