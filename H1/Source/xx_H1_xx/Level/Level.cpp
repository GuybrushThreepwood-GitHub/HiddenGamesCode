
/*===================================================================
	File: Level.cpp
	App: AirCadets

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include <cmath>
#include <vector>
#include "MathBase.h"
#include "RenderBase.h"
#include "CollisionBase.h"
#include "SoundBase.h"
#include "Model/ModelCommon.h"
#include "ModelBase.h"

#include "Audio/AudioSystem.h"
#include "Resources/EmitterResources.h"
#include "Resources/TextureResources.h"
#include "Resources/ModelResources.h"
#include "Resources/StringResources.h"
#include "Resources/ObjectResources.h"
#include "Player/Player.h"

#include "Profiles/ProfileManager.h"

#include "Level.h"
#include "GameSystems.h"

using mdl::ModelHGM;
using mdl::ModelHGA;

namespace
{

	math::Vec3 zeroVec(0.0f,0.0f,0.0f);
	//math::Vec4Lite targetSpriteColour( 255,255,255,255 );

	GLfloat m[16] = 
	{ 1.0f, 0.0f, 0.0f, 0.0f,
	  0.0f, 1.0f, 0.0f, 0.0f,
	  0.0f, 0.0f, 1.0f, 0.0f,
	  0.0f, 0.0f, 0.0f, 1.0f };

	// temp data
	struct groupData
	{
		int groupId;
		int numGates;
	};
}

///
void PhysicsTestRayToWorld (void *data, dGeomID o1, dGeomID o2);

/////////////////////////////////////////////////////
/// Default constructor
/// 
///
/////////////////////////////////////////////////////
Level::Level()
{
	SetBaseId( PHYSICSBASICID_WORLD );
	SetCastingId( PHYSICSCASTID_WORLD );

	Initialise();
}

/////////////////////////////////////////////////////
/// Default destructor
/// 
///
/////////////////////////////////////////////////////
Level::~Level()
{
	Release();
}

/////////////////////////////////////////////////////
/// Method: Initialise
/// Params: None
///
/////////////////////////////////////////////////////
void Level::Initialise( void )
{
	int i=0;
	m_GameData = GetScriptDataHolder()->GetGameData();
	m_DevData = GetScriptDataHolder()->GetDevData();

    //m_GameData.MAX_SPRITES_PER_BATCH = 4000;
    //m_GameData.MAX_SPRITE_DRAW_DISTANCE = 6000.0f;
    //m_GameData.SPHERE_DRAW_DISTANCE_ON_PLANE = 4000.0f;
    
	m_pMeshData = 0;
	m_PhysicsData = 0;
	m_TotalTriMeshes = 0;
	m_TriMeshData = 0;

	m_EntitySpace = 0;
	m_CategoryBits = CATEGORY_WORLD;
	m_CollideBits = (CATEGORY_RAYCAST | CATEGORY_BULLET | CATEGORY_PLAYER);

	m_PhysicsIdList.clear();

	m_EmitterList.clear();
	m_SoundAreas.clear();
	m_SpawnPoints.clear();
	m_Gates.clear();
	m_Sights.clear();
	m_GateGroups.clear();

	m_NumBatchesInUse = 0;

	m_PlayerStartPoint = math::Vec2( 0.0f, 5.0f );

	m_ActiveGateGroup = -1;
	m_PlayerRespawnPoint = zeroVec;
	m_PlayerRespawnRotation = zeroVec;

	m_SpriteTextureId = renderer::INVALID_OBJECT;
	m_SpriteCount = 0;
	m_SpriteData.clear();
	m_SpriteList = 0;

	m_SpriteTextureId = res::GetTextureResource( -100 )->texId;

	m_NumTargetsUsed = 0;
	m_TargetObjId = -1;
	m_TotalNumTargets = 0;
	m_NextTargetIndex = 0;
	for( i=0; i < MAX_TARGET_DRAWS; ++i )
	{
		m_TargetData[i].inUse = false;
		m_TargetData[i].unprojPoint = math::Vec3( -10000.0f, -10000.0f, -10000.0f );
		m_TargetData[i].distance = 999999.0f;
		m_TargetData[i].pObj = 0;
	}

	m_TotalDestroyable = 0;

	m_TokenRotation = 0.0f;
	m_TokenWasCollected = false;
	m_AllCadetsCollected = false;
	m_TotalCadetTokens = 0;

	m_TotalTakuTokens = 0;
	m_TotalMeiTokens = 0;
	m_TotalEarlTokens = 0;
	m_TotalFioTokens = 0;
	m_TotalMitoTokens = 0;
	m_TotalUkiTokens = 0;

	m_AllCadetTokensCollectedCount = 0;
	m_TakuCollectedCount = 0;
	m_MeiCollectedCount = 0;
	m_EarlCollectedCount = 0;
	m_FioCollectedCount = 0;
	m_MitoCollectedCount = 0;
	m_UkiCollectedCount = 0;

	m_StarCollectedCount = 0;
	m_HiddenTokenCollectedCount = 0;

	for( i=0; i < TOTAL_CADETS*TOTAL_TOKENS_PER_CADET; ++i )
	{
		m_CadetTokens[i].tokenId = -1;
		m_CadetTokens[i].collected = false;
		m_CadetTokens[i].s.vCenterPoint = math::Vec3(0.0f, 0.0f, 0.0f);
		m_CadetTokens[i].s.fRadius = m_GameData.DEFAULT_TOKEN_COLLISION_RADIUS;
		m_CadetTokens[i].pModel = 0;
	}

	m_TotalStarTokens = 0;
	for( i=0; i < STAR_TOKENS; ++i )
	{
		m_StarTokens[i].tokenId = -1;
		m_StarTokens[i].collected = false;
		m_StarTokens[i].s.vCenterPoint = math::Vec3(0.0f, 0.0f, 0.0f);
		m_StarTokens[i].s.fRadius = m_GameData.DEFAULT_TOKEN_COLLISION_RADIUS;
		m_StarTokens[i].pModel = 0;
	}

	m_TotalHiddenTokens = 0;
	for( i=0; i < HIDDEN_TOKENS; ++i )
	{
		m_HiddenTokens[i].tokenId = -1;
		m_HiddenTokens[i].collected = false;
		m_HiddenTokens[i].s.vCenterPoint = math::Vec3(0.0f, 0.0f, 0.0f);
		m_HiddenTokens[i].s.fRadius = m_GameData.DEFAULT_TOKEN_COLLISION_RADIUS;
		m_HiddenTokens[i].pModel = 0;
	}

	m_pSkyData = 0;
	m_SkyTextureId = renderer::INVALID_OBJECT;
	m_SkyUseFog = false;
	m_SkyboxOffset.setZero();
	m_SkyRot = 0.0f;

	m_DetailMapTexture = renderer::INVALID_OBJECT;
	m_DetailMapScaleX = 1.0f;
	m_DetailMapScaleY = 1.0f;

	m_BatchSprites.SetAlphaBlend( m_DevData.alphaBlendBatchSprites, m_DevData.alphaBlendValue );
	m_BatchSprites.SetDrawStyle( m_DevData.batchSpriteDrawMode );

	m_TokenCollect = AudioSystem::GetInstance()->AddAudioFile( 500/*cadet_pickup.wav*/ );
	m_StarCollect = AudioSystem::GetInstance()->AddAudioFile( 501/*star_pickup.wav*/ );
	m_HiddenIconCollect = AudioSystem::GetInstance()->AddAudioFile( 502/*hiddenicon_pickup.wav*/ );
}

/////////////////////////////////////////////////////
/// Method: Release
/// Params: None
///
/////////////////////////////////////////////////////
void Level::Release( void )
{
	unsigned int i=0;

	m_Coverage.Free();

	if( m_SkyTextureId != renderer::INVALID_OBJECT )
	{
		renderer::RemoveTexture( m_SkyTextureId );
		m_SkyTextureId = renderer::INVALID_OBJECT;
	}

	if( m_DetailMapTexture != renderer::INVALID_OBJECT )
	{
		renderer::RemoveTexture( m_DetailMapTexture );
		m_DetailMapTexture = renderer::INVALID_OBJECT;
	}

	for( i=0; i < m_PhysicsIdList.size(); ++i )
	{
		delete m_PhysicsIdList[i];
		m_PhysicsIdList[i] = 0;
	}

	for( i=0; i < m_EmitterList.size(); ++i )
	{
		delete m_EmitterList[i];
		m_EmitterList[i] = 0;
	}
	

	if( m_PhysicsData != 0 )
	{
		if( m_PhysicsData->boxList )
			delete[] m_PhysicsData->boxList;

		if( m_PhysicsData->sphereList )
			delete[] m_PhysicsData->sphereList;

		if( m_PhysicsData->capsuleList )
			delete[] m_PhysicsData->capsuleList;

		if( m_PhysicsData->cylinderList )
			delete[] m_PhysicsData->cylinderList;

		for( i=0; i < static_cast<unsigned int>(m_TotalTriMeshes); ++i )	
		{
			dGeomTriMeshDataDestroy( m_TriMeshData[i].triMeshDataID );
			m_TriMeshData[i].triMeshDataID = 0;
			
			if( m_TriMeshData[i].triangleData != 0 )
				delete[] m_TriMeshData[i].triangleData;

			if( m_TriMeshData[i].indices != 0 )
				delete[] m_TriMeshData[i].indices;

			if( m_TriMeshData[i].normals != 0 )
				delete[] m_TriMeshData[i].normals;	
		}
	
		if( m_TriMeshData != 0 )
			delete[] m_TriMeshData;

		if( m_PhysicsData )
		{
			delete m_PhysicsData;
			m_PhysicsData = 0;
		}
	}

	if( m_pSkyData != 0 )
	{
		res::RemoveModel( m_pSkyData );
		m_pSkyData = 0;
	}

	if( m_pMeshData != 0 )
	{
		res::RemoveModel( m_pMeshData );
		m_pMeshData = 0;
	}

	for( i=0; i < TOTAL_CADETS*TOTAL_TOKENS_PER_CADET; ++i )
	{
		if( m_CadetTokens[i].pModel != 0 )
			res::RemoveModel(m_CadetTokens[i].pModel );
	}

	for( i=0; i < STAR_TOKENS; ++i )
	{
		if( m_StarTokens[i].pModel != 0 )
			res::RemoveModel( m_StarTokens[i].pModel );
	}

	for( i=0; i < HIDDEN_TOKENS; ++i )
	{
		if( m_HiddenTokens[i].pModel != 0 )
			res::RemoveModel(m_HiddenTokens[i].pModel);
	}

	for( i=0; i < m_SoundAreas.size(); ++i )
	{
		delete m_SoundAreas[i];
		m_SoundAreas[i] = 0;
	}
	
	AudioSystem::GetInstance()->StopAll();

	for( i=0; i < m_Gates.size(); ++i  )
	{
		if( m_Gates[i].model != 0 )
		{
			res::RemoveModel( m_Gates[i].model );
		}
	}
	m_Gates.clear();
	m_Sights.clear();

	for( i=0; i < m_GateGroups.size(); ++i  )
	{
		if( m_GateGroups[i].gateList != 0 )
		{
			delete[] m_GateGroups[i].gateList;
			m_GateGroups[i].gateList = 0;
		}
	}
	m_GateGroups.clear();

	// sprite data
	m_SpriteCount = 0;
	for( i=0; i < m_SpriteData.size(); ++i )
	{
		delete m_SpriteData[i];
		m_SpriteData[i] = 0;
	}
	m_SpriteData.clear();

	delete[] m_SpriteList;
	m_SpriteList = 0;

	// clear out all lists
	m_PhysicsIdList.clear();
	m_EmitterList.clear();
	m_SoundAreas.clear();
	m_SpawnPoints.clear();

	m_NumBatchesInUse = 0;
}

/////////////////////////////////////////////////////
/// Method: LoadMeshData
/// Params: [in]meshResId
///
/////////////////////////////////////////////////////
int Level::LoadMeshData( int meshResId, bool useAABBCull )
{
	//Release();

	if( meshResId != -1 )
	{
		m_pMeshData = res::LoadModel( meshResId );
		DBG_ASSERT( m_pMeshData != 0 );

		if( useAABBCull )
			m_pMeshData->SetModelGeneralFlags( mdl::MODELFLAG_GENERAL_AUTOCULLING | mdl::MODELFLAG_GENERAL_AABBCULL | mdl::MODELFLAG_GENERAL_SUBMESHCULL /*| mdl::MODELFLAG_GENERAL_DRAWBOUNDINGBOX*/ );
	}
	return(0);
}


