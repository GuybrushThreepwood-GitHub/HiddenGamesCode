

/*===================================================================
	File: MicroGameWiring.cpp
	Game: H4

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "CollisionBase.h"
#include "SoundBase.h"
#include "ModelBase.h"
#include "InputBase.h"

#include "AppConsts.h"
#include "H4.h"

#include "Audio/AudioSystem.h"

#include "Resources/SoundResources.h"
#include "Resources/ModelResources.h"
#include "Resources/StringResources.h"
#include "MicroGamePickupItem.h"

// generated files
#include "GameStates/UI/GeneratedFiles/microgame_pickupitem.h" // mesh element list
#include "GameStates/UI/GeneratedFiles/microgame_pickupitem.hui.h" // UI element list

namespace
{
	const int MAX_TOUCH_TESTS = 2;
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
MicroGamePickupItem::MicroGamePickupItem( const math::Vec3& camPos, const math::Vec3& camLookAt, float maxYRotation, float maxXRotation, float minZoom, float maxZoom, int objectId, int uiModelId, int description1Index, int description2Index )
{
	m_ItemModel = 0;
	m_PickUIModel = 0;

	m_CamPos = camPos;
	m_CamLookAt = camLookAt;

	m_MaxYRot = maxYRotation;
	m_MaxXRot = maxXRotation;

	m_CurrentZoom = 0.0f;
	m_MinZoom = minZoom;
	m_MaxZoom = maxZoom;

	m_ItemModelId = objectId;
	m_UIModelId = uiModelId;

	m_Rot = math::Vec3( 0.0f, 0.0f, 0.0f );

	m_Rot.X = m_MaxXRot*0.5f;
	m_Rot.Y = m_MaxYRot*0.25f;


	if( description1Index != -1 )
		m_Description1 = res::GetScriptString( description1Index );
	else
		m_Description1 = 0;

	if( description2Index != -1 )
		m_Description2 = res::GetScriptString( description2Index );
	else
		m_Description2 = 0;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
MicroGamePickupItem::~MicroGamePickupItem()
{

}

/////////////////////////////////////////////////////
/// Method: Initialise
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGamePickupItem::Initialise()
{

}

/////////////////////////////////////////////////////
/// Method: Release
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGamePickupItem::Release()
{

}

/////////////////////////////////////////////////////
/// Method: OnEnter
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGamePickupItem::OnEnter()
{
	int i=0;

	// achievement check
// 6004 shotgun
// 6025 disembodiedhead1.hgm
// 6026 disembodiedhead2.hgm
// 6027 disembodiedhead3.hgm
// 6028 disembodiedhead4.hgm
// 6029 disembodiedhead5.hgm
// 6031 rope.hgm

	switch( m_ItemModelId )
	{
		case 6004:
		{
			// disembodiedhead1
			GameSystems::GetInstance()->AwardAchievement(1);
		}break;
		case 6025:
		{
			// disembodiedhead1
			GameSystems::GetInstance()->AwardAchievement(3);
		}break;
		case 6026:
		{
			// disembodiedhead2
			GameSystems::GetInstance()->AwardAchievement(4);
		}break;
		case 6027:
		{
			// disembodiedhead3
			GameSystems::GetInstance()->AwardAchievement(5);
		}break;
		case 6028:
		{
			// disembodiedhead4
			GameSystems::GetInstance()->AwardAchievement(6);
		}break;
		case 6029:
		{
			// disembodiedhead5
			GameSystems::GetInstance()->AwardAchievement(7);
		}break;
		case 6031:
		{
			// rope 
			GameSystems::GetInstance()->AwardAchievement(2);
		}break;

		default:
			break;
	}

	if( H4::GetHiResMode() )
	{
		// pistol/shotgun hi res
		if( m_ItemModelId == 6001 )
			m_ItemModelId = 9001;
		else
		if( m_ItemModelId == 6004 )
			m_ItemModelId = 9004;

		// hi res heads
		if( m_ItemModelId == 6025 )
			m_ItemModelId = 9025;
		else 
		if( m_ItemModelId == 6026 )
			m_ItemModelId = 9026;
		else
		if( m_ItemModelId == 6027 )
			m_ItemModelId = 9027;
		else 
		if( m_ItemModelId == 6028 )
			m_ItemModelId = 9028;
		else
		if( m_ItemModelId == 6029 )
			m_ItemModelId = 9029;
	}

	m_ItemModel = res::LoadModel( m_ItemModelId );
	DBG_ASSERT( (m_ItemModel != 0) );

	m_PickUIModel = res::LoadModel( m_UIModelId );
	DBG_ASSERT( (m_PickUIModel != 0) );

	math::Vec2 srcAssetDims( static_cast<float>(core::app::GetBaseAssetsWidth()), static_cast<float>(core::app::GetBaseAssetsHeight()) );
	m_PickupUI.Load( "hui/microgame_pickupitem.hui", srcAssetDims );

	math::Vec3 scaleFactor = math::Vec3( core::app::GetWidthScale(), core::app::GetHeightScale(), 1.0f );
	for( i=PICKUPITEM_BGMESH; i <= PICKUPITEM_INSTRUCTIONSMESH; ++i )
	{		
		m_PickUIModel->SetMeshScale(i, scaleFactor);
	}

	if( !core::IsEmptyString( m_Description1 ) )
		m_PickupUI.ChangeElementText( PICKUPITEM_DESCRIPTION1, m_Description1 );
	else
		m_PickupUI.ChangeElementText( PICKUPITEM_DESCRIPTION1, "" );

	if( !core::IsEmptyString( m_Description2 ) )
		m_PickupUI.ChangeElementText( PICKUPITEM_DESCRIPTION2, m_Description2 );
	else
		m_PickupUI.ChangeElementText( PICKUPITEM_DESCRIPTION2, "" );
}

/////////////////////////////////////////////////////
/// Method: OnExit
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGamePickupItem::OnExit()
{
	res::RemoveModel( m_ItemModel );
	m_ItemModel = 0;

	res::RemoveModel( m_PickUIModel );
	m_PickUIModel = 0;

	int docCount = 0;
	// criminal docs
	docCount += GameSystems::GetInstance()->GetDataValue( "criminal_docs_1" );
	docCount += GameSystems::GetInstance()->GetDataValue( "criminal_docs_2" );
	docCount += GameSystems::GetInstance()->GetDataValue( "criminal_docs_3" );
	docCount += GameSystems::GetInstance()->GetDataValue( "criminal_docs_4" );
	docCount += GameSystems::GetInstance()->GetDataValue( "criminal_docs_5" );

	if( docCount >= 5 )
	{
		GameSystems::GetInstance()->AwardAchievement(20);
	}

	int tokenCount = 0;
	// tokens
	tokenCount += GameSystems::GetInstance()->GetDataValue( "hidden_token_1" );
	if( GameSystems::GetInstance()->GetDataValue( "hidden_token_1" ) >= 1 )
		GameSystems::GetInstance()->AwardAchievement(9);

	tokenCount += GameSystems::GetInstance()->GetDataValue( "hidden_token_2" );
	if( GameSystems::GetInstance()->GetDataValue( "hidden_token_2" ) >= 1 )
		GameSystems::GetInstance()->AwardAchievement(10);

	tokenCount += GameSystems::GetInstance()->GetDataValue( "hidden_token_3" );
	if( GameSystems::GetInstance()->GetDataValue( "hidden_token_3" ) >= 1 )
		GameSystems::GetInstance()->AwardAchievement(11);

	tokenCount += GameSystems::GetInstance()->GetDataValue( "hidden_token_4" );
	if( GameSystems::GetInstance()->GetDataValue( "hidden_token_4" ) >= 1 )
		GameSystems::GetInstance()->AwardAchievement(12);

	tokenCount += GameSystems::GetInstance()->GetDataValue( "hidden_token_5" );
	if( GameSystems::GetInstance()->GetDataValue( "hidden_token_5" ) >= 1 )
		GameSystems::GetInstance()->AwardAchievement(13);

	if( tokenCount >= 5 )
	{
		GameSystems::GetInstance()->AwardAchievement(14);
	}

	int headCount = 0;
	// head hunter count
	headCount += GameSystems::GetInstance()->GetDataValue( "head_hunter_1" );
	headCount += GameSystems::GetInstance()->GetDataValue( "head_hunter_2" );
	headCount += GameSystems::GetInstance()->GetDataValue( "head_hunter_3" );
	headCount += GameSystems::GetInstance()->GetDataValue( "head_hunter_4" );
	headCount += GameSystems::GetInstance()->GetDataValue( "head_hunter_5" );

	if( headCount >= 5 )
	{
		GameSystems::GetInstance()->AwardAchievement(8);
	}
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void MicroGamePickupItem::Update( float deltaTime )
{
	math::Vec3 zeroVec( 0.0f, 0.0f, 0.0f );
	int i=0;

	// update UI elements
	m_PickupUI.Update( TOUCH_SIZE*0.5f, MAX_TOUCH_TESTS, deltaTime );

	if( m_PickupUI.CheckElementForSingleTouch(PICKUPITEM_EXIT) )
	{
		m_Complete = true;
		m_ReturnCode = true;
		return;
	}

#if defined( BASE_PLATFORM_iOS ) || defined( BASE_PLATFORM_ANDROID )
	for( i=0; i < 2; ++i )
	{		
		const input::TInputState::TouchData* pData = 0;
		pData = input::GetTouch(i);
		
		if( pData->bActive == false )
			continue;

		if( input::gInputState.nTouchCount == 1 )
		{
			//if( pData->nYDelta > 0.0f )
				m_Rot.X -= (pData->nYDelta*100.0f)*deltaTime;
			//else if( pData->nYDelta < 0.0f )
			//	m_Rot.X += ROTATION_XSPEED*deltaTime;

			//if( pData->nXDelta > 0.0f )
				m_Rot.Y += (pData->nXDelta*100.0f)*deltaTime;
			//else if( pData->nXDelta < 0.0f )
			//	m_Rot.Y -= ROTATION_YSPEED*deltaTime;

			if( m_MaxYRot < 180.0f )
			{
				if( m_Rot.Y > m_MaxYRot )
					m_Rot.Y = m_MaxYRot;
				else if( m_Rot.Y < -m_MaxYRot )
					m_Rot.Y = -m_MaxYRot;
			}

			if( m_MaxXRot < 180.0f )
			{
				if( m_Rot.X > m_MaxXRot )
					m_Rot.X = m_MaxXRot;
				else if( m_Rot.X < -m_MaxXRot )
					m_Rot.X = -m_MaxXRot;
			}
		}
		else
		{
			//if( pData->nYDelta > 0.0f )
			//	m_CurrentZoom -= ZOOM_SPEED*deltaTime;
			//else if( pData->nYDelta < 0.0f )
			//	m_CurrentZoom += ZOOM_SPEED*deltaTime;
			
			if( i == 0 )
			{
				m_CurrentZoom += (pData->nXDelta*0.1f)*deltaTime;
				m_CurrentZoom += (pData->nYDelta*0.1f)*deltaTime;
			}
			else
			{
				m_CurrentZoom -= (pData->nXDelta*0.1f)*deltaTime;
				m_CurrentZoom -= (pData->nYDelta*0.1f)*deltaTime;				
			}
			
			if( m_CurrentZoom < m_MinZoom )
				m_CurrentZoom = m_MinZoom;
			
			if( m_CurrentZoom > m_MaxZoom )
				m_CurrentZoom = m_MaxZoom;			
		}
		
	}
#else
	// rotation
	const float ROTATION_XSPEED = 180.0f;
	const float ROTATION_YSPEED = 360.0f;
	
	// zoom speed
	const float ZOOM_SPEED = 1.0f;
	
	if( input::gInputState.TouchesData[input::FIRST_TOUCH].bPress ||
		input::gInputState.TouchesData[input::FIRST_TOUCH].bHeld )
	{
		if( input::gInputState.nMouseYDelta > 0.0f )
			m_Rot.X -= ROTATION_XSPEED*deltaTime;
		else if( input::gInputState.nMouseYDelta < 0.0f )
			m_Rot.X += ROTATION_XSPEED*deltaTime;

		if( input::gInputState.nMouseXDelta > 0.0f )
			m_Rot.Y += ROTATION_YSPEED*deltaTime;
		else if( input::gInputState.nMouseXDelta < 0.0f )
			m_Rot.Y -= ROTATION_YSPEED*deltaTime;

		if( m_MaxYRot < 180.0f )
		{
			if( m_Rot.Y > m_MaxYRot )
				m_Rot.Y = m_MaxYRot;
			else if( m_Rot.Y < -m_MaxYRot )
				m_Rot.Y = -m_MaxYRot;
		}

		if( m_MaxXRot < 180.0f )
		{
			if( m_Rot.X > m_MaxXRot )
				m_Rot.X = m_MaxXRot;
			else if( m_Rot.X < -m_MaxXRot )
				m_Rot.X = -m_MaxXRot;
		}
	}

	// zoom
	if( input::gInputState.RButtonPressed )
	{
		if( input::gInputState.nMouseYDelta > 0.0f )
			m_CurrentZoom -= ZOOM_SPEED*deltaTime;
		else if( input::gInputState.nMouseYDelta < 0.0f )
			m_CurrentZoom += ZOOM_SPEED*deltaTime;

		if( m_CurrentZoom < m_MinZoom )
			m_CurrentZoom = m_MinZoom;

		if( m_CurrentZoom > m_MaxZoom )
			m_CurrentZoom = m_MaxZoom;
	}
#endif //
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGamePickupItem::Draw()
{
	renderer::OpenGL::GetInstance()->ClearColour( 0.0f, 0.0f, 0.0f, 1.0f );

	renderer::OpenGL::GetInstance()->DepthMode( false, GL_LESS );
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	renderer::OpenGL::GetInstance()->AlphaMode( false, GL_ALWAYS, 0.0f );

	renderer::OpenGL::GetInstance()->SetNearFarClip( -100.0f, 100.0f );
	renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );

	m_PickUIModel->Draw();

	renderer::OpenGL::GetInstance()->SetNearFarClip( 0.1f, 50.0f );
	renderer::OpenGL::GetInstance()->SetupPerspectiveView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );
	renderer::OpenGL::GetInstance()->SetLookAt( m_CamPos.X, m_CamPos.Y, m_CamPos.Z + m_CurrentZoom, m_CamLookAt.X, m_CamLookAt.Y, m_CamLookAt.Z );

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_LESS );

	glPushMatrix();
		glRotatef( m_Rot.X, 1.0f, 0.0f, 0.0f );
		glRotatef( m_Rot.Y, 0.0f, 1.0f, 0.0f );
		m_ItemModel->Draw();
	glPopMatrix();

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_ALWAYS );
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	renderer::OpenGL::GetInstance()->SetNearFarClip( -100.0f, 100.0f );
	renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );

	m_PickupUI.Draw();
}

