
/*===================================================================
	File: WorldObject.cpp
	Game: AirCadets

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "CollisionBase.h"
#include "RenderBase.h"
#include "PhysicsBase.h"
#include "ModelBase.h"
#include "ScriptBase.h"
#include "SoundBase.h"

#include "Resources/ModelResources.h"
#include "Resources/TextureResources.h"
#include "Resources/ResourceHelper.h"
#include "Resources/ObjectResources.h"

#include "Physics/PhysicsContact.h"

#include "Audio/AudioSystem.h"
#include "GameObjects/WorldObject.h"
#include "GameSystems.h"


namespace
{
	GLfloat m[16] = 
	{ 1.0f, 0.0f, 0.0f, 0.0f,
	  0.0f, 1.0f, 0.0f, 0.0f,
	  0.0f, 0.0f, 1.0f, 0.0f,
	  0.0f, 0.0f, 0.0f, 1.0f };

	math::Vec3 zeroVec(0.0f,0.0f,0.0f);
	const float DEFAULT_DENSITY = 0.15f;
	const int DEFAULT_DAMAGE = 5;

}


///
void PhysicsTestObjectToWorld (void *data, dGeomID o1, dGeomID o2);

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
WorldObject::WorldObject( )
	: PhysicsIdentifier(PHYSICSBASICID_WORLDOBJECT, PHYSICSCASTID_WORLDOBJECT)
{
	m_PhysicsData = 0;
	m_EntitySpace = 0;
	m_ResId = -1;

	m_CategoryBits = 0;
	m_CollideBits = 0;

	m_Density = DEFAULT_DENSITY;

	m_ModelOnly = false;
	m_pModelHigh = 0;
	m_pModelMedium = 0;
	m_pModelLow = 0;

	m_DisableDistance = 100.0f;
	m_LowDistance = 50.0f;
	m_MediumDistance = 30.0f;
	m_HighDistance = 20.0f;
	m_CurrentDistance = 0.0f;

	m_EntitySpace = 0;

	m_Pos.setZero();
	m_PosCOM.setZero();
	m_Rot.setZero();
	m_COM.setZero();

	m_CreationPos.setZero();
	m_CreationRot.setZero();

	m_ProcessCollisions = true;
	m_FlagBitChange = false;
	m_NewCategoryBits = CATEGORY_NOTHING;
	m_NewCollideBits = CATEGORY_NOTHING2;

	m_CategoryBits = CATEGORY_WORLDOBJECT;
	m_CollideBits = (CATEGORY_RAYCAST |  CATEGORY_BULLET | CATEGORY_PLAYER);
	m_PhysicsEnabled = true;
	m_IsDrawn = false;
	m_AABB.Reset();

	m_CanDestroy = true;
	m_Damage = DEFAULT_DAMAGE;
	m_Destroyed = false;
	m_IsTarget = false;

	m_UnProjectedPoint = math::Vec3( -10000.0f, -10000.0f, -10000.0f );
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
WorldObject::~WorldObject()
{
	Destroy();
}

/////////////////////////////////////////////////////
/// Method: Create
/// Params: None
///
/////////////////////////////////////////////////////
int WorldObject::Create( const math::Vec3& pos, const math::Vec3& rot, const math::Vec3& terrainNormal, int worldType, int resId, int groupId, bool snapToGrid, bool orientateToGrid, float lodScaler )
{
	unsigned int i=0;

	const res::ObjectResourceStore* objRes = res::GetObjectResource( resId );
	DBG_ASSERT( objRes != 0 );

	m_ResId = resId;
	m_CanDestroy = objRes->destroyFlag;

	dMatrix3 mat;
	dQuaternion qTerrainOri, qGeomOri, qNodeOri, qFinalOri, qTmp;
	m_Pos = pos;
	m_Rot.X = rot.X;
	m_Rot.Y = rot.Y;
	m_Rot.Z = rot.Z;

	if( objRes != 0 )
	{
		m_EntitySpace = physics::PhysicsWorldODE::GetSpace();

		if( !core::IsEmptyString(objRes->physicsFile) )
		{
			if( LoadAndParseOde(objRes->physicsFile) )
				return 1;
		}

		math::Vec3 cross;
		math::Vec3 worldUp( 0.0f, 1.0f, 0.0f );
		math::Vec3 forwardVec( 0.0f, 0.0f, 1.0f );
		math::Vec3 finalForwardVec = zeroVec;

		// rotate the heading by the node rotation
		dRSetIdentity( m_Orientation );
		dRFromEulerAngles( m_Orientation, math::DegToRad(m_Rot.X), math::DegToRad(m_Rot.Y), math::DegToRad(m_Rot.Z) );

		finalForwardVec.X = (forwardVec.X * m_Orientation[0]) + (forwardVec.Y * m_Orientation[1]) + (forwardVec.Z * m_Orientation[2]) + zeroVec.X;
		finalForwardVec.Y = (forwardVec.X * m_Orientation[4]) + (forwardVec.Y * m_Orientation[5]) + (forwardVec.Z * m_Orientation[6]) + zeroVec.Y;
		finalForwardVec.Z = (forwardVec.X * m_Orientation[8]) + (forwardVec.Y * m_Orientation[9]) + (forwardVec.Z * m_Orientation[10]) + zeroVec.Z;

		finalForwardVec.normalise();

		// heading
		float angle = math::AngleBetweenVectors(finalForwardVec,forwardVec);
		cross = math::CrossProduct(forwardVec,finalForwardVec);
		cross.normalise();

		dQSetIdentity( qNodeOri );
		dQFromAxisAndAngle( qNodeOri, cross.X, cross.Y, cross.Z, angle );

		// up vector
		angle = math::AngleBetweenVectors(worldUp,terrainNormal);
		cross = math::CrossProduct(terrainNormal,worldUp);
		cross.normalise();

		// terrain slope orientation
		dQSetIdentity( qTerrainOri );
		dQFromAxisAndAngle( qTerrainOri, cross.X, cross.Y, cross.Z, angle );

		if( orientateToGrid )
		{
			// multiply terrain and node rotations
			dQSetIdentity( qTmp );
			dQMultiply0( qTmp, qTerrainOri, qNodeOri );

			qTerrainOri[0] = qTmp[0];
			qTerrainOri[1] = qTmp[1];
			qTerrainOri[2] = qTmp[2];
			qTerrainOri[3] = qTmp[3];
			
			// use this orientation for the model
			dRSetIdentity( m_Orientation );
			dRfromQ( m_Orientation, qTerrainOri );
		}
		else
		{
			// only node rotation
			qTerrainOri[0] = qNodeOri[0];
			qTerrainOri[1] = qNodeOri[1];
			qTerrainOri[2] = qNodeOri[2];
			qTerrainOri[3] = qNodeOri[3];

			// use this orientation for the model
			dRFromEulerAngles( m_Orientation, math::DegToRad(m_Rot.X), math::DegToRad(m_Rot.Y), math::DegToRad(m_Rot.Z) );
		}

		if( m_PhysicsData != 0 )
		{
			// Box Geoms
			for( i=0; i < m_PhysicsData->totalPhysicsBox; ++i )
			{
				dGeomID geom = 0;
				geom = CreateGeom( dBoxClass, m_EntitySpace, m_PhysicsData->boxList[i].dims );
				DBG_ASSERT_MSG( (geom != 0), "Could not create ode box" );

				m_PhysicsData->boxList[i].geomId = geom;

				// rotate the placement point of each geom
				math::Vec3 placePoint = math::Vec3( m_PhysicsData->boxList[i].pos[0], m_PhysicsData->boxList[i].pos[1], m_PhysicsData->boxList[i].pos[2]);
				math::Vec3 finalPoint = zeroVec;

				finalPoint.X = (placePoint.X * m_Orientation[0]) + (placePoint.Y * m_Orientation[1]) + (placePoint.Z * m_Orientation[2]) + m_Pos.X;
				finalPoint.Y = (placePoint.X * m_Orientation[4]) + (placePoint.Y * m_Orientation[5]) + (placePoint.Z * m_Orientation[6]) + m_Pos.Y;
				finalPoint.Z = (placePoint.X * m_Orientation[8]) + (placePoint.Y * m_Orientation[9]) + (placePoint.Z * m_Orientation[10]) + m_Pos.Z;

				dGeomSetPosition( geom, finalPoint.X, finalPoint.Y, finalPoint.Z );

				dRSetIdentity( mat );	
				dRFromEulerAngles( mat, 
									m_PhysicsData->boxList[i].rot.X, 
									m_PhysicsData->boxList[i].rot.Y, 
									m_PhysicsData->boxList[i].rot.Z );

				// rotate the geom
				dQSetIdentity( qGeomOri );
				dQfromR( qGeomOri, mat );

				// multiply by the terrain orientation
				dQMultiply0( qFinalOri, qTerrainOri, qGeomOri );

				dGeomSetQuaternion( geom, qFinalOri );

				dGeomSetData( geom, this );	
				dGeomSetCategoryBits( geom, m_CategoryBits );
				dGeomSetCollideBits( geom, m_CollideBits );

				dReal aabb[6];
				dGeomGetAABB( geom, aabb );

				// The aabb array has elements (minx, maxx, miny, maxy, minz, maxz)
				math::Vec3 minPoint( aabb[0], aabb[2], aabb[4] );
				math::Vec3 maxPoint( aabb[1], aabb[3], aabb[5] );

				collision::AABB AABB;
				AABB.vBoxMin.X = minPoint.X;
				AABB.vBoxMin.Y = minPoint.Y;
				AABB.vBoxMin.Z = minPoint.Z;

				AABB.vBoxMax.X = maxPoint.X;
				AABB.vBoxMax.Y = maxPoint.Y;
				AABB.vBoxMax.Z = maxPoint.Z;

				AABB.vCenter = (minPoint + maxPoint)*0.5f;

				m_AABB += AABB;
			}

			// Sphere Geoms
			for( i=0; i < m_PhysicsData->totalPhysicsSphere; ++i )
			{
				dGeomID geom = 0;
				geom = CreateGeom( dSphereClass, m_EntitySpace, zeroVec, m_PhysicsData->sphereList[i].radius );
				DBG_ASSERT_MSG( (geom != 0), "Could not create ode sphere" );

				m_PhysicsData->sphereList[i].geomId = geom;
		
				// rotate the placement point of each geom
				math::Vec3 placePoint = math::Vec3( m_PhysicsData->sphereList[i].pos[0], m_PhysicsData->sphereList[i].pos[1], m_PhysicsData->sphereList[i].pos[2]);
				math::Vec3 finalPoint = zeroVec;

				finalPoint.X = (placePoint.X * m_Orientation[0]) + (placePoint.Y * m_Orientation[1]) + (placePoint.Z * m_Orientation[2]) + m_Pos.X;
				finalPoint.Y = (placePoint.X * m_Orientation[4]) + (placePoint.Y * m_Orientation[5]) + (placePoint.Z * m_Orientation[6]) + m_Pos.Y;
				finalPoint.Z = (placePoint.X * m_Orientation[8]) + (placePoint.Y * m_Orientation[9]) + (placePoint.Z * m_Orientation[10]) + m_Pos.Z;

				dGeomSetPosition( geom, finalPoint.X, finalPoint.Y, finalPoint.Z );

				dRSetIdentity( mat );	

				// rotate the geom
				dQSetIdentity( qGeomOri );
				dQfromR( qGeomOri, mat );

				// multiply by the terrain orientation
				dQMultiply0( qFinalOri, qTerrainOri, qGeomOri );

				dGeomSetQuaternion( geom, qFinalOri );
				dGeomSetData( geom, this );
				dGeomSetCategoryBits( geom, m_CategoryBits );
				dGeomSetCollideBits( geom, m_CollideBits );

				dReal aabb[6];
				dGeomGetAABB( geom, aabb );

				// The aabb array has elements (minx, maxx, miny, maxy, minz, maxz)
				math::Vec3 minPoint( aabb[0], aabb[2], aabb[4] );
				math::Vec3 maxPoint( aabb[1], aabb[3], aabb[5] );

				collision::AABB AABB;
				AABB.vBoxMin.X = minPoint.X;
				AABB.vBoxMin.Y = minPoint.Y;
				AABB.vBoxMin.Z = minPoint.Z;

				AABB.vBoxMax.X = maxPoint.X;
				AABB.vBoxMax.Y = maxPoint.Y;
				AABB.vBoxMax.Z = maxPoint.Z;

				AABB.vCenter = (minPoint + maxPoint)*0.5f;

				m_AABB += AABB;
			}

			// Capsule Geoms
			for( i=0; i < m_PhysicsData->totalPhysicsCapsule; ++i )
			{
				dGeomID geom = 0;
				geom = CreateGeom( dCapsuleClass, m_EntitySpace, zeroVec, m_PhysicsData->capsuleList[i].radius, m_PhysicsData->capsuleList[i].length );
				DBG_ASSERT_MSG( (geom != 0), "Could not create ode capsule" );

				m_PhysicsData->capsuleList[i].geomId = geom;
				// rotate the placement point of each geom
				math::Vec3 placePoint = math::Vec3( m_PhysicsData->capsuleList[i].pos[0], m_PhysicsData->capsuleList[i].pos[1], m_PhysicsData->capsuleList[i].pos[2]);
				math::Vec3 finalPoint = zeroVec;

				finalPoint.X = (placePoint.X * m_Orientation[0]) + (placePoint.Y * m_Orientation[1]) + (placePoint.Z * m_Orientation[2]) + m_Pos.X;
				finalPoint.Y = (placePoint.X * m_Orientation[4]) + (placePoint.Y * m_Orientation[5]) + (placePoint.Z * m_Orientation[6]) + m_Pos.Y;
				finalPoint.Z = (placePoint.X * m_Orientation[8]) + (placePoint.Y * m_Orientation[9]) + (placePoint.Z * m_Orientation[10]) + m_Pos.Z;

				dGeomSetPosition( geom, finalPoint.X, finalPoint.Y, finalPoint.Z );

				dRSetIdentity( mat );	
				dRFromEulerAngles( mat, 
									m_PhysicsData->capsuleList[i].rot.X, 
									m_PhysicsData->capsuleList[i].rot.Y, 
									m_PhysicsData->capsuleList[i].rot.Z );

				// rotate the geom
				dQSetIdentity( qGeomOri );
				dQfromR( qGeomOri, mat );

				// multiply by the terrain orientation
				dQMultiply0( qFinalOri, qTerrainOri, qGeomOri );

				dGeomSetQuaternion( geom, qFinalOri );

				dGeomSetData( geom, this );
				dGeomSetCategoryBits( geom, m_CategoryBits );
				dGeomSetCollideBits( geom, m_CollideBits );

				dReal aabb[6];
				dGeomGetAABB( geom, aabb );

				// The aabb array has elements (minx, maxx, miny, maxy, minz, maxz)
				math::Vec3 minPoint( aabb[0], aabb[2], aabb[4] );
				math::Vec3 maxPoint( aabb[1], aabb[3], aabb[5] );

				collision::AABB AABB;
				AABB.vBoxMin.X = minPoint.X;
				AABB.vBoxMin.Y = minPoint.Y;
				AABB.vBoxMin.Z = minPoint.Z;

				AABB.vBoxMax.X = maxPoint.X;
				AABB.vBoxMax.Y = maxPoint.Y;
				AABB.vBoxMax.Z = maxPoint.Z;

				AABB.vCenter = (minPoint + maxPoint)*0.5f;

				m_AABB += AABB;
			}

			// Cylinder Geoms
			for( i=0; i < m_PhysicsData->totalPhysicsCylinder; ++i )
			{
				dGeomID geom = 0;
				geom = CreateGeom( dCylinderClass, m_EntitySpace, zeroVec, m_PhysicsData->cylinderList[i].radius, m_PhysicsData->cylinderList[i].length );
				DBG_ASSERT_MSG( (geom != 0), "Could not create ode cylinder" );

				m_PhysicsData->cylinderList[i].geomId = geom;

				// rotate the placement point of each geom
				math::Vec3 placePoint = math::Vec3( m_PhysicsData->cylinderList[i].pos[0], m_PhysicsData->cylinderList[i].pos[1], m_PhysicsData->cylinderList[i].pos[2]);
				math::Vec3 finalPoint = zeroVec;

				finalPoint.X = (placePoint.X * m_Orientation[0]) + (placePoint.Y * m_Orientation[1]) + (placePoint.Z * m_Orientation[2]) + m_Pos.X;
				finalPoint.Y = (placePoint.X * m_Orientation[4]) + (placePoint.Y * m_Orientation[5]) + (placePoint.Z * m_Orientation[6]) + m_Pos.Y;
				finalPoint.Z = (placePoint.X * m_Orientation[8]) + (placePoint.Y * m_Orientation[9]) + (placePoint.Z * m_Orientation[10]) + m_Pos.Z;

				dGeomSetPosition( geom, finalPoint.X, finalPoint.Y, finalPoint.Z );

				dRSetIdentity( mat );	
				dRFromEulerAngles( mat, 
									m_PhysicsData->cylinderList[i].rot.X, 
									m_PhysicsData->cylinderList[i].rot.Y, 
									m_PhysicsData->cylinderList[i].rot.Z );

				// rotate the geom
				dQSetIdentity( qGeomOri );
				dQfromR( qGeomOri, mat );

				// multiply by the terrain orientation
				dQMultiply0( qFinalOri, qTerrainOri, qGeomOri );

				dGeomSetQuaternion( geom, qFinalOri );

				dGeomSetData( geom, this );
				dGeomSetCategoryBits( geom, m_CategoryBits );
				dGeomSetCollideBits( geom, m_CollideBits );

				dReal aabb[6];
				dGeomGetAABB( geom, aabb );

				// The aabb array has elements (minx, maxx, miny, maxy, minz, maxz)
				math::Vec3 minPoint( aabb[0], aabb[2], aabb[4] );
				math::Vec3 maxPoint( aabb[1], aabb[3], aabb[5] );

				collision::AABB AABB;
				AABB.vBoxMin.X = minPoint.X;
				AABB.vBoxMin.Y = minPoint.Y;
				AABB.vBoxMin.Z = minPoint.Z;

				AABB.vBoxMax.X = maxPoint.X;
				AABB.vBoxMax.Y = maxPoint.Y;
				AABB.vBoxMax.Z = maxPoint.Z;

				AABB.vCenter = (minPoint + maxPoint)*0.5f;

				m_AABB += AABB;
			}

			// triMesh Geoms
			for( i=0; i < m_PhysicsData->totalPhysicsTriMesh; ++i )
			{
				dGeomID geom = 0;
				m_PhysicsData->triMeshList[i].triMeshDataID = dGeomTriMeshDataCreate();
				geom = CreateGeom( dTriMeshClass, m_EntitySpace, zeroVec, 0.0f, 0.0f, m_PhysicsData->triMeshList[i].triMeshDataID );
				DBG_ASSERT_MSG( (geom != 0), "Could not create tri-mesh" );

				//dGeomTriMeshDataBuildSingle( m_PhysicsData->triMeshList[i].triMeshDataID, m_PhysicsData->triMeshList[i].triangleData, sizeof(math::Vec3), m_PhysicsData->triMeshList[i].totalMeshTriangles*3, m_PhysicsData->triMeshList[i].indices, m_PhysicsData->triMeshList[i].totalMeshTriangles*3, sizeof(int) );
				dGeomTriMeshDataBuildSingle1( m_PhysicsData->triMeshList[i].triMeshDataID, m_PhysicsData->triMeshList[i].triangleData, sizeof(math::Vec3), m_PhysicsData->triMeshList[i].totalMeshTriangles*3, m_PhysicsData->triMeshList[i].indices, m_PhysicsData->triMeshList[i].totalMeshTriangles*3, sizeof(int), m_PhysicsData->triMeshList[i].normals );

				m_PhysicsData->triMeshList[i].geomId = geom;
				dGeomSetPosition( geom, m_PhysicsData->triMeshList[i].pos[0] + pos.X, m_PhysicsData->triMeshList[i].pos[1] + pos.Y, m_PhysicsData->triMeshList[i].pos[2] + pos.Z );
		
				dRSetIdentity( mat );	
				dRFromEulerAngles( mat, 
									m_PhysicsData->triMeshList[i].rot.X, 
									m_PhysicsData->triMeshList[i].rot.Y, 
									m_PhysicsData->triMeshList[i].rot.Z );

				dGeomSetRotation( geom, mat );

				dGeomSetData( geom, this );
				dGeomSetCategoryBits( geom, m_CategoryBits );
				dGeomSetCollideBits( geom, m_CollideBits );

				dReal aabb[6];
				dGeomGetAABB( geom, aabb );

				// The aabb array has elements (minx, maxx, miny, maxy, minz, maxz)
				math::Vec3 minPoint( aabb[0], aabb[2], aabb[4] );
				math::Vec3 maxPoint( aabb[1], aabb[3], aabb[5] );

				collision::AABB AABB;
				AABB.vBoxMin.X = minPoint.X;
				AABB.vBoxMin.Y = minPoint.Y;
				AABB.vBoxMin.Z = minPoint.Z;

				AABB.vBoxMax.X = maxPoint.X;
				AABB.vBoxMax.Y = maxPoint.Y;
				AABB.vBoxMax.Z = maxPoint.Z;

				AABB.vCenter = (minPoint + maxPoint)*0.5f;

				m_AABB += AABB;
			}
		}

		m_Sphere.vCenterPoint = m_Pos;

		// models
		if( objRes->highModelId != -1 )
		{
			m_pModelHigh = res::LoadModel(objRes->highModelId);
			//m_pModelHigh->SetModelGeneralFlags( mdl::MODELFLAG_GENERAL_AUTOCULLING | mdl::MODELFLAG_GENERAL_AABBCULL | mdl::MODELFLAG_GENERAL_SUBMESHCULL /*| mdl::MODELFLAG_GENERAL_DRAWBOUNDINGBOX*/ );
			DBG_ASSERT( m_pModelHigh != 0 );

			m_Sphere.vCenterPoint += m_pModelHigh->modelSphere.vCenterPoint;
			m_Sphere.fRadius = m_pModelHigh->modelSphere.fRadius;
		}

		if( objRes->mediumModelId != -1 )
		{
			m_pModelMedium = res::LoadModel(objRes->mediumModelId);
			//m_pModelMedium->SetModelGeneralFlags( mdl::MODELFLAG_GENERAL_AUTOCULLING | mdl::MODELFLAG_GENERAL_AABBCULL | mdl::MODELFLAG_GENERAL_SUBMESHCULL /*| mdl::MODELFLAG_GENERAL_DRAWBOUNDINGBOX*/ );
			DBG_ASSERT( m_pModelMedium != 0 );
		}

		if( objRes->lowModelId != -1 )
		{
			m_pModelLow = res::LoadModel(objRes->lowModelId);
			//m_pModelLow->SetModelGeneralFlags( mdl::MODELFLAG_GENERAL_AUTOCULLING | mdl::MODELFLAG_GENERAL_AABBCULL | mdl::MODELFLAG_GENERAL_SUBMESHCULL /*| mdl::MODELFLAG_GENERAL_DRAWBOUNDINGBOX*/ );
			DBG_ASSERT( m_pModelLow != 0 );
		}

		m_DisableDistance	= objRes->disableDistance*lodScaler;
		m_HighDistance		= objRes->highDistance*lodScaler;
		m_MediumDistance	= objRes->mediumDistance*lodScaler;
		m_LowDistance		= objRes->lowDistance*lodScaler;
	}

	return 0;
}

