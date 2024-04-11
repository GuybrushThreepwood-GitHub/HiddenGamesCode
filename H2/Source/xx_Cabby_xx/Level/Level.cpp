
/*===================================================================
	File: Level.cpp
	App: Taxi

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include <cmath>
#include "MathBase.h"
#include "RenderBase.h"
#include "CollisionBase.h"
#include "SoundBase.h"
#include "Model/ModelCommon.h"
#include "ModelBase.h"

#include "Audio/AudioSystem.h"
#include "Resources/EmitterResources.h"
#include "Resources/SpriteResources.h"
#include "Resources/TextureResources.h"
#include "Resources/ModelResources.h"
#include "Player/Player.h"

#include "Physics/Objects/HShape.h"
#include "Physics/Objects/YShape.h"
#include "Physics/Objects/DoorSlideRight.h"
#include "Physics/Objects/DoorSlideLeft.h"
#include "Physics/Objects/DoorSlideUp.h"
#include "Physics/Objects/DoorSlideDown.h"

#ifdef ALLOW_UNUSED_OBJECTS
	#include "Physics/Objects/IShape.h"
	#include "Physics/Objects/UShape.h"
	#include "Physics/Objects/XShape.h"

	#include "Physics/Enemies/DustDevil.h"
	#include "Physics/Enemies/Piranha.h"
	#include "Physics/Enemies/Rat.h"
	#include "Physics/Enemies/Bomb.h"
	#include "Physics/Enemies/BouncingBomb.h"
	#include "Physics/Enemies/PufferFish.h"
#endif // ALLOW_UNUSED_OBJECTS

#include "Physics/Enemies/Fish.h"
#include "Physics/Enemies/Mine.h"
#include "Physics/Enemies/Sentry.h"

#include "Level.h"

using mdl::ModelHGM;
using mdl::ModelHGA;

namespace
{
	GLenum minFilter = GL_NEAREST/*_MIPMAP_NEAREST*/;
	GLenum magFilter = GL_NEAREST;
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
	m_GameData = GetScriptDataHolder()->GetGameData();

	m_pMeshData = 0;
	m_pBackground = 0;
	m_pForeground = 0;
	
	m_StarLocation.pos.setZero();
	m_StarLocation.rot.setZero();
	m_StarLocation.s.vCenterPoint.setZero();
	m_StarLocation.s.fRadius = m_GameData.ARCADE_STAR_COLLISION_RADIUS;
	m_StarPickedUp = false;
	m_StarPickupId = snd::INVALID_SOUNDBUFFER;
	m_StarRotation = 0.0f;
	m_pStarFX = 0;

	m_PhysicsIdList.clear();

	m_EmitterList.clear();
	m_Water.clear();
	m_SoundAreas.clear();

	m_PortAreas.clear();
	m_GravityAreas.clear();
	m_ForceAreas.clear();
	m_StarLocators.clear();

	m_PortList.clear();

	m_ShapeList.clear();

	m_DoorSlideList.clear();

	m_EnemyList.clear();

	m_CustomerMaxSpawn = 1;
	m_PlayerStartPoint = math::Vec2( 0.0f, 5.0f );
	m_PlayerSpawnDir = Player::PLAYERSPAWN_LEFT;

	m_SpriteTextureId = renderer::INVALID_OBJECT;
	m_SpriteCount = 0;
	m_SpriteData.clear();
	m_SpriteList = 0;

	m_SpriteTextureId = res::GetTextureResource( -100 )->texId;

	m_WaterTextureId = renderer::INVALID_OBJECT;
	m_WaterTextureId = res::GetTextureResource( -99 )->texId;

	m_BGTextureId = renderer::INVALID_OBJECT;
	m_BGUseFog = false;
	m_BGPos.setZero();
	m_BGDims = math::Vec2( 1.0f, 1.0f );

	m_BGPoints[0] = math::Vec3( -m_BGDims.Width, -m_BGDims.Height, 0.0f );
	m_BGPoints[1] = math::Vec3( m_BGDims.Width, -m_BGDims.Height, 0.0f );
	m_BGPoints[2] = math::Vec3( -m_BGDims.Width, m_BGDims.Height, 0.0f );
	m_BGPoints[3] = math::Vec3( m_BGDims.Width, m_BGDims.Height, 0.0f );

	m_BGUVCoords[0] = math::Vec2( 0.0f, 0.0f );// bottom left
	m_BGUVCoords[1] = math::Vec2( 1.0f, 0.0f );// bottom right
	m_BGUVCoords[2] = math::Vec2( 0.0f, 1.0f );// top left
	m_BGUVCoords[3] = math::Vec2( 1.0f, 1.0f );// top right

	m_DetailMapTexture = renderer::INVALID_OBJECT;
	m_DetailMapScaleX = 1.0f;
	m_DetailMapScaleY = 1.0f;
}

