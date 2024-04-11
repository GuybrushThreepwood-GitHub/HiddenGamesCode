
/*===================================================================
	File: SoundAccess.cpp
	Game: H4

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "SoundBase.h"
#include "CollisionBase.h"
#include "ModelBase.h"
#include "InputBase.h"
#include "ScriptBase.h"

#include "Audio/AudioSystem.h"
#include "Resources/SoundResources.h"

#include "ScriptAccess/Sound/SoundAccess.h"


/////////////////////////////////////////////////////
/// Function: RegisterSoundFunctions
/// Params: 
///
/////////////////////////////////////////////////////
void RegisterSoundFunctions( )
{
	script::LuaScripting::GetInstance()->RegisterFunction( "PlaySound",			ScriptPlaySound );
}

/////////////////////////////////////////////////////
/// Function: ScriptPlaySound
/// Params: 
///
/////////////////////////////////////////////////////
int ScriptPlaySound( lua_State* pState )
{
	const char* sr = 0;
	snd::Sound sndLoad;
	ALuint bufferId = snd::INVALID_SOUNDBUFFER;
	math::Vec3 pos( 0.0f, 0.0f, 0.0f );

	int soundId = static_cast<int>( lua_tonumber( pState, 1 ) );
	pos.X = static_cast<float>( lua_tonumber( pState, 2 ) );
	pos.Y = static_cast<float>( lua_tonumber( pState, 3 ) );
	pos.Z = static_cast<float>( lua_tonumber( pState, 4 ) );
	float pitch = static_cast<float>( lua_tonumber( pState, 5 ) );
	float gain = static_cast<float>( lua_tonumber( pState, 6 ) );

	bool loop = false;
	if( lua_isboolean( pState, 7 ) )
		loop = (lua_toboolean( pState, 7 )!=0);

	sr = res::GetSoundResource( soundId );

	if( sr )
	{
		bufferId = snd::SoundLoad( sr, sndLoad );
		DBG_ASSERT( (bufferId != snd::INVALID_SOUNDBUFFER) );

		if( loop )
			AudioSystem::GetInstance()->PlayAudio( bufferId, pos, true, true, pitch, gain );
		else
			AudioSystem::GetInstance()->PlayAndRemove( bufferId, pos, true, false, pitch, gain );
	}

	return(0);
}

