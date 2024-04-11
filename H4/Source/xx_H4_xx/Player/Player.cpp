
/*===================================================================
	File: Player.cpp
	Game: H4

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "SoundBase.h"
#include "CollisionBase.h"
#include "ModelBase.h"
#include "InputBase.h"
#include "ScriptBase.h"

#include "Camera/Camera.h"
#include "Camera/DebugCamera.h"

#include "AppConsts.h"
#include "H4.h"

#include "Resources/SoundResources.h"
#include "Resources/TextureResources.h"

#include "Player/PlayerAnimationLookup.h"
#include "Player/Player.h"

namespace
{
	const int REFJOINT_IDX = 0;
	const int BACKJOINT_IDX = 2;
	const int GUNJOINT_IDX = 12;

	const float ROTATION_INCREMENT = 360.0f;
	const float ROTATION_TOLERANCE = 10.0f;

	const float MAX_VEL = 2.0f;
	const float LINVEL_MIN = 0.01f;

	const float BOUNDING_RADIUS = 0.85f;

	const float GUNEXTEND_OFFSETX = 0.45f;
	const float GUNEXTEND_RADIUS = 0.35f;

	math::Vec3 zeroVec( 0.0f, 0.0f, 0.0f );
	b2Vec2 zeroVec2(0.0f,0.0f);

	// used to be an enum, now uses lua
	int mMuzzleFlash_Shotgun = 0;
	int mMuzzleFlash_Pistol = 1;
	int mHand_Shotgun = 2;
	int mHand_Pistol = 3;
	int mHand_Empty = 4;
	int mBody = 5;

	const int PLAYER_TEX_SKIN_START = 0;
	const int PLAYER_TEX_SKIN_END = 17;

	const int PLAYER_TEX_SKIN_WEAPON = 18;
	
	const float ANALOGUE_RANGE = 255.0f;
}

/////////////////////////////////////////////////////
/// Function: ScriptSetPlayerModelIndex
/// Params: [in]pState
///
/////////////////////////////////////////////////////
int ScriptSetPlayerModelIndex( lua_State* pState );
int ScriptSetPlayerModelIndex( lua_State* pState )
{
	if( !H4::GetHiResMode() )
	{
		if( lua_istable( pState, 1 ) )
		{
			// grab the data
			mMuzzleFlash_Shotgun	= static_cast<int>(script::LuaGetNumberFromTableItem( "mMuzzleFlash_Shotgun", 1 )); 
			mMuzzleFlash_Pistol		= static_cast<int>(script::LuaGetNumberFromTableItem( "mMuzzleFlash_Pistol", 1 ));
			mHand_Shotgun			= static_cast<int>(script::LuaGetNumberFromTableItem( "mHand_Shotgun", 1 ));
			mHand_Pistol			= static_cast<int>(script::LuaGetNumberFromTableItem( "mHand_Pistol", 1 )); 
			mHand_Empty				= static_cast<int>(script::LuaGetNumberFromTableItem( "mHand_Empty", 1 )); 
			mBody					= static_cast<int>(script::LuaGetNumberFromTableItem( "mBody", 1 ));  
		}
	}
	return(0);
}

/////////////////////////////////////////////////////
/// Function: ScriptSetPlayerModelHiIndex
/// Params: [in]pState
///
/////////////////////////////////////////////////////
int ScriptSetPlayerModelHiIndex( lua_State* pState );
int ScriptSetPlayerModelHiIndex( lua_State* pState )
{
	if( H4::GetHiResMode() )
	{
		if( lua_istable( pState, 1 ) )
		{
			// grab the data
			mMuzzleFlash_Shotgun	= static_cast<int>(script::LuaGetNumberFromTableItem( "mMuzzleFlash_Shotgun", 1 )); 
			mMuzzleFlash_Pistol		= static_cast<int>(script::LuaGetNumberFromTableItem( "mMuzzleFlash_Pistol", 1 ));
			mHand_Shotgun			= static_cast<int>(script::LuaGetNumberFromTableItem( "mHand_Shotgun", 1 ));
			mHand_Pistol			= static_cast<int>(script::LuaGetNumberFromTableItem( "mHand_Pistol", 1 )); 
			mHand_Empty				= static_cast<int>(script::LuaGetNumberFromTableItem( "mHand_Empty", 1 )); 
			mBody					= static_cast<int>(script::LuaGetNumberFromTableItem( "mBody", 1 ));  
		}
	}

	return(0);
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
Player::Player( InputSystem& inputSystem )
	: PhysicsIdentifier( PHYSICSBASICID_PLAYER, PHYSICSCASTID_PLAYER ) 
	, NavObject( 0 )
	, m_InputSystem(inputSystem)
{
	int i=0;

	m_pScriptData = GetScriptDataHolder();
	m_CameraData = m_pScriptData->GetCameraData();

	m_FPSMode = false;
	m_CamPos = math::Vec3( m_Pos.X+m_CameraData.cam_posx_offset, m_Pos.Y+m_CameraData.cam_posy_offset, m_Pos.Z+m_CameraData.cam_posz_offset );
	m_CamTarget = math::Vec3( m_Pos.X+m_CameraData.cam_targetx_offset, m_Pos.Y+m_CameraData.cam_targety_offset, m_Pos.Z+m_CameraData.cam_targetz_offset);
	m_CamHeight = 0.0f;
	m_TiltAngle = 0.0f;

	m_SmoothRotation = true;
	m_RotationAngle = 0.0f;
	m_TargetRotation = 0.0f;

	m_pModelData = 0;

	m_pPlayerBody = 0;
	m_HasContact = false;

	m_IsRunning = false;
	m_UseTorch = false;

	math::Vec3 pos( 0.0f, 0.0f, 0.0f );
	SetPosition( pos );

	m_RespawnPosition = m_Pos;
	m_RespawnDirection = 0;

	m_WeaponIndex = -1;
	m_HasPistol = false;
	m_HasShotgun = false;

	m_ClothingIndex = 0;

	m_NextBulletIndex = 0;
	m_LastBulletTime = 0.0f;
	math::Vec3 dir( 0.0f, 0.0f, -1.0f );
	for( i=0; i < MAX_PLAYER_BULLETS; ++i )
	{
		m_Bullets[i] = 0;
		m_Bullets[i] = new Bullet( );
	}

	m_PistolBulletMagCount = 12;
	m_PistolBulletTotal = 99;

	m_ShotgunBulletMagCount = 6;
	m_ShotgunBulletTotal = 99;

	m_ShotgunReloadTimer = 0.0f;
	m_ShotgunShellAudioCount = 0;

	m_IsReloading = false;

	m_IsUsing = false;
	m_InUseArea = false;
	m_pUseArea = 0;

	m_LightZoneIndex = -1;
	m_StepAudioIndex = -1;

	m_Health = 100;
	m_IsDead = false;

	m_TakeDamage = false; 
	m_DamageTimer = 0.5f;

	m_PlayerDeathSound = snd::INVALID_SOUNDBUFFER;

	m_PistolAudio = snd::INVALID_SOUNDBUFFER;
	m_ShotgunAudio = snd::INVALID_SOUNDBUFFER;

	m_PistolReloadAudio = snd::INVALID_SOUNDBUFFER;
	m_ShotgunReloadAudio = snd::INVALID_SOUNDBUFFER;

	m_LastStepFrame = -1;

	for( i=0; i < STEP_AUDIO_COUNT; ++i )
	{
		m_StepSounds[i].StepLeft = snd::INVALID_SOUNDBUFFER;
		m_StepSounds[i].StepRight = snd::INVALID_SOUNDBUFFER;
	}

	m_TextureList.clear();

}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
Player::~Player()
{
	for( int i=0; i < MAX_PLAYER_BULLETS; ++i )
	{
		if( m_Bullets[i] != 0 )
		{
			delete m_Bullets[i];
			m_Bullets[i] = 0;
		}
	}

	if( m_pModelData != 0 )
	{
		delete m_pModelData;
		m_pModelData = 0;
	}

	m_UseAreas.clear();
	m_LightZones.clear();
	m_StepAudioZones.clear();

	renderer::RemoveTexture( m_CurrentClothes );
	renderer::RemoveTexture( m_BaseClothing );
	renderer::RemoveTexture( m_BaseBloodClothing );

	//m_TextureList.clear();
}

/////////////////////////////////////////////////////
/// Method: Setup
/// Params: None
///
/////////////////////////////////////////////////////
void Player::Setup()
{
	int i=0;
	const res::TextureResourceStore* rs = 0;

	m_AllowControl = true;

	// setup model and anims
	m_pModelData = 0;
	m_pModelData = new mdl::ModelHGA;
	DBG_ASSERT_MSG( (m_pModelData != 0), "Could not allocate player HGM model" );

	script::LuaScripting::GetInstance()->RegisterFunction( "SetPlayerModelHiIndex",		ScriptSetPlayerModelHiIndex );
	script::LuaScripting::GetInstance()->RegisterFunction( "SetPlayerModelIndex",		ScriptSetPlayerModelIndex );
	script::LoadScript( "lua/model-index/player-model-index.lua" );

	if( H4::GetHiResMode() )
	{
		m_pModelData->LoadBindPose( "hga-hi/pl_game_bind.hga", GL_LINEAR, GL_LINEAR, core::app::GetLowPowerMode() );
	}
	else
	{
		m_pModelData->LoadBindPose( "hga/pl_game_bind.hga", GL_LINEAR, GL_LINEAR, core::app::GetLowPowerMode() );
	}

	for( i=0; i < PlayerAnim::NUM_ANIMS; ++i )
	{
		PlayerAnim::AnimData animData = GetPlayerAnimationData(i);
		m_pModelData->LoadAnimation( animData.szFile, animData.animId, animData.loopFlag );
	}

	// get the clothes
	if( H4::GetHiResMode() )
	{
		//for( i=PLAYER_TEX_SKIN_START; i <= PLAYER_TEX_SKIN_END; ++i )
		//{
			rs = res::GetTextureResource( PLAYER_TEX_SKIN_START );
			DBG_ASSERT( rs != 0 );

			if( !rs->preLoad )
			{
				res::LoadTexture( PLAYER_TEX_SKIN_START );
			}
			m_BaseClothing = rs->texId;

			rs = res::GetTextureResource( PLAYER_TEX_SKIN_START+1 );
			DBG_ASSERT( rs != 0 );

			if( !rs->preLoad )
			{
				res::LoadTexture( PLAYER_TEX_SKIN_START+1 );
			}
			m_BaseBloodClothing = rs->texId;
		//}

		// hand
		m_pModelData->ChangeTextureOnMaterialId( mHand_Empty, 0, m_BaseClothing );

		// weapons
		rs = res::GetTextureResource( PLAYER_TEX_SKIN_WEAPON );
		m_pModelData->ChangeTextureOnMaterialId( mMuzzleFlash_Shotgun, 0, rs->texId );
		m_pModelData->ChangeTextureOnMaterialId( mMuzzleFlash_Pistol, 0, rs->texId );
		m_pModelData->ChangeTextureOnMaterialId( mHand_Shotgun, 0, rs->texId );
		m_pModelData->ChangeTextureOnMaterialId( mHand_Pistol, 0, rs->texId );
	}

	// no culling as they are moving models
	m_pModelData->SetModelGeneralFlags( 0 );
	m_pModelData->SetupAndFree();

	m_pModelData->SetAnimation( PlayerAnim::IDLE );

	if( m_WeaponIndex == WEAPONINDEX_PISTOL )
	{
		m_pModelData->SetSkinDrawState( mHand_Empty, false );
		m_pModelData->SetSkinDrawState( mMuzzleFlash_Pistol, false );
		m_pModelData->SetSkinDrawState( mMuzzleFlash_Shotgun, false );
		m_pModelData->SetSkinDrawState( mHand_Shotgun, false );

		m_pModelData->SetSkinDrawState( mHand_Pistol, true );
		m_pModelData->BlendAnimation( PlayerAnim::AIM_PISTOL, 1.0f, true );
	}
	else if( m_WeaponIndex == WEAPONINDEX_SHOTGUN )
	{
		m_pModelData->SetSkinDrawState( mHand_Empty, false );
		m_pModelData->SetSkinDrawState( mMuzzleFlash_Pistol, false );
		m_pModelData->SetSkinDrawState( mMuzzleFlash_Shotgun, false );
		m_pModelData->SetSkinDrawState( mHand_Pistol, false );

		m_pModelData->SetSkinDrawState( mHand_Shotgun, true );
		m_pModelData->BlendAnimation( PlayerAnim::AIM_SHOTGUN, 1.0f, true );
	}
	else
	{
		// no weapon
		m_pModelData->SetSkinDrawState( mHand_Empty, true );

		m_pModelData->SetSkinDrawState( mMuzzleFlash_Pistol, false );
		m_pModelData->SetSkinDrawState( mMuzzleFlash_Shotgun, false );
		m_pModelData->SetSkinDrawState( mHand_Pistol, false );
		m_pModelData->SetSkinDrawState( mHand_Shotgun, false );
	}

	SetupPhysics();

	// audio
	const char* sr = 0;

	// pistol
	sr = res::GetSoundResource( 500 );
	m_PistolAudio = snd::SoundLoad( sr, m_SndLoad );

	// shotgun
	sr = res::GetSoundResource( 501 );
	m_ShotgunAudio = snd::SoundLoad( sr, m_SndLoad );

	// pistol reload
	sr = res::GetSoundResource( 502 );
	m_PistolReloadAudio = snd::SoundLoad( sr, m_SndLoad );

	// shotgun reload
	sr = res::GetSoundResource( 503 );
	m_ShotgunReloadAudio = snd::SoundLoad( sr, m_SndLoad );

	// pistol empty
	sr = res::GetSoundResource( 504 );
	m_PistolEmptyAudio = snd::SoundLoad( sr, m_SndLoad );

	// shotgun empty
	sr = res::GetSoundResource( 505 );
	m_ShotgunEmptyAudio = snd::SoundLoad( sr, m_SndLoad );

	// player death
	sr = res::GetSoundResource( 506 );
	m_PlayerDeathSound = snd::SoundLoad( sr, m_SndLoad );

	const int STEP_RESOURCE_START = 520;
	int resourceOffset = STEP_RESOURCE_START;
	for( i=0; i < STEP_AUDIO_COUNT; ++i )
	{
		// step left
		sr = res::GetSoundResource( resourceOffset );
		m_StepSounds[i].StepLeft = snd::SoundLoad( sr, m_SndLoad );

		resourceOffset++;

		// step right
		sr = res::GetSoundResource( resourceOffset );
		m_StepSounds[i].StepRight = snd::SoundLoad( sr, m_SndLoad );

		resourceOffset++;
	}
}

/////////////////////////////////////////////////////
/// Method: SetupPhysics
/// Params: None
///
/////////////////////////////////////////////////////
void Player::SetupPhysics()
{
	b2BodyDef bd;
	bd.type = b2_dynamicBody;
	bd.gravityScale = 0.0f;
	bd.awake = true;
	bd.fixedRotation = true;
	bd.linearDamping = 3.0f;
	
	// setup physics
	m_pPlayerBody = physics::PhysicsWorldB2D::GetWorld()->CreateBody(&bd);
	DBG_ASSERT_MSG( (m_pPlayerBody != 0), "Could not create player physics body" );
	
	b2FixtureDef fd;
	fd.friction = 0.1f;
	fd.restitution = 0.0f;
	fd.density = 0.1f;

	fd.filter.groupIndex = PLAYER_AND_BULLET_GROUP;
	fd.filter.categoryBits = PLAYER_CATEGORY;
	fd.filter.maskBits = WORLDHIGH_CATEGORY | WORLDLOW_CATEGORY | ENEMY_CATEGORY;	

	b2CircleShape circleShape;
	circleShape.m_p = b2Vec2( 0.0f, 0.0f );
	circleShape.m_radius = PLAYER_RADIUS;

	fd.shape = &circleShape;

	b2Fixture* pFixture = 0;
	pFixture = m_pPlayerBody->CreateFixture( &fd );
	DBG_ASSERT_MSG( (pFixture != 0), "Could not create player physics fixture" );

	pFixture->SetUserData( reinterpret_cast<void *>(this) );

	// gun extend
	circleShape.m_p = b2Vec2(GUNEXTEND_OFFSETX, 0.0f );
	circleShape.m_radius = GUNEXTEND_RADIUS;

	fd.shape = &circleShape;

	pFixture = m_pPlayerBody->CreateFixture( &fd );
	DBG_ASSERT_MSG( (pFixture != 0), "Could not create player physics fixture" );

	pFixture->SetUserData( reinterpret_cast<void *>(this) );

	// mass setup
	b2MassData massData; 
	massData.mass = 0.1f;
	massData.center = b2Vec2( 0.0f, 0.0f );
	massData.I = 1.0f;
	
	m_pPlayerBody->SetMassData( &massData );

	m_pPlayerBody->SetUserData( reinterpret_cast<void *>(this) );

	m_BoundSphere.fRadius = BOUNDING_RADIUS;
	m_UseSphere.vCenterPoint = math::Vec3( 0.0f, 0.0f, 0.0f );
	m_UseSphere.fRadius = GUNEXTEND_RADIUS;

	Respawn();
}

/////////////////////////////////////////////////////
/// Method: SetupLevelData
/// Params: [in]activeLevel, [in]pNavMesh
///
/////////////////////////////////////////////////////
void Player::SetupLevelData( Level& activeLevel, NavigationMesh* pNavMesh )
{
	m_pLevel = &activeLevel;
	m_Parent = pNavMesh;

	m_UseAreas.clear();
	m_LightZones.clear();
	m_StepAudioZones.clear();

	m_UseAreas = m_pLevel->GetUseAreaList();
	m_LightZones = m_pLevel->GetLightZoneList();
	m_StepAudioZones = m_pLevel->GetStepAudioZoneList();

	PlayerInLightZone();

	if( m_LightZoneIndex != -1 )
	{
		m_LightDiffuseTarget = math::Vec4( m_LightZones[m_LightZoneIndex]->col[0], m_LightZones[m_LightZoneIndex]->col[1], m_LightZones[m_LightZoneIndex]->col[2], m_LightZones[m_LightZoneIndex]->col[3] );
		m_LightDiffuse = m_LightDiffuseTarget;
		renderer::OpenGL::GetInstance()->SetLightDiffuse( 0, m_LightDiffuse );

		m_LightPosTarget = math::Vec4( m_LightZones[m_LightZoneIndex]->dir.v[0], m_LightZones[m_LightZoneIndex]->dir.v[1], m_LightZones[m_LightZoneIndex]->dir.v[2], m_LightZones[m_LightZoneIndex]->dir.v[3] );
		m_LightPos = m_LightPosTarget;
		renderer::OpenGL::GetInstance()->SetLightPosition( 0, m_LightPos );

		math::Vec4 amb( 0.001f, 0.001f, 0.001f, 1.0f );
		renderer::OpenGL::GetInstance()->SetLightAmbient( 0, amb );

		renderer::OpenGL::GetInstance()->SetLightSpotExponent( 0, 0.0f );
		renderer::OpenGL::GetInstance()->SetLightSpotCutoff( 0, 180.0f );

		if( m_LightZones[m_LightZoneIndex]->type == 0 )
		{
			renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_CONSTANT_ATTENUATION, 0.0f );
			renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_LINEAR_ATTENUATION, 0.0f );
			renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_QUADRATIC_ATTENUATION, 0.25f );
		}
		else
		{

			//static float linear = 0.002f;
			//static float quadratic = 0.0005f;
			renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_CONSTANT_ATTENUATION, 0.0f );
			renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_LINEAR_ATTENUATION, 0.0f );
			renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_QUADRATIC_ATTENUATION, 0.25f );
		}
	}
	else
	{
		m_LightDiffuseTarget = math::Vec4( 0.1f, 0.1f, 0.1f, 1.0f );
		m_LightDiffuse = m_LightDiffuseTarget;
		
		renderer::OpenGL::GetInstance()->SetLightDiffuse( 0, m_LightDiffuse );

		m_LightPos.X = 0.0f;
		m_LightPos.Y = 0.0f;
		m_LightPos.Z = 0.0f;
		m_LightPos.W = 0.0f;

		renderer::OpenGL::GetInstance()->SetLightPosition( 0, m_LightPos );
		
		math::Vec4 m_LightSpecular = math::Vec4( 0.0f, 0.0f, 0.0f, 1.0f );
		
		math::Vec4 amb( 0.001f, 0.001f, 0.001f, 1.0f );
		renderer::OpenGL::GetInstance()->SetLightAmbient( 0, amb );

		renderer::OpenGL::GetInstance()->SetLightSpotExponent( 0, 0.0f );
		renderer::OpenGL::GetInstance()->SetLightSpotCutoff( 0, 180.0f );

		renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_CONSTANT_ATTENUATION, 0.0f );
		renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_LINEAR_ATTENUATION, 0.0f );
		renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_QUADRATIC_ATTENUATION, 0.25f );
	}

	PlayerInStepAudioZone();

	if( m_FPSMode )
	{
		m_CamPos = math::Vec3( m_Pos.X-0.1f, m_Pos.Y+1.60f, m_Pos.Z+0.2f );
		m_CamTarget = math::Vec3( m_Pos.X-0.1f, (m_Pos.Y+0.5f)+m_CamHeight, m_Pos.Z+10.0f);
		
		m_CamPos = math::RotateAroundPoint( m_CamPos, m_Pos, math::DegToRad(m_RotationAngle) );
		m_CamTarget = math::RotateAroundPoint( m_CamTarget, m_Pos, math::DegToRad(m_RotationAngle) );
	}
	else
	{
		m_CamPos = math::Vec3( m_Pos.X+m_CameraData.cam_posx_offset, (m_Pos.Y+m_CameraData.cam_posy_offset), m_Pos.Z+m_CameraData.cam_posz_offset );
		m_CamTarget = math::Vec3( m_Pos.X+m_CameraData.cam_targetx_offset, (m_Pos.Y+m_CameraData.cam_targety_offset)+m_CamHeight, m_Pos.Z+m_CameraData.cam_targetz_offset);
		
		m_CamPos = math::RotateAroundPoint( m_CamPos, m_Pos, math::DegToRad(m_RotationAngle) );
		m_CamTarget = math::RotateAroundPoint( m_CamTarget, m_Pos, math::DegToRad(m_RotationAngle) );
	}
	
	// just in case player had died while reloading
	m_IsReloading = false;
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void Player::Draw( bool pauseFlag )
{
	renderer::OpenGL::GetInstance()->SetColour4ub( 255,255,255,255 );

	int i=0;
	if( m_LightZoneIndex != -1 )
	{
		m_LightDiffuseTarget = math::Vec4( m_LightZones[m_LightZoneIndex]->col[0], m_LightZones[m_LightZoneIndex]->col[1], m_LightZones[m_LightZoneIndex]->col[2], m_LightZones[m_LightZoneIndex]->col[3] );
		bool colArrive = math::WithinTolerance( m_LightDiffuse, m_LightDiffuseTarget, 0.05f );
		if( !colArrive )
			m_LightDiffuse = math::Lerp( m_LightDiffuse, m_LightDiffuseTarget, 4.0f*m_LastDelta );
		renderer::OpenGL::GetInstance()->SetLightDiffuse( 0, m_LightDiffuse );

		m_LightPosTarget = math::Vec4( m_LightZones[m_LightZoneIndex]->dir.v[0], m_LightZones[m_LightZoneIndex]->dir.v[1], m_LightZones[m_LightZoneIndex]->dir.v[2], m_LightZones[m_LightZoneIndex]->dir.v[3] );
		bool posArrive = math::WithinTolerance( m_LightPos, m_LightPosTarget, 0.05f );
		if( !posArrive )
			m_LightPos = math::Lerp( m_LightPos, m_LightPosTarget, 4.0f*m_LastDelta );

		renderer::OpenGL::GetInstance()->SetLightPosition( 0, m_LightPos );

		math::Vec4 amb( 0.001f, 0.001f, 0.001f, 1.0f );
		renderer::OpenGL::GetInstance()->SetLightAmbient( 0, amb );

		renderer::OpenGL::GetInstance()->SetLightSpotExponent( 0, 0.0f );
		renderer::OpenGL::GetInstance()->SetLightSpotCutoff( 0, 180.0f );

		if( m_LightZones[m_LightZoneIndex]->type == 0 )
		{
			renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_CONSTANT_ATTENUATION, 1.0f );
			renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_LINEAR_ATTENUATION, 0.0f );
			renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_QUADRATIC_ATTENUATION, 0.0f );
		}
		else
		{

			//static float linear = 0.002f;
			//static float quadratic = 0.0005f;
			renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_CONSTANT_ATTENUATION, 0.0f );
			renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_LINEAR_ATTENUATION, 0.0f );
			renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_QUADRATIC_ATTENUATION, 0.25f );
		}
	}
	else
	{
		m_LightDiffuseTarget = math::Vec4( 0.1f, 0.1f, 0.1f, 1.0f );
		bool colArrive = math::WithinTolerance( m_LightDiffuse, m_LightDiffuseTarget, 0.05f );
		if( !colArrive )
			m_LightDiffuse = math::Lerp( m_LightDiffuse, m_LightDiffuseTarget, 4.0f*m_LastDelta );
		else
		{
			m_LightPos = math::Vec4( 0.0f, 1.0f, 0.0f, m_LightPos.W );
		}

		renderer::OpenGL::GetInstance()->SetLightDiffuse( 0, m_LightDiffuse );

		//m_LightPos.W = m_LightPosTarget.W;
		//bool posArrive = math::WithinTolerance( m_LightPos, m_LightPosTarget, 0.05f );
		//if( !posArrive )
		//	m_LightPos = math::Lerp( m_LightPos, m_LightPosTarget, 4.0f*m_LastDelta );

		renderer::OpenGL::GetInstance()->SetLightPosition( 0, m_LightPos );
		
		math::Vec4 amb( 0.1f, 0.1f, 0.1f, 1.0f );
		renderer::OpenGL::GetInstance()->SetLightAmbient( 0, amb );

		renderer::OpenGL::GetInstance()->SetLightSpotExponent( 0, 0.0f );
		renderer::OpenGL::GetInstance()->SetLightSpotCutoff( 0, 180.0f );
	}

	glPushMatrix();
		glTranslatef( m_Pos.X, m_Pos.Y, m_Pos.Z );
		glRotatef( m_RotationAngle, 0.0f, 1.0f, 0.0f );

		m_pModelData->Draw();
	glPopMatrix();


	bool lightState = renderer::OpenGL::GetInstance()->GetLightingState();
	bool textureState = renderer::OpenGL::GetInstance()->GetTextureState();

	if( lightState )
		renderer::OpenGL::GetInstance()->DisableLighting();
	if( textureState )
		renderer::OpenGL::GetInstance()->DisableTexturing();

	renderer::OpenGL::GetInstance()->SetColour4ub( 128,128,0,128 );

	renderer::OpenGL::GetInstance()->SetCullState( false, GL_BACK );
	for( i=0; i < MAX_PLAYER_BULLETS; ++i )
		m_Bullets[i]->Draw();
	renderer::OpenGL::GetInstance()->SetCullState( true, GL_BACK );

	if( lightState )
		renderer::OpenGL::GetInstance()->EnableLighting();
	if( textureState )
		renderer::OpenGL::GetInstance()->EnableTexturing();
}

/////////////////////////////////////////////////////
/// Method: Draw2D
/// Params: None
///
/////////////////////////////////////////////////////
void Player::Draw2D()
{

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

	renderer::OpenGL::GetInstance()->SetColour4ub( 255,255,255,255 );
	glPushMatrix();
		glTranslatef( m_BoundSphere.vCenterPoint.X, m_BoundSphere.vCenterPoint.Y, m_BoundSphere.vCenterPoint.Z );
		renderer::DrawSphere( m_BoundSphere.fRadius );
	glPopMatrix();

	glPushMatrix();
		glTranslatef( m_UseSphere.vCenterPoint.X, m_UseSphere.vCenterPoint.Y, m_UseSphere.vCenterPoint.Z );
		renderer::DrawSphere( m_UseSphere.fRadius );
	glPopMatrix();

	int i=0;
	for( i=0; i < MAX_PLAYER_BULLETS; ++i )
		m_Bullets[i]->DebugDraw();

	glPushMatrix();
		glTranslatef( m_Pos.X, m_Pos.Y, m_Pos.Z );
		glRotatef( m_RotationAngle, 0.0f, 1.0f, 0.0f );
	
		m_pModelData->DrawBones();
	glPopMatrix();

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

	b2Vec2 bodyPos = m_pPlayerBody->GetWorldPoint(zeroVec2);
	b2Vec2 linVel = m_pPlayerBody->GetLinearVelocity();
	m_pPlayerBody->SetTransform( bodyPos, math::DegToRad(-m_RotationAngle+90.0f) );

	if( m_TakeDamage )
	{
		m_DamageTimer -= deltaTime;
		if( m_DamageTimer <= 0.0f )
		{
			m_DamageTimer = 0.0f;
			m_TakeDamage = false;
		}
	}

	// reload can stop various anims
	UpdateReloading( deltaTime );

	if( m_IsUsing )
	{
		UpdateUsing( deltaTime );
		if( !m_IsUsing ) // just in case a level load was requested
		{
			m_pModelData->SetAnimation( PlayerAnim::IDLE );
			return;
		}
	}
	else
		UpdateNormalB2D(deltaTime);

	for( i=0; i < MAX_PLAYER_BULLETS; ++i )
		m_Bullets[i]->Update( deltaTime );

	UpdateModel(deltaTime);

	bodyPos = m_pPlayerBody->GetWorldPoint(zeroVec2);
	linVel = m_pPlayerBody->GetLinearVelocity();

	// finish up
	m_PrevPos = m_Pos;

	m_Pos.X = bodyPos.x;
	m_Pos.Z = bodyPos.y;

	m_NavCellPos = m_Pos;

	if (m_Parent)
	{
		// compute the next desired location
		NavigationCell* NextCell = 0;

		// test location on the NavigationMesh and resolve collisions
		m_Parent->ResolveMotionOnMesh( m_PrevPos, m_CurrentCell, m_NavCellPos, &NextCell);

		//m_Pos = NextPosition;
		m_CurrentCell = NextCell;
	}

	// try and get to the rotation smoother
	if( m_SmoothRotation )
	{
		if( m_RotationAngle != m_TargetRotation )
		{
			if( m_TargetRotation < m_RotationAngle )
				m_RotationAngle -= ROTATION_INCREMENT*deltaTime;
			else if( m_TargetRotation > m_RotationAngle )
				m_RotationAngle += ROTATION_INCREMENT*deltaTime;

			if( std::fabs(m_RotationAngle-m_TargetRotation) < ROTATION_TOLERANCE  )
				m_RotationAngle = m_TargetRotation;
		}
	}
	else
	{
		m_RotationAngle = m_TargetRotation;
	}

	// update bounding sphere
	math::Vec3 centerOffset(m_Pos.X, m_Pos.Y+0.5f, m_Pos.Z+0.1f);
	m_BoundSphere.vCenterPoint = math::RotateAroundPoint( centerOffset, m_Pos, math::DegToRad(m_RotationAngle) );

	// update use area
	centerOffset = math::Vec3(m_Pos.X+GUNEXTEND_OFFSETX, m_Pos.Y, m_Pos.Z);
	m_UseSphere.vCenterPoint = math::RotateAroundPoint( centerOffset, m_Pos, math::DegToRad(m_RotationAngle-90.0f) );

	m_pPlayerBody->SetAwake( true );
	
	if( m_FPSMode )
	{
		m_CamPos = math::Vec3( m_Pos.X-0.1f, m_Pos.Y+1.60f, m_Pos.Z+0.2f );
		m_CamTarget = math::Vec3( m_Pos.X-0.1f, (m_Pos.Y+0.5f)+m_CamHeight, m_Pos.Z+10.0f);
		
		m_CamPos = math::RotateAroundPoint( m_CamPos, m_Pos, math::DegToRad(m_RotationAngle) );
		m_CamTarget = math::RotateAroundPoint( m_CamTarget, m_Pos, math::DegToRad(m_RotationAngle) );
	}
	else
	{
		m_CamPos = math::Vec3( m_Pos.X+m_CameraData.cam_posx_offset, (m_Pos.Y+m_CameraData.cam_posy_offset), m_Pos.Z+m_CameraData.cam_posz_offset );
		m_CamTarget = math::Vec3( m_Pos.X+m_CameraData.cam_targetx_offset, (m_Pos.Y+m_CameraData.cam_targety_offset)+m_CamHeight, m_Pos.Z+m_CameraData.cam_targetz_offset);
		
		m_CamPos = math::RotateAroundPoint( m_CamPos, m_Pos, math::DegToRad(m_RotationAngle) );
		m_CamTarget = math::RotateAroundPoint( m_CamTarget, m_Pos, math::DegToRad(m_RotationAngle) );
	}	
}

/////////////////////////////////////////////////////
/// Method: UpdateModel
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void Player::UpdateModel( float deltaTime )
{
	PlayerInLightZone();

	m_pModelData->Update(deltaTime);

	PlayerInStepAudioZone();
}

/////////////////////////////////////////////////////
/// Method: UpdateNormalB2D
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void Player::UpdateNormalB2D( float deltaTime )
{
	if( !m_AllowControl )
		return;
	
	b2Vec2 linVel = m_pPlayerBody->GetLinearVelocity();
	
	float MOVE_SPEED = 55.0f;
	float STRAFE_SPEED = 40.0f;

	bool actionState = m_InputSystem.GetAction();
	bool useState = m_InputSystem.GetUse();

	bool reloadState = m_InputSystem.GetReload();

	m_IsRunning = m_InputSystem.GetRun();

	/*if( m_IsRunning )
	{
		MOVE_SPEED = 70.0f;
		//STRAFE_SPEED = 100.0f;
	}*/

	float angleX = std::sin( math::DegToRad(m_RotationAngle) );
	float angleZ = std::cos( math::DegToRad(m_RotationAngle) );

	float VelX = angleX*(MOVE_SPEED*deltaTime);
	float VelZ = angleZ*(MOVE_SPEED*deltaTime);

	// swap weapon
	if( m_InputSystem.GetWeaponSwap() && !m_IsReloading )
	{
		if( m_WeaponIndex != -1 )
		{
			int oldIndex = m_WeaponIndex;

			if( m_HasShotgun )
				m_WeaponIndex = !m_WeaponIndex;
			
			if( (oldIndex != m_WeaponIndex) &&
				(m_WeaponIndex == WEAPONINDEX_PISTOL) )
			{
				m_pModelData->SetSkinDrawState( mHand_Empty, false );
				m_pModelData->SetSkinDrawState( mMuzzleFlash_Pistol, false );
				m_pModelData->SetSkinDrawState( mMuzzleFlash_Shotgun, false );
				m_pModelData->SetSkinDrawState( mHand_Shotgun, false );

				m_pModelData->SetSkinDrawState( mHand_Pistol, true );
				m_pModelData->ClearBlend();
				m_pModelData->BlendAnimation( PlayerAnim::AIM_PISTOL, 1.0f, true );
			}
			else if( (oldIndex != m_WeaponIndex) &&
					(m_WeaponIndex == WEAPONINDEX_SHOTGUN) )
			{
				m_pModelData->SetSkinDrawState( mHand_Empty, false );
				m_pModelData->SetSkinDrawState( mMuzzleFlash_Pistol, false );
				m_pModelData->SetSkinDrawState( mMuzzleFlash_Shotgun, false );
				m_pModelData->SetSkinDrawState( mHand_Pistol, false );

				m_pModelData->SetSkinDrawState( mHand_Shotgun, true );
				m_pModelData->ClearBlend();
				m_pModelData->BlendAnimation( PlayerAnim::AIM_SHOTGUN, 1.0f, true );
			}
		}
	}

	// check for reload
	if( reloadState && !m_IsReloading )
	{
		if( m_WeaponIndex == WEAPONINDEX_PISTOL )
		{
			if( m_PistolBulletMagCount <= 0 )
			{
				if( m_PistolBulletTotal > 0 )
				{
					m_pModelData->BlendAnimation( PlayerAnim::RELOAD_PISTOL, 1.0f, true );
					
					if( m_PistolReloadAudio != snd::INVALID_SOUNDBUFFER )
						AudioSystem::GetInstance()->PlayAudio( m_PistolReloadAudio, zeroVec, true );

					m_IsReloading = true;
				}
			}
		}
		else if( m_WeaponIndex == WEAPONINDEX_SHOTGUN )
		{
			if( m_ShotgunBulletMagCount <= 0 )
			{
				if( m_ShotgunBulletTotal > 0 )
				{
					m_pModelData->BlendAnimation( PlayerAnim::RELOAD_SHOTGUN, 1.0f, true );

					m_ShotgunReloadTimer = 0.0f;
					m_ShotgunShellAudioCount = 0;

					m_IsReloading = true;
				}
			}
		}
	}

	m_LastBulletTime += deltaTime;

	// keep number sane
	if( m_LastBulletTime > 10.0f )
		m_LastBulletTime = BULLET_DELAY;

	// use logic
	m_InUseArea = PlayerInUseArea();

	if( useState && !m_IsReloading )
	{
		if( m_InUseArea )
		{	
			m_IsUsing = true;
			return;
		}
	}

	// shoot logic
	if(m_WeaponIndex != -1 && 
	   actionState && 
	   !m_IsReloading )
	{
		// spawn a bullet	
		if( m_LastBulletTime >= BULLET_DELAY )
		{
			bool allowBullet = true;

			if( m_WeaponIndex == WEAPONINDEX_PISTOL )
			{
				if( m_PistolBulletMagCount > 0 )
				{
					// stealth suit does not use bullets
					if( m_ClothingIndex != 8 )
					{
						m_PistolBulletMagCount--;
					}
				}
				else
				{
					allowBullet = false;
										
					if( m_PistolEmptyAudio != snd::INVALID_SOUNDBUFFER )
						AudioSystem::GetInstance()->PlayAudio( m_PistolEmptyAudio, zeroVec, true );
				}
			}
			else if( m_WeaponIndex == WEAPONINDEX_SHOTGUN )
			{
				if( m_ShotgunBulletMagCount > 0 )
				{
					// stealth suit does not use bullets
					if( m_ClothingIndex != 8 )
					{
						m_ShotgunBulletMagCount--;
					}
				}
				else
				{
					allowBullet = false;

					if( m_ShotgunEmptyAudio != snd::INVALID_SOUNDBUFFER )
						AudioSystem::GetInstance()->PlayAudio( m_ShotgunEmptyAudio, zeroVec, true );
				}
			}

			if( allowBullet )
			{
				m_LastBulletTime = 0.0f;

				if( m_WeaponIndex == 0 )
				{
					SpawnBullet( Bullet::BULLET_PISTOL );

					m_pModelData->SetSkinDrawState( mMuzzleFlash_Pistol, true );
					m_pModelData->BlendAnimation( PlayerAnim::RECOIL_PISTOL, 1.0f, true );

					if( m_PistolAudio != snd::INVALID_SOUNDBUFFER )
						AudioSystem::GetInstance()->PlayAudio( m_PistolAudio, zeroVec, true );
				}
				else
				{
					SpawnBullet( Bullet::BULLET_SHOTGUN );

					m_pModelData->SetSkinDrawState( mMuzzleFlash_Shotgun, true );
					m_pModelData->BlendAnimation( PlayerAnim::RECOIL_SHOTGUN, 1.0f, true );

					if( m_ShotgunAudio != snd::INVALID_SOUNDBUFFER )
						AudioSystem::GetInstance()->PlayAudio( m_ShotgunAudio, zeroVec, true );
				}
			}
		}
	}
	else
	{
		if( m_WeaponIndex == WEAPONINDEX_PISTOL )
		{
			m_pModelData->SetSkinDrawState( mMuzzleFlash_Pistol, false );
			m_pModelData->BlendAnimation( PlayerAnim::AIM_PISTOL, 1.0f );
		}
		else if( m_WeaponIndex == WEAPONINDEX_SHOTGUN )
		{
			m_pModelData->SetSkinDrawState( mMuzzleFlash_Shotgun, false );
			m_pModelData->BlendAnimation( PlayerAnim::AIM_SHOTGUN, 1.0f );
		}
	}
	
	if( m_InputSystem.GetUpVal() != 0.0f )
	{
		float val = m_InputSystem.GetUpVal();
		
		/*if( m_IsRunning )
		{
			VelX *= 1.5f;
			VelZ *= 1.5f;

			b2Vec2 force( VelX*(val/m_InputSystem.GetAnalogueVal()), VelZ*(val/m_InputSystem.GetAnalogueVal()) );
			m_pPlayerBody->ApplyForce( force, zeroVec2 );
			m_pModelData->SetAnimation( PlayerAnim::RUN );
		}
		else*/
		{
			b2Vec2 force( VelX*(val/m_InputSystem.GetAnalogueVal()), VelZ*(val/m_InputSystem.GetAnalogueVal()) );
			m_pPlayerBody->ApplyForce( force, zeroVec2, true );
			m_pModelData->SetAnimation( PlayerAnim::WALK );
		}
	}
	else if( m_InputSystem.GetDownVal() != 0.0f )
	{
		float val = m_InputSystem.GetDownVal();
		
		b2Vec2 force( VelX*(val/m_InputSystem.GetAnalogueVal()), VelZ*(val/m_InputSystem.GetAnalogueVal()) );
		m_pPlayerBody->ApplyForce( force, zeroVec2, true );
		m_pModelData->SetAnimation( PlayerAnim::WALK );
	}

	if( m_InputSystem.GetLeftVal() != 0.0f )
	{
		float val = m_InputSystem.GetLeftVal();
		
		angleX = std::sin( math::DegToRad(m_RotationAngle+90.0f) );
		angleZ = std::cos( math::DegToRad(m_RotationAngle+90.0f) );

		VelX = angleX*(-STRAFE_SPEED*deltaTime);
		VelZ = angleZ*(-STRAFE_SPEED*deltaTime);

		b2Vec2 force( VelX*(val/m_InputSystem.GetAnalogueVal()), VelZ*(val/m_InputSystem.GetAnalogueVal()) );
		m_pPlayerBody->ApplyForce( force, zeroVec2, true );

	}
	else if( m_InputSystem.GetRightVal() != 0.0f )
	{
		float val = m_InputSystem.GetRightVal();
		
		angleX = std::sin( math::DegToRad(m_RotationAngle-90.0f) );
		angleZ = std::cos( math::DegToRad(m_RotationAngle-90.0f) );

		VelX = angleX*(STRAFE_SPEED*deltaTime);
		VelZ = angleZ*(STRAFE_SPEED*deltaTime);

		b2Vec2 force( VelX*(val/m_InputSystem.GetAnalogueVal()), VelZ*(val/m_InputSystem.GetAnalogueVal()) );
		m_pPlayerBody->ApplyForce( force, zeroVec2, true );
	}

	float ROTATE_SPEED = 65.0f;
	/*if( m_IsRunning )
	{
		ROTATE_SPEED = 75.0f;
	}*/
	
	if(  m_InputSystem.GetRotateLeftVal() != 0.0f  )
	{
		float val = m_InputSystem.GetRotateLeftVal();
		
		m_TargetRotation += (ROTATE_SPEED*deltaTime) * (val/m_InputSystem.GetAnalogueVal());
	}
	else if( m_InputSystem.GetRotateRightVal() != 0.0f )
	{
		float val = m_InputSystem.GetRotateRightVal();
		
		m_TargetRotation -= (ROTATE_SPEED*deltaTime) * (val/m_InputSystem.GetAnalogueVal());
	}

	// camera look rotation
	const float LOOK_SPEED = 2.5f;
	if( m_InputSystem.GetLookUpVal() != 0.0f ) 
	{
		float val = m_InputSystem.GetLookUpVal();	
		
		m_CamHeight += (LOOK_SPEED*deltaTime) * (val/m_InputSystem.GetAnalogueVal());
	}
	else if( m_InputSystem.GetLookDownVal() != 0.0f ) 
	{
		float val = m_InputSystem.GetLookDownVal();	
		
		m_CamHeight -= (LOOK_SPEED*deltaTime) * (val/m_InputSystem.GetAnalogueVal());
	}

	const float LOOK_RANGE = 0.85f;
	if( m_CamHeight < -LOOK_RANGE )
		m_CamHeight = -LOOK_RANGE;
	if( m_CamHeight > LOOK_RANGE )
		m_CamHeight = LOOK_RANGE;

	const float MAX_ANGLE = 5.0f;
	m_TiltAngle = m_CamHeight*MAX_ANGLE;

	// create a tilt for the player aim
	math::Vec3 eulerRot( 0.0f, math::DegToRad(-m_TiltAngle), 0.0f );
	math::Quaternion q( eulerRot );
	m_pModelData->SetBoneControllerRotation( BACKJOINT_IDX, q );

	if( m_TargetRotation > 360.0f )
	{
		m_RotationAngle -= 360.0f;
		m_TargetRotation -= 360.0f;
	}
	else if( m_TargetRotation < 0.0f )
	{
		m_RotationAngle += 360.0f;
		m_TargetRotation += 360.0f;
	}

	if( (m_InputSystem.GetUpVal() == 0.0f)  &&
		(m_InputSystem.GetDownVal() == 0.0f) &&
		(m_InputSystem.GetLeftVal() == 0.0f) &&
		(m_InputSystem.GetRightVal() == 0.0f) )
	{
		m_pModelData->SetAnimation( PlayerAnim::IDLE );
	}
	

	// check for step audio time
	if( m_pModelData->GetPrimaryAnimId() == PlayerAnim::WALK )
	{
		const int WALK_LEFT_STEP_FRAME = 16;
		const int WALK_RIGHT_STEP_FRAME = 33;

		if( m_pModelData->GetPrimaryAnim() &&
			(m_pModelData->GetPrimaryAnim()->nCurrentFrame == WALK_LEFT_STEP_FRAME || m_pModelData->GetPrimaryAnim()->nCurrentFrame == WALK_RIGHT_STEP_FRAME) ) 
		{
			if( m_pModelData->GetPrimaryAnim()->nCurrentFrame == WALK_LEFT_STEP_FRAME &&
				m_LastStepFrame != WALK_LEFT_STEP_FRAME )
			{
				if( m_StepAudioIndex != -1 )
				{					
					int type = m_StepAudioZones[m_StepAudioIndex]->type;
					AudioSystem::GetInstance()->PlayAudio( m_StepSounds[type].StepLeft, zeroVec, true, false, 1.0f, 0.25f );
				}
				else
				{
					AudioSystem::GetInstance()->PlayAudio( m_StepSounds[0].StepLeft, zeroVec, true, false, 1.0f, 0.25f );
				}

				m_LastStepFrame = WALK_LEFT_STEP_FRAME;
			}

			if( m_pModelData->GetPrimaryAnim()->nCurrentFrame == WALK_RIGHT_STEP_FRAME &&
				m_LastStepFrame != WALK_RIGHT_STEP_FRAME )
			{
				if( m_StepAudioIndex != -1 )
				{					
					int type = m_StepAudioZones[m_StepAudioIndex]->type;
					AudioSystem::GetInstance()->PlayAudio( m_StepSounds[type].StepRight, zeroVec, true, false, 1.0f, 0.25f );
				}
				else
				{
					AudioSystem::GetInstance()->PlayAudio( m_StepSounds[0].StepRight, zeroVec, true, false, 1.0f, 0.25f );
				}

				m_LastStepFrame = WALK_RIGHT_STEP_FRAME;
			}
		}
	}
	else if( m_pModelData->GetPrimaryAnimId() == PlayerAnim::RUN )
	{
		const int RUN_LEFT_STEP_FRAME = 15;
		const int RUN_RIGHT_STEP_FRAME = 28;

		if( m_pModelData->GetPrimaryAnim() &&
			(m_pModelData->GetPrimaryAnim()->nCurrentFrame == RUN_LEFT_STEP_FRAME || m_pModelData->GetPrimaryAnim()->nCurrentFrame == RUN_RIGHT_STEP_FRAME) ) 
		{
			if( m_pModelData->GetPrimaryAnim()->nCurrentFrame == RUN_LEFT_STEP_FRAME &&
				m_LastStepFrame != RUN_LEFT_STEP_FRAME )
			{
				if( m_StepAudioIndex != -1 )
				{					
					int type = m_StepAudioZones[m_StepAudioIndex]->type;
					AudioSystem::GetInstance()->PlayAudio( m_StepSounds[type].StepLeft, zeroVec, true, false, 1.0f, 0.25f );
				}
				else
				{
					AudioSystem::GetInstance()->PlayAudio( m_StepSounds[0].StepLeft, zeroVec, true, false, 1.0f, 0.25f );
				}

				m_LastStepFrame = RUN_LEFT_STEP_FRAME;
			}

			if( m_pModelData->GetPrimaryAnim()->nCurrentFrame == RUN_RIGHT_STEP_FRAME &&
				m_LastStepFrame != RUN_RIGHT_STEP_FRAME )
			{
				if( m_StepAudioIndex != -1 )
				{					
					int type = m_StepAudioZones[m_StepAudioIndex]->type;
					AudioSystem::GetInstance()->PlayAudio( m_StepSounds[type].StepRight, zeroVec, true, false, 1.0f, 0.25f );
				}
				else
				{
					AudioSystem::GetInstance()->PlayAudio( m_StepSounds[0].StepRight, zeroVec, true, false, 1.0f, 0.25f );
				}

				m_LastStepFrame = RUN_RIGHT_STEP_FRAME;
			}
		}
	}
	else
		m_LastStepFrame = -1;
}