/////////////////////////////////////////////////////
/// Method: LoadComplexPhysicsData
/// Params: [in]szFilename
/// 
/////////////////////////////////////////////////////
int Level::LoadComplexPhysicsData( const char *szFilename )
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
	m_TotalTriMeshes = pBlock->totalPhysicsTriMesh;

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

	//DBG_ASSERT( pBlock->triMeshList == 0 );
	if( pBlock->totalPhysicsTriMesh > 0 )
	{
		m_TriMeshData = new TerrainTriMesh[m_TotalTriMeshes];
		DBG_ASSERT( m_TriMeshData != 0 );
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

					file::FileReadFloat( &pCylinderPtr->radius, sizeof(float), 1, &fileHandle );

					file::FileReadFloat( &pCylinderPtr->length, sizeof(float), 1, &fileHandle );

					++cylinderIndex;
				}break;
			case ODETYPE_TRIMESH: // mesh
				{
					TerrainTriMesh* pTriMesh = &m_TriMeshData[triMeshIndex];
					DBG_ASSERT( pTriMesh != 0 );

					pTriMesh->index = triMeshIndex;
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
/// Method: LoadData
/// Params: [in]szFilename
/// 
/////////////////////////////////////////////////////
int Level::LoadData( const char* szFilename, bool targetMode, int whichResId, int objRes1, int objRes2, int objRes3  )
{
	int i=0, j=0;

	if( core::IsEmptyString(szFilename) )
		return 0;

	// load the data script
	if( script::LoadScript( szFilename ) == 0 )
	{
		// get the data table
		lua_getglobal( script::LuaScripting::GetState(), "data" );

		if( lua_istable( script::LuaScripting::GetState(), -1 ) )
		{
			int numTypes = static_cast<int>( script::LuaGetNumberFromTableItem( "numTypes", 1, 0.0 ) );
			m_NumObjects = static_cast<int>( script::LuaGetNumberFromTableItem( "numObjects", 1, 0.0 ) );

			if( m_NumObjects > 0 )
			{
				// grab what levels were generated for this career
				lua_pushstring( script::LuaScripting::GetState(), "objects" );
				lua_gettable( script::LuaScripting::GetState(), -2 );

				if( lua_istable( script::LuaScripting::GetState(), -1 ) )
				{
					int n = luaL_len( script::LuaScripting::GetState(), -1 );
				
					// go through all the tables in this table
					for( i = 1; i <= n; ++i )
					{
						lua_rawgeti( script::LuaScripting::GetState(), -1, i );
						if( lua_istable( script::LuaScripting::GetState(), -1 ) )
						{
							int paramIndex = 3;

							int baseType = -9999;	
							int baseId	= -9999;
									
							ParseObject( paramIndex, baseType, baseId, targetMode, whichResId, objRes1, objRes2, objRes3 );

						}
						lua_pop( script::LuaScripting::GetState(), 1 );
					}
				}
				lua_pop( script::LuaScripting::GetState(), 1 );
			}

			if( numTypes > 0 )
			{
				// grab what levels were generated for this career
				lua_pushstring( script::LuaScripting::GetState(), "types" );
				lua_gettable( script::LuaScripting::GetState(), -2 );

				if( lua_istable( script::LuaScripting::GetState(), -1 ) )
				{
					int n = luaL_len( script::LuaScripting::GetState(), -1 );
				
					// go through all the tables in this table
					for( i = 1; i <= n; ++i )
					{
						lua_rawgeti( script::LuaScripting::GetState(), -1, i );
						if( lua_istable( script::LuaScripting::GetState(), -1 ) )
						{
							int paramIndex = 3;

							int baseType = static_cast<int>( script::LuaGetNumberFromTableItem( "baseType", paramIndex, -9999.0 ) );	
							int baseId	= static_cast<int>( script::LuaGetNumberFromTableItem( "baseId", paramIndex, -9999.0 ) );

							// DBGLOG( "Type = %d\n Id = %d\n", baseType, baseId );
							switch( baseType )
							{
								case POSROT:
								{
									ParsePosRot( paramIndex, baseType, baseId );
								}break;
								case SPAWN_POINT:
								{
									ParseSpawnPoint( paramIndex, baseType, baseId );
								}break;
								case GATE:
								{
									ParseGate( paramIndex, baseType, baseId );
								}break;
								case TARGET:
								{
									ParseTarget( paramIndex, baseType, baseId );
								}break;
								case SPRITE:
								{
									ParseSprite( paramIndex, baseType, baseId );
								}break;
								case EMITTER:
								{
									ParseEmitter( paramIndex, baseType, baseId );
								}break;
								case SOUND:
								{
									ParseSound( paramIndex, baseType, baseId );
								}break;
								default:
									DBG_ASSERT(0);
								break;
							}
						}
						lua_pop( script::LuaScripting::GetState(), 1 );
					}
				}
				lua_pop( script::LuaScripting::GetState(), 1 );
			}
		}

		lua_pop( script::LuaScripting::GetState(), 1 );
	}

	// put in a default spawn
	if( m_SpawnPoints.size() <= 0 )
	{
		SpawnPoint defaultSpawn;
		defaultSpawn.pos = math::Vec3( 0.0f, 1000.0f, 0.0f );
		defaultSpawn.rot = math::Vec3( 0.0f, 0.0f, 0.0f );
		defaultSpawn.spawnType = -1;
		defaultSpawn.spawnGroup = -1;

		m_SpawnPoints.push_back(defaultSpawn);
	}

	// create the sprite list
	if( m_SpriteData.size() )
	{
		m_SpriteCount = static_cast<unsigned int>( m_SpriteData.size()*6 );

		m_SpriteList = new SpriteVert[m_SpriteCount];

		for( i=0, j=0; i < m_SpriteCount; i+=6, j++ )
		{
			SpriteType *s = m_SpriteData[j];
			const res::SpriteResourceStore* sr = res::GetSpriteResource( s->resId );

			if( sr != 0 )
			{
				m_SpriteList[i].uv		= math::Vec2( sr->uOffset,			sr->vOffset );
				m_SpriteList[i+1].uv	= math::Vec2( sr->uOffset + sr->w,	sr->vOffset );
				m_SpriteList[i+2].uv	= math::Vec2( sr->uOffset,			sr->vOffset + sr->h );
				m_SpriteList[i+3].uv	= math::Vec2( sr->uOffset + sr->w,	sr->vOffset );
				m_SpriteList[i+4].uv	= math::Vec2( sr->uOffset + sr->w,	sr->vOffset + sr->h );
				m_SpriteList[i+5].uv	= math::Vec2( sr->uOffset,			sr->vOffset + sr->h );
			}

			m_SpriteList[i].col		= math::Vec4Lite( s->col.R, s->col.G, s->col.B, s->col.A );
			m_SpriteList[i+1].col	= math::Vec4Lite( s->col.R, s->col.G, s->col.B, s->col.A );
			m_SpriteList[i+2].col	= math::Vec4Lite( s->col.R, s->col.G, s->col.B, s->col.A );
			m_SpriteList[i+3].col	= math::Vec4Lite( s->col.R, s->col.G, s->col.B, s->col.A );
			m_SpriteList[i+4].col	= math::Vec4Lite( s->col.R, s->col.G, s->col.B, s->col.A );
			m_SpriteList[i+5].col	= math::Vec4Lite( s->col.R, s->col.G, s->col.B, s->col.A );
		}
	}

	SetupGateGroups();

	return 0;
}

/////////////////////////////////////////////////////
/// Method: LoadMappingData
/// Params: [in]szHeightData
/// 
/////////////////////////////////////////////////////
int Level::LoadMappingData( const char* szCoverageData, const spriteBatchData* spriteBatch, int spriteBatchCount )
{
	int i=0, j=0;

	m_BatchSprites.Clear();
	m_BatchSprites.SetMaxDrawDistance( m_GameData.MAX_SPRITE_DRAW_DISTANCE );

	TerrainTriMesh* pTriMesh = 0;
	TerrainTriMesh tempTriMesh;

	if( core::IsEmptyString( szCoverageData ) )
		return 1;

	if( m_Coverage.Load( szCoverageData ) )
		return 1;

	// ray 
	RayObject* pRayTest = new RayObject( physics::PhysicsWorldODE::GetEditorSpace(), 1.0f ); //GameSystems::GetInstance()->GetRayObject();
	DBG_ASSERT( pRayTest != 0 );

	if( spriteBatchCount > 0 )
	{
		for( i=0; i < m_TotalTriMeshes; ++i )
		{
			dGeomID geom = 0;
			tempTriMesh.triMeshDataID = dGeomTriMeshDataCreate();
			geom = CreateGeom( dTriMeshClass, physics::PhysicsWorldODE::GetEditorSpace(), zeroVec, 0.0f, 0.0f, tempTriMesh.triMeshDataID );
			tempTriMesh.geomId = geom;

			dGeomSetCategoryBits( geom, m_CategoryBits );
			dGeomSetCollideBits( geom, m_CollideBits );

			tempTriMesh.index = i;
			tempTriMesh.SetBaseId(PHYSICSBASICID_WORLD);
			tempTriMesh.SetCastingId(PHYSICSCASTID_TERRAIN);

			dGeomSetData(geom, &tempTriMesh);
			DBG_ASSERT_MSG( (geom != 0), "Could not create tri-mesh" );

			dGeomTriMeshDataBuildSingle1( tempTriMesh.triMeshDataID, m_TriMeshData[i].triangleData, sizeof(math::Vec3), m_TriMeshData[i].totalMeshTriangles*3, m_TriMeshData[i].indices, m_TriMeshData[i].totalMeshTriangles*3, sizeof(int), m_TriMeshData[i].normals );

			tempTriMesh.meshAABB = m_TriMeshData[i].meshAABB;

			// create sprites
			int processCount = 0;
			int spriteUsed = 0;

			for( j=0; j < m_GameData.MAX_SPRITES_PER_BATCH; ++j )
			{
				math::Vec3 pos( math::RandomNumber(m_TriMeshData[i].meshAABB.vBoxMin.X, m_TriMeshData[i].meshAABB.vBoxMax.X), math::RandomNumber(m_TriMeshData[i].meshAABB.vBoxMin.Y, m_TriMeshData[i].meshAABB.vBoxMax.Y), math::RandomNumber(m_TriMeshData[i].meshAABB.vBoxMin.Z, m_TriMeshData[i].meshAABB.vBoxMax.Z) );

				// get the position of the AABB and convert it to a pixel coordinate
				float tmpX = (pos.X + 8000.0f) / HEIGHTMAP_SCALE;
				float tmpZ = (pos.Z + 8000.0f) / HEIGHTMAP_SCALE;

				int pixelCoordX = static_cast<int>(tmpX * static_cast<float>(m_Coverage.nWidth));
				int pixelCoordY = m_Coverage.nHeight - static_cast<int>(tmpZ * static_cast<float>(m_Coverage.nHeight));
				
				math::Clamp( &pixelCoordX, 0, m_Coverage.nWidth-1 );
				math::Clamp( &pixelCoordY, 0, m_Coverage.nHeight-1 );

				int pixelValue = static_cast<int>(m_Coverage.image[pixelCoordX + (pixelCoordY*m_Coverage.nHeight)]);

				if( pixelValue >= 246 &&
					pixelValue <= 255 )
				{
					// ray check
					pRayTest->hasHits = false;
					pRayTest->totalHits = 0;
					pRayTest->nextHitIndex = 0;

					// start the ray high
					pRayTest->rayPos = math::Vec3( pos.X, pos.Y + 5000.0f, pos.Z );
					pRayTest->rayDir = math::Vec3( 0.0f, -1.0f, 0.0f );

					dGeomRaySet (pRayTest->rayGeom, pRayTest->rayPos.X, pRayTest->rayPos.Y, pRayTest->rayPos.Z, 
							pRayTest->rayDir.X, pRayTest->rayDir.Y, pRayTest->rayDir.Z );

					dGeomRaySetLength (pRayTest->rayGeom, 10000.0f);

					dGeomSetCategoryBits( pRayTest->rayGeom, CATEGORY_RAYCAST );
					dGeomSetCollideBits( pRayTest->rayGeom, CATEGORY_WORLD );

					dSpaceCollide(physics::PhysicsWorldODE::GetEditorSpace(), 0, &PhysicsTestRayToWorld);

					if( pRayTest->totalHits <= 0 )
					{
						// no point placing a sprite in mid air
						continue;
					}

					// ray hit something
					if( pRayTest->totalHits > 0 )
					{
						// move the position slightly higher
						pTriMesh = reinterpret_cast<TerrainTriMesh*>( pRayTest->rayHits[0].pData );
						DBG_ASSERT( pTriMesh != 0 );

						math::Vec3 terrainPos = math::Vec3( pRayTest->rayHits[0].pos[0], pRayTest->rayHits[0].pos[1], pRayTest->rayHits[0].pos[2] );

						int index = 255 - pixelValue;

						if( (index >= 0) &&  (index<=spriteBatchCount) )
						{
							math::Vec3 dims = spriteBatch[index].dims;
							dims.X += math::RandomNumber( 0.0f, spriteBatch[index].range.X );
							dims.Y += math::RandomNumber( 0.0f, spriteBatch[index].range.Y );
							dims.Z += math::RandomNumber( 0.0f, spriteBatch[index].range.Z );

							m_BatchSprites.AddSprite( pTriMesh->index, pTriMesh->meshAABB, terrainPos, dims, spriteBatch[index].spriteId );

							spriteUsed++;
						}
					}
				}

				// allow multiple loops to use as much of the batch as possible
				if( j+1 == m_GameData.MAX_SPRITES_PER_BATCH )
				{
					if( spriteUsed < m_GameData.MAX_SPRITES_PER_BATCH )
					{
						const int PROCESS_COUNT = 3;

						// to stop infinite loops
						if( processCount < PROCESS_COUNT )
						{
							j = spriteUsed;
							processCount++;
						}
					}
				}
			}	

			dGeomTriMeshDataDestroy( tempTriMesh.triMeshDataID );
			dGeomDestroy(tempTriMesh.geomId);
		}
	}

	m_Coverage.Free();

	if( pRayTest != 0 )
	{
		delete pRayTest;
		pRayTest = 0;
	}

	return 0;
}

/////////////////////////////////////////////////////
/// Method: DetailMapSetup
/// Params: [in]textureResId
/// 
/////////////////////////////////////////////////////
void Level::DetailMapSetup( int textureResId, float scaleX, float scaleY )
{
	if( textureResId != -1 )
	{
		const res::TextureResourceStore* rs = 0;
		rs = res::GetTextureResource( textureResId );
		DBG_ASSERT( rs != 0 );

		res::LoadTexture( textureResId );

		m_DetailMapTexture = rs->texId;

		if( m_pMeshData != 0 )
		{
			m_pMeshData->SetTextureToMaterial( m_DetailMapTexture, -1, 1 );
			m_pMeshData->SetMaterialScale( -1, -1, 1, scaleX, scaleY );
		}
	}

	m_DetailMapScaleX = scaleX;
	m_DetailMapScaleY = scaleY;
}

/////////////////////////////////////////////////////
/// Method: SetupBGQuad
/// Params: [in]textureId, [in]useFog, [in]pos
/// 
/////////////////////////////////////////////////////
void Level::SetupSkybox( int textureResId, bool useFog, const math::Vec3& offset )
{
	const res::TextureResourceStore* rs = 0;
	rs = res::GetTextureResource( textureResId );
	DBG_ASSERT( rs != 0 );

	if( m_pSkyData == 0 )
	{
		m_pSkyData = res::LoadModel( 100 );
	}

	if( m_SkyTextureId != renderer::INVALID_OBJECT )
	{
		renderer::RemoveTexture( m_SkyTextureId );
		m_SkyTextureId = renderer::INVALID_OBJECT;
	}

	res::LoadTexture( textureResId );

	m_SkyTextureId = rs->texId;
	m_SkyUseFog = useFog;
	m_SkyboxOffset = offset;
	m_SkyRot = 0.0f;
	if( m_pSkyData != 0 )
	{
		m_pSkyData->SetTextureToMaterial( m_SkyTextureId, 0, 0 );
	}
}

/////////////////////////////////////////////////////
/// Method: SetTimeOfDay
/// Params: [in]timeOfDay
/// 
/////////////////////////////////////////////////////
void Level::SetTimeOfDay( int timeOfDay )
{
	m_TimeOfDay = timeOfDay;
}

/////////////////////////////////////////////////////
/// Method: SetGateGroup
/// Params: [in]groupId
/// 
/////////////////////////////////////////////////////
void Level::SetGateGroup( int groupId )
{
	if( groupId < 0 || 
		groupId > static_cast<int>(m_GateGroups.size()-1) )
	{
		DBG_ASSERT(0);
		m_ActiveGateGroup = -1;

		m_PlayerStartPoint = m_SpawnPoints[0].pos;
		m_PlayerSpawnRot = m_SpawnPoints[0].rot;
	}
	else
	{
		m_ActiveGateGroup = groupId;

		m_PlayerStartPoint = m_GateGroups[m_ActiveGateGroup].spawnPos;
		m_PlayerSpawnRot = m_GateGroups[m_ActiveGateGroup].spawnRot;
	}
}

/////////////////////////////////////////////////////
/// Method: DrawMainLevel
/// Params: None
/// 
/////////////////////////////////////////////////////
void Level::DrawMainLevel()
{
	int i=0, j=0;

	renderer::OpenGL::GetInstance()->SetColour4ub( 255, 255, 255, 255 );
	renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	renderer::OpenGL::GetInstance()->AlphaMode( false, GL_ALWAYS, 0.0f );
	renderer::OpenGL::GetInstance()->DepthMode( true, GL_LESS);
	
	if( m_DetailMapTexture != renderer::INVALID_OBJECT )
	{
		glActiveTexture( GL_TEXTURE1 );
		glDisable(GL_TEXTURE_2D);
		glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE );
		glTexEnvf( GL_TEXTURE_ENV, GL_RGB_SCALE, 2.0f );
		glActiveTexture( GL_TEXTURE0 );
	}

	if( m_pMeshData != 0 )
		m_pMeshData->Draw();
	
	if( m_DetailMapTexture != renderer::INVALID_OBJECT )
	{
		glActiveTexture( GL_TEXTURE1 );
		glDisable(GL_TEXTURE_2D);
		glActiveTexture( GL_TEXTURE0 );
	}

	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	collision::Sphere objSphere;
	collision::Sphere playerDraw = m_pPlayer->GetDrawDistanceSphere();

	for( i=0; i < m_NumBatchesInUse; ++i )
	{
		if( renderer::OpenGL::GetInstance()->AABBInFrustum( m_ObjectBatchList[i].objBatchAABB ) ||
			playerDraw.SphereCollidesWithAABB( m_ObjectBatchList[i].objBatchAABB ) )
		{
			for( j=0; j < m_ObjectBatchList[i].numObjects; ++j )
			{
				objSphere = m_ObjectBatchList[i].objectList[j].GetSphere();
				if( renderer::OpenGL::GetInstance()->SphereInFrustum( objSphere.vCenterPoint.X, objSphere.vCenterPoint.Y, objSphere.vCenterPoint.Z, objSphere.fRadius ) &&
					m_ObjectBatchList[i].objectList[j].IsDrawn() )
				{
					m_ObjectBatchList[i].objectList[j].Draw();
				}
			}
		}
	}
}

