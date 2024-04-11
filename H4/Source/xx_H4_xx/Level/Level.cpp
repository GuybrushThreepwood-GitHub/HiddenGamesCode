
/*===================================================================
	File: Level.cpp
	App: H4

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include <cmath>
#include "MathBase.h"
#include "RenderBase.h"
#include "CollisionBase.h"
#include "SoundBase.h"
#include "ModelBase.h"

#include "H4.h"
#include "AppConsts.h"

#include "Audio/AudioSystem.h"
#include "Player/Player.h"

#include "Resources/ModelResources.h"
#include "Resources/SpriteResources.h"
#include "Resources/EmitterResources.h"
#include "Resources/TextureResources.h"
#include "Resources/SoundResources.h"

#include "Resources/ResourceHelper.h"

#include "GameSystems.h"

#include "LevelTypes.h"
#include "Level.h"

namespace
{
	math::Vec3 zeroVec(0.0f,0.0f,0.0f);

	int SpriteSort( const Level::SpriteType* s1, const Level::SpriteType* s2 )
	{
		if(s1->sceneDepth > s2->sceneDepth)
			return(1);

		return(0);
	}

	const int ENEMY_SPAWNTYPE_STATIC = 0;
	const int ENEMY_SPAWNTYPE_CONSTANT = 1;
	const int ENEMY_SPAWNTYPE_FLOATINGHEAD = 2;
	const int ENEMY_SPAWNTYPE_HANGINGSPIDER = 3;
	const int ENEMY_SPAWNTYPE_CRAWLINGSPIDER = 4;
	const int ENEMY_SPAWNTYPE_ANTINSECTS = 5;
	const int ENEMY_SPAWNTYPE_COCKROACHINSECTS = 6;
}

/////////////////////////////////////////////////////
/// Default constructor
/// 
///
/////////////////////////////////////////////////////
Level::Level()
	: PhysicsIdentifier( PHYSICSBASICID_WORLD, PHYSICSCASTID_WORLD )
{
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
	m_LastDelta = 0.0f;
	m_FirstUpdate = true;
	m_pMeshData = 0;
	m_pSkyData = 0;

	m_PhysicsIdList.clear();

	m_GameObjects.clear();
	m_UseAreas.clear();
	m_Doors.clear();
	m_LightZones.clear();
	m_AnimatedMeshes.clear();
	m_SpriteData.clear();
	m_SoundLocations.clear();
	m_StepAudioZones.clear();

	m_LimitedInstances.clear();
	m_ExtendedInstances.clear();

	m_LimitedRoomInstances.clear();
	m_ExtendedRoomInstances.clear();

	m_SpriteTextureId = renderer::INVALID_OBJECT;
	m_SpriteCount = 0;
	m_SpriteData.clear();
	m_SpriteList = 0;

	m_TotalAmmoRooms = 0;
	m_MaxAmmoRooms = 0;

	m_TotalHealthRooms = 0;
	m_MaxHealthRooms = 0;

	m_TotalZombieRooms = 0;
	m_MaxZombieRooms = 0;

	m_TotalEmptyRooms = 0;
	m_MaxEmptyRooms = 0;

	m_EmitterList.clear();

	m_pBody = 0;
	m_pGenericPhysicsId = 0;

	m_PlayerSpawnId = -1;
	m_PlayerStartPoint = math::Vec2( 0.0f, 5.0f );
}

/////////////////////////////////////////////////////
/// Method: Release
/// Params: None
///
/////////////////////////////////////////////////////
void Level::Release( void )
{
	unsigned int i=0;

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

	if( m_pMeshData != 0 )
	{
		delete m_pMeshData;
		m_pMeshData = 0;
	}

	if( m_pSkyData != 0 )
	{
		res::RemoveModel( m_pSkyData );
		m_pSkyData = 0;
	}

	m_UseAreas.clear();
	m_Doors.clear();
	m_AnimatedMeshes.clear();

	// delete light zones
	for( i=0; i < m_LightZones.size(); ++i )
	{
		delete m_LightZones[i];
		m_LightZones[i] = 0;
	}
	m_LightZones.clear();

	// delete step audio zones
	for( i=0; i < m_StepAudioZones.size(); ++i )
	{
		delete m_StepAudioZones[i];
		m_StepAudioZones[i] = 0;
	}
	m_StepAudioZones.clear();

	// sprite data
	m_SpriteCount = 0;
	for( i=0; i < m_SpriteData.size(); ++i )
	{
		delete m_SpriteData[i];
		m_SpriteData[i] = 0;
	}
	m_SpriteData.clear();
	//renderer::RemoveTexture(m_SpriteTextureId);

	delete[] m_SpriteList;
	m_SpriteList = 0;

	for( i=0; i < m_SoundLocations.size(); ++i )
	{
		if( m_SoundLocations[i]->bufferId != snd::INVALID_SOUNDBUFFER )
			snd::RemoveSound( m_SoundLocations[i]->bufferId );

		delete m_SoundLocations[i];
		m_SoundLocations[i] = 0;
	}
	m_SoundLocations.clear();

	m_TotalAmmoRooms = 0;
	m_MaxAmmoRooms = 0;

	m_TotalHealthRooms = 0;
	m_MaxHealthRooms = 0;

	m_TotalZombieRooms = 0;
	m_MaxZombieRooms = 0;

	m_TotalEmptyRooms = 0;
	m_MaxEmptyRooms = 0;

	// delete limited instances
	for( i=0; i < m_LimitedInstances.size(); ++i )
	{
		delete m_LimitedInstances[i];
		m_LimitedInstances[i] = 0;
	}
	m_LimitedInstances.clear();

	// delete extended instances
	for( i=0; i < m_ExtendedInstances.size(); ++i )
	{
		delete m_ExtendedInstances[i];
		m_ExtendedInstances[i] = 0;
	}
	m_ExtendedInstances.clear();

	// delete limited room instances
	for( i=0; i < m_LimitedRoomInstances.size(); ++i )
	{
		delete m_LimitedRoomInstances[i];
		m_LimitedRoomInstances[i] = 0;
	}	
	m_LimitedRoomInstances.clear();

	// delete extended room instances
	for( i=0; i < m_ExtendedRoomInstances.size(); ++i )
	{
		delete m_ExtendedRoomInstances[i];
		m_ExtendedRoomInstances[i] = 0;
	}	
	m_ExtendedRoomInstances.clear();

	// delete objects
	for( i=0; i < m_GameObjects.size(); ++i )
	{
		delete m_GameObjects[i];
		m_GameObjects[i] = 0;
	}
	m_GameObjects.clear();

	// clear out all lists
	m_PhysicsIdList.clear();
	m_EmitterList.clear();

	if( m_pBody != 0 )
	{
		if( physics::PhysicsWorldB2D::GetWorld() )
			physics::PhysicsWorldB2D::GetWorld()->DestroyBody(m_pBody);

		m_pBody = 0;
	}
	m_pGenericPhysicsId = 0;

	m_PlayerSpawnId = -1;

	// stop all audio
	AudioSystem::GetInstance()->StopAll();
	AudioSystem::GetInstance()->ClearOneShotList();
}

/////////////////////////////////////////////////////
/// Method: LoadMeshData
/// Params: [in]szFilename
///
/////////////////////////////////////////////////////
int Level::LoadMeshData( const char *szFilename, GLenum magFilter, GLenum minFilter )
{
	if( core::IsEmptyString( szFilename ) )
		return(1);

	m_pMeshData = new mdl::ModelHGM;
	DBG_ASSERT_MSG( (m_pMeshData != 0), "Could not allocate level HGM mesh memory" );

	m_MagFilter = magFilter;
	m_MinFilter = minFilter;

	ScriptDataHolder* pScriptData = GetScriptDataHolder();
	ScriptDataHolder::DevScriptData& DevData = pScriptData->GetDevData();
	if( DevData.smoothFiltering )
	{
		if( m_MagFilter == GL_NEAREST )
			m_MagFilter = GL_LINEAR;

		if( m_MinFilter == GL_NEAREST )
			m_MinFilter = GL_LINEAR;
		else if( (m_MinFilter == GL_NEAREST_MIPMAP_NEAREST) || 
				(m_MinFilter == GL_LINEAR_MIPMAP_NEAREST) || 
				(m_MinFilter == GL_NEAREST_MIPMAP_LINEAR) )
		{
			m_MinFilter = GL_LINEAR_MIPMAP_LINEAR;
		}
	}

	if( !DevData.levelMipmap )
	{
		// don't mipmap, match the min filter to the mag filter
		if( m_MagFilter == GL_NEAREST )
			m_MinFilter = GL_NEAREST;
		else
			m_MinFilter = GL_LINEAR;
	}

	m_pMeshData->Load( szFilename, m_MagFilter, m_MinFilter, core::app::GetLowPowerMode() );
	m_pMeshData->SetModelGeneralFlags( mdl::MODELFLAG_GENERAL_AUTOCULLING | mdl::MODELFLAG_GENERAL_AABBCULL | mdl::MODELFLAG_GENERAL_SUBMESHCULL /*| mdl::MODELFLAG_GENERAL_DRAWBOUNDINGBOX*/ );

	m_pMeshData->SetupAndFree();

	return(0);
}

