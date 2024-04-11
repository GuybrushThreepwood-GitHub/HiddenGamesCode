
/*===================================================================
	File: IconsTabletSDUI.cpp
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "SoundBase.h"
#include "CollisionBase.h"
#include "Input/Input.h"
#include "ModelBase.h"
#include "ScriptBase.h"

#include "CabbyConsts.h"
#include "Cabby.h"

#include "StateManage/IState.h"

#include "Resources/ModelResources.h"
#include "Resources/StringResources.h"

#include "GameEffects/FullscreenEffects.h"
#include "Profiles/ProfileManager.h"

#include "GameStates/UI/UIIds.h"
#include "GameStates/UI/UIBaseState.h"
#include "GameStates/UI/IconsTabletSDUI.h"
#include "GameStates/UI/UIFileLoader.h"

#include "GameStates/UI/GeneratedFiles/icons_tablet_sd.hgm.h"
#include "GameStates/UI/GeneratedFiles/icons_tablet_sd.hui.h"

namespace
{
	const float AREA_RADIUS = 4.0f;

	math::Vec4Lite activeIcon( 255, 255, 255, 255 );
	math::Vec4Lite inactiveIcon( 255, 255, 255, 64 );
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
IconsTabletSDUI::IconsTabletSDUI( StateManager& stateManager, InputSystem& inputSystem )
: IBaseGameState( stateManager, inputSystem, UI_ICONSTABLETSD )
{
	m_Player = PhysicsWorld::GetPlayer();
	m_Customers = GameSystems::GetInstance()->GetCustomerManager();

	m_UIMesh = 0;

	m_FinishedTransitionIn = true;
	m_FinishedTransitionOut = true;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
IconsTabletSDUI::~IconsTabletSDUI()
{
	m_Player = 0;
	m_Customers = 0;
}

/////////////////////////////////////////////////////
/// Method: Enter
/// Params: None
///
/////////////////////////////////////////////////////
void IconsTabletSDUI::Enter()
{
	math::Vec2 srcDims( 768.0f, 1024.0f );
	m_UIMesh = res::LoadModel(2003);
	DBG_ASSERT( m_UIMesh != 0 );

	m_Elements.Load( "assets/ui/icons_tablet_sd.hui", srcDims, m_UIMesh );

	m_UIMesh->SetMeshDrawState( -1, false );
	m_Elements.ChangeElementDrawState( false );

	m_GameData = GetScriptDataHolder()->GetGameData();

	m_PortAreaList = m_Player->GetPortAreaList();

	if( renderer::OpenGL::GetInstance()->GetIsRotated() )
	{
		m_HalfWidth = static_cast<const float>(core::app::GetOrientationHeight()/2);
		m_HalfHeight = static_cast<const float>(core::app::GetOrientationWidth()/2);
	
		m_OriWidth = static_cast<const float>(core::app::GetOrientationHeight());
		m_OriHeight = static_cast<const float>(core::app::GetOrientationWidth());	
	}
	else
	{
		m_HalfWidth = static_cast<const float>(core::app::GetOrientationWidth()/2);
		m_HalfHeight = static_cast<const float>(core::app::GetOrientationHeight()/2);
	
		m_OriWidth = static_cast<const float>(core::app::GetOrientationWidth());
		m_OriHeight = static_cast<const float>(core::app::GetOrientationHeight());
	}
}

/////////////////////////////////////////////////////
/// Method: Exit
/// Params: None
///
/////////////////////////////////////////////////////
void IconsTabletSDUI::Exit()
{
	if( m_UIMesh != 0 )
	{
		res::RemoveModel( m_UIMesh );
		m_UIMesh = 0;
	}
}

/////////////////////////////////////////////////////
/// Method: PrepareTransitionIn
/// Params: None
///
/////////////////////////////////////////////////////
void IconsTabletSDUI::PrepareTransitionIn()
{
	m_FinishedTransitionIn = false;
	m_FadeTransition = 255;
}

/////////////////////////////////////////////////////
/// Method: PrepareTransitionOut
/// Params: None
///
/////////////////////////////////////////////////////
void IconsTabletSDUI::PrepareTransitionOut()
{
	m_FinishedTransitionOut = false;
	m_FadeTransition = 0;
}

/////////////////////////////////////////////////////
/// Method: TransitionIn
/// Params: None
///
/////////////////////////////////////////////////////
int IconsTabletSDUI::TransitionIn()
{
	if( !m_FinishedTransitionIn )
	{
		// draw normal
		Draw();

		renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

		// draw fade
		m_FadeTransition -= 15;
		if( m_FadeTransition <= 0 )
		{
			m_FadeTransition = 0;
			m_FinishedTransitionIn = true;
		}
		DrawFullscreenQuad( 0.0f, m_FadeTransition );

		renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

		return(1);
	}

	// transition finished, make sure to do a clean draw
	Draw();
	return(0);
}

/////////////////////////////////////////////////////
/// Method: TransitionOut
/// Params: None
///
/////////////////////////////////////////////////////
int IconsTabletSDUI::TransitionOut()
{
	if( !m_FinishedTransitionOut )
	{
		// draw normal
		Draw();

		renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

		// draw fade
		m_FadeTransition += 15;
		if( m_FadeTransition >= 255 )
		{
			m_FadeTransition = 255;
			m_FinishedTransitionOut = true;
		}
		DrawFullscreenQuad( 0.0f, m_FadeTransition );

		renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

		return(1);
	}
	return(0);
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void IconsTabletSDUI::Update( float deltaTime )
{
	bool customerAngry = false;

	m_LastDeltaTime = deltaTime;

	m_Elements.Update( TOUCH_SIZE_MENU, 2, deltaTime );

	m_Player = PhysicsWorld::GetPlayer();
	if( m_Player == 0 || m_Player->IsDead() )
		return;

	// disable all draws
	m_UIMesh->SetMeshDrawState( -1, false );
	m_Elements.ChangeElementDrawState( false );

	// store player pos
	math::Vec3 unprojPoint = m_Player->GetUnprojection();
	math::Vec3 m_PlayerPos = m_Player->GetPosition();
	m_CustomerList = m_Customers->GetCustomerList();
	m_SpawnAreaList =  m_Customers->GetSpawnAreas();

	// speech bubble
	if( m_Player->ShowPortNumber() )
	{
		math::Vec3 bubblePos( unprojPoint.X+m_GameData.SPEECHBUBBLE_XOFFSET_TABLET_SD, unprojPoint.Y+m_GameData.SPEECHBUBBLE_YOFFSET_TABLET_SD, 0.0f );
		m_UIMesh->SetMeshDrawState( SPEECH_BUBBLE, true );
		m_Elements.ChangeElementDrawState( HUI_ICON_TEXT_SPEECHBUBBLE, true );
		
		m_Elements.ChangeElementPosition( HUI_ICON_TEXT_SPEECHBUBBLE, bubblePos );
		m_Elements.ChangeElementText( HUI_ICON_TEXT_SPEECHBUBBLE, "%d %s", m_Player->PortRequested(), res::GetScriptString(63) );
	}
	else
	{	
		m_UIMesh->SetMeshDrawState( SPEECH_BUBBLE, false );
		m_Elements.ChangeElementDrawState( HUI_ICON_TEXT_SPEECHBUBBLE, false );
	}

	// low fuel, pump location
	if( m_Player->GetFuelCount() <= m_GameData.NEAR_LOW_FUEL )
	{
		math::Vec3 areaPos = math::Vec3( m_PortAreaList[m_Player->GetRefuelZoneIndex()].pos );
		math::Vec3 iconPos( 0.0f, 0.0f, 0.0f );

		const collision::Sphere sphere( areaPos, AREA_RADIUS );

		if( !renderer::OpenGL::GetInstance()->SphereInFrustum( sphere.vCenterPoint.X, sphere.vCenterPoint.Y, sphere.vCenterPoint.Z, sphere.fRadius ) )
		{
			// work out the normalised direction
			math::Vec3 destinationPos = math::Vec3(m_PortAreaList[m_Player->GetRefuelZoneIndex()].pos);

			// don't care about depth
			destinationPos.Z = 0.0f;

			math::Vec3 dir = destinationPos - m_PlayerPos;
			dir.normalise();

			iconPos.X = m_HalfWidth + (dir.X * m_HalfWidth);
			iconPos.Y = m_HalfHeight + (dir.Y * m_HalfHeight);

			iconPos = GetSafeIconPosition( iconPos, m_OriWidth, m_OriHeight );

			m_Elements.ChangeElementPosition( HUI_ICON_INDICATOR_FUEL, iconPos );
			m_Elements.ChangeElementDrawState( HUI_ICON_INDICATOR_FUEL, true );
			m_UIMesh->SetMeshDrawState( INDICATOR_FUEL, true );
		}
		else
		{
			m_UIMesh->SetMeshDrawState( INDICATOR_FUEL, false );
			m_Elements.ChangeElementDrawState( HUI_ICON_INDICATOR_FUEL, false );
		}
	}
	else
	{
		m_Elements.ChangeElementDrawState( HUI_ICON_INDICATOR_FUEL, false );
		m_UIMesh->SetMeshDrawState( INDICATOR_FUEL, false );
	}

	// hq return
	if( ProfileManager::GetInstance()->GetGameMode() == ProfileManager::CAREER_MODE &&
		m_Player->GetCustomersComplete() >= m_Player->GetLevelCustomerTotal() )
	{
		const float AREA_RADIUS = 8.0f;

		math::Vec3 areaPos = math::Vec3( m_PortAreaList[m_Player->GetHQZoneIndex()].pos );
		math::Vec3 iconPos( 0.0f, 0.0f, 0.0f );

		const collision::Sphere sphere( areaPos, AREA_RADIUS );

		if( !renderer::OpenGL::GetInstance()->SphereInFrustum( sphere.vCenterPoint.X, sphere.vCenterPoint.Y, sphere.vCenterPoint.Z, sphere.fRadius ) )
		{
			// work out the normalised direction
			math::Vec3 destinationPos = math::Vec3(m_PortAreaList[m_Player->GetHQZoneIndex()].pos);

			// don't care about depth
			destinationPos.Z = 0.0f;

			math::Vec3 dir = destinationPos - m_PlayerPos;
			dir.normalise();

			iconPos.X = m_HalfWidth + (dir.X * m_HalfWidth);
			iconPos.Y = m_HalfHeight + (dir.Y * m_HalfHeight);

			iconPos = GetSafeIconPosition( iconPos, m_OriWidth, m_OriHeight );

			m_Elements.ChangeElementPosition( HUI_ICON_INDICATOR_HOME, iconPos );
			m_Elements.ChangeElementDrawState( HUI_ICON_INDICATOR_HOME, true );
			m_UIMesh->SetMeshDrawState( INDICATOR_HOME, true );
		}
		else
		{
			m_UIMesh->SetMeshDrawState( INDICATOR_HOME, false );
			m_Elements.ChangeElementDrawState( HUI_ICON_INDICATOR_HOME, false );
		}
	}
	else
	{
		m_UIMesh->SetMeshDrawState( INDICATOR_HOME, false );
		m_Elements.ChangeElementDrawState( HUI_ICON_INDICATOR_HOME, false );
	}

	// CUSTOMERS
	std::vector<Customer*>::iterator it = m_CustomerList.begin();
	
	math::Vec3 customerPos( 0.0f, 0.0f, 0.0f );
	math::Vec3 destinationPos( 0.0f, 0.0f, 0.0f );
	math::Vec3 dir( 1.0f, 0.0f, 0.0f );
	math::Vec3 iconPos( 0.0f, 0.0f, 0.0f );

	m_ParcelIconIndex = 0;
	m_CustomerIconIndex = 0;

	while( it != m_CustomerList.end() )
	{
		if( (*it)->GetState() != Customer::CustomerState_InTaxi &&
			(*it)->GetState() != Customer::CustomerState_InActive && 
			(*it)->GetState() != Customer::CustomerState_DroppedOff &&
			(*it)->GetState() != Customer::CustomerState_WalkAway &&
			(*it)->GetState() != Customer::CustomerState_WalkAwayWithCargo )
		{
			// if not drawn, display 2D icon on edges
			const collision::Sphere& sphere = (*it)->GetBoundingSphere();
			if( !renderer::OpenGL::GetInstance()->SphereInFrustum( sphere.vCenterPoint.X, sphere.vCenterPoint.Y, sphere.vCenterPoint.Z, sphere.fRadius ) )
			{
				// work out the normalised direction
				customerPos = (*it)->GetPosition();

				// don't care about depth
				customerPos.Z = 0.0f;
				m_PlayerPos.Z = 0.0f;

				dir = customerPos - m_PlayerPos;
				dir.normalise();

				iconPos.X = m_HalfWidth + (dir.X * m_HalfWidth);
				iconPos.Y = m_HalfHeight + (dir.Y * m_HalfHeight);

				iconPos = GetSafeIconPosition( iconPos, m_OriWidth, m_OriHeight );

				// if a cargo holder
				if( (*it)->GetState() == Customer::CustomerState_WaitingWithCargo ||
					(*it)->GetState() == Customer::CustomerState_WaitingForCargo )
				{
					if( (*it)->GetState() == Customer::CustomerState_WaitingWithCargo )
					{
						if( !m_Player->HasCargo() )
						{
							if( m_Player->HasPassenger() )
								m_UIMesh->SetMeshDefaultColour( INDICATOR_PARCEL_1+m_CustomerIconIndex, inactiveIcon );
							else
								m_UIMesh->SetMeshDefaultColour( INDICATOR_PARCEL_1+m_CustomerIconIndex, activeIcon );
						}
						else
							m_UIMesh->SetMeshDefaultColour( INDICATOR_PARCEL_1+m_CustomerIconIndex, inactiveIcon );

						// draw cargo icon
						m_Elements.ChangeElementDrawState(HUI_ICON_INDICATOR_PARCEL1+m_ParcelIconIndex, true ); 
						m_Elements.ChangeElementPosition(HUI_ICON_INDICATOR_PARCEL1+m_ParcelIconIndex, iconPos ); 

						m_UIMesh->SetMeshDrawState( INDICATOR_PARCEL_1+m_ParcelIconIndex, true );
						m_UIMesh->EnableMeshDefaultColour(INDICATOR_PARCEL_1+m_ParcelIconIndex);
					}
/*					else if( (*it).GetState() == Customer::CustomerState_WaitingForCargo )
					{
						if( m_Player.HasCargo() )
						{
							if( m_Player.HasPassenger() )
								m_UIImage.SetActiveColour( math::Vec4Lite( 255, 255, 255, 64 ) );
							else
								m_UIImage.SetActiveColour( math::Vec4Lite( 255, 255, 255, 255 ) );
							
							// draw cargo icon
							if( (*it).GetSpawnNumeric() == m_Player.GetCargoDestination() )
								m_UIImage.Draw();
						}
					}
*/
				}
				else
				{
					// number is showing, alpha out this icon
					if( m_Player->HasPassenger() || m_Player->HasCargo() )
						m_UIMesh->SetMeshDefaultColour( INDICATOR_CUSTOMER_1+m_CustomerIconIndex, inactiveIcon );
					else
						m_UIMesh->SetMeshDefaultColour( INDICATOR_CUSTOMER_1+m_CustomerIconIndex, activeIcon );
				
					m_Elements.ChangeElementDrawState(HUI_ICON_INDICATOR_CUSTOMER1+m_CustomerIconIndex, true ); 
					m_Elements.ChangeElementPosition(HUI_ICON_INDICATOR_CUSTOMER1+m_CustomerIconIndex, iconPos ); 

					m_UIMesh->SetMeshDrawState( INDICATOR_CUSTOMER_1+m_CustomerIconIndex, true );
					m_UIMesh->EnableMeshDefaultColour(INDICATOR_CUSTOMER_1+m_CustomerIconIndex);
					
					// assume a normal customer icon
				}
			}
			else
			{
				// draw angry speech bubble
				if( (*it)->IsAngry() )
				{
					if( !m_Player->ShowPortNumber() )
					{
						math::Vec2 point = (*it)->GetUnprojection();

						
						math::Vec3 bubblePos( point.X+m_GameData.CUSTOMER_SWEAR_BUBBLE_OFFSETX_TABLET_SD, point.Y+m_GameData.CUSTOMER_SWEAR_BUBBLE_OFFSETY_TABLET_SD, 0.0f );
						m_UIMesh->SetMeshDrawState( SPEECH_BUBBLE, true );
						m_Elements.ChangeElementDrawState( HUI_ICON_TEXT_SPEECHBUBBLE, true );
		
						m_Elements.ChangeElementPosition( HUI_ICON_TEXT_SPEECHBUBBLE, bubblePos );
						m_Elements.ChangeElementText( HUI_ICON_TEXT_SPEECHBUBBLE, "%s", res::GetScriptString(8000) );

						customerAngry = true;
					}
					
				}
				else
				{
					if( !m_Player->ShowPortNumber() &&
						!customerAngry )
					{
						m_UIMesh->SetMeshDrawState( SPEECH_BUBBLE, false );
						m_Elements.ChangeElementDrawState( HUI_ICON_TEXT_SPEECHBUBBLE, false );
					}
				}
			}
		}

		m_ParcelIconIndex++;
		m_CustomerIconIndex++;

		// next
		it++;
	}

	// ensure numbers are drawn above customer icons
	it = m_CustomerList.begin();

	while( it != m_CustomerList.end() )
	{
		if( (*it)->GetState() == Customer::CustomerState_InTaxi )
		{	
			// show destination ?
			int destZone = (*it)->GetDestinationZone();
			int destNum = (*it)->GetDestinationNumeric();
			math::Vec3 areaPos = math::Vec3( m_SpawnAreaList[destZone].boundArea.pos );

			const collision::Sphere sphere( areaPos, AREA_RADIUS );

			if( !renderer::OpenGL::GetInstance()->SphereInFrustum( sphere.vCenterPoint.X, sphere.vCenterPoint.Y, sphere.vCenterPoint.Z, sphere.fRadius ) )
			{
				// work out the normalised direction
				destinationPos = m_SpawnAreaList[destZone].boundArea.pos;

				// don't care about depth
				destinationPos.Z = 0.0f;
				m_PlayerPos.Z = 0.0f;

				dir = destinationPos - m_PlayerPos;
				dir.normalise();

				iconPos.X = m_HalfWidth + (dir.X * m_HalfWidth);
				iconPos.Y = m_HalfHeight + (dir.Y * m_HalfHeight);

				iconPos = GetSafeIconPosition( iconPos, m_OriWidth, m_OriHeight );

				m_Elements.ChangeElementDrawState(HUI_ICON_TEXT_PORT, true ); 
				m_Elements.ChangeElementPosition(HUI_ICON_TEXT_PORT, iconPos ); 
				m_Elements.ChangeElementText(HUI_ICON_TEXT_PORT, "%d", destNum ); 

				m_UIMesh->SetMeshDrawState( INDICATOR_PORT, true );

			}
		}
		else if( (*it)->GetState() == Customer::CustomerState_WaitingForCargo &&
				m_Player->HasCargo() &&
				(*it)->GetSpawnNumeric() == m_Player->GetCargoDestination() )
		{
			// show destination ?
			int destZone = (*it)->GetSpawnZone();
			int destNum = (*it)->GetSpawnNumeric();
			math::Vec3 areaPos = math::Vec3( m_SpawnAreaList[destZone].boundArea.pos );

			const collision::Sphere sphere( areaPos, AREA_RADIUS );

			if( !renderer::OpenGL::GetInstance()->SphereInFrustum( sphere.vCenterPoint.X, sphere.vCenterPoint.Y, sphere.vCenterPoint.Z, sphere.fRadius ) )
			{
				// work out the normalised direction
				destinationPos = m_SpawnAreaList[destZone].boundArea.pos;

				// don't care about depth
				destinationPos.Z = 0.0f;
				m_PlayerPos.Z = 0.0f;

				dir = destinationPos - m_PlayerPos;
				dir.normalise();

				iconPos.X = m_HalfWidth + (dir.X * m_HalfWidth);
				iconPos.Y = m_HalfHeight + (dir.Y * m_HalfHeight);

				iconPos = GetSafeIconPosition( iconPos, m_OriWidth, m_OriHeight );

				m_Elements.ChangeElementDrawState(HUI_ICON_TEXT_PORT, true ); 
				m_Elements.ChangeElementPosition(HUI_ICON_TEXT_PORT, iconPos ); 
				m_Elements.ChangeElementText(HUI_ICON_TEXT_PORT, "%d", destNum ); 

				m_UIMesh->SetMeshDrawState( INDICATOR_PORT, true );
			}
		}

		// next
		it++;
	}
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void IconsTabletSDUI::Draw()
{
	m_Player = PhysicsWorld::GetPlayer();
	if( m_Player == 0 || m_Player->IsDead() )
		return;

	renderer::OpenGL::GetInstance()->SetNearFarClip( NEAR_CLIP_ORTHO, FAR_CLIP_ORTHO );
	renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );
	renderer::OpenGL::GetInstance()->DisableLighting();

	renderer::OpenGL::GetInstance()->SetColour4ub( 255, 255, 255, 255 );

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_ALWAYS );
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	if( m_UIMesh != 0 )
		m_UIMesh->Draw();

	m_Elements.Draw();

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_LESS );
	renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}

/////////////////////////////////////////////////////
/// Method: GetSafeIconPosition
/// Params: [in]iconPos, [in]oriWidth, [in]oriHeight
///
/////////////////////////////////////////////////////
math::Vec3& IconsTabletSDUI::GetSafeIconPosition( math::Vec3& iconPos, float oriWidth, float oriHeight )
{
	if( iconPos.X <= m_GameData.ICON_EDGE_CLOSEST )
		iconPos.X = m_GameData.ICON_EDGE_CLOSEST ;
	else if( iconPos.X >= (m_OriWidth-m_GameData.ICON_EDGE_CLOSEST) )
		iconPos.X = (m_OriWidth-m_GameData.ICON_EDGE_CLOSEST);

	// make sure not to hit the UI
	if( iconPos.Y <= m_GameData.ICON_LOWEST_POS ) // lower UI
		iconPos.Y = (m_GameData.ICON_LOWEST_POS );
	else if( iconPos.Y >= m_OriHeight-m_GameData.ICON_HIGHEST_POS ) // upper UI
		iconPos.Y = (m_OriHeight-m_GameData.ICON_HIGHEST_POS);

	return iconPos;
}
