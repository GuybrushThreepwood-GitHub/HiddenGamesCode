
/*===================================================================
	File: WorldObject.h
	Game: AirCadets

	(C)Hidden Games
=====================================================================*/

#ifndef __WORLDOBJECT_H__
#define __WORLDOBJECT_H__

#include "ScriptBase.h"
#include "CollisionBase.h"
#include "ModelBase.h"
#include "SoundBase.h"
#include "PhysicsBase.h"

#include "Resources/PhysicsResources.h"
#include "Level/LevelTypes.h"
#include "Physics/PhysicsIds.h"

class WorldObject : public physics::PhysicsIdentifier
{
	public:
		WorldObject();
		virtual ~WorldObject();

		int Create( const math::Vec3& pos, const math::Vec3& rot, const math::Vec3& normal, int worldType, int resId, int groupId, bool snapToGrid, bool orientateToGrid, float lodScaler );

		void Draw();
		void DrawPhysicsData();
		void DrawAABB();
		void DrawSphere();

		void Update( float deltaTime, const math::Vec3& camPos );
		void CalculateUnprojectedPos();

		void Destroy();

		// position
		void SetPos( const math::Vec3& pos, bool adjustWithCOM=true );
		const math::Vec3& GetPos() const;		
		const math::Vec3& GetPosCOM() const;

		// rotation
		void SetRot( const math::Vec3& rot );
		const math::Vec3& GetRot() const;		

		void SetCreationPos( const math::Vec3& pos );
		const math::Vec3& GetCreationPos( )				{ return m_CreationPos; }

		void SetCreationRot( const math::Vec3& rot );
		const math::Vec3& GetCreationRot( )				{ return m_CreationRot; }

		int GetResId()									{ return m_ResId; }
		math::Vec3& GetUnprojection()					{ return m_UnProjectedPoint; }

		void SetCategoryBits( unsigned long bits );
		unsigned long GetCategoryBits()					{ return m_CategoryBits; }

		void SetCollideBits( unsigned long bits );
		unsigned long GetCollideBits()					{ return m_CollideBits; }

		void SetCurrentDistance( float dist )			{ m_CurrentDistance = dist; }
		float GetCurrentDistance()						{ return m_CurrentDistance; }

		const collision::Sphere& GetSphere()			{ return m_Sphere; }

		bool CanDestroy()								{ return m_CanDestroy; }
		void SetDamage( int damage )					{ m_Damage = damage; }
		int GetDamage()									{ return m_Damage; }
		bool IsDestroyed()								{ return m_Destroyed; }

		void SetTarget()								{ m_IsTarget = true; }
		bool IsTarget()									{ return m_IsTarget; }

		bool IsDrawn()									{ return m_IsDrawn; }

		void Reset();

	private:
		int LoadAndParseOde( const char* szFilename );
		dGeomID CreateGeom( int shapeClass, dSpaceID spaceId, const math::Vec3& dims, float radius=0.0f, float length=0.0f, dTriMeshDataID triMeshDataId=0 );

		void EnablePhysics();
		void DisablePhysics();

	private:
		float m_Density;

		res::PhysicsBlock* m_PhysicsData;
		dSpaceID m_EntitySpace;
		int m_ResId;

		bool m_ProcessCollisions;

		bool m_FlagBitChange;
		unsigned long m_NewCategoryBits;
		unsigned long m_NewCollideBits;

		unsigned long m_CategoryBits;
		unsigned long m_CollideBits;

		bool m_ModelOnly;
		mdl::ModelHGM* m_pModelHigh;
		mdl::ModelHGM* m_pModelMedium;
		mdl::ModelHGM* m_pModelLow;

		float m_DisableDistance;
		float m_HighDistance;
		float m_MediumDistance;
		float m_LowDistance;

		float m_CurrentDistance;

		bool m_IsDrawn;
		bool m_PhysicsEnabled;

		math::Vec3 m_CreationPos;
		math::Vec3 m_CreationRot;

		math::Vec3 m_Pos;
		math::Vec3 m_PosCOM;

		math::Vec3 m_Rot;
		dMatrix3 m_Orientation;

		math::Vec3 m_COM;
		collision::AABB m_AABB; // default AABB when object is created
		collision::Sphere m_Sphere;

		bool m_CanDestroy;
		int m_Damage;
		bool m_Destroyed;
		bool m_IsTarget;

		math::Vec3 m_UnProjectedPoint;
};

/////////////////////////////////////////////////////
/// Method: GetPos
/// Params: None
///
/////////////////////////////////////////////////////
inline const math::Vec3& WorldObject::GetPos() const
{
	return m_Pos;
}

/////////////////////////////////////////////////////
/// Method: GetPosCOM
/// Params: None
///
/////////////////////////////////////////////////////
inline const math::Vec3& WorldObject::GetPosCOM() const
{
	return m_PosCOM;
}

/////////////////////////////////////////////////////
/// Method: GetRot
/// Params: None
///
/////////////////////////////////////////////////////
inline const math::Vec3& WorldObject::GetRot() const
{
	return m_Rot;
}

#endif // __WORLDOBJECT_H__