/////////////////////////////////////////////////////
/// Method: LoadPhysicsB2DData
/// Params: [in]szFilename
/// 
/////////////////////////////////////////////////////
int Level::LoadPhysicsB2DData( const char *szFilename, const math::Vec3& pos, float angle, b2World* pWorld )
{
	file::TFileHandle fileHandle;
	file::TFile physFile;
	unsigned int i = 0, j = 0, k = 0;

	if( core::IsEmptyString( szFilename ) )
		return(1);

	if( file::FileOpen( szFilename, file::FILETYPE_BINARY_READ, &fileHandle ) == false )
	{
		DBGLOG( "LEVEL: *ERROR* Could not open %s file\n", szFilename );
		return(1);
	}

	b2Fixture* pFixture = 0;

	// create a generic body
	if( m_pBody == 0 )
	{
		b2BodyDef bd;
		bd.type = b2_staticBody;
		m_pBody = pWorld->CreateBody(&bd);
		DBG_ASSERT_MSG( (m_pBody != 0), "Could not create level generic holding body" );
		m_pBody->SetUserData( reinterpret_cast<void *>(this) );
	}

	if( m_pGenericPhysicsId == 0 )
	{
		// generic id
		m_pGenericPhysicsId = new physics::PhysicsIdentifier( PHYSICSBASICID_WORLD, PHYSICSCASTID_WORLD );
		DBG_ASSERT_MSG( (m_pGenericPhysicsId != 0), "Could not create level world physics identifier" );

		// add to the id list
		m_PhysicsIdList.push_back( m_pGenericPhysicsId );
	}

	unsigned int totalPhysicsBox = 0;
	file::FileReadUInt( &totalPhysicsBox, sizeof(unsigned int), 1, &fileHandle );

	unsigned int totalPhysicsCircle = 0;
	file::FileReadUInt( &totalPhysicsCircle, sizeof(unsigned int), 1, &fileHandle );

	unsigned int totalMeshes = 0;
	file::FileReadUInt( &totalMeshes, sizeof(unsigned int), 1, &fileHandle );

	unsigned int totalPhysicsObjects = 0;
	file::FileReadUInt( &totalPhysicsObjects, sizeof(unsigned int), 1, &fileHandle );

	for( i=0; i < totalPhysicsObjects; ++i )
	{
		int type=0;
		file::FileReadUInt( &type, sizeof(unsigned int), 1, &fileHandle );

		int numeric1 = -9999;
		int numeric2 = -9999;
		int numeric3 = -9999;
		int numeric4 = -9999;
		int numeric5 = -9999;

		unsigned short categoryBits = 0x0001;
		unsigned short maskBits = 0xFFFF;
		short groupIndex = 1;
		switch( type )
		{
			case B2DTYPE_BOX: // polyBox
				{
					b2PolygonShape boxDef;
					b2FixtureDef fd;

					file::FileReadFloat( &fd.friction, sizeof(float), 1, &fileHandle );
					file::FileReadFloat( &fd.restitution, sizeof(float), 1, &fileHandle );
					file::FileReadFloat( &fd.density, sizeof(float), 1, &fileHandle );

					file::FileReadUShort( &fd.filter.categoryBits, sizeof(unsigned short), 1, &fileHandle );
					file::FileReadUShort( &fd.filter.maskBits, sizeof(unsigned short), 1, &fileHandle );
					file::FileReadShort( &fd.filter.groupIndex, sizeof(short), 1, &fileHandle );

					file::FileReadFloat( &numeric1, sizeof(int), 1, &fileHandle );
					file::FileReadFloat( &numeric2, sizeof(int), 1, &fileHandle );
					file::FileReadFloat( &numeric3, sizeof(int), 1, &fileHandle );
					file::FileReadFloat( &numeric4, sizeof(int), 1, &fileHandle );
					file::FileReadFloat( &numeric5, sizeof(int), 1, &fileHandle );


					b2Vec2 center( 0.0f, 0.0f );
					file::FileReadFloat( &center, sizeof(float), 2, &fileHandle );
					float boxAngle = 0.0f;
					file::FileReadFloat( &boxAngle, sizeof(float), 1, &fileHandle );
					boxAngle += math::DegToRad(angle);

					float width = 1.0f;
					file::FileReadFloat( &width, sizeof(float), 1, &fileHandle );

					float height = 1.0f;
					file::FileReadFloat( &height, sizeof(float), 1, &fileHandle );

					// use position as an offset
					math::Vec3 newPos( center.x + pos.X, 0.0f, center.y + pos.Z );
								
					// rotate
					newPos = math::RotateAroundPoint( newPos, pos, math::DegToRad(angle) );

					center.x = newPos.X;
					center.y = newPos.Z;

					// filter based on the numeric
					if( numeric1 == NUMERIC1_WORLD_HIGHWALL )
						fd.filter.categoryBits = WORLDHIGH_CATEGORY;
					else if( numeric1 == NUMERIC1_WORLD_LOWWALL )
						fd.filter.categoryBits = WORLDLOW_CATEGORY;
					else
						fd.filter.categoryBits = WORLDHIGH_CATEGORY;

					fd.filter.maskBits = 0xFFFF;
					fd.filter.groupIndex = WORLDGROUP;

					boxDef.SetAsBox( width, height, center, boxAngle );

					fd.shape = &boxDef;

					pFixture = m_pBody->CreateFixture( &fd );
					DBG_ASSERT_MSG( (pFixture != 0), "Could not create level box fixture [object index %d]", i );

					if( numeric1 != -9999 ||
						numeric2 != -9999 ||
						numeric3 != -9999 ||
						numeric4 != -9999 ||
						numeric5 != -9999 )
					{

						physics::PhysicsIdentifier* pPhysicsId = 0;
						pPhysicsId = new physics::PhysicsIdentifier( PHYSICSBASICID_WORLD, PHYSICSCASTID_WORLD );
						DBG_ASSERT_MSG( (pPhysicsId != 0), "Could not create level box shape, physics identifier [object index %d]", i );

						pPhysicsId->SetNumeric1( numeric1 );
						pPhysicsId->SetNumeric2( numeric2 );
						pPhysicsId->SetNumeric3( numeric3 );
						pPhysicsId->SetNumeric4( numeric4 );
						pPhysicsId->SetNumeric5( numeric5 );

						// add to the id list
						m_PhysicsIdList.push_back( pPhysicsId );

						// tell the body to point user data to it
						pFixture->SetUserData( reinterpret_cast<void *>(pPhysicsId) );
					}
					else
					{
						pFixture->SetUserData( reinterpret_cast<void *>(m_pGenericPhysicsId) );
					}
				}break;
			case B2DTYPE_CIRCLE: // circle
				{
					b2CircleShape circleDef;
					b2FixtureDef fd;

					file::FileReadFloat( &fd.friction, sizeof(float), 1, &fileHandle );
					file::FileReadFloat( &fd.restitution, sizeof(float), 1, &fileHandle );
					file::FileReadFloat( &fd.density, sizeof(float), 1, &fileHandle );
					file::FileReadUShort( &fd.filter.categoryBits, sizeof(unsigned short), 1, &fileHandle );
					file::FileReadUShort( &fd.filter.maskBits, sizeof(unsigned short), 1, &fileHandle );
					file::FileReadShort( &fd.filter.groupIndex, sizeof(short), 1, &fileHandle );
					file::FileReadFloat( &numeric1, sizeof(int), 1, &fileHandle );
					file::FileReadFloat( &numeric2, sizeof(int), 1, &fileHandle );
					file::FileReadFloat( &numeric3, sizeof(int), 1, &fileHandle );
					file::FileReadFloat( &numeric4, sizeof(int), 1, &fileHandle );
					file::FileReadFloat( &numeric5, sizeof(int), 1, &fileHandle );

					b2Vec2 circlePos;
					file::FileReadFloat( &circleDef.m_p, sizeof(float), 2, &fileHandle );
								
					// use position as an offset
					math::Vec3 newPos( circleDef.m_p.x + pos.X, 0.0f, circleDef.m_p.y + pos.Z );
								
					// rotate
					newPos = math::RotateAroundPoint( newPos, pos, math::DegToRad(angle) );

					circleDef.m_p.x = newPos.X;
					circleDef.m_p.y = newPos.Z;

					file::FileReadFloat( &circleDef.m_radius, sizeof(float), 1, &fileHandle );

					// filter based on the numeric
					if( numeric1 == NUMERIC1_WORLD_HIGHWALL )
						fd.filter.categoryBits = WORLDHIGH_CATEGORY;
					else if( numeric1 == NUMERIC1_WORLD_LOWWALL )
						fd.filter.categoryBits = WORLDLOW_CATEGORY;
					else
						fd.filter.categoryBits = WORLDHIGH_CATEGORY;

					fd.filter.maskBits = 0xFFFF;
					fd.filter.groupIndex = WORLDGROUP;

					fd.shape = &circleDef;

					pFixture = m_pBody->CreateFixture( &fd );
					DBG_ASSERT_MSG( (pFixture != 0), "Could not create level circle fixture [object index %d]", i );

					if( numeric1 != -9999 ||
						numeric2 != -9999 ||
						numeric3 != -9999 ||
						numeric4 != -9999 ||
						numeric5 != -9999 )
					{

						physics::PhysicsIdentifier* pPhysicsId = 0;
						pPhysicsId = new physics::PhysicsIdentifier( PHYSICSBASICID_WORLD, PHYSICSCASTID_WORLD );
						DBG_ASSERT_MSG( (pPhysicsId != 0), "Could not create level circle fixture, physics identifier [object index %d]", i );

						pPhysicsId->SetNumeric1( numeric1 );
						pPhysicsId->SetNumeric2( numeric2 );
						pPhysicsId->SetNumeric3( numeric3 );
						pPhysicsId->SetNumeric4( numeric4 );
						pPhysicsId->SetNumeric5( numeric5 );

						// add to the id list
						m_PhysicsIdList.push_back( pPhysicsId );

						// tell the shape to point user data to it
						pFixture->SetUserData( reinterpret_cast<void *>(pPhysicsId) );
					}
					else
					{
						pFixture->SetUserData( reinterpret_cast<void *>(m_pGenericPhysicsId) );
					}
				}break;
			case B2DTYPE_MESH: // mesh
				{
					float friction = 1.0f;
					float restitution = 0.0f;
					float density = 0.0f;

					categoryBits = 0x0001;
					maskBits = 0xFFFF;
					groupIndex = 1;

					file::FileReadFloat( &friction, sizeof(float), 1, &fileHandle );
					file::FileReadFloat( &restitution, sizeof(float), 1, &fileHandle );
					file::FileReadFloat( &density, sizeof(float), 1, &fileHandle );
					file::FileReadUShort( &categoryBits, sizeof(unsigned short), 1, &fileHandle );
					file::FileReadUShort( &maskBits, sizeof(unsigned short), 1, &fileHandle );
					file::FileReadShort( &groupIndex, sizeof(short), 1, &fileHandle );
					file::FileReadFloat( &numeric1, sizeof(int), 1, &fileHandle );
					file::FileReadFloat( &numeric2, sizeof(int), 1, &fileHandle );
					file::FileReadFloat( &numeric3, sizeof(int), 1, &fileHandle );
					file::FileReadFloat( &numeric4, sizeof(int), 1, &fileHandle );
					file::FileReadFloat( &numeric5, sizeof(int), 1, &fileHandle );

					unsigned int totalMeshTriangles = 0;
					file::FileReadUInt( &totalMeshTriangles, sizeof(unsigned int), 1, &fileHandle );

					unsigned int polyMeshCount = 0;
					file::FileReadUInt( &polyMeshCount, sizeof(unsigned int), 1, &fileHandle );

					for( j = 0; j < polyMeshCount; ++j )
					{
						unsigned int polyCount = 0;
						file::FileReadUInt( &polyCount, sizeof(unsigned int), 1, &fileHandle );

						for( k = 0; k < polyCount; ++k )
						{
							unsigned int a = 0;
							unsigned int vertCount = 0;

							file::FileReadUInt( &vertCount, sizeof(unsigned int), 1, &fileHandle );
							DBG_ASSERT_MSG( (vertCount >= 3 && vertCount <= b2_maxPolygonVertices), "Level physics mesh has too many/few vertices [object index %d, polyMesh %d, poly %d]", i, j, k );

							b2PolygonShape polyDef;
							b2FixtureDef fd;
							b2Vec2 vertices[b2_maxPolygonVertices];

							fd.friction = friction;
							fd.restitution = restitution;
							fd.density = density;
							polyDef.m_count = vertCount;

							// filter based on the numeric
							if( numeric1 == NUMERIC1_WORLD_HIGHWALL )
								fd.filter.categoryBits = WORLDHIGH_CATEGORY;
							else if( numeric1 == NUMERIC1_WORLD_LOWWALL )
								fd.filter.categoryBits = WORLDLOW_CATEGORY;
							else
								fd.filter.categoryBits = WORLDHIGH_CATEGORY;

							fd.filter.maskBits = 0xFFFF;
							fd.filter.groupIndex = WORLDGROUP;

							for( a=0; a < vertCount; a++ )
							{
								file::FileReadFloat( &vertices[a], sizeof(float), 2, &fileHandle );

								// use position as an offset
								math::Vec3 vertPos( vertices[a].x + pos.X, 0.0f, vertices[a].y + pos.Z );
								
								// rotate
								vertPos = math::RotateAroundPoint( vertPos, pos, math::DegToRad(angle) );

								vertices[a].x = vertPos.X;
								vertices[a].y = vertPos.Z;
							}

							polyDef.Set( &vertices[0], vertCount );
							fd.shape = &polyDef;

							pFixture = m_pBody->CreateFixture( &fd );
							DBG_ASSERT_MSG( (pFixture != 0), "Could not create level mesh fixture [object index %d]", i );
				
							if( numeric1 != -9999 ||
								numeric2 != -9999 ||
								numeric3 != -9999 ||
								numeric4 != -9999 ||
								numeric5 != -9999 )
							{

								physics::PhysicsIdentifier* pPhysicsId = 0;
								pPhysicsId = new physics::PhysicsIdentifier( PHYSICSBASICID_WORLD, PHYSICSCASTID_WORLD );
								DBG_ASSERT_MSG( (pPhysicsId != 0), "Could not create level mesh, physics identifier [object index %d]", i );

								pPhysicsId->SetNumeric1( numeric1 );
								pPhysicsId->SetNumeric2( numeric2 );
								pPhysicsId->SetNumeric3( numeric3 );
								pPhysicsId->SetNumeric4( numeric4 );
								pPhysicsId->SetNumeric5( numeric5 );

								// add to the id list
								m_PhysicsIdList.push_back( pPhysicsId );

								// tell the shape to point user data to it
								pFixture->SetUserData( reinterpret_cast<void *>(pPhysicsId) );
							}
							else
							{
								pFixture->SetUserData( reinterpret_cast<void *>(m_pGenericPhysicsId) );
							}
						}
					}
				}break;

				case B2DTYPE_DISTANCEJOINT:
				case B2DTYPE_GEARJOINT:
				case B2DTYPE_MOUSEJOINT:
				case B2DTYPE_PRISMATICJOINT:
				case B2DTYPE_PULLEYJOINT:
				case B2DTYPE_REVOLUTEJOINT:
				break;

				default:
					DBG_ASSERT_MSG( 0, "Unknown physics type in level file" );
				break;
		}
	}

	file::FileClose( &fileHandle );

	return(0);
}

