
/*===================================================================
	File: PhysicsResources.cpp
	Game: AirCadets

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "CollisionBase.h"
#include "PhysicsBase.h"
#include "ScriptBase.h"

#include <vector>

#include "Resources/PhysicsResources.h"

namespace
{
	typedef std::map< int, res::PhysicsResourceStore > TIntPhysicsMap;
	typedef std::pair< int, res::PhysicsResourceStore > TIntPhysicsPair;

	// maps
	TIntPhysicsMap ResourceMap;
}

/////////////////////////////////////////////////////
/// Function: LoadAndParseOde
/// Params: [in]filename, [in]newResource
///
/////////////////////////////////////////////////////
int res::LoadAndParseOde( const char* filename, res::PhysicsResourceStore& newResource )
{
	file::TFileHandle fileHandle;
	file::TFile physFile;
	unsigned int i = 0, j=0, k=0;

	if( core::IsEmptyString( filename ) )
		return(1);

	if( file::FileOpen( filename, file::FILETYPE_BINARY_READ, &fileHandle ) == false )
	{
		DBGLOG( "PHYSICSRESOURCES: *ERROR* Could not open %s file\n", filename );
		return(1);
	}

	unsigned int boxIndex = 0;
	unsigned int sphereIndex = 0;
	unsigned int capsuleIndex = 0;
	unsigned int cylinderIndex = 0;
	unsigned int triMeshIndex = 0;

	// can create the physics block
	newResource.block = new res::PhysicsBlock;
	//DBGLOG( "res: %s @ %p\n", filename, newResource.block);
	DBG_ASSERT( newResource.block != 0 );

	std::memset( newResource.block, 0, sizeof(res::PhysicsBlock) );

	file::FileReadUInt( &newResource.block->totalPhysicsBox, sizeof(unsigned int), 1, &fileHandle );
	file::FileReadUInt( &newResource.block->totalPhysicsSphere, sizeof(unsigned int), 1, &fileHandle );
	file::FileReadUInt( &newResource.block->totalPhysicsCapsule, sizeof(unsigned int), 1, &fileHandle );
	file::FileReadUInt( &newResource.block->totalPhysicsCylinder, sizeof(unsigned int), 1, &fileHandle );
	file::FileReadUInt( &newResource.block->totalPhysicsTriMesh, sizeof(unsigned int), 1, &fileHandle );
	file::FileReadUInt( &newResource.block->totalPhysicsObjects, sizeof(unsigned int), 1, &fileHandle );

	// allocations
	DBG_ASSERT( newResource.block->boxList == 0 );
	if( newResource.block->totalPhysicsBox > 0 )
	{
		newResource.block->boxList = new res::PhysicsBoxShape[newResource.block->totalPhysicsBox];
		DBG_ASSERT( newResource.block->boxList != 0 );
	}

	DBG_ASSERT( newResource.block->sphereList == 0 );
	if( newResource.block->totalPhysicsSphere > 0 )
	{
		newResource.block->sphereList = new res::PhysicsSphereShape[newResource.block->totalPhysicsSphere];
		DBG_ASSERT( newResource.block->sphereList != 0 );
	}

	DBG_ASSERT( newResource.block->capsuleList == 0 );
	if( newResource.block->totalPhysicsCapsule > 0 )
	{
		newResource.block->capsuleList = new res::PhysicsCapsuleShape[newResource.block->totalPhysicsCapsule];
		DBG_ASSERT( newResource.block->capsuleList != 0 );
	}

	DBG_ASSERT( newResource.block->cylinderList == 0 );
	if( newResource.block->totalPhysicsCylinder > 0 )
	{
		newResource.block->cylinderList = new res::PhysicsCylinderShape[newResource.block->totalPhysicsCylinder];
		DBG_ASSERT( newResource.block->cylinderList != 0 );
	}

	DBG_ASSERT( newResource.block->triMeshList == 0 );
	if( newResource.block->totalPhysicsTriMesh > 0 )
	{
		newResource.block->triMeshList = new res::PhysicsTriMesh[newResource.block->totalPhysicsTriMesh];
		DBG_ASSERT( newResource.block->triMeshList != 0 );
	}

	math::Vec3 pos, rot;

	for( i=0; i < newResource.block->totalPhysicsObjects; ++i )
	{
		int type=0;
		file::FileReadUInt( &type, sizeof(unsigned int), 1, &fileHandle );

		switch( type )
		{
			case ODETYPE_BOX: // polyBox
				{
					res::PhysicsBoxShape* pBoxPtr = &newResource.block->boxList[boxIndex];
					DBG_ASSERT( pBoxPtr != 0 );

					file::FileReadFloat( &pBoxPtr->numeric1, sizeof(int), 1, &fileHandle );
					file::FileReadFloat( &pBoxPtr->numeric2, sizeof(int), 1, &fileHandle );
					file::FileReadFloat( &pBoxPtr->numeric3, sizeof(int), 1, &fileHandle );
					file::FileReadFloat( &pBoxPtr->numeric4, sizeof(int), 1, &fileHandle );
					file::FileReadFloat( &pBoxPtr->numeric5, sizeof(int), 1, &fileHandle );

					file::FileReadFloat( &pBoxPtr->pos, sizeof(float), 3, &fileHandle );

					file::FileReadFloat( &pBoxPtr->rot, sizeof(float), 3, &fileHandle );

					file::FileReadFloat( &pBoxPtr->dims, sizeof(float), 3, &fileHandle );

					++boxIndex;
				}break;
			case ODETYPE_SPHERE: // sphere
				{
					res::PhysicsSphereShape* pSpherePtr = &newResource.block->sphereList[sphereIndex];
					DBG_ASSERT( pSpherePtr != 0 );

					file::FileReadFloat( &pSpherePtr->numeric1, sizeof(int), 1, &fileHandle );
					file::FileReadFloat( &pSpherePtr->numeric2, sizeof(int), 1, &fileHandle );
					file::FileReadFloat( &pSpherePtr->numeric3, sizeof(int), 1, &fileHandle );
					file::FileReadFloat( &pSpherePtr->numeric4, sizeof(int), 1, &fileHandle );
					file::FileReadFloat( &pSpherePtr->numeric5, sizeof(int), 1, &fileHandle );

					file::FileReadFloat( &pSpherePtr->pos, sizeof(float), 3, &fileHandle );

					file::FileReadFloat( &pSpherePtr->radius, sizeof(float), 1, &fileHandle );

					++sphereIndex;
				}break;
			case ODETYPE_CAPSULE:
				{
					res::PhysicsCapsuleShape* pCapsulePtr = &newResource.block->capsuleList[capsuleIndex];
					DBG_ASSERT( pCapsulePtr != 0 );

					file::FileReadFloat( &pCapsulePtr->numeric1, sizeof(int), 1, &fileHandle );
					file::FileReadFloat( &pCapsulePtr->numeric2, sizeof(int), 1, &fileHandle );
					file::FileReadFloat( &pCapsulePtr->numeric3, sizeof(int), 1, &fileHandle );
					file::FileReadFloat( &pCapsulePtr->numeric4, sizeof(int), 1, &fileHandle );
					file::FileReadFloat( &pCapsulePtr->numeric5, sizeof(int), 1, &fileHandle );

					file::FileReadFloat( &pCapsulePtr->pos, sizeof(float), 3, &fileHandle );

					file::FileReadFloat( &pCapsulePtr->rot, sizeof(float), 3, &fileHandle );

					file::FileReadFloat( &pCapsulePtr->radius, sizeof(float), 1, &fileHandle );
					file::FileReadFloat( &pCapsulePtr->length, sizeof(float), 1, &fileHandle );

					++capsuleIndex;
				}break;
			case ODETYPE_CYLINDER:
				{
					res::PhysicsCylinderShape* pCylinderPtr = &newResource.block->cylinderList[cylinderIndex];
					DBG_ASSERT( pCylinderPtr != 0 );

					file::FileReadFloat( &pCylinderPtr->numeric1, sizeof(int), 1, &fileHandle );
					file::FileReadFloat( &pCylinderPtr->numeric2, sizeof(int), 1, &fileHandle );
					file::FileReadFloat( &pCylinderPtr->numeric3, sizeof(int), 1, &fileHandle );
					file::FileReadFloat( &pCylinderPtr->numeric4, sizeof(int), 1, &fileHandle );
					file::FileReadFloat( &pCylinderPtr->numeric5, sizeof(int), 1, &fileHandle );

					file::FileReadFloat( &pCylinderPtr->pos, sizeof(float), 3, &fileHandle );

					file::FileReadFloat( &pCylinderPtr->rot, sizeof(float), 3, &fileHandle );

					file::FileReadFloat( &pCylinderPtr->radius, sizeof(float), 1, &fileHandle );

					file::FileReadFloat( &pCylinderPtr->length, sizeof(float), 1, &fileHandle );

					++cylinderIndex;
				}break;
			case ODETYPE_TRIMESH: // mesh
				{
					res::PhysicsTriMesh* pTriMesh = &newResource.block->triMeshList[triMeshIndex];
					DBG_ASSERT( pTriMesh != 0 );

					pTriMesh->totalMeshTriangles = 0;
					pTriMesh->indices = 0;
					pTriMesh->triangleData = 0;
					pTriMesh->normals = 0;

					file::FileReadFloat( &pTriMesh->numeric1, sizeof(int), 1, &fileHandle );
					file::FileReadFloat( &pTriMesh->numeric2, sizeof(int), 1, &fileHandle );
					file::FileReadFloat( &pTriMesh->numeric3, sizeof(int), 1, &fileHandle );
					file::FileReadFloat( &pTriMesh->numeric4, sizeof(int), 1, &fileHandle );
					file::FileReadFloat( &pTriMesh->numeric5, sizeof(int), 1, &fileHandle );

					unsigned int totalMeshTriangles = 0;
					file::FileReadUInt( &totalMeshTriangles, sizeof(unsigned int), 1, &fileHandle );

					//unsigned int meshPolyListCount = 0;
					//file::FileReadUInt( &meshPolyListCount, sizeof(unsigned int), 1, &fileHandle );

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
					for( j = 0; j < polyMeshCount; ++j )
					{
						unsigned int polyCount = 0;
						file::FileReadUInt( &polyCount, sizeof(unsigned int), 1, &fileHandle );

						for( k = 0; k < polyCount; ++k )
						{
							file::FileReadFloat( &pTriMesh->triangleData[pointIndex], sizeof(float), 3, &fileHandle );
							file::FileReadFloat( &pTriMesh->triangleData[pointIndex+1], sizeof(float), 3, &fileHandle );
							file::FileReadFloat( &pTriMesh->triangleData[pointIndex+2], sizeof(float), 3, &fileHandle );

							math::Vec3 edge1 = pTriMesh->triangleData[(pointIndex+1)] - pTriMesh->triangleData[(pointIndex+0)];
							math::Vec3 edge2 = pTriMesh->triangleData[(pointIndex+2)] - pTriMesh->triangleData[(pointIndex+0)];

							pTriMesh->normals[normalCount] = CrossProduct( edge1, edge2 );
							pTriMesh->normals[normalCount].normalise();

							normalCount++;

							pointIndex+=3;
						}
					}

					for( j = 0; j < pTriMesh->totalMeshTriangles*3; j++ )
						pTriMesh->indices[j] = j;

					++triMeshIndex;
				}break;
			
				default:
					DBG_ASSERT_MSG( 0, "Unknown ode physics type in prefab file" );
				break;
		}
	}

	file::FileClose( &fileHandle );

	return(0);
}

/////////////////////////////////////////////////////
/// Method: CreatePhysicsResourceMap
/// Params: None
///
/////////////////////////////////////////////////////
void res::CreatePhysicsResourceMap()
{
	ResourceMap.clear();

#ifdef _DEBUG
	if( script::LuaFunctionCheck( "RegisterPhysicsResources" ) == 0 )
#endif // _DEBUG
	{
		int result = 0;

		int errorFuncIndex;
		errorFuncIndex = script::GetErrorFuncIndex();

		// get the function
		lua_getglobal( script::LuaScripting::GetState(), "RegisterPhysicsResources" );

		result = lua_pcall( script::LuaScripting::GetState(), 0, 1, errorFuncIndex );

		// LUA_ERRRUN --- a runtime error. 
		// LUA_ERRMEM --- memory allocation error. For such errors, Lua does not call the error handler function. 
		// LUA_ERRERR --- error while running the error handler function. 

		if( result == LUA_ERRRUN || result == LUA_ERRMEM || result == LUA_ERRERR )
		{
			DBGLOG( "RESOURCES: *ERROR* Calling function '%s' failed\n", "RegisterPhysicsResources" );
			DBGLOG( "\tLUA_TRACEBACK: %s\n", lua_tostring( script::LuaScripting::GetState(), -1 ) );
			
			DBG_ASSERT_MSG( 0, "RESOURCES: *ERROR* Calling function '%s' failed", "RegisterPhysicsResources" );
			return;
		}

		// should be a table of tables
		if( lua_istable( script::LuaScripting::GetState(), -1 ) )
		{
			int i=0;
			int n = luaL_len( script::LuaScripting::GetState(), -1 );

			// go through all the tables in this table
			for( i = 1; i <= n; ++i )
			{
				lua_rawgeti( script::LuaScripting::GetState(), -1, i );
				if( lua_istable( script::LuaScripting::GetState(), -1 ) )
				{
					int innerCount = luaL_len( script::LuaScripting::GetState(), -1 );
					
					if( innerCount == 4 )
					{
						int paramIndex = 1;

						// id first
						lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
						int resId = static_cast<int>( lua_tonumber( script::LuaScripting::GetState(), -1 ) );

						lua_pop( script::LuaScripting::GetState(), 1 );

						// filename
						lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
						const char* resName = lua_tostring( script::LuaScripting::GetState(), -1 );

						lua_pop( script::LuaScripting::GetState(), 1 );

						// optional model index
						lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
						int modelId = static_cast<int>( lua_tonumber( script::LuaScripting::GetState(), -1 ) );

						lua_pop( script::LuaScripting::GetState(), 1 );

						// object density
						lua_rawgeti( script::LuaScripting::GetState(), -1, paramIndex++ );
						float density = static_cast<float>( lua_tonumber( script::LuaScripting::GetState(), -1 ) );

						lua_pop( script::LuaScripting::GetState(), 1 );

						// load the physics resource and set its data
						PhysicsResourceStore resourceInfo;
						resourceInfo.block = 0;
						resourceInfo.modelResource = modelId;
						resourceInfo.density = density;

						// no physics data (probably just a model)
						if( resName != 0 )
						{
							// parse the physics data
							if( LoadAndParseOde( resName, resourceInfo ) == 0 )
							{
								ResourceMap.insert( TIntPhysicsPair( resId, resourceInfo ) );
							}
							else
							{
								DBG_ASSERT( 0 );
							}
						}
						else
							ResourceMap.insert( TIntPhysicsPair( resId, resourceInfo ) );
					}
					else
					{
						DBG_ASSERT(0);
					}
				}
				lua_pop( script::LuaScripting::GetState(), 1 );	
			}
			lua_pop( script::LuaScripting::GetState(), 1 );
		}
	}
}

/////////////////////////////////////////////////////
/// Method: ClearPhysicsResources
/// Params: None
///
/////////////////////////////////////////////////////
void res::ClearPhysicsResources()
{
	TIntPhysicsMap::iterator resIt;
	unsigned int i=0;

	resIt = ResourceMap.begin();

	while( resIt != ResourceMap.end() )
	{
		if( resIt->second.block != 0 )
		{
			delete[] resIt->second.block->boxList;
			delete[] resIt->second.block->sphereList;
			delete[] resIt->second.block->capsuleList;
			delete[] resIt->second.block->cylinderList;
				
			for( i=0; i < resIt->second.block->totalPhysicsTriMesh; ++i )
			{
				if( resIt->second.block->triMeshList[i].triangleData != 0 )
					delete[] resIt->second.block->triMeshList[i].triangleData;

				if( resIt->second.block->triMeshList[i].indices != 0 )
					delete[] resIt->second.block->triMeshList[i].indices;

				if( resIt->second.block->triMeshList[i].normals != 0 )
					delete[] resIt->second.block->triMeshList[i].normals;	
			}
			delete[] resIt->second.block->triMeshList;

			delete resIt->second.block;
		}
		resIt++;
	}

	ResourceMap.clear();
}

/////////////////////////////////////////////////////
/// Method: GetPhysicsResource
/// Params: [in]index
///
/////////////////////////////////////////////////////
const res::PhysicsResourceStore* res::GetPhysicsResource( int index )
{
	TIntPhysicsMap::iterator resIt;
	
	resIt = ResourceMap.find( index );

	if( resIt != ResourceMap.end() )
		return( &resIt->second );

	return(0);
}

/////////////////////////////////////////////////////
/// Method: GetPhysicsResourceMap
/// Params: None
///
/////////////////////////////////////////////////////
const std::map< int, res::PhysicsResourceStore >& res::GetPhysicsResourceMap()
{
	return ResourceMap;
}