/////////////////////////////////////////////////////
/// Function: LoadAndParseOde
/// Params: [in]filename
///
/////////////////////////////////////////////////////
int WorldObject::LoadAndParseOde( const char* szFilename )
{
	file::TFileHandle fileHandle;
	file::TFile physFile;
	unsigned int i=0, j=0;

	if( core::IsEmptyString( szFilename ) )
		return(1);

	if( file::FileOpen( szFilename, file::FILETYPE_BINARY_READ, &fileHandle ) == false )
	{
		DBGLOG( "PHYSICSRESOURCES: *ERROR* Could not open %s file\n", szFilename );
		return(1);
	}

	unsigned int boxIndex = 0;
	unsigned int sphereIndex = 0;
	unsigned int capsuleIndex = 0;
	unsigned int cylinderIndex = 0;
	unsigned int triMeshIndex = 0;

	res::PhysicsBlock* pBlock = 0;

	m_AABB.Reset();

	// can create the physics m_PhysicsData
	m_PhysicsData = new res::PhysicsBlock;
	DBG_ASSERT( m_PhysicsData != 0 );

	pBlock = m_PhysicsData;
	DBG_ASSERT( pBlock != 0 );

	std::memset( pBlock, 0, sizeof(res::PhysicsBlock) );

	file::FileReadUInt( &pBlock->totalPhysicsBox, sizeof(unsigned int), 1, &fileHandle );
	file::FileReadUInt( &pBlock->totalPhysicsSphere, sizeof(unsigned int), 1, &fileHandle );
	file::FileReadUInt( &pBlock->totalPhysicsCapsule, sizeof(unsigned int), 1, &fileHandle );
	file::FileReadUInt( &pBlock->totalPhysicsCylinder, sizeof(unsigned int), 1, &fileHandle );
	file::FileReadUInt( &pBlock->totalPhysicsTriMesh, sizeof(unsigned int), 1, &fileHandle );
	file::FileReadUInt( &pBlock->totalPhysicsObjects, sizeof(unsigned int), 1, &fileHandle );

	// allocations
	DBG_ASSERT( pBlock->boxList == 0 );
	if( pBlock->totalPhysicsBox > 0 )
	{
		pBlock->boxList = new res::PhysicsBoxShape[pBlock->totalPhysicsBox];
		DBG_ASSERT( pBlock->boxList != 0 );
	}

	DBG_ASSERT( pBlock->sphereList == 0 );
	if( pBlock->totalPhysicsSphere > 0 )
	{
		pBlock->sphereList = new res::PhysicsSphereShape[pBlock->totalPhysicsSphere];
		DBG_ASSERT( pBlock->sphereList != 0 );
	}

	DBG_ASSERT( pBlock->capsuleList == 0 );
	if( pBlock->totalPhysicsCapsule > 0 )
	{
		pBlock->capsuleList = new res::PhysicsCapsuleShape[pBlock->totalPhysicsCapsule];
		DBG_ASSERT( pBlock->capsuleList != 0 );
	}

	DBG_ASSERT( pBlock->cylinderList == 0 );
	if( pBlock->totalPhysicsCylinder > 0 )
	{
		pBlock->cylinderList = new res::PhysicsCylinderShape[pBlock->totalPhysicsCylinder];
		DBG_ASSERT( pBlock->cylinderList != 0 );
	}

	DBG_ASSERT( pBlock->triMeshList == 0 );
	if( pBlock->totalPhysicsTriMesh > 0 )
	{
		pBlock->triMeshList = new res::PhysicsTriMesh[pBlock->totalPhysicsTriMesh];
		DBG_ASSERT( pBlock->triMeshList != 0 );
	}

	math::Vec3 pos, rot;

	for( i=0; i < pBlock->totalPhysicsObjects; ++i )
	{
		unsigned int type=0;
		file::FileReadUInt( &type, sizeof(unsigned int), 1, &fileHandle );

		switch( type )
		{
			case ODETYPE_BOX: // polyBox
				{
					res::PhysicsBoxShape* pBoxPtr = &pBlock->boxList[boxIndex];
					DBG_ASSERT( pBoxPtr != 0 );

					file::FileReadInt( &pBoxPtr->numeric1, sizeof(int), 1, &fileHandle );
					file::FileReadInt( &pBoxPtr->numeric2, sizeof(int), 1, &fileHandle );
					file::FileReadInt( &pBoxPtr->numeric3, sizeof(int), 1, &fileHandle );
					file::FileReadInt( &pBoxPtr->numeric4, sizeof(int), 1, &fileHandle );
					file::FileReadInt( &pBoxPtr->numeric5, sizeof(int), 1, &fileHandle );

					file::FileReadFloat( &pBoxPtr->pos, sizeof(float), 3, &fileHandle );

					file::FileReadFloat( &pBoxPtr->rot, sizeof(float), 3, &fileHandle );
					pBoxPtr->rot.X = -pBoxPtr->rot.X;
					pBoxPtr->rot.Y = -pBoxPtr->rot.Y;
					pBoxPtr->rot.Z = -pBoxPtr->rot.Z;

					file::FileReadFloat( &pBoxPtr->dims, sizeof(float), 3, &fileHandle );

					++boxIndex;
				}break;
			case ODETYPE_SPHERE: // sphere
				{
					res::PhysicsSphereShape* pSpherePtr = &pBlock->sphereList[sphereIndex];
					DBG_ASSERT( pSpherePtr != 0 );

					file::FileReadInt( &pSpherePtr->numeric1, sizeof(int), 1, &fileHandle );
					file::FileReadInt( &pSpherePtr->numeric2, sizeof(int), 1, &fileHandle );
					file::FileReadInt( &pSpherePtr->numeric3, sizeof(int), 1, &fileHandle );
					file::FileReadInt( &pSpherePtr->numeric4, sizeof(int), 1, &fileHandle );
					file::FileReadInt( &pSpherePtr->numeric5, sizeof(int), 1, &fileHandle );

					file::FileReadFloat( &pSpherePtr->pos, sizeof(float), 3, &fileHandle );

					file::FileReadFloat( &pSpherePtr->radius, sizeof(float), 1, &fileHandle );


					++sphereIndex;
				}break;
			case ODETYPE_CAPSULE:
				{
					res::PhysicsCapsuleShape* pCapsulePtr = &pBlock->capsuleList[capsuleIndex];
					DBG_ASSERT( pCapsulePtr != 0 );

					file::FileReadInt( &pCapsulePtr->numeric1, sizeof(int), 1, &fileHandle );
					file::FileReadInt( &pCapsulePtr->numeric2, sizeof(int), 1, &fileHandle );
					file::FileReadInt( &pCapsulePtr->numeric3, sizeof(int), 1, &fileHandle );
					file::FileReadInt( &pCapsulePtr->numeric4, sizeof(int), 1, &fileHandle );
					file::FileReadInt( &pCapsulePtr->numeric5, sizeof(int), 1, &fileHandle );

					file::FileReadFloat( &pCapsulePtr->pos, sizeof(float), 3, &fileHandle );

					file::FileReadFloat( &pCapsulePtr->rot, sizeof(float), 3, &fileHandle );
					pCapsulePtr->rot.X = -pCapsulePtr->rot.X;
					pCapsulePtr->rot.Y = -pCapsulePtr->rot.Y;
					pCapsulePtr->rot.Z = -pCapsulePtr->rot.Z;

					file::FileReadFloat( &pCapsulePtr->radius, sizeof(float), 1, &fileHandle );
					file::FileReadFloat( &pCapsulePtr->length, sizeof(float), 1, &fileHandle );

					++capsuleIndex;
				}break;
			case ODETYPE_CYLINDER:
				{
					res::PhysicsCylinderShape* pCylinderPtr = &pBlock->cylinderList[cylinderIndex];
					DBG_ASSERT( pCylinderPtr != 0 );

					file::FileReadInt( &pCylinderPtr->numeric1, sizeof(int), 1, &fileHandle );
					file::FileReadInt( &pCylinderPtr->numeric2, sizeof(int), 1, &fileHandle );
					file::FileReadInt( &pCylinderPtr->numeric3, sizeof(int), 1, &fileHandle );
					file::FileReadInt( &pCylinderPtr->numeric4, sizeof(int), 1, &fileHandle );
					file::FileReadInt( &pCylinderPtr->numeric5, sizeof(int), 1, &fileHandle );

					file::FileReadFloat( &pCylinderPtr->pos, sizeof(float), 3, &fileHandle );

					file::FileReadFloat( &pCylinderPtr->rot, sizeof(float), 3, &fileHandle );
					pCylinderPtr->rot.X = -pCylinderPtr->rot.X;
					pCylinderPtr->rot.Y = -pCylinderPtr->rot.Y;
					pCylinderPtr->rot.Z = -pCylinderPtr->rot.Z;

					file::FileReadFloat( &pCylinderPtr->radius, sizeof(float), 1, &fileHandle );

					file::FileReadFloat( &pCylinderPtr->length, sizeof(float), 1, &fileHandle );

					++cylinderIndex;
				}break;
			case ODETYPE_TRIMESH: // mesh
				{
					res::PhysicsTriMesh* pTriMesh = &pBlock->triMeshList[triMeshIndex];
					DBG_ASSERT( pTriMesh != 0 );

					pTriMesh->totalMeshTriangles = 0;
					pTriMesh->indices = 0;
					pTriMesh->triangleData = 0;
					pTriMesh->normals = 0;
					pTriMesh->triMeshDataID = 0;

					file::FileReadInt( &pTriMesh->numeric1, sizeof(int), 1, &fileHandle );
					file::FileReadInt( &pTriMesh->numeric2, sizeof(int), 1, &fileHandle );
					file::FileReadInt( &pTriMesh->numeric3, sizeof(int), 1, &fileHandle );
					file::FileReadInt( &pTriMesh->numeric4, sizeof(int), 1, &fileHandle );
					file::FileReadInt( &pTriMesh->numeric5, sizeof(int), 1, &fileHandle );

					file::FileReadFloat( &pTriMesh->pos, sizeof(float), 3, &fileHandle );
					file::FileReadFloat( &pTriMesh->rot, sizeof(float), 3, &fileHandle );
					pTriMesh->rot.X = pTriMesh->rot.X;
					pTriMesh->rot.Y = pTriMesh->rot.Y;
					pTriMesh->rot.Z = pTriMesh->rot.Z;

					unsigned int totalMeshTriangles = 0;
					file::FileReadUInt( &totalMeshTriangles, sizeof(unsigned int), 1, &fileHandle );

					unsigned int polyMeshCount = 0;
					file::FileReadUInt( &polyMeshCount, sizeof(unsigned int), 1, &fileHandle );

					// allocate data
					pTriMesh->totalMeshTriangles = totalMeshTriangles;
					pTriMesh->triangleData = new math::Vec3[totalMeshTriangles*3];
					pTriMesh->indices = new int[totalMeshTriangles*3];
					pTriMesh->normals = new math::Vec3[totalMeshTriangles];

					DBG_ASSERT( pTriMesh->triangleData != 0 );
					DBG_ASSERT( pTriMesh->indices != 0 );

					int pointIndex = 0;
					int normalCount = 0;
					for( j = 0; j < totalMeshTriangles; ++j )
					{
						file::FileReadFloat( &pTriMesh->triangleData[pointIndex+0], sizeof(float), 3, &fileHandle );
						file::FileReadFloat( &pTriMesh->triangleData[pointIndex+1], sizeof(float), 3, &fileHandle );
						file::FileReadFloat( &pTriMesh->triangleData[pointIndex+2], sizeof(float), 3, &fileHandle );

						// calc normal
						math::Vec3 edge1 = pTriMesh->triangleData[(pointIndex+1)] - pTriMesh->triangleData[(pointIndex+0)];
						math::Vec3 edge2 = pTriMesh->triangleData[(pointIndex+2)] - pTriMesh->triangleData[(pointIndex+0)];

						pTriMesh->normals[normalCount] = CrossProduct( edge1, edge2 );
						pTriMesh->normals[normalCount].normalise();

						normalCount++;

						pointIndex+=3;
					}

					for( j = 0; j < pTriMesh->totalMeshTriangles*3; j++ )
						pTriMesh->indices[j] = j;

					++triMeshIndex;

				}break;
			
			default:
				DBG_ASSERT_MSG( 0, "Unknown ode physics type in file" );
			break;
		}
	}

	file::FileClose( &fileHandle );

	return(0);
}