/////////////////////////////////////////////////////
/// Method: LoadAreas
/// Params: [in]szFilename
/// 
/////////////////////////////////////////////////////
int Level::LoadTypes( const char *szFilename, bool inRoomInstance, RoomInstance* roomInstance, const math::Vec3& pos, float angle  )
{
	file::TFileHandle fileHandle;
	file::TFile areaFile;
	int i = 0;
	renderer::Texture texLoad;
	snd::Sound sndLoad;

	if( core::IsEmptyString( szFilename ) )
		return(1);

	if( file::FileOpen( szFilename, file::FILETYPE_BINARY_READ, &fileHandle ) == false )
	{
		DBGLOG( "LEVEL: *ERROR* Could not open %s file\n", szFilename );
		return(1);
	}

	// read count
	int typeCount = 0;

	file::FileReadInt( &typeCount, sizeof( int ), 1, &fileHandle );

	for( i=0; i < typeCount; ++i )
	{
		// MEL saves float types as 8byte doubles
		int typeIndex=0;

		file::FileReadInt( &typeIndex, sizeof( int ), 1, &fileHandle );
		switch( typeIndex )
		{
			case POSROT:
			{
				int objectId = -1;
				math::Vec3 playerPos(0.0f,0.0f,0.0f);
				math::Vec3 rot(0.0f,0.0f,0.0f);

				file::FileReadInt( &objectId, sizeof( int ), 1, &fileHandle );

				file::FileReadDouble( &playerPos[0], sizeof(double), 3, &fileHandle );

				file::FileReadDouble( &rot.Y, sizeof(double), 1, &fileHandle );

				if( inRoomInstance )
				{
					// use position as an offset
					math::Vec3 newPos( playerPos.X + pos.X, playerPos.Y + pos.Y, playerPos.Z + pos.Z );
								
					// rotate
					newPos = math::RotateAroundPoint( newPos, pos, math::DegToRad(angle) );

					playerPos = newPos;
					rot.Y += angle;
				}

				BaseObject* pObj = new BaseObject( PHYSICSBASICID_UNKNOWN, PHYSICSCASTID_UNKNOWN, objectId, POSROT, script::LuaScripting::GetState() );
				DBG_ASSERT( (pObj != 0) );

				pObj->SetPos( playerPos );
				pObj->SetRot( rot.Y );

				m_GameObjects.push_back(pObj);

			}break;
			case USE_AREA:
			{
				int objectId = -1;
				collision::Sphere s;
				float useAngle;
				
				file::FileReadInt( &objectId, sizeof( int ), 1, &fileHandle );

				file::FileReadDouble( &s.vCenterPoint[0], sizeof(double), 3, &fileHandle );

				file::FileReadDouble( &s.fRadius, sizeof(double), 1, &fileHandle );

				file::FileReadDouble( &useAngle, sizeof(double), 1, &fileHandle );

				if( inRoomInstance )
				{
					// use position as an offset
					math::Vec3 newPos( s.vCenterPoint.X + pos.X, 0.0f, s.vCenterPoint.Z + pos.Z );
								
					// rotate
					newPos = math::RotateAroundPoint( newPos, pos, math::DegToRad(angle) );

					s.vCenterPoint.X = newPos.X;
					s.vCenterPoint.Z = newPos.Z;

					useAngle += angle;
				}

				UseArea* pObj = new UseArea( objectId, script::LuaScripting::GetState(), s, useAngle );
				DBG_ASSERT( (pObj != 0) );

				pObj->SetPos( s.vCenterPoint );
				pObj->SetRot( angle );

				m_UseAreas.push_back(pObj);
				m_GameObjects.push_back(pObj);
			}break;

			case DOOR:
			{
				int objectId = -1;
				int meshId = -1;
				int meshType = -1;
				int reflect = false;
				math::Vec3 doorPos(0.0f,0.0f,0.0f);
				math::Vec3 rot(0.0f,0.0f,0.0f);
				
				file::FileReadInt( &objectId, sizeof( int ), 1, &fileHandle );

				file::FileReadDouble( &doorPos[0], sizeof(double), 3, &fileHandle );

				file::FileReadDouble( &rot.Y, sizeof(double), 1, &fileHandle );

				file::FileReadInt( &meshId, sizeof( int ), 1, &fileHandle );
				file::FileReadInt( &meshType, sizeof( int ), 1, &fileHandle );
				file::FileReadInt( &reflect, sizeof( int ), 1, &fileHandle );

				if( inRoomInstance )
				{
					// use position as an offset
					math::Vec3 newPos( doorPos.X + pos.X, doorPos.Y + pos.Y, doorPos.Z + pos.Z );
								
					// rotate
					newPos = math::RotateAroundPoint( newPos, pos, math::DegToRad(angle) );

					doorPos = newPos;

					rot.Y += angle;
				}

				Door* pObj = new Door( objectId, meshId, meshType, (reflect != 0), roomInstance, script::LuaScripting::GetState() );
				DBG_ASSERT( (pObj != 0) );

				pObj->SetPos( doorPos );
				pObj->SetRot( rot.Y );

				m_Doors.push_back(pObj);
				m_GameObjects.push_back(pObj);
			}break;
			
			case LIGHT_ZONE:
			{
				int objectId = -1;
				
				file::FileReadInt( &objectId, sizeof( int ), 1, &fileHandle );

				LightZone *pObj = new LightZone;
				DBG_ASSERT( (pObj != 0) );

				file::FileReadDouble( &pObj->pos[0], sizeof(double), 3, &fileHandle );

				file::FileReadDouble( &pObj->angle, sizeof(double), 1, &fileHandle );

				if( inRoomInstance )
				{
					// use position as an offset
					math::Vec3 newPos( pObj->pos.X + pos.X, pObj->pos.Y + pos.Y, pObj->pos.Z + pos.Z );
								
					// rotate
					newPos = math::RotateAroundPoint( newPos, pos, math::DegToRad(angle) );

					pObj->pos = newPos;
				}

				file::FileReadDouble( &pObj->dims[0], sizeof(double), 3, &fileHandle );

				file::FileReadInt( &pObj->type, sizeof( int ), 1, &fileHandle );

				file::FileReadDouble( &pObj->dir[0], sizeof(double), 3, &fileHandle );

				if( pObj->type == 0 )
				{
					pObj->dir.W = 0.0f;
				}
				else
				{
					pObj->dir.X = pObj->pos.X + pObj->dir.X;
					pObj->dir.Y = pObj->pos.Y + pObj->dir.Y;
					pObj->dir.Z = pObj->pos.Z + pObj->dir.Z;

					pObj->dir.W = 1.0f;
				}

				int col[3];
				file::FileReadInt( &col[0], sizeof( int ), 1, &fileHandle );
				file::FileReadInt( &col[1], sizeof( int ), 1, &fileHandle );
				file::FileReadInt( &col[2], sizeof( int ), 1, &fileHandle );
				if( col[0] != 0.0f )
					pObj->col[0] =  static_cast<float>(col[0]) / 255.0f;
				else
					pObj->col[0] = 0.0f;

				if( col[1] != 0.0f )
					pObj->col[1] =  static_cast<float>(col[1]) / 255.0f;
				else
					pObj->col[1] = 0.0f;

				if( col[2] != 0.0f )
					pObj->col[2] =  static_cast<float>(col[2]) / 255.0f;
				else
					pObj->col[2] = 0.0f;

				if( pObj->angle == 0.0f )
				{
					pObj->aabb.vCenter = pObj->pos;
					pObj->aabb.vBoxMin.X = pObj->pos.X-(pObj->dims.Width*0.5f);
					pObj->aabb.vBoxMin.Y = pObj->pos.Y-(pObj->dims.Height*0.5f);
					pObj->aabb.vBoxMin.Z = pObj->pos.Z-(pObj->dims.Depth*0.5f);

					pObj->aabb.vBoxMax.X = pObj->pos.X+(pObj->dims.Width*0.5f);
					pObj->aabb.vBoxMax.Y = pObj->pos.Y+(pObj->dims.Height*0.5f);
					pObj->aabb.vBoxMax.Z = pObj->pos.Z+(pObj->dims.Depth*0.5f);
				}
				else
				{
					pObj->obb.vCenter = pObj->pos;
					pObj->obb.vHalfWidths.Width = (pObj->dims.Width*0.5f);
					pObj->obb.vHalfWidths.Height = (pObj->dims.Height*0.5f);
					pObj->obb.vHalfWidths.Depth = (pObj->dims.Depth*0.5f);

					math::Vec3 angles( 0.0f, math::DegToRad(-pObj->angle), 0.0f );
					math::Matrix4x4 mat;
					mat.SetRotationAngles( angles );

					pObj->obb.vAxis[0] = math::Vec3( mat.m11, mat.m12, mat.m13 );
					pObj->obb.vAxis[1] = math::Vec3( mat.m21, mat.m22, mat.m23 );
					pObj->obb.vAxis[2] = math::Vec3( mat.m31, mat.m32, mat.m33 );
				}

				m_LightZones.push_back(pObj);
			}break;

			case LIMITED_INSTANCE:
			{
				if( inRoomInstance )
				{
					DBGLOG( "*ERROR* LIMITED_INSTANCE type not allowed in a room instance\n" );
					DBG_ASSERT_MSG( 0, "*ERROR* LIMITED_INSTANCE type not allowed in a room instance" );
				}

				int objectId = -1;

				file::FileReadInt( &objectId, sizeof( int ), 1, &fileHandle );

				Instance *pObj = new Instance;
				DBG_ASSERT( (pObj != 0) );

				pObj->used = false;

				file::FileReadDouble( &pObj->pos[0], sizeof(double), 3, &fileHandle );

				file::FileReadDouble( &pObj->angle, sizeof(double), 1, &fileHandle );

				file::FileReadDouble( &pObj->scale[0], sizeof(double), 3, &fileHandle );

				m_LimitedInstances.push_back(pObj);

			}break;

			case EXTENDED_INSTANCE:
			{
				if( inRoomInstance )
				{
					DBGLOG( "*ERROR* EXTENDED_INSTANCE type not allowed in a room instance\n" );
					DBG_ASSERT_MSG( 0, "*ERROR* EXTENDED_INSTANCE type not allowed in a room instance" );
				}

				int objectId = -1;

				file::FileReadInt( &objectId, sizeof( int ), 1, &fileHandle );

				Instance *pObj = new Instance;
				DBG_ASSERT( (pObj != 0) );

				pObj->used = false;

				file::FileReadDouble( &pObj->pos[0], sizeof(double), 3, &fileHandle );

				file::FileReadDouble( &pObj->angle, sizeof(double), 1, &fileHandle );

				file::FileReadDouble( &pObj->scale[0], sizeof(double), 3, &fileHandle );

				m_ExtendedInstances.push_back(pObj);

			}break;

			case ANIMATED_MESH:
			{
				int objectId = -1;
				int meshId = -1;
				math::Vec3 objPos(0.0f,0.0f,0.0f);
				math::Vec3 rot(0.0f,0.0f,0.0f);

				file::FileReadInt( &objectId, sizeof( int ), 1, &fileHandle );

				file::FileReadDouble( &objPos[0], sizeof(double), 3, &fileHandle );

				file::FileReadDouble( &rot[0], sizeof(double), 3, &fileHandle );

				file::FileReadInt( &meshId, sizeof( int ), 1, &fileHandle );

				if( inRoomInstance )
				{
					// use position as an offset
					math::Vec3 newPos( objPos.X + pos.X, objPos.Y + pos.Y, objPos.Z + pos.Z );
								
					// rotate
					newPos = math::RotateAroundPoint( newPos, pos, math::DegToRad(angle) );

					objPos = newPos;

					//rot.Y += angle;
				}

				AnimatedMesh* pObj = new AnimatedMesh( objectId, rot, meshId, script::LuaScripting::GetState() );
				DBG_ASSERT( (pObj != 0) );

				pObj->SetPos( objPos );
				//pObj->SetRot( rot.Y );

				m_AnimatedMeshes.push_back(pObj);
				m_GameObjects.push_back(pObj);

			}break;

			case SPRITE:
			{
				int objectId = -1;

				file::FileReadInt( &objectId, sizeof( int ), 1, &fileHandle );

				SpriteType* newSprite = new SpriteType;
				DBG_ASSERT( (newSprite != 0) );

				file::FileReadDouble( &newSprite->pos[0], sizeof(double), 3, &fileHandle );

				file::FileReadDouble( &newSprite->angle, sizeof(double), 1, &fileHandle );
				
				if( inRoomInstance )
				{
					// use position as an offset
					math::Vec3 newPos( newSprite->pos.X + pos.X, newSprite->pos.Y + pos.Y, newSprite->pos.Z + pos.Z );
								
					// rotate
					newPos = math::RotateAroundPoint( newPos, pos, math::DegToRad(angle) );

					newSprite->pos = newPos;

					newSprite->angle += angle;
				}
				
				newSprite->angle = math::DegToRad(newSprite->angle);

				file::FileReadInt( &newSprite->Id, sizeof( int ), 1, &fileHandle );
				file::FileReadInt( &newSprite->type, sizeof( int ), 1, &fileHandle );

				file::FileReadDouble( &newSprite->w, sizeof(double), 1, &fileHandle );
				file::FileReadDouble( &newSprite->h, sizeof(double), 1, &fileHandle );
				file::FileReadDouble( &newSprite->d, sizeof(double), 1, &fileHandle );

				int col[4];
				file::FileReadInt( &col[0], sizeof( int ), 1, &fileHandle );
				file::FileReadInt( &col[1], sizeof( int ), 1, &fileHandle );
				file::FileReadInt( &col[2], sizeof( int ), 1, &fileHandle );
				file::FileReadInt( &col[3], sizeof( int ), 1, &fileHandle );
					
				newSprite->col.R = static_cast<unsigned char>(col[0]);
				newSprite->col.G = static_cast<unsigned char>(col[1]);
				newSprite->col.B = static_cast<unsigned char>(col[2]);
				newSprite->col.A = static_cast<unsigned char>(col[3]);

				if( H4::GetHiResMode() )
				{
					const res::SpriteResourceStore* sr = res::GetSpriteResource( newSprite->Id );
					if( sr && m_SpriteTextureId == renderer::INVALID_OBJECT )
					{
						const res::TextureResourceStore* rs = 0;
						rs = res::GetTextureResource( sr->textureResId );
						DBG_ASSERT( rs != 0 );

						m_SpriteTextureId = rs->texId; 
					}
				}
				else
				{
					const res::SpriteResourceStore* sr = res::GetSpriteResource( newSprite->Id );
					if( sr && m_SpriteTextureId == renderer::INVALID_OBJECT )
					{
						renderer::Texture texLoad;
						m_SpriteTextureId = renderer::TextureLoad( "textures/fx.png", texLoad, renderer::TEXTURE_PNG, 0, false, sr->magFilter, sr->minFilter );
					}
				}

				m_SpriteData.push_back( newSprite );

			}break;

			case EMITTER:
			{
				int objectId = -1;
				int emitterId = -1;
				int startMode = 0;
				float timeOn = -1.0f;
				float timeOff = -1.0f;

				math::Vec3 emitPos(0.0f,0.0f,0.0f);
				math::Vec3 rot(0.0f,0.0f,0.0f);

				file::FileReadInt( &objectId, sizeof( int ), 1, &fileHandle );

				file::FileReadDouble( &emitPos[0], sizeof(double), 3, &fileHandle );

				file::FileReadDouble( &rot.Y, sizeof(double), 1, &fileHandle );

				if( inRoomInstance )
				{
					// use position as an offset
					math::Vec3 newPos( emitPos.X + pos.X, emitPos.Y + pos.Y, emitPos.Z + pos.Z );
								
					// rotate
					newPos = math::RotateAroundPoint( newPos, pos, math::DegToRad(angle) );

					emitPos = newPos;
				}

				file::FileReadInt( &emitterId, sizeof( int ), 1, &fileHandle );
				file::FileReadInt( &startMode, sizeof( int ), 1, &fileHandle );

				file::FileReadDouble( &timeOn, sizeof(double), 1, &fileHandle );
				file::FileReadDouble( &timeOff, sizeof(double), 1, &fileHandle );

				// lookup the emitter block
				const res::EmitterResourceStore* er = 0;
				er = res::GetEmitterResource( emitterId );
				if( er )
				{
					efx::Emitter* pEmitter = 0;
					pEmitter = new efx::Emitter;
					DBG_ASSERT( (pEmitter != 0) );

					pEmitter->Create( *er->block );
					pEmitter->SetPos( emitPos );
					pEmitter->SetRot( rot );

					// start off
					if( startMode == 0 )
					{
						pEmitter->Disable();
						if( timeOff > -1.0f )
							pEmitter->SetTimeToWait(timeOff);
					}
					else
					{
						pEmitter->Enable();
						if( timeOff > -1.0f )
							pEmitter->SetTimeToWait(timeOn);
					}

					pEmitter->SetTimeOn( timeOn );
					pEmitter->SetTimeOff( timeOff );

					// grab the sprite resource
					res::SetupTexturesOnEmitter( pEmitter );

					m_EmitterList.push_back(pEmitter);

					BaseObject* pObj = new BaseObject( PHYSICSBASICID_UNKNOWN, PHYSICSCASTID_UNKNOWN, objectId, EMITTER, script::LuaScripting::GetState(), reinterpret_cast<void*>(pEmitter) );
					DBG_ASSERT( (pObj != 0) );
					pObj->SetPos( emitPos );

					m_GameObjects.push_back(pObj);
				}
			}break;

			case SOUND:
			{
				int objectId = -1;
				int soundId = -1;
				int type = 0;
				float time = 0.0f;
				math::Vec3 soundPos(0.0f,0.0f,0.0f);
				math::Vec3 soundOffset(0.0f,0.0f,0.0f);
				float soundAngle = 0.0f;
				float pitch = 1.0f;
				float gain = 1.0f;
				int relative = false;
				int loop = false;
				int areaType = 0;
				math::Vec3 dims(1.0f,1.0f,1.0f);
				float radius;

				file::FileReadInt( &objectId, sizeof( int ), 1, &fileHandle );

				file::FileReadDouble( &soundPos[0], sizeof(double), 3, &fileHandle );

				file::FileReadDouble( &soundAngle, sizeof(double), 1, &fileHandle );

				file::FileReadInt( &soundId, sizeof( int ), 1, &fileHandle );
				file::FileReadInt( &type, sizeof( int ), 1, &fileHandle );

				file::FileReadDouble( &time, sizeof(double), 1, &fileHandle );

				file::FileReadDouble( &soundOffset[0], sizeof(double), 3, &fileHandle );

				file::FileReadDouble( &pitch, sizeof(double), 1, &fileHandle );
				file::FileReadDouble( &gain, sizeof(double), 1, &fileHandle );

				file::FileReadInt( &relative, sizeof( int ), 1, &fileHandle );
				file::FileReadInt( &loop, sizeof( int ), 1, &fileHandle );

				file::FileReadInt( &areaType, sizeof( int ), 1, &fileHandle );

				file::FileReadDouble( &dims[0], sizeof(double), 3, &fileHandle );

				file::FileReadDouble( &radius, sizeof(double), 1, &fileHandle );

				if( inRoomInstance )
				{
					// use position as an offset
					math::Vec3 newPos( soundPos.X + pos.X, soundPos.Y + pos.Y, soundPos.Z + pos.Z );
								
					// rotate
					newPos = math::RotateAroundPoint( newPos, pos, math::DegToRad(angle) );

					soundPos = newPos;
				}

				SoundType* newSound = new SoundType;
				DBG_ASSERT( (newSound != 0) );

				newSound->sourceId = snd::INVALID_SOUNDSOURCE;
				newSound->type = type;
				newSound->time = time;
				newSound->lastTimeUpdate = 0.0f;

				const char* sr = 0;
				sr = res::GetSoundResource(soundId);
				newSound->bufferId = snd::SoundLoad( sr, sndLoad );

				newSound->pos = soundPos;
				if( relative )
					newSound->offset = soundOffset;
				else
					newSound->offset = soundPos+soundOffset;

				newSound->pitch = pitch;
				newSound->gain = gain;
#ifdef BASE_PLATFORM_iOS
				newSound->gain = 2.0f;
#endif
				newSound->srcRel = ( relative != 0 );
				newSound->loop = ( loop != 0 );
				newSound->angle = soundAngle;

				newSound->areaType = areaType;
				if( areaType == 0 )
				{
					if( soundAngle == 0.0f )
					{
						newSound->aabb.vCenter = soundPos;
						newSound->aabb.vBoxMin.X = soundPos.X-(dims.Width*0.5f);
						newSound->aabb.vBoxMin.Y = soundPos.Y-(dims.Height*0.5f);
						newSound->aabb.vBoxMin.Z = soundPos.Z-(dims.Depth*0.5f);

						newSound->aabb.vBoxMax.X = soundPos.X+(dims.Width*0.5f);
						newSound->aabb.vBoxMax.Y = soundPos.Y+(dims.Height*0.5f);
						newSound->aabb.vBoxMax.Z = soundPos.Z+(dims.Depth*0.5f);
					}
					else
					{
						newSound->obb.vCenter = soundPos;
						newSound->obb.vHalfWidths.Width = (dims.Width*0.5f);
						newSound->obb.vHalfWidths.Height = (dims.Height*0.5f);
						newSound->obb.vHalfWidths.Depth = (dims.Depth*0.5f);

						math::Vec3 angles( 0.0f, math::DegToRad(-soundAngle), 0.0f );
						math::Matrix4x4 mat;
						mat.SetRotationAngles( angles );

						newSound->obb.vAxis[0] = math::Vec3( mat.m11, mat.m12, mat.m13 );
						newSound->obb.vAxis[1] = math::Vec3( mat.m21, mat.m22, mat.m23 );
						newSound->obb.vAxis[2] = math::Vec3( mat.m31, mat.m32, mat.m33 );
					}
				}
				else
				{
					newSound->sphere.vCenterPoint = soundPos;
					newSound->sphere.fRadius = radius;
				}

				m_SoundLocations.push_back(newSound);

			}break;

			case ENEMY_SPAWN:
			{
				int objectId = -1;
				math::Vec3 enemyPos(0.0f,0.0f,0.0f);
				math::Vec3 rot(0.0f,0.0f,0.0f);

				file::FileReadInt( &objectId, sizeof( int ), 1, &fileHandle );

				file::FileReadDouble( &enemyPos[0], sizeof(double), 3, &fileHandle );

				file::FileReadDouble( &rot.Y, sizeof(double), 1, &fileHandle );

				if( inRoomInstance )
				{
					// use position as an offset
					math::Vec3 newPos( enemyPos.X + pos.X, enemyPos.Y + pos.Y, enemyPos.Z + pos.Z );
								
					// rotate
					newPos = math::RotateAroundPoint( newPos, pos, math::DegToRad(angle) );

					enemyPos = newPos;
					rot.Y += angle;
				}			

				int type = 0;
				file::FileReadInt( &type, sizeof( int ), 1, &fileHandle );

				EnemyManagement& manager = GameSystems::GetInstance()->GetEnemyManager();

				if( type == ENEMY_SPAWNTYPE_STATIC )
				{
					// add to the enemy management
					EnemyManagement::EnemySpawn spawnPoint;
					spawnPoint.spawnId = objectId;
					spawnPoint.enemyPlaced = false;
					spawnPoint.pos = enemyPos;
					spawnPoint.angle = rot.Y;
					
					manager.AddEnemySpawn(spawnPoint);
				}
				else if( type == ENEMY_SPAWNTYPE_CONSTANT )
				{
					// add to the enemy management
					EnemyManagement::ConstantSpawn spawnPoint;
					spawnPoint.spawnId = objectId;
					spawnPoint.enabled = false;
					spawnPoint.cooldownTimer = 0.0f;
					spawnPoint.pos = enemyPos;
					spawnPoint.angle = rot.Y;
					
					manager.AddConstantSpawn(spawnPoint);
				}
				else if( type == ENEMY_SPAWNTYPE_FLOATINGHEAD )
				{
					// add to the enemy management
					EnemyManagement::EnemySpawn spawnPoint;
					spawnPoint.spawnId = objectId;
					spawnPoint.enemyPlaced = false;
					spawnPoint.pos = enemyPos;
					spawnPoint.angle = rot.Y;
					
					manager.AddFloatingHeadSpawn(spawnPoint);
				}
				else if( type == ENEMY_SPAWNTYPE_HANGINGSPIDER )
				{
					// add to the enemy management
					EnemyManagement::EnemySpawn spawnPoint;
					spawnPoint.spawnId = objectId;
					spawnPoint.enemyPlaced = false;
					spawnPoint.pos = enemyPos;
					spawnPoint.angle = rot.Y;
					
					manager.AddHangingSpiderSpawn(spawnPoint);
				}
				else if( type == ENEMY_SPAWNTYPE_CRAWLINGSPIDER )
				{
					// add to the enemy management
					EnemyManagement::EnemySpawn spawnPoint;
					spawnPoint.spawnId = objectId;
					spawnPoint.enemyPlaced = false;
					spawnPoint.pos = enemyPos;
					spawnPoint.angle = rot.Y;
					
					manager.AddCrawlingSpiderSpawn(spawnPoint);
				}
				else if( type == ENEMY_SPAWNTYPE_ANTINSECTS )
				{
					// add to the enemy management
					enemyPos.Y += 0.01f;

					EnemyManagement::EnemySpawn spawnPoint;
					spawnPoint.spawnId = objectId;
					spawnPoint.enemyPlaced = false;
					spawnPoint.pos = enemyPos;
					spawnPoint.angle = rot.Y;

					SpriteType* newSprite = new SpriteType;
					DBG_ASSERT( (newSprite != 0) );

					newSprite->pos = enemyPos;
					newSprite->angle = 0.0f;
					
					if( inRoomInstance )
					{
						// use position as an offset
						math::Vec3 newPos( newSprite->pos.X + enemyPos.X, newSprite->pos.Y + enemyPos.Y, newSprite->pos.Z + enemyPos.Z );
									
						// rotate
						newPos = math::RotateAroundPoint( newPos, pos, math::DegToRad(spawnPoint.angle) );

						newSprite->pos = newPos;

						newSprite->angle += angle;
					}
					
					newSprite->angle = math::DegToRad(newSprite->angle);

					newSprite->Id = 44;
					newSprite->type = 2; // Y rot only

					newSprite->w = 0.5f;
					newSprite->h = 0.0f;
					newSprite->d = 0.5f;
						
					newSprite->col.R = 128;
					newSprite->col.G = 128;
					newSprite->col.B = 128;
					newSprite->col.A = 255;

					//const res::SpriteResourceStore* sr = res::GetSpriteResource( newSprite->Id );
					//if( sr && m_SpriteTextureId == renderer::INVALID_OBJECT )
					//{
					//	m_SpriteTextureId = res::LoadTextureName( sr->texFile, false, sr->magFilter, sr->minFilter ); 
					//}

					m_SpriteData.push_back( newSprite );
					
					manager.AddInsectSpawn(spawnPoint);

					// lookup the emitter block
					const res::EmitterResourceStore* er = 0;
					er = res::GetEmitterResource( 23 );
					if( er )
					{
						efx::Emitter* pEmitter = 0;
						pEmitter = new efx::Emitter;
						DBG_ASSERT( (pEmitter != 0) );

						pEmitter->Create( *er->block );
						pEmitter->SetPos( enemyPos );
						pEmitter->SetRot( zeroVec );

						// start on
						pEmitter->Enable();
						pEmitter->SetTimeToWait(-1.0f);

						pEmitter->SetTimeOn( -1.0f );
						pEmitter->SetTimeOff( -1.0f );

						// grab the sprite resource
						res::SetupTexturesOnEmitter( pEmitter );

						m_EmitterList.push_back(pEmitter);

						BaseObject* pObj = new BaseObject( PHYSICSBASICID_UNKNOWN, PHYSICSCASTID_UNKNOWN, objectId, EMITTER, script::LuaScripting::GetState(), reinterpret_cast<void*>(pEmitter) );
						DBG_ASSERT( (pObj != 0) );
						pObj->SetPos( enemyPos );

						m_GameObjects.push_back(pObj);
					}

				}
				else if( type == ENEMY_SPAWNTYPE_COCKROACHINSECTS )
				{
					// add to the enemy management
					EnemyManagement::EnemySpawn spawnPoint;
					spawnPoint.spawnId = objectId;
					spawnPoint.enemyPlaced = false;
					spawnPoint.pos = enemyPos;
					spawnPoint.angle = rot.Y;
					
					manager.AddInsectSpawn(spawnPoint);
				}

			}break;

			case STEP_AUDIO:
			{
				int objectId = -1;
				
				file::FileReadInt( &objectId, sizeof( int ), 1, &fileHandle );

				StepAudio *pObj = new StepAudio;
				DBG_ASSERT( (pObj != 0) );

				file::FileReadDouble( &pObj->pos[0], sizeof(double), 3, &fileHandle );

				file::FileReadDouble( &pObj->angle, sizeof(double), 1, &fileHandle );

				if( inRoomInstance )
				{
					// use position as an offset
					math::Vec3 newPos( pObj->pos.X + pos.X, pObj->pos.Y + pos.Y, pObj->pos.Z + pos.Z );
								
					// rotate
					newPos = math::RotateAroundPoint( newPos, pos, math::DegToRad(angle) );

					pObj->pos = newPos;
				}

				file::FileReadInt( &pObj->type, sizeof( int ), 1, &fileHandle );

				file::FileReadDouble( &pObj->dims[0], sizeof(double), 3, &fileHandle );

				if( pObj->angle == 0.0f )
				{
					pObj->aabb.vCenter = pObj->pos;
					pObj->aabb.vBoxMin.X = pObj->pos.X-(pObj->dims.Width*0.5f);
					pObj->aabb.vBoxMin.Y = pObj->pos.Y-(pObj->dims.Height*0.5f);
					pObj->aabb.vBoxMin.Z = pObj->pos.Z-(pObj->dims.Depth*0.5f);

					pObj->aabb.vBoxMax.X = pObj->pos.X+(pObj->dims.Width*0.5f);
					pObj->aabb.vBoxMax.Y = pObj->pos.Y+(pObj->dims.Height*0.5f);
					pObj->aabb.vBoxMax.Z = pObj->pos.Z+(pObj->dims.Depth*0.5f);
				}
				else
				{
					pObj->obb.vCenter = pObj->pos;
					pObj->obb.vHalfWidths.Width = (pObj->dims.Width*0.5f);
					pObj->obb.vHalfWidths.Height = (pObj->dims.Height*0.5f);
					pObj->obb.vHalfWidths.Depth = (pObj->dims.Depth*0.5f);

					math::Vec3 angles( 0.0f, math::DegToRad(-pObj->angle), 0.0f );
					math::Matrix4x4 mat;
					mat.SetRotationAngles( angles );

					pObj->obb.vAxis[0] = math::Vec3( mat.m11, mat.m12, mat.m13 );
					pObj->obb.vAxis[1] = math::Vec3( mat.m21, mat.m22, mat.m23 );
					pObj->obb.vAxis[2] = math::Vec3( mat.m31, mat.m32, mat.m33 );
				}

				m_StepAudioZones.push_back(pObj);
			}break;

			default:
				DBG_ASSERT_MSG( 0, "Unknown level type found" );
				break;
		}
	}

	file::FileClose( &fileHandle );

	return(0);
}