/////////////////////////////////////////////////////
/// Method: DrawPickups
/// Params: None
/// 
/////////////////////////////////////////////////////
void Level::DrawPickups()
{
	int i=0;
	float distance = 999999.0f;

	renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	for( i=0; i < TOTAL_CADETS*TOTAL_TOKENS_PER_CADET; ++i )
	{
		if( m_CadetTokens[i].tokenId != -1 &&
			renderer::OpenGL::GetInstance()->SphereInFrustum( m_CadetTokens[i].s.vCenterPoint.X, m_CadetTokens[i].s.vCenterPoint.Y, m_CadetTokens[i].s.vCenterPoint.Z, m_CadetTokens[i].s.fRadius ) &&
			!m_CadetTokens[i].collected )
		{
			distance = std::abs( (m_pPlayer->GetPosition() - m_CadetTokens[i].s.vCenterPoint).length() );

			if( m_CadetTokens[i].pModel != 0 &&
				distance <= m_GameData.TOKEN_DRAW_DISTANCE )
			{
				glPushMatrix();
					glTranslatef( m_CadetTokens[i].s.vCenterPoint.X, m_CadetTokens[i].s.vCenterPoint.Y, m_CadetTokens[i].s.vCenterPoint.Z );
					glRotatef( m_TokenRotation, 0.0f, 1.0f, 0.0f );
					m_CadetTokens[i].pModel->Draw();
				glPopMatrix();
			}
		}
	}

	for( i=0; i < HIDDEN_TOKENS; ++i )
	{
		if( m_HiddenTokens[i].tokenId != -1 &&
			renderer::OpenGL::GetInstance()->SphereInFrustum( m_HiddenTokens[i].s.vCenterPoint.X, m_HiddenTokens[i].s.vCenterPoint.Y, m_HiddenTokens[i].s.vCenterPoint.Z, m_HiddenTokens[i].s.fRadius ) &&
			!m_HiddenTokens[i].collected )
		{
			distance = std::abs( (m_pPlayer->GetPosition() - m_HiddenTokens[i].s.vCenterPoint).length() );

			if( m_HiddenTokens[i].pModel != 0 &&
				distance <= m_GameData.TOKEN_DRAW_DISTANCE)
			{
				glPushMatrix();
					glTranslatef( m_HiddenTokens[i].s.vCenterPoint.X, m_HiddenTokens[i].s.vCenterPoint.Y, m_HiddenTokens[i].s.vCenterPoint.Z );
					glRotatef( m_TokenRotation, 0.0f, 1.0f, 0.0f );
					m_HiddenTokens[i].pModel->Draw();
				glPopMatrix();
			}
		}
	}

	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	for( i=0; i < STAR_TOKENS; ++i )
	{
		if( m_StarTokens[i].tokenId != -1 &&
			renderer::OpenGL::GetInstance()->SphereInFrustum( m_StarTokens[i].s.vCenterPoint.X, m_StarTokens[i].s.vCenterPoint.Y, m_StarTokens[i].s.vCenterPoint.Z, m_StarTokens[i].s.fRadius ) &&
			!m_StarTokens[i].collected )
		{
			distance = std::abs( (m_pPlayer->GetPosition() - m_StarTokens[i].s.vCenterPoint).length() );

			if( m_StarTokens[i].pModel != 0 &&
				distance <= m_GameData.TOKEN_DRAW_DISTANCE )
			{
				glPushMatrix();
					glTranslatef( m_StarTokens[i].s.vCenterPoint.X, m_StarTokens[i].s.vCenterPoint.Y, m_StarTokens[i].s.vCenterPoint.Z );
					glRotatef( m_TokenRotation, 0.0f, 1.0f, 0.0f );
					m_StarTokens[i].pModel->Draw();
				glPopMatrix();
			}
		}
	}

	renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}

/////////////////////////////////////////////////////
/// Method: DrawSpriteAndEmitters
/// Params: None
/// 
/////////////////////////////////////////////////////
void Level::DrawSpriteAndEmitters()
{
	unsigned int i=0;
	
	renderer::OpenGL::GetInstance()->SetColour4ub( 255, 255, 255, 255 );
	
	for( i=0; i < m_EmitterList.size(); ++i )
		m_EmitterList[i]->Draw();	

	// sprites
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	m_BatchSprites.Draw();
	renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	if( m_SpriteCount )
	{
		renderer::OpenGL::GetInstance()->DisableVBO();

		renderer::OpenGL::GetInstance()->DepthMode( false, GL_LEQUAL );
		renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

		glClientActiveTexture( GL_TEXTURE0 );
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );

		renderer::OpenGL::GetInstance()->EnableColourArray();

		if( m_SpriteTextureId != renderer::INVALID_OBJECT )
			renderer::OpenGL::GetInstance()->BindTexture( m_SpriteTextureId );

		if( m_SpriteCount )
		{
			glVertexPointer( 3, GL_FLOAT, sizeof(SpriteVert), &m_SpriteList[0].v );
			glTexCoordPointer( 2, GL_FLOAT, sizeof(SpriteVert), &m_SpriteList[0].uv );
			glColorPointer( 4, GL_UNSIGNED_BYTE, sizeof(SpriteVert), &m_SpriteList[0].col );

			glDrawArrays( GL_TRIANGLES, 0, m_SpriteCount );
		}

		renderer::OpenGL::GetInstance()->DisableColourArray();

		renderer::OpenGL::GetInstance()->DepthMode( true, GL_LESS );
		renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		
		glClientActiveTexture( GL_TEXTURE0 );
			glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	}	
}

/////////////////////////////////////////////////////
/// Method: DrawSkybox
/// Params: None
/// 
/////////////////////////////////////////////////////
void Level::DrawSkybox()
{
	renderer::TGLFogState curFogState;
	math::Vec3 camPos = GameSystems::GetInstance()->GetGameCamera()->GetPosition();

	renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	renderer::OpenGL::GetInstance()->DepthMode( false, GL_LESS );

	renderer::OpenGL::GetInstance()->SetColour4ub( 255, 255, 255, 255 );

	bool lightState = renderer::OpenGL::GetInstance()->GetLightingState();
	if( lightState )
		renderer::OpenGL::GetInstance()->DisableLighting();

	if( !m_SkyUseFog )
	{
		renderer::OpenGL::GetInstance()->SaveFogState( &curFogState );

		bool fogState = curFogState.bFogState;
		curFogState.bFogState = false;
		renderer::OpenGL::GetInstance()->SetFogState( &curFogState );
		curFogState.bFogState = fogState;
	}

	if( m_SkyTextureId != renderer::INVALID_OBJECT )
	{
		renderer::OpenGL::GetInstance()->BindTexture(m_SkyTextureId);

		//glClientActiveTexture( GL_TEXTURE0 );
		//glEnableClientState( GL_TEXTURE_COORD_ARRAY );		

		if( m_pSkyData != 0 )
		{
			glPushMatrix();
				glTranslatef( camPos.X+m_SkyboxOffset.X, camPos.Y+m_SkyboxOffset.Y, camPos.Z+m_SkyboxOffset.Z );
				//glRotatef( m_SkyRot, 0.0f, 1.0f, 0.0f );
				m_pSkyData->Draw();
			glPopMatrix();
		}

		//glClientActiveTexture( GL_TEXTURE0 );
		//	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	}

	if( lightState )
		renderer::OpenGL::GetInstance()->EnableLighting();

	if( !m_SkyUseFog )
	{
		renderer::OpenGL::GetInstance()->SetFogState( &curFogState );
	}

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_LESS );
}

/////////////////////////////////////////////////////
/// Method: DrawGates
/// Params: None
/// 
/////////////////////////////////////////////////////
void Level::DrawGates()
{
	std::vector<Gate>::iterator it = m_Gates.begin();

	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	while( it != m_Gates.end() )
	{
		if( (*it).enabled )
		{
			if( (*it).model != 0 )
			{
				glPushMatrix();
					m[0] = (*it).orientation[0];
					m[1] = (*it).orientation[4];
					m[2] = (*it).orientation[8];
					m[3] = 0.0f;
					m[4] = (*it).orientation[1];
					m[5] = (*it).orientation[5];
					m[6] = (*it).orientation[9];
					m[7] = 0.0f;
					m[8] = (*it).orientation[2];
					m[9] = (*it).orientation[6];
					m[10] = (*it).orientation[10];
					m[11] = 0.0f;
					m[12] = (*it).pos.X;
					m[13] = (*it).pos.Y;
					m[14] = (*it).pos.Z;
					m[15] = 1.0f;

					glMultMatrixf( m );

					(*it).model->Draw();
				glPopMatrix();
			}
		}
		it++;
	}
	renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}