/////////////////////////////////////////////////////
/// Method: UpdateUsing
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void Player::UpdateUsing( float deltaTime )
{
	//if( m_RotationAngle == m_TargetRotation )
	//{
		m_IsUsing = false;

		// tell the object it has been used
		m_pUseArea->OnUse();
		m_pUseArea = 0;
	//}
}

/////////////////////////////////////////////////////
/// Method: UpdateReloading
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void Player::UpdateReloading( float deltaTime )
{
	if( !m_IsReloading )
		return;

	// shell reload
	if( m_WeaponIndex == WEAPONINDEX_SHOTGUN )
	{
		m_ShotgunReloadTimer += deltaTime;
		if( m_ShotgunReloadTimer > 0.5f )
		{
			if( m_ShotgunShellAudioCount < 5 )
			{
				if( m_ShotgunReloadAudio != snd::INVALID_SOUNDBUFFER )
					AudioSystem::GetInstance()->PlayAudio( m_ShotgunReloadAudio, zeroVec, true );

				m_ShotgunShellAudioCount++;
			}

			m_ShotgunReloadTimer = 0.0f;
		}
	}

	if( (m_pModelData->GetSecondaryAnim()->nAnimId != PlayerAnim::RELOAD_PISTOL) &&
		(m_pModelData->GetSecondaryAnim()->nAnimId != PlayerAnim::RELOAD_SHOTGUN) )
	{
		if( m_WeaponIndex == WEAPONINDEX_PISTOL )
		{
			if( m_PistolBulletMagCount <= 0 )
			{
				if( m_PistolBulletTotal > 0 )
				{
					if( m_PistolBulletTotal < 12 )
					{
						m_PistolBulletMagCount = m_PistolBulletTotal;
						m_PistolBulletTotal = 0;
					}
					else
					{
						m_PistolBulletMagCount = 12;
						m_PistolBulletTotal -= 12;
					}
				}
			}
		}
		else if( m_WeaponIndex == WEAPONINDEX_SHOTGUN )
		{
			if( m_ShotgunBulletMagCount <= 0 )
			{
				if( m_ShotgunBulletTotal > 0 )
				{
					if( m_ShotgunBulletTotal < 6 )
					{
						m_ShotgunBulletMagCount = m_ShotgunBulletTotal;
						m_ShotgunBulletTotal = 0;
					}
					else
					{
						m_ShotgunBulletMagCount = 6;
						m_ShotgunBulletTotal -= 6;
					}			
				}
			}
		}
		
		m_IsReloading = false;
	}
}