/////////////////////////////////////////////////////
/// Method: LoadMeshData
/// Params: [in]szFilename
///
/////////////////////////////////////////////////////
int Level::LoadSkybox( int index )
{
	m_pSkyData = res::LoadModel( index );
	DBG_ASSERT_MSG( (m_pSkyData != 0), "Could not allocate level HGM mesh memory" );

	m_pSkyData->SetupAndFree();

	return(0);
}

/////////////////////////////////////////////////////
/// Method: CallOnCreate
/// Params: 
/// 
/////////////////////////////////////////////////////
void Level::CallOnCreate()
{
	// call the level setup
	if( script::LuaFunctionCheck( "LevelSetup" ) == 0 )
	{
		int err = 0;
		int errorFuncIndex;
		errorFuncIndex = script::GetErrorFuncIndex();

		// function exists call it
		lua_getglobal( script::LuaScripting::GetState(), "LevelSetup" );
		lua_pushnumber( script::LuaScripting::GetState(), m_PlayerSpawnId );

		lua_pcall( script::LuaScripting::GetState(), 1, 0, errorFuncIndex );

		// LUA_ERRRUN --- a runtime error. 
		// LUA_ERRMEM --- memory allocation error. For such errors, Lua does not call the error handler function. 
		// LUA_ERRERR --- error while running the error handler function. 

		if( err == LUA_ERRRUN || err == LUA_ERRMEM || err == LUA_ERRERR )
		{
			DBGLOG( "LUASCRIPTING: *ERROR* Calling script '%s' failed\n", "LevelSetup" );
			DBGLOG( "\tLUA_TRACEBACK: %s\n", lua_tostring( script::LuaScripting::GetState(), -1 ) );

			script::StackDump(script::LuaScripting::GetState());

			DBG_ASSERT_MSG( 0, "LUASCRIPTING: *ERROR* Calling script '%s' failed", "LevelSetup" );
		}
	}
	else
	{
		DBG_ASSERT( 0 );
	}

	std::vector<BaseObject *>::iterator it = m_GameObjects.begin();
	while( it != m_GameObjects.end() )
	{
		(*it)->PostConstruction();

		it++;
	}

	it = m_GameObjects.begin();
	while( it != m_GameObjects.end() )
	{
		(*it)->OnCreate();

		it++;
	}

	// call the level setup
	if( script::LuaFunctionCheck( "LogicSetup" ) == 0 )
	{
		int err = 0;
		int errorFuncIndex;
		errorFuncIndex = script::GetErrorFuncIndex();

		// function exists call it
		lua_getglobal( script::LuaScripting::GetState(), "LogicSetup" );
		lua_pcall( script::LuaScripting::GetState(), 0, 0, errorFuncIndex );

		// LUA_ERRRUN --- a runtime error. 
		// LUA_ERRMEM --- memory allocation error. For such errors, Lua does not call the error handler function. 
		// LUA_ERRERR --- error while running the error handler function. 

		if( err == LUA_ERRRUN || err == LUA_ERRMEM || err == LUA_ERRERR )
		{
			DBGLOG( "LUASCRIPTING: *ERROR* Calling script '%s' failed\n", "LogicSetup" );
			DBGLOG( "\tLUA_TRACEBACK: %s\n", lua_tostring( script::LuaScripting::GetState(), -1 ) );

			script::StackDump(script::LuaScripting::GetState());

			DBG_ASSERT_MSG( 0, "LUASCRIPTING: *ERROR* Calling script '%s' failed", "LogicSetup" );
		}
	}
	else
	{
		DBG_ASSERT( 0 );
	}

	// create the sprite list
	if( m_SpriteData.size() )
	{
		m_SpriteCount = static_cast<unsigned int>( m_SpriteData.size()*6 );

		m_SpriteList = new SpriteVert[m_SpriteCount];
	}
}