/////////////////////////////////////////////////////
/// Method: DrawPhysicsObjects
/// Params: None
/// 
/////////////////////////////////////////////////////
void Level::DrawPhysicsObjects( )
{

}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void Level::Update( float deltaTime )
{
	int i=0, j=0, k=0;

	if( m_pPlayer == 0 )
		return;

	// update all sprite positions
	math::Vec3 vEye, vCenter;
	renderer::OpenGL::GetInstance()->GetLookAt( vEye, vCenter );
	GLfloat *mat = renderer::OpenGL::GetInstance()->GetModelViewMatrix();

	// get the right and up vectors
	math::Vec3 right;
	right.X = mat[0];
	right.Y = mat[4];
	right.Z = mat[8];

	math::Vec3 up;
	up.X = mat[1];
	up.Y = mat[5];
	up.Z = mat[9];

	if( m_pMeshData != 0 )
		m_pMeshData->Update( deltaTime );

	UpdateGates( deltaTime );

	collision::Sphere objSphere;
	collision::Sphere playerDraw = m_pPlayer->GetDrawDistanceSphere();

	// process all targets
	m_NumTargetsUsed = 0;
	for( i=0; i < MAX_TARGET_DRAWS; ++i )
	{
		m_TargetData[i].inUse = false;
		m_TargetData[i].unprojPoint = math::Vec3( -10000.0f, -10000.0f, -10000.0f );
		m_TargetData[i].distance = 999999.0f;

		if( m_TargetData[i].pObj != 0 )
		{
			math::Vec3 objPos = m_TargetData[i].pObj->GetPos();
			math::Vec3 playerPos = m_pPlayer->GetPosition();

			math::Vec3 dir = (playerPos - objPos).normalise();
			math::Vec3 playerDir = m_pPlayer->GetDirection();

			if( FacingCamera( playerDir, dir ) )
			{
				if( !m_TargetData[i].pObj->IsDestroyed() &&
					m_TargetData[i].pObj->IsTarget() )
				{
					m_TargetData[i].inUse = true;
					m_TargetData[i].pObj->CalculateUnprojectedPos();
					m_TargetData[i].unprojPoint = m_TargetData[i].pObj->GetUnprojection();

					m_NumTargetsUsed++;
				}
			}
		}
	}

	for( i=0; i < m_NumBatchesInUse; ++i )
	{
		if( renderer::OpenGL::GetInstance()->AABBInFrustum( m_ObjectBatchList[i].objBatchAABB ) ||
			playerDraw.SphereCollidesWithAABB( m_ObjectBatchList[i].objBatchAABB ) )
		{
			for( j=0; j < m_ObjectBatchList[i].numObjects; ++j )
			{
				objSphere = m_ObjectBatchList[i].objectList[j].GetSphere();
				if( renderer::OpenGL::GetInstance()->SphereInFrustum( objSphere.vCenterPoint.X, objSphere.vCenterPoint.Y, objSphere.vCenterPoint.Z, objSphere.fRadius ) )
				{
					m_ObjectBatchList[i].objectList[j].Update( deltaTime, vEye );

					/*if( GameSystems::GetInstance()->GetGameMode() == GameSystems::GAMEMODE_TARGET )
					{
						if( m_ObjectBatchList[i].objectList[j].GetResId() == m_TargetObjId &&
							!m_ObjectBatchList[i].objectList[j].IsDestroyed() &&
							m_ObjectBatchList[i].objectList[j].IsDrawn() )
						{
							math::Vec3 objPos = m_ObjectBatchList[i].objectList[j].GetPos();
							float distance = std::abs( (objPos - playerDraw.vCenterPoint).length() );

							// if list not full, just add
							if( m_NumTargetsUsed < MAX_TARGET_DRAWS )
							{
								m_TargetData[m_NumTargetsUsed].unprojPoint = m_ObjectBatchList[i].objectList[j].GetUnprojection();
								m_TargetData[m_NumTargetsUsed].distance = distance;

								m_NumTargetsUsed++;
							}
							else
							{
								// see if it can be inserted
								int slotSet = -1;
								float biggestDistance = 0.0f;

								// find the slot with the biggest distance, and try and swap
								for( k=0; k < MAX_TARGET_DRAWS; ++k )
								{
									if( m_TargetData[k].distance > biggestDistance )
									{
										biggestDistance = m_TargetData[k].distance;
										slotSet = k;
									}
								}

								// should have the biggest distance
								if( slotSet != -1 )
								{
									// we lower than this
									if( distance < m_TargetData[slotSet].distance )
									{
										m_TargetData[slotSet].unprojPoint = m_ObjectBatchList[i].objectList[j].GetUnprojection();
										m_TargetData[slotSet].distance = distance;
									}
								}
							}
						}
					}*/
				}
			}
		}
	}

	math::Vec3 spritePos;

	for( i=0, j=0; i < m_SpriteCount; i+=6, j++ )
	{
		SpriteType *s = m_SpriteData[j];
		DBG_ASSERT( s != 0 );
		
		spritePos = s->pos;
		if( !s->enabled )
			s->pos = math::Vec3( -100000.0f, -100000.0f, -100000.0f );

		float hw = s->w*0.5f;
		float hh = s->h*0.5f;
		float hd = s->d*0.5f;

		if( s->type == 0 )// full rotation
		{
			m_SpriteList[i].v	= math::Vec3( s->pos.X + (-right.X - up.X)*(hw), s->pos.Y + (-right.Y - up.Y)*(hh), s->pos.Z + (-right.Z - up.Z)*(hw) );
			m_SpriteList[i+1].v = math::Vec3( s->pos.X + (right.X - up.X)*(hw), s->pos.Y + (right.Y - up.Y)*(hh), s->pos.Z + (right.Z - up.Z)*(hw) );
			m_SpriteList[i+2].v = math::Vec3( s->pos.X + (up.X - right.X)*(hw), s->pos.Y + (up.Y - right.Y)*(hh), s->pos.Z + (up.Z - right.Z)*(hw) );

			m_SpriteList[i+3].v = math::Vec3( s->pos.X + (right.X - up.X)*(hw), s->pos.Y + (right.Y - up.Y)*(hh), s->pos.Z + (right.Z - up.Z)*(hw) );
			m_SpriteList[i+4].v = math::Vec3( s->pos.X + (right.X + up.X)*(hw), s->pos.Y + (right.Y + up.Y)*(hh), s->pos.Z + (right.Z + up.Z)*(hw) );
			m_SpriteList[i+5].v = math::Vec3( s->pos.X + (up.X - right.X)*(hw), s->pos.Y + (up.Y - right.Y)*(hh), s->pos.Z + (up.Z - right.Z)*(hw) );
		}
		else if( s->type == 1 )// Y rotation only
		{
			float sinVal;
			float cosVal;
			math::Vec3 vVector;

			float angle = math::AngleBetweenXZ( vEye, s->pos );
			math::sinCos( &sinVal, &cosVal, angle ); 

			m_SpriteList[i].v	= math::Vec3( s->pos.X + (-hw), s->pos.Y + (-hh), s->pos.Z + (-hd) );
			vVector = m_SpriteList[i].v - s->pos;
			m_SpriteList[i].v.X = (float)(s->pos.X + cosVal*vVector.X + sinVal*vVector.Z);
			m_SpriteList[i].v.Z = (float)(s->pos.Z - sinVal*vVector.X + cosVal*vVector.Z);

			m_SpriteList[i+1].v = math::Vec3( s->pos.X + (hw), s->pos.Y + (-hh), s->pos.Z + (-hd) );
			vVector = m_SpriteList[i+1].v - s->pos;
			m_SpriteList[i+1].v.X = (float)(s->pos.X + cosVal*vVector.X + sinVal*vVector.Z);
			m_SpriteList[i+1].v.Z = (float)(s->pos.Z - sinVal*vVector.X + cosVal*vVector.Z);

			m_SpriteList[i+2].v = math::Vec3( s->pos.X + (-hw), s->pos.Y + (hh), s->pos.Z + (hd) );
			vVector = m_SpriteList[i+2].v - s->pos;
			m_SpriteList[i+2].v.X = (float)(s->pos.X + cosVal*vVector.X + sinVal*vVector.Z);
			m_SpriteList[i+2].v.Z = (float)(s->pos.Z - sinVal*vVector.X + cosVal*vVector.Z);

			m_SpriteList[i+3].v = math::Vec3( s->pos.X + (hw), s->pos.Y + (-hh), s->pos.Z + (-hd) );
			vVector = m_SpriteList[i+3].v - s->pos;
			m_SpriteList[i+3].v.X = (float)(s->pos.X + cosVal*vVector.X + sinVal*vVector.Z);
			m_SpriteList[i+3].v.Z = (float)(s->pos.Z - sinVal*vVector.X + cosVal*vVector.Z);

			m_SpriteList[i+4].v = math::Vec3( s->pos.X + (hw), s->pos.Y + (hh), s->pos.Z + (hd) );
			vVector = m_SpriteList[i+4].v - s->pos;
			m_SpriteList[i+4].v.X = (float)(s->pos.X + cosVal*vVector.X + sinVal*vVector.Z);
			m_SpriteList[i+4].v.Z = (float)(s->pos.Z - sinVal*vVector.X + cosVal*vVector.Z);

			m_SpriteList[i+5].v = math::Vec3( s->pos.X + (-hw), s->pos.Y + (hh), s->pos.Z + (hd) );
			vVector = m_SpriteList[i+5].v - s->pos;
			m_SpriteList[i+5].v.X = (float)(s->pos.X + cosVal*vVector.X + sinVal*vVector.Z);
			m_SpriteList[i+5].v.Z = (float)(s->pos.Z - sinVal*vVector.X + cosVal*vVector.Z);
		}
		else if( s->type == 2 )// static
		{
			float sinVal;
			float cosVal;
			math::Vec3 vVector;
			math::sinCos( &sinVal, &cosVal, s->angle ); 

			m_SpriteList[i].v	= math::Vec3( s->pos.X + (-hw), s->pos.Y + (-hh), s->pos.Z + (-hd) );
			vVector = m_SpriteList[i].v - s->pos;
			m_SpriteList[i].v.X = (float)(s->pos.X + cosVal*vVector.X + sinVal*vVector.Z);
			m_SpriteList[i].v.Z = (float)(s->pos.Z - sinVal*vVector.X + cosVal*vVector.Z);

			m_SpriteList[i+1].v = math::Vec3( s->pos.X + (hw), s->pos.Y + (-hh), s->pos.Z + (-hd) );
			vVector = m_SpriteList[i+1].v - s->pos;
			m_SpriteList[i+1].v.X = (float)(s->pos.X + cosVal*vVector.X + sinVal*vVector.Z);
			m_SpriteList[i+1].v.Z = (float)(s->pos.Z - sinVal*vVector.X + cosVal*vVector.Z);

			m_SpriteList[i+2].v = math::Vec3( s->pos.X + (-hw), s->pos.Y + (hh), s->pos.Z + (hd) );
			vVector = m_SpriteList[i+2].v - s->pos;
			m_SpriteList[i+2].v.X = (float)(s->pos.X + cosVal*vVector.X + sinVal*vVector.Z);
			m_SpriteList[i+2].v.Z = (float)(s->pos.Z - sinVal*vVector.X + cosVal*vVector.Z);

			m_SpriteList[i+3].v = math::Vec3( s->pos.X + (hw), s->pos.Y + (-hh), s->pos.Z + (-hd) );
			vVector = m_SpriteList[i+3].v - s->pos;
			m_SpriteList[i+3].v.X = (float)(s->pos.X + cosVal*vVector.X + sinVal*vVector.Z);
			m_SpriteList[i+3].v.Z = (float)(s->pos.Z - sinVal*vVector.X + cosVal*vVector.Z);

			m_SpriteList[i+4].v = math::Vec3( s->pos.X + (hw), s->pos.Y + (hh), s->pos.Z + (hd) );
			vVector = m_SpriteList[i+4].v - s->pos;
			m_SpriteList[i+4].v.X = (float)(s->pos.X + cosVal*vVector.X + sinVal*vVector.Z);
			m_SpriteList[i+4].v.Z = (float)(s->pos.Z - sinVal*vVector.X + cosVal*vVector.Z);

			m_SpriteList[i+5].v = math::Vec3( s->pos.X + (-hw), s->pos.Y + (hh), s->pos.Z + (hd) );
			vVector = m_SpriteList[i+5].v - s->pos;
			m_SpriteList[i+5].v.X = (float)(s->pos.X + cosVal*vVector.X + sinVal*vVector.Z);
			m_SpriteList[i+5].v.Z = (float)(s->pos.Z - sinVal*vVector.X + cosVal*vVector.Z);
		}

		if( !s->enabled )
			s->pos = spritePos;
	}

	UpdatePhysicsObjects( deltaTime );

	UpdateAudio( deltaTime );

	m_BatchSprites.Update(deltaTime);

	// tokens
	m_TokenRotation += 180.0f*deltaTime; 

	collision::Sphere playerCollision = m_pPlayer->GetBoundingSphere();

	m_TokenWasCollected = false;

	char awardText[64];
	for( i=0; i < TOTAL_CADETS*TOTAL_TOKENS_PER_CADET; ++i )
	{
		if( m_CadetTokens[i].tokenId != -1 )
		{
			if( !m_CadetTokens[i].collected &&
				playerCollision.SphereCollidesWithSphere( m_CadetTokens[i].s ) )
			{
				m_CadetTokens[i].collected = true;
				m_TokenWasCollected = true;

				AudioSystem::GetInstance()->PlayAudio( m_TokenCollect, zeroVec, AL_TRUE, AL_FALSE, 1.0f, 2.0f );

				m_AllCadetTokensCollectedCount++;

				if( m_CadetTokens[i].tokenId == 2000 )
				{
					m_TakuCollectedCount++;

					snprintf( awardText, 64, "%s %d / %d", res::GetScriptString(m_CadetTokens[i].tokenId), m_TakuCollectedCount, m_TotalTakuTokens );
					GameSystems::GetInstance()->AwardAchievement(-1, awardText );
				}
				else if( m_CadetTokens[i].tokenId == 2001 )
				{
					m_MeiCollectedCount++;

					snprintf( awardText, 64, "%s %d / %d", res::GetScriptString(m_CadetTokens[i].tokenId), m_MeiCollectedCount, m_TotalMeiTokens );
					GameSystems::GetInstance()->AwardAchievement(-1, awardText );
				}
				else if( m_CadetTokens[i].tokenId == 2002 )
				{
					m_EarlCollectedCount++;

					snprintf( awardText, 64, "%s %d / %d", res::GetScriptString(m_CadetTokens[i].tokenId), m_EarlCollectedCount, m_TotalEarlTokens );
					GameSystems::GetInstance()->AwardAchievement(-1, awardText );
				}
				else if( m_CadetTokens[i].tokenId == 2003 )
				{
					m_FioCollectedCount++;

					snprintf( awardText, 64, "%s %d / %d", res::GetScriptString(m_CadetTokens[i].tokenId), m_FioCollectedCount, m_TotalFioTokens );
					GameSystems::GetInstance()->AwardAchievement(-1, awardText );
				}
				else if( m_CadetTokens[i].tokenId == 2004 )
				{
					m_MitoCollectedCount++;

					snprintf( awardText, 64, "%s %d / %d", res::GetScriptString(m_CadetTokens[i].tokenId), m_MitoCollectedCount, m_TotalMitoTokens );
					GameSystems::GetInstance()->AwardAchievement(-1, awardText );
				}
				else if( m_CadetTokens[i].tokenId == 2005 )
				{
					m_UkiCollectedCount++;

					snprintf( awardText, 64, "%s %d / %d", res::GetScriptString(m_CadetTokens[i].tokenId), m_UkiCollectedCount, m_TotalUkiTokens );
					GameSystems::GetInstance()->AwardAchievement(-1, awardText );
				}
				else
					DBG_ASSERT(0);

				if( m_AllCadetTokensCollectedCount >= m_TotalCadetTokens )
				{
					m_AllCadetsCollected = true;

					int levelIndex = (ProfileManager::GetInstance()->GetModePackId() * m_GameData.MAX_ITEMS_PER_PACK) + ProfileManager::GetInstance()->GetModeLevelId();
					GameSystems::GetInstance()->AwardAchievement( 12+levelIndex, -1 );

					// check for all levels
					bool hasAllTokensOnAllLevels = true;
					for( j=0; j < m_GameData.TOTAL_LEVELS; ++j )
					{
						if( !GameSystems::GetInstance()->HasAchievement(12+j) )
							hasAllTokensOnAllLevels = false;
					}

					if( hasAllTokensOnAllLevels )
					{
						GameSystems::GetInstance()->AwardAchievement( 44, -1 );
					}
				}
			}
		}
	}

	for( i=0; i < STAR_TOKENS; ++i )
	{
		if( m_StarTokens[i].tokenId != -1 )
		{
			if( !m_StarTokens[i].collected &&
				playerCollision.SphereCollidesWithSphere( m_StarTokens[i].s ) )
			{
				m_StarTokens[i].collected = true;
				m_StarCollectedCount++;

				snprintf( awardText, 64, "%s %d / %d", res::GetScriptString(m_StarTokens[i].tokenId), m_StarCollectedCount, STAR_TOKENS );
				GameSystems::GetInstance()->AwardAchievement(-1, awardText );

				AudioSystem::GetInstance()->PlayAudio( m_StarCollect, zeroVec, AL_TRUE, AL_FALSE, 1.0f, 2.0f );

				if( m_StarCollectedCount >= STAR_TOKENS )
				{
					int levelIndex = (ProfileManager::GetInstance()->GetModePackId() * m_GameData.MAX_ITEMS_PER_PACK) + ProfileManager::GetInstance()->GetModeLevelId();
					GameSystems::GetInstance()->AwardAchievement( 28+levelIndex, -1 );


					// check for all levels
					bool hasAllStarsOnAllLevels = true;
					for( j=0; j < m_GameData.TOTAL_LEVELS; ++j )
					{
						if( !GameSystems::GetInstance()->HasAchievement(28+j) )
							hasAllStarsOnAllLevels = false;
					}

					if( hasAllStarsOnAllLevels )
					{
						GameSystems::GetInstance()->AwardAchievement( 45, -1 );
					}
				}
			}
		}
	}

	for( i=0; i < HIDDEN_TOKENS; ++i )
	{
		if( m_HiddenTokens[i].tokenId != -1 )
		{
			if( !m_HiddenTokens[i].collected &&
				playerCollision.SphereCollidesWithSphere( m_HiddenTokens[i].s ) )
			{
				m_HiddenTokens[i].collected = true;
				m_HiddenTokenCollectedCount++;

				snprintf( awardText, 64, "%s %d / %d", res::GetScriptString(m_HiddenTokens[i].tokenId), m_HiddenTokenCollectedCount, HIDDEN_TOKENS );
				GameSystems::GetInstance()->AwardAchievement(-1, awardText );

				AudioSystem::GetInstance()->PlayAudio( m_HiddenIconCollect, zeroVec, AL_TRUE, AL_FALSE, 1.0f, 2.0f );

				if( m_HiddenTokenCollectedCount >= HIDDEN_TOKENS )
				{
					int levelIndex = (ProfileManager::GetInstance()->GetModePackId() * m_GameData.MAX_ITEMS_PER_PACK) + ProfileManager::GetInstance()->GetModeLevelId();
					GameSystems::GetInstance()->AwardAchievement( 36+levelIndex, -1 );

					// check for all levels
					bool hasAllHiddenTokensOnAllLevels = true;
					for( j=0; j < m_GameData.TOTAL_LEVELS; ++j )
					{
						if( !GameSystems::GetInstance()->HasAchievement(36+j) )
							hasAllHiddenTokensOnAllLevels = false;
					}

					if( hasAllHiddenTokensOnAllLevels )
					{
						GameSystems::GetInstance()->AwardAchievement( 46, -1 );
					}
				}
			}
		}
	}
}