/////////////////////////////////////////////////////
/// Method: HandleContact
/// Params: [in]idNum, [in]pOtherBody
///
/////////////////////////////////////////////////////
void Player::HandleContact( int idNum, int castId, const ContactPoint* contact, const b2Fixture* pOtherFixture, const b2Body* pOtherBody )
{
	//int numeric1 = -9999;
	//int numeric2 = -9999;
	//int numeric3 = -9999;
	//int numeric4 = -9999;
	//int numeric5 = -9999;

	switch( idNum )
	{
		case PHYSICSBASICID_WORLD:
		{
			physics::PhysicsIdentifier* pUserData = 0;
			if( pOtherBody != 0 )
			{
				pUserData = reinterpret_cast<physics::PhysicsIdentifier*>( pOtherFixture->GetUserData() );
				DBG_ASSERT_MSG( (pUserData != 0), "Player world contact has no physics identifier" );
				
				//numeric1 = pUserData->GetNumeric1();
				//numeric2 = pUserData->GetNumeric2();
				//numeric3 = pUserData->GetNumeric3();
				//numeric4 = pUserData->GetNumeric4();
				//numeric5 = pUserData->GetNumeric5();
			}

			m_HasContact = true;

		}break;

		case PHYSICSBASICID_ENEMY:
		case PHYSICSBASICID_FLOATINGHEAD:
		case PHYSICSBASICID_HANGINGSPIDER:
		case PHYSICSBASICID_CRAWLINGSPIDER:
		{

		}break;

		default:
			// colliding with something that doesn't have an id
			DBG_ASSERT_MSG( 0, "Player in contact with object that has no physics identifier" );
		break;
	}
}

