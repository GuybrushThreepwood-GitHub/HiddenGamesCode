
/*===================================================================
	File: Sfx.cpp
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "SoundBase.h"

#include "Audio/Sfx.h"

Sfx::Sfx()
{
	std::memset( &FileInfo, 0, sizeof(file::TFile) );
	std::memset( szLocator, 0, sizeof(char)*32 );

	vPos = math::Vec3( 0.0f, 0.0f, 0.0f );
	vRot = math::Vec3( 0.0f, 0.0f, 0.0f );
	vVel = math::Vec3( 0.0f, 0.0f, 0.0f );
	vDir = math::Vec3( 0.0f, 0.0f, 0.0f );

	fPitch				= 1.0f;
	fGain				= 1.0f;
	fMaxDistance		= 0.0f;
	fRolloffFactor		= 1.0f;
	fReferenceDistance	= 1.0f;
	fMinGain			= 1.0f;
	fMaxGain			= 1.0f;
	fConeOuterGain		= 1.0f;
	fConeInnerAngle		= 360.0f;
	fConeOuterAngle		= 360.0f;

	fMaxSoundRange = 100.0f;
	bDrawSoundRange = false;

	fMinX = 1.0f;
	fMinY = 1.0f;
	fMinZ = 1.0f;
	fMaxX = 1.0f;
	fMaxY = 1.0f;
	fMaxZ = 1.0f;

	bSourceRelative		= AL_FALSE;
	bLoopFlag			= AL_FALSE;
	nState				= AL_STOPPED;

	nSourceID			= snd::INVALID_SOUNDSOURCE;
	nBufferID			= snd::INVALID_SOUNDBUFFER;
}

Sfx::~Sfx()
{

}