/////////////////////////////////////////////////////
/// Method: Release
/// Params: None
///
/////////////////////////////////////////////////////
void Level::Release( void )
{
	unsigned int i=0;

	if (m_DetailMapTexture != renderer::INVALID_OBJECT)
	{
		renderer::RemoveTexture(m_DetailMapTexture);
		m_DetailMapTexture = renderer::INVALID_OBJECT;
	}

	if( m_BGTextureId != renderer::INVALID_OBJECT )
	{
		renderer::RemoveTexture( m_BGTextureId );
		m_BGTextureId = renderer::INVALID_OBJECT;
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

	if( m_pMeshData != 0 )
	{
		res::RemoveModel( m_pMeshData );
		m_pMeshData = 0;
	}
	
	if( m_pBackground != 0 )
	{
		res::RemoveModel( m_pBackground );
		m_pBackground = 0;
	}

	if( m_pForeground != 0 )
	{
		res::RemoveModel( m_pForeground );
		m_pForeground = 0;
	}

	if( m_pArcadeStar != 0 )
	{
		res::RemoveModel( m_pArcadeStar );
		m_pArcadeStar = 0;
	}

	if( m_pStarFX != 0 )
	{
		delete m_pStarFX;
		m_pStarFX = 0;
	}

	for( i=0; i < m_Water.size(); ++i )
	{
		if( m_Water[i].grid != 0 )
		{
			delete m_Water[i].grid;
		}
	}

	for( i=0; i < m_SoundAreas.size(); ++i )
	{
		delete m_SoundAreas[i];
		m_SoundAreas[i] = 0;
	}
	
	AudioSystem::GetInstance()->StopAll();

	// clear out ports
	for( i=0; i < m_PortList.size(); ++i )
	{
		delete m_PortList[i];
		m_PortList[i] = 0;
	}

	// clear out all shapes
	for( i=0; i < m_ShapeList.size(); ++i )
	{
		delete m_ShapeList[i];
		m_ShapeList[i] = 0;
	}

	for( i=0; i < m_DoorSlideList.size(); ++i )
	{
		delete m_DoorSlideList[i];
		m_DoorSlideList[i] = 0;
	}

	for( i=0; i < m_EnemyList.size(); ++i )
	{
		delete m_EnemyList[i];
		m_EnemyList[i] = 0;
	}

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
	m_Water.clear();

	m_PortList.clear();
	m_PortAreas.clear();
	m_GravityAreas.clear();
	m_ForceAreas.clear();
	m_StarLocators.clear();

	m_ShapeList.clear();
	m_DoorSlideList.clear();
	m_EnemyList.clear();
}

/////////////////////////////////////////////////////
/// Method: LoadMeshData
/// Params: [in]meshResId
///
/////////////////////////////////////////////////////
int Level::LoadMeshData( int meshResId )
{
	Release();

	if( meshResId != -1 )
	{
		m_pMeshData = res::LoadModel( meshResId );
		DBG_ASSERT( m_pMeshData != 0 );

		m_pMeshData->SetModelGeneralFlags( mdl::MODELFLAG_GENERAL_AUTOCULLING | mdl::MODELFLAG_GENERAL_AABBCULL | mdl::MODELFLAG_GENERAL_SUBMESHCULL /*| mdl::MODELFLAG_GENERAL_DRAWBOUNDINGBOX*/ );
	}
	return(0);
}

/////////////////////////////////////////////////////
/// Method: LoadBackgroundModel
/// Params: [in]szFilename
///
/////////////////////////////////////////////////////
int Level::LoadBackgroundModel( int meshResId )
{
	if( meshResId != -1 )
	{
		m_pBackground = res::LoadModel( meshResId );
		DBG_ASSERT( m_pBackground != 0 );

		m_pBackground->SetModelGeneralFlags( mdl::MODELFLAG_GENERAL_AUTOCULLING | mdl::MODELFLAG_GENERAL_AABBCULL | mdl::MODELFLAG_GENERAL_SUBMESHCULL /*| mdl::MODELFLAG_GENERAL_DRAWBOUNDINGBOX*/ );
	}
	return(0);
}

/////////////////////////////////////////////////////
/// Method: LoadForegroundModel
/// Params: [in]szFilename
///
/////////////////////////////////////////////////////
int Level::LoadForegroundModel( int meshResId )
{
	if( meshResId != -1 )
	{
		m_pForeground = res::LoadModel( meshResId );
		DBG_ASSERT( m_pForeground != 0 );
	
		m_pForeground->SetModelGeneralFlags( mdl::MODELFLAG_GENERAL_AUTOCULLING | mdl::MODELFLAG_GENERAL_AABBCULL | mdl::MODELFLAG_GENERAL_SUBMESHCULL /*| mdl::MODELFLAG_GENERAL_DRAWBOUNDINGBOX*/ );
	}
	return(0);
}

/////////////////////////////////////////////////////
/// Method: LoadComplexPhysicsData
/// Params: [in]szFilename
/// 
/////////////////////////////////////////////////////
int Level::LoadComplexPhysicsData( const char *szFilename, b2World* pWorld )
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

	// create a generic body
	b2BodyDef bd;
	b2Body* pBody = 0;
	b2Shape* pShape = 0;
	b2FilterData fd;

	fd.categoryBits = WORLD_CATEGORY;
	fd.maskBits = PLAYER_CATEGORY;
	fd.groupIndex = WORLD_GROUP;

	pBody = pWorld->CreateBody(&bd);
	DBG_ASSERT( pBody != 0 );
	pBody->SetUserData( reinterpret_cast<void *>(this) );

	unsigned int totalPhysicsBox = 0;
	file::FileReadUInt( &totalPhysicsBox, sizeof(unsigned int), 1, &fileHandle );

	unsigned int totalPhysicsCircle = 0;
	file::FileReadUInt( &totalPhysicsCircle, sizeof(unsigned int), 1, &fileHandle );

	unsigned int totalMeshes = 0;
	file::FileReadUInt( &totalMeshes, sizeof(unsigned int), 1, &fileHandle );

	unsigned int totalPhysicsObjects = 0;
	file::FileReadUInt( &totalPhysicsObjects, sizeof(unsigned int), 1, &fileHandle );

	const int TYPE_BOX				=	0;
	const int TYPE_CIRCLE			=	1;
	const int TYPE_MESH				=	2;
	const int TYPE_DISTANCEJOINT	=	3;
	const int TYPE_GEARJOINT		=	4;
	const int TYPE_MOUSEJOINT		=	5;
	const int TYPE_PRISMATICJOINT	=	6;
	const int TYPE_PULLEYJOINT		=	7;
	const int TYPE_REVOLUTEJOINT	=	8;

	// generic id
	PhysicsIdentifier* pGenericPhysicsId = 0;
	pGenericPhysicsId = new PhysicsIdentifier( PHYSICSBASICID_WORLD, PHYSICSCASTID_WORLD );
	DBG_ASSERT( pGenericPhysicsId != 0 );
	// add to the id list
	m_PhysicsIdList.push_back( pGenericPhysicsId );

	for( i=0; i < totalPhysicsObjects; ++i )
	{
		int type=0;
		file::FileReadUInt( &type, sizeof(unsigned int), 1, &fileHandle );

		int numeric1 = -9999;
		int numeric2 = -9999;
		int numeric3 = -9999;
		int numeric4 = -9999;
		int numeric5 = -9999;

		//unsigned short categoryBits = 0x0001;
		//unsigned short maskBits = 0xFFFF;
		//short groupIndex = 1;

		switch( type )
		{
			case TYPE_BOX: // polyBox
				{
					b2PolygonDef boxDef;

					file::FileReadFloat( &boxDef.friction, sizeof(float), 1, &fileHandle );
					file::FileReadFloat( &boxDef.restitution, sizeof(float), 1, &fileHandle );
					file::FileReadFloat( &boxDef.density, sizeof(float), 1, &fileHandle );

					file::FileReadFloat( &numeric1, sizeof(int), 1, &fileHandle );
					file::FileReadFloat( &numeric2, sizeof(int), 1, &fileHandle );
					file::FileReadFloat( &numeric3, sizeof(int), 1, &fileHandle );
					file::FileReadFloat( &numeric4, sizeof(int), 1, &fileHandle );
					file::FileReadFloat( &numeric5, sizeof(int), 1, &fileHandle );


					b2Vec2 center( 0.0f, 0.0f );
					file::FileReadFloat( &center, sizeof(float), 2, &fileHandle );
					float angle = 0.0f;
					file::FileReadFloat( &angle, sizeof(float), 1, &fileHandle );

					float width = 1.0f;
					file::FileReadFloat( &width, sizeof(float), 1, &fileHandle );

					float height = 1.0f;
					file::FileReadFloat( &height, sizeof(float), 1, &fileHandle );

					boxDef.SetAsBox( width, height, center, angle );

					pShape = pBody->CreateShape( &boxDef );
					DBG_ASSERT( pShape != 0 );
					pShape->SetFilterData( fd );

					if( numeric1 != -9999 ||
						numeric2 != -9999 ||
						numeric3 != -9999 ||
						numeric4 != -9999 ||
						numeric5 != -9999 )
					{

						PhysicsIdentifier* pPhysicsId = 0;
						pPhysicsId = new PhysicsIdentifier( PHYSICSBASICID_WORLD, PHYSICSCASTID_WORLD );
						DBG_ASSERT( pPhysicsId != 0 );

						pPhysicsId->SetNumeric1( numeric1 );
						pPhysicsId->SetNumeric2( numeric2 );
						pPhysicsId->SetNumeric3( numeric3 );
						pPhysicsId->SetNumeric4( numeric4 );
						pPhysicsId->SetNumeric5( numeric5 );

						// add to the id list
						m_PhysicsIdList.push_back( pPhysicsId );

						// tell the body to point user data to it
						pShape->SetUserData( reinterpret_cast<void *>(pPhysicsId) );
					}
					else
					{
						pShape->SetUserData( reinterpret_cast<void *>(pGenericPhysicsId) );
					}
				}break;
			case TYPE_CIRCLE: // circle
				{
					b2CircleDef circleDef;

					file::FileReadFloat( &circleDef.friction, sizeof(float), 1, &fileHandle );
					file::FileReadFloat( &circleDef.restitution, sizeof(float), 1, &fileHandle );
					file::FileReadFloat( &circleDef.density, sizeof(float), 1, &fileHandle );

					file::FileReadFloat( &numeric1, sizeof(int), 1, &fileHandle );
					file::FileReadFloat( &numeric2, sizeof(int), 1, &fileHandle );
					file::FileReadFloat( &numeric3, sizeof(int), 1, &fileHandle );
					file::FileReadFloat( &numeric4, sizeof(int), 1, &fileHandle );
					file::FileReadFloat( &numeric5, sizeof(int), 1, &fileHandle );

					file::FileReadFloat( &circleDef.localPosition, sizeof(float), 2, &fileHandle );

					file::FileReadFloat( &circleDef.radius, sizeof(float), 1, &fileHandle );

					pShape = pBody->CreateShape( &circleDef );
					DBG_ASSERT( pShape != 0 );
					pShape->SetFilterData( fd );

					if( numeric1 != -9999 ||
						numeric2 != -9999 ||
						numeric3 != -9999 ||
						numeric4 != -9999 ||
						numeric5 != -9999 )
					{

						PhysicsIdentifier* pPhysicsId = 0;
						pPhysicsId = new PhysicsIdentifier( PHYSICSBASICID_WORLD, PHYSICSCASTID_WORLD );
						DBG_ASSERT( pPhysicsId != 0 );

						pPhysicsId->SetNumeric1( numeric1 );
						pPhysicsId->SetNumeric2( numeric2 );
						pPhysicsId->SetNumeric3( numeric3 );
						pPhysicsId->SetNumeric4( numeric4 );
						pPhysicsId->SetNumeric5( numeric5 );

						// add to the id list
						m_PhysicsIdList.push_back( pPhysicsId );

						// tell the shape to point user data to it
						pShape->SetUserData( reinterpret_cast<void *>(pPhysicsId) );
					}
					else
					{
						pShape->SetUserData( reinterpret_cast<void *>(pGenericPhysicsId) );
					}
				}break;
			case TYPE_MESH: // mesh
				{
					float friction = 1.0f;
					float restitution = 0.0f;
					float density = 0.0f;
					unsigned short categoryBits;
					unsigned short maskBits;
					short groupIndex;

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
							DBG_ASSERT( (vertCount >= 3 && vertCount <= 8) );

							b2PolygonDef polyDef;
							polyDef.friction = friction;
							polyDef.restitution = restitution;
							polyDef.density = density;
							polyDef.vertexCount = vertCount;

							for( a=0; a < vertCount; a++ )
							{
								file::FileReadFloat( &polyDef.vertices[a], sizeof(float), 2, &fileHandle );
							}

							pShape = pBody->CreateShape( &polyDef );
							DBG_ASSERT( pShape != 0 );
							pShape->SetFilterData( fd );

							if( numeric1 != -9999 ||
								numeric2 != -9999 ||
								numeric3 != -9999 ||
								numeric4 != -9999 ||
								numeric5 != -9999 )
							{

								PhysicsIdentifier* pPhysicsId = 0;
								pPhysicsId = new PhysicsIdentifier( PHYSICSBASICID_WORLD, PHYSICSCASTID_WORLD );
								DBG_ASSERT( pPhysicsId != 0 );

								pPhysicsId->SetNumeric1( numeric1 );
								pPhysicsId->SetNumeric2( numeric2 );
								pPhysicsId->SetNumeric3( numeric3 );
								pPhysicsId->SetNumeric4( numeric4 );
								pPhysicsId->SetNumeric5( numeric5 );

								// add to the id list
								m_PhysicsIdList.push_back( pPhysicsId );

								// tell the shape to point user data to it
								pShape->SetUserData( reinterpret_cast<void *>(pPhysicsId) );
							}
							else
							{
								pShape->SetUserData( reinterpret_cast<void *>(pGenericPhysicsId) );
							}
						}
					}
				}break;

				case TYPE_DISTANCEJOINT:
				{
					b2DistanceJointDef distJoint;

					file::FileReadFloat( &distJoint.localAnchor1, sizeof(float), 2, &fileHandle );
					file::FileReadFloat( &distJoint.localAnchor2, sizeof(float), 2, &fileHandle );
					file::FileReadFloat( &distJoint.length, sizeof(float), 1, &fileHandle );
					file::FileReadFloat( &distJoint.frequencyHz, sizeof(float), 1, &fileHandle );
					file::FileReadFloat( &distJoint.dampingRatio, sizeof(float), 1, &fileHandle );

				}break;
				case TYPE_GEARJOINT:
				{
					b2GearJointDef gearJoint;

					file::FileReadFloat( &gearJoint.ratio, sizeof(float), 1, &fileHandle );

				}break;
				case TYPE_MOUSEJOINT:
				{
					b2MouseJointDef mouseJoint;

					file::FileReadFloat( &mouseJoint.target, sizeof(float), 2, &fileHandle );
					file::FileReadFloat( &mouseJoint.maxForce, sizeof(float), 1, &fileHandle );
					file::FileReadFloat( &mouseJoint.frequencyHz, sizeof(float), 1, &fileHandle );
					file::FileReadFloat( &mouseJoint.dampingRatio, sizeof(float), 1, &fileHandle );
					file::FileReadFloat( &mouseJoint.timeStep, sizeof(float), 1, &fileHandle );
				}break;
				case TYPE_PRISMATICJOINT:
				{
					b2PrismaticJointDef prismaticJoint;

					file::FileReadFloat( &prismaticJoint.localAnchor1, sizeof(float), 2, &fileHandle );
					file::FileReadFloat( &prismaticJoint.localAnchor2, sizeof(float), 2, &fileHandle );

					file::FileReadFloat( &prismaticJoint.localAxis1, sizeof(float), 2, &fileHandle );

					file::FileReadFloat( &prismaticJoint.referenceAngle, sizeof(float), 1, &fileHandle );
					file::FileReadBool( &prismaticJoint.enableLimit, sizeof(bool), 1, &fileHandle );

					file::FileReadFloat( &prismaticJoint.lowerTranslation, sizeof(float), 1, &fileHandle );
					file::FileReadFloat( &prismaticJoint.upperTranslation, sizeof(float), 1, &fileHandle );

					file::FileReadBool( &prismaticJoint.enableMotor, sizeof(bool), 1, &fileHandle );

					file::FileReadFloat( &prismaticJoint.maxMotorForce, sizeof(float), 1, &fileHandle );
					file::FileReadFloat( &prismaticJoint.motorSpeed, sizeof(float), 1, &fileHandle );
				}break;
				case TYPE_PULLEYJOINT:
				{
					b2PulleyJointDef pulleyJoint;

					file::FileReadFloat( &pulleyJoint.groundAnchor1, sizeof(float), 2, &fileHandle );
					file::FileReadFloat( &pulleyJoint.groundAnchor2, sizeof(float), 2, &fileHandle );
					file::FileReadFloat( &pulleyJoint.localAnchor1, sizeof(float), 2, &fileHandle );
					file::FileReadFloat( &pulleyJoint.localAnchor2, sizeof(float), 2, &fileHandle );

					file::FileReadFloat( &pulleyJoint.length1, sizeof(float), 1, &fileHandle );
					file::FileReadFloat( &pulleyJoint.maxLength1, sizeof(float), 1, &fileHandle );
					file::FileReadFloat( &pulleyJoint.length2, sizeof(float), 1, &fileHandle );
					file::FileReadFloat( &pulleyJoint.maxLength2, sizeof(float), 1, &fileHandle );
					file::FileReadFloat( &pulleyJoint.ratio, sizeof(float), 1, &fileHandle );

					file::FileReadBool( &pulleyJoint.collideConnected, sizeof(bool), 1, &fileHandle );		

				}break;
				case TYPE_REVOLUTEJOINT:
				{
					b2RevoluteJointDef revoluteJoint;

					file::FileReadFloat( &revoluteJoint.localAnchor1, sizeof(float), 2, &fileHandle );
					file::FileReadFloat( &revoluteJoint.localAnchor2, sizeof(float), 2, &fileHandle );

					file::FileReadFloat( &revoluteJoint.referenceAngle, sizeof(float), 1, &fileHandle );
					file::FileReadFloat( &revoluteJoint.lowerAngle, sizeof(float), 1, &fileHandle );
					file::FileReadFloat( &revoluteJoint.upperAngle, sizeof(float), 1, &fileHandle );
					file::FileReadFloat( &revoluteJoint.maxMotorTorque, sizeof(float), 1, &fileHandle );
					file::FileReadFloat( &revoluteJoint.motorSpeed, sizeof(float), 1, &fileHandle );

					file::FileReadBool( &revoluteJoint.enableLimit, sizeof(bool), 1, &fileHandle );
					file::FileReadBool( &revoluteJoint.enableMotor, sizeof(bool), 1, &fileHandle );
				}
				break;

				default:
					DBG_ASSERT(0);
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
int Level::LoadData( const char* szFilename )
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
			int numObjects = static_cast<int>( script::LuaGetNumberFromTableItem( "numObjects", 1, 0.0 ) );
			if( numObjects > 0 )
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
								case ANIMATED_MESH:
								{
									ParseAnimatedMesh( paramIndex, baseType, baseId );
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
								case ENEMY_SPAWN:
								{
									ParseEnemySpawn( paramIndex, baseType, baseId );
								}break;
								case SHAPE_ROTATING_SPAWN:
								{
									ParseRotatingShape( paramIndex, baseType, baseId );
								}break;
								case SHAPE_MOVING_SPAWN:
								{
									ParseMovingShape( paramIndex, baseType, baseId );
								}break;
								case PORT:
								{
									ParsePort( paramIndex, baseType, baseId );
								}break;
								case GRAVITY_AREA:
								{
									ParseGravityArea( paramIndex, baseType, baseId );
								}break;
								case FORCE_AREA:
								{
									ParseForceArea( paramIndex, baseType, baseId );
								}break;
								case WATER_GRID:
								{
									ParseWaterGrid( paramIndex, baseType, baseId );
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

	return 0;
}

/////////////////////////////////////////////////////
/// Method: DetailMapSetup
/// Params: [in]textureResId
/// 
/////////////////////////////////////////////////////
void Level::DetailMapSetup(int textureResId, float scaleX, float scaleY)
{
	if (textureResId != -1)
	{
		const res::TextureResourceStore* rs = 0;
		rs = res::GetTextureResource(textureResId);
		DBG_ASSERT(rs != 0);

		res::LoadTexture(textureResId);

		m_DetailMapTexture = rs->texId;

		if (m_pMeshData != 0)
		{
			m_pMeshData->SetTextureToMaterial(m_DetailMapTexture, -1, 1);
			m_pMeshData->SetMaterialScale(-1, -1, 1, scaleX, scaleY);
		}
	}

	m_DetailMapScaleX = scaleX;
	m_DetailMapScaleY = scaleY;
}

/////////////////////////////////////////////////////
/// Method: SetupBGQuad
/// Params: [in]textureId, [in]useFog, [in]dims, [in]pos
/// 
/////////////////////////////////////////////////////
void Level::SetupBGQuad( int textureResId, bool useFog, const math::Vec2& dims, const math::Vec3& pos )
{
	const res::TextureResourceStore* rs = 0;
	rs = res::GetTextureResource( textureResId );
	DBG_ASSERT( rs != 0 );

	if( m_BGTextureId != renderer::INVALID_OBJECT )
	{
		renderer::RemoveTexture( m_BGTextureId );
		m_BGTextureId = renderer::INVALID_OBJECT;
	}

	res::LoadTexture( textureResId );

	m_BGTextureId = rs->texId;
	m_BGUseFog = useFog;
	m_BGPos = pos;
	m_BGDims = dims;

	m_BGPoints[0] = math::Vec3( m_BGPos.X-m_BGDims.Width, m_BGPos.Y-m_BGDims.Height, m_BGPos.Z );
	m_BGPoints[1] = math::Vec3( m_BGPos.X+m_BGDims.Width, m_BGPos.Y-m_BGDims.Height, m_BGPos.Z );
	m_BGPoints[2] = math::Vec3( m_BGPos.X-m_BGDims.Width, m_BGPos.Y+m_BGDims.Height, m_BGPos.Z );
	m_BGPoints[3] = math::Vec3( m_BGPos.X+m_BGDims.Width, m_BGPos.Y+m_BGDims.Height, m_BGPos.Z );
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
/// Method: SetupStarLocation
/// Params: None
/// 
/////////////////////////////////////////////////////
void Level::SetupStarLocation()
{
	if( m_StarLocators.size() <= 0 )
	{
		DBGLOG( "LEVEL: *ERROR* There are no star locators\n" );
		return;
	}
	
	int index = math::RandomNumber( 0, static_cast<int>( m_StarLocators.size()-1 ) );
	m_StarLocation = m_StarLocators[index];
	m_StarPickedUp = false;
	m_pArcadeStar = res::LoadModel( 140 );
	DBG_ASSERT( m_pArcadeStar != 0 );
	m_StarRotation = 0.0f;

	m_StarPickupId = AudioSystem::GetInstance()->AddAudioFile( 501/*"star_pickup.wav"*/ ); 

	const res::EmitterResourceStore* er = res::GetEmitterResource(200);
	DBG_ASSERT( er != 0 );

	m_pStarFX = new efx::Emitter;
	m_pStarFX->Create( *er->block );
	res::SetupTexturesOnEmitter( m_pStarFX );
}

/////////////////////////////////////////////////////
/// Method: DrawBackground
/// Params: None
/// 
/////////////////////////////////////////////////////
void Level::DrawBackground()
{
	renderer::OpenGL::GetInstance()->SetColour4ub( 255, 255, 255, 255 );
	renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	//renderer::OpenGL::GetInstance()->DepthMode( false, GL_LESS );

	// draw static world meshes
	if( m_pBackground != 0 )
		m_pBackground->Draw();

	//renderer::OpenGL::GetInstance()->DepthMode( true, GL_LESS );
}

/////////////////////////////////////////////////////
/// Method: DrawForeground
/// Params: None
/// 
/////////////////////////////////////////////////////
void Level::DrawForeground()
{
	renderer::OpenGL::GetInstance()->SetColour4ub( 255, 255, 255, 255 );
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	renderer::OpenGL::GetInstance()->DepthMode( false, GL_LESS );

	if( m_pForeground != 0 )
		m_pForeground->Draw();

	renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	renderer::OpenGL::GetInstance()->DepthMode( true, GL_LESS );
}

/////////////////////////////////////////////////////
/// Method: DrawMainLevel
/// Params: None
/// 
/////////////////////////////////////////////////////
void Level::DrawMainLevel()
{
	renderer::OpenGL::GetInstance()->SetColour4ub( 255, 255, 255, 255 );
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	if (m_DetailMapTexture != renderer::INVALID_OBJECT)
	{
		glActiveTexture(GL_TEXTURE1);
		glDisable(GL_TEXTURE_2D);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
		glTexEnvf(GL_TEXTURE_ENV, GL_RGB_SCALE, 2.0f);
		glActiveTexture(GL_TEXTURE0);
	}

	if( m_pMeshData != 0 )
		m_pMeshData->Draw();	

	if (m_DetailMapTexture != renderer::INVALID_OBJECT)
	{
		glActiveTexture(GL_TEXTURE1);
		glDisable(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE0);
	}

	renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}

/////////////////////////////////////////////////////
/// Method: DrawWater
/// Params: None
/// 
/////////////////////////////////////////////////////
void Level::DrawWater()
{
	unsigned int i=0;

	renderer::OpenGL::GetInstance()->SetCullState( false, GL_BACK );
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	renderer::OpenGL::GetInstance()->DepthMode( false, GL_LESS );
	renderer::OpenGL::GetInstance()->SetColour4ub( 255, 255, 255, 255 );

	if( m_WaterTextureId != renderer::INVALID_OBJECT )
		renderer::OpenGL::GetInstance()->BindTexture( m_WaterTextureId );

	renderer::OpenGL::GetInstance()->EnableTextureArray();

	for( i=0; i < m_Water.size(); ++i )
	{
		if( m_Water[i].grid != 0 )
		{
			glPushMatrix();
				glTranslatef( m_Water[i].pos.X, m_Water[i].pos.Y, m_Water[i].pos.Z );
				m_Water[i].grid->Draw();
			glPopMatrix();
		}
	}

	renderer::OpenGL::GetInstance()->SetCullState( true, GL_BACK );
	renderer::OpenGL::GetInstance()->DepthMode( true, GL_LESS );
	renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	renderer::OpenGL::GetInstance()->DisableTextureArray();
}

/////////////////////////////////////////////////////
/// Method: DrawPorts
/// Params: None
/// 
/////////////////////////////////////////////////////
void Level::DrawPorts()
{
	unsigned int i=0;
	renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	renderer::OpenGL::GetInstance()->SetColour4ub( 255, 255, 255, 255 );
	
	for( i=0; i < m_PortList.size(); ++i )
		m_PortList[i]->Draw(  );
}

/////////////////////////////////////////////////////
/// Method: DrawEmitters
/// Params: None
/// 
/////////////////////////////////////////////////////
void Level::DrawEmitters()
{
	unsigned int i=0;
	
	renderer::OpenGL::GetInstance()->SetColour4ub( 255, 255, 255, 255 );
	
	for( i=0; i < m_EmitterList.size(); ++i )
		m_EmitterList[i]->Draw();	

	renderer::OpenGL::GetInstance()->SetColour4ub( 255, 255, 255, 255 );

	if( m_SpriteCount )
	{
		renderer::OpenGL::GetInstance()->DisableVBO();

		renderer::OpenGL::GetInstance()->DepthMode( false, GL_LEQUAL );
		renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		renderer::OpenGL::GetInstance()->SetCullState( false, GL_BACK );

		renderer::OpenGL::GetInstance()->EnableTextureArray();
		renderer::OpenGL::GetInstance()->EnableColourArray();

		if( m_SpriteTextureId != renderer::INVALID_OBJECT )
			renderer::OpenGL::GetInstance()->BindTexture( m_SpriteTextureId );

		glVertexPointer( 3, GL_FLOAT, sizeof(SpriteVert), &m_SpriteList[0].v );
		glTexCoordPointer( 2, GL_FLOAT, sizeof(SpriteVert), &m_SpriteList[0].uv );
		glColorPointer( 4, GL_UNSIGNED_BYTE, sizeof(SpriteVert), &m_SpriteList[0].col );

		glDrawArrays( GL_TRIANGLES, 0, m_SpriteCount );

		renderer::OpenGL::GetInstance()->SetCullState( true, GL_BACK );
	
		renderer::OpenGL::GetInstance()->DepthMode( true, GL_LESS );
		renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		
		renderer::OpenGL::GetInstance()->DisableColourArray();
		renderer::OpenGL::GetInstance()->DisableTextureArray();
	}	

	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	if( m_pArcadeStar != 0 && !m_StarPickedUp )
	{
		if( renderer::OpenGL::GetInstance()->SphereInFrustum( m_StarLocation.s.vCenterPoint.X, m_StarLocation.s.vCenterPoint.Y, m_StarLocation.s.vCenterPoint.Z, m_StarLocation.s.fRadius ) )
		{
			glPushMatrix();
				glTranslatef( m_StarLocation.pos.X, m_StarLocation.pos.Y, m_StarLocation.pos.Z );
				glRotatef( m_StarRotation, 0.0f, 1.0f, 0.0f );
				m_pArcadeStar->Draw();
			glPopMatrix();
		}
	}		

	if( m_pStarFX != 0 )
		m_pStarFX->Draw();
	renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}

/////////////////////////////////////////////////////
/// Method: DrawBGQuad
/// Params: None
/// 
/////////////////////////////////////////////////////
void Level::DrawBGQuad()
{
	renderer::TGLFogState curFogState;

	renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	renderer::OpenGL::GetInstance()->DepthMode( false, GL_LESS );

	renderer::OpenGL::GetInstance()->DisableVBO();

	renderer::OpenGL::GetInstance()->SetColour4ub( 255, 255, 255, 255 );

	bool lightState = renderer::OpenGL::GetInstance()->GetLightingState();
	if( lightState )
		renderer::OpenGL::GetInstance()->DisableLighting();

	if( !m_BGUseFog )
	{
		renderer::OpenGL::GetInstance()->SaveFogState( &curFogState );

		bool fogState = curFogState.bFogState;
		curFogState.bFogState = false;
		renderer::OpenGL::GetInstance()->SetFogState( &curFogState );
		curFogState.bFogState = fogState;
	}

	if( m_BGTextureId != renderer::INVALID_OBJECT )
	{
		renderer::OpenGL::GetInstance()->BindTexture(m_BGTextureId);

		renderer::OpenGL::GetInstance()->EnableTextureArray();			

		glVertexPointer( 3, GL_FLOAT, sizeof( math::Vec3 ), m_BGPoints );
		glTexCoordPointer( 2, GL_FLOAT, sizeof( math::Vec2 ), m_BGUVCoords );
		glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

		renderer::OpenGL::GetInstance()->DisableTextureArray();
	}

	if( lightState )
		renderer::OpenGL::GetInstance()->EnableLighting();

	if( !m_BGUseFog )
	{
		renderer::OpenGL::GetInstance()->SetFogState( &curFogState );
	}

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_LESS );
}

/////////////////////////////////////////////////////
/// Method: DrawPhysicsObjects
/// Params: None
/// 
/////////////////////////////////////////////////////
void Level::DrawPhysicsObjects( )
{
	unsigned int i=0;

	renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	for( i=0; i < m_ShapeList.size(); ++i )
		m_ShapeList[i]->Draw();

	for( i=0; i < m_DoorSlideList.size(); ++i )
		m_DoorSlideList[i]->Draw();

	for( i=0; i < m_EnemyList.size(); ++i )
		m_EnemyList[i]->Draw();
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void Level::Update( float deltaTime )
{
	int i=0, j=0;
	Player* pPlayer = PhysicsWorld::GetPlayer();

	// draw static world meshes
	if( m_pBackground != 0 )
		m_pBackground->Update( deltaTime );

	if( m_pMeshData != 0 )
		m_pMeshData->Update( deltaTime );

	if( m_pForeground != 0 )
		m_pForeground->Update( deltaTime );

	for( i=0; i < static_cast<int>(m_Water.size()); ++i )
	{
		if( m_Water[i].grid != 0 )
		{
			m_Water[i].grid->Update(deltaTime);
		}
	}

	if( pPlayer != 0 && !m_StarPickedUp )
	{
		m_StarRotation += m_GameData.ARCADE_STAR_ROTATION*deltaTime;
		if( m_StarRotation > 360.0f )
			m_StarRotation -= 360.0f;

		collision::Sphere s1 = pPlayer->GetBoundingSphere();
		if( s1.SphereCollidesWithSphere( m_StarLocation.s ) )
		{
			m_StarPickedUp = true;

			if( m_pStarFX != 0 )
			{
				m_pStarFX->SetPos( m_StarLocation.s.vCenterPoint );

				m_pStarFX->Enable();
				m_pStarFX->Disable();
			}

			// play audio
			ALuint srcId = snd::SoundManager::GetInstance()->GetFreeSoundSource();
			AudioSystem::GetInstance()->PlayAudio( srcId, m_StarPickupId, math::Vec3(0.0f,0.0f,0.0f), AL_TRUE, AL_FALSE );
		}
	}

	if( m_pStarFX != 0 )
		m_pStarFX->Update( deltaTime );

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

	/*math::Vec3 axis( 0.0f, 0.0f, 1.0f );
	for( i=0; i<m_SpriteData.size(); ++i )
	{
		SpriteType *s = m_SpriteData[i];
		s->sceneDepth = std::abs( math::DotProduct( (s->pos - vEye), axis ) );
	}
	// sort the sprites
	std::sort( m_SpriteData.begin(), m_SpriteData.end(), SpriteSort );*/

	for( i=0, j=0; i < m_SpriteCount; i+=6, j++ )
	{
		SpriteType *s = m_SpriteData[j];
		DBG_ASSERT( s != 0 );

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

		// is this a night sprite, but it's not night, zero out
		if( s->baseId == m_GameData.NIGHT_SPRITES_START &&
			m_TimeOfDay != LEVEL_NIGHT )
		{
			m_SpriteList[i].v	= math::Vec3( 0.0f, 0.0f, 0.0f );
			m_SpriteList[i+1].v = math::Vec3( 0.0f, 0.0f, 0.0f );
			m_SpriteList[i+2].v = math::Vec3( 0.0f, 0.0f, 0.0f );

			m_SpriteList[i+3].v = math::Vec3( 0.0f, 0.0f, 0.0f );
			m_SpriteList[i+4].v = math::Vec3( 0.0f, 0.0f, 0.0f );
			m_SpriteList[i+5].v = math::Vec3( 0.0f, 0.0f, 0.0f );
		}

	}

	UpdatePhysicsObjects( deltaTime );

	UpdateAudio( deltaTime );
}

/////////////////////////////////////////////////////
/// Method: UpdatePhysicsObjects
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void Level::UpdatePhysicsObjects( float deltaTime )
{
	unsigned int i=0;

	for( i=0; i < m_PortList.size(); ++i )
		m_PortList[i]->Update( deltaTime );

	for( i=0; i < m_ShapeList.size(); ++i )
		m_ShapeList[i]->Update( deltaTime );

	for( i=0; i < m_DoorSlideList.size(); ++i )
		m_DoorSlideList[i]->Update( deltaTime );

	for( i=0; i < m_EnemyList.size(); ++i )
		m_EnemyList[i]->Update( deltaTime );

	for( i=0; i < m_EmitterList.size(); ++i )
		m_EmitterList[i]->Update( deltaTime );
}

/////////////////////////////////////////////////////
/// Method: ClearPhysicsObjects
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void Level::ClearPhysicsObjects()
{
	unsigned int i=0;

	for( i=0; i < m_ShapeList.size(); ++i )
		m_ShapeList[i]->Remove();

	for( i=0; i < m_EnemyList.size(); ++i )
		m_EnemyList[i]->Remove();

	for( i=0; i < m_DoorSlideList.size(); ++i )
		m_DoorSlideList[i]->Remove();
}

/////////////////////////////////////////////////////
/// Method: ResetPhysicsObjects
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void Level::ResetPhysicsObjects()
{
	unsigned int i=0;

	for( i=0; i < m_ShapeList.size(); ++i )
		m_ShapeList[i]->Reset();

	for( i=0; i < m_EnemyList.size(); ++i )
		m_EnemyList[i]->Reset();

	for( i=0; i < m_DoorSlideList.size(); ++i )
		m_DoorSlideList[i]->Reset();
}

/////////////////////////////////////////////////////
/// Method: ResetStarPickup
/// Params: None
///
/////////////////////////////////////////////////////
void Level::ResetStarPickup()
{
	if( m_StarLocators.size() <= 0 )
	{
		DBGLOG( "LEVEL: *ERROR* There are no star locators\n" );
		return;
	}
	
	int index = math::RandomNumber( 0, static_cast<int>( m_StarLocators.size()-1 ) );
	m_StarLocation = m_StarLocators[index];
	m_StarPickedUp = false;
	if( m_pArcadeStar == 0 )
	{
		m_pArcadeStar = res::LoadModel( 140 );
		DBG_ASSERT( m_pArcadeStar != 0 );
	}
	m_StarRotation = 0.0f;

	if( m_StarPickupId == snd::INVALID_SOUNDBUFFER )
		m_StarPickupId = AudioSystem::GetInstance()->AddAudioFile( 501/*"star_pickup.wav"*/ ); 

	if( m_pStarFX != 0 )
	{
		m_pStarFX->Reset();
		m_pStarFX->Disable();
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
/// Method: UpdateAudio
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void Level::UpdateAudio( float deltaTime )
{
	unsigned int i=0;
	Player* pPlayer = PhysicsWorld::GetPlayer();
	
	if( pPlayer == 0 )
		return;

	collision::Sphere s1 = pPlayer->GetBoundingSphere();
	
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

/*	// process sounds
	unsigned int i=0;
	Player* pPlayer = PhysicsWorld::GetPlayer();
	
	if( pPlayer == 0 )
		return;

	collision::Sphere s1 = pPlayer->GetBoundingSphere();

	for( i=0; i < m_SoundAreas.size(); ++i )
	{
		// always update the sound timer
		switch( m_SoundAreas[i]->type )
		{
			case eAudioType_Normal:
			{

			}break;
			case eAudioType_Timed:
			{
				m_SoundAreas[i]->lastTimeUpdate += deltaTime;

			}break;
			default:
				DBG_ASSERT(0);
				break;
		}

		bool playerInAudioArea = false;
		if( m_SoundAreas[i]->triggerType == eAudioTrigger_Sphere )
		{
			math::Vec3 sPos( m_SoundAreas[i]->pos.X, m_SoundAreas[i]->pos.Y, 0.0f );
			collision::Sphere s2( sPos, m_SoundAreas[i]->radius );
			if( s1.SphereCollidesWithSphere( s2 ) )
				playerInAudioArea = true;
		}
		else if( m_SoundAreas[i]->triggerType == eAudioTrigger_Box )
		{
			collision::AABB aabb;
			aabb.vBoxMin = math::Vec3( m_SoundAreas[i]->pos.X-(m_SoundAreas[i]->boxDim.fWidth*0.5f), m_SoundAreas[i]->pos.Y-(m_SoundAreas[i]->boxDim.fHeight*0.5f), 0.0f );
			aabb.vBoxMax = math::Vec3( m_SoundAreas[i]->pos.X+(m_SoundAreas[i]->boxDim.fWidth*0.5f), m_SoundAreas[i]->pos.Y+(m_SoundAreas[i]->boxDim.fHeight*0.5f), 0.0f );
			if( s1.SphereCollidesWithAABB( aabb ) )
				playerInAudioArea = true;
		}

		if( playerInAudioArea )
		{
			float finalGain = 1.0f;

			// custom attenuation
			if( m_SoundAreas[i]->gainAttenuation )
			{
				if( m_SoundAreas[i]->triggerType == eAudioTrigger_Sphere )
				{
					math::Vec3 sPos( m_SoundAreas[i]->pos.X, m_SoundAreas[i]->pos.Y, 0.0f );
					math::Vec3 pPos( s1.vCenterPoint.X, s1.vCenterPoint.Y, 0.0f );

					float distance = std::fabs((sPos-pPos).length());
					finalGain = 1.0f-(distance/m_SoundAreas[i]->radius);
					math::Clamp( &finalGain, 0.0f, 2.0f );
				}
				else
				{
					math::Vec3 sPos( m_SoundAreas[i]->pos.X, m_SoundAreas[i]->pos.Y, 0.0f );
					math::Vec3 pPos( s1.vCenterPoint.X, s1.vCenterPoint.Y, 0.0f );

					float largestLength = std::fabs( m_SoundAreas[i]->boxDim.fWidth );
					if( largestLength < std::fabs(m_SoundAreas[i]->boxDim.fHeight ) )
						largestLength = std::fabs(m_SoundAreas[i]->boxDim.fHeight );
					
					float distance = std::fabs((sPos-pPos).length());
					finalGain = 1.0f-(distance/largestLength);
					math::Clamp( &finalGain, 0.0f, 2.0f );
				}
			}

			// normal update
			switch( m_SoundAreas[i]->type )
			{
				case eAudioType_Normal:
				{
					if( m_SoundAreas[i]->sourceId == snd::INVALID_SOUNDSOURCE )
					{
						// sound is not playing/setup
						if( SetAudioSourceData( m_SoundAreas[i] ) )
						{
							float multiply = 1.0f;
							if( !AudioSystem::GetInstance()->GetSFXState() )
								multiply = 0.0f;

							// play
							CHECK_OPENAL_ERROR( alSourcef( m_SoundAreas[i]->sourceId, AL_GAIN, finalGain*multiply ) )
							CHECK_OPENAL_ERROR( alSource3f( m_SoundAreas[i]->sourceId, AL_POSITION, m_SoundAreas[i]->pos.X, m_SoundAreas[i]->pos.Y, 0.0f ) )

							CHECK_OPENAL_ERROR( alSourcePlay( m_SoundAreas[i]->sourceId ) )

							if( !m_SoundAreas[i]->loop )
								m_SoundAreas[i]->sourceId = snd::INVALID_SOUNDSOURCE;
						}
					}
				}break;
				case eAudioType_Timed:
				{
					if( m_SoundAreas[i]->lastTimeUpdate >= m_SoundAreas[i]->time )
					{
						if( m_SoundAreas[i]->sourceId == snd::INVALID_SOUNDSOURCE )
						{
							// create another sound
							if( SetAudioSourceData( m_SoundAreas[i] ) )
							{
								float multiply = 1.0f;
								if( !AudioSystem::GetInstance()->GetSFXState() )
									multiply = 0.0f;

								// play
								CHECK_OPENAL_ERROR( alSourcef( m_SoundAreas[i]->sourceId, AL_GAIN, finalGain ) )
								CHECK_OPENAL_ERROR( alSource3f( m_SoundAreas[i]->sourceId, AL_POSITION, m_SoundAreas[i]->pos.X, m_SoundAreas[i]->pos.Y, 0.0f ) )

								CHECK_OPENAL_ERROR( alSourcePlay( m_SoundAreas[i]->sourceId ) )

								if( !m_SoundAreas[i]->loop )
									m_SoundAreas[i]->sourceId = snd::INVALID_SOUNDSOURCE;

							}
						}

						m_SoundAreas[i]->lastTimeUpdate = 0.0f;
					}
				}break;
				default:
					DBG_ASSERT(0);
					break;
			}
		}
		else
		{
			// reset the timer even if not in the area
			switch( m_SoundAreas[i]->type )
			{
				case eAudioType_Normal:
				{

				}break;
				case eAudioType_Timed:
				{
					if( m_SoundAreas[i]->lastTimeUpdate >= m_SoundAreas[i]->time )
						m_SoundAreas[i]->lastTimeUpdate = 0.0f;
				}break;
				default:
					DBG_ASSERT(0);
					break;
			}

			// if the sound is playing and it's looped, stop the loop
			if( m_SoundAreas[i]->sourceId != snd::INVALID_SOUNDSOURCE )
			{
				if( snd::SoundManager::GetInstance()->GetSoundState( m_SoundAreas[i]->sourceId ) == AL_PLAYING )
				{
					if( m_SoundAreas[i]->loop )
					{
						CHECK_OPENAL_ERROR( alSourcei( m_SoundAreas[i]->sourceId, AL_LOOPING, AL_FALSE ) )
					}
				}
			
				m_SoundAreas[i]->sourceId = snd::INVALID_SOUNDSOURCE;
			}
		}
	}*/
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

	bool lightState = renderer::OpenGL::GetInstance()->GetLightingState();
	bool textureState = renderer::OpenGL::GetInstance()->GetTextureState();

	if( lightState )
		renderer::OpenGL::GetInstance()->DisableLighting();
	if( textureState )
		renderer::OpenGL::GetInstance()->DisableTexturing();

	for( i=0; i < m_EnemyList.size(); ++i )
		m_EnemyList[i]->DrawDebug();

	if( m_pArcadeStar != 0 && !m_StarPickedUp )
	{
		const float k_segments = 16.0f;
		const float k_increment = 2.0f * math::PI / k_segments;
		float theta = 0.0f;
		renderer::OpenGL::GetInstance()->SetColour4ub( 255,255,255,255 );

		math::Vec2 vaPoints[16];	
		for (int j = 0; j < static_cast<int>(k_segments); ++j)
		{
			math::Vec2 starPos( m_StarLocation.pos.X, m_StarLocation.pos.Y );
			math::Vec2 v = starPos + (math::Vec2(std::cos(theta), std::sin(theta)) * m_StarLocation.s.fRadius);
			vaPoints[j] = v;
			theta += k_increment;
		}
			
		glVertexPointer(2, GL_FLOAT, sizeof(math::Vec2), vaPoints);
		glDrawArrays(GL_LINE_LOOP, 0, 16 );
	}

	for( i=0; i < m_GravityAreas.size(); ++i )
	{
		math::Vec2 vaSegmentPoints[4];

		renderer::OpenGL::GetInstance()->SetColour4ub( 255,0,0,255 );

		if( m_GravityAreas[i].type == eGravityType_Vacuum )
			renderer::OpenGL::GetInstance()->SetColour4ub( 0,0,0,255 );
		else if( m_GravityAreas[i].type == eGravityType_Air )
			renderer::OpenGL::GetInstance()->SetColour4ub( 255,255,255,255 );
		else if( m_GravityAreas[i].type == eGravityType_Water )
			renderer::OpenGL::GetInstance()->SetColour4ub( 0,0,255,255 );

		vaSegmentPoints[0] = math::Vec2( m_GravityAreas[i].pos.X - m_GravityAreas[i].dim.Width*0.5f, m_GravityAreas[i].pos.Y - m_GravityAreas[i].dim.Height*0.5f );
		vaSegmentPoints[1] = math::Vec2( m_GravityAreas[i].pos.X + m_GravityAreas[i].dim.Width*0.5f, m_GravityAreas[i].pos.Y - m_GravityAreas[i].dim.Height*0.5f );
		vaSegmentPoints[2] = math::Vec2( m_GravityAreas[i].pos.X + m_GravityAreas[i].dim.Width*0.5f, m_GravityAreas[i].pos.Y + m_GravityAreas[i].dim.Height*0.5f );
		vaSegmentPoints[3] = math::Vec2( m_GravityAreas[i].pos.X - m_GravityAreas[i].dim.Width*0.5f, m_GravityAreas[i].pos.Y + m_GravityAreas[i].dim.Height*0.5f );

		glVertexPointer(2, GL_FLOAT, sizeof(math::Vec2), vaSegmentPoints);
		glDrawArrays(GL_LINE_LOOP, 0, 4 );
	}

	for( i=0; i < m_ForceAreas.size(); ++i )
	{
		math::Vec2 vaSegmentPoints[4];

		renderer::OpenGL::GetInstance()->SetColour4ub( 255,255,0,255 );

		vaSegmentPoints[0] = math::Vec2( m_ForceAreas[i].pos.X - m_ForceAreas[i].dim.Width*0.5f, m_ForceAreas[i].pos.Y - m_ForceAreas[i].dim.Height*0.5f );
		vaSegmentPoints[1] = math::Vec2( m_ForceAreas[i].pos.X + m_ForceAreas[i].dim.Width*0.5f, m_ForceAreas[i].pos.Y - m_ForceAreas[i].dim.Height*0.5f );
		vaSegmentPoints[2] = math::Vec2( m_ForceAreas[i].pos.X + m_ForceAreas[i].dim.Width*0.5f, m_ForceAreas[i].pos.Y + m_ForceAreas[i].dim.Height*0.5f );
		vaSegmentPoints[3] = math::Vec2( m_ForceAreas[i].pos.X - m_ForceAreas[i].dim.Width*0.5f, m_ForceAreas[i].pos.Y + m_ForceAreas[i].dim.Height*0.5f );

		glVertexPointer(2, GL_FLOAT, sizeof(math::Vec2), vaSegmentPoints);
		glDrawArrays(GL_LINE_LOOP, 0, 4 );
	}

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

	for( i=0; i < m_Water.size(); ++i )
	{
		renderer::OpenGL::GetInstance()->SetColour4ub( 64,128,124,255 );
		if(m_Water[i].grid != 0 )
		{
			glPushMatrix();
				m_Water[i].grid->DrawDebug();
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
		int playerSpawnDirection = Player::PLAYERSPAWN_LEFT;
		if( rot.Y != 0.0f )
			playerSpawnDirection = Player::PLAYERSPAWN_RIGHT;

		m_PlayerStartPoint = math::Vec2( pos.X, pos.Y );
		m_PlayerSpawnDir = playerSpawnDirection;
	}
	else if( baseId == 1 )
	{
		StarLocator starLoc;
		starLoc.pos = pos;
		starLoc.rot = rot;
		starLoc.s.vCenterPoint = pos;
		starLoc.s.fRadius = m_GameData.ARCADE_STAR_COLLISION_RADIUS;
		m_StarLocators.push_back(starLoc);
	}
}

/////////////////////////////////////////////////////
/// Method: ParseAnimatedMesh
/// Params: 
///
/////////////////////////////////////////////////////
void Level::ParseAnimatedMesh( int paramIndex, int baseType, int baseId )
{
	math::Vec3 pos;
	math::Vec3 rot;

	// all types have position
	pos = ParsePosition( paramIndex );

	rot.X = static_cast<float>( script::LuaGetNumberFromTableItem( "rotX", paramIndex, 0.0 ) );
	rot.Y = static_cast<float>( script::LuaGetNumberFromTableItem( "rotY", paramIndex, 0.0 ) );
	rot.Z = static_cast<float>( script::LuaGetNumberFromTableItem( "rotZ", paramIndex, 0.0 ) );

	int animResId = static_cast<int>( script::LuaGetNumberFromTableItem( "animResId", paramIndex, -1 ) );
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
/// Method: ParseEnemySpawn
/// Params: 
///
/////////////////////////////////////////////////////
void Level::ParseEnemySpawn( int paramIndex, int baseType, int baseId )
{
	math::Vec3 pos;
	math::Vec2 dims(1.0f,1.0f);

	// all types have position
	pos = ParsePosition( paramIndex );

	int spawnType = static_cast<int>( script::LuaGetNumberFromTableItem( "spawnType", paramIndex, 0.0 ) );	
	float w = static_cast<float>( script::LuaGetNumberFromTableItem( "w", paramIndex, 1.0 ) );
	float h = static_cast<float>( script::LuaGetNumberFromTableItem( "h", paramIndex, 1.0 ) );
	int moveDir = static_cast<int>( script::LuaGetNumberFromTableItem( "moveDir", paramIndex, 0.0 ) );	
	int startPoint = static_cast<int>( script::LuaGetNumberFromTableItem( "startPoint", paramIndex, 0.0 ) );	
	float moveSpeed = static_cast<float>( script::LuaGetNumberFromTableItem( "moveSpeed", paramIndex, 5.0 ) );
	float rotateSpeed = static_cast<float>( script::LuaGetNumberFromTableItem( "rotateSpeed", paramIndex, 5.0 ) );
	bool respawn = static_cast<bool>( script::LuaGetBoolFromTableItem( "respawn", paramIndex, 1.0 ) );
	float respawnTime = static_cast<float>( script::LuaGetNumberFromTableItem( "respawnTime", paramIndex, 5.0 ) );

	dims = math::Vec2(w,h);

	switch( spawnType )
	{
#ifdef ALLOW_UNUSED_OBJECTS
		case BaseEnemy::ENEMYTYPE_DUSTDEVIL:
		{
			DustDevil* pEnemy = 0;
			pEnemy = new DustDevil();
			DBG_ASSERT( pEnemy != 0 );

			pEnemy->Create( pos, dims );

			m_EnemyList.push_back( pEnemy );
		}break;
		case BaseEnemy::ENEMYTYPE_PIRANHA:
		{
			Piranha* pEnemy = 0;
			pEnemy = new Piranha();
			DBG_ASSERT( pEnemy != 0 );

			pEnemy->Create( pos, dims );

			m_EnemyList.push_back( pEnemy );
		}break;
		case BaseEnemy::ENEMYTYPE_RAT:
		{
			Rat* pEnemy = 0;
			pEnemy = new Rat();
			DBG_ASSERT( pEnemy != 0 );

			pEnemy->Create( pos, dims );

			m_EnemyList.push_back( pEnemy );
		}break;
		case BaseEnemy::ENEMYTYPE_BOMB:
		{
			Bomb* pEnemy = 0;
			pEnemy = new Bomb();
			DBG_ASSERT( pEnemy != 0 );

			pEnemy->Create( pos, dims );

			m_EnemyList.push_back( pEnemy );
		}break;
		case BaseEnemy::ENEMYTYPE_BOUNCINGBOMB:
		{
			BouncingBomb* pEnemy = 0;
			pEnemy = new BouncingBomb();
			DBG_ASSERT( pEnemy != 0 );

			pEnemy->Create( pos, dims );

			m_EnemyList.push_back( pEnemy );
		}break;
		case BaseEnemy::ENEMYTYPE_PUFFERFISH:
		{
			PufferFish* pEnemy = 0;
			pEnemy = new PufferFish();
			DBG_ASSERT( pEnemy != 0 );

			pEnemy->Create( pos, dims );

			m_EnemyList.push_back( pEnemy );
		}break;
#endif // ALLOW_UNUSED_OBJECTS
		case BaseEnemy::ENEMYTYPE_FISH:
		{
			Fish* pEnemy = 0;
			pEnemy = new Fish();
			DBG_ASSERT( pEnemy != 0 );

			pEnemy->Create( pos, dims );

			m_EnemyList.push_back( pEnemy );
		}break;
		case BaseEnemy::ENEMYTYPE_SENTRY:
		{
			Sentry* pEnemy = 0;
			pEnemy = new Sentry();
			DBG_ASSERT( pEnemy != 0 );

			pEnemy->Create( pos, dims, moveDir, startPoint, moveSpeed, rotateSpeed );

			m_EnemyList.push_back( pEnemy );
		}break;
		case BaseEnemy::ENEMYTYPE_MINE:
		{
			Mine* pEnemy = 0;
			pEnemy = new Mine();
			DBG_ASSERT( pEnemy != 0 );

			pEnemy->Create( pos, dims, (respawn != 0), respawnTime );

			m_EnemyList.push_back( pEnemy );
		}break;

		default:
			DBG_ASSERT(0);
		break;
	}
}

/////////////////////////////////////////////////////
/// Method: ParseRotatingShape
/// Params: 
///
/////////////////////////////////////////////////////
void Level::ParseRotatingShape( int paramIndex, int baseType, int baseId )
{
	math::Vec3 pos;

	// all types have position
	pos = ParsePosition( paramIndex );

	int spawnType = static_cast<int>( script::LuaGetNumberFromTableItem( "spawnType", paramIndex, 0.0 ) );	
	int rotType = static_cast<int>( script::LuaGetNumberFromTableItem( "rotType", paramIndex, 0.0 ) );	
	float rotMotorSpeed = static_cast<float>( script::LuaGetNumberFromTableItem( "rotMotorSpeed", paramIndex, 1.0 ) );
	float rotPauseTime = static_cast<float>( script::LuaGetNumberFromTableItem( "rotPauseTime", paramIndex, 2.0 ) );

	switch( spawnType )
	{
		case ROTATING_HSHAPE:
		{
			HShape* pShape = 0;
			pShape = new HShape();
			DBG_ASSERT( pShape != 0 );

			pShape->Create( pos, static_cast<BaseShape::eRotationType>(rotType), rotMotorSpeed, rotPauseTime );

			// add to the list
			m_ShapeList.push_back( pShape );
		}break;
#ifdef ALLOW_UNUSED_OBJECTS
		case ROTATING_ISHAPE:
		{
			IShape* pShape = 0;
			pShape = new IShape();
			DBG_ASSERT( pShape != 0 );

			pShape->Create( pos, static_cast<BaseShape::eRotationType>(rotType), rotMotorSpeed, rotPauseTime );

			// add to the list
			m_ShapeList.push_back( pShape );
		}break;
		case ROTATING_USHAPE:
		{
			UShape* pShape = 0;
			pShape = new UShape();
			DBG_ASSERT( pShape != 0 );

			pShape->Create( pos, static_cast<BaseShape::eRotationType>(rotType), rotMotorSpeed, rotPauseTime );

			// add to the list
			m_ShapeList.push_back( pShape );
		}break;
		case ROTATING_XSHAPE:
		{
			XShape* pShape = 0;
			pShape = new XShape();
			DBG_ASSERT( pShape != 0 );

			pShape->Create( pos, static_cast<BaseShape::eRotationType>(rotType), rotMotorSpeed, rotPauseTime );

			// add to the list
			m_ShapeList.push_back( pShape );
		}break;
#endif // ALLOW_UNUSED_OBJECTS
		case ROTATING_YSHAPE:
		{
			YShape* pShape = 0;
			pShape = new YShape();
			DBG_ASSERT( pShape != 0 );

			pShape->Create( pos, static_cast<BaseShape::eRotationType>(rotType), rotMotorSpeed, rotPauseTime );

			// add to the list
			m_ShapeList.push_back( pShape );
		}break;
		default:
			DBG_ASSERT(0);
		break;
	}
}

/////////////////////////////////////////////////////
/// Method: ParseMovingShape
/// Params: 
///
/////////////////////////////////////////////////////
void Level::ParseMovingShape( int paramIndex, int baseType, int baseId )
{
	math::Vec3 pos;

	// all types have position
	pos = ParsePosition( paramIndex );

	int spawnType = static_cast<int>( script::LuaGetNumberFromTableItem( "spawnType", paramIndex, 0.0 ) );	
	float moveExtendMotorSpeed = static_cast<float>( script::LuaGetNumberFromTableItem( "moveExtendMotorSpeed", paramIndex, 0.5 ) );
	float moveRetractMotorSpeed = static_cast<float>( script::LuaGetNumberFromTableItem( "moveRetractMotorSpeed", paramIndex, 0.5 ) );
	float moveExtendWait = static_cast<float>( script::LuaGetNumberFromTableItem( "moveExtendWait", paramIndex, 0.0 ) );
	float moveRetractWait = static_cast<float>( script::LuaGetNumberFromTableItem( "moveRetractWait", paramIndex, 0.0 ) );

	switch( spawnType )
	{
		case MOVING_RIGHT:
		{
			DoorSlideRight* pShape = 0;
			pShape = new DoorSlideRight();
			DBG_ASSERT( pShape != 0 );

			pShape->Create( pos, moveExtendMotorSpeed, moveRetractMotorSpeed, moveExtendWait, moveRetractWait );

			// add to the list
			m_DoorSlideList.push_back( pShape );
		}break;
		case MOVING_LEFT:
		{
			DoorSlideLeft* pShape = 0;
			pShape = new DoorSlideLeft();
			DBG_ASSERT( pShape != 0 );

			pShape->Create( pos, moveExtendMotorSpeed, moveRetractMotorSpeed, moveExtendWait, moveRetractWait );

			// add to the list
			m_DoorSlideList.push_back( pShape );
		}break;
		case MOVING_UP:
		{
			DoorSlideUp* pShape = 0;
			pShape = new DoorSlideUp();
			DBG_ASSERT( pShape != 0 );

			pShape->Create( pos, moveExtendMotorSpeed, moveRetractMotorSpeed, moveExtendWait, moveRetractWait );

			// add to the list
			m_DoorSlideList.push_back( pShape );
		}break;
		case MOVING_DOWN:
		{
			DoorSlideDown* pShape = 0;
			pShape = new DoorSlideDown();
			DBG_ASSERT( pShape != 0 );

			pShape->Create( pos, moveExtendMotorSpeed, moveRetractMotorSpeed, moveExtendWait, moveRetractWait );

			// add to the list
			m_DoorSlideList.push_back( pShape );
		}break;
		default:
			DBG_ASSERT(0);
		break;
	}
}

/////////////////////////////////////////////////////
/// Method: ParsePort
/// Params: 
///
/////////////////////////////////////////////////////
void Level::ParsePort( int paramIndex, int baseType, int baseId )
{
	math::Vec3 pos;

	// all types have position
	pos = ParsePosition( paramIndex );

	int portType = static_cast<int>( script::LuaGetNumberFromTableItem( "portType", paramIndex, 0.0 ) );	
	float w = static_cast<float>( script::LuaGetNumberFromTableItem( "w", paramIndex, 1.0 ) );
	float h = static_cast<float>( script::LuaGetNumberFromTableItem( "h", paramIndex, 1.0 ) );
	int num = static_cast<int>( script::LuaGetNumberFromTableItem( "num", paramIndex, 0.0 ) );	
	int size = static_cast<int>( script::LuaGetNumberFromTableItem( "size", paramIndex, 0.0 ) );	

	Port* pPort = 0;
	pPort = new Port();
	DBG_ASSERT( pPort != 0 );

	pPort->Create( pos, static_cast<BasePort::ePortTypes>(portType), num, size );

	m_PortList.push_back( pPort );

	PortArea newArea;
	newArea.pos = pos;
	newArea.dim = math::Vec2(w,h);
	newArea.type = static_cast<ePortType>(portType);
	newArea.numeric = num;
	m_PortAreas.push_back(newArea);
}

/////////////////////////////////////////////////////
/// Method: ParseGravityArea
/// Params: 
///
/////////////////////////////////////////////////////
void Level::ParseGravityArea( int paramIndex, int baseType, int baseId )
{
	math::Vec3 pos;

	// all types have position
	pos = ParsePosition( paramIndex );

	int gravityType = static_cast<int>( script::LuaGetNumberFromTableItem( "gravityType", paramIndex, 0.0 ) );	
	float w = static_cast<float>( script::LuaGetNumberFromTableItem( "w", paramIndex, 1.0 ) );
	float h = static_cast<float>( script::LuaGetNumberFromTableItem( "h", paramIndex, 1.0 ) );

	GravityArea newArea;
	newArea.pos = pos;
	newArea.dim = math::Vec2(w,h);
	newArea.type = static_cast<eGravityType>(gravityType);
	m_GravityAreas.push_back( newArea );
}	

/////////////////////////////////////////////////////
/// Method: ParseForceArea
/// Params: 
///
/////////////////////////////////////////////////////
void Level::ParseForceArea( int paramIndex, int baseType, int baseId )
{
	math::Vec3 pos;

	// all types have position
	pos = ParsePosition( paramIndex );

	float w = static_cast<float>( script::LuaGetNumberFromTableItem( "w", paramIndex, 1.0 ) );
	float h = static_cast<float>( script::LuaGetNumberFromTableItem( "h", paramIndex, 1.0 ) );
	float forceX = static_cast<float>( script::LuaGetNumberFromTableItem( "forceX", paramIndex, 0.0 ) );
	float forceY = static_cast<float>( script::LuaGetNumberFromTableItem( "forceY", paramIndex, 0.0 ) );
	float timeOn = static_cast<float>( script::LuaGetNumberFromTableItem( "timeOn", paramIndex, 0.0 ) );
	float timeOff = static_cast<float>( script::LuaGetNumberFromTableItem( "timeOff", paramIndex, 0.0 ) );

	ForceArea newArea;
	newArea.pos = pos;
	newArea.dim = math::Vec2(w,h);
	newArea.force = math::Vec2(forceX,forceY);
	newArea.forceTimeOn = timeOn;
	newArea.forceTimeOff = timeOff;

	m_ForceAreas.push_back( newArea );
}

/////////////////////////////////////////////////////
/// Method: ParseWaterGrid
/// Params: 
///
/////////////////////////////////////////////////////
void Level::ParseWaterGrid( int paramIndex, int baseType, int baseId )
{
	math::Vec3 pos;

	// all types have position
	pos = ParsePosition( paramIndex );

	int w = static_cast<int>( script::LuaGetNumberFromTableItem( "w", paramIndex, 10.0 ) );
	int d = static_cast<int>( script::LuaGetNumberFromTableItem( "d", paramIndex, 10.0 ) );
	int widthPoints = static_cast<int>( script::LuaGetNumberFromTableItem( "widthPoints", paramIndex, 10.0 ) );
	int depthPoints = static_cast<int>( script::LuaGetNumberFromTableItem( "depthPoints", paramIndex, 10.0 ) );
	float sinAngle = static_cast<float>( script::LuaGetNumberFromTableItem( "sinAngle", paramIndex, 180.0 ) );
	float waveUpdateTime = static_cast<float>( script::LuaGetNumberFromTableItem( "waveUpdateTime", paramIndex, 0.1 ) );

	WaterGrid newGrid;
	pos.X = pos.X - (static_cast<float>(w)*0.5f);
	pos.Z = pos.Z - (static_cast<float>(d)*0.5f);
	newGrid.pos = pos;
	newGrid.grid = new WaveGrid( pos, w, d, widthPoints, depthPoints, sinAngle, waveUpdateTime );
	DBG_ASSERT( newGrid.grid != 0 );

	m_Water.push_back(newGrid);
}