/////////////////////////////////////////////////////
/// Method: ClearContact
/// Params: None
///
/////////////////////////////////////////////////////
void Player::ClearContact( int idNum, int castId, const ContactPoint* contact, const b2Fixture* pOtherFixture, const b2Body* pOtherBody )
{
	switch( idNum )
	{
		case PHYSICSBASICID_WORLD:
		{
			m_HasContact = false;

		}break;

		case PHYSICSBASICID_ENEMY:
		case PHYSICSBASICID_FLOATINGHEAD:
		case PHYSICSBASICID_HANGINGSPIDER:
		case PHYSICSBASICID_CRAWLINGSPIDER:
		{

		}break;

		default:
			// colliding with something that doesn't have an id
			DBG_ASSERT_MSG( 0, "Player removing contact from object that has no physics identifier" );
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////
/// Method: SetWeaponIndex
/// Params: [in]index
///
/////////////////////////////////////////////////////
void Player::SetWeaponIndex( int index )
{
	m_WeaponIndex = index;

	if( m_WeaponIndex == WEAPONINDEX_PISTOL )
	{
		m_pModelData->SetSkinDrawState( mHand_Empty, false );
		m_pModelData->SetSkinDrawState( mMuzzleFlash_Pistol, false );
		m_pModelData->SetSkinDrawState( mMuzzleFlash_Shotgun, false );
		m_pModelData->SetSkinDrawState( mHand_Shotgun, false );

		m_pModelData->SetSkinDrawState( mHand_Pistol, true );
		m_pModelData->BlendAnimation( PlayerAnim::AIM_PISTOL, 1.0f, true );
	}
	else if( m_WeaponIndex == WEAPONINDEX_SHOTGUN )
	{
		m_pModelData->SetSkinDrawState( mHand_Empty, false );
		m_pModelData->SetSkinDrawState( mMuzzleFlash_Pistol, false );
		m_pModelData->SetSkinDrawState( mMuzzleFlash_Shotgun, false );
		m_pModelData->SetSkinDrawState( mHand_Pistol, false );

		m_pModelData->SetSkinDrawState( mHand_Shotgun, true );
		m_pModelData->BlendAnimation( PlayerAnim::AIM_SHOTGUN, 1.0f, true );
	}
	else
	{
		// no weapon
		m_pModelData->SetSkinDrawState( mHand_Empty, true );

		m_pModelData->SetSkinDrawState( mMuzzleFlash_Pistol, false );
		m_pModelData->SetSkinDrawState( mMuzzleFlash_Shotgun, false );
		m_pModelData->SetSkinDrawState( mHand_Pistol, false );
		m_pModelData->SetSkinDrawState( mHand_Shotgun, false );
		m_pModelData->ClearBlend();
	}
}

/////////////////////////////////////////////////////
/// Method: SetClothingIndex
/// Params: [in]index
///
/////////////////////////////////////////////////////
void Player::SetClothingIndex( int index )
{
	if( H4::GetHiResMode() )
	{
		const res::TextureResourceStore* rs = 0;

		switch(index)
		{
			case 0: // inmate
			{
				if( m_ClothingIndex != 0 )
				{
					// not the inmate clothing
					renderer::RemoveTexture( m_CurrentClothes );
				}

				if( m_Health >= 50 )
				{
					m_CurrentClothes = m_BaseClothing;
					m_pModelData->ChangeTextureOnMaterialId( mBody, 0, m_BaseClothing );
				}
				else
				{
					m_CurrentClothes = m_BaseBloodClothing;
					m_pModelData->ChangeTextureOnMaterialId( mBody, 0, m_BaseBloodClothing );
				}
			}break;
			case 1: // Maintenance
			case 2: // Birthday
			case 3: // Leather Biker 
			case 4: // Hidden Tattoo
			case 5: // Assassin's Suit 
			case 6: // Classic Prison Stripes 
			case 7: // Rainbow T-Shirt
			case 8: // Stealth Suit
			{
				if( m_ClothingIndex != 0 )
				{
					// not the inmate clothing
					renderer::RemoveTexture( m_CurrentClothes );
				}

				if( m_Health >= 50 )
				{
					rs = res::GetTextureResource( PLAYER_TEX_SKIN_START+(index*2) );
					DBG_ASSERT( rs != 0 );

					if( !rs->preLoad )
					{
						res::LoadTexture( PLAYER_TEX_SKIN_START+(index*2) );
					}
					m_CurrentClothes = rs->texId;
					m_pModelData->ChangeTextureOnMaterialId( mBody, 0, m_CurrentClothes );
				}
				else
				{
					rs = res::GetTextureResource( PLAYER_TEX_SKIN_START+(index*2)+1 );
					DBG_ASSERT( rs != 0 );

					if( !rs->preLoad )
					{
						res::LoadTexture( PLAYER_TEX_SKIN_START+(index*2)+1 );
					}
					m_CurrentClothes = rs->texId;
					m_pModelData->ChangeTextureOnMaterialId( mBody, 0, m_CurrentClothes );
				}
			}break;
			default:
				DBG_ASSERT(0);
				break;
		}

		m_ClothingIndex = index;

		//if( m_Health >= 50 )
		//	m_pModelData->ChangeTextureOnMaterialId( mBody, 0, m_TextureList[(m_ClothingIndex*2)] );
		//else
		//	m_pModelData->ChangeTextureOnMaterialId( mBody, 0, m_TextureList[(m_ClothingIndex*2)+1] );
	}
	else
	{
		m_ClothingIndex = index;

		switch(m_ClothingIndex)
		{
			case 0: // inmate
			{
				if( m_Health >= 50 )
					m_pModelData->ChangeTextureOnMaterial( mBody, 0, 0 );
				else
					m_pModelData->ChangeTextureOnMaterial( mBody, 0, 2 );
			}break;
			case 1: // Maintenance
			{
				if( m_Health >= 50 )
					m_pModelData->ChangeTextureOnMaterial( mBody, 0, 3 );
				else
					m_pModelData->ChangeTextureOnMaterial( mBody, 0, 4 );
			}break;
			case 2: // Birthday
			{	
				if( m_Health >= 50 )
					m_pModelData->ChangeTextureOnMaterial( mBody, 0, 13 );
				else
					m_pModelData->ChangeTextureOnMaterial( mBody, 0, 14 );
			}break;
			case 3: // Leather Biker 
			{
				if( m_Health >= 50 )
					m_pModelData->ChangeTextureOnMaterial( mBody, 0, 15 );
				else
					m_pModelData->ChangeTextureOnMaterial( mBody, 0, 16 );
			}break;
			case 4: // Hidden Tattoo
			{
				if( m_Health >= 50 )
					m_pModelData->ChangeTextureOnMaterial( mBody, 0, 17 );
				else
					m_pModelData->ChangeTextureOnMaterial( mBody, 0, 18 );
			}break;
			case 5: // Assassin's Suit 
			{
				if( m_Health >= 50 )
					m_pModelData->ChangeTextureOnMaterial( mBody, 0, 5 );
				else
					m_pModelData->ChangeTextureOnMaterial( mBody, 0, 6 );
			}break;
			case 6: // Classic Prison Stripes 
			{
				if( m_Health >= 50 )
					m_pModelData->ChangeTextureOnMaterial( mBody, 0, 7 );
				else
					m_pModelData->ChangeTextureOnMaterial( mBody, 0, 8 );
			}break;
			case 7: // Rainbow T-Shirt
			{
				if( m_Health >= 50 )
					m_pModelData->ChangeTextureOnMaterial( mBody, 0, 9 );
				else
					m_pModelData->ChangeTextureOnMaterial( mBody, 0, 10 );
			}break;
			case 8: // Stealth Suit
			{
				if( m_Health >= 50 )
					m_pModelData->ChangeTextureOnMaterial( mBody, 0, 11 );
				else
					m_pModelData->ChangeTextureOnMaterial( mBody, 0, 12 );
			}break;
			default:
				DBG_ASSERT(0);
				break;
		}
	}
}

/////////////////////////////////////////////////////
/// Method: SpawnBullet
/// Params: None
///
/////////////////////////////////////////////////////
void Player::SpawnBullet( Bullet::BulletSource src )
{
	const float IMPULSE = 5.0f;
	math::Vec3 dir;

	const mdl::TBoneJoint* pJoint = &m_pModelData->GetBoneJointsPtr()[GUNJOINT_IDX];

	float xWeaponOffset = 0.0f;

	if( m_WeaponIndex == WEAPONINDEX_PISTOL )
		xWeaponOffset = 0.1f;
	else
		xWeaponOffset = 0.02f;

	math::Vec3 gunPos = math::Vec3( pJoint->final.m41-xWeaponOffset, pJoint->final.m42, pJoint->final.m43 );

	dir.X = std::sin(math::DegToRad(m_RotationAngle));
	dir.Y = m_CamTarget.Y-gunPos.Y;
	dir.Z = std::cos(math::DegToRad(m_RotationAngle));
	dir.normalise();

	math::Vec3 gunOffset = math::RotateAroundPoint( gunPos, zeroVec, math::DegToRad(m_RotationAngle) );
	math::Vec3 spawnPos( m_Pos.X+gunOffset.X, gunOffset.Y+0.05f, m_Pos.Z+gunOffset.Z );

	math::Vec3 impulse( dir.X*IMPULSE, (dir.Y*IMPULSE)*1.1f, dir.Z*IMPULSE );
	m_Bullets[m_NextBulletIndex]->Spawn( src, spawnPos, m_RotationAngle, impulse );

	m_NextBulletIndex++;

	if( m_NextBulletIndex >= MAX_PLAYER_BULLETS )
		m_NextBulletIndex = 0;
}

/////////////////////////////////////////////////////
/// Method: ClearBullets
/// Params: None
///
/////////////////////////////////////////////////////
void Player::ClearBullets()
{
	int i=0;

	for( i=0; i < MAX_PLAYER_BULLETS; ++i )
		m_Bullets[i]->Clear();

	m_NextBulletIndex = 0;
}

/////////////////////////////////////////////////////
/// Method: PlayerInUseArea
/// Params: None
///
/////////////////////////////////////////////////////
bool Player::PlayerInUseArea()
{
	unsigned int i=0;
	for( i=0; i < m_UseAreas.size(); ++i )
	{
		bool inArea = m_UseSphere.SphereCollidesWithSphere( m_UseAreas[i]->GetSphere() );

		if( inArea && 
			m_UseAreas[i]->GetActiveState() )
		{
			math::Vec3 usePos = m_UseAreas[i]->GetPos();

			math::Vec3 dir1;
			dir1.X = std::sin( math::DegToRad(m_RotationAngle) );
			dir1.Y = 0.0f;
			dir1.Z = std::cos( math::DegToRad(m_RotationAngle) );
			dir1.normalise();

			math::Vec3 dir2;
			dir2.X = std::sin( math::DegToRad(m_UseAreas[i]->GetAngle()) );
			dir2.Y = 0.0f;
			dir2.Z = std::cos( math::DegToRad(m_UseAreas[i]->GetAngle()) );
			dir2.normalise();

			float angle = math::RadToDeg( math::AngleBetweenVectors(dir1, dir2) );
		
			if( (m_UseAreas[i]->GetAngle() == 360.0f) ||
				((angle <= 30.0f) && 
				(angle >= -30.0f)) )
			{
				m_pUseArea = m_UseAreas[i];
				return(true);
			}
			else
				return(false);
		}
	}

	return(false);
}

/////////////////////////////////////////////////////
/// Method: PlayerInLightZone
/// Params: None
///
/////////////////////////////////////////////////////
bool Player::PlayerInLightZone()
{
	unsigned int i=0;
	bool inArea = false;
	int index = -1;

	for( i=0; i < m_LightZones.size(); ++i )
	{
		if( m_LightZones[i]->angle != 0.0f )
		{
			inArea = m_LightZones[i]->obb.OBBCollidesWithSphere( m_BoundSphere );
			index = i;
		}
		else
		{
			inArea = m_LightZones[i]->aabb.AABBCollidesWithSphere( m_BoundSphere );
			index = i;
		}

		if( inArea )
		{
			m_LightZoneIndex = index;
			return(true);
		}
	}

	m_LightZoneIndex = -1;
	return(false);
}

/////////////////////////////////////////////////////
/// Method: PlayerInStepAudioZone
/// Params: None
///
/////////////////////////////////////////////////////
bool Player::PlayerInStepAudioZone()
{
	unsigned int i=0;
	bool inArea = false;
	int index = -1;

	for( i=0; i < m_StepAudioZones.size(); ++i )
	{
		if( m_StepAudioZones[i]->angle != 0.0f )
		{
			inArea = m_StepAudioZones[i]->obb.OBBCollidesWithSphere( m_BoundSphere );
			index = i;
		}
		else
		{
			inArea = m_StepAudioZones[i]->aabb.AABBCollidesWithSphere( m_BoundSphere );
			index = i;
		}

		if( inArea )
		{
			m_StepAudioIndex = index;
			return(true);
		}
	}

	m_StepAudioIndex = -1;
	return(false);
}

/////////////////////////////////////////////////////
/// Method: ClearMovement
/// Params: None
///
/////////////////////////////////////////////////////
void Player::ClearMovement( bool resetAngles )
{
	m_pPlayerBody->SetAngularVelocity(0.0f);
	m_pPlayerBody->SetLinearVelocity(zeroVec2);

	b2Vec2 bodyPos = m_pPlayerBody->GetWorldPoint(zeroVec2);

	m_PrevPos = m_Pos;

	m_Pos.X = bodyPos.x;
	m_Pos.Z = bodyPos.y;

	m_NavCellPos = m_Pos;
	m_LightZoneIndex = -1;
	m_StepAudioIndex = -1;

	if( resetAngles )
	{
		m_CamHeight = 0.0f;
		m_TiltAngle = 0.0f;
	}
}

/////////////////////////////////////////////////////
/// Method: ApplyImpulse
/// Params: [in]impulse
///
/////////////////////////////////////////////////////
void Player::ApplyImpulse( b2Vec2& impulse )
{
	m_pPlayerBody->ApplyLinearImpulse( impulse, zeroVec2, true );
}

/////////////////////////////////////////////////////
/// Method: GiveHealth
/// Params: [in]amount
///
/////////////////////////////////////////////////////
void Player::GiveHealth( int amount )
{
	m_Health += amount;

	if( m_Health > 100 )
		m_Health = 100;

	SetClothingIndex(m_ClothingIndex);
}

/////////////////////////////////////////////////////
/// Method: GiveAmmo
/// Params: [in]pistolAmmo, [in]shotgunAmmo
///
/////////////////////////////////////////////////////
void Player::GiveAmmo( int pistolAmmo, int shotgunAmmo )
{
	m_PistolBulletTotal += pistolAmmo;
	if( m_PistolBulletTotal > 99 )
		m_PistolBulletTotal = 99;

	m_ShotgunBulletTotal += shotgunAmmo;
	if( m_ShotgunBulletTotal > 99 )
		m_ShotgunBulletTotal = 99;
}

/////////////////////////////////////////////////////
/// Method: Debug_DisablePlayerCollision
/// Params: None
///
/////////////////////////////////////////////////////		
void Player::Debug_DisablePlayerCollision()
{
	b2Fixture* fixtureList = 0;
	fixtureList = m_pPlayerBody->GetFixtureList();

	b2Filter fd;

	// hits nothing
	fd.groupIndex = PLAYER_AND_BULLET_GROUP;
	fd.categoryBits = 0;
	fd.maskBits = 0;

	while( fixtureList )
	{
		fixtureList->SetFilterData( fd );

		fixtureList = fixtureList->GetNext();
	}
}

/////////////////////////////////////////////////////
/// Method: Debug_EnablePlayerCollision
/// Params: None
///
/////////////////////////////////////////////////////
void Player::Debug_EnablePlayerCollision()
{
	b2Fixture* fixtureList = 0;
	fixtureList = m_pPlayerBody->GetFixtureList();

	b2Filter fd;

	// hits nothing
	fd.groupIndex = PLAYER_AND_BULLET_GROUP;
	fd.categoryBits = PLAYER_CATEGORY;
	fd.maskBits = WORLDHIGH_CATEGORY | WORLDLOW_CATEGORY | ENEMY_CATEGORY;	

	while( fixtureList )
	{
		fixtureList->SetFilterData( fd );

		fixtureList = fixtureList->GetNext();
	}
}