
/*===================================================================
	File: RayObject.h
	Game: H5

	(C)Hidden Games
=====================================================================*/

#ifndef __RAYOBJECT_H__
#define __RAYOBJECT_H__

#include "ScriptBase.h"
#include "ModelBase.h"

#include "Physics/PhysicsIds.h"

const int MAX_RAY_HITS = 5;

class RayObject : public physics::PhysicsIdentifier 
{
	public:
		RayObject( dSpaceID spaceId, float length );
		virtual ~RayObject();

		dGeomID GetRayGeom()		{ return rayGeom; }

	public:
		bool hasHits;
		int totalHits;
		int nextHitIndex;

		dGeomID rayGeom;
		math::Vec3 rayPos;
		math::Vec3 rayTarget;
		math::Vec3 rayDir;
		float rayLength;

		struct RayHits
		{
			dGeomID otherGeom;
			math::Vec3 pos;
			math::Vec3 n;
			float distance;

			int hitObject;
			void* pData;
		};

		RayHits rayHits[MAX_RAY_HITS];
};

bool FacingCamera( const math::Vec3& cameraDir, const math::Vec3& n );

#endif // __RAYOBJECT_H__
