
#ifndef __BASESCRIPTSUPPORT_H__
#define __BASESCRIPTSUPPORT_H__

#ifdef BASE_SUPPORT_SCRIPTING

/// ScriptLoadScript - Loads a script file
/// \param pState - pointer to lua state
/// \return integer - number of values the function returns to lua
int ScriptLoadScript( lua_State* pState );

/// ScriptRandomNumber - Gets a random number between a min/max set of values
/// \param pState - pointer to lua state
/// \return integer - number of values the function returns to lua
int ScriptRandomNumber( lua_State* pState );

#endif // BASE_SUPPORT_SCRIPTING

#endif // __BASESCRIPTSUPPORT_H__

