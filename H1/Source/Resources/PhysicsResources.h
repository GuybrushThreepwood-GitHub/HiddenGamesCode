
#ifndef __PHYSICSRESOURCES_H__
#define __PHYSICSRESOURCES_H__

namespace res
{
	struct PhysicsBoxShape
	{
		PhysicsBoxShape()
		{
			geomId = 0;
			bodyId = 0;

			numeric1 = -9999;
			numeric2 = -9999;
			numeric3 = -9999;
			numeric4 = -9999;
			numeric5 = -9999;

			pos.setZero();
			rot.setZero();

			dims.setZero();
		}

		dGeomID geomId;
		dBodyID bodyId;

		int numeric1;
		int numeric2;
		int numeric3;
		int numeric4;
		int numeric5;

		math::Vec3 pos;
		math::Vec3 rot;

		math::Vec3 dims;
	};

	struct PhysicsSphereShape
	{
		PhysicsSphereShape()
		{
			geomId = 0;
			bodyId = 0;

			numeric1 = -9999;
			numeric2 = -9999;
			numeric3 = -9999;
			numeric4 = -9999;
			numeric5 = -9999;

			pos.setZero();

			radius = 0;
		}

		dGeomID geomId;
		dBodyID bodyId;

		int numeric1;
		int numeric2;
		int numeric3;
		int numeric4;
		int numeric5;

		math::Vec3 pos;
	
		float radius;
	};

	struct PhysicsCapsuleShape
	{
		PhysicsCapsuleShape()
		{
			geomId = 0;
			bodyId = 0;

			numeric1 = -9999;
			numeric2 = -9999;
			numeric3 = -9999;
			numeric4 = -9999;
			numeric5 = -9999;

			pos.setZero();
			rot.setZero();

			radius = 0;
			length = 0;
		}

		dGeomID geomId;
		dBodyID bodyId;

		int numeric1;
		int numeric2;
		int numeric3;
		int numeric4;
		int numeric5;

		math::Vec3 pos;
		math::Vec3 rot;

		float radius;
		float length;
	};

	struct PhysicsCylinderShape
	{
		PhysicsCylinderShape()
		{
			geomId = 0;
			bodyId = 0;

			numeric1 = -9999;
			numeric2 = -9999;
			numeric3 = -9999;
			numeric4 = -9999;
			numeric5 = -9999;

			pos.setZero();
			rot.setZero();

			radius = 0;
			length = 0;
		}

		dGeomID geomId;
		dBodyID bodyId;

		int numeric1;
		int numeric2;
		int numeric3;
		int numeric4;
		int numeric5;

		math::Vec3 pos;
		math::Vec3 rot;

		float radius;
		float length;
	};

	struct PhysicsTriMesh
	{
		PhysicsTriMesh()
		{
			geomId = 0;
			bodyId = 0;

			numeric1 = -9999;
			numeric2 = -9999;
			numeric3 = -9999;
			numeric4 = -9999;
			numeric5 = -9999;

			pos.setZero();
			rot.setZero();

			totalMeshTriangles = 0;

			triMeshDataID = 0;
			triangleData = 0;
			indices = 0;
			normals = 0;
		}

		dGeomID geomId;
		dBodyID bodyId;

		int numeric1;
		int numeric2;
		int numeric3;
		int numeric4;
		int numeric5;

		math::Vec3 pos;
		math::Vec3 rot;

		unsigned int totalMeshTriangles;

		dTriMeshDataID triMeshDataID;
		math::Vec3* triangleData;
		int* indices;
		math::Vec3* normals;
	};

	struct PhysicsBlock
	{
		unsigned int totalPhysicsBox;
		unsigned int totalPhysicsSphere;
		unsigned int totalPhysicsCapsule;
		unsigned int totalPhysicsCylinder;
		unsigned int totalPhysicsTriMesh;

		unsigned int totalPhysicsObjects;
	
		res::PhysicsBoxShape* boxList;
		res::PhysicsSphereShape* sphereList;
		res::PhysicsCapsuleShape* capsuleList;
		res::PhysicsCylinderShape* cylinderList;
		res::PhysicsTriMesh* triMeshList;
	};

	struct PhysicsResourceStore
	{
		res::PhysicsBlock* block;
		int modelResource;
		float density;
	};

	int LoadAndParseOde( const char* filename, res::PhysicsResourceStore& newResource );

	void CreatePhysicsResourceMap();

	void ClearPhysicsResources();
	
	const PhysicsResourceStore* GetPhysicsResource( int index );

	const std::map< int, PhysicsResourceStore >& GetPhysicsResourceMap();
}

#endif // __PHYSICSRESOURCES_H__
