
/*===================================================================
	File: Player.cpp
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "SoundBase.h"
#include "CollisionBase.h"
#include "Model/ModelCommon.h"
#include "ModelBase.h"
#include "InputBase.h"
#include "ScriptBase.h"

#include "Camera/Camera.h"
#include "Camera/DebugCamera.h"

#include "CabbyConsts.h"
#include "Cabby.h"

#include "Audio/CustomerAudio.h"
#include "Profiles/ProfileManager.h"

#include "Resources/EmitterResources.h"
#include "Resources/ModelResources.h"
#include "Resources/TextureResources.h"

#include "Physics/PhysicsWorld.h"
#include "Physics/Enemies/Bomb.h"
#include "Physics/Enemies/BouncingBomb.h"
#include "Physics/Enemies/DustDevil.h"
#include "Physics/Enemies/Fish.h"
#include "Physics/Enemies/Mine.h"
#include "Physics/Enemies/Piranha.h"
#include "Physics/Enemies/PufferFish.h"
#include "Physics/Enemies/Rat.h"
#include "Physics/Enemies/Sentry.h"

#include "Player/Player.h"

namespace
{
	GLfloat m[16] = 
	{ 1.0f, 0.0f, 0.0f, 0.0f,
	  0.0f, 1.0f, 0.0f, 0.0f,
	  0.0f, 0.0f, 1.0f, 0.0f,
	  0.0f, 0.0f, 0.0f, 1.0f };
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
Player::Player( InputSystem& inputSystem, ScriptDataHolder::VehicleScriptData& vehicleData, Level& taxiLevel )
	: PhysicsIdentifier( PHYSICSBASICID_PLAYER, PHYSICSCASTID_PLAYER ) 
	, m_InputSystem(inputSystem)
	, m_VehicleData(vehicleData)
	, m_TaxiLevel(taxiLevel)
{
	m_InputSystem.ResetAllInput();

	m_pScriptData = GetScriptDataHolder();
	m_GameData = m_pScriptData->GetGameData();
	m_DevData = m_pScriptData->GetDevData();

	m_LastDelta = 0.0f;
	m_pModelData = 0;

	m_pCrashModel = 0;
	m_pCrashAnim = 0;
	m_CrashRotation = 0.0f;
	m_CrashPositionDriver = math::Vec3(0.0f,0.0f,0.0f);
	m_CrashPositionPassenger = math::Vec3(0.0f,0.0f,0.0f);
	m_CrashVelocity = math::Vec3(0.0f,0.0f,0.0f);
	m_HadCustomerOnCrash = false;
	m_PlayerTexture = res::GetTextureResource( Customer::CustomerValue_MaleLow )->texId;
	m_CustomerModelIndex = -1;
	m_CustomerTexture = renderer::INVALID_OBJECT;

	m_pPlayerBody = 0;
	m_pLandingGearShape = 0;

	m_SmoothRotation = false;
	m_UpwardsForce = 1.0f;
	m_MoveForce = 1.0f;
	m_MaxXVel = 1.0f;
	m_MinXVel = m_GameData.MIN_VEL;
	m_MaxYVel = 1.0f;
	m_CurrentUpForce = 0.0f;
	m_CurrentMoveForce = 0.0f;

	m_TargetRotation = 0.0f;
	m_RotationAngle = 0.0f;
	m_TargetConformAngle = 0.0f;
	m_ConformAngle = 0.0f;

	m_IsContactWorld = false;
	m_WorldEdgeContact = false;
	m_IsContactPort = false;
	m_IsContactEnemy = false;
	m_IsContactShape = false;
	m_IsOnGroundIdle = false;

	m_IsRefueling = false;
	m_IsRefusedFuel = false;
	m_OutOfFuel = false;
	m_SurvivedFuelRunout = false;
	m_FuelCount = m_GameData.MAX_FUEL;
	m_FuelUseTimer = 0.0f;
	m_FuelIncreaseTimer = 0.0f;
	m_RefuelZoneIndex = -1;

	m_LandingGear = false;
	m_LandedTime = 0.0f;

	m_BoundSphere.vCenterPoint = math::Vec3( 0.0f, 0.0f, 0.0f );
	m_BoundSphere.fRadius = 1.0f;

	m_AtmosType = Level::eGravityType_Air;
	m_OldAtmosType = Level::eGravityType_UnKnown;

	m_bInLandingZone = false;
	m_LandingType = Level::ePortType_UnKnown;
	m_LandingZoneIndex = (~0);

	m_CustomersComplete = 0;

	m_VacBufferId = snd::INVALID_SOUNDBUFFER;
	m_AirBufferId = snd::INVALID_SOUNDBUFFER;
	m_SubBufferId = snd::INVALID_SOUNDBUFFER;

	m_AtmosChangeAir2WaterId = snd::INVALID_SOUNDBUFFER;
	m_AtmosChangeWater2AirId = snd::INVALID_SOUNDBUFFER;
	m_LandingGearExtendId = snd::INVALID_SOUNDBUFFER;

	m_FuelLowId = snd::INVALID_SOUNDBUFFER;

	// create model and anims
	unsigned int i=0;

	m_pModelData = res::LoadModel( m_VehicleData.modelIndex );
	DBG_ASSERT( m_pModelData != 0 );

	// by default disable everything
	SetMeshDrawState( -1, false );

	// load crash models
	m_pCrashModel = new mdl::ModelHGA;
	DBG_ASSERT( m_pModelData != 0 );

	if( m_DevData.hiresCustomers )
		m_pCrashModel->LoadBindPose( "assets/models/customers/bind_high.hga" );
	else
		m_pCrashModel->LoadBindPose( "assets/models/customers/bind_low.hga" );

	// load just the flail anim
	m_pCrashModel->LoadAnimation( "assets/models/customers/anim_flail.hga", 0, true );

	// no culling as they are moving models
	m_pCrashModel->SetModelGeneralFlags( 0 );
	SetCrashAnimation( 0, 0, true );

	// main body
	if( m_VehicleData.mainBodyMesh != -1 )
		SetMeshDrawState( m_VehicleData.mainBodyMesh, true );

	// permanent meshes
	if( m_VehicleData.hasPermanentMeshes )
	{
		for( i=0; i < static_cast<unsigned int>(m_VehicleData.permanentSubMeshCount); ++i )
		{
			if( m_VehicleData.permanentSubMeshList[i] != -1 )
				SetMeshDrawState( m_VehicleData.permanentSubMeshList[i], true );
		}
	}

	// turn off landing gear
	if( m_VehicleData.landingGearMeshUp != -1 )
		SetMeshDrawState( m_VehicleData.landingGearMeshUp, true );
	if( m_VehicleData.landingGearMeshDown != -1 )
		SetMeshDrawState( m_VehicleData.landingGearMeshDown, false );

	// parcel/bomb/key
	if( m_VehicleData.parcelMesh != -1 )
		SetMeshDrawState( m_VehicleData.parcelMesh, false );
	if( m_VehicleData.bombMesh != -1 )
		SetMeshDrawState( m_VehicleData.bombMesh, false );
	if( m_VehicleData.keyMesh != -1 )
		SetMeshDrawState( m_VehicleData.keyMesh, false );

	// check level data for night time flag

		if( m_VehicleData.hasNightChanges )
		{
			for( i=0; i < static_cast<unsigned int>(m_VehicleData.nightSubMeshCount); ++i )
			{
				if( m_VehicleData.nightSubMeshList[i] != -1 )
					SetMeshDrawState( m_VehicleData.nightSubMeshList[i], false );
			}
		}

	//m_ShowSplash = false;
	m_SplashInTime = 0.0f;
	m_SplashOutTime = 0.0f;
	m_InWater = false;

	m_ShowCrash = false;
	m_CrashTime = 0.0f;
	m_ShowPortNumber = false;
	m_PortNumberTime = 0.0f;
	m_PortRequest = 0;

	m_ShowCustomerReward = false;
	m_CustomerRewardTime = 0.0f;
	m_CustomerReward = 0;

	m_ShowExtraLifeReward = false;
	m_ExtraLifeRewardTime = 0.0f;
	m_ExtraLifeReward = 0;

	m_HasPassenger = false;
	m_HasCargo = false;
	m_CargoDestination = -1;
	m_CargoTextureId = renderer::INVALID_OBJECT;

	m_HQZoneIndex = -1;

	// flight audio
	if( m_VehicleData.audioFileVac != 0 )
		m_VacBufferId = AudioSystem::GetInstance()->AddAudioFile( m_VehicleData.audioFileVac );
	else
		m_VacBufferId = AudioSystem::GetInstance()->AddAudioFile( 160/*"propul_vac.wav"*/ );

	if( m_VehicleData.audioFileAir != 0 )
		m_AirBufferId = AudioSystem::GetInstance()->AddAudioFile( m_VehicleData.audioFileAir );
	else
		m_AirBufferId = AudioSystem::GetInstance()->AddAudioFile( 162/*"propul_air.wav"*/ );

	if( m_VehicleData.audioFileSub != 0 )
		m_SubBufferId = AudioSystem::GetInstance()->AddAudioFile( m_VehicleData.audioFileSub );
	else
		m_SubBufferId = AudioSystem::GetInstance()->AddAudioFile( 161/*"propul_sub.wav"*/ );

	// atmos change
	m_AtmosChangeAir2WaterId = AudioSystem::GetInstance()->AddAudioFile( 150/*"atmos_air2water.wav"*/ );
	m_AtmosChangeWater2AirId = AudioSystem::GetInstance()->AddAudioFile( 151/*"atmos_water2air.wav"*/ );
	
	// landing gear extend 
	m_LandingGearExtendId = AudioSystem::GetInstance()->AddAudioFile( 159/*"land_ext.wav"*/ );

	// fuel aduio
	m_FuelFillId = AudioSystem::GetInstance()->AddAudioFile( 157/*"fuel_fill.wav"*/ ); 
	m_FuelLowId = AudioSystem::GetInstance()->AddAudioFile( 158/*"fuellow.wav"*/ ); 
	m_FuelRefuseId = AudioSystem::GetInstance()->AddAudioFile( 163/*"fuel_refuse.wav"*/ ); 

	// collision audio
	m_CollisionSmallId = AudioSystem::GetInstance()->AddAudioFile( 155/*"collide_small.wav"*/ ); 
	m_CollisionLargeId = AudioSystem::GetInstance()->AddAudioFile( 153/*"collide_large.wav"*/ ); 
	m_CollisionMassiveId = AudioSystem::GetInstance()->AddAudioFile( 154/*"collide_massive.wav"*/ ); 
	m_DestroyedId = AudioSystem::GetInstance()->AddAudioFile( 152/*"cab_destroyed.wav"*/ ); 

	m_UnProjectedPoint = math::Vec3( 0.0f, 0.0f, 0.0f );

	m_LivesCount = m_GameData.DEFAULT_LIVES_COUNT;
	m_TotalMoney = 0;
	m_MoneyToAdd = 0;
	m_CustomerFare = 0;

	m_Damage = 0.0f;
	m_IsDead = false;
	m_AllowCollisionSound = true;

	m_ThrustTiltAngle = 0.0f;
	m_ThrustTiltTarget = 0.0f;

	m_AirSettings = m_pScriptData->GetAirSettingsData();
	m_VacSettings = m_pScriptData->GetVacuumSettingsData();
	m_WaterSettings = m_pScriptData->GetWaterSettingsData();

	InitialiseEmitters();

	m_LightPos = math::Vec4( 0.0f, 1.0f, 0.0f, 0.0f );
	m_LightDiffuse = math::Vec4( 1.0f, 1.0f, 1.0f, 1.0f );
	m_LightAmbient = math::Vec4( 1.0f, 1.0f, 1.0f, 1.0f );
	m_LightSpecular = math::Vec4( 0.0f, 0.0f, 0.0f, 1.0f );

	m_LightPosTarget = math::Vec4( 0.0f, 1.0f, 0.0f, 0.0f );
	m_LightDiffuseTarget = math::Vec4( 1.0f, 1.0f, 1.0f, 1.0f );
	m_LightAmbientTarget = math::Vec4( 1.0f, 1.0f, 1.0f, 1.0f );
	m_LightSpecularTarget = math::Vec4( 0.0f, 0.0f, 0.0f, 1.0f );

	int areaIndex = 0;
	m_PortAreaList = m_TaxiLevel.GetPortAreaList();
	m_GravityAreaList = m_TaxiLevel.GetGravityAreaList();
	m_ForceAreaList = m_TaxiLevel.GetForceAreaList();

	std::vector<Level::PortArea>::iterator it = m_PortAreaList.begin();

	while( it != m_PortAreaList.end() )
	{
		math::Vec3 boxMin( (*it).pos.X - ((*it).dim.Width*0.5f), (*it).pos.Y - ((*it).dim.Height*0.5f), 0.0f );
		math::Vec3 boxMax( (*it).pos.X + ((*it).dim.Width*0.5f), (*it).pos.Y + ((*it).dim.Height*0.5f), 0.0f );

		collision::AABB aabb;
		aabb.vBoxMin = boxMin; 
		aabb.vBoxMax = boxMax;

		if( (*it).type == Level::ePortType_HQ )
			m_HQZoneIndex = areaIndex;
		if( (*it).type == Level::ePortType_Refuel )
			m_RefuelZoneIndex = areaIndex;

		areaIndex++;

		it++;
	}

	m_CrashMaleAudioId = snd::INVALID_SOUNDBUFFER;//AudioSystem::GetInstance()->AddAudioFile( 405/*"male-scream.wav"*/ );
	m_CrashFemaleAudioId = snd::INVALID_SOUNDBUFFER;//AudioSystem::GetInstance()->AddAudioFile( 402/*"female-scream_alt.wav"*/ );

	m_DriverScream = snd::INVALID_SOUNDSOURCE;
	m_DriverAscend = true;
	m_DriverScreamGain = 0.0f;

	m_PassengerScream = snd::INVALID_SOUNDSOURCE;
	m_PassengerAscend = true;
	m_PassengerScreamGain = 0.0f;
    
    m_SetWasPaused = false;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
Player::~Player()
{
	int i=0;

	StopAllAudio();

	if( m_pExplodeSparks != 0 )
	{
		delete m_pExplodeSparks;
		m_pExplodeSparks = 0;
	}
	if( m_pExplodeSmoke != 0 )
	{
		delete m_pExplodeSmoke;
		m_pExplodeSmoke = 0;
	}

	if( m_pWaterEnter != 0 )
	{
		delete m_pWaterEnter;
		m_pWaterEnter = 0;
	}

	if( m_pWaterExit != 0 )
	{
		delete m_pWaterExit;
		m_pWaterExit = 0;
	}

	if( m_pLowDamage != 0 )
	{
		delete m_pLowDamage;
		m_pLowDamage = 0;
	}
	if( m_pHighDamage != 0 )
	{
		delete m_pHighDamage;
		m_pHighDamage = 0;
	}
	if( m_pEmitHoriz != 0 )
	{
		delete m_pEmitHoriz;
		m_pEmitHoriz = 0;
	}
	for( i=0; i < m_GameData.VERTICAL_THRUSTERS; ++i )
	{
		if( m_pEmitVert[i] != 0 )
		{
			delete m_pEmitVert[i];
			m_pEmitVert[i] = 0;
		}
	}

	if( m_pEmitHorizWater != 0 )
	{
		delete m_pEmitHorizWater;
		m_pEmitHorizWater = 0;
	}
	for( i=0; i < m_GameData.VERTICAL_THRUSTERS; ++i )
	{
		if( m_pEmitVertWater[i] != 0 )
		{
			delete m_pEmitVertWater[i];
			m_pEmitVertWater[i] = 0;
		}
	}

	if( m_pCrashModel != 0 )
	{
		delete m_pCrashModel;
		m_pCrashModel = 0;
	}

	if( m_pModelData != 0 )
	{
		res::RemoveModel( m_pModelData );
		m_pModelData = 0;
	}
}