/////////////////////////////////////////////////////
/// Method: CreatePhysics
/// Params: None
///
/////////////////////////////////////////////////////
void Level::CreatePhysics()
{
	unsigned int i = 0;

	DBG_ASSERT( m_PhysicsData != 0 );

	dSpaceID physicsSpaceId = 0;
	physicsSpaceId = physics::PhysicsWorldODE::GetSpace();

	m_EntitySpace = physicsSpaceId;

	m_AABB.Reset();

	// Box Geoms
	for( i=0; i < m_PhysicsData->totalPhysicsBox; ++i )
	{
		dGeomID geom = 0;
		geom = CreateGeom( dBoxClass, m_EntitySpace, m_PhysicsData->boxList[i].dims );
		dGeomSetCategoryBits( geom, m_CategoryBits );
		dGeomSetCollideBits( geom, m_CollideBits );
		dGeomSetData(geom, this);
		DBG_ASSERT_MSG( (geom != 0), "Could not create ode box" );
	}

	// Sphere Geoms
	for( i=0; i < m_PhysicsData->totalPhysicsSphere; ++i )
	{
		dGeomID geom = 0;
		geom = CreateGeom( dSphereClass, m_EntitySpace, zeroVec, m_PhysicsData->sphereList[i].radius );
		dGeomSetCategoryBits( geom, m_CategoryBits );
		dGeomSetCollideBits( geom, m_CollideBits );
		dGeomSetData(geom, this);
		DBG_ASSERT_MSG( (geom != 0), "Could not create ode sphere" );
	}

	// Capsule Geoms
	for( i=0; i < m_PhysicsData->totalPhysicsCapsule; ++i )
	{
		dGeomID geom = 0;
		geom = CreateGeom( dCapsuleClass, m_EntitySpace, zeroVec, m_PhysicsData->capsuleList[i].radius, m_PhysicsData->capsuleList[i].length );
		dGeomSetCategoryBits( geom, m_CategoryBits );
		dGeomSetCollideBits( geom, m_CollideBits );
		dGeomSetData(geom, this);
		DBG_ASSERT_MSG( (geom != 0), "Could not create ode capsule" );
	}

	// Cylinder Geoms
	for( i=0; i < m_PhysicsData->totalPhysicsCylinder; ++i )
	{
		dGeomID geom = 0;
		geom = CreateGeom( dCylinderClass, m_EntitySpace, zeroVec, m_PhysicsData->cylinderList[i].radius, m_PhysicsData->cylinderList[i].length );
		dGeomSetData(geom, this);
		DBG_ASSERT_MSG( (geom != 0), "Could not create ode cylinder" );
	}

	// triMesh Geoms
	for( i=0; i < m_PhysicsData->totalPhysicsTriMesh; ++i )
	{
		dGeomID geom = 0;
		m_TriMeshData[i].triMeshDataID = dGeomTriMeshDataCreate();
		geom = CreateGeom( dTriMeshClass, m_EntitySpace, zeroVec, 0.0f, 0.0f, m_TriMeshData[i].triMeshDataID );
		m_TriMeshData[i].geomId = geom;

		dGeomSetCategoryBits( geom, m_CategoryBits );
		dGeomSetCollideBits( geom, m_CollideBits );

		m_TriMeshData[i].SetBaseId(PHYSICSBASICID_WORLD);
		m_TriMeshData[i].SetCastingId(PHYSICSCASTID_TERRAIN);

		dGeomSetData(geom, &m_TriMeshData[i]);
		DBG_ASSERT_MSG( (geom != 0), "Could not create tri-mesh" );

		//dGeomTriMeshDataBuildSingle( m_TriMeshData[i].triMeshDataID, m_TriMeshData[i].triangleData, sizeof(math::Vec3), m_TriMeshData[i].totalMeshTriangles*3, m_TriMeshData[i].indices, m_TriMeshData[i].totalMeshTriangles*3, sizeof(int) );
		dGeomTriMeshDataBuildSingle1( m_TriMeshData[i].triMeshDataID, m_TriMeshData[i].triangleData, sizeof(math::Vec3), m_TriMeshData[i].totalMeshTriangles*3, m_TriMeshData[i].indices, m_TriMeshData[i].totalMeshTriangles*3, sizeof(int), m_TriMeshData[i].normals );

		dReal aabb[6];
		dGeomGetAABB( m_TriMeshData[i].geomId, aabb );

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
		m_TriMeshData[i].meshAABB = AABB;

		dGeomTriMeshDataPreprocess( m_TriMeshData[i].triMeshDataID );


	}
}

/////////////////////////////////////////////////////
/// Method: UpdatePhysicsObjects
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void Level::UpdatePhysicsObjects( float deltaTime )
{
	unsigned int i=0;

	for( i=0; i < m_EmitterList.size(); ++i )
		m_EmitterList[i]->Update( deltaTime );
}

/////////////////////////////////////////////////////
/// Method: ResetTokens
/// Params: None
///
/////////////////////////////////////////////////////
void Level::ResetTokens()
{
	int i=0;

	for( i=0; i < TOTAL_CADETS*TOTAL_TOKENS_PER_CADET; ++i )
	{
		if( m_CadetTokens[i].tokenId != -1 )
		{
			m_CadetTokens[i].collected = false;
		}
	}

	for( i=0; i < STAR_TOKENS; ++i )
	{
		if( m_StarTokens[i].tokenId != -1 )
		{
			m_StarTokens[i].collected = false;
		}
	}

	for( i=0; i < HIDDEN_TOKENS; ++i )
	{
		if( m_HiddenTokens[i].tokenId != -1 )
		{
			m_HiddenTokens[i].collected = false;
		}
	}

	m_TokenWasCollected = false;
	m_AllCadetsCollected = false;

	m_AllCadetTokensCollectedCount = 0;
	m_TakuCollectedCount = 0;
	m_MeiCollectedCount = 0;
	m_EarlCollectedCount = 0;
	m_FioCollectedCount = 0;
	m_MitoCollectedCount = 0;
	m_UkiCollectedCount = 0;

	m_StarCollectedCount = 0;
	m_HiddenTokenCollectedCount = 0;
}

/////////////////////////////////////////////////////
/// Method: ResetGateGroup
/// Params: None
///
/////////////////////////////////////////////////////
void Level::ResetGateGroup( bool clearAll )
{
	int i=0;

	if( m_ActiveGateGroup != -1 )
	{
		if( m_ActiveGateGroup < static_cast<int>( m_GateGroups.size() ) )
		{
			GateGroup* pGroup = &m_GateGroups[m_ActiveGateGroup];

			pGroup->nextGateIndex = 0;

			int matchNum = pGroup->nextGateIndex;
			int matchNum1 = -100;//pGroup->nextGateIndex+1;

			for( i=0; i<pGroup->numGates; ++i ) 
			{
				// enable all gates with correct index
				if( clearAll )
				{
					pGroup->enabled = false;
					pGroup->gateList[i]->enabled = false;
				}
				else
				{
					pGroup->enabled = true;

					if( pGroup->gateList[i]->number == matchNum ||
						pGroup->gateList[i]->number == matchNum1 )
					{
						pGroup->gateList[i]->enabled = true;
					}
					else
						pGroup->gateList[i]->enabled = false;
				}
			}
		}
	}

	if( clearAll )
		m_ActiveGateGroup = -1;
}

/////////////////////////////////////////////////////
/// Method: ResetTargets
/// Params: None
///
/////////////////////////////////////////////////////
void Level::ResetTargets()
{
	if( GameSystems::GetInstance()->GetGameMode() != GameSystems::GAMEMODE_TARGET )
		return;

	int i=0, j=0;
	for( i=0; i < m_NumBatchesInUse; ++i )
	{
		for( j=0; j < m_ObjectBatchList[i].numObjects; ++j )
		{
			if( m_ObjectBatchList[i].objectList[j].IsTarget() )
				m_ObjectBatchList[i].objectList[j].Reset();
		}
	}
}

/////////////////////////////////////////////////////
/// Method: ResetAudio
/// Params: None
///
/////////////////////////////////////////////////////
void Level::ResetAudio()
{
	std::vector< AudioLocator *>::iterator it = m_SoundAreas.begin();

	while( it != m_SoundAreas.end() )
	{
		if( (*it)->sourceId != snd::INVALID_SOUNDSOURCE )
		{
			CHECK_OPENAL_ERROR( alSourceStop( (*it)->sourceId ) )
		}
		(*it)->sourceId = snd::INVALID_SOUNDSOURCE;
		it++;
	}
}

/////////////////////////////////////////////////////
/// Method: GetRandomSpawn
/// Params: None
///
/////////////////////////////////////////////////////
void Level::GetRandomSpawn( math::Vec3* pos, math::Vec3* rot )
{
	DBG_ASSERT( pos != 0 );
	DBG_ASSERT( rot != 0 );
	
	int idx = math::RandomNumber(0, (int)m_SpawnPoints.size() - 1);

	*pos = m_SpawnPoints[idx].pos;
	*rot = m_SpawnPoints[idx].rot;
}

/////////////////////////////////////////////////////
/// Method: GetNearestSpawn
/// Params: None
///
/////////////////////////////////////////////////////
void Level::GetNearestSpawn( const math::Vec3& currentPos, math::Vec3* pos, math::Vec3* rot )
{
	DBG_ASSERT( pos != 0 );
	DBG_ASSERT( rot != 0 );

	unsigned int i=0;

	int nearestIndex = -1;
	float nearestDistance = 999999.0f;

	for( i=0; i < m_SpawnPoints.size(); ++i )
	{
		math::Vec3 distVec = currentPos - m_SpawnPoints[i].pos;
		float distance = std::abs( distVec.length() );

		if( distance < nearestDistance )
		{
			nearestDistance = distance;
			nearestIndex = i;
		}
	}

	DBG_ASSERT( nearestIndex != -1 );

	if( nearestIndex != -1 )
	{
		*pos = m_SpawnPoints[nearestIndex].pos;
		*rot = m_SpawnPoints[nearestIndex].rot;
	}
}

/////////////////////////////////////////////////////
/// Method: CreateGeom
/// Params: [in]shapeClass, [in]spaceId, [in]dims, [in]radius, [in]length, [in]triMeshDataId
///
/////////////////////////////////////////////////////
dGeomID Level::CreateGeom( int shapeClass, dSpaceID spaceId, const math::Vec3& dims, float radius, float length, dTriMeshDataID triMeshDataId )
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
/// Method: UpdateAudio
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void Level::UpdateAudio( float deltaTime )
{

	if( m_pPlayer == 0 )
		return;

	collision::Sphere s1 = m_pPlayer->GetBoundingSphere();
	
	float sfxState = 1.0f;
	if( !AudioSystem::GetInstance()->GetSFXState() )
		sfxState = 0.0f;

	// process sounds
	float GAIN_SMOOTH_IN = 0.3f*deltaTime;
	float GAIN_SMOOTH_OUT = 0.5f*deltaTime;

	std::vector< AudioLocator *>::iterator it = m_SoundAreas.begin();

	while( it != m_SoundAreas.end() )
	{
		// always update the sound timer
		if( (*it)->type == 1 )
		{
			(*it)->lastTimeUpdate += deltaTime;
		}

		bool playerInAudioArea = false;
		if( (*it)->triggerType == eAudioTrigger_Box )
		{
			//if( (*it)->angle == 0.0f )
			//{
				if( s1.SphereCollidesWithAABB( (*it)->aabb ) )
					playerInAudioArea = true;
			//}
			//else
			//{
			//	if( (*it)->obb.OBBCollidesWithSphere( player ) )
			//		playerInAudioArea = true;
			//}
		}
		else if( (*it)->triggerType == eAudioTrigger_Sphere )
		{
			if( s1.SphereCollidesWithSphere( (*it)->sphere ) )
				playerInAudioArea = true;
		}

		if( playerInAudioArea )
		{
			/*float finalGain = 1.0f;

			// custom attenuation
			if( (*it)->gainAtt )
			{
				if( (*it)->areaType == 0 )
				{
					math::Vec3 distVec = (*it)->pos - player.vCenterPoint;

					// FIXME: sort out some gain system for this
					float largestLength = 1.0f;
					//if( largestLength < std::fabs( (*it)->aabb.fHeight ) )
					//	largestLength = std::fabs( (*it)->aabb.fHeight );
					
					float distance = std::fabs( distVec.length());
					finalGain = 1.0f-(distance/largestLength);
					math::Clamp( &finalGain, 0.0f, 2.0f );
				}
				else if( (*it)->areaType == 1 )
				{
					math::Vec3 distVec = (*it)->sphere.vCenterPoint - player.vCenterPoint;

					float distance = std::fabs( distVec.length());
					finalGain = 1.0f-(distance/(*it)->sphere.fRadius);
					math::Clamp( &finalGain, 0.0f, 2.0f );
				}
			}*/

			// normal update
			if( (*it)->type == 0 )
			{
				if( (*it)->sourceId == snd::INVALID_SOUNDSOURCE )
				{
					// sound is not playing/setup
					if( SetAudioSourceData( (*it) ) )
					{
						// play
						CHECK_OPENAL_ERROR( alSource3f( (*it)->sourceId, AL_POSITION, (*it)->offset.X, (*it)->offset.Y, (*it)->offset.Z ) )

						(*it)->gainControl = 0.0f;
						CHECK_OPENAL_ERROR( alSourcef( (*it)->sourceId, AL_GAIN, (*it)->gainControl*sfxState ) )

						CHECK_OPENAL_ERROR( alSourcePlay( (*it)->sourceId ) )

						if( !(*it)->loop )
							(*it)->sourceId = snd::INVALID_SOUNDSOURCE;
					}
				}
				else
				{
					// sound is playing, make it smooth in
					if( (*it)->gainControl != (*it)->gain )
					{
						if( (*it)->gainControl < (*it)->gain )
						{
							(*it)->gainControl += GAIN_SMOOTH_IN;
						}

						if( std::fabs( (*it)->gainControl - (*it)->gain ) < GAIN_SMOOTH_IN )
							(*it)->gainControl = (*it)->gain;

						CHECK_OPENAL_ERROR( alSourcef( (*it)->sourceId, AL_GAIN, (*it)->gainControl*sfxState ) )
					}
				}
			}
			else if( (*it)->type == 1 )
			{
				// timed
				if( (*it)->lastTimeUpdate >= (*it)->time )
				{
					if( (*it)->sourceId == snd::INVALID_SOUNDSOURCE )
					{
						// create another sound
						if( SetAudioSourceData( (*it) ) )
						{
							// play
							CHECK_OPENAL_ERROR( alSourcef( (*it)->sourceId, AL_GAIN, (*it)->gain*sfxState ) )

							CHECK_OPENAL_ERROR( alSource3f( (*it)->sourceId, AL_POSITION, (*it)->offset.X, (*it)->offset.Y, (*it)->offset.Z ) )

							CHECK_OPENAL_ERROR( alSourcePlay( (*it)->sourceId ) )

							if( !(*it)->loop )
								(*it)->sourceId = snd::INVALID_SOUNDSOURCE;
						}
					}

					(*it)->lastTimeUpdate = 0.0f;
				}
			}
		}
		else
		{
			// reset the timer even if not in the area
			if( (*it)->type == 1 )
			{
				if( (*it)->lastTimeUpdate >= (*it)->time )
					(*it)->lastTimeUpdate = 0.0f;
			}

			// if the sound is playing and it's looped, stop the loop
			if( (*it)->sourceId != snd::INVALID_SOUNDSOURCE )
			{
				if( snd::SoundManager::GetInstance()->GetSoundState( (*it)->sourceId ) == AL_PLAYING )
				{
					// sound is playing, make it smooth out
					if( (*it)->gainControl > 0.0f )
					{
						if( (*it)->loop )
							(*it)->gainControl -= GAIN_SMOOTH_OUT*2.0f;
						else
							(*it)->gainControl -= GAIN_SMOOTH_OUT;

						if( (*it)->gainControl <= 0.0f )
							(*it)->gainControl = 0.0f;

						CHECK_OPENAL_ERROR( alSourcef( (*it)->sourceId, AL_GAIN, (*it)->gainControl*sfxState ) )

						if( (*it)->gainControl <= 0.0f )
						{
							CHECK_OPENAL_ERROR( alSourceStop( (*it)->sourceId ) )

							(*it)->sourceId = snd::INVALID_SOUNDSOURCE;
						}
					}
				}
				else
					(*it)->sourceId = snd::INVALID_SOUNDSOURCE;
			}
		}

		// next 
		it++;
	}
}

/////////////////////////////////////////////////////
/// Method: SetAudioSourceData
/// Params: [in]pAudioLocator
///
/////////////////////////////////////////////////////
bool Level::SetAudioSourceData( AudioLocator* pAudioLocator )
{
	DBG_ASSERT( pAudioLocator !=0 );

	pAudioLocator->sourceId = snd::SoundManager::GetInstance()->GetFreeSoundSource();

	// no free sources
	if( pAudioLocator->sourceId == snd::INVALID_SOUNDSOURCE )
		return(false);

	//math::Vec3 pos3d( pAudioLocator->pos );
	//AudioSystem::GetInstance()->PlayAudio( pAudioLocator->sourceId, pAudioLocator->bufferId, pos3d, false, pAudioLocator->loop, pAudioLocator->pitch, pAudioLocator->gain );
	
	if( pAudioLocator->loop )
	{
		CHECK_OPENAL_ERROR( alSourcei( pAudioLocator->sourceId, AL_LOOPING, AL_TRUE ) )
	}
	else
	{
		CHECK_OPENAL_ERROR( alSourcei( pAudioLocator->sourceId, AL_LOOPING, AL_FALSE ) )
	}

	// setup the sound
	CHECK_OPENAL_ERROR( alSourcei( pAudioLocator->sourceId, AL_BUFFER, pAudioLocator->bufferId ) )
	CHECK_OPENAL_ERROR( alSourcei( pAudioLocator->sourceId, AL_SOURCE_RELATIVE, pAudioLocator->relative ) )
	CHECK_OPENAL_ERROR( alSourcef( pAudioLocator->sourceId, AL_PITCH, pAudioLocator->pitch ) )
	
	CHECK_OPENAL_ERROR( alSourcef( pAudioLocator->sourceId, AL_MAX_DISTANCE, MAX_AUDIO_DISTANCE ) )
	CHECK_OPENAL_ERROR( alSourcef( pAudioLocator->sourceId, AL_ROLLOFF_FACTOR, ROLL_OFF ) )
	CHECK_OPENAL_ERROR( alSourcef( pAudioLocator->sourceId, AL_REFERENCE_DISTANCE, REF_DISTANCE ) )
	
	//CHECK_OPENAL_ERROR( alSourcef( pAudioLocator->sourceId, AL_GAIN, pAudioLocator->gain ) )

	return(true);
}

/////////////////////////////////////////////////////
/// Method: DrawDebug
/// Params: 
///
/////////////////////////////////////////////////////
void Level::DrawDebug()
{
	unsigned int i=0;
	int j=0;

	bool lightState = renderer::OpenGL::GetInstance()->GetLightingState();
	bool textureState = renderer::OpenGL::GetInstance()->GetTextureState();

	if( lightState )
		renderer::OpenGL::GetInstance()->DisableLighting();
	if( textureState )
		renderer::OpenGL::GetInstance()->DisableTexturing();

	renderer::OpenGL::GetInstance()->SetColour4ub( 255,255,255,255 );
	for( i=0; i < m_SoundAreas.size(); ++i )
	{
		if( m_SoundAreas[i]->triggerType == eAudioTrigger_Sphere )
		{
			const float k_segments = 16.0f;
			const float k_increment = 2.0f * math::PI / k_segments;
			float theta = 0.0f;
			renderer::OpenGL::GetInstance()->SetColour4ub( 255,255,255,255 );

			math::Vec2 vaPoints[16];	
			for (int j = 0; j < static_cast<int>(k_segments); ++j)
			{
				math::Vec2 soundPos( m_SoundAreas[i]->pos.X, m_SoundAreas[i]->pos.Y );
				math::Vec2 v = soundPos + (math::Vec2(std::cos(theta), std::sin(theta)) * m_SoundAreas[i]->radius );
				vaPoints[j] = v;
				theta += k_increment;
			}
			
			glVertexPointer(2, GL_FLOAT, sizeof(math::Vec2), vaPoints);
			glDrawArrays(GL_LINE_LOOP, 0, 16 );
		}
		else if( m_SoundAreas[i]->triggerType == eAudioTrigger_Box )
		{
			renderer::OpenGL::GetInstance()->SetColour4ub( 255,255,255,255 );
			renderer::DrawAABB( m_SoundAreas[i]->aabb.vBoxMin, m_SoundAreas[i]->aabb.vBoxMax );
		}
	}

	collision::Sphere objSphere;
	collision::Sphere playerDraw = m_pPlayer->GetDrawDistanceSphere();

	for( i=0; i < static_cast<unsigned int>(m_NumBatchesInUse); ++i )
	{
		if( renderer::OpenGL::GetInstance()->AABBInFrustum( m_ObjectBatchList[i].objBatchAABB ) ||
			playerDraw.SphereCollidesWithAABB( m_ObjectBatchList[i].objBatchAABB ) )
		{
			for( j=0; j < m_ObjectBatchList[i].numObjects; ++j )
			{
				objSphere = m_ObjectBatchList[i].objectList[j].GetSphere();
				if( renderer::OpenGL::GetInstance()->SphereInFrustum( objSphere.vCenterPoint.X, objSphere.vCenterPoint.Y, objSphere.vCenterPoint.Z, objSphere.fRadius ) &&
					m_ObjectBatchList[i].objectList[j].IsDrawn() )
				{
					m_ObjectBatchList[i].objectList[j].DrawPhysicsData();
				}
			}
		}
	}

	for( i=0; i < m_GateGroups.size(); ++i )
	{
		if( m_GateGroups[i].enabled )
		{
			for( j=0; j < m_GateGroups[i].numGates; ++j )
			{
				if( m_GateGroups[i].gateList[j]->collisionType == 0 )
				{
					renderer::OpenGL::GetInstance()->SetColour4ub( 255,255,255,255 );
					glPushMatrix();
						glTranslatef( m_GateGroups[i].gateList[j]->s.vCenterPoint.X, m_GateGroups[i].gateList[j]->s.vCenterPoint.Y, m_GateGroups[i].gateList[j]->s.vCenterPoint.Z );
						renderer::DrawSphere( m_GateGroups[i].gateList[j]->s.fRadius );
					glPopMatrix();
				}
				else
				{
					renderer::OpenGL::GetInstance()->SetColour4ub( 255,255,255,255 );
					glPushMatrix();
						renderer::DrawOBB( m_GateGroups[i].gateList[j]->obb.vCenter, m_GateGroups[i].gateList[j]->obb.vAxis, m_GateGroups[i].gateList[j]->obb.vHalfWidths );
					glPopMatrix();
				}
			}
		}
	}

	for( i=0; i < static_cast<int>(m_Sights.size()); ++i )
	{
		if( m_Sights[i].enabled )
		{
				if( m_Sights[i].collisionType == 0 )
				{
					renderer::OpenGL::GetInstance()->SetColour4ub( 255,255,255,255 );
					glPushMatrix();
						glTranslatef( m_Sights[i].s.vCenterPoint.X, m_Sights[i].s.vCenterPoint.Y, m_Sights[i].s.vCenterPoint.Z );
						renderer::DrawSphere( m_Sights[i].s.fRadius );
					glPopMatrix();
				}
				else
				{
					renderer::OpenGL::GetInstance()->SetColour4ub( 255,255,255,255 );
					glPushMatrix();
						renderer::DrawOBB( m_Sights[i].obb.vCenter, m_Sights[i].obb.vAxis, m_Sights[i].obb.vHalfWidths );
					glPopMatrix();
				}
		}
	}

	for( i=0; i < m_EmitterList.size(); ++i )
		m_EmitterList[i]->DrawDebug();

	for( i=0; i < static_cast<unsigned int>(m_TotalTriMeshes); ++i )
	{
		renderer::OpenGL::GetInstance()->SetColour4ub( 0,0,255,255 );
		glPushMatrix();
			renderer::DrawAABB( m_TriMeshData[i].meshAABB.vBoxMin, m_TriMeshData[i].meshAABB.vBoxMax );
		glPopMatrix();
	}

	/*collision::AABB boundingArea;
	boundingArea.vBoxMin = math::Vec3( m_GameData.MIN_BOUNDING_X, -8000.0f, m_GameData.MIN_BOUNDING_Z );
	boundingArea.vBoxMax = math::Vec3( m_GameData.MAX_BOUNDING_X, 8000.0f, m_GameData.MAX_BOUNDING_Z );

	renderer::OpenGL::GetInstance()->SetColour4ub( 255, 0, 0, 64 );
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	renderer::DrawAABB( boundingArea.vBoxMin, boundingArea.vBoxMax, true );
	renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );*/

	if( lightState )
		renderer::OpenGL::GetInstance()->EnableLighting();
	if( textureState )
		renderer::OpenGL::GetInstance()->EnableTexturing();
}

/////////////////////////////////////////////////////
/// Method: ParsePosition
/// Params: 
///
/////////////////////////////////////////////////////
math::Vec3 Level::ParsePosition( int paramIndex )
{
	math::Vec3 pos;
	pos.setZero();

	pos.X	= static_cast<float>( script::LuaGetNumberFromTableItem( "posX", paramIndex, 0.0 ) );
	pos.Y	= static_cast<float>( script::LuaGetNumberFromTableItem( "posY", paramIndex, 0.0 ) );
	pos.Z	= static_cast<float>( script::LuaGetNumberFromTableItem( "posZ", paramIndex, 0.0 ) );

	return pos;
}
		
/////////////////////////////////////////////////////
/// Method: ParsePosRot
/// Params: 
///
/////////////////////////////////////////////////////
void Level::ParsePosRot( int paramIndex, int baseType, int baseId )
{
	math::Vec3 pos;
	math::Vec3 rot;

	// all types have position
	pos = ParsePosition( paramIndex );

	rot.X = static_cast<float>( script::LuaGetNumberFromTableItem( "rotX", paramIndex, 0.0 ) );
	rot.Y = static_cast<float>( script::LuaGetNumberFromTableItem( "rotY", paramIndex, 0.0 ) );
	rot.Z = static_cast<float>( script::LuaGetNumberFromTableItem( "rotZ", paramIndex, 0.0 ) );

	if( baseId == 0 )
	{
		//int playerSpawnDirection = Player::PLAYERSPAWN_LEFT;
		//if( rot.Y != 0.0f )
		//	playerSpawnDirection = Player::PLAYERSPAWN_RIGHT;

		m_PlayerStartPoint = math::Vec2( pos.X, pos.Y );
		//m_PlayerSpawnDir = playerSpawnDirection;
	}
	else if( baseId == 2000 )
	{
		if( m_TotalTakuTokens < TOTAL_TOKENS_PER_CADET )
		{
			int dataOffset = (0 * TOTAL_TOKENS_PER_CADET) + m_TotalTakuTokens; 

			m_CadetTokens[dataOffset].tokenId = baseId;
			m_CadetTokens[dataOffset].collected = false;
			m_CadetTokens[dataOffset].s.vCenterPoint = pos;
			m_CadetTokens[dataOffset].pModel = res::LoadModel(410);
			DBG_ASSERT( m_CadetTokens[dataOffset].pModel != 0 );

			m_TotalTakuTokens++;

			m_TotalCadetTokens++;
		}
		else
			DBG_ASSERT(0); // too many tokens?
	}
	else if( baseId == 2001 )
	{
		if( m_TotalMeiTokens < TOTAL_TOKENS_PER_CADET )
		{
			int dataOffset = (1 * TOTAL_TOKENS_PER_CADET) + m_TotalMeiTokens; 

			m_CadetTokens[dataOffset].tokenId = baseId;
			m_CadetTokens[dataOffset].collected = false;
			m_CadetTokens[dataOffset].s.vCenterPoint = pos;
			m_CadetTokens[dataOffset].pModel = res::LoadModel(411);
			DBG_ASSERT( m_CadetTokens[dataOffset].pModel != 0 );

			m_TotalMeiTokens++;

			m_TotalCadetTokens++;
		}
		else
			DBG_ASSERT(0); // too many tokens?
	}
	else if( baseId == 2002 )
	{
		if( m_TotalEarlTokens < TOTAL_TOKENS_PER_CADET )
		{
			int dataOffset = (2 * TOTAL_TOKENS_PER_CADET) + m_TotalEarlTokens; 

			m_CadetTokens[dataOffset].tokenId = baseId;
			m_CadetTokens[dataOffset].collected = false;
			m_CadetTokens[dataOffset].s.vCenterPoint = pos;
			m_CadetTokens[dataOffset].pModel = res::LoadModel(412);
			DBG_ASSERT( m_CadetTokens[dataOffset].pModel != 0 );

			m_TotalEarlTokens++;

			m_TotalCadetTokens++;
		}
		else
			DBG_ASSERT(0); // too many tokens?
	}
	else if( baseId == 2003 )
	{
		if( m_TotalFioTokens < TOTAL_TOKENS_PER_CADET )
		{
			int dataOffset = (3 * TOTAL_TOKENS_PER_CADET) + m_TotalFioTokens; 

			m_CadetTokens[dataOffset].tokenId = baseId;
			m_CadetTokens[dataOffset].collected = false;
			m_CadetTokens[dataOffset].s.vCenterPoint = pos;
			m_CadetTokens[dataOffset].pModel = res::LoadModel(413);
			DBG_ASSERT( m_CadetTokens[dataOffset].pModel != 0 );

			m_TotalFioTokens++;

			m_TotalCadetTokens++;
		}
		else
			DBG_ASSERT(0); // too many tokens?
	}
	else if( baseId == 2004 )
	{
		if( m_TotalMitoTokens < TOTAL_TOKENS_PER_CADET )
		{
			int dataOffset = (4 * TOTAL_TOKENS_PER_CADET) + m_TotalMitoTokens; 

			m_CadetTokens[dataOffset].tokenId = baseId;
			m_CadetTokens[dataOffset].collected = false;
			m_CadetTokens[dataOffset].s.vCenterPoint = pos;
			m_CadetTokens[dataOffset].pModel = res::LoadModel(414);
			DBG_ASSERT( m_CadetTokens[dataOffset].pModel != 0 );

			m_TotalMitoTokens++;

			m_TotalCadetTokens++;
		}
		else
			DBG_ASSERT(0); // too many tokens?
	}
	else if( baseId == 2005 )
	{
		if( m_TotalUkiTokens < TOTAL_TOKENS_PER_CADET )
		{
			int dataOffset = (5 * TOTAL_TOKENS_PER_CADET) + m_TotalUkiTokens; 

			m_CadetTokens[dataOffset].tokenId = baseId;
			m_CadetTokens[dataOffset].collected = false;
			m_CadetTokens[dataOffset].s.vCenterPoint = pos;
			m_CadetTokens[dataOffset].pModel = res::LoadModel(415);
			DBG_ASSERT( m_CadetTokens[dataOffset].pModel != 0 );

			m_TotalUkiTokens++;

			m_TotalCadetTokens++;
		}
		else
			DBG_ASSERT(0); // too many tokens?
	}

	if( baseId == 3000 )
	{
		if( m_TotalStarTokens < STAR_TOKENS )
		{
			m_StarTokens[m_TotalStarTokens].tokenId = baseId;
			m_StarTokens[m_TotalStarTokens].collected = false;
			m_StarTokens[m_TotalStarTokens].s.vCenterPoint = pos;
			m_StarTokens[m_TotalStarTokens].pModel = res::LoadModel(400);
			DBG_ASSERT( m_StarTokens[m_TotalStarTokens].pModel != 0 );

			m_TotalStarTokens++;
		}
		else
			DBG_ASSERT(0); // too many tokens?
	}
	else if( baseId == 3001 )
	{	
		if( m_TotalHiddenTokens < HIDDEN_TOKENS )
		{
			m_HiddenTokens[m_TotalHiddenTokens].tokenId = baseId;
			m_HiddenTokens[m_TotalHiddenTokens].collected = false;
			m_HiddenTokens[m_TotalHiddenTokens].s.vCenterPoint = pos;
			m_HiddenTokens[m_TotalHiddenTokens].pModel = res::LoadModel(401);
			DBG_ASSERT( m_HiddenTokens[m_TotalHiddenTokens].pModel != 0 );

			m_TotalHiddenTokens++;
		}
		else
			DBG_ASSERT(0); // too many tokens?
	}
}

