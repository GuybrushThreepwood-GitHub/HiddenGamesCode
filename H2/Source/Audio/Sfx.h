
#ifndef __SFX_H__
#define __SFX_H__

#include "SoundBase.h"

class Sfx
{
	public:
		Sfx();
		~Sfx();

		file::TFile FileInfo;
		char szLocator[32];

		/// sound position
		math::Vec3	vPos;
		/// sound rotation
		math::Vec3	vRot;
		/// sound velocity
		math::Vec3	vVel;
		/// sound direction
		math::Vec3	vDir;
		/// sound pitch
		float fPitch;
		/// sound gain
		float fGain;
		/// sound max distance before cutoff
		float fMaxDistance;
		/// sound rolloff factor
		float fRolloffFactor;
		/// sound reference distance
		float fReferenceDistance;
		/// sound minimum gain
		float fMinGain;
		/// sound maximum gain
		float fMaxGain;
		/// sound cone outer gain
		float fConeOuterGain;
		/// sound cone inner angle
		float fConeInnerAngle;
		/// sound cone outer angle
		float fConeOuterAngle;
		/// sound max sound range
		float fMaxSoundRange;
		/// sound range draw flag
		bool bDrawSoundRange;
		/// use AABB flag
		bool bUseAABB;
		/// AABB min X value
		float fMinX;
		/// AABB min Y value
		float fMinY;
		/// AABB min Z value
		float fMinZ;
		/// AABB max X value
		float fMaxX;
		/// AABB max Y value
		float fMaxY;
		/// AABB max Z value
		float fMaxZ;
		/// AABB trigger
		//collision::AABB aabbTrigger;
		/// sound source relative flag
		bool bSourceRelative;
		/// loop flag
		bool bLoopFlag;
		/// current AL state
		int nState;
		/// AL buffer object id
		ALuint nBufferID;
		/// AL source object id
		ALuint nSourceID;
};

#endif // __SFX_H__