/////////////////////////////////////////////////////
/// Method: CreatePhysicsShapes
/// Params: [in]vehicleData
///
/////////////////////////////////////////////////////
void Player::Setup()
{
	b2FilterData fd;
	
	m_pPlayerBody = m_VehicleData.pBody;
	m_pPlayerBody->AllowSleeping(false);

	m_pLandingGearShapeDef = m_VehicleData.pLandingGearDef;

	m_pPlayerBody->SetUserData( reinterpret_cast<void *>(this) );

	m_pPlayerBody->SetFixedRotation(true);

	fd.categoryBits = PLAYER_CATEGORY;
	fd.maskBits = WORLD_CATEGORY;
	fd.groupIndex = PLAYER_GROUP;

	b2Shape* shapeList = m_pPlayerBody->GetShapeList();
	while( shapeList != 0 )
	{
		shapeList->SetFilterData(fd);

		shapeList = shapeList->GetNext();
	}
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void Player::Draw( bool disableMeshDraw, bool pauseFlag )
{
	ScriptDataHolder::PlayerScriptData playerData = m_pScriptData->GetPlayerData();

	m_SmoothRotation = playerData.smoothRotation;

	//math::Vec4 lightPos( m_Pos.X, m_Pos.Y, m_Pos.Z, 1.0f );
	//renderer::OpenGL::GetInstance()->SetLightPosition( 2, lightPos );	

	glPushMatrix();
		glTranslatef( m_pPlayerBody->GetWorldCenter().x, m_pPlayerBody->GetWorldCenter().y, 0.0f );

		if( /*!m_pPlayerBody->IsFixedRotation()*/1 )
		{
			const b2XForm xfm = m_pPlayerBody->GetXForm();
		
			m[0] = xfm.R.col1.x;	m[4] = xfm.R.col2.x;
			m[1] = xfm.R.col1.y;	m[5] = xfm.R.col2.y;
		
			glMultMatrixf( m );
		}
		else
		{
			if( playerData.conformToNormal )
			{
				b2Vec2 normal = PhysicsWorld::ContactNormal;

				// angle tolerance
				if( (normal.x >= -0.225f && normal.x <= 0.225f) &&
					(normal.y >= 0.4f) )
				{
					float angle = 0.0f;
					angle = std::atan2( normal.x, normal.y );

					m_TargetConformAngle = angle;
				}
				else
				{
					m_TargetConformAngle = 0.0f;
				}
			}
			glRotatef( math::RadToDeg(-m_ConformAngle), 0.0f, 0.0f, 1.0f );
		}
		
		glRotatef( m_RotationAngle, 0.0f, 1.0f, 0.0f );

		if( !disableMeshDraw )
			m_pModelData->Draw();
	glPopMatrix();

	//DrawEmitters();

	if( m_IsDead )
	{
		//renderer::OpenGL::GetInstance()->DisableLighting();
		renderer::OpenGL::GetInstance()->DepthMode( false, GL_ALWAYS );

		// disable all crash anim models
		m_pCrashModel->SetSkinDrawState( -1, false );		

		// active model to update
		m_pCrashModel->SetSkinDrawState( m_GameData.CUSTOMER_MALE_MESH, true );
		if( m_HadCustomerOnCrash )
		{
			if( m_CustomerModelIndex != -1 )
			{
				m_pCrashModel->SetSkinDrawState( m_CustomerModelIndex, true );
				m_pCrashModel->ChangeTextureOnMaterialId( m_CustomerModelIndex, 0, m_CustomerTexture );
			}
		}

		// update only active meshes
		if( !pauseFlag )
			UpdateCrashModel( m_LastDelta );

		// disable the player
		m_pCrashModel->SetSkinDrawState(m_GameData.CUSTOMER_MALE_MESH, false );

		// draw optional passenger
		
		if( m_HadCustomerOnCrash )
		{
			if( m_CustomerModelIndex != -1 )
			{
				m_pCrashModel->SetSkinDrawState( m_CustomerModelIndex, true );

				glPushMatrix();
					glTranslatef( m_CrashPositionPassenger.X, m_CrashPositionPassenger.Y, m_CrashPositionPassenger.Z );
					//glRotatef( -m_CrashRotation, 0.0f, 0.0f, 1.0f );
					//glTranslatef( 0.0, CUSTOMER_OFFSET, 0.0 );

					m_pCrashModel->Draw();
				glPopMatrix();

				m_pCrashModel->SetSkinDrawState( m_CustomerModelIndex, false );
			}
		}

		// always draw the driver
		m_pCrashModel->SetSkinDrawState( m_GameData.CUSTOMER_MALE_MESH, true );
		m_pCrashModel->ChangeTextureOnMaterialId( m_GameData.CUSTOMER_MALE_MESH, 0, m_PlayerTexture );
		glPushMatrix();
			glTranslatef( m_CrashPositionDriver.X, m_CrashPositionDriver.Y, m_CrashPositionDriver.Z );
			//glRotatef( m_CrashRotation, 0.0f, 0.0f, 1.0f );
			//glTranslatef( 0.0, CUSTOMER_OFFSET, 0.0 );

			m_pCrashModel->Draw();
		glPopMatrix();

		renderer::OpenGL::GetInstance()->DepthMode( true, GL_LESS );
		renderer::OpenGL::GetInstance()->EnableLighting();
	}

	//GLint viewport[4];
	//glGetIntegerv( GL_VIEWPORT, viewport );

	GLfloat *mdl = renderer::OpenGL::GetInstance()->GetModelViewMatrix();
	GLfloat *proj = renderer::OpenGL::GetInstance()->GetProjectionMatrix();
	GLint *viewport = renderer::OpenGL::GetInstance()->GetViewport();

	gluUtil::gluProjectf( m_Pos.X, m_Pos.Y, m_Pos.Z,
				mdl, proj, viewport,
				&m_UnProjectedPoint.X, &m_UnProjectedPoint.Y, &m_UnProjectedPoint.Z );
	
}

/////////////////////////////////////////////////////
/// Method: DrawNightMode
/// Params: None
///
/////////////////////////////////////////////////////
void Player::DrawNightMode()
{
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	renderer::OpenGL::GetInstance()->AlphaMode( true, GL_GREATER, 0.0f );
	int i=0;
	if( m_VehicleData.hasNightChanges && m_NightMode )
	{
		for( i=0; i < m_VehicleData.nightSubMeshCount; ++i )
		{
			if( m_VehicleData.nightSubMeshList[i] != -1 )
			{
				if( m_pModelData != 0 )
				{
					ScriptDataHolder::PlayerScriptData playerData = m_pScriptData->GetPlayerData();

					m_SmoothRotation = playerData.smoothRotation;

					//math::Vec4 lightPos( m_Pos.X, m_Pos.Y, m_Pos.Z, 1.0f );
					//renderer::OpenGL::GetInstance()->SetLightPosition( 2, lightPos );	

					glPushMatrix();
						glTranslatef( m_pPlayerBody->GetWorldCenter().x, m_pPlayerBody->GetWorldCenter().y, 0.0f );

						if( /*!m_pPlayerBody->IsFixedRotation()*/1 )
						{
							const b2XForm xfm = m_pPlayerBody->GetXForm();
		
							m[0] = xfm.R.col1.x;	m[4] = xfm.R.col2.x;
							m[1] = xfm.R.col1.y;	m[5] = xfm.R.col2.y;
		
							glMultMatrixf( m );
						}
						else
						{
							if( playerData.conformToNormal )
							{
								b2Vec2 normal = PhysicsWorld::ContactNormal;

								// angle tolerance
								if( (normal.x >= -0.225f && normal.x <= 0.225f) &&
									(normal.y >= 0.4f) )
								{
									float angle = 0.0f;
									angle = std::atan2( normal.x, normal.y );

									m_TargetConformAngle = angle;
								}
								else
								{
									m_TargetConformAngle = 0.0f;
								}
							}
							glRotatef( math::RadToDeg(-m_ConformAngle), 0.0f, 0.0f, 1.0f );
						}
		
						glRotatef( m_RotationAngle, 0.0f, 1.0f, 0.0f );

						//if( !disableMeshDraw )
						{
							//m_pModelData->Draw();

							SetMeshDrawState( m_VehicleData.nightSubMeshList[i], m_NightMode );
							m_pModelData->DrawMeshId( m_VehicleData.nightSubMeshList[i] );
							SetMeshDrawState( m_VehicleData.nightSubMeshList[i], false );
						}
					glPopMatrix();
				}
			}
		}
	}

	renderer::OpenGL::GetInstance()->AlphaMode( false, GL_ALWAYS, 0.0f );
	renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}

/////////////////////////////////////////////////////
/// Method: DrawDebug
/// Params: None
///
/////////////////////////////////////////////////////
void Player::DrawDebug()
{
	bool lightState = renderer::OpenGL::GetInstance()->GetLightingState();
	bool textureState = renderer::OpenGL::GetInstance()->GetTextureState();
	
	if( lightState )
		renderer::OpenGL::GetInstance()->DisableLighting();
	if( textureState )
		renderer::OpenGL::GetInstance()->DisableTexturing();
	
	const float k_segments = 16.0f;
	const float k_increment = 2.0f * math::PI / k_segments;
	float theta = 0.0f;
	renderer::OpenGL::GetInstance()->SetColour4ub( 255,255,255,255 );
	
	math::Vec3 vaPoints[16];	
	for (int j = 0; j < static_cast<int>(k_segments); ++j)
	{
		math::Vec3 v = m_BoundSphere.vCenterPoint + (math::Vec3(std::cos(theta), std::sin(theta), 0.0f) * m_BoundSphere.fRadius );
		vaPoints[j] = v;
		theta += k_increment;
	}
	
	glVertexPointer(3, GL_FLOAT, sizeof(math::Vec3), vaPoints);
	glDrawArrays(GL_LINE_LOOP, 0, 16 );
	
	if( lightState )
		renderer::OpenGL::GetInstance()->EnableLighting();
	if( textureState )
		renderer::OpenGL::GetInstance()->EnableTexturing();	
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void Player::Update( float deltaTime )
{
	int i=0;
	m_LastDelta = deltaTime;

	ProfileManager::Profile* pProfile = 0;
	pProfile = ProfileManager::GetInstance()->GetProfile();
	DBG_ASSERT( pProfile != 0 );
	
	if( m_IsDead )
	{
		m_CrashRotation += m_GameData.CRASH_ROTATION*deltaTime;

		m_CrashVelocity.Y -= 0.25f;

		m_CrashPositionDriver += (m_CrashVelocity*deltaTime);

		m_CrashPositionPassenger.X -= (m_CrashVelocity.X*deltaTime);

		m_CrashPositionPassenger.Y = m_CrashPositionDriver.Y;
		m_CrashPositionPassenger.Z = m_CrashPositionDriver.Z;

		if( m_DriverScream == snd::INVALID_SOUNDSOURCE  )
		{
			if( m_CrashMaleAudioId != snd::INVALID_SOUNDBUFFER )
			{
				m_DriverScreamGain = 0.6f;
				m_DriverAscend = true;
				m_DriverScream = snd::SoundManager::GetInstance()->GetFreeSoundSource();

				//if( m_DriverScream != snd::INVALID_SOUNDSOURCE )
				//	AudioSystem::GetInstance()->PlayAudio( m_DriverScream, m_CrashMaleAudioId, m_CrashPositionDriver, AL_FALSE, AL_FALSE, 1.1f, m_DriverScreamGain );	
			}
		}
		else
		{
			if( m_DriverAscend )
			{
				m_DriverScreamGain += 0.5f*deltaTime;
	
				if( m_DriverScreamGain > 1.0f )
				{
					m_DriverScreamGain = 1.0f;
					m_DriverAscend = false;
				}
			}
			else
			{
				m_DriverScreamGain -= 0.5f*deltaTime;

				if( m_DriverScreamGain < 0.0f )
					m_DriverScreamGain = 0.0f;
			}

			// update position
			CHECK_OPENAL_ERROR( alSourcef( m_DriverScream, AL_GAIN, m_DriverScreamGain) )
			CHECK_OPENAL_ERROR( alSource3f( m_DriverScream, AL_POSITION, m_CrashPositionDriver.X, m_CrashPositionDriver.Y, m_CrashPositionDriver.Z ) )
		}

		// passenger
		if( m_HadCustomerOnCrash )
		{
			if( m_PassengerScream == snd::INVALID_SOUNDSOURCE  )
			{
				if( m_CustomerGender == Customer::CUSTOMER_FEMALE )
				{
					if( m_CrashFemaleAudioId != snd::INVALID_SOUNDBUFFER )
					{
						m_PassengerScreamGain = math::RandomNumber( 0.0f, 0.1f );
						m_PassengerAscend = true;
						m_PassengerScream = snd::SoundManager::GetInstance()->GetFreeSoundSource();

						//if( m_PassengerScream != snd::INVALID_SOUNDSOURCE )
						//	AudioSystem::GetInstance()->PlayAudio( m_PassengerScream, m_CrashFemaleAudioId, m_CrashPositionPassenger, AL_FALSE, AL_FALSE, 1.5f, m_PassengerScreamGain );	
					}
				}
				else
				{
					if( m_CrashMaleAudioId != snd::INVALID_SOUNDBUFFER )
					{
						m_PassengerScreamGain = math::RandomNumber( 0.0f, 0.1f );
						m_PassengerAscend = true;
						m_PassengerScream = snd::SoundManager::GetInstance()->GetFreeSoundSource();

						//if( m_DriverScream != snd::INVALID_SOUNDSOURCE )
						//	AudioSystem::GetInstance()->PlayAudio( m_PassengerScream, m_CrashMaleAudioId, m_CrashPositionPassenger, AL_FALSE, AL_FALSE, 0.8f, m_PassengerScreamGain );	
					}
				}
			}
			else
			{
				if( m_PassengerAscend )
				{
					m_PassengerScreamGain += 0.5f*deltaTime;
		
					if( m_PassengerScreamGain > 1.0f )
					{
						m_PassengerScreamGain = 1.0f;
						m_PassengerAscend = false;
					}
				}
				else
				{
					m_PassengerScreamGain -= 0.5f*deltaTime;

					if( m_PassengerScreamGain < 0.0f )
						m_PassengerScreamGain = 0.0f;
				}

				// update position
				CHECK_OPENAL_ERROR( alSourcef( m_PassengerScream, AL_GAIN, m_PassengerScreamGain ) )
				CHECK_OPENAL_ERROR( alSource3f( m_PassengerScream, AL_POSITION, m_CrashPositionPassenger.X, m_CrashPositionPassenger.Y, m_CrashPositionPassenger.Z ) )
			}
		}
	}
	else
	{
		if( m_DriverScream != snd::INVALID_SOUNDSOURCE  )
		{
			AudioSystem::GetInstance()->StopAudio( m_DriverScream );	
			m_DriverScream = snd::INVALID_SOUNDSOURCE;
		}

		if( m_PassengerScream != snd::INVALID_SOUNDSOURCE  )
		{
			AudioSystem::GetInstance()->StopAudio( m_PassengerScream );	
			m_PassengerScream = snd::INVALID_SOUNDSOURCE;
		}

		//b2Vec2 normal = PhysicsWorld::ContactNormal;
		
		if( m_IsContactWorld ||
			m_IsContactPort )
		{
			//if( PhysicsWorld::ContactNormal.y >= 0.0f )
			m_pPlayerBody->SetFixedRotation(false);
		}
		else
			m_pPlayerBody->SetFixedRotation(true);
	}

	b2Vec2 linVel = m_pPlayerBody->GetLinearVelocity();
	float angVel = m_pPlayerBody->GetAngularVelocity();

	bool throttleState = m_InputSystem.GetThrottle();
	bool airBrakeState = m_InputSystem.GetAirBrake();
	bool leftState = m_InputSystem.GetLeft();
	bool rightState = m_InputSystem.GetRight();
	bool landingGearState = m_InputSystem.GetLandingGear();

    if( m_SetWasPaused )
        landingGearState = false;
    
	// no movement allowed during landing gear
	if( landingGearState ||
		m_LandingGear )
	{
		leftState = false;
		rightState = false;
	}

	/*if( throttleState )
	{
		static bool switchLight = false;
		static int framesToSwitch = 0;

		framesToSwitch++;
		if( framesToSwitch > 1 )
		{
			switchLight = !switchLight;
			framesToSwitch = 0;
		}

		if(switchLight)
			renderer::OpenGL::GetInstance()->EnableLight(2);
		else	
			renderer::OpenGL::GetInstance()->DisableLight(2);

		math::Vec4 lightDiffuse = math::Vec4 ( 1.0f, 1.0f, 0.0f, 1.0f );
		renderer::OpenGL::GetInstance()->SetLightDiffuse( 2, lightDiffuse );
		renderer::OpenGL::GetInstance()->SetLightAmbient( 2, lightDiffuse );
		renderer::OpenGL::GetInstance()->SetLightAttenuation( 2, GL_CONSTANT_ATTENUATION, 0.0f );
		renderer::OpenGL::GetInstance()->SetLightAttenuation( 2, GL_LINEAR_ATTENUATION, 0.0f );
		renderer::OpenGL::GetInstance()->SetLightAttenuation( 2, GL_QUADRATIC_ATTENUATION, 0.03f );
	}
	else
		renderer::OpenGL::GetInstance()->DisableLight(2);*/

	if( !m_IsDead && 
		!m_OutOfFuel &&
		throttleState )
	{
		if( m_CurrentUpForce < 0.0f )
			m_CurrentUpForce = 0.0f;

		if( linVel.y < m_MaxYVel )
		{
			//m_CurrentUpForce += m_UpwardsForce*deltaTime;
			//if( m_CurrentUpForce > m_UpwardsForce )
				m_CurrentUpForce = m_UpwardsForce;

			b2Vec2 f = b2Vec2(0.0f, m_CurrentUpForce);
			b2Vec2 p = m_pPlayerBody->GetWorldPoint( b2Vec2(0.0f, 0.0f) );
			m_pPlayerBody->ApplyForce( f, p );
		}

		m_FuelUseTimer += deltaTime;

		for( i=0; i < m_GameData.VERTICAL_THRUSTERS; ++i )
		{
			if( m_AtmosType == Level::eGravityType_Water )
			{
				if( m_pEmitVert[i] != 0 )
				{
					m_pEmitVert[i]->Disable();
					m_pEmitVert[i]->Stop();
				}

				if( m_pEmitVertWater[i] != 0 )
				{
					m_pEmitVertWater[i]->SetSpawnDir1( math::Vec3( 0.0f, -10.0f, 0.0f) );
					m_pEmitVertWater[i]->SetSpawnDir2( math::Vec3( 0.0f, -10.0f, 0.0f) );
					m_pEmitVertWater[i]->Enable();
				}
			}
			else
			{
				if( m_pEmitVertWater[i] != 0 )
					m_pEmitVertWater[i]->Disable();

				if( m_pEmitVert[i] != 0 )
				{
					m_pEmitVert[i]->SetSpawnDir1( math::Vec3( 0.0f, -10.0f, 0.0f) );
					m_pEmitVert[i]->SetSpawnDir2( math::Vec3( 0.0f, -10.0f, 0.0f) );
					m_pEmitVert[i]->Enable();
				}
			}
		}
	}
	else if( !m_IsDead && 
			!m_OutOfFuel &&
			airBrakeState )
	{
		if( m_CurrentUpForce > 0.0f )
			m_CurrentUpForce = 0.0f;

		if( linVel.y < m_MaxYVel )
		{
			//m_CurrentUpForce -= m_UpwardsForce*deltaTime;
			//if( m_CurrentUpForce < -m_UpwardsForce*0.5f )
				m_CurrentUpForce = -m_UpwardsForce*0.5f;

			b2Vec2 f = b2Vec2( 0.0f, m_CurrentUpForce );
			b2Vec2 p = m_pPlayerBody->GetWorldPoint( b2Vec2(0.0f, 0.0f) );
			m_pPlayerBody->ApplyForce( f, p );
		}

		for( i=0; i < m_GameData.VERTICAL_THRUSTERS; ++i )
		{
			if( m_AtmosType == Level::eGravityType_Water )
			{
				if( m_pEmitVert[i] != 0 )
				{
					m_pEmitVert[i]->Disable();
					m_pEmitVert[i]->Stop();
				}

				if( m_pEmitVertWater[i] != 0 )
				{
					m_pEmitVertWater[i]->SetSpawnDir1( math::Vec3( 0.0f, 0.0f, 0.0f) );
					m_pEmitVertWater[i]->SetSpawnDir2( math::Vec3( 0.0f, 0.0f, 0.0f) );
					m_pEmitVertWater[i]->Disable();
				}
			}
			else
			{
				if( m_pEmitVert[i] != 0 )
				{
					m_pEmitVert[i]->SetSpawnDir1( math::Vec3( 0.0f, 0.0f, 0.0f) );
					m_pEmitVert[i]->SetSpawnDir2( math::Vec3( 0.0f, 0.0f, 0.0f) );
					m_pEmitVert[i]->Disable();
				}
			}
		}
	}
	else
	{
//		if( m_CurrentUpForce > 5.0f )
//			m_CurrentUpForce -= 5.0f*deltaTime;
//		else if( m_CurrentUpForce < -5.0f )
//			m_CurrentUpForce += 5.0f*deltaTime;
//		else
//			m_CurrentUpForce = 0.0f;

		for( i=0; i < m_GameData.VERTICAL_THRUSTERS; ++i )
		{
			if( m_AtmosType == Level::eGravityType_Water )
			{
				if( m_pEmitVert[i] != 0 )
				{
					m_pEmitVert[i]->Disable();
					m_pEmitVert[i]->Stop();
				}

				if( m_pEmitVertWater[i] != 0 )
				{
					m_pEmitVertWater[i]->SetSpawnDir1( math::Vec3( 0.0f, 0.0f, 0.0f) );
					m_pEmitVertWater[i]->SetSpawnDir2( math::Vec3( 0.0f, 0.0f, 0.0f) );
					m_pEmitVertWater[i]->Disable();
				}
			}
			else
			{

				if( m_pEmitVert[i] != 0 )
				{
					m_pEmitVert[i]->SetSpawnDir1( math::Vec3( 0.0f, 0.0f, 0.0f) );
					m_pEmitVert[i]->SetSpawnDir2( math::Vec3( 0.0f, 0.0f, 0.0f) );
					m_pEmitVert[i]->Disable();
				}
			}
		}
	}

    /*if( core::app::GetAccelerometerState() )
    {
        if( input::gInputState.Accelerometers[1] > TILT_CONTROL_THRESHOLD )
        {
            leftState = true;
        }
        else if( input::gInputState.Accelerometers[1] < -TILT_CONTROL_THRESHOLD )
        {
            rightState = true;
        }
    }*/
    
	if( !m_IsDead &&
		!m_OutOfFuel &&
		(leftState || rightState) )
	{
		if( leftState )
		{
			// only allow turn/left when landing gear is up
			if( !m_LandingGear )
			{
				if( core::app::GetAccelerometerState() )
				{
					if( linVel.x > -m_MaxXVel )
					{
						if( input::gInputState.Accelerometers[1] > TILT_CONTROL_THRESHOLD ||
						   input::gInputState.Accelerometers[1] < -TILT_CONTROL_THRESHOLD )
						{
							m_CurrentMoveForce = (m_MoveForce * std::abs(input::gInputState.Accelerometers[1]))*2.0f;
							if( m_CurrentMoveForce > m_MoveForce )
								m_CurrentMoveForce = m_MoveForce;
							b2Vec2 f = b2Vec2(-m_CurrentMoveForce, 0.0f);
							b2Vec2 p = m_pPlayerBody->GetWorldPoint( b2Vec2(0.0f, 0.0f) );
							m_pPlayerBody->ApplyForce( f, p );
						
							m_FuelUseTimer += deltaTime;
						}
					}					
				}
				else
				{
					if( linVel.x > -m_MaxXVel )
					{
						b2Vec2 f = b2Vec2(-m_MoveForce, 0.0f);
						b2Vec2 p = m_pPlayerBody->GetWorldPoint( b2Vec2(0.0f, 0.0f) );
						m_pPlayerBody->ApplyForce( f, p );

						m_FuelUseTimer += deltaTime;
					}
				}

				if( m_AtmosType == Level::eGravityType_Water )
				{
					m_pEmitHoriz->Disable();

					m_pEmitHorizWater->SetSpawnDir1( math::Vec3( 5.0f, 0.0f, 0.0f) );
					m_pEmitHorizWater->SetSpawnDir2( math::Vec3( 5.0f, 0.0f, 0.0f) );
					m_pEmitHorizWater->SetGravity( math::Vec3(5.0f, 0.0f, 0.0f) );
					m_pEmitHorizWater->Enable();
				}
				else
				{
					m_pEmitHorizWater->Disable();

					m_pEmitHoriz->SetSpawnDir1( math::Vec3( 5.0f, 0.0f, 0.0f) );
					m_pEmitHoriz->SetSpawnDir2( math::Vec3( 5.0f, 0.0f, 0.0f) );
					m_pEmitHoriz->SetGravity( math::Vec3(5.0f, 0.0f, 0.0f) );
					m_pEmitHoriz->Enable();
				}

			}
		}
		else if( rightState )
		{
			// only allow turn/left when landing gear is up
			if( !m_LandingGear )
			{
				if( core::app::GetAccelerometerState() )
				{
					if( linVel.x < m_MaxXVel )
					{
						if( input::gInputState.Accelerometers[1] > TILT_CONTROL_THRESHOLD ||
						   input::gInputState.Accelerometers[1] < -TILT_CONTROL_THRESHOLD )
						{						
							m_CurrentMoveForce = (m_MoveForce * std::abs(input::gInputState.Accelerometers[1]))*2.0f;
							if( m_CurrentMoveForce > m_MoveForce )
								m_CurrentMoveForce = m_MoveForce;
							b2Vec2 f = b2Vec2(m_CurrentMoveForce, 0.0f);
							b2Vec2 p = m_pPlayerBody->GetWorldPoint( b2Vec2(0.0f, 0.0f) );
							m_pPlayerBody->ApplyForce( f, p );
						
							m_FuelUseTimer += deltaTime;
						}
					}					
				}
				else
				{				
					if( linVel.x < m_MaxXVel )
					{
						b2Vec2 f = b2Vec2(m_MoveForce, 0.0f);
						b2Vec2 p = m_pPlayerBody->GetWorldPoint( b2Vec2(0.0f, 0.0f) );
						m_pPlayerBody->ApplyForce( f, p );

						m_FuelUseTimer += deltaTime;
					}
				}
			
				if( m_AtmosType == Level::eGravityType_Water )
				{
					m_pEmitHoriz->Disable();

					m_pEmitHorizWater->SetSpawnDir1( math::Vec3( -5.0f, 0.0f, 0.0f) );
					m_pEmitHorizWater->SetSpawnDir2( math::Vec3( -5.0f, 0.0f, 0.0f) );
					m_pEmitHorizWater->SetGravity( math::Vec3(-5.0f, 0.0f, 0.0f) );
					m_pEmitHorizWater->Enable();
				}
				else
				{
					m_pEmitHorizWater->Disable();

					m_pEmitHoriz->SetSpawnDir1( math::Vec3( -5.0f, 0.0f, 0.0f) );
					m_pEmitHoriz->SetSpawnDir2( math::Vec3( -5.0f, 0.0f, 0.0f) );
					m_pEmitHoriz->SetGravity( math::Vec3(-5.0f, 0.0f, 0.0f) );
					m_pEmitHoriz->Enable();
				}
			}
		}			
	}
	else
	{
		if( m_AtmosType == Level::eGravityType_Water )
		{
			m_pEmitHorizWater->Disable();
			m_pEmitHorizWater->SetGravity( math::Vec3(0.0f, 0.0f, 0.0f) );
			m_pEmitHorizWater->SetSpawnDir1( math::Vec3( 0.0f, 0.0f, 0.0f) );
			m_pEmitHorizWater->SetSpawnDir2( math::Vec3( 0.0f, 0.0f, 0.0f) );
		}
		else
		{
			m_pEmitHoriz->Disable();
			m_pEmitHoriz->SetGravity( math::Vec3(0.0f, 0.0f, 0.0f) );
			m_pEmitHoriz->SetSpawnDir1( math::Vec3( 0.0f, 0.0f, 0.0f) );
			m_pEmitHoriz->SetSpawnDir2( math::Vec3( 0.0f, 0.0f, 0.0f) );
		}

		linVel = m_pPlayerBody->GetLinearVelocity();

		b2Vec2 v( linVel.x*m_GameData.SLOW_DOWN, linVel.y );
		m_pPlayerBody->SetLinearVelocity( v );
	}

	// when on ground ship will not rotate
	if( !m_IsContactWorld &&
		!m_IsDead )
	{
		linVel = m_pPlayerBody->GetLinearVelocity();

		if( linVel.x < -0.5f )
			SetRotationAngle( 0.0f );
		else if( linVel.x > 0.5f )
			SetRotationAngle( 180.0f );
	}

	if( !m_IsDead && landingGearState )
	{
		// retract
		if( m_LandingGear )
		{
			m_LandingGear = false;

			if( m_VehicleData.landingGearMeshDown != -1 )
				SetMeshDrawState( m_VehicleData.landingGearMeshDown, false );
			if( m_VehicleData.landingGearMeshUp != -1 )
				SetMeshDrawState( m_VehicleData.landingGearMeshUp, true );

			// landing gear
			if( m_pLandingGearShape != 0 )
			{
				m_pPlayerBody->DestroyShape(m_pLandingGearShape);
				m_pLandingGearShape = 0;

				m_pPlayerBody->WakeUp();
			}
		}
		else
		{
			// allow extension if there's no contact with the world or the contact point is not flat
			if( !m_IsContactWorld ||
				PhysicsWorld::ContactNormal.y < 0.95f )
			{
				// extend
				m_LandingGear = true;

				if( m_VehicleData.landingGearMeshDown != -1 )
					SetMeshDrawState( m_VehicleData.landingGearMeshDown, true );
				if( m_VehicleData.landingGearMeshUp != -1 )
					SetMeshDrawState( m_VehicleData.landingGearMeshUp, false );

				// re-create physics for legs
				{
					if( m_pLandingGearShape == 0 )
					{
						m_pLandingGearShape = m_pPlayerBody->CreateShape( m_pLandingGearShapeDef );
						b2FilterData fd;

						fd.categoryBits = PLAYER_CATEGORY;
						fd.maskBits = WORLD_CATEGORY;
						fd.groupIndex = PLAYER_GROUP;
						m_pLandingGearShape->SetFilterData(fd);

						m_pPlayerBody->WakeUp();
					}
				}

				m_OneShotSFX.nSourceID = snd::SoundManager::GetInstance()->GetFreeSoundSource();

				if( m_OneShotSFX.nSourceID != snd::INVALID_SOUNDSOURCE )
					AudioSystem::GetInstance()->PlayAudio( m_OneShotSFX.nSourceID, m_LandingGearExtendId, math::Vec3(0.0f,0.0f,0.0f), AL_TRUE, AL_FALSE );
			}
		}
	}

	// if landing gear is extended and there's contact with the world (retract)
	if( m_LandingGear )
	{
		// check for contact with the world
		if( m_IsContactWorld &&
			PhysicsWorld::ContactNormal.y >= 0.95f )
		{
			m_LandingGear = false;

			if( m_VehicleData.landingGearMeshDown != -1 )
				SetMeshDrawState( m_VehicleData.landingGearMeshDown, false );
			if( m_VehicleData.landingGearMeshUp != -1 )
				SetMeshDrawState( m_VehicleData.landingGearMeshUp, true );

			// landing gear
			if( m_pLandingGearShape != 0 )
			{
				m_pPlayerBody->DestroyShape(m_pLandingGearShape);
				m_pLandingGearShape = 0;

				m_pPlayerBody->WakeUp();
			}
		}
	}

	// draw vertical thrust
	if( !m_IsDead && 
		!m_OutOfFuel &&
		throttleState )
	{
		if( m_VehicleData.burnersVerticalMesh != -1 )
			SetMeshDrawState( m_VehicleData.burnersVerticalMesh, true );
		if( m_VehicleData.airBrakeMesh != -1 )
			SetMeshDrawState( m_VehicleData.airBrakeMesh, false );
	}
	else if( !m_IsDead && 
				!m_OutOfFuel &&
				airBrakeState )
	{
		if( m_VehicleData.burnersVerticalMesh != -1 )
			SetMeshDrawState( m_VehicleData.burnersVerticalMesh, false );
		if( m_VehicleData.airBrakeMesh != -1 )
			SetMeshDrawState( m_VehicleData.airBrakeMesh, true );
	}
	else
	{
		if( m_VehicleData.burnersVerticalMesh != -1 )
			SetMeshDrawState( m_VehicleData.burnersVerticalMesh, false );
		if( m_VehicleData.airBrakeMesh != -1 )
			SetMeshDrawState( m_VehicleData.airBrakeMesh, false );
	}

	// draw horizontal fire
	if( !m_IsDead && 
		!m_OutOfFuel &&
		!m_LandingGear && 
		(leftState || rightState)  )
	{
		if( m_VehicleData.burnersHorizontalMesh != -1 )
			SetMeshDrawState( m_VehicleData.burnersHorizontalMesh, true );
	}
	else
	{
		if( m_VehicleData.burnersHorizontalMesh != -1 )
			SetMeshDrawState( m_VehicleData.burnersHorizontalMesh, false );
	}

	// don't fall too fast
	linVel = m_pPlayerBody->GetLinearVelocity();

	// allow faster fall rate when air brake is on
	float maxFallIncrease = 0.0f;
	if( !m_IsDead && 
		!m_OutOfFuel &&
		airBrakeState )
		maxFallIncrease = 2.0f;

	if( linVel.y < m_MaxFallVel-maxFallIncrease )
	{
		b2Vec2 v( linVel.x, m_MaxFallVel-maxFallIncrease );
		m_pPlayerBody->SetLinearVelocity( v );
	}

	// try and get to the rotation smoother
	if( m_SmoothRotation )
	{
		if( m_RotationAngle != m_TargetRotation )
		{
			if( m_TargetRotation < m_RotationAngle )
				m_RotationAngle -= m_GameData.ROTATION_INCREMENT*deltaTime;
			else if( m_TargetRotation > m_RotationAngle )
				m_RotationAngle += m_GameData.ROTATION_INCREMENT*deltaTime;

			if( (m_RotationAngle-m_TargetRotation) < m_GameData.ROTATION_TOLERANCE &&
				(m_RotationAngle-m_TargetRotation) > -m_GameData.ROTATION_TOLERANCE )
					m_RotationAngle = m_TargetRotation;
		}

		// conforming to normal
		if( m_ConformAngle != m_TargetConformAngle )
		{
			if( m_TargetConformAngle < m_ConformAngle )
				m_ConformAngle -= m_GameData.ROTATION_INCREMENT_CONFORM*deltaTime;
			else if( m_TargetConformAngle > m_ConformAngle )
				m_ConformAngle += m_GameData.ROTATION_INCREMENT_CONFORM*deltaTime;

			if( (m_ConformAngle-m_TargetConformAngle) < 0.5f &&
				(m_ConformAngle-m_TargetConformAngle) > -0.5f )
					m_ConformAngle = m_TargetConformAngle;
		}
	}
	else
	{
		m_RotationAngle = m_TargetRotation;
		m_ConformAngle = m_TargetConformAngle;
	}

	b2Vec2 pos = m_pPlayerBody->GetWorldCenter();
	m_Pos = math::Vec3( pos.x, pos.y, m_Pos.Z );
	// bounding sphere
	m_BoundSphere.vCenterPoint = m_Pos;

	// update sound data
	ProcessSounds();

	// normal model update
	m_pModelData->Update( deltaTime );

	// update fuel gauge
	if( m_IsContactWorld && 
		m_bInLandingZone &&
		m_LandingType == Level::ePortType_Refuel )
	{
		if( (m_TotalMoney+m_MoneyToAdd) > (-m_GameData.FUEL_CHARGE) )
		{
			m_IsRefueling = true;
			m_IsRefusedFuel = false;

			// skin of my teeth award
#ifdef CABBY_LITE
			// ignore
#else
			if( m_FuelCount <= 1 )
				GameSystems::GetInstance()->AwardAchievement(20);
#endif // CABBY_LITE

			m_FuelIncreaseTimer += deltaTime;
			if( m_FuelIncreaseTimer >= m_GameData.FUEL_INCREASE_TIME )
			{
				m_FuelCount += 1;
				m_FuelIncreaseTimer = 0.0f;

				// charge for the fuel
				if( m_FuelCount < m_GameData.MAX_FUEL )
				{
					AddMoney( m_GameData.FUEL_CHARGE );

					ProfileManager::ActiveLevelData* pLevelData = ProfileManager::GetInstance()->GetActiveLevelData();
					pLevelData->fuelPurchased += (-m_GameData.FUEL_CHARGE);
				}
			}

			// reset use timer
			m_FuelUseTimer = 0.0f;
		}
		else
		{
			m_IsRefueling = false;
			m_IsRefusedFuel = true;
		}
	}
	else
	{
		m_IsRefueling = false;
		m_IsRefusedFuel = false;

		// not in contact
		m_FuelIncreaseTimer = 0.0f;

		if( m_FuelUseTimer >= m_GameData.FUEL_LOSS_TIME )
		{
			m_FuelCount -= 1;
			m_FuelUseTimer = 0.0f;
		}
	}

	if( m_FuelCount <= 0 )
	{
		m_FuelCount = 0;
		m_OutOfFuel = true;
	}
	else
		m_OutOfFuel = false;

	if( m_FuelCount > m_GameData.MAX_FUEL )
		m_FuelCount = m_GameData.MAX_FUEL;

	// see if the player is totally idle
	if( m_IsContactWorld )
	{
		linVel = m_pPlayerBody->GetLinearVelocity();
		float angle = math::RadToDeg( m_pPlayerBody->GetAngle() );

		if( (linVel.x <= 0.001f && linVel.x >= -0.001f) &&
			(linVel.y <= 0.001f && linVel.y >= -0.001f) )
		{
			if( angle <= 5.0f && angle >= -5.0f )
				m_IsOnGroundIdle = true;
		}
		else
			m_IsOnGroundIdle = false;
		
		if( m_bInLandingZone )
		{
			m_LandedTime += deltaTime;

			if( m_LandedTime > 1.0f )
				m_LandedTime = 1.0f;
		}

	}
	else
	{
		//m_LandedTime -= deltaTime;
		//if( m_LandedTime < 0.0f )
			m_LandedTime = 0.0f;

		m_IsOnGroundIdle = false;
	}

	// showing the splash graphics
	//if( m_ShowSplash )
	//{
	//	m_SplashTime -= deltaTime;
	//	if( m_SplashTime < 0.0f )
	//		m_ShowSplash = false;
	//}

	if( m_SplashInTime > 0.0f )
	{
		m_SplashInTime -= deltaTime;
		if( m_SplashInTime < 0.0f )
			m_SplashInTime = 0.0f;
	}
	if( m_SplashOutTime > 0.0f )
	{
		m_SplashOutTime -= deltaTime;
		if( m_SplashOutTime < 0.0f )
			m_SplashOutTime = 0.0f;
	}


	// show port request
	if( m_ShowPortNumber )
	{
		m_PortNumberTime -= deltaTime;
		if( m_PortNumberTime < 0.0f )
			m_ShowPortNumber = false;
	}

	// show extra reward
	if( m_ShowExtraLifeReward )
	{
		m_ExtraLifeRewardTime -= deltaTime;
		if( m_ExtraLifeRewardTime < 0.0f )
			m_ShowExtraLifeReward = false;
	}

	// show customer reward
	if( m_ShowCustomerReward )
	{
		m_CustomerRewardTime -= deltaTime;
		if( m_CustomerRewardTime < 0.0f )
			m_ShowCustomerReward = false;
	}

	// show/hide the cargo mesh
	if( m_HasCargo )
	{
		if( m_VehicleData.parcelMesh != -1 )
			SetMeshDrawState( m_VehicleData.parcelMesh, true );
	}
	else
	{
		if( m_VehicleData.parcelMesh != -1 )
			SetMeshDrawState( m_VehicleData.parcelMesh, false );
	}

	UpdateEmitters( deltaTime );

	if( m_Damage >= 100.0f )
	{
		if( !m_IsDead )
		{
			m_OneShotSFX.nSourceID = snd::SoundManager::GetInstance()->GetFreeSoundSource();

			if( m_OneShotSFX.nSourceID != snd::INVALID_SOUNDSOURCE )
				AudioSystem::GetInstance()->PlayAudio( m_OneShotSFX.nSourceID, m_DestroyedId, math::Vec3(0.0f,0.0f,0.0f), AL_TRUE, AL_FALSE );

			m_pExplodeSparks->SetPos( m_Pos );
			m_pExplodeSparks->Enable();
			m_pExplodeSparks->Disable();

			m_pExplodeSmoke->SetPos( math::Vec3(m_Pos.X, m_Pos.Y, 1.0f) );
			m_pExplodeSmoke->Enable();
			m_pExplodeSmoke->Disable();
		}
		m_Damage = 100.0f;

		// retract the landing gear if it was extended
		if(m_LandingGear)
		{
			m_LandingGear = false;

			if( m_VehicleData.landingGearMeshDown != -1 )
				SetMeshDrawState( m_VehicleData.landingGearMeshDown, false );
			if( m_VehicleData.landingGearMeshUp != -1 )
				SetMeshDrawState( m_VehicleData.landingGearMeshUp, true );

			// landing gear
			if( m_pLandingGearShape != 0 )
			{
				m_pPlayerBody->DestroyShape(m_pLandingGearShape);
				m_pLandingGearShape = 0;

				m_pPlayerBody->WakeUp();
			}
		}

		m_pPlayerBody->SetFixedRotation(false);

		m_IsDead = true;
	}

	if( !m_IsDead &&
		!m_IsContactWorld &&
		!m_IsContactPort &&
		m_pPlayerBody->IsFixedRotation() )
	{
		if( !m_IsDead && 
			!m_OutOfFuel )
		{
			if( throttleState && 
				leftState )
			{
				m_ThrustTiltTarget = m_GameData.MAX_TILT_ANGLE;

				// no angular velocity
				if( angVel != 0.0f )
					m_pPlayerBody->SetAngularVelocity(0.0f);
			}
			else if( throttleState && 
				rightState)
			{
				m_ThrustTiltTarget = -m_GameData.MAX_TILT_ANGLE;

				// no angular velocity
				if( angVel != 0.0f )
					m_pPlayerBody->SetAngularVelocity(0.0f);
			}
			else
			{
				m_ThrustTiltTarget = 0.0f;
				m_ThrustTiltAngle = math::RadToDeg( m_pPlayerBody->GetAngle() );

				if( m_ThrustTiltAngle > m_GameData.MAX_TILT_ANGLE )
				{
					m_ThrustTiltAngle = m_GameData.MAX_TILT_ANGLE;
					//m_pPlayerBody->SetXForm( m_pPlayerBody->GetWorldCenter(), math::DegToRad(MAX_TILT_ANGLE) );
				}
				else if( m_ThrustTiltAngle < -m_GameData.MAX_TILT_ANGLE )
				{
					m_ThrustTiltAngle = -m_GameData.MAX_TILT_ANGLE;
					//m_pPlayerBody->SetXForm( m_pPlayerBody->GetWorldCenter(), math::DegToRad(-MAX_TILT_ANGLE) );
				}

				if( throttleState )
				{
					m_pPlayerBody->SetXForm( m_pPlayerBody->GetWorldCenter(), math::DegToRad(m_ThrustTiltAngle) );
					m_pPlayerBody->SetAngularVelocity(0.0f);
				}
			}
		}
		else
			m_ThrustTiltTarget = 0.0f;

		if( m_ThrustTiltAngle != m_ThrustTiltTarget )
		{
			if( m_ThrustTiltTarget < m_ThrustTiltAngle )
				m_ThrustTiltAngle -= m_GameData.TILT_INCREMENT*deltaTime;
			else if( m_ThrustTiltTarget > m_ThrustTiltAngle )
				m_ThrustTiltAngle += m_GameData.TILT_INCREMENT*deltaTime;

			if( (m_ThrustTiltAngle-m_ThrustTiltTarget) < m_GameData.TILT_TOLERANCE &&
				(m_ThrustTiltAngle-m_ThrustTiltTarget) > -m_GameData.TILT_TOLERANCE )
			{
				m_ThrustTiltAngle = m_ThrustTiltTarget;
			}
			
			m_pPlayerBody->SetXForm( m_pPlayerBody->GetWorldCenter(), math::DegToRad(m_ThrustTiltAngle) );
		}			
	}

	if( m_WorldEdgeContact )
	{
		m_pPlayerBody->SetAngularVelocity(0.0f);
		m_pPlayerBody->SetXForm( m_pPlayerBody->GetWorldCenter(), 0.0f );
	}

	m_WorldEdgeContact = false;
}

/////////////////////////////////////////////////////
/// Method: CheckAgainstLevel
/// Params: [in]taxiLevel
///
/////////////////////////////////////////////////////
void Player::CheckAgainstLevel( Level& taxiLevel )
{
	int i=0;

	if( m_OldAtmosType != Level::eGravityType_UnKnown )
		m_OldAtmosType = m_AtmosType;
	else
	{	
		// astro disables
		for( i=0; i < m_VehicleData.astroSubMeshCount; ++i )
		{
			if( m_VehicleData.astroSubMeshList[i] != -1 )
				SetMeshDrawState( m_VehicleData.astroSubMeshList[i], false );
		}
				
		// aqua disables
		for( i=0; i < m_VehicleData.aquaSubMeshCount; ++i )
		{
			if( m_VehicleData.aquaSubMeshList[i] != -1 )
				SetMeshDrawState( m_VehicleData.aquaSubMeshList[i], false );
		}
				
		// aero enables
		for( i=0; i < m_VehicleData.aeroSubMeshCount; ++i )
		{
			if( m_VehicleData.aeroSubMeshList[i] != -1 )
				SetMeshDrawState( m_VehicleData.aeroSubMeshList[i], true );
		}
				
		m_MaxFallVel				= m_VehicleData.maxFallVelAir;
		m_MaxXVel					= m_VehicleData.maxXVelAir;
		m_MaxYVel					= m_VehicleData.maxYVelAir;

		m_UpwardsForce				= m_VehicleData.upwardsForceAir;
		m_MoveForce					= m_VehicleData.movementForceAir;

		PhysicsWorld::GetWorld()->SetGravity( m_AirSettings.gravity );

		m_OldAtmosType = Level::eGravityType_Air;
	}

	std::vector<Level::PortArea>::iterator portAreas = m_PortAreaList.begin();
	std::vector<Level::GravityArea>::iterator gravityAreas = m_GravityAreaList.begin();
	std::vector<Level::ForceArea>::iterator forceAreas = m_ForceAreaList.begin();

	bool landingZoneFlag = false;
	bool notInAtmosphere = true;
	bool insideForceArea = false;
	Level::ForceArea* pForceArea = 0;

	int areaIndex = 0;

	// check ports
	while( portAreas != m_PortAreaList.end() )
	{
		if( (*portAreas).type == Level::ePortType_HQ )
			m_HQZoneIndex = areaIndex;
		if( (*portAreas).type == Level::ePortType_Refuel )
			m_RefuelZoneIndex = areaIndex;

		math::Vec3 boxMin( (*portAreas).pos.X - ((*portAreas).dim.Width*0.5f), (*portAreas).pos.Y - ((*portAreas).dim.Height*0.5f), 0.0f );
		math::Vec3 boxMax( (*portAreas).pos.X + ((*portAreas).dim.Width*0.5f), (*portAreas).pos.Y + ((*portAreas).dim.Height*0.5f), 0.0f );

		collision::AABB aabb;
		aabb.vBoxMin = boxMin; 
		aabb.vBoxMax = boxMax;

		if( m_BoundSphere.SphereCollidesWithAABB( aabb ) )
		{
			// inside a landing zone
			if( (*portAreas).type == Level::ePortType_LandingZone )
			{
				m_LandingZoneIndex = areaIndex;
			}

			// inside a landing type
			if( (*portAreas).type == Level::ePortType_LandingZone ||
				(*portAreas).type == Level::ePortType_Refuel ||
				(*portAreas).type == Level::ePortType_HQ )
			{
				landingZoneFlag = true;
				
				m_LandingType = (*portAreas).type;
			}
		}

		areaIndex++;

		portAreas++;
	}

	// gravity

	// depending on direction, shift the bound area
	bool throttleState = m_InputSystem.GetThrottle();
	float oldCenterY = m_BoundSphere.vCenterPoint.Y;
	if( throttleState )
		m_BoundSphere.vCenterPoint.Y += 2.0f;
	else
		m_BoundSphere.vCenterPoint.Y -= 1.0f;

	while( gravityAreas != m_GravityAreaList.end() )
	{
		math::Vec3 boxMin( (*gravityAreas).pos.X - ((*gravityAreas).dim.Width*0.5f), (*gravityAreas).pos.Y - ((*gravityAreas).dim.Height*0.5f), 0.0f );
		math::Vec3 boxMax( (*gravityAreas).pos.X + ((*gravityAreas).dim.Width*0.5f), (*gravityAreas).pos.Y + ((*gravityAreas).dim.Height*0.5f), 0.0f );

		collision::AABB aabb;
		aabb.vBoxMin = boxMin; 
		aabb.vBoxMax = boxMax;

		if( m_BoundSphere.SphereCollidesWithAABB( aabb ) )
		{
			// inside an atmospheric type
			if( (*gravityAreas).type == Level::eGravityType_Vacuum ||
				(*gravityAreas).type == Level::eGravityType_Air ||
				(*gravityAreas).type == Level::eGravityType_Water )
			{
				m_AtmosType = (*gravityAreas).type;
				notInAtmosphere = false;
			}
		}

		// next
		gravityAreas++;
	}
	m_BoundSphere.vCenterPoint.Y = oldCenterY;

	while( forceAreas != m_ForceAreaList.end() )
	{
		math::Vec3 boxMin( (*forceAreas).pos.X - ((*forceAreas).dim.Width*0.5f), (*forceAreas).pos.Y - ((*forceAreas).dim.Height*0.5f), 0.0f );
		math::Vec3 boxMax( (*forceAreas).pos.X + ((*forceAreas).dim.Width*0.5f), (*forceAreas).pos.Y + ((*forceAreas).dim.Height*0.5f), 0.0f );

		collision::AABB aabb;
		aabb.vBoxMin = boxMin; 
		aabb.vBoxMax = boxMax;

		if( m_BoundSphere.SphereCollidesWithAABB( aabb ) )
		{
			// inside a force type
			pForceArea = &(*forceAreas);
			insideForceArea = true;
		}

		// next
		forceAreas++;
	}

	if( notInAtmosphere )
		m_AtmosType = Level::eGravityType_Air;

	if( landingZoneFlag )
		m_bInLandingZone = true;
	else
	{
		m_bInLandingZone = false;

		m_LandingZoneIndex = (~0);

		// can allow forces when not on a landing pad
		if( insideForceArea && pForceArea != 0 )
		{
			b2Vec2 affectForce( pForceArea->force.X, pForceArea->force.Y );
			
			b2Vec2 p = m_pPlayerBody->GetWorldPoint( b2Vec2(0.0f, 0.0f) );
			m_pPlayerBody->ApplyForce( affectForce, p );
		}
	}

	// there has been a change of atmosphere
	if( m_AtmosType != m_OldAtmosType )
	{
		switch( m_AtmosType )
		{
			case Level::eGravityType_Vacuum:
			{
				if( m_VehicleData.hasSubMeshChanges )
				{
					// astro disables
					for( i=0; i < m_VehicleData.astroSubMeshCount; ++i )
					{
						if( m_VehicleData.astroSubMeshList[i] != -1 )
							SetMeshDrawState( m_VehicleData.astroSubMeshList[i], true );
					}

					// aqua disables
					for( i=0; i < m_VehicleData.aquaSubMeshCount; ++i )
					{
						if( m_VehicleData.aquaSubMeshList[i] != -1 )
							SetMeshDrawState( m_VehicleData.aquaSubMeshList[i], false );
					}

					// aero enables
					for( i=0; i < m_VehicleData.aeroSubMeshCount; ++i )
					{
						if( m_VehicleData.aeroSubMeshList[i] != -1 )
							SetMeshDrawState( m_VehicleData.aeroSubMeshList[i], false );
					}
				}

				if( !m_IsDead ) // stop light changes on the vehicle when dead
					m_InWater = false;

				m_MaxFallVel				= m_VehicleData.maxFallVelVac;
				m_MaxXVel					= m_VehicleData.maxXVelVac;
				m_MaxYVel					= m_VehicleData.maxYVelVac;

				m_UpwardsForce				= m_VehicleData.upwardsForceVac;
				m_MoveForce					= m_VehicleData.movementForceVac;

				PhysicsWorld::GetWorld()->SetGravity( m_VacSettings.gravity );

				m_OldAtmosType = m_AtmosType;

			}break;
			case Level::eGravityType_Air:
			{
				if( m_VehicleData.hasSubMeshChanges )
				{
					// astro disables
					for( i=0; i < m_VehicleData.astroSubMeshCount; ++i )
					{
						if( m_VehicleData.astroSubMeshList[i] != -1 )
							SetMeshDrawState( m_VehicleData.astroSubMeshList[i], false );	
					}
					
					// aqua disables
					for( i=0; i < m_VehicleData.aquaSubMeshCount; ++i )
					{
						if( m_VehicleData.aquaSubMeshList[i] != -1 )
							SetMeshDrawState( m_VehicleData.aquaSubMeshList[i], false );
					}

					// aero enables
					for( i=0; i < m_VehicleData.aeroSubMeshCount; ++i )
					{
						if( m_VehicleData.aeroSubMeshList[i] != -1 )
							SetMeshDrawState( m_VehicleData.aeroSubMeshList[i], true );
					}
				}

				if( m_OldAtmosType == Level::eGravityType_Water )
				{
					m_OneShotSFX.nSourceID = snd::SoundManager::GetInstance()->GetFreeSoundSource();

					if( m_OldAtmosType == Level::eGravityType_Water &&
						m_OneShotSFX.nSourceID != snd::INVALID_SOUNDSOURCE )
					{
						if( m_SplashOutTime <= 0.0f )
							AudioSystem::GetInstance()->PlayAudio( m_OneShotSFX.nSourceID, m_AtmosChangeWater2AirId, math::Vec3(0.0f,0.0f,0.0f), AL_TRUE, AL_FALSE );
					}
					
					if( m_SplashOutTime <= 0.0f )
					{
						m_pWaterExit->Enable();
						m_pWaterExit->Disable();
						m_SplashOutTime = 2.0f;
					}
					//m_ShowSplash = true;
					//m_SplashTime = SPLASH_TIMER;
				}

				if( !m_IsDead ) // stop light changes on the vehicle when dead
					m_InWater = false;

				m_MaxFallVel				= m_VehicleData.maxFallVelAir;
				m_MaxXVel					= m_VehicleData.maxXVelAir;
				m_MaxYVel					= m_VehicleData.maxYVelAir;

				m_UpwardsForce				= m_VehicleData.upwardsForceAir;
				m_MoveForce					= m_VehicleData.movementForceAir;

				PhysicsWorld::GetWorld()->SetGravity( m_AirSettings.gravity );

				m_OldAtmosType = m_AtmosType;

			}break;
			case Level::eGravityType_Water:
			{
				if( m_VehicleData.hasSubMeshChanges )
				{
					// astro disables
					for( i=0; i < m_VehicleData.astroSubMeshCount; ++i )
					{
						if( m_VehicleData.astroSubMeshList[i] != -1 )
							SetMeshDrawState( m_VehicleData.astroSubMeshList[i], false );
					}

					// aqua disables
					for( i=0; i < m_VehicleData.aquaSubMeshCount; ++i )
					{
						if( m_VehicleData.aquaSubMeshList[i] != -1 )
							SetMeshDrawState( m_VehicleData.aquaSubMeshList[i], true );
					}

					// aero enables
					for( i=0; i < m_VehicleData.aeroSubMeshCount; ++i )
					{
						if( m_VehicleData.aeroSubMeshList[i] != -1 )
							SetMeshDrawState( m_VehicleData.aeroSubMeshList[i], false );
					}
				}

				m_OneShotSFX.nSourceID = snd::SoundManager::GetInstance()->GetFreeSoundSource();

				if( m_OldAtmosType == Level::eGravityType_Air &&
					m_OneShotSFX.nSourceID != snd::INVALID_SOUNDSOURCE )
				{
					if( m_SplashInTime <= 0.0f )
					{
						AudioSystem::GetInstance()->PlayAudio( m_OneShotSFX.nSourceID, m_AtmosChangeAir2WaterId, math::Vec3(0.0f,0.0f,0.0f), AL_TRUE, AL_FALSE );

						m_pWaterEnter->Enable();
						m_pWaterEnter->Disable();

						m_SplashInTime = 2.0f;
					}
				}

				if( !m_IsDead ) // stop light changes on the vehicle when dead
					m_InWater = true;

				m_MaxFallVel				= m_VehicleData.maxFallVelWater;
				m_MaxXVel					= m_VehicleData.maxXVelWater;
				m_MaxYVel					= m_VehicleData.maxYVelWater;

				m_UpwardsForce				= m_VehicleData.upwardsForceWater;
				m_MoveForce					= m_VehicleData.movementForceWater;

				PhysicsWorld::GetWorld()->SetGravity( m_WaterSettings.gravity );

				//m_ShowSplash = true;
				//m_SplashTime = SPLASH_TIMER;

				m_OldAtmosType = m_AtmosType;

			}break;
				
			default:
				break;				
		}
	}
}

/////////////////////////////////////////////////////
/// Method: StopAllAudio
/// Params: None
///
/////////////////////////////////////////////////////
void Player::StopAllAudio()
{
	// stop all values
	if( m_PropulsionSFX.nSourceID != snd::INVALID_SOUNDSOURCE )
	{
		AudioSystem::GetInstance()->StopAudio( m_PropulsionSFX.nSourceID );
		m_PropulsionSFX.nSourceID = snd::INVALID_SOUNDSOURCE;
	}
	if( m_BrakeSFX.nSourceID != snd::INVALID_SOUNDSOURCE )
	{
		AudioSystem::GetInstance()->StopAudio( m_BrakeSFX.nSourceID );
		m_BrakeSFX.nSourceID = snd::INVALID_SOUNDSOURCE;
	}
	if( m_OneShotSFX.nSourceID != snd::INVALID_SOUNDSOURCE )
	{
		AudioSystem::GetInstance()->StopAudio( m_OneShotSFX.nSourceID );
		m_OneShotSFX.nSourceID = snd::INVALID_SOUNDSOURCE;
	}
	if( m_LoopedSFX.nSourceID != snd::INVALID_SOUNDSOURCE )
	{
		AudioSystem::GetInstance()->StopAudio( m_LoopedSFX.nSourceID );
		m_LoopedSFX.nSourceID = snd::INVALID_SOUNDSOURCE;
	}
}

/////////////////////////////////////////////////////
/// Method: HandleContact
/// Params: [in]idNum, [in]pOtherBody
///
/////////////////////////////////////////////////////
void Player::HandleContact( int idNum, int castId, const b2ContactPoint* point, b2Shape* pOtherShape, b2Body* pOtherBody )
{
	if( m_IsDead )
	{
		if( m_AllowCollisionSound )
		{
			//m_OneShotSFX.nSourceID = snd::SoundManager::GetInstance()->GetFreeSoundSource();

			//if( m_OneShotSFX.nSourceID != snd::INVALID_SOUNDSOURCE )
			//	AudioSystem::GetInstance()->PlayAudio( m_OneShotSFX.nSourceID, m_DestroyedId, math::Vec3(0.0f,0.0f,0.0f), AL_TRUE, AL_FALSE );
		}
		return;
	}

	float damageToApply = 0.0f;

	switch( idNum )
	{
		case PHYSICSBASICID_PLAYER:
		{
			// shouldn't happen
			DBG_ASSERT(0);
		}break;
		case PHYSICSBASICID_WORLD:
		{
			// taxi has hit the world
			//DBGLOG( "Hit World\n" );
			PhysicsIdentifier* pUserData = 0;

			if( pOtherBody != 0 )
				pUserData = reinterpret_cast<PhysicsIdentifier*>( pOtherShape->GetUserData() );

			DBG_ASSERT( pUserData != 0 );

			// find out if it's the world edge
			if( pUserData->GetNumeric1() == NUMERIC_WORLDEDGE )
			{
				m_WorldEdgeContact = true;
			}

			if( !m_WorldEdgeContact &&
				!m_IsContactWorld )
			{
				m_IsContactWorld = true;

				if( !m_LandingGear )
				{
					// see how fast it was
					float len = std::abs( point->velocity.LengthSquared() );

					const float FORCE = 3.0f;
					b2Vec2 force( point->normal.x*FORCE, point->normal.y*FORCE );

					// really fast kill the player?
					if( len >= m_GameData.WORLD_DAMAGE_SPEED_MASSIVE ) 
					{
						if( m_DevData.singleHitKill )
							m_Damage = 100.0f;
						else
							m_Damage += 30.0f;

						DBGLOG( "Massive Hit\n" );

						m_pPlayerBody->SetLinearVelocity( b2Vec2( 0.0f, 0.0f ) );

						b2Vec2 p = m_pPlayerBody->GetWorldCenter();
						m_pPlayerBody->ApplyImpulse( force, p );	

						m_OneShotSFX.nSourceID = snd::SoundManager::GetInstance()->GetFreeSoundSource();

						if( m_OneShotSFX.nSourceID != snd::INVALID_SOUNDSOURCE )
							AudioSystem::GetInstance()->PlayAudio( m_OneShotSFX.nSourceID, m_CollisionMassiveId, math::Vec3(0.0f,0.0f,0.0f), AL_TRUE, AL_FALSE );
					}
					else
                    if( len >= m_GameData.WORLD_DAMAGE_SPEED_LARGE && len < m_GameData.WORLD_DAMAGE_SPEED_MASSIVE ) // large impact
					{
						if( m_DevData.singleHitKill )
							m_Damage = 100.0f;
						else
                        {
                            if( !core::app::IstvOS() )
                                m_Damage += m_VehicleData.largeDamage;
                            else
                                m_Damage += m_VehicleData.smallDamage;
                        }
                        
						DBGLOG( "Large Hit\n" );

						m_pPlayerBody->SetLinearVelocity( b2Vec2( 0.0f, 0.0f ) );

						b2Vec2 p = m_pPlayerBody->GetWorldCenter();
						m_pPlayerBody->ApplyImpulse( force, p );		

						m_OneShotSFX.nSourceID = snd::SoundManager::GetInstance()->GetFreeSoundSource();

						if( m_OneShotSFX.nSourceID != snd::INVALID_SOUNDSOURCE )
							AudioSystem::GetInstance()->PlayAudio( m_OneShotSFX.nSourceID, m_CollisionLargeId, math::Vec3(0.0f,0.0f,0.0f), AL_TRUE, AL_FALSE );
					}
					else  // small
					{
						// not flat ground, push player away
						if( point->normal.y < 0.99f )
						{
							if( m_DevData.singleHitKill )
								m_Damage = 100.0f;
							else
								m_Damage += m_VehicleData.smallDamage;

							DBGLOG( "Small Hit\n" );

							m_pPlayerBody->SetLinearVelocity( b2Vec2( 0.0f, 0.0f ) );

							b2Vec2 p = m_pPlayerBody->GetWorldCenter();
							m_pPlayerBody->ApplyImpulse( force, p );

							m_OneShotSFX.nSourceID = snd::SoundManager::GetInstance()->GetFreeSoundSource();

							if( m_OneShotSFX.nSourceID != snd::INVALID_SOUNDSOURCE )
								AudioSystem::GetInstance()->PlayAudio( m_OneShotSFX.nSourceID, m_CollisionSmallId, math::Vec3(0.0f,0.0f,0.0f), AL_TRUE, AL_FALSE );

						}
					}
				}
				else
				{
					// landing gear
					const float FORCE = 3.0f;
					b2Vec2 force( point->normal.x*FORCE, point->normal.y*FORCE );

					// not flat ground, push player away
					if( point->normal.y < 0.99f )
					{
						if( m_DevData.singleHitKill )
							m_Damage = 100.0f;
						else
							m_Damage += m_VehicleData.smallDamage;

						DBGLOG( "Small Hit\n" );

						m_pPlayerBody->SetLinearVelocity( b2Vec2( 0.0f, 0.0f ) );

						b2Vec2 p = m_pPlayerBody->GetWorldCenter();
						m_pPlayerBody->ApplyImpulse( force, p );

						m_OneShotSFX.nSourceID = snd::SoundManager::GetInstance()->GetFreeSoundSource();

						if( m_OneShotSFX.nSourceID != snd::INVALID_SOUNDSOURCE )
							AudioSystem::GetInstance()->PlayAudio( m_OneShotSFX.nSourceID, m_CollisionSmallId, math::Vec3(0.0f,0.0f,0.0f), AL_TRUE, AL_FALSE );

					}
				}
			}

			if( !m_IsDead )
			{
				b2Vec2 force( 1.0f*m_GameData.TILT_PUSH_FORCE, 2.0f*m_GameData.TILT_PUSH_FORCE );

				b2Vec2 p = m_pPlayerBody->GetWorldCenter();
				float a = math::RadToDeg( m_pPlayerBody->GetAngle() );

				if( a >= m_GameData.MAX_TILT_LAND_ANGLE || 
					a <= -m_GameData.MAX_TILT_LAND_ANGLE )
				{
					if( a > 0.0f )
						force.x = -force.x;

					m_pPlayerBody->ApplyImpulse( force, p );	
				}
			}

			// in contact with flat world and not dead, and out of fuel
			if( m_IsContactWorld &&
				!m_WorldEdgeContact &&
				!m_IsDead &&
				m_IsOnGroundIdle &&
				m_OutOfFuel &&
				!m_IsRefueling )
			{
				// check velocity

				// start timing it
				m_SurvivedFuelRunout = true;
			}

		}break;
		case PHYSICSBASICID_ENEMY:
		{
			// hit an enemy	
			switch( castId )
			{
				case PHYSICSCASTID_ENEMYBOMB:
				case PHYSICSCASTID_ENEMYBOUNCINGBOMB:
				case PHYSICSCASTID_ENEMYDUSTDEVIL:
				case PHYSICSCASTID_ENEMYFISH:
				{
					damageToApply += m_VehicleData.smallDamage;
				}break;
				case PHYSICSCASTID_ENEMYMINE:
				{
					Mine* pData = 0;
					pData = reinterpret_cast<Mine*>( pOtherBody->GetUserData() );
					DBG_ASSERT( pData != 0 );

					pData->HandleContact( point );

					damageToApply = 100.0f;

				}break;
				case PHYSICSCASTID_ENEMYPIRANHA:
				case PHYSICSCASTID_ENEMYPUFFERFISH:
				case PHYSICSCASTID_ENEMYRAT:
				case PHYSICSCASTID_ENEMYSENTRY:
				{
					damageToApply += m_VehicleData.smallDamage;
				}break;
				default:
					// dodgy enemy id
					DBG_ASSERT(0);
				break;
			}

			if( !m_IsContactEnemy )
			{
				DBGLOG( "Hit Enemy\n" );

				if( m_DevData.singleHitKill )
					m_Damage = 100.0f;
				else
					m_Damage += damageToApply;

				m_OneShotSFX.nSourceID = snd::SoundManager::GetInstance()->GetFreeSoundSource();

				if( m_OneShotSFX.nSourceID != snd::INVALID_SOUNDSOURCE )
					AudioSystem::GetInstance()->PlayAudio( m_OneShotSFX.nSourceID, m_CollisionSmallId, math::Vec3(0.0f,0.0f,0.0f), AL_TRUE, AL_FALSE );

				m_IsContactEnemy = true;
			}
		}break;
		case PHYSICSBASICID_PORT:
		{
			m_IsContactPort = true;
			if( !m_IsContactWorld )
			{
				m_IsContactWorld = true;
				// hit a port
				if( !m_LandingGear )
				{
					// see how fast it was
					float len = std::abs( point->velocity.LengthSquared() );

					const float FORCE = 3.0f;
					b2Vec2 force( point->normal.x*FORCE, point->normal.y*FORCE );

					// really fast kill the player?
					if( len >= m_GameData.PORT_DAMAGE_SPEED_MASSIVE ) 
					{
						m_pPlayerBody->SetLinearVelocity( b2Vec2( 0.0f, 0.0f ) );

						b2Vec2 p = m_pPlayerBody->GetWorldCenter();
						m_pPlayerBody->ApplyImpulse( force, p );	

						DBGLOG( "Hit Port - Massive Hit\n" );
							
						if( m_DevData.singleHitKill )
							m_Damage = 100.0f;
						else
							m_Damage += 30.0f;

						m_OneShotSFX.nSourceID = snd::SoundManager::GetInstance()->GetFreeSoundSource();

						if( m_OneShotSFX.nSourceID != snd::INVALID_SOUNDSOURCE )
							AudioSystem::GetInstance()->PlayAudio( m_OneShotSFX.nSourceID, m_CollisionMassiveId, math::Vec3(0.0f,0.0f,0.0f), AL_TRUE, AL_FALSE );
					}
					else
					if( len >= m_GameData.PORT_DAMAGE_SPEED_LARGE && len < m_GameData.PORT_DAMAGE_SPEED_MASSIVE ) // large impact
					{
						m_pPlayerBody->SetLinearVelocity( b2Vec2( 0.0f, 0.0f ) );

						b2Vec2 p = m_pPlayerBody->GetWorldCenter();
						m_pPlayerBody->ApplyImpulse( force, p );	

						DBGLOG( "Hit Port - Large Hit\n" );
							
						//if( devData.singleHitKill )
						//	m_Damage = 100.0f;
						//else
						//	m_Damage += m_VehicleData.largeDamage;

						m_OneShotSFX.nSourceID = snd::SoundManager::GetInstance()->GetFreeSoundSource();

						if( m_OneShotSFX.nSourceID != snd::INVALID_SOUNDSOURCE )
							AudioSystem::GetInstance()->PlayAudio( m_OneShotSFX.nSourceID, m_CollisionSmallId, math::Vec3(0.0f,0.0f,0.0f), AL_TRUE, AL_FALSE );
					}
				}
			}
			else
			{
				// no landing gear
			}

			if( !m_IsDead )
			{
				b2Vec2 force( 1.0f*m_GameData.TILT_PUSH_FORCE, 2.0f*m_GameData.TILT_PUSH_FORCE );

				b2Vec2 p = m_pPlayerBody->GetWorldCenter();
				float a = math::RadToDeg( m_pPlayerBody->GetAngle() );

				if( a >= m_GameData.MAX_TILT_LAND_ANGLE || 
					a <= -m_GameData.MAX_TILT_LAND_ANGLE )
				{
					if( a > 0.0f )
						force.x = -force.x;

					m_pPlayerBody->ApplyImpulse( force, p );	
				}
			}

			// in contact with a port and not dead, and out of fuel
			if( m_IsContactPort &&
				!m_WorldEdgeContact &&
				!m_IsDead &&
				m_IsOnGroundIdle &&
				m_OutOfFuel &&
				!m_IsRefueling)
			{
				// check velocity

				// start timing it
				m_SurvivedFuelRunout = true;
			}
		}break;
		case PHYSICSBASICID_SHAPE:
		{
			// hit a shape
			const float FORCE = 3.0f;
			b2Vec2 force( point->normal.x*FORCE, point->normal.y*FORCE );

			m_pPlayerBody->SetLinearVelocity( b2Vec2( 0.0f, 0.0f ) );

			b2Vec2 p = m_pPlayerBody->GetWorldCenter();
			m_pPlayerBody->ApplyImpulse( force, p );

			if( !m_IsContactShape )
			{
				DBGLOG( "Hit Shape\n" );

				if( m_DevData.singleHitKill )
					m_Damage = 100.0f;
				else
					m_Damage += m_VehicleData.smallDamage;

				m_OneShotSFX.nSourceID = snd::SoundManager::GetInstance()->GetFreeSoundSource();

				if( m_OneShotSFX.nSourceID != snd::INVALID_SOUNDSOURCE )
					AudioSystem::GetInstance()->PlayAudio( m_OneShotSFX.nSourceID, m_CollisionSmallId, math::Vec3(0.0f,0.0f,0.0f), AL_TRUE, AL_FALSE );

				m_IsContactShape = true;
			}
		}break;
		case PHYSICSBASICID_DOOR:
		{
			// hit a door
			const float FORCE = 3.0f;
			b2Vec2 force( point->normal.x*FORCE, point->normal.y*FORCE );

			m_pPlayerBody->SetLinearVelocity( b2Vec2( 0.0f, 0.0f ) );

			b2Vec2 p = m_pPlayerBody->GetWorldCenter();
			m_pPlayerBody->ApplyImpulse( force, p );	

			if( !m_IsContactDoor )
			{
				DBGLOG( "Hit Door\n" );

				if( m_DevData.singleHitKill )
					m_Damage = 100.0f;
				else
					m_Damage += m_VehicleData.smallDamage;

				m_OneShotSFX.nSourceID = snd::SoundManager::GetInstance()->GetFreeSoundSource();

				if( m_OneShotSFX.nSourceID != snd::INVALID_SOUNDSOURCE )
					AudioSystem::GetInstance()->PlayAudio( m_OneShotSFX.nSourceID, m_CollisionSmallId, math::Vec3(0.0f,0.0f,0.0f), AL_TRUE, AL_FALSE );
			
				m_IsContactDoor = true;
			}
		}break;

		default:
			// colliding with something that doesn't have an id
			DBG_ASSERT(0);
		break;
	}
}

/////////////////////////////////////////////////////
/// Method: HandlePersistantContact
/// Params: None
///
/////////////////////////////////////////////////////
void Player::HandlePersistantContact( int idNum, int castId, const b2ContactPoint* point, b2Shape* pOtherShape, b2Body* pOtherBody )
{
	if( m_IsDead )
	{
		// probably jammed don't allow sound effects to play
		if( idNum == PHYSICSBASICID_DOOR )
		{
			m_AllowCollisionSound = false;
		}
		return;
	}

	m_AllowCollisionSound = true;
	switch( idNum )
	{
		case PHYSICSBASICID_PLAYER:
		{
			// shouldn't happen
			DBG_ASSERT(0);
		}break;
		case PHYSICSBASICID_WORLD:
		{
			PhysicsIdentifier* pUserData = 0;

			if( pOtherBody != 0 )
				pUserData = reinterpret_cast<PhysicsIdentifier*>( pOtherShape->GetUserData() );

			DBG_ASSERT( pUserData != 0 );

			// find out if it's the world edge
			if( pUserData->GetNumeric1() == NUMERIC_WORLDEDGE )
			{
				m_WorldEdgeContact = true;
			}

			// taxi has hit the world
			m_IsContactWorld = true;

			if( !m_IsDead )
			{
				b2Vec2 force( 1.0f*m_GameData.TILT_PUSH_FORCE, 2.0f*m_GameData.TILT_PUSH_FORCE );

				b2Vec2 p = m_pPlayerBody->GetWorldCenter();
				float a = math::RadToDeg( m_pPlayerBody->GetAngle() );

				if( a >= m_GameData.MAX_TILT_LAND_ANGLE || 
					a <= -m_GameData.MAX_TILT_LAND_ANGLE )
				{
					if( a > 0.0f )
						force.x = -force.x;

					m_pPlayerBody->ApplyImpulse( force, p );	
				}
			}

			if( m_IsContactWorld &&
				!m_WorldEdgeContact &&
				!m_IsDead &&
				m_IsOnGroundIdle &&
				m_OutOfFuel &&
				!m_IsRefueling )
			{
				// check velocity

				// start timing it
				m_SurvivedFuelRunout = true;
			}
		}break;
		case PHYSICSBASICID_ENEMY:
		{
			// hit an enemy	
			switch( castId )
			{
				case PHYSICSCASTID_ENEMYBOMB:
				case PHYSICSCASTID_ENEMYBOUNCINGBOMB:
				case PHYSICSCASTID_ENEMYDUSTDEVIL:
				case PHYSICSCASTID_ENEMYFISH:
				case PHYSICSCASTID_ENEMYMINE:
				case PHYSICSCASTID_ENEMYPIRANHA:
				case PHYSICSCASTID_ENEMYPUFFERFISH:
				case PHYSICSCASTID_ENEMYRAT:
				case PHYSICSCASTID_ENEMYSENTRY:
				break;
				default:
					// dodgy enemy id
					DBG_ASSERT(0);
				break;
			}

		}break;
		case PHYSICSBASICID_PORT:
		{
			// should be normal
			m_IsContactWorld = true;
			m_IsContactPort = true;

			if( !m_IsDead )
			{
				b2Vec2 force( 1.0f*m_GameData.TILT_PUSH_FORCE, 2.0f*m_GameData.TILT_PUSH_FORCE );

				b2Vec2 p = m_pPlayerBody->GetWorldCenter();
				float a = math::RadToDeg( m_pPlayerBody->GetAngle() );

				if( a >= m_GameData.MAX_TILT_LAND_ANGLE || 
					a <= -m_GameData.MAX_TILT_LAND_ANGLE )
				{
					if( a > 0.0f )
						force.x = -force.x;

					m_pPlayerBody->ApplyImpulse( force, p );	
				}
			}

			if( m_IsContactPort &&
				!m_WorldEdgeContact &&
				!m_IsDead &&
				m_IsOnGroundIdle &&
				m_OutOfFuel &&
				!m_IsRefueling )
			{
				// check velocity

				// start timing it
				m_SurvivedFuelRunout = true;
			}
		}break;
		case PHYSICSBASICID_SHAPE:
		{
			// hit a shape
			DBGLOG( "Persist Shape\n" );

			const float FORCE = 3.0f;
			b2Vec2 force( point->normal.x*FORCE, point->normal.y*FORCE );

			m_pPlayerBody->SetLinearVelocity( b2Vec2( 0.0f, 0.0f ) );

			b2Vec2 p = m_pPlayerBody->GetWorldCenter();
			m_pPlayerBody->ApplyImpulse( force, p );	
			//m_Damage += m_VehicleData.smallDamage;
		}break;
		case PHYSICSBASICID_DOOR:
		{
			// hit a door
			DBGLOG( "Persist Door\n" );
			const float FORCE = 3.0f;
			b2Vec2 force( point->normal.x*FORCE, point->normal.y*FORCE );

			m_pPlayerBody->SetLinearVelocity( b2Vec2( 0.0f, 0.0f ) );

			b2Vec2 p = m_pPlayerBody->GetWorldCenter();
			m_pPlayerBody->ApplyImpulse( force, p );	
			//m_Damage += m_VehicleData.smallDamage;
		}break;

		default:
			// colliding with something that doesn't have an id
			DBG_ASSERT(0);
		break;
	}
}

/////////////////////////////////////////////////////
/// Method: ClearContact
/// Params: None
///
/////////////////////////////////////////////////////
void Player::ClearContact( int idNum )
{
	switch( idNum )
	{
		case PHYSICSBASICID_PLAYER:
		{
			// shouldn't happen
			DBG_ASSERT(0);
		}break;
		case PHYSICSBASICID_WORLD:
		{
			// taxi has hit the world
			m_IsContactWorld = false;
		}break;
		case PHYSICSBASICID_ENEMY:
		{
			// hit an enemy
			m_IsContactEnemy = false;

		}break;
		case PHYSICSBASICID_PORT:
		{
			// should be normal
			m_IsContactPort = false;
		}break;
		case PHYSICSBASICID_SHAPE:
		{
			m_IsContactShape = false;
		}break;
		case PHYSICSBASICID_DOOR:
		{
			m_IsContactDoor = false;
		}break;

		default:
			// colliding with something that doesn't have an id
			DBG_ASSERT(0);
		break;
	}
}

/////////////////////////////////////////////////////
/// Method: ProcessSounds
/// Params: None
///
/////////////////////////////////////////////////////
void Player::ProcessSounds()
{
	m_PropulsionSFX.vPos = m_Pos;

	if( !m_IsDead && !m_OutOfFuel )
	{
		if( m_InputSystem.GetThrottle() || 
			m_InputSystem.GetAirBrake() ||
			( (m_InputSystem.GetLeft() || m_InputSystem.GetRight()) && !m_LandingGear ) ) 
		{
			if( m_PropulsionSFX.nSourceID == snd::INVALID_SOUNDSOURCE )
			{
				m_PropulsionSFX.nSourceID = snd::SoundManager::GetInstance()->GetFreeSoundSource();
				ALuint bufferId = snd::INVALID_SOUNDBUFFER;

				float gain = 1.0f;
				if( m_AtmosType == Level::eGravityType_Vacuum )
				{
					bufferId = m_VacBufferId;
					gain = 0.75f;
				}
				else if( m_AtmosType == Level::eGravityType_Air )
				{
					bufferId = m_AirBufferId;
					gain = 0.25f;
				}
				else if( m_AtmosType == Level::eGravityType_Water )
				{
					bufferId = m_SubBufferId;
					gain = 0.75f;
				}

				AudioSystem::GetInstance()->PlayAudio( m_PropulsionSFX.nSourceID, bufferId, math::Vec3(0.0f,0.0f,0.0f), AL_TRUE, AL_TRUE, 1.0f, gain );
			}
			
			if( m_AtmosType == Level::eGravityType_Vacuum && m_PropulsionSFX.nBufferID != m_VacBufferId )
			{
				AudioSystem::GetInstance()->StopAudio( m_PropulsionSFX.nSourceID );

				m_PropulsionSFX.nBufferID = m_VacBufferId;

				AudioSystem::GetInstance()->PlayAudio( m_PropulsionSFX.nSourceID, m_VacBufferId, math::Vec3(0.0f,0.0f,0.0f), AL_TRUE, AL_TRUE, 1.0f, 0.75f );
			}
			else if( m_AtmosType == Level::eGravityType_Air && m_PropulsionSFX.nBufferID != m_AirBufferId )
			{
				AudioSystem::GetInstance()->StopAudio( m_PropulsionSFX.nSourceID );

				m_PropulsionSFX.nBufferID = m_AirBufferId;

				AudioSystem::GetInstance()->PlayAudio( m_PropulsionSFX.nSourceID, m_AirBufferId, math::Vec3(0.0f,0.0f,0.0f), AL_TRUE, AL_TRUE, 1.0f, 0.25f );
			}
			else if( m_AtmosType == Level::eGravityType_Water && m_PropulsionSFX.nBufferID != m_SubBufferId )
			{
				AudioSystem::GetInstance()->StopAudio( m_PropulsionSFX.nSourceID );

				m_PropulsionSFX.nBufferID = m_SubBufferId;

				AudioSystem::GetInstance()->PlayAudio( m_PropulsionSFX.nSourceID, m_SubBufferId, math::Vec3(0.0f,0.0f,0.0f), AL_TRUE, AL_TRUE, 1.0f, 0.75f );
			}
		}
		else
		{
			// not playing back
			if( m_PropulsionSFX.nSourceID != snd::INVALID_SOUNDSOURCE )
			{
				AudioSystem::GetInstance()->StopAudio( m_PropulsionSFX.nSourceID );
				m_PropulsionSFX.nSourceID = snd::INVALID_SOUNDSOURCE;
			}
		}

		if( m_IsRefueling )
		{
			if( m_FuelCount < m_GameData.MAX_FUEL )
			{
				m_LoopedSFX.vPos = m_Pos;

				if( m_LoopedSFX.nSourceID == snd::INVALID_SOUNDSOURCE )
				{
					m_LoopedSFX.nSourceID = snd::SoundManager::GetInstance()->GetFreeSoundSource();

					AudioSystem::GetInstance()->StopAudio( m_LoopedSFX.nSourceID );

					m_LoopedSFX.nBufferID = m_FuelFillId;

					AudioSystem::GetInstance()->PlayAudio( m_LoopedSFX.nSourceID, m_FuelFillId, math::Vec3(0.0f,0.0f,0.0f), AL_TRUE, AL_TRUE );
				}
				else
				{
					if( m_LoopedSFX.nBufferID != m_FuelFillId )
					{
						AudioSystem::GetInstance()->StopAudio( m_LoopedSFX.nSourceID );

						m_LoopedSFX.nSourceID = snd::INVALID_SOUNDSOURCE;
					}
				}
			}
			else
			{
				if( m_LoopedSFX.nSourceID != snd::INVALID_SOUNDSOURCE )
				{
					AudioSystem::GetInstance()->StopAudio( m_LoopedSFX.nSourceID );

					m_LoopedSFX.nSourceID = snd::INVALID_SOUNDSOURCE;
				}
			}
		}
		else if( m_IsRefusedFuel )
		{
			m_LoopedSFX.vPos = m_Pos;

			if( m_LoopedSFX.nSourceID == snd::INVALID_SOUNDSOURCE )
			{
				m_LoopedSFX.nSourceID = snd::SoundManager::GetInstance()->GetFreeSoundSource();

				AudioSystem::GetInstance()->StopAudio( m_LoopedSFX.nSourceID );

				m_LoopedSFX.nBufferID = m_FuelRefuseId;

				AudioSystem::GetInstance()->PlayAudio( m_LoopedSFX.nSourceID, m_FuelRefuseId, math::Vec3(0.0f,0.0f,0.0f), AL_TRUE, AL_TRUE );
			}
			else
			{
				if( m_LoopedSFX.nBufferID != m_FuelRefuseId )
				{
					AudioSystem::GetInstance()->StopAudio( m_LoopedSFX.nSourceID );

					m_LoopedSFX.nSourceID = snd::INVALID_SOUNDSOURCE;
				}
			}
		}
		else
		{
			// fuel low audio
			if( m_FuelCount <= m_GameData.LOW_FUEL )
			{
				m_LoopedSFX.vPos = m_Pos;

				if( m_LoopedSFX.nSourceID == snd::INVALID_SOUNDSOURCE )
				{
					m_LoopedSFX.nSourceID = snd::SoundManager::GetInstance()->GetFreeSoundSource();

					AudioSystem::GetInstance()->StopAudio( m_LoopedSFX.nSourceID );

					m_LoopedSFX.nBufferID = m_FuelLowId;

					AudioSystem::GetInstance()->PlayAudio( m_LoopedSFX.nSourceID, m_FuelLowId, math::Vec3(0.0f,0.0f,0.0f), AL_TRUE, AL_TRUE );
				}
				else
				{
					if( m_LoopedSFX.nBufferID != m_FuelLowId )
					{
						AudioSystem::GetInstance()->StopAudio( m_LoopedSFX.nSourceID );

						m_LoopedSFX.nSourceID = snd::INVALID_SOUNDSOURCE;
					}
				}
			}
			else
			{
				if( m_LoopedSFX.nSourceID != snd::INVALID_SOUNDSOURCE )
				{
					AudioSystem::GetInstance()->StopAudio( m_LoopedSFX.nSourceID );

					m_LoopedSFX.nSourceID = snd::INVALID_SOUNDSOURCE;
				}
			}
		}
	}
	else
	{
		// player is dead stop all values
		if( m_PropulsionSFX.nSourceID != snd::INVALID_SOUNDSOURCE )
		{
			AudioSystem::GetInstance()->StopAudio( m_PropulsionSFX.nSourceID );
			m_PropulsionSFX.nSourceID = snd::INVALID_SOUNDSOURCE;
		}
		if( m_BrakeSFX.nSourceID != snd::INVALID_SOUNDSOURCE )
		{
			AudioSystem::GetInstance()->StopAudio( m_BrakeSFX.nSourceID );
			m_BrakeSFX.nSourceID = snd::INVALID_SOUNDSOURCE;
		}
		//if( m_OneShotSFX.nSourceID != snd::INVALID_SOUNDSOURCE )
		//{
		//	AudioSystem::GetInstance()->StopAudio( m_OneShotSFX.nSourceID );
		//	m_OneShotSFX.nSourceID = snd::INVALID_SOUNDSOURCE;
		//}
		if( m_LoopedSFX.nSourceID != snd::INVALID_SOUNDSOURCE )
		{
			AudioSystem::GetInstance()->StopAudio( m_LoopedSFX.nSourceID );
			m_LoopedSFX.nSourceID = snd::INVALID_SOUNDSOURCE;
		}
	}
}

/////////////////////////////////////////////////////
/// Method: InitialiseEmitters
/// Params: None
///
/////////////////////////////////////////////////////
void Player::InitialiseEmitters()
{
	int i=0;

	if( m_VehicleData.burnersHorizontalEmitterId != 0 )
	{
		m_pEmitHoriz = 0;
		m_pEmitHoriz = new efx::Emitter;
		DBG_ASSERT( m_pEmitHoriz != 0 );

		m_pEmitHoriz->Create( *res::GetEmitterResource( m_VehicleData.burnersHorizontalEmitterId  )->block );
		res::SetupTexturesOnEmitter( m_pEmitHoriz );
		m_pEmitHoriz->Disable();

		m_pEmitHorizWater = 0;
		m_pEmitHorizWater = new efx::Emitter;
		DBG_ASSERT( m_pEmitHorizWater != 0 );

		m_pEmitHorizWater->Create( *res::GetEmitterResource( 4 )->block );
		res::SetupTexturesOnEmitter( m_pEmitHorizWater );
		m_pEmitHorizWater->Disable();
	}

	if( m_VehicleData.burnersVerticalEmitterId != 0 )
	{
		for( i=0; i < m_GameData.VERTICAL_THRUSTERS; ++i )
		{
			m_pEmitVert[i] = 0;
			m_pEmitVert[i] = new efx::Emitter;
			DBG_ASSERT( m_pEmitVert[i] != 0 );

			m_pEmitVert[i]->Create( *res::GetEmitterResource( m_VehicleData.burnersVerticalEmitterId  )->block );
			res::SetupTexturesOnEmitter( m_pEmitVert[i] );
			m_pEmitVert[i]->Disable();

			m_pEmitVertWater[i] = 0;
			m_pEmitVertWater[i] = new efx::Emitter;
			DBG_ASSERT( m_pEmitVertWater[i] != 0 );

			m_pEmitVertWater[i]->Create( *res::GetEmitterResource( 3 )->block );
			res::SetupTexturesOnEmitter( m_pEmitVertWater[i] );
			m_pEmitVertWater[i]->Disable();
		}
	}

	m_pLowDamage = 0;
	m_pLowDamage = new efx::Emitter;
	DBG_ASSERT(m_pLowDamage != 0 );

	m_pLowDamage->Create( *res::GetEmitterResource(21)->block );
	res::SetupTexturesOnEmitter( m_pLowDamage );
	m_pLowDamage->Disable();

	m_pHighDamage = 0;
	m_pHighDamage = new efx::Emitter;
	DBG_ASSERT(m_pHighDamage != 0 );

	m_pHighDamage->Create( *res::GetEmitterResource(23)->block );
	res::SetupTexturesOnEmitter( m_pHighDamage );
	m_pHighDamage->Disable();

	m_pExplodeSparks = 0;
	m_pExplodeSparks = new efx::Emitter;
	DBG_ASSERT(m_pExplodeSparks != 0 );

	m_pExplodeSparks->Create( *res::GetEmitterResource(22)->block );
	res::SetupTexturesOnEmitter( m_pExplodeSparks );
	m_pExplodeSparks->Disable();

	m_pExplodeSmoke = 0;
	m_pExplodeSmoke = new efx::Emitter;
	DBG_ASSERT(m_pExplodeSmoke != 0 );

	m_pExplodeSmoke->Create( *res::GetEmitterResource(20)->block );
	res::SetupTexturesOnEmitter( m_pExplodeSmoke );
	m_pExplodeSmoke->Disable();

	m_pWaterEnter = 0;
	m_pWaterEnter = new efx::Emitter;
	DBG_ASSERT(m_pWaterEnter != 0 );

	m_pWaterEnter->Create( *res::GetEmitterResource(30)->block );
	res::SetupTexturesOnEmitter( m_pWaterEnter );
	m_pWaterEnter->Disable();

	m_pWaterExit = 0;
	m_pWaterExit = new efx::Emitter;
	DBG_ASSERT(m_pWaterExit != 0 );

	m_pWaterExit->Create( *res::GetEmitterResource(31)->block );
	res::SetupTexturesOnEmitter( m_pWaterExit );
	m_pWaterExit->Disable();
}

/////////////////////////////////////////////////////
/// Method: DrawEmitters
/// Params: None
///
/////////////////////////////////////////////////////
void Player::DrawEmitters()
{
	int i=0;

	if( m_pEmitHoriz != 0 )
		m_pEmitHoriz->Draw();

	if( m_AtmosType == Level::eGravityType_Water )
	{	
		if( m_pEmitHorizWater != 0 )
			m_pEmitHorizWater->Draw();
	}

	for( i=0; i < m_GameData.VERTICAL_THRUSTERS; ++i )
	{
		if( m_pEmitVert[i] != 0 )
			m_pEmitVert[i]->Draw();
	
		if( m_AtmosType == Level::eGravityType_Water )
		{
			if( m_pEmitVertWater[i] != 0 )
				m_pEmitVertWater[i]->Draw();
		}
	}

	if( m_Damage > 40.0f && m_Damage < 80.0f )
	{
		if( m_pLowDamage != 0 )
			m_pLowDamage->Draw();
	}
	else if( m_Damage >= 80.0f )
	{
		if( m_pHighDamage != 0 )
			m_pHighDamage->Draw();
	}
	else
	{

	}

	if( m_pExplodeSparks != 0 )
		m_pExplodeSparks->Draw();
	if( m_pExplodeSmoke != 0 )
		m_pExplodeSmoke->Draw();

	if( m_pWaterEnter != 0 )
		m_pWaterEnter->Draw();

	if( m_pWaterExit != 0 )
		m_pWaterExit->Draw();
}

/////////////////////////////////////////////////////
/// Method: UpdateEmitters
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void Player::UpdateEmitters(float deltaTime)
{
	int i=0;
	math::Vec3 offset( 0.0f, 0.0f, 0.0f );
	float angleInRads = math::DegToRad(m_RotationAngle);

	if( m_pEmitHoriz != 0 )
	{
		offset = math::Vec2( m_Pos.X+m_VehicleData.brunersHorizontalOffset.X, m_Pos.Y+m_VehicleData.brunersHorizontalOffset.Y );

		math::Vec3 vVector = offset - m_Pos;

		offset.X = (float)(m_Pos.X + std::cos(angleInRads)*vVector.X + std::sin(angleInRads)*vVector.Z);
		offset.Z = (float)(m_Pos.Z - std::sin(angleInRads)*vVector.X + std::cos(angleInRads)*vVector.Z);

		m_pEmitHoriz->SetPos( offset );
		m_pEmitHoriz->Update( deltaTime );
		
		m_pEmitHorizWater->SetPos( offset );
		m_pEmitHorizWater->Update( deltaTime );
	}

	for( i=0; i < m_GameData.VERTICAL_THRUSTERS; ++i )
	{
		if( m_pEmitVert[i] != 0 )
		{
			// yellow cab
			if( i == 0 )
				offset = math::Vec2( m_Pos.X+m_VehicleData.burnersVerticalFrontOffset.X, m_Pos.Y+m_VehicleData.burnersVerticalFrontOffset.Y );
			else
				offset = math::Vec2( m_Pos.X+m_VehicleData.burnersVerticalBackOffset.X, m_Pos.Y+m_VehicleData.burnersVerticalBackOffset.Y );

			math::Vec3 vVector = offset - m_Pos;

			offset.X = (float)(m_Pos.X + std::cos(angleInRads)*vVector.X + std::sin(angleInRads)*vVector.Z);
			offset.Z = (float)(m_Pos.Z - std::sin(angleInRads)*vVector.X + std::cos(angleInRads)*vVector.Z);

			m_pEmitVert[i]->SetPos( offset );
			m_pEmitVert[i]->Update( deltaTime );

			m_pEmitVertWater[i]->SetPos( offset );
			m_pEmitVertWater[i]->Update( deltaTime );
		}
	}

	if( m_pLowDamage != 0 && m_pHighDamage != 0 )
	{
		if( m_Damage > 40.0f && m_Damage < 80.0f )
		{
			if( !m_pLowDamage->IsEnabled() )
				m_pLowDamage->Enable();
		}
		else if( m_Damage >= 80.0f )
		{
			if( m_pLowDamage->IsEnabled() )
				m_pLowDamage->Disable();

			if( !m_pHighDamage->IsEnabled() )
				m_pHighDamage->Enable();
		}
		else
		{
			if( m_pLowDamage->IsEnabled() )
				m_pLowDamage->Disable();

			if( m_pHighDamage->IsEnabled() )
				m_pHighDamage->Disable();
		}

		m_pLowDamage->SetPos( m_Pos );
		m_pHighDamage->SetPos( math::Vec3( m_Pos.X, m_Pos.Y, 1.0f ) );

		m_pLowDamage->Update(deltaTime);
		m_pHighDamage->Update(deltaTime);
	}

	if( m_pExplodeSparks != 0 )
		m_pExplodeSparks->Update(deltaTime);
	if( m_pExplodeSmoke != 0 )
		m_pExplodeSmoke->Update(deltaTime);

	if( m_pWaterEnter != 0 && m_pWaterExit != 0 )
	{
		m_pWaterEnter->SetPos( math::Vec3( m_Pos.X, m_Pos.Y, 1.0f ) );
		m_pWaterExit->SetPos( math::Vec3( m_Pos.X, m_Pos.Y, 1.0f ) );

		m_pWaterEnter->Update(deltaTime);
		m_pWaterExit->Update(deltaTime);
	}
}

/////////////////////////////////////////////////////
/// Method: AddPassenger
/// Params: [in]portNumber
///
/////////////////////////////////////////////////////
void Player::AddPassenger( int portNumber, Customer& customer )
{
	m_ShowPortNumber = true;
	m_PortNumberTime = m_GameData.PORT_NUMBER_TIMER;
	m_PortRequest = portNumber;

	m_HasPassenger = true;
	m_CustomerModelIndex = customer.GetBodyModelIndex();
	m_CustomerGender = customer.GetGender();
	m_CustomerTexture = customer.GetTexture();
	PlayCustomerRequestAudio( m_PortRequest, customer );
}

/////////////////////////////////////////////////////
/// Method: AddCargo
/// Params: [in]portNumber
///
/////////////////////////////////////////////////////
void Player::AddCargo( int portNumber, Customer& customer )
{
	m_CargoDestination = portNumber;
	m_HasCargo = true;
	m_CargoTextureId = customer.GetCargoTextureId();
	PlayCustomerRequestAudio( m_CargoDestination, customer );
}

/////////////////////////////////////////////////////
/// Method: SetAnimation
/// Params: [in]nAnimId, [in]nFrameOffset, [in]bForceChange
///
/////////////////////////////////////////////////////
void Player::SetCrashAnimation( unsigned int nAnimId, int nFrameOffset, bool bForceChange )
{
	if( m_pCrashModel != 0 )
	{
		unsigned int i=0;

		if( m_pCrashAnim )
		{
			// if it asks for the same animation and the animation is looping then there's no need to change
			if( ( m_pCrashAnim->nAnimId == nAnimId ) && (m_pCrashAnim->bLooping || bForceChange==false) )
			{
				return;
			}
		}


		// forcing a change, update the primary animation pointer
		if( bForceChange )
		{
			for( i=0; i < m_pCrashModel->GetBoneAnimationList().size(); ++i )
			{
				if( m_pCrashModel->GetBoneAnimationList()[i]->nAnimId == nAnimId  )
				{
					m_pCrashAnim = m_pCrashModel->GetBoneAnimationList()[i];
					if( nFrameOffset != -1 && nFrameOffset < m_pCrashAnim->nTotalFrames )
						m_pCrashAnim->nCurrentFrame = nFrameOffset;
					else
						m_pCrashAnim->nCurrentFrame = 0;
					//m_pCrashAnim->animTimer.Reset();
					//m_pCrashAnim->animTimer.Start();
					m_pCrashAnim->animTimer = 0.0f;
					return;
				}
			}
		}

		// got this far, the animation doesn't exist so just set the bind pose
		m_pCrashAnim = 0;
	}
}

/////////////////////////////////////////////////////
/// Method: UpdateCrashModel
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void Player::UpdateCrashModel( float deltaTime )
{
	unsigned int i = 0, j = 0;
	mdl::TBoneJointKeyframe curFrame;
	mdl::TBoneJointKeyframe prevFrame;	

	math::Vec3 vPrimaryTranslate( 0.0f, 0.0f, 0.0f );
	math::Quaternion qPrimaryRotation( 0.0f, 0.0f, 0.0f, 1.0f );

	if( m_pCrashAnim )
	{
		int nFrame = 0;
		int nStartFrame = 0;

		m_pCrashAnim->animTimer += (deltaTime*1000.0f);

		// find out current time from the animation timer
		m_pCrashAnim->dCurrentTime = m_pCrashAnim->animTimer; //m_pCrashAnim->animTimer.GetTimeInMilliseconds();

		// should the current time be offset from the start frame (anims usually start at frame 0 though)
		float fTimeOffset = (float)((( nStartFrame )*1000) / m_pCrashAnim->fAnimationFramerate );
		m_pCrashAnim->dCurrentTime += fTimeOffset; 

		if( (m_pCrashAnim->nTotalFrames > 0) && (m_pCrashAnim->nTotalFrames < m_pCrashAnim->nTotalFrames) )
			m_pCrashAnim->fEndTime = (float)( ( (m_pCrashAnim->nTotalFrames)*1000) / m_pCrashAnim->fAnimationFramerate );
		else 
			m_pCrashAnim->fEndTime = (float)m_pCrashAnim->dTotalAnimationTime;

		// get the animation status
		if( m_pCrashAnim->dCurrentTime > m_pCrashAnim->fEndTime )
		{
			// no animation waiting, either loop or stop
			//if( !m_bPaused )
			{
				if( m_pCrashAnim->bLooping )
				{
					m_pCrashAnim->nCurrentFrame = 0;
					//m_pCrashAnim->animTimer.Reset();
					m_pCrashAnim->animTimer = 0.0f;
					m_pCrashAnim->dCurrentTime = fTimeOffset;
				}
				else
				{
					m_pCrashAnim->dCurrentTime = m_pCrashAnim->fEndTime; 
				}
			}
		} 

		// go through the joints and transform/rotate
		for( i = 0; i < m_pCrashModel->GetNumBoneJoints(); i++ )
		{
			// allow enough time to pass before updating the frame
			nFrame = m_pCrashAnim->nCurrentFrame;
			//if( !m_bPaused )
			{
				while( (nFrame < m_pCrashAnim->nTotalFrames) && (m_pCrashAnim->pFrames[i][nFrame].fTime < m_pCrashAnim->dCurrentTime) )
				{
					nFrame++;
				}
			}
			m_pCrashAnim->nCurrentFrame = nFrame;

			if( nFrame == 0 )
			{
				// first frame
				vPrimaryTranslate = m_pCrashAnim->pFrames[i][0].vTranslate;
				qPrimaryRotation = m_pCrashAnim->pFrames[i][0].qRotation;
			}
			else if( nFrame == m_pCrashAnim->nTotalFrames )
			{
				// last frame
				vPrimaryTranslate = m_pCrashAnim->pFrames[i][nFrame-1].vTranslate;
				qPrimaryRotation = m_pCrashAnim->pFrames[i][nFrame-1].qRotation;
			}
			else
			{
				// between first and last frame 
				curFrame = m_pCrashAnim->pFrames[i][nFrame];
				prevFrame = m_pCrashAnim->pFrames[i][nFrame-1];

				// find a point between the previous and current frame to move to
				math::Vec3 vCurPos = m_pCrashAnim->pFrames[i][nFrame].vTranslate;
				math::Vec3 vPrevPos = m_pCrashAnim->pFrames[i][nFrame-1].vTranslate;

				vPrimaryTranslate = Lerp( vPrevPos, vCurPos, deltaTime );

				// use quaternions
				math::Quaternion qPrev( m_pCrashAnim->pFrames[i][nFrame-1].qRotation );
				math::Quaternion qCur( m_pCrashAnim->pFrames[i][nFrame].qRotation );

				qPrimaryRotation = Slerp( qPrev, qCur, deltaTime );
			}

			// just use the primary
			m_pCrashModel->GetBoneJointsPtr()[i].relative.Identity();
			m_pCrashModel->GetBoneJointsPtr()[i].absolute.Identity();
			m_pCrashModel->GetBoneJointsPtr()[i].final.Identity();

			// add bone controller rotation
			//if( pBoneControllers[i].nBoneIndex != -1 )
			//{
			//	qPrimaryRotation *= pBoneControllers[i].qRotation;
			//	qPrimaryRotation.normalise();
			//}

			m_pCrashModel->GetBoneJointsPtr()[i].relative = math::Matrix4x4( qPrimaryRotation );

			m_pCrashModel->GetBoneJointsPtr()[i].relative.SetTranslation( vPrimaryTranslate );


			// if bone has a parent, multiply the child by the parent
			if ( m_pCrashModel->GetBoneJointsPtr()[i].nParentJointIndex != -1 )
			{
				m_pCrashModel->GetBoneJointsPtr()[i].absolute.SetMatrix( m_pCrashModel->GetBoneJointsPtr()[m_pCrashModel->GetBoneJointsPtr()[i].nParentJointIndex].absolute.m  );
				m_pCrashModel->GetBoneJointsPtr()[i].absolute = m_pCrashModel->GetBoneJointsPtr()[i].relative * m_pCrashModel->GetBoneJointsPtr()[i].absolute;
			}
			else
				m_pCrashModel->GetBoneJointsPtr()[i].absolute.SetMatrix( m_pCrashModel->GetBoneJointsPtr()[i].relative.m  );

			// assign results to a final matrix to update the verts
			m_pCrashModel->GetBoneJointsPtr()[i].final.SetMatrix( m_pCrashModel->GetBoneJointsPtr()[i].absolute.m );
		}
	}
	else
	{
		// go through the joints and just set the bind pose
		for( i = 0; i < m_pCrashModel->GetNumBoneJoints(); i++ )
		{
			// if bone has a parent, multiply the child by the parent
			if ( m_pCrashModel->GetBoneJointsPtr()[i].nParentJointIndex != -1 )
			{
				m_pCrashModel->GetBoneJointsPtr()[i].absolute.SetMatrix( m_pCrashModel->GetBoneJointsPtr()[m_pCrashModel->GetBoneJointsPtr()[i].nParentJointIndex].absolute.m  );
				m_pCrashModel->GetBoneJointsPtr()[i].absolute = m_pCrashModel->GetBoneJointsPtr()[i].relative * m_pCrashModel->GetBoneJointsPtr()[i].absolute;
			}
			else
				m_pCrashModel->GetBoneJointsPtr()[i].absolute.SetMatrix( m_pCrashModel->GetBoneJointsPtr()[i].relative.m  );

			// assign results to a final matrix to update the verts
			m_pCrashModel->GetBoneJointsPtr()[i].final.SetMatrix( m_pCrashModel->GetBoneJointsPtr()[i].absolute.m );
		}
	}

	// go through each skin
	for ( i = 0; i < m_pCrashModel->GetNumSkinMeshes(); i++ )
	{
		// update the mesh if it's enabled
		if( m_pCrashModel->GetSkinMeshListPtr()[i].bDraw )
		{
			for ( j = 0; j < m_pCrashModel->GetSkinMeshListPtr()[i].nNumPoints; j++ )
			{
				int nJointIndex0 = m_pCrashModel->GetSkinMeshListPtr()[i].pVertexWeights[j][0].nBoneIndex;
				float fWeight0 = m_pCrashModel->GetSkinMeshListPtr()[i].pVertexWeights[j][0].fWeight;

				int nJointIndex1 = m_pCrashModel->GetSkinMeshListPtr()[i].pVertexWeights[j][1].nBoneIndex;
				float fWeight1 = m_pCrashModel->GetSkinMeshListPtr()[i].pVertexWeights[j][1].fWeight;

				int nJointIndex2 = m_pCrashModel->GetSkinMeshListPtr()[i].pVertexWeights[j][2].nBoneIndex;
				float fWeight2 = m_pCrashModel->GetSkinMeshListPtr()[i].pVertexWeights[j][2].fWeight;

				int nJointIndex3 = m_pCrashModel->GetSkinMeshListPtr()[i].pVertexWeights[j][3].nBoneIndex;
				float fWeight3 = m_pCrashModel->GetSkinMeshListPtr()[i].pVertexWeights[j][3].fWeight;

				// move each vertex by the joints final matrix
				// nJointIndex0 and nWeight0 should always be valid, as there is always at least one bone affecting the vertex
				m_pCrashModel->GetSkinMeshListPtr()[i].pVertices[j] = math::Vec3( 0.0f, 0.0f, 0.0f );
						
				math::Vec3 vInfluence = math::TransformByMatrix4x4( m_pCrashModel->GetSkinMeshListPtr()[i].pVertexWeights[j][0].vTransformedVertex, &m_pCrashModel->GetBoneJointsPtr()[ nJointIndex0 ].final.m[0] );
				vInfluence *= fWeight0;
				m_pCrashModel->GetSkinMeshListPtr()[i].pVertices[j] += vInfluence;

				// ADD POSSIBLE WEIGHT 2
				if( fWeight1 > 0.0f && nJointIndex1 != -1 )
				{
					// move vert
					vInfluence = math::TransformByMatrix4x4( m_pCrashModel->GetSkinMeshListPtr()[i].pVertexWeights[j][1].vTransformedVertex, &m_pCrashModel->GetBoneJointsPtr()[ nJointIndex1 ].final.m[0]  );
					vInfluence *= fWeight1;
					m_pCrashModel->GetSkinMeshListPtr()[i].pVertices[j] += vInfluence;
				}
				// ADD POSSIBLE WEIGHT 3
				if( fWeight2 > 0.0f && nJointIndex2 != -1 )
				{
					// move vert
					vInfluence = math::TransformByMatrix4x4( m_pCrashModel->GetSkinMeshListPtr()[i].pVertexWeights[j][2].vTransformedVertex, &m_pCrashModel->GetBoneJointsPtr()[ nJointIndex2 ].final.m[0]  );
					vInfluence *= fWeight2;
					m_pCrashModel->GetSkinMeshListPtr()[i].pVertices[j] += vInfluence;
				}
				// ADD POSSIBLE WEIGHT 4
				if( fWeight3 > 0.0f && nJointIndex3 != -1 )
				{
					// move vert
					vInfluence = math::TransformByMatrix4x4( m_pCrashModel->GetSkinMeshListPtr()[i].pVertexWeights[j][3].vTransformedVertex, &m_pCrashModel->GetBoneJointsPtr()[ nJointIndex3 ].final.m[0]  );
					vInfluence *= fWeight3;
					m_pCrashModel->GetSkinMeshListPtr()[i].pVertices[j] += vInfluence;
				}
			}
		}

		//pSkinMeshList[i].Update( fDelta );
	}
}
