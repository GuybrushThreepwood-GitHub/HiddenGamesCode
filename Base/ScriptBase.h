
#ifndef __SCRIPTBASE_H__
#define __SCRIPTBASE_H__

#ifdef BASE_SUPPORT_SCRIPTING

extern "C"
{
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>
} // extern "C"

#ifndef __SCRIPTCONSTS_H__
	#include "Script/ScriptConsts.h"
#endif // __SCRIPTCONSTS_H__

#ifndef __LUASCRIPTING_H__
	#include "Script/LuaScripting.h"
#endif // __LUASCRIPTING_H__

#ifndef __BASESCRIPTSUPPORT_H__
	#include "Script/BaseScriptSupport.h"
#endif // __BASESCRIPTSUPPORT_H__


#ifdef BASE_SUPPORT_BOX2D
	#ifndef __PHYSICSACCESSB2D_H__
		#include "Script/box2d/PhysicsAccessB2D.h"
	#endif // __PHYSICSACCESSB2D_H__
#endif // BASE_SUPPORT_BOX2D

#ifndef __INPUTACCESS_H__
    #include "Script/input/InputAccess.h"
#endif // __INPUTACCESS_H__

#endif // BASE_SUPPORT_SCRIPTING

#endif // __SCRIPTBASE_H__