/////////////////////////////////////////////////////
/// Method: DrawSkybox
/// Params: 
/// 
/////////////////////////////////////////////////////
void Level::DrawSkybox( )
{
	renderer::OpenGL::GetInstance()->SetColour4ub( 255,255,255,255 );
	if( m_pSkyData != 0 )
		m_pSkyData->Draw();
}

/////////////////////////////////////////////////////
/// Method: DrawOpaque
/// Params: [in]disableMeshDraw
/// 
/////////////////////////////////////////////////////
void Level::DrawOpaque( bool disableMeshDraw )
{
	renderer::OpenGL::GetInstance()->SetColour4ub( 255, 255, 255, 255 );
	
/*	static float timer = 0.0f;
	static float lastTime = math::RandomNumber( 5.0f, 10.0f ); 
	static bool swap = true;

	timer += m_LastDelta;
	if( timer >= lastTime )
	{
		if( swap )
		{
			// stay bright
			lastTime = lastTime+0.1f;
			swap = false;
			
			renderer::OpenGL::GetInstance()->SetAmbientColour( 1.0f, 1.0f, 1.0f, 1.0f );
			renderer::OpenGL::GetInstance()->SetColour4ub( 255, 255, 255, 255 );
		}
		else
		{
			timer = 0.0f;
			lastTime = math::RandomNumber( 5.0f, 10.0f );
			swap = true;
		}
	}
	else
	{
		if( swap )
		{
			renderer::OpenGL::GetInstance()->SetAmbientColour( 0.2f, 0.2f, 0.2f, 1.0f );
			renderer::OpenGL::GetInstance()->SetColour4ub( 64, 64, 64, 255 );
		}
	}
*/

/*
	std::vector<RoomInstance *>::iterator itRooms = m_LimitedRoomInstances.begin();
	while( itRooms != m_LimitedRoomInstances.end() )
	{
		(*itRooms)->Draw( false );

		itRooms++;
	}

	std::vector<RoomInstance *>::iterator itRoomsExt = m_ExtendedRoomInstances.begin();
	while( itRoomsExt != m_ExtendedRoomInstances.end() )
	{
		(*itRoomsExt)->Draw( false );

		itRoomsExt++;
	}
*/

	std::vector<Door *>::iterator itDoors = m_Doors.begin();
	while( itDoors != m_Doors.end() )
	{
		(*itDoors)->Draw();

		itDoors++;
	}

	// draw static opaque world meshes
	if( m_pMeshData != 0 && !disableMeshDraw )
		m_pMeshData->Draw( false );

	std::vector<AnimatedMesh *>::iterator itAnimated = m_AnimatedMeshes.begin();
	while( itAnimated != m_AnimatedMeshes.end() )
	{
		(*itAnimated)->Draw();

		itAnimated++;
	}
}