/////////////////////////////////////////////////////
/// Method: ParseSpawnPoint
/// Params: 
///
/////////////////////////////////////////////////////
void Level::ParseSpawnPoint( int paramIndex, int baseType, int baseId )
{
	math::Vec3 pos;
	math::Vec3 rot;

	// all types have position
	pos = ParsePosition( paramIndex );

	rot.X = static_cast<float>( script::LuaGetNumberFromTableItem( "rotX", paramIndex, 0.0 ) );
	rot.Y = static_cast<float>( script::LuaGetNumberFromTableItem( "rotY", paramIndex, 0.0 ) );
	rot.Z = static_cast<float>( script::LuaGetNumberFromTableItem( "rotZ", paramIndex, 0.0 ) );

	int spawnType = static_cast<int>( script::LuaGetNumberFromTableItem( "spawnType", paramIndex, 0.0 ) );
	int spawnGroup = static_cast<int>( script::LuaGetNumberFromTableItem( "spawnGroup", paramIndex, -1.0 ) );

	SpawnPoint newSpawn;
	newSpawn.pos = pos;
	newSpawn.rot = rot;
	newSpawn.spawnType = spawnType;
	newSpawn.spawnGroup = spawnGroup;

	m_SpawnPoints.push_back(newSpawn);
}

/////////////////////////////////////////////////////
/// Method: ParseGate
/// Params: 
///
/////////////////////////////////////////////////////
void Level::ParseGate( int paramIndex, int baseType, int baseId )
{
	math::Vec3 pos;
	math::Vec3 rot;

	// all types have position
	pos = ParsePosition( paramIndex );

	rot.X = static_cast<float>( script::LuaGetNumberFromTableItem( "rotX", paramIndex, 0.0 ) );
	rot.Y = static_cast<float>( script::LuaGetNumberFromTableItem( "rotY", paramIndex, 0.0 ) );
	rot.Z = static_cast<float>( script::LuaGetNumberFromTableItem( "rotZ", paramIndex, 0.0 ) );

	int gateType = static_cast<int>( script::LuaGetNumberFromTableItem( "gateType", paramIndex, 0.0 ) );
	int gateModelId = static_cast<int>( script::LuaGetNumberFromTableItem( "gateModelId", paramIndex, -1.0 ) );
	int gateGroupId = static_cast<int>( script::LuaGetNumberFromTableItem( "gateGroupId", paramIndex, -1.0 ) );
	int gateNumber = static_cast<int>( script::LuaGetNumberFromTableItem( "gateNumber", paramIndex, 0 ) );
	int gateAlwaysDraw = static_cast<int>( script::LuaGetNumberFromTableItem( "gateAlwaysDraw", paramIndex, 0 ) );
	int gateCollisionType = static_cast<int>( script::LuaGetNumberFromTableItem( "gateCollisionType", paramIndex, 0 ) );

	float w = static_cast<float>( script::LuaGetNumberFromTableItem( "gateBoxWidth", paramIndex, 1.0 ) );
	float h = static_cast<float>( script::LuaGetNumberFromTableItem( "gateBoxHeight", paramIndex, 1.0 ) );
	float d = static_cast<float>( script::LuaGetNumberFromTableItem( "gateBoxDepth", paramIndex, 1.0 ) );
	float r = static_cast<float>( script::LuaGetNumberFromTableItem( "gateSphereRadius", paramIndex, 1.0 ) );

	int gateAchievementId = static_cast<int>( script::LuaGetNumberFromTableItem( "gateAchievement", paramIndex, -1.0 ) );
	int gateTextId = static_cast<int>( script::LuaGetNumberFromTableItem( "gateTextId", paramIndex, -1.0 ) );

	Gate newGate;
	newGate.enabled = false;
	newGate.pos = pos;
	newGate.rot = rot;
	newGate.type = gateType; // 0 = ring 1=speedup 2=sightsee
	if( gateModelId != -1 )
		newGate.model = res::LoadModel( gateModelId );
	else
		newGate.model = 0;
	//newGate.model->SetTextureAnimation(0);
	newGate.groupId = gateGroupId;
	newGate.number = gateNumber;
	newGate.alwaysDraw = gateAlwaysDraw;
	newGate.collisionType = gateCollisionType;
	newGate.s.vCenterPoint = pos;
	newGate.s.fRadius = m_GameData.DEFAULT_RING_COLLISION_RADIUS;
	newGate.obb.vCenter = pos;
	newGate.obb.vHalfWidths = math::Vec3( w*0.5f, h*0.5f, d*0.5f );

	// rotate the heading by the node rotation
	dRSetIdentity( newGate.orientation );
	if( gateType == 0 || gateType == 1 ) 
		dRFromEulerAngles( newGate.orientation, math::DegToRad(-rot.X), math::DegToRad(-rot.Y), math::DegToRad(-rot.Z) );
	else
		dRFromEulerAngles( newGate.orientation, math::DegToRad(-rot.X), math::DegToRad(-rot.Y), math::DegToRad(-rot.Z) );

	newGate.obb.vAxis[0] = math::Vec3( newGate.orientation[0], newGate.orientation[1], newGate.orientation[2] );
	newGate.obb.vAxis[1] = math::Vec3( newGate.orientation[4], newGate.orientation[5], newGate.orientation[6] );
	newGate.obb.vAxis[2] = math::Vec3( newGate.orientation[8], newGate.orientation[9], newGate.orientation[10] );

	newGate.achievementId = gateAchievementId;
	newGate.textId = gateTextId;

	if( gateType == 0 )
		m_Gates.push_back(newGate);
	else
	{
		newGate.enabled = true;
		m_Sights.push_back(newGate);
	}
}

/////////////////////////////////////////////////////
/// Method: ParseTarget
/// Params: 
///
/////////////////////////////////////////////////////
void Level::ParseTarget( int paramIndex, int baseType, int baseId )
{
/*	math::Vec3 pos;
	math::Vec3 rot;

	// all types have position
	pos = ParsePosition( paramIndex );

	rot.X = static_cast<float>( script::LuaGetNumberFromTableItem( "rotX", paramIndex, 0.0 ) );
	rot.Y = static_cast<float>( script::LuaGetNumberFromTableItem( "rotY", paramIndex, 0.0 ) );
	rot.Z = static_cast<float>( script::LuaGetNumberFromTableItem( "rotZ", paramIndex, 0.0 ) );

	int targetType = static_cast<int>( script::LuaGetNumberFromTableItem( "targetType", paramIndex, 0.0 ) );
	int targetModelId = static_cast<int>( script::LuaGetNumberFromTableItem( "targetModelId", paramIndex, -1.0 ) );
	int targetSize = static_cast<int>( script::LuaGetNumberFromTableItem( "targetSize", paramIndex, 1.0 ) );
	float targetValue = static_cast<float>( script::LuaGetNumberFromTableItem( "targetValue", paramIndex, 1.0 ) );
	float r = static_cast<float>( script::LuaGetNumberFromTableItem( "targetSphereRadius", paramIndex, 1.0 ) );
*/
}	

/////////////////////////////////////////////////////
/// Method: ParseObject
/// Params: 
///
/////////////////////////////////////////////////////
void Level::ParseObject( int paramIndex, int baseType, int baseId, bool targetMode, int whichResId, int objRes1, int objRes2, int objRes3 )
{
	math::Vec3 pos;
	math::Vec3 rot;
	math::Vec3 normal( 0.0f, 1.0f, 0.0f );
	TerrainTriMesh* pTriMesh = 0;

	// all types have position
	pos = ParsePosition( paramIndex );

	rot.X = -static_cast<float>( script::LuaGetNumberFromTableItem( "rotX", paramIndex, 0.0 ) );
	rot.Y = -static_cast<float>( script::LuaGetNumberFromTableItem( "rotY", paramIndex, 0.0 ) );
	rot.Z = -static_cast<float>( script::LuaGetNumberFromTableItem( "rotZ", paramIndex, 0.0 ) );

	int objectType = static_cast<int>( script::LuaGetNumberFromTableItem( "objectType", paramIndex, 0.0 ) );
	int objectResId = static_cast<int>( script::LuaGetNumberFromTableItem( "objectResId", paramIndex, -1.0 ) );
	int objectGroupId = static_cast<int>( script::LuaGetNumberFromTableItem( "objectGroupId", paramIndex, -1.0 ) );
	int objectSnapToGrid = static_cast<int>( script::LuaGetNumberFromTableItem( "objectSnapToGrid", paramIndex, 1.0 ) );
	int objectOrientateToGrid = static_cast<int>( script::LuaGetNumberFromTableItem( "objectOrientateToGrid", paramIndex, 1.0 ) );

	// find the position
	//if( objectSnapToGrid )
	{
		// ray 
		RayObject* pRayTest = GameSystems::GetInstance()->GetRayObject();
		DBG_ASSERT( pRayTest != 0 );

		// ray check
		pRayTest->hasHits = false;
		pRayTest->totalHits = 0;
		pRayTest->nextHitIndex = 0;

		// start the ray high
		pRayTest->rayPos = math::Vec3( pos.X, pos.Y + 5000.0f, pos.Z );
		pRayTest->rayDir = math::Vec3( 0.0f, -1.0f, 0.0f );

		dGeomRaySet (pRayTest->rayGeom, pRayTest->rayPos.X, pRayTest->rayPos.Y, pRayTest->rayPos.Z, 
				pRayTest->rayDir.X, pRayTest->rayDir.Y, pRayTest->rayDir.Z );

		dGeomRaySetLength (pRayTest->rayGeom, 10000.0f);

		dGeomSetCategoryBits( pRayTest->rayGeom, CATEGORY_RAYCAST );
		dGeomSetCollideBits( pRayTest->rayGeom, CATEGORY_WORLD );

		dSpaceCollide(physics::PhysicsWorldODE::GetSpace(), 0, &PhysicsTestRayToWorld);

		if( pRayTest->totalHits <= 0 )
		{
			pRayTest->rayHits[0].pos[0] = m_Pos.X;
			pRayTest->rayHits[0].pos[1] = m_Pos.Y;
			pRayTest->rayHits[0].pos[2] = m_Pos.Z;

			pRayTest->rayHits[0].n[0] = 0.0f;
			pRayTest->rayHits[0].n[1] = 1.0f;
			pRayTest->rayHits[0].n[2] = 0.0f;

			pRayTest->totalHits = 1;

			DBGLOG( "*WARNING* raycast failed on terrain, placing object at the node\n" );
		}

		// ray hit something
		if( pRayTest->totalHits > 0 )
		{
			if( objectSnapToGrid )
			{
				// move the position slightly higher
				pos.X = pRayTest->rayHits[0].pos[0] + (pRayTest->rayHits[0].n[0]*m_GameData.SHADOW_OFFSET);
				pos.Y = pRayTest->rayHits[0].pos[1] + (-pRayTest->rayHits[0].n[1]*m_GameData.SHADOW_OFFSET);
				pos.Z = pRayTest->rayHits[0].pos[2] + (pRayTest->rayHits[0].n[2]*m_GameData.SHADOW_OFFSET);

				normal = math::Vec3( pRayTest->rayHits[0].n[0], -pRayTest->rayHits[0].n[1], pRayTest->rayHits[0].n[2] );
			}

			pTriMesh = reinterpret_cast<TerrainTriMesh*>( pRayTest->rayHits[0].pData );
			DBG_ASSERT( pTriMesh != 0 );
		}
		else
			return;
	}

	const res::ObjectResourceStore* objRes = res::GetObjectResource( objectResId );
	DBG_ASSERT( objRes != 0 );

	if( targetMode )
	{
		if( objectResId == objRes1 ||
			objectResId == objRes2 ||
			objectResId == objRes3 )
		{
			// only add the res that matches the current target
			if( whichResId == objectResId )
			{
				m_TargetData[m_NextTargetIndex].pObj = AddObject( pTriMesh->index, pTriMesh->meshAABB, pos, rot, normal, objectType, objectResId, objectGroupId, (objectSnapToGrid!=0), (objectOrientateToGrid!=0)  );
				m_NextTargetIndex++;
				if( m_NextTargetIndex > MAX_TARGET_DRAWS )
				{
					DBG_ASSERT(0);
					m_NextTargetIndex = 0;
				}
			}
		}
		else
		{
			// normal object
			AddObject( pTriMesh->index, pTriMesh->meshAABB, pos, rot, normal, objectType, objectResId, objectGroupId, (objectSnapToGrid!=0), (objectOrientateToGrid!=0)  );

			// barn
			if( objectResId == 1 )
			{
				Gate newGate;
				newGate.enabled = false;
				newGate.pos = pos;
				newGate.rot = rot;
				newGate.type = 2; // 0 = ring 1=speedup 2=sightsee
				newGate.model = 0;
				newGate.groupId = -1;
				newGate.number = -1;
				newGate.alwaysDraw = true;
				newGate.collisionType = 1;
				newGate.s.vCenterPoint = pos;
				newGate.s.fRadius = m_GameData.DEFAULT_RING_COLLISION_RADIUS;
				newGate.obb.vCenter = pos;
				newGate.obb.vHalfWidths = math::Vec3( 10.0f*0.5f, 20.0f*0.5f, 10.0f*0.5f );

				// rotate the heading by the node rotation
				dRSetIdentity( newGate.orientation );
				//if( gateType == 0 ) 
				//	dRFromEulerAngles( newGate.orientation, 0.0f, math::DegToRad(rot.Y), 0.0f );
				//else
					dRFromEulerAngles( newGate.orientation, math::DegToRad(rot.X), math::DegToRad(rot.Y), math::DegToRad(rot.Z) );

				newGate.obb.vAxis[0] = math::Vec3( newGate.orientation[0], newGate.orientation[1], newGate.orientation[2] );
				newGate.obb.vAxis[1] = math::Vec3( newGate.orientation[4], newGate.orientation[5], newGate.orientation[6] );
				newGate.obb.vAxis[2] = math::Vec3( newGate.orientation[8], newGate.orientation[9], newGate.orientation[10] );

				newGate.achievementId = 52;
				newGate.textId = -1;

				newGate.enabled = true;
				m_Sights.push_back(newGate);
			}

			if( objRes->destroyFlag )
				m_TotalDestroyable++;
		}
	}
	else
	{
		// don't add any targets
		if( objectResId != objRes1 &&
			objectResId != objRes2 &&
			objectResId != objRes3 )		
		{
			// normal
			AddObject( pTriMesh->index, pTriMesh->meshAABB, pos, rot, normal, objectType, objectResId, objectGroupId, (objectSnapToGrid!=0), (objectOrientateToGrid!=0)  );

			// barn
			if( objectResId == 1 )
			{
				Gate newGate;
				newGate.enabled = false;
				newGate.pos = pos;
				newGate.rot = rot;
				newGate.type = 2; // 0 = ring 1=speedup 2=sightsee
				newGate.model = 0;
				newGate.groupId = -1;
				newGate.number = -1;
				newGate.alwaysDraw = true;
				newGate.collisionType = 1;
				newGate.s.vCenterPoint = pos;
				newGate.s.fRadius = m_GameData.DEFAULT_RING_COLLISION_RADIUS;
				newGate.obb.vCenter = pos;
				newGate.obb.vHalfWidths = math::Vec3( 10.0f*0.5f, 20.0f*0.5f, 10.0f*0.5f );

				// rotate the heading by the node rotation
				dRSetIdentity( newGate.orientation );
				//if( gateType == 0 ) 
				//	dRFromEulerAngles( newGate.orientation, 0.0f, math::DegToRad(rot.Y), 0.0f );
				//else
					dRFromEulerAngles( newGate.orientation, math::DegToRad(rot.X), math::DegToRad(rot.Y), math::DegToRad(rot.Z) );

				newGate.obb.vAxis[0] = math::Vec3( newGate.orientation[0], newGate.orientation[1], newGate.orientation[2] );
				newGate.obb.vAxis[1] = math::Vec3( newGate.orientation[4], newGate.orientation[5], newGate.orientation[6] );
				newGate.obb.vAxis[2] = math::Vec3( newGate.orientation[8], newGate.orientation[9], newGate.orientation[10] );

				newGate.achievementId = 52;
				newGate.textId = -1;

				newGate.enabled = true;
				m_Sights.push_back(newGate);
			}

			if( objRes->destroyFlag )
				m_TotalDestroyable++;
		}
	}
}

/////////////////////////////////////////////////////
/// Method: ParseSprite
/// Params: 
///
/////////////////////////////////////////////////////
void Level::ParseSprite( int paramIndex, int baseType, int baseId )
{
	math::Vec3 pos;
	math::Vec3 rot;

	// all types have position
	pos = ParsePosition( paramIndex );

	rot.Y = static_cast<float>( script::LuaGetNumberFromTableItem( "rotY", paramIndex, 0.0 ) );

	int spriteResId = static_cast<int>( script::LuaGetNumberFromTableItem( "spriteResId", paramIndex, -1 ) );
	int spriteType = static_cast<int>( script::LuaGetNumberFromTableItem( "spriteType", paramIndex, 0.0 ) );
	float w = static_cast<float>( script::LuaGetNumberFromTableItem( "w", paramIndex, 1.0 ) );
	float h = static_cast<float>( script::LuaGetNumberFromTableItem( "h", paramIndex, 1.0 ) );
	float d = static_cast<float>( script::LuaGetNumberFromTableItem( "d", paramIndex, 1.0 ) );
	int r = static_cast<int>( script::LuaGetNumberFromTableItem( "r", paramIndex, 255.0 ) );
	int g = static_cast<int>( script::LuaGetNumberFromTableItem( "g", paramIndex, 255.0 ) );
	int b = static_cast<int>( script::LuaGetNumberFromTableItem( "b", paramIndex, 255.0 ) );
	int a = static_cast<int>( script::LuaGetNumberFromTableItem( "a", paramIndex, 255.0 ) );

	SpriteType* newSprite = new SpriteType;
	DBG_ASSERT( (newSprite != 0) );

	newSprite->enabled = true;
	newSprite->pos.X = pos.X;
	newSprite->pos.Y = pos.Y;
	newSprite->pos.Z = pos.Z;
			
	newSprite->angle = math::DegToRad(rot.Y);

	newSprite->resId = spriteResId;
	newSprite->baseId = baseId;
	newSprite->type = spriteType;

	newSprite->w = w;
	newSprite->h = h;
	newSprite->d = d;
					
	newSprite->col.R = static_cast<unsigned char>(r);
	newSprite->col.G = static_cast<unsigned char>(g);
	newSprite->col.B = static_cast<unsigned char>(b);
	newSprite->col.A = static_cast<unsigned char>(a);

	if( spriteResId != -1 )
	{
		const res::SpriteResourceStore* sr = res::GetSpriteResource( newSprite->resId );
		if( sr && m_SpriteTextureId == renderer::INVALID_OBJECT )
		{
			m_SpriteTextureId = res::GetTextureResource( sr->textureResId )->texId; 
		}	
	}

	m_SpriteData.push_back( newSprite );
}	

/////////////////////////////////////////////////////
/// Method: ParseEmitter
/// Params: 
///
/////////////////////////////////////////////////////
void Level::ParseEmitter( int paramIndex, int baseType, int baseId )
{
	math::Vec3 pos;
	math::Vec3 rot;

	// all types have position
	pos = ParsePosition( paramIndex );

	rot.Y = static_cast<float>( script::LuaGetNumberFromTableItem( "rotY", paramIndex, 0.0 ) );

	int emitterResId = static_cast<int>( script::LuaGetNumberFromTableItem( "emitterResId", paramIndex, -1.0 ) );
	int state = static_cast<int>( script::LuaGetNumberFromTableItem( "state", paramIndex, 0.0 ) );						

	float timeOn = static_cast<float>( script::LuaGetNumberFromTableItem( "timeOn", paramIndex, -1.0 ) );
	float timeOff = static_cast<float>( script::LuaGetNumberFromTableItem( "timeOff", paramIndex, -1.0 ) );

	if( emitterResId != -1 )
	{
		efx::Emitter* pEmitter = 0;
		pEmitter = new efx::Emitter;
		DBG_ASSERT( pEmitter != 0 );

		pEmitter->Create( *res::GetEmitterResource( emitterResId )->block );
		res::SetupTexturesOnEmitter( pEmitter );
		pEmitter->SetPos( pos );
		pEmitter->SetTimeOn(timeOn);
		pEmitter->SetTimeOff(timeOff);
		if( state == efx::Emitter::EMITTERSTATE_STARTON )
			pEmitter->Enable();
		else
			pEmitter->Disable();

		m_EmitterList.push_back(pEmitter);
	}
}

/////////////////////////////////////////////////////
/// Method: ParseSound
/// Params: 
///
/////////////////////////////////////////////////////
void Level::ParseSound( int paramIndex, int baseType, int baseId )
{
	math::Vec3 pos;
	math::Vec3 offset;
	math::Vec3 rot;

	// all types have position
	pos = ParsePosition( paramIndex );

	rot.Y = static_cast<float>( script::LuaGetNumberFromTableItem( "rotY", paramIndex, 0.0 ) );
		
	int soundResId = static_cast<int>( script::LuaGetNumberFromTableItem( "soundResId", paramIndex, -1 ) );
	int soundType = static_cast<int>( script::LuaGetNumberFromTableItem( "soundType", paramIndex, 0.0 ) );	

	float soundTime = static_cast<float>( script::LuaGetNumberFromTableItem( "soundTime", paramIndex, 1.0 ) );
	float x = static_cast<float>( script::LuaGetNumberFromTableItem( "x", paramIndex, 0.0 ) );
	float y = static_cast<float>( script::LuaGetNumberFromTableItem( "y", paramIndex, 0.0 ) );
	float z = static_cast<float>( script::LuaGetNumberFromTableItem( "z", paramIndex, 0.0 ) );

	float pitch = static_cast<float>( script::LuaGetNumberFromTableItem( "pitch", paramIndex, 1.0 ) );
	float gain = static_cast<float>( script::LuaGetNumberFromTableItem( "gain", paramIndex, 1.0 ) );
	bool rel = static_cast<int>( script::LuaGetNumberFromTableItem( "rel", paramIndex, 0.0 ) ) != 0;
	bool loop = static_cast<int>( script::LuaGetNumberFromTableItem( "loop", paramIndex, 0.0 ) ) != 0;
	int areaType = static_cast<int>( script::LuaGetNumberFromTableItem( "areaType", paramIndex, 0.0 ) );

	float w = static_cast<float>( script::LuaGetNumberFromTableItem( "w", paramIndex, 1.0 ) );
	float h = static_cast<float>( script::LuaGetNumberFromTableItem( "h", paramIndex, 1.0 ) );
	float d = static_cast<float>( script::LuaGetNumberFromTableItem( "d", paramIndex, 1.0 ) );
	float r = static_cast<float>( script::LuaGetNumberFromTableItem( "r", paramIndex, 1.0 ) );

	AudioLocator* pNewAudioLocator = new AudioLocator;

	offset = math::Vec3( x, y, z );
	
	pNewAudioLocator->sourceId = snd::INVALID_SOUNDSOURCE;
	if( soundResId != -1 )
		pNewAudioLocator->bufferId = AudioSystem::GetInstance()->AddAudioFile( soundResId );
	pNewAudioLocator->pos = pos;

	if( rel )
		pNewAudioLocator->offset = offset;
	else
		pNewAudioLocator->offset = pos+offset;

	pNewAudioLocator->pitch = pitch;
	pNewAudioLocator->gain = gain;
#ifdef BASE_PLATFORM_iOS
	pNewAudioLocator->gain = 2.0f;
#endif
	pNewAudioLocator->loop = loop;
	pNewAudioLocator->relative = rel;
	pNewAudioLocator->type = soundType;

	pNewAudioLocator->time = soundTime;

	pNewAudioLocator->triggerType = areaType;

	pNewAudioLocator->sphere.vCenterPoint = pNewAudioLocator->pos;
	pNewAudioLocator->sphere.fRadius = r;

	pNewAudioLocator->aabb.vCenter = pNewAudioLocator->pos;
	pNewAudioLocator->aabb.vBoxMin.X = pos.X-(w*0.5f);
	pNewAudioLocator->aabb.vBoxMin.Y = pos.Y-(h*0.5f);
	pNewAudioLocator->aabb.vBoxMin.Z = pos.Z-(d*0.5f);

	pNewAudioLocator->aabb.vBoxMax.X = pos.X+(w*0.5f);
	pNewAudioLocator->aabb.vBoxMax.Y = pos.Y+(h*0.5f);
	pNewAudioLocator->aabb.vBoxMax.Z = pos.Z+(d*0.5f);

	//	pNewAudioLocator->gainAttenuation = (iVal != 0);

	pNewAudioLocator->lastTimeUpdate = 0.0f;
	
	// don't allow looping and timed
	if( pNewAudioLocator->loop && pNewAudioLocator->type == eAudioType_Timed )
		pNewAudioLocator->loop = false;

	m_SoundAreas.push_back( pNewAudioLocator );
}

/////////////////////////////////////////////////////
/// Method: GetSpawnPointForGroup
/// Params: 
///
/////////////////////////////////////////////////////
int Level::GetSpawnPointForGroup( int groupId )
{
	unsigned int i=0;

	for( i=0; i < m_SpawnPoints.size(); ++i )
	{
		if( m_SpawnPoints[i].spawnGroup == groupId )
		{
			return i;
		}
	}

	return -1;
}

/////////////////////////////////////////////////////
/// Method: SetupGateGroups
/// Params: 
///
/////////////////////////////////////////////////////
void Level::SetupGateGroups()
{
	unsigned int i=0, j=0;

	std::vector<Gate>::iterator it = m_Gates.begin();
	std::vector<groupData> groupIdList;
	groupData newData;

	groupIdList.clear();

	// go through the complete gate list
	while( it != m_Gates.end() )
	{
		bool needToAddGroup = true;

		newData.groupId = (*it).groupId;
		newData.numGates = 0;

		// does this gates group already exist?
		for( i=0; i < groupIdList.size(); ++i )
		{
			if( (*it).groupId == groupIdList[i].groupId )
			{
				needToAddGroup = false;
				groupIdList[i].numGates++;
			}
		}

		if( needToAddGroup )
		{
			newData.numGates++;
			groupIdList.push_back(newData);
		}

		// next gate
		it++;
	}

	// now create the real groups
	for( i=0; i < groupIdList.size(); ++i )
	{
		GateGroup newGroup;
		newGroup.enabled = false;
		newGroup.groupId = groupIdList[i].groupId;
		newGroup.numGates = groupIdList[i].numGates;
		newGroup.highestGateNum = 0;
		newGroup.gateList = new Gate*[newGroup.numGates];
		for( j=0; j < groupIdList.size(); ++j )
			newGroup.gateList[j] = 0;

		newGroup.nextGateIndex = 0;

		int spawnIndex = GetSpawnPointForGroup(newGroup.groupId);
		if( spawnIndex != -1 )
		{
			newGroup.spawnPos = m_SpawnPoints[spawnIndex].pos;
			newGroup.spawnRot = m_SpawnPoints[spawnIndex].rot;
		}
		else
		{
			newGroup.spawnPos = m_SpawnPoints[0].pos;
			newGroup.spawnRot = m_SpawnPoints[0].rot;
		}

		m_GateGroups.push_back(newGroup);
	}

	// loop again assigning pointers
	it = m_Gates.begin();
	while( it != m_Gates.end() )
	{
		// find and assign
		for( i=0; i < m_GateGroups.size(); ++i )
		{
			if( (*it).groupId == m_GateGroups[i].groupId )
			{
				m_GateGroups[i].gateList[m_GateGroups[i].nextGateIndex] = &(*it);

				//if( m_GateGroups[i].nextGateIndex == 0 )
				//	m_GateGroups[i].gateList[m_GateGroups[i].nextGateIndex]->enabled = true;

				if( m_GateGroups[i].gateList[m_GateGroups[i].nextGateIndex]->number > m_GateGroups[i].highestGateNum )
					m_GateGroups[i].highestGateNum = m_GateGroups[i].gateList[m_GateGroups[i].nextGateIndex]->number;
				
				m_GateGroups[i].nextGateIndex++;
			}
		}

		// next gate
		it++;
	}
}

/////////////////////////////////////////////////////
/// Method: UpdateGates
/// Params: 
///
/////////////////////////////////////////////////////
void Level::UpdateGates( float deltaTime )
{
	std::vector<Gate>::iterator it = m_Gates.begin();

	while( it != m_Gates.end() )
	{
		if( (*it).enabled )
		{
			if( (*it).model != 0 )
				(*it).model->Update( deltaTime );
		}

		it++;
	}
}

/////////////////////////////////////////////////////
/// Method: AddObject
/// Params: None
///
/////////////////////////////////////////////////////
WorldObject* Level::AddObject( int batchId, const collision::AABB& batchAABB, const math::Vec3& pos, const math::Vec3& rot, const math::Vec3& normal, int worldType, int resId, int groupId, bool snapToGrid, bool orientateToGrid )
{
	int i=0;
	ObjectBatch* pBatch = 0;

	for( i=0; i < m_NumBatchesInUse; ++i )
	{
		if( m_ObjectBatchList[i].batchId == batchId )
		{
			if( m_ObjectBatchList[i].numObjects >= MAX_PER_BATCH-1 )
				continue;
			else
			{
				pBatch = &m_ObjectBatchList[i];
			}
		}
	}

	// batch not found, create a new one
	if( pBatch == 0 )
	{
		m_NumBatchesInUse++;
		if( m_NumBatchesInUse >= MAX_OBJECT_BATCHES-1 )
		{
			m_NumBatchesInUse = MAX_OBJECT_BATCHES-1;
			return 0;
		}

		pBatch = &m_ObjectBatchList[m_NumBatchesInUse-1];
		pBatch->numObjects = 0;
		pBatch->batchId = batchId;
		pBatch->objBatchAABB = batchAABB;
	}

	pBatch->objectList[pBatch->numObjects].Create( pos, rot, normal, worldType, resId, groupId, (snapToGrid!=0), (orientateToGrid!=0), m_GameData.LOD_SCALER );
	pBatch->numObjects++;

	return &pBatch->objectList[pBatch->numObjects-1];
}

/////////////////////////////////////////////////////
/// Function: SetTargetObjectId
/// Params: [in]resId
///
/////////////////////////////////////////////////////
void Level::SetTargetObjectId( int resId )
{
	m_TargetObjId = resId;

	int i=0, j=0;
	for( i=0; i < m_NumBatchesInUse; ++i )
	{
		for( j=0; j < m_ObjectBatchList[i].numObjects; ++j )
		{
			if( m_ObjectBatchList[i].objectList[j].GetResId() == m_TargetObjId )
			{
				m_ObjectBatchList[i].objectList[j].SetTarget();
				m_TotalNumTargets++;
			}
		}
	}
}

/////////////////////////////////////////////////////
/// Function: SetPlayer
/// Params: [in]pPlayer
///
/////////////////////////////////////////////////////
void Level::SetPlayer( Player* pPlayer )
{
	m_pPlayer = pPlayer;
	m_BatchSprites.SetPlayer( pPlayer );
}

/////////////////////////////////////////////////////
/// Function: PhysicsTestRayToWorld
/// Params: [in]data, [in]o1, [in]o2
///
/////////////////////////////////////////////////////
void PhysicsTestRayToWorld (void *data, dGeomID o1, dGeomID o2)
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

						pRay->rayHits[pRay->nextHitIndex].pData = dGeomGetData(o2);
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

						pRay->rayHits[pRay->nextHitIndex].pData = dGeomGetData(o1);
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