/////////////////////////////////////////////////////
/// Method: CreateGeom
/// Params: None
///
/////////////////////////////////////////////////////
dGeomID WorldObject::CreateGeom( int shapeClass, dSpaceID spaceId, const math::Vec3& dims, float radius, float length, dTriMeshDataID triMeshDataId )
{
	switch( shapeClass )
	{
		case dBoxClass:
			return dCreateBox( spaceId, dims.Width, dims.Height, dims.Depth );
		break;
		case dSphereClass:
			return dCreateSphere( spaceId, radius );
		break;
		case dCapsuleClass:
			return dCreateCapsule( spaceId, radius, length );
		break;
		case dCylinderClass:
			return dCreateCylinder( spaceId, radius, length );
		break;
		case dTriMeshClass:
			return dCreateTriMesh( spaceId, triMeshDataId, 0, 0, 0 );
		break;

		default:
			DBG_ASSERT(0);
			break;
	}		

	// bad shape class?
	DBG_ASSERT(0);
	return 0;
}

/////////////////////////////////////////////////////
/// Method: EnablePhysics
/// Params: None
///
/////////////////////////////////////////////////////
void WorldObject::EnablePhysics()
{
	if( m_PhysicsEnabled )
		return;

	if( m_PhysicsData != 0 )
	{
		unsigned int i=0;

		m_CategoryBits = CATEGORY_WORLDOBJECT;
		m_CollideBits = (CATEGORY_RAYCAST | CATEGORY_BULLET | CATEGORY_PLAYER);

		// Box Geoms
		for( i=0; i < m_PhysicsData->totalPhysicsBox; ++i )
		{
			dGeomEnable( m_PhysicsData->boxList[i].geomId );
			dGeomSetCategoryBits( m_PhysicsData->boxList[i].geomId, m_CategoryBits );
			dGeomSetCollideBits( m_PhysicsData->boxList[i].geomId, m_CollideBits );
		}

		// Sphere Geoms
		for( i=0; i < m_PhysicsData->totalPhysicsSphere; ++i )
		{
			dGeomEnable( m_PhysicsData->sphereList[i].geomId );
			dGeomSetCategoryBits( m_PhysicsData->sphereList[i].geomId, m_CategoryBits );
			dGeomSetCollideBits( m_PhysicsData->sphereList[i].geomId, m_CollideBits );
		}

		// Capsule Geoms
		for( i=0; i < m_PhysicsData->totalPhysicsCapsule; ++i )
		{
			dGeomEnable( m_PhysicsData->capsuleList[i].geomId );
			dGeomSetCategoryBits( m_PhysicsData->capsuleList[i].geomId, m_CategoryBits );
			dGeomSetCollideBits( m_PhysicsData->capsuleList[i].geomId, m_CollideBits );
		}

		// Cylinder Geoms
		for( i=0; i < m_PhysicsData->totalPhysicsCylinder; ++i )
		{
			dGeomEnable( m_PhysicsData->cylinderList[i].geomId );
			dGeomSetCategoryBits( m_PhysicsData->cylinderList[i].geomId, m_CategoryBits );
			dGeomSetCollideBits( m_PhysicsData->cylinderList[i].geomId, m_CollideBits );
		}

		// triMesh Geoms
		for( i=0; i < m_PhysicsData->totalPhysicsTriMesh; ++i )
		{
			dGeomEnable( m_PhysicsData->triMeshList[i].geomId );
			dGeomSetCategoryBits( m_PhysicsData->triMeshList[i].geomId, m_CategoryBits );
			dGeomSetCollideBits( m_PhysicsData->triMeshList[i].geomId, m_CollideBits );
		}
	}

	m_PhysicsEnabled = true;
}
		