/////////////////////////////////////////////////////
/// Method: DrawSorted
/// Params: [in]disableMeshDraw
/// 
/////////////////////////////////////////////////////
void Level::DrawSorted( bool disableMeshDraw )
{
	renderer::OpenGL::GetInstance()->SetColour4ub( 255, 255, 255, 255 );

/*	std::vector<RoomInstance *>::iterator itRooms = m_LimitedRoomInstances.begin();
	while( itRooms != m_LimitedRoomInstances.end() )
	{
		(*itRooms)->Draw( true );

		itRooms++;
	}

	std::vector<RoomInstance *>::iterator itRoomsExt = m_ExtendedRoomInstances.begin();
	while( itRoomsExt != m_ExtendedRoomInstances.end() )
	{
		(*itRoomsExt)->Draw( true );

		itRoomsExt++;
	}
*/
	// draw static transparent world meshes
	if( m_pMeshData != 0 && !disableMeshDraw )
		m_pMeshData->Draw( true );

	renderer::OpenGL::GetInstance()->DepthMode( false, GL_LEQUAL );
	if( m_SpriteCount )
	{
		renderer::OpenGL::GetInstance()->EnableTextureArray();
		renderer::OpenGL::GetInstance()->EnableColourArray();

		renderer::OpenGL::GetInstance()->BindTexture( m_SpriteTextureId );

		
		renderer::OpenGL::GetInstance()->SetCullState( false, GL_BACK );

		glVertexPointer( 3, GL_FLOAT, sizeof(SpriteVert), &m_SpriteList[0].v );
		glTexCoordPointer( 2, GL_FLOAT, sizeof(SpriteVert), &m_SpriteList[0].uv );
		glColorPointer( 4, GL_UNSIGNED_BYTE, sizeof(SpriteVert), &m_SpriteList[0].col );

		glDrawArrays( GL_TRIANGLES, 0, m_SpriteCount );

		
		renderer::OpenGL::GetInstance()->SetCullState( true, GL_BACK );

		renderer::OpenGL::GetInstance()->DisableColourArray();
		renderer::OpenGL::GetInstance()->DisableTextureArray();
	}	
	renderer::OpenGL::GetInstance()->DepthMode( true, GL_LESS );


}

