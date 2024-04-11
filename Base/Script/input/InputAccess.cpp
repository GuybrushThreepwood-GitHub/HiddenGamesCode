
/*===================================================================
	File: PhysicsAccessB2D.cpp
	Library: Script

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "ScriptBase.h"
#include "Input/InputDeviceController.h"
#include "Script/input/InputAccess.h"

script::LuaGlobal sdlInputGlobals[] = 
{
	// input access
	{ "SDL_CONTROLLER_BUTTON_A", static_cast<int>(SDL_CONTROLLER_BUTTON_A), LUA_TNUMBER },
	{ "SDL_CONTROLLER_BUTTON_B", static_cast<int>(SDL_CONTROLLER_BUTTON_B), LUA_TNUMBER },
	{ "SDL_CONTROLLER_BUTTON_X", static_cast<int>(SDL_CONTROLLER_BUTTON_X), LUA_TNUMBER },
	{ "SDL_CONTROLLER_BUTTON_Y", static_cast<int>(SDL_CONTROLLER_BUTTON_Y), LUA_TNUMBER },
	{ "SDL_CONTROLLER_BUTTON_BACK", static_cast<int>(SDL_CONTROLLER_BUTTON_BACK), LUA_TNUMBER },
	{ "SDL_CONTROLLER_BUTTON_GUIDE", static_cast<int>(SDL_CONTROLLER_BUTTON_GUIDE), LUA_TNUMBER },
	{ "SDL_CONTROLLER_BUTTON_START", static_cast<int>(SDL_CONTROLLER_BUTTON_START), LUA_TNUMBER },
	{ "SDL_CONTROLLER_BUTTON_LEFTSTICK", static_cast<int>(SDL_CONTROLLER_BUTTON_LEFTSTICK), LUA_TNUMBER },
	{ "SDL_CONTROLLER_BUTTON_RIGHTSTICK", static_cast<int>(SDL_CONTROLLER_BUTTON_RIGHTSTICK), LUA_TNUMBER },
	{ "SDL_CONTROLLER_BUTTON_LEFTSHOULDER", static_cast<int>(SDL_CONTROLLER_BUTTON_LEFTSHOULDER), LUA_TNUMBER },
	{ "SDL_CONTROLLER_BUTTON_RIGHTSHOULDER", static_cast<int>(SDL_CONTROLLER_BUTTON_RIGHTSHOULDER), LUA_TNUMBER },
	{ "SDL_CONTROLLER_BUTTON_DPAD_UP", static_cast<int>(SDL_CONTROLLER_BUTTON_DPAD_UP), LUA_TNUMBER },
	{ "SDL_CONTROLLER_BUTTON_DPAD_DOWN", static_cast<int>(SDL_CONTROLLER_BUTTON_DPAD_DOWN), LUA_TNUMBER },
	{ "SDL_CONTROLLER_BUTTON_DPAD_LEFT", static_cast<int>(SDL_CONTROLLER_BUTTON_DPAD_LEFT), LUA_TNUMBER },
	{ "SDL_CONTROLLER_BUTTON_DPAD_RIGHT", static_cast<int>(SDL_CONTROLLER_BUTTON_DPAD_RIGHT), LUA_TNUMBER },

	{ "SDL_CONTROLLER_AXIS_LEFTX", static_cast<int>(SDL_CONTROLLER_AXIS_LEFTX), LUA_TNUMBER },
	{ "SDL_CONTROLLER_AXIS_LEFTY", static_cast<int>(SDL_CONTROLLER_AXIS_LEFTY), LUA_TNUMBER },
	{ "SDL_CONTROLLER_AXIS_RIGHTX", static_cast<int>(SDL_CONTROLLER_AXIS_RIGHTX), LUA_TNUMBER },
	{ "SDL_CONTROLLER_AXIS_RIGHTY", static_cast<int>(SDL_CONTROLLER_AXIS_RIGHTY), LUA_TNUMBER },
	{ "SDL_CONTROLLER_AXIS_TRIGGERLEFT", static_cast<int>(SDL_CONTROLLER_AXIS_TRIGGERLEFT), LUA_TNUMBER },
	{ "SDL_CONTROLLER_AXIS_TRIGGERRIGHT", static_cast<int>(SDL_CONTROLLER_AXIS_TRIGGERRIGHT), LUA_TNUMBER }
};

/////////////////////////////////////////////////////
/// Function: RegisterInputFunctions
/// Params: None
///
/////////////////////////////////////////////////////
void script::RegisterInputFunctions()
{
	int i=0;

	// setup globals
	for (i = 0; i < sizeof(sdlInputGlobals) / sizeof(script::LuaGlobal); ++i)
	{
		lua_pushnumber(script::LuaScripting::GetState(), sdlInputGlobals[i].nConstantValue);
		lua_setglobal(script::LuaScripting::GetState(), sdlInputGlobals[i].szConstantName);
	}

	// create 32 generic buttons
	for (int i = 0; i < 32; ++i)
	{
		char tmpName[64];
		std::memset(tmpName, 0, sizeof(char) * 64);

		snprintf(tmpName, 64, "JOYSTICK_BUTTON_%d", i);

		lua_pushnumber(script::LuaScripting::GetState(), i);
		lua_setglobal(script::LuaScripting::GetState(), tmpName);
	}

	// create 8 generic axis
	for (int i = 0; i < 8; ++i)
	{
		char tmpName[64];
		std::memset(tmpName, 0, sizeof(char) * 64);

		snprintf(tmpName, 64, "JOYSTICK_AXIS_%d", i);

		lua_pushnumber(script::LuaScripting::GetState(), i);
		lua_setglobal(script::LuaScripting::GetState(), tmpName);
	}
	//script::LuaScripting::GetInstance()->RegisterFunction( "CreateBody",			script::ScriptCreateBody );
}

