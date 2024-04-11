
/*===================================================================
	File: EmitterResources.h
	Game: H4

	(C)Hidden Games
=====================================================================*/

#ifndef __EMITTERRESOURCES_H__
#define __EMITTERRESOURCES_H__

#include "Effects/Emitter.h"

namespace res
{
	struct EmitterResourceStore
	{
		efx::Emitter::EmitterBlock* block;
	};

	void CreateEmitterResourceMap();

	void ClearEmitterResources();
	
	const EmitterResourceStore* GetEmitterResource( int index );

	void SetupTexturesOnEmitter( efx::Emitter* pEmitter );
}

#endif // __EMITTERRESOURCES_H__