/////////////////////////////////////////////////////
/// Method: DrawEmitters
/// Params: None
/// 
/////////////////////////////////////////////////////
void Level::DrawEmitters( )
{
	std::vector<efx::Emitter *>::iterator itEmitters = m_EmitterList.begin();
	while( itEmitters != m_EmitterList.end() )
	{
		(*itEmitters)->Draw();

		itEmitters++;
	}
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
	unsigned int i=0, j=0;

	m_LastDelta = deltaTime;
	
	// draw static world meshes
	if( m_pMeshData != 0 )
		m_pMeshData->Update( deltaTime, true );

	std::vector<AnimatedMesh *>::iterator itAnimated = m_AnimatedMeshes.begin();
	while( itAnimated != m_AnimatedMeshes.end() )
	{
		(*itAnimated)->Update( deltaTime );

		itAnimated++;
	}

	std::vector<Door *>::iterator itDoors = m_Doors.begin();
	while( itDoors != m_Doors.end() )
	{
		(*itDoors)->Update( deltaTime );

		itDoors++;
	}

	std::vector<efx::Emitter *>::iterator itEmitters = m_EmitterList.begin();
	while( itEmitters != m_EmitterList.end() )
	{
		(*itEmitters)->Update( deltaTime );

		itEmitters++;
	}

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

	math::Vec3 axis( 0.0f, 0.0f, 1.0f );
	for( i=0; i<m_SpriteData.size(); ++i )
	{
		SpriteType *s = m_SpriteData[i];
		s->sceneDepth = std::abs( math::DotProduct( (s->pos - vEye), axis ) );
	}
	// sort the sprites
	std::sort( m_SpriteData.begin(), m_SpriteData.end(), SpriteSort );

	for( i=0, j=0; i < m_SpriteCount; i+=6, j++ )
	{
		SpriteType *s = m_SpriteData[j];
		const res::SpriteResourceStore* sr = res::GetSpriteResource( s->Id );

		m_SpriteList[i].uv		= math::Vec2( sr->uOffset,			sr->vOffset );
		m_SpriteList[i+1].uv	= math::Vec2( sr->uOffset + sr->w,	sr->vOffset );
		m_SpriteList[i+2].uv	= math::Vec2( sr->uOffset,			sr->vOffset + sr->h );
		m_SpriteList[i+3].uv	= math::Vec2( sr->uOffset + sr->w,	sr->vOffset );
		m_SpriteList[i+4].uv	= math::Vec2( sr->uOffset + sr->w,	sr->vOffset + sr->h );
		m_SpriteList[i+5].uv	= math::Vec2( sr->uOffset,			sr->vOffset + sr->h );

		m_SpriteList[i].col		= math::Vec4Lite( s->col.R, s->col.G, s->col.B, s->col.A );
		m_SpriteList[i+1].col	= math::Vec4Lite( s->col.R, s->col.G, s->col.B, s->col.A );
		m_SpriteList[i+2].col	= math::Vec4Lite( s->col.R, s->col.G, s->col.B, s->col.A );
		m_SpriteList[i+3].col	= math::Vec4Lite( s->col.R, s->col.G, s->col.B, s->col.A );
		m_SpriteList[i+4].col	= math::Vec4Lite( s->col.R, s->col.G, s->col.B, s->col.A );
		m_SpriteList[i+5].col	= math::Vec4Lite( s->col.R, s->col.G, s->col.B, s->col.A );

		float hw = s->w*0.5f;
		float hh = s->h*0.5f;
		float hd = s->d*0.5f;

		if( s->type == 0 )
		{
			// full rotation
			m_SpriteList[i].v	= math::Vec3( s->pos.X + (-right.X - up.X)*(hw), s->pos.Y + (-right.Y - up.Y)*(hh), s->pos.Z + (-right.Z - up.Z)*(hw) );
			m_SpriteList[i+1].v = math::Vec3( s->pos.X + (right.X - up.X)*(hw), s->pos.Y + (right.Y - up.Y)*(hh), s->pos.Z + (right.Z - up.Z)*(hw) );
			m_SpriteList[i+2].v = math::Vec3( s->pos.X + (up.X - right.X)*(hw), s->pos.Y + (up.Y - right.Y)*(hh), s->pos.Z + (up.Z - right.Z)*(hw) );

			m_SpriteList[i+3].v = math::Vec3( s->pos.X + (right.X - up.X)*(hw), s->pos.Y + (right.Y - up.Y)*(hh), s->pos.Z + (right.Z - up.Z)*(hw) );
			m_SpriteList[i+4].v = math::Vec3( s->pos.X + (right.X + up.X)*(hw), s->pos.Y + (right.Y + up.Y)*(hh), s->pos.Z + (right.Z + up.Z)*(hw) );
			m_SpriteList[i+5].v = math::Vec3( s->pos.X + (up.X - right.X)*(hw), s->pos.Y + (up.Y - right.Y)*(hh), s->pos.Z + (up.Z - right.Z)*(hw) );
		}
		else if( s->type == 1 )
		{
			// Y rotation only
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
		else if( s->type == 2 )
		{
			// static

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
	}

	UpdatePhysicsObjects( deltaTime );
}

/////////////////////////////////////////////////////
/// Method: UpdatePhysicsObjects
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void Level::UpdatePhysicsObjects( float deltaTime )
{

}
/////////////////////////////////////////////////////
/// Method: UpdateAudio
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void Level::UpdateAudio( collision::Sphere& player, float deltaTime )
{
	// process sounds
	float GAIN_SMOOTH_IN = 0.3f*deltaTime;
	float GAIN_SMOOTH_OUT = 0.3f*deltaTime;

	std::vector< SoundType *>::iterator it = m_SoundLocations.begin();

	while( it != m_SoundLocations.end() )
	{
		// always update the sound timer
		if( (*it)->type == 1 )
		{
			(*it)->lastTimeUpdate += deltaTime;
		}

		bool playerInAudioArea = false;
		if( (*it)->areaType == 0 )
		{
			if( (*it)->angle == 0.0f )
			{
				if( player.SphereCollidesWithAABB( (*it)->aabb ) )
					playerInAudioArea = true;
			}
			else
			{
				if( (*it)->obb.OBBCollidesWithSphere( player ) )
					playerInAudioArea = true;
			}
		}
		else if( (*it)->areaType == 1 )
		{
			if( player.SphereCollidesWithSphere( (*it)->sphere ) )
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
					if( SetAudioSourceData( *(*it) ) )
					{
						// play
						CHECK_OPENAL_ERROR( alSource3f( (*it)->sourceId, AL_POSITION, (*it)->offset.X, (*it)->offset.Y, (*it)->offset.Z ) )

						(*it)->gainControl = 0.0f;
						CHECK_OPENAL_ERROR( alSourcef( (*it)->sourceId, AL_GAIN, (*it)->gainControl ) )

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

						CHECK_OPENAL_ERROR( alSourcef( (*it)->sourceId, AL_GAIN, (*it)->gainControl ) )
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
						if( SetAudioSourceData( *(*it) ) )
						{
							// play
							CHECK_OPENAL_ERROR( alSourcef( (*it)->sourceId, AL_GAIN, (*it)->gain ) )

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

						CHECK_OPENAL_ERROR( alSourcef( (*it)->sourceId, AL_GAIN, (*it)->gainControl ) )

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
/// Method: ClearPhysicsObjects
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void Level::ClearPhysicsObjects()
{

}

/////////////////////////////////////////////////////
/// Method: ResetPhysicsObjects
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void Level::ResetPhysicsObjects()
{

}

/////////////////////////////////////////////////////
/// Method: SetExtendedRoomLimits
/// Params: [in]roomMaxEmpty, [in]roomMaxAmmo, [in]roomMaxHealth, [in]roomMaxZombie
///
/////////////////////////////////////////////////////
void Level::SetExtendedRoomLimits( unsigned int roomMaxEmpty, unsigned int roomMaxAmmo, unsigned int roomMaxHealth, unsigned int roomMaxZombie )
{
	m_MaxEmptyRooms = roomMaxEmpty;
	m_MaxAmmoRooms = roomMaxAmmo;
	m_MaxHealthRooms = roomMaxHealth;
	m_MaxZombieRooms = roomMaxZombie;	
}

/////////////////////////////////////////////////////
/// Method: FindLimitedInstance
/// Params: None
///
/////////////////////////////////////////////////////
int Level::FindLimitedInstance()
{
	int index = -1;

	while( index == -1 )
	{
		int val = math::RandomNumber( 0, ( static_cast<int>(m_LimitedInstances.size())-1) );

		if( m_LimitedInstances[val]->used == false )
		{
			m_LimitedInstances[val]->used = true;
			index = val;
			return(index);
		}
	}
	return(-1);
}

/////////////////////////////////////////////////////
/// Method: FindExtendedInstance
/// Params: None
///
/////////////////////////////////////////////////////
int Level::FindExtendedInstance( int roomType )
{
	int index = -1;

	while( index == -1 )
	{
		int val = math::RandomNumber( 0, ( static_cast<int>(m_ExtendedInstances.size())-1) );

		if( m_ExtendedInstances[val]->used == false )
		{
			m_ExtendedInstances[val]->used = true;
			index = val;

			return(index);
		}
	}
	return(-1);
}

/////////////////////////////////////////////////////
/// Method: AddLimitedInstanceRoom
/// Params: [in]modelFile, [in]b2dFile, [in]typesFile
///
/////////////////////////////////////////////////////
void Level::AddLimitedInstanceRoom( const char* modelFile, const char* b2dFile, const char* typesFile )
{
	int index = -1;
	index = FindLimitedInstance();

	RoomInstance* pObj = new RoomInstance( modelFile, m_MagFilter, m_MinFilter, m_LimitedInstances[index]->pos, m_LimitedInstances[index]->angle, m_LimitedInstances[index]->scale  );

	if( b2dFile )
	{
		LoadPhysicsB2DData( b2dFile, m_LimitedInstances[index]->pos, m_LimitedInstances[index]->angle, physics::PhysicsWorldB2D::GetWorld() );
	}

	if( typesFile )
	{
		LoadTypes( typesFile, true, pObj, m_LimitedInstances[index]->pos, m_LimitedInstances[index]->angle );
	}

	m_LimitedRoomInstances.push_back(pObj);
}

/////////////////////////////////////////////////////
/// Method: AddExtendedInstanceRoom
/// Params: [in]modelFile, [in]b2dFile, [in]typesFile
///
/////////////////////////////////////////////////////
void Level::AddExtendedInstanceRoom( int roomType, const char* modelFile, const char* b2dFile, const char* typesFile )
{
	unsigned int i=0;
	int index = -1;
	RoomInstance* pObj = 0;

	switch( roomType )
	{
		case TYPE_EXT_LOCKED:
		{
			// fill in all blank spaces
			for( i=0; i < m_ExtendedInstances.size(); ++i )
			{
				if( m_ExtendedInstances[i]->used == false )
				{
					m_ExtendedInstances[i]->used = true;

					pObj = new RoomInstance( modelFile, m_MagFilter, m_MinFilter, m_ExtendedInstances[i]->pos, m_ExtendedInstances[i]->angle, m_ExtendedInstances[i]->scale );

					if( b2dFile )
					{
						LoadPhysicsB2DData( b2dFile, m_ExtendedInstances[i]->pos, m_ExtendedInstances[i]->angle, physics::PhysicsWorldB2D::GetWorld() );
					}

					if( typesFile )
					{
						LoadTypes( typesFile, true, pObj, m_ExtendedInstances[i]->pos, m_ExtendedInstances[i]->angle );
					}

					m_ExtendedRoomInstances.push_back(pObj);
				}
			}

		}break;

		case TYPE_EXT_EMPTY:
		{
			DBG_ASSERT_MSG( (m_TotalEmptyRooms == 0), "Empty rooms have already been allocated" );

			for( i=0; i < m_MaxEmptyRooms; ++i )
			{
				index = FindExtendedInstance( roomType );

				pObj = new RoomInstance( modelFile, m_MagFilter, m_MinFilter, m_ExtendedInstances[index]->pos, m_ExtendedInstances[index]->angle, m_ExtendedInstances[i]->scale  );

				if( b2dFile )
				{
					LoadPhysicsB2DData( b2dFile, m_ExtendedInstances[index]->pos, m_ExtendedInstances[index]->angle, physics::PhysicsWorldB2D::GetWorld() );
				}

				if( typesFile )
				{
					LoadTypes( typesFile, true, pObj, m_ExtendedInstances[index]->pos, m_ExtendedInstances[index]->angle );
				}

				m_TotalEmptyRooms++;
				m_ExtendedRoomInstances.push_back(pObj);
			}
		}break;

		case TYPE_EXT_AMMO:
		{
			DBG_ASSERT_MSG( (m_TotalAmmoRooms == 0), "Ammo rooms have already been allocated" );

			for( i=0; i < m_MaxAmmoRooms; ++i )
			{
				index = FindExtendedInstance( roomType );

				pObj = new RoomInstance( modelFile, m_MagFilter, m_MinFilter, m_ExtendedInstances[index]->pos, m_ExtendedInstances[index]->angle, m_ExtendedInstances[i]->scale  );

				if( b2dFile )
				{
					LoadPhysicsB2DData( b2dFile, m_ExtendedInstances[index]->pos, m_ExtendedInstances[index]->angle, physics::PhysicsWorldB2D::GetWorld() );
				}

				if( typesFile )
				{
					LoadTypes( typesFile, true, pObj, m_ExtendedInstances[index]->pos, m_ExtendedInstances[index]->angle );
				}

				m_TotalAmmoRooms++;
				m_ExtendedRoomInstances.push_back(pObj);
			}
		}break;

		case TYPE_EXT_HEALTH:
		{
			DBG_ASSERT_MSG( (m_TotalHealthRooms == 0), "Health rooms have already been allocated" );

			for( i=0; i < m_MaxHealthRooms; ++i )
			{
				index = FindExtendedInstance( roomType );

				pObj = new RoomInstance( modelFile, m_MagFilter, m_MinFilter, m_ExtendedInstances[index]->pos, m_ExtendedInstances[index]->angle, m_ExtendedInstances[i]->scale );

				if( b2dFile )
				{
					LoadPhysicsB2DData( b2dFile, m_ExtendedInstances[index]->pos, m_ExtendedInstances[index]->angle, physics::PhysicsWorldB2D::GetWorld() );
				}

				if( typesFile )
				{
					LoadTypes( typesFile, true, pObj, m_ExtendedInstances[index]->pos, m_ExtendedInstances[index]->angle );
				}

				m_TotalHealthRooms++;
				m_ExtendedRoomInstances.push_back(pObj);
			}

		}break;

		case TYPE_EXT_ZOMBIE:
		{
			DBG_ASSERT_MSG( (m_TotalZombieRooms == 0), "Zombie rooms have already been allocated" );

			for( i=0; i < m_MaxZombieRooms; ++i )
			{
				index = FindExtendedInstance( roomType );

				pObj = new RoomInstance( modelFile, m_MagFilter, m_MinFilter, m_ExtendedInstances[index]->pos, m_ExtendedInstances[index]->angle, m_ExtendedInstances[i]->scale  );

				if( b2dFile )
				{
					LoadPhysicsB2DData( b2dFile, m_ExtendedInstances[index]->pos, m_ExtendedInstances[index]->angle, physics::PhysicsWorldB2D::GetWorld() );
				}

				if( typesFile )
				{
					LoadTypes( typesFile, true, pObj, m_ExtendedInstances[index]->pos, m_ExtendedInstances[index]->angle );
				}

				m_TotalZombieRooms++;
				m_ExtendedRoomInstances.push_back(pObj);
			}

		}break;

		default:
		{
			DBGLOG( "*ERROR* extended room type not recognised\n" );
			DBG_ASSERT_MSG( 0, "*ERROR* extended room type not recognised" );
		}break;
	}
}

/////////////////////////////////////////////////////
/// Method: SetAudioSourceData
/// Params: [in]audioLocator
///
/////////////////////////////////////////////////////
bool Level::SetAudioSourceData( SoundType& audioLocator )
{
	audioLocator.sourceId = snd::SoundManager::GetInstance()->GetFreeSoundSource();

	// no free sources
	if( audioLocator.sourceId == snd::INVALID_SOUNDSOURCE )
		return(false);

	//math::CVector3D pos3d( pAudioLocator->pos );
	//CAudioSystem::GetInstance()->PlayAudio( pAudioLocator->sourceId, pAudioLocator->bufferId, pos3d, false, pAudioLocator->loop, pAudioLocator->pitch, pAudioLocator->gain );
	
	if( audioLocator.loop )
	{
		CHECK_OPENAL_ERROR( alSourcei( audioLocator.sourceId, AL_LOOPING, AL_TRUE ) )
	}
	else
	{
		CHECK_OPENAL_ERROR( alSourcei( audioLocator.sourceId, AL_LOOPING, AL_FALSE ) )
	}
	
	// setup the sound
	CHECK_OPENAL_ERROR( alSourcei( audioLocator.sourceId, AL_BUFFER, audioLocator.bufferId ) )
	CHECK_OPENAL_ERROR( alSourcei( audioLocator.sourceId, AL_SOURCE_RELATIVE, audioLocator.srcRel ) )
	CHECK_OPENAL_ERROR( alSourcef( audioLocator.sourceId, AL_PITCH, audioLocator.pitch ) )
	//CHECK_OPENAL_ERROR( alSourcef( audioLocator.sourceId, AL_GAIN, audioLocator.gain ) )

	CHECK_OPENAL_ERROR( alSourcef( audioLocator.sourceId, AL_MAX_DISTANCE, MAX_AUDIO_DISTANCE ) )
	CHECK_OPENAL_ERROR( alSourcef( audioLocator.sourceId, AL_ROLLOFF_FACTOR, ROLL_OFF ) )
	CHECK_OPENAL_ERROR( alSourcef( audioLocator.sourceId, AL_REFERENCE_DISTANCE, REF_DISTANCE ) )

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

	bool lightState = renderer::OpenGL::GetInstance()->GetLightingState();
	bool textureState = renderer::OpenGL::GetInstance()->GetTextureState();

	if( lightState )
		renderer::OpenGL::GetInstance()->DisableLighting();
	if( textureState )
		renderer::OpenGL::GetInstance()->DisableTexturing();

	m_pMeshData->DrawBoundingBox();

	for( i=0; i < m_LightZones.size(); ++i )
	{
		if( m_LightZones[i]->angle != 0.0f )
		{
			glPushMatrix();
				renderer::DrawOBB( m_LightZones[i]->obb.vCenter, m_LightZones[i]->obb.vAxis, m_LightZones[i]->obb.vHalfWidths );
			glPopMatrix();
		}
		else
		{
			glPushMatrix();
				renderer::DrawAABB( m_LightZones[i]->aabb.vBoxMin, m_LightZones[i]->aabb.vBoxMax );
			glPopMatrix();
		}
	}

	for( i=0; i < m_StepAudioZones.size(); ++i )
	{
		if( m_StepAudioZones[i]->angle != 0.0f )
		{
			glPushMatrix();
				renderer::DrawOBB( m_StepAudioZones[i]->obb.vCenter, m_StepAudioZones[i]->obb.vAxis, m_StepAudioZones[i]->obb.vHalfWidths );
			glPopMatrix();
		}
		else
		{
			glPushMatrix();
				renderer::DrawAABB( m_StepAudioZones[i]->aabb.vBoxMin, m_StepAudioZones[i]->aabb.vBoxMax );
			glPopMatrix();
		}
	}

	for( i=0; i < m_SoundLocations.size(); ++i )
	{
		if( m_SoundLocations[i]->areaType == 0 )
		{
			if( m_SoundLocations[i]->angle != 0.0f )
			{
				glPushMatrix();
					renderer::DrawOBB( m_SoundLocations[i]->obb.vCenter, m_SoundLocations[i]->obb.vAxis, m_SoundLocations[i]->obb.vHalfWidths );
				glPopMatrix();
			}
			else
			{
				glPushMatrix();
					renderer::DrawAABB( m_SoundLocations[i]->aabb.vBoxMin, m_SoundLocations[i]->aabb.vBoxMax );
				glPopMatrix();
			}
		}
		else
		{
			glPushMatrix();
				glTranslatef( m_SoundLocations[i]->sphere.vCenterPoint.X, m_SoundLocations[i]->sphere.vCenterPoint.Y, m_SoundLocations[i]->sphere.vCenterPoint.Z );
				renderer::DrawSphere( m_SoundLocations[i]->sphere.fRadius );
			glPopMatrix();
		}
	}

	for( i=0; i < m_EmitterList.size(); ++i )
		m_EmitterList[i]->DrawDebug();

	if( lightState )
		renderer::OpenGL::GetInstance()->EnableLighting();
	if( textureState )
		renderer::OpenGL::GetInstance()->EnableTexturing();
}