/////////////////////////////////////////////////////
/// Method: DisablePhysics
/// Params: None
///
/////////////////////////////////////////////////////
void WorldObject::DisablePhysics()
{
	if( !m_PhysicsEnabled )
		return;

	if( m_PhysicsData != 0 )
	{
		unsigned int i=0;

		m_CategoryBits = CATEGORY_WORLDOBJECT;
		m_CollideBits = CATEGORY_NOTHING2;

		// Box Geoms
		for( i=0; i < m_PhysicsData->totalPhysicsBox; ++i )
		{
			dGeomDisable( m_PhysicsData->boxList[i].geomId );
			dGeomSetCategoryBits( m_PhysicsData->boxList[i].geomId, m_CategoryBits );
			dGeomSetCollideBits( m_PhysicsData->boxList[i].geomId, m_CollideBits );
		}

		// Sphere Geoms
		for( i=0; i < m_PhysicsData->totalPhysicsSphere; ++i )
		{
			dGeomDisable( m_PhysicsData->sphereList[i].geomId );
			dGeomSetCategoryBits( m_PhysicsData->sphereList[i].geomId, m_CategoryBits );
			dGeomSetCollideBits( m_PhysicsData->sphereList[i].geomId, m_CollideBits );
		}

		// Capsule Geoms
		for( i=0; i < m_PhysicsData->totalPhysicsCapsule; ++i )
		{
			dGeomDisable( m_PhysicsData->capsuleList[i].geomId );
			dGeomSetCategoryBits( m_PhysicsData->capsuleList[i].geomId, m_CategoryBits );
			dGeomSetCollideBits( m_PhysicsData->capsuleList[i].geomId, m_CollideBits );
		}

		// Cylinder Geoms
		for( i=0; i < m_PhysicsData->totalPhysicsCylinder; ++i )
		{
			dGeomDisable( m_PhysicsData->cylinderList[i].geomId );
			dGeomSetCategoryBits( m_PhysicsData->cylinderList[i].geomId, m_CategoryBits );
			dGeomSetCollideBits( m_PhysicsData->cylinderList[i].geomId, m_CollideBits );
		}

		// triMesh Geoms
		for( i=0; i < m_PhysicsData->totalPhysicsTriMesh; ++i )
		{
			dGeomDisable( m_PhysicsData->triMeshList[i].geomId );
			dGeomSetCategoryBits( m_PhysicsData->triMeshList[i].geomId, m_CategoryBits );
			dGeomSetCollideBits( m_PhysicsData->triMeshList[i].geomId, m_CollideBits );
		}
	}

	m_PhysicsEnabled = false;
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void WorldObject::Draw()
{
	if( m_CurrentDistance >= m_DisableDistance )
	{
		return;
	}

	bool drawnMesh = false;

	bool textureState = renderer::OpenGL::GetInstance()->GetTextureState();
	bool lightingState = renderer::OpenGL::GetInstance()->GetLightingState();
	math::Vec4Lite col = renderer::OpenGL::GetInstance()->GetColour4ub();

	if( m_Destroyed )
	{
		if( m_IsTarget )
			return;

		if( textureState )
			renderer::OpenGL::GetInstance()->DisableTexturing();

		if( lightingState )
			renderer::OpenGL::GetInstance()->DisableLighting();

		renderer::OpenGL::GetInstance()->SetColour4ub( 0, 0, 0, 255 );
	}

	glPushMatrix();
		m[0] = m_Orientation[0];
		m[1] = m_Orientation[4];
		m[2] = m_Orientation[8];
		m[3] = 0.0f;
		m[4] = m_Orientation[1];
		m[5] = m_Orientation[5];
		m[6] = m_Orientation[9];
		m[7] = 0.0f;
		m[8] = m_Orientation[2];
		m[9] = m_Orientation[6];
		m[10] = m_Orientation[10];
		m[11] = 0.0f;
		m[12] = m_Pos[0];
		m[13] = m_Pos[1];
		m[14] = m_Pos[2];
		m[15] = 1.0f;

		glMultMatrixf( m );

		if( m_CurrentDistance <= m_HighDistance &&
			m_pModelHigh != 0 )
		{
			if( m_pModelHigh != 0 )
				m_pModelHigh->Draw();

			drawnMesh = true;
		}

		if( !drawnMesh &&
			m_CurrentDistance > m_HighDistance && 
			m_CurrentDistance < m_LowDistance &&
			m_pModelMedium != 0 )
		{
			if( m_pModelMedium != 0 )
				m_pModelMedium->Draw();

			drawnMesh = true;
		}
		
		if(	!drawnMesh &&
			m_CurrentDistance >= m_LowDistance &&
			m_pModelLow != 0 )
		{
			if( m_pModelLow != 0 )
				m_pModelLow->Draw();

			drawnMesh = true;
		}

	glPopMatrix();

	if( m_Destroyed )
	{
		if( textureState )
			renderer::OpenGL::GetInstance()->EnableTexturing();

		if( lightingState )
			renderer::OpenGL::GetInstance()->EnableLighting();

		renderer::OpenGL::GetInstance()->SetColour4ub( col.R, col.G, col.B, col.A );
	}
}
		
/////////////////////////////////////////////////////
/// Method: DrawPhysicsData
/// Params: None
///
/////////////////////////////////////////////////////
void WorldObject::DrawPhysicsData()
{
	unsigned int i=0;
	const dReal* pos = 0;
	const dReal* rot = 0;

	bool textureState = renderer::OpenGL::GetInstance()->GetTextureState();

	if( textureState )
		renderer::OpenGL::GetInstance()->DisableTexturing();

	bool lightingState = renderer::OpenGL::GetInstance()->GetLightingState();

	if( lightingState )
		renderer::OpenGL::GetInstance()->DisableLighting();

	if( m_PhysicsData != 0 )
	{
		// Box Geoms
		for( i=0; i < m_PhysicsData->totalPhysicsBox; ++i )
		{
			if( dGeomIsEnabled(m_PhysicsData->boxList[i].geomId) )
				renderer::OpenGL::GetInstance()->SetColour4ub( 0,255,0,255 );
			else
				renderer::OpenGL::GetInstance()->SetColour4ub( 255,0,0,255 );

			pos = dGeomGetPosition( m_PhysicsData->boxList[i].geomId );
			rot = dGeomGetRotation( m_PhysicsData->boxList[i].geomId );

			physics::DrawPhysicsGeometry( m_PhysicsData->boxList[i].geomId, pos, rot );
		}

		// Sphere Geoms
		for( i=0; i < m_PhysicsData->totalPhysicsSphere; ++i )
		{
			if( dGeomIsEnabled(m_PhysicsData->sphereList[i].geomId) )
				renderer::OpenGL::GetInstance()->SetColour4ub( 0,255,0,255 );
			else
				renderer::OpenGL::GetInstance()->SetColour4ub( 255,0,0,255 );

			pos = dGeomGetPosition( m_PhysicsData->sphereList[i].geomId );
			rot = dGeomGetRotation( m_PhysicsData->sphereList[i].geomId );

			physics::DrawPhysicsGeometry( m_PhysicsData->sphereList[i].geomId, pos, rot );
		}

		// Capsule Geoms
		for( i=0; i < m_PhysicsData->totalPhysicsCapsule; ++i )
		{
			if( dGeomIsEnabled(m_PhysicsData->capsuleList[i].geomId) )
				renderer::OpenGL::GetInstance()->SetColour4ub( 0,255,0,255 );
			else
				renderer::OpenGL::GetInstance()->SetColour4ub( 255,0,0,255 );

			pos = dGeomGetPosition( m_PhysicsData->capsuleList[i].geomId );
			rot = dGeomGetRotation( m_PhysicsData->capsuleList[i].geomId );

			physics::DrawPhysicsGeometry( m_PhysicsData->capsuleList[i].geomId, pos, rot );
		}

		// Cylinder Geoms
		for( i=0; i < m_PhysicsData->totalPhysicsCylinder; ++i )
		{
			if( dGeomIsEnabled(m_PhysicsData->cylinderList[i].geomId) )
				renderer::OpenGL::GetInstance()->SetColour4ub( 0,255,0,255 );
			else
				renderer::OpenGL::GetInstance()->SetColour4ub( 255,0,0,255 );

			pos = dGeomGetPosition( m_PhysicsData->cylinderList[i].geomId );
			rot = dGeomGetRotation( m_PhysicsData->cylinderList[i].geomId );

			physics::DrawPhysicsGeometry( m_PhysicsData->cylinderList[i].geomId, pos, rot );
		}

		// triMesh Geoms
		for( i=0; i < m_PhysicsData->totalPhysicsTriMesh; ++i )
		{
			if( dGeomIsEnabled(m_PhysicsData->triMeshList[i].geomId) )
				renderer::OpenGL::GetInstance()->SetColour4ub( 0,255,0,255 );
			else
				renderer::OpenGL::GetInstance()->SetColour4ub( 255,0,0,255 );

			pos = dGeomGetPosition( m_PhysicsData->triMeshList[i].geomId );
			rot = dGeomGetRotation( m_PhysicsData->triMeshList[i].geomId );

			physics::DrawPhysicsGeometry( m_PhysicsData->triMeshList[i].geomId, pos, rot );
		}

		//DrawAABB();
		DrawSphere();
	}

	if( lightingState )
		renderer::OpenGL::GetInstance()->EnableLighting();

	if( textureState )
		renderer::OpenGL::GetInstance()->EnableTexturing();
}

/////////////////////////////////////////////////////
/// Method: DrawAABB
/// Params: None
///
/////////////////////////////////////////////////////
void WorldObject::DrawAABB()
{
	renderer::OpenGL::GetInstance()->SetColour4ub( 255,0,0,255 );

	glLineWidth( 1.5f );
	glPushMatrix();
		renderer::DrawAABB( m_AABB.vBoxMin, m_AABB.vBoxMax );
	glPopMatrix();
	glLineWidth( 1.0f );
}

/////////////////////////////////////////////////////
/// Method: DrawSphere
/// Params: None
///
/////////////////////////////////////////////////////
void WorldObject::DrawSphere()
{
	renderer::OpenGL::GetInstance()->SetColour4ub( 255,0,0,255 );

	glPushMatrix();
		glTranslatef( m_Sphere.vCenterPoint.X, m_Sphere.vCenterPoint.Y, m_Sphere.vCenterPoint.Z );
		renderer::DrawSphere( m_Sphere.fRadius );
	glPopMatrix();
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: None
///
/////////////////////////////////////////////////////
void WorldObject::Update( float deltaTime, const math::Vec3& camPos )
{
	if( !m_Destroyed && m_CanDestroy )
	{
		if( m_Damage <= 0 )
		{
			m_Destroyed = true;
			m_Damage = 0;

			if( m_ResId == GameSystems::GetInstance()->GetGameModeExtraData() )
			{
				int targetsDestroyed = GameSystems::GetInstance()->GetTargetsDestroyed();
				GameSystems::GetInstance()->SetTargetsDestroyed(targetsDestroyed+1);
			}
			else
			{
				int objectsDestroyed = GameSystems::GetInstance()->GetObjectsDestroyed();
				GameSystems::GetInstance()->SetObjectsDestroyed(objectsDestroyed+1);
			}

			// turn off collisions
			if( m_IsTarget )
			{
				GameSystems::GetInstance()->SpawnStars( m_Sphere.vCenterPoint );
				GameSystems::GetInstance()->PlayTargetDestroyedAudio();
				DisablePhysics();
			}
			else
			{
				GameSystems::GetInstance()->SpawnFire(  m_Sphere.vCenterPoint );
				GameSystems::GetInstance()->PlayObjectDestroyedAudio();
			}
		}
	}

	m_CurrentDistance = std::abs( (camPos - m_Pos).length() );//renderer::OpenGL::GetInstance()->SphereDistanceFromFrustum(m_Pos.X,m_Pos.Y,m_Pos.Z, m_Sphere.fRadius );

	if( m_IsTarget &&
		m_Destroyed )
	{
		m_IsDrawn = false;
		return;
	}

	if( m_CurrentDistance >= m_DisableDistance )
	{
		DisablePhysics();
		m_IsDrawn = false;

		return;
	}

	bool updatedPhysics = false;

	if( m_CurrentDistance <= m_HighDistance &&
		m_pModelHigh != 0 )
	{
		updatedPhysics = true;
		m_IsDrawn = true;
		EnablePhysics();
	}

	if( !updatedPhysics &&
		m_CurrentDistance > m_HighDistance && 
		m_CurrentDistance < m_LowDistance &&
		m_pModelMedium != 0 )
	{
		updatedPhysics = true;
		m_IsDrawn = true;
		DisablePhysics();
	}
		
	if(	!updatedPhysics &&
		m_CurrentDistance >= m_LowDistance &&
		m_pModelLow != 0 )
	{
		updatedPhysics = true;
		m_IsDrawn = true;
		DisablePhysics();
	}

	/*if( m_IsDrawn )
	{
		CalculateUnprojectedPos();
	}
	else
	{
		m_UnProjectedPoint = math::Vec3( -10000.0f, -10000.0f, -10000.0f );
	}*/
}

/////////////////////////////////////////////////////
/// Method: CalculateUnprojectedPos
/// Params: None
///
/////////////////////////////////////////////////////
void WorldObject::CalculateUnprojectedPos()
{
	GLfloat *mdl = renderer::OpenGL::GetInstance()->GetModelViewMatrix();
	GLfloat *proj = renderer::OpenGL::GetInstance()->GetProjectionMatrix();
	GLint *viewport = renderer::OpenGL::GetInstance()->GetViewport();

	gluUtil::gluProjectf(  m_Sphere.vCenterPoint.X,  m_Sphere.vCenterPoint.Y,  m_Sphere.vCenterPoint.Z,
							mdl, proj, viewport,
							&m_UnProjectedPoint.X, &m_UnProjectedPoint.Y, &m_UnProjectedPoint.Z );
}

/////////////////////////////////////////////////////
/// Method: Reset
/// Params: None
///
/////////////////////////////////////////////////////
void WorldObject::Reset()
{
	m_Destroyed = false;
	m_Damage = DEFAULT_DAMAGE;
}

/////////////////////////////////////////////////////
/// Method: Destroy
/// Params: None
///
/////////////////////////////////////////////////////
void WorldObject::Destroy( )
{
	unsigned int i=0, j=0;

	if( m_pModelHigh != 0 )
		res::RemoveModel(m_pModelHigh);

	if( m_pModelMedium != 0 )
		res::RemoveModel(m_pModelMedium);

	if( m_pModelLow != 0 )
		res::RemoveModel(m_pModelLow);

	if( m_PhysicsData != 0 )
	{
		for( i=0; i < m_PhysicsData->totalPhysicsTriMesh; ++i )	
		{
			dGeomTriMeshDataDestroy( m_PhysicsData->triMeshList[i].triMeshDataID );
			m_PhysicsData->triMeshList[i].triMeshDataID = 0;
		}
	
		if( m_PhysicsData->boxList )
			delete[] m_PhysicsData->boxList;

		if( m_PhysicsData->sphereList )
			delete[] m_PhysicsData->sphereList;

		if( m_PhysicsData->capsuleList )
			delete[] m_PhysicsData->capsuleList;

		if( m_PhysicsData->cylinderList )
			delete[] m_PhysicsData->cylinderList;

		for( j=0; j < m_PhysicsData->totalPhysicsTriMesh; ++j )
		{
			if( m_PhysicsData->triMeshList[j].triMeshDataID != 0 )
			{
				dGeomTriMeshDataDestroy( m_PhysicsData->triMeshList[j].triMeshDataID );
				m_PhysicsData->triMeshList[j].triMeshDataID = 0;
			}

			if( m_PhysicsData->triMeshList[j].triangleData != 0 )
				delete[] m_PhysicsData->triMeshList[j].triangleData;

			if( m_PhysicsData->triMeshList[j].indices != 0 )
				delete[] m_PhysicsData->triMeshList[j].indices;

			if( m_PhysicsData->triMeshList[j].normals != 0 )
				delete[] m_PhysicsData->triMeshList[j].normals;	
		}
		if( m_PhysicsData->triMeshList )
			delete[] m_PhysicsData->triMeshList;

		if( m_PhysicsData )
		{
			delete m_PhysicsData;
			m_PhysicsData = 0;
		}
	}

	/*if( m_EntitySpace != 0 &&
		m_EntitySpace != physics::PhysicsWorldODE::GetSpace() &&
		m_EntitySpace != physics::PhysicsWorldODE::GetEditorSpace() )
	{
		dSpaceDestroy( m_EntitySpace );
		m_EntitySpace = 0;
	}*/
}

/////////////////////////////////////////////////////
/// Method: SetPos
/// Params: None
///
/////////////////////////////////////////////////////
void WorldObject::SetPos( const math::Vec3& pos, bool adjustWithCOM )
{

}

/////////////////////////////////////////////////////
/// Function: PhysicsTestObjectToWorld
/// Params: [in]data, [in]o1, [in]o2
///
/////////////////////////////////////////////////////
void PhysicsTestObjectToWorld (void *data, dGeomID o1, dGeomID o2)
{
	int i=0;

	dBodyID b1 = dGeomGetBody(o1);
	dBodyID b2 = dGeomGetBody(o2);

	// exit without doing anything if the two bodies are connected by a joint
	if (b1 && b2 && dAreConnected (b1,b2)) 
		return;

	dContact contact[MAX_CONTACTS];

	// run the collision
	int numCollisions = dCollide( o1, o2, MAX_CONTACTS, &contact[0].geom, sizeof(dContact) );

	if( numCollisions <= 0 ) 
		return;

	physics::PhysicsIdentifier* pGeomData1 = reinterpret_cast<physics::PhysicsIdentifier*>(dGeomGetData(o1));
	physics::PhysicsIdentifier* pGeomData2 = reinterpret_cast<physics::PhysicsIdentifier*>(dGeomGetData(o2));

	// check for a contact point between a ray and selectable
	DBG_ASSERT( (pGeomData1 != 0) && (pGeomData2 != 0) );

	// if no geom is the ray, don't care
	if( pGeomData1->GetBaseId() != PHYSICSBASICID_RAYTESTER &&
		pGeomData2->GetBaseId() != PHYSICSBASICID_RAYTESTER )
		return;

	RayObject* pRay = 0;

	// only want breakables and rays
	if( (pGeomData1->GetBaseId() == PHYSICSBASICID_RAYTESTER || 
		pGeomData2->GetBaseId() == PHYSICSBASICID_RAYTESTER) ) 
	{
		for( i=0; i < numCollisions; ++i )
		{
			bool newHit = false;

			if( pGeomData1->GetBaseId() == PHYSICSBASICID_RAYTESTER )
			{
				pRay = reinterpret_cast<RayObject*>(dGeomGetData(o1)); 

				if( pRay->totalHits < MAX_RAY_HITS )
				{
					pRay->rayHits[pRay->nextHitIndex].pos = math::Vec3( contact[i].geom.pos[0], contact[i].geom.pos[1], contact[i].geom.pos[2] );
					pRay->rayHits[pRay->nextHitIndex].otherGeom = o2;
					pRay->rayHits[pRay->nextHitIndex].n = math::Vec3( contact[i].geom.normal[0], contact[i].geom.normal[1], contact[i].geom.normal[2] );
					pRay->rayHits[pRay->nextHitIndex].pData = dGeomGetData(o2);

					//if( FacingCamera( math::Normalise(pRay->rayDir), pRay->rayHits[pRay->nextHitIndex].n ) )
					{
						newHit = true;

						pRay->hasHits = true;
						pRay->totalHits++;

						math::Vec3 distVec = pRay->rayHits[pRay->nextHitIndex].pos - pRay->rayPos;
						pRay->rayHits[pRay->nextHitIndex].distance = distVec.length();
					}
				}
			}
			else
			{
				pRay = reinterpret_cast<RayObject*>(dGeomGetData(o2)); 

				if( pRay->totalHits < MAX_RAY_HITS )
				{
					pRay->rayHits[pRay->nextHitIndex].pos = math::Vec3( contact[i].geom.pos[0], contact[i].geom.pos[1], contact[i].geom.pos[2] );
					pRay->rayHits[pRay->nextHitIndex].otherGeom = o1;
					pRay->rayHits[pRay->nextHitIndex].n = math::Vec3( contact[i].geom.normal[0], contact[i].geom.normal[1], contact[i].geom.normal[2] );
					pRay->rayHits[pRay->nextHitIndex].pData = dGeomGetData(o1);

					//if( FacingCamera( math::Normalise(pRay->rayDir), pRay->rayHits[pRay->nextHitIndex].n ) )
					{
						newHit = true;

						pRay->hasHits = true;
						pRay->totalHits++;

						math::Vec3 distVec = pRay->rayHits[pRay->nextHitIndex].pos - pRay->rayPos;
						pRay->rayHits[pRay->nextHitIndex].distance = distVec.length();
					}
				}
			}

			if( newHit == true )
				pRay->nextHitIndex++;

			if( pRay->nextHitIndex >= MAX_RAY_HITS )
				pRay->nextHitIndex = 0;
		}
	}
}
