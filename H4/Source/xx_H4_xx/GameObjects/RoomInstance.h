
/*===================================================================
	File: RoomInstance.h
	Game: H4

	(C)Hidden Games
=====================================================================*/

#ifndef __ROOMINSTANCE_H__
#define __ROOMINSTANCE_H__

#include "AppConsts.h"

#include "BaseObject.h"

class RoomInstance
{
	public:
		RoomInstance( const char* modelFile, GLenum magFilter, GLenum minFilter, const math::Vec3& pos, float angle, const math::Vec3& scale );
		~RoomInstance();

		void Draw( bool sortedMeshes );

	private:
		mdl::ModelHGM* m_pModel;
		
		math::Vec3 m_Pos;
		math::Vec3 m_Scale;
		float m_Angle;

		collision::AABB m_AABB;
		math::Vec3 m_MinExtend;
		math::Vec3 m_MaxExtend;
};

#endif  // __ROOMINSTANCE_H__
