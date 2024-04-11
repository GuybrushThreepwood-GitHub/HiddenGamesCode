
/*===================================================================
	File: CustomerManagement.cpp
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"

#include "MathBase.h"
#include "RenderBase.h"

#include "MathBase.h"
#include "RenderBase.h"
#include "SoundBase.h"
#include "CollisionBase.h"
#include "Model/ModelCommon.h"
#include "ModelBase.h"
#include "InputBase.h"
#include "ScriptBase.h"

#include "Resources/EmitterResources.h"

#include "CabbyConsts.h"
#include "Cabby.h"

#include "Audio/CustomerAudio.h"

#include "ScriptAccess/ScriptDataHolder.h"
#include "ScriptAccess/ScriptAccess.h"
#include "Level/Level.h"
#include "Player/Player.h"

#include "Customers/CustomerAnimationLookup.h"
#include "Customers/CustomerManagement.h"

#include "Profiles/ProfileManager.h"

namespace
{
	const int TOTAL_PITCH_TWEAKS = 5;

	static float femalePitchTweaks[TOTAL_PITCH_TWEAKS] = 
	{
		0.55f, 0.65f, 0.75f, 0.85f, 0.95f
	};

	static float malePitchTweaks[TOTAL_PITCH_TWEAKS] = 
	{
		0.55f, 0.65f, 0.75f, 0.85f, 0.95f
	};
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
CustomerManagement::CustomerManagement( Player& player, Level& taxiLevel  )
	:	m_Player( player )
	,	m_TaxiLevel( taxiLevel )
{
	int i=0;

	m_DevData = GetScriptDataHolder()->GetDevData();
	m_GameData = GetScriptDataHolder()->GetGameData();

	for( i=0; i < FREE_SLOTS; ++i )
		m_FreeSlots[i] = -1;

	for( i=0; i < MAX_CUSTOMER_EMITTERS; ++i )
	{
		const res::EmitterResourceStore* er = res::GetEmitterResource(300);
		DBG_ASSERT( er != 0 );
		
		m_Emitters[i] = new efx::Emitter;
		DBG_ASSERT( m_Emitters[i] != 0 );

		m_Emitters[i]->Create( *er->block );
		res::SetupTexturesOnEmitter( m_Emitters[i] );
	}

	m_EmitterIndex = 0;

	m_ActiveCustomers = 0;
	m_MaxActiveCustomers = m_TaxiLevel.GetCustomerMaxSpawn();

	m_MaxSpawnTime = m_GameData.CUSTOMER_SPAWN_TIME;
	m_LastDeltaTime = 0.0f;
	m_LastSpawnTime = m_MaxSpawnTime*0.5f;

	m_pModelData = 0;
	m_pModelData = new mdl::ModelHGA;
	DBG_ASSERT( m_pModelData != 0 );

	if( m_DevData.hiresCustomers )
		m_pModelData->LoadBindPose( "assets/models/customers/bind_high.hga" );
	else
		m_pModelData->LoadBindPose( "assets/models/customers/bind_low.hga" );

	for( i=0; i < CustomerAnim::NUM_ANIMS; ++i )
	{
		CustomerAnim::AnimData animData = GetCustomerAnimationData(i);
		m_pModelData->LoadAnimation( animData.szFile, animData.animId, animData.loopFlag );
	}

	// no culling as they are moving models
	m_pModelData->SetModelGeneralFlags( 0 );

	// find out how many landing zones there are in the level
	std::vector<Level::PortArea> areaList = taxiLevel.GetPortAreaList();
	
	std::vector<Level::PortArea>::iterator it = areaList.begin();
	int index = 0;
	while( it != areaList.end() )
	{
		CustomerSpawnArea spawnArea;

		spawnArea.boundArea = (*it);

		spawnArea.inUseCounter = 0;
		//spawnArea.lastTime = 0.0f;
		spawnArea.levelAreaIndex = index;

		// move the area in a bit so customers spawn in safe ranges
		if( spawnArea.boundArea.type == Level::ePortType_LandingZone )
		{
			spawnArea.boundArea.dim.Width *= 0.95f;

			// shift up for shadow to work
			spawnArea.boundArea.pos.Y += 1.3f;
		}

		m_SpawnAreaList.push_back(spawnArea);

		index++;
		it++;
	}

	// audio files
	m_MoneyBigTipBufferId = snd::INVALID_SOUNDBUFFER;
	m_MoneyBigTipBufferId = AudioSystem::GetInstance()->AddAudioFile( 104/*"money.wav"*/ );

	m_DoorOpenId = AudioSystem::GetInstance()->AddAudioFile( 156/*"door_openclose.wav"*/ );
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
CustomerManagement::~CustomerManagement()
{
	int i=0;
	unsigned int j=0;
	if( m_pModelData != 0 )
	{
		delete m_pModelData;
		m_pModelData = 0;
	}

	for( i=0; i < MAX_CUSTOMER_EMITTERS; ++i )
	{
		if( m_Emitters[i] != 0 )
		{
			delete m_Emitters[i];
			m_Emitters[i] = 0;
		}
	}

	for( j=0; j < m_CustomerList.size(); ++j )
	{
		delete m_CustomerList[j];
		m_CustomerList[j] = 0;
	}	

	m_CustomerList.clear();
	m_SpawnAreaList.clear();
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void CustomerManagement::Draw( bool pauseFlag )
{
	int i=0;
	std::vector<Customer*>::iterator it = m_CustomerList.begin();

	renderer::OpenGL::GetInstance()->SetColour4ub( 235, 235, 235, 255 );
	while( it != m_CustomerList.end() )
	{
		// do not draw or animate when outside the frustun
		const collision::Sphere& sphere = (*it)->GetBoundingSphere();
		if( renderer::OpenGL::GetInstance()->SphereInFrustum( sphere.vCenterPoint.X, sphere.vCenterPoint.Y, sphere.vCenterPoint.Z, sphere.fRadius ) )
		{
			// set subemshes to only update the required ones
			(*it)->SetupSubmeshes();

			// update skins
			(*it)->UpdateSkins();

			// normal update
			(*it)->Update( m_LastDeltaTime );

			(*it)->Draw(pauseFlag);
		}

		// next
		it++;
	}

	for( i=0; i < MAX_CUSTOMER_EMITTERS; ++i )
	{
		if( m_Emitters[i] != 0 )
			m_Emitters[i]->Draw();
	}
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void CustomerManagement::Update( float deltaTime )
{
	m_LastDeltaTime = deltaTime;
	int i=0;

	std::vector<Customer*>::iterator customerIt = m_CustomerList.begin();

	while( customerIt != m_CustomerList.end() )
	{
		// always do this
		(*customerIt)->UpdateTimer( deltaTime );
		(*customerIt)->UpdateFade( deltaTime );

		// customer is inactive, remove from the list
		if( (*customerIt)->GetState() == Customer::CustomerState_InActive )
		{
			delete (*customerIt);
			customerIt = m_CustomerList.erase( customerIt );

			m_ActiveCustomers--;

			// check for last customer on last level of career and do awards now
			if( m_Player.GetCustomersComplete() >= m_Player.GetLevelCustomerTotal() )
			{
				if( ProfileManager::GetInstance()->GetGameMode() == ProfileManager::CAREER_MODE )
				{
					// last level?
					if( ProfileManager::GetInstance()->GetProfile()->currentLevelIndex+1 >= LEVELS_IN_A_CAREER )
					{
						// models are 1-9
						int modelIdx = m_Player.GetVehicleModelIndex();

						// achievements are 9-18
						if( modelIdx >= 1 && modelIdx <= 9 )
							GameSystems::GetInstance()->AwardAchievement(9+(modelIdx-1));
						else // achievements are 46-50
							GameSystems::GetInstance()->AwardAchievement(46+((modelIdx-1)-10) );
					}
				}
			}

			if( customerIt == m_CustomerList.end() )
				break;
		}

		if( (*customerIt)->IsCargoCustomer() )
		{
			UpdateCargo( *(*customerIt), deltaTime );
		}
		else
		{
			UpdateCustomer( *(*customerIt), deltaTime );
		}
		
		(*customerIt)->PreDrawSetup();

		// next customer
		customerIt++;
	}

	for( i=0; i < MAX_CUSTOMER_EMITTERS; ++i )
	{
		if( m_Emitters[i] != 0 )
			m_Emitters[i]->Update(deltaTime);
	}

	if( m_Player.IsDead() || m_Player.HasSurvivedFuelRunout() )
		return;

	// update spawn timer
	m_LastSpawnTime += deltaTime;

	if( m_LastSpawnTime >= m_MaxSpawnTime )
		CheckForCustomerSpawn();

	// check for player landing
	if( !m_Player.IsDead() &&
		m_Player.GetFuelCount() > 0 )
		CheckForPlayerLand( deltaTime );
}

/////////////////////////////////////////////////////
/// Method: UpdateCargo
/// Params: [in]customer
///
/////////////////////////////////////////////////////
void CustomerManagement::UpdateCargo( Customer& customer, float deltaTime )
{
	if( customer.GetState() == Customer::CustomerState_WalkAway ||
		customer.GetState() == Customer::CustomerState_WalkAwayWithCargo )
	{
		// walking to a drop off point
		math::Vec3 pos = customer.GetPosition();
		if( pos.X < customer.GetDropOffWalkPoint().X )
		{
			pos.X += m_GameData.CUSTOMER_WALK_SPEED*deltaTime;
			customer.SetTargetRotation(90.0f);
		}
		else if( pos.X > customer.GetDropOffWalkPoint().X )
		{
			pos.X -= m_GameData.CUSTOMER_WALK_SPEED*deltaTime;
			customer.SetTargetRotation(-90.0f);
		}
			
		// walk anim
		if( customer.GetState() == Customer::CustomerState_WalkAway )
			customer.SetAnimation( CustomerAnim::WALK, 0, true );
		else
			customer.SetAnimation( CustomerAnim::WALK_WITHPARCEL, 0, true );

		// arrived at drop off point
		if( std::fabs(pos.X - customer.GetDropOffWalkPoint().X) < m_GameData.CUSTOMER_CLOSE_TO_TAXI )
		{
			customer.SetState( Customer::CustomerState_InActive );

			PlayEmitter( customer.GetBoundingSphere().vCenterPoint );

			// clear out spawn locations as free
			m_SpawnAreaList[customer.GetSpawnZone()].inUseCounter--;
		}

		customer.SetPosition(pos);
	}
	else if( customer.GetState() == Customer::CustomerState_WalkToTaxiWithCargo ||
			customer.GetState() == Customer::CustomerState_WalkToTaxiForCargo )
	{
		if( !m_Player.IsInContactWithWorld() )
		{
			if( customer.GetState() == Customer::CustomerState_WalkToTaxiWithCargo )
			{
				customer.SetState( Customer::CustomerState_WaitingWithCargo );
				customer.SetIdleAnim();
			}
			else
			{
				customer.SetState( Customer::CustomerState_WaitingForCargo );
				customer.SetIdleAnim();
			}

			customer.SetTargetRotation(0.0f);
			
			// player had landed for a while, then took off, get angry
			if( m_Player.GetLandedTime() >= m_GameData.LAND_TIME_ANGRY )
				customer.SetAngry();
		}
	}
	else if( customer.GetState() == Customer::CustomerState_WaitingWithCargo ||
		customer.GetState() == Customer::CustomerState_WaitingForCargo )
	{
		// player had the cargo and crashed
		if( m_Player.IsDead() &&
			customer.GetState() == Customer::CustomerState_WaitingForCargo &&
			m_Player.HasCargo() )
		{
			m_Player.RemoveCargo();

			// now walk away
			customer.SetState( Customer::CustomerState_WalkAway );

			// find a random spot to walk to
			int zoneIndex = customer.GetSpawnZone();
			float customerX = (m_SpawnAreaList[zoneIndex].boundArea.pos.X+(m_SpawnAreaList[zoneIndex].boundArea.dim.Width*0.5f));
			math::Vec3 pos( customerX, (m_SpawnAreaList[zoneIndex].boundArea.pos.Y - m_SpawnAreaList[zoneIndex].boundArea.dim.Height*0.5f), 0.0f );
				
			customer.SetDropOffWalkPoint( pos );

			m_Player.AddMoney( m_GameData.PARCEL_LOSS, true );
		}
		else
		if( m_Player.GetFuelCount() <= 0 &&
			customer.GetState() == Customer::CustomerState_WaitingForCargo &&
			m_Player.HasCargo() )
		{
			m_Player.RemoveCargo();

			// now walk away
			customer.SetState( Customer::CustomerState_WalkAway );

			// find a random spot to walk to
			int zoneIndex = customer.GetSpawnZone();
			float customerX = (m_SpawnAreaList[zoneIndex].boundArea.pos.X+(m_SpawnAreaList[zoneIndex].boundArea.dim.Width*0.5f));
			math::Vec3 pos( customerX, (m_SpawnAreaList[zoneIndex].boundArea.pos.Y - m_SpawnAreaList[zoneIndex].boundArea.dim.Height*0.5f), 0.0f );
				
			customer.SetDropOffWalkPoint( pos );

			//m_Player.AddMoney( m_GameData.PARCEL_LOSS, true );
		}
		else
		if( !m_Player.IsInContactWithWorld() )
		{
			customer.SetTargetRotation(0.0f);
			if( customer.GetState() == Customer::CustomerState_WaitingWithCargo )
			{
				customer.SetIdleAnim();
			}
			else
			{
				customer.SetIdleAnim();
			}
		}
	}

}

/////////////////////////////////////////////////////
/// Method: UpdateCustomer
/// Params: [in]customer
///
/////////////////////////////////////////////////////
void CustomerManagement::UpdateCustomer( Customer& customer, float deltaTime )
{
	if( customer.GetState() == Customer::CustomerState_DroppedOff )
	{
		// walking to a drop off point
		math::Vec3 pos = customer.GetPosition();
		if( pos.X < customer.GetDropOffWalkPoint().X )
		{
			pos.X += m_GameData.CUSTOMER_WALK_SPEED*deltaTime;
			customer.SetTargetRotation(90.0f);
		}
		else if( pos.X > customer.GetDropOffWalkPoint().X )
		{
			pos.X -= m_GameData.CUSTOMER_WALK_SPEED*deltaTime;
			customer.SetTargetRotation(-90.0f);
		}
			
		// walk anim
		customer.SetAnimation( CustomerAnim::WALK, 0, true );

		// arrived at drop off point
		if( std::fabs(pos.X - customer.GetDropOffWalkPoint().X) < m_GameData.CUSTOMER_CLOSE_TO_TAXI )
		{
			customer.SetState( Customer::CustomerState_InActive );

			PlayEmitter( customer.GetBoundingSphere().vCenterPoint );
		}

		customer.SetPosition(pos);
	}
	else if( customer.GetState() == Customer::CustomerState_WalkToTaxi )
	{
		if( !m_Player.IsInContactWithWorld() )
		{
			customer.SetState( Customer::CustomerState_Waiting );
			customer.SetTargetRotation(0.0f);

			customer.SetIdleAnim();

			// player had landed for a while, then took off, get angry
			if( m_Player.GetLandedTime() >= m_GameData.LAND_TIME_ANGRY )
				customer.SetAngry();
		}
	}
	else if( customer.GetState() == Customer::CustomerState_Waiting )
	{
		if( !m_Player.IsInContactWithWorld() )
		{
			customer.SetTargetRotation(0.0f);
			customer.SetIdleAnim();
		}
	}
	else if( customer.GetState() == Customer::CustomerState_InTaxi )
	{
		// customer in taxi when player crashed
		if( m_Player.IsDead() )
		{
			m_Player.RemovePassenger();
			customer.SetState( Customer::CustomerState_InActive );
			m_Player.AddMoney( m_GameData.PASSENGER_LOSS, true );
		}
		else if( m_Player.GetFuelCount() <= 0 )
		{
			m_Player.RemovePassenger();
			customer.SetState( Customer::CustomerState_InActive );
			//m_Player.AddMoney( m_GameData.PASSENGER_LOSS, true );
		}
	}
}

/////////////////////////////////////////////////////
/// Method: CheckForCustomerSpawn
/// Params: None
///
/////////////////////////////////////////////////////
void CustomerManagement::CheckForCustomerSpawn()
{
	// are there enough customers to complete the level already active or
	// is there too many customers, no need to spawn
	int maxCustomers = 100;
	if( ProfileManager::GetInstance()->GetGameMode() == ProfileManager::ARCADE_MODE )
		maxCustomers = 100;
	else
		maxCustomers = m_Player.GetLevelCustomerTotal();

	// stop spawning
	if( (m_Player.GetCustomersComplete()+m_ActiveCustomers) >= maxCustomers ||
		(m_ActiveCustomers >= m_MaxActiveCustomers) )
	{
		m_LastSpawnTime = 0.0f;
		return;
	}

	// find a free landing zone
	std::vector<CustomerSpawnArea>::iterator it = m_SpawnAreaList.begin();
	int index = 0;
	int i = 0;
	bool freeSpawn = false;

	int lastFreeSlotIndex = 0;

	while( it != m_SpawnAreaList.end() )
	{
		// does spawn area look free
		if( (*it).inUseCounter == 0 &&
			(*it).boundArea.type == Level::ePortType_LandingZone )
		{
			// player is not landed on this spot
			if( (m_Player.GetLandingZoneIndex() != index) )
			{
				freeSpawn = true;

				m_FreeSlots[lastFreeSlotIndex] = index;
				lastFreeSlotIndex++;

				if( lastFreeSlotIndex >= FREE_SLOTS )
					break;
			}
		}			
		
		index++;
		it++;
	}

	// there is no free slot
	if( !freeSpawn )
	{
		m_LastSpawnTime = 0.0f;
		return;
	}
	else
	{
		// possibility for cargo
		bool doCargo = false;

		if( lastFreeSlotIndex >= 2 )
		{
			// 10% probability for a cargo mission
			if( math::RandomNumber(1,10) >= 10 )
			{
				doCargo = CreateCargo( lastFreeSlotIndex );
			}
		}

		// normal customer mission
		if( !doCargo )
		{
			CreateCustomer( lastFreeSlotIndex );
		}
	}

	// reset free slots
	for( i=0; i < FREE_SLOTS; ++i )
		m_FreeSlots[i] = -1;
}

/////////////////////////////////////////////////////
/// Method: CreateCargo
/// Params: [in]freeSlotCount
///
/////////////////////////////////////////////////////
bool CustomerManagement::CreateCargo( int freeSlotCount )
{
	int i = 0;
	Customer* newCustomer = 0;
	math::Vec3 pos;
	float randomX = 0.0f;

	// pick a rough random slot
	int srcIndex = m_FreeSlots[math::RandomNumber( 0, freeSlotCount-1 )];

	if( srcIndex == -1 )
	{
		// check again 1 second from now
		m_LastSpawnTime = m_MaxSpawnTime*0.5f;

		// reset free slots
		for( i=0; i < FREE_SLOTS; ++i )
			m_FreeSlots[i] = -1;

		return(false);
	}

	// set here to make sure the destination isn't the same
	m_SpawnAreaList[srcIndex].inUseCounter++;
	//m_SpawnAreaList[srcIndex].lastTime = 0.0f;

	int destIndex = FindDestinationForCargo( srcIndex );

	// there's no free destination
	if( destIndex == -1 )
	{
		// check again 1 second from now
		m_LastSpawnTime = m_MaxSpawnTime*0.5f;

		// clear out source
		m_SpawnAreaList[srcIndex].inUseCounter--;

		// reset free slots
		for( i=0; i < FREE_SLOTS; ++i )
			m_FreeSlots[i] = -1;

		return(false);
	}

	// place pickup customer here
	m_SpawnAreaList[destIndex].inUseCounter++;

	// spawn a customer on the area
	newCustomer = new Customer;
	DBG_ASSERT( newCustomer != 0 );

	newCustomer->SetupCustomer( static_cast<Customer::CustomerType>( math::RandomNumber( static_cast<int>(Customer::CustomerType_Male), static_cast<int>(Customer::CustomerType_Female) ) ), true, m_pModelData );

	// get a random spawn position in the area
	randomX = math::RandomNumber( (m_SpawnAreaList[srcIndex].boundArea.pos.X-(m_SpawnAreaList[srcIndex].boundArea.dim.Width*0.5f)), (m_SpawnAreaList[srcIndex].boundArea.pos.X+(m_SpawnAreaList[srcIndex].boundArea.dim.Width*0.5f)) );
	pos = math::Vec3( randomX, (m_SpawnAreaList[srcIndex].boundArea.pos.Y - m_SpawnAreaList[srcIndex].boundArea.dim.Height*0.5f), m_GameData.CUSTOMER_DEPTH );
						
	// create
	newCustomer->SetPosition( pos );
	newCustomer->SetSpawnZone( srcIndex );
	newCustomer->SetSpawnNumeric( m_SpawnAreaList[srcIndex].boundArea.numeric );
	newCustomer->SetState( Customer::CustomerState_WaitingWithCargo );
	newCustomer->SetIdleAnim();
	newCustomer->SetCargoCustomerFlag();

	newCustomer->SetDestinationZone( destIndex );
	newCustomer->SetDestinationNumeric( m_SpawnAreaList[destIndex].boundArea.numeric );

	// fire a call
	m_OneShotSFX.nSourceID = snd::SoundManager::GetInstance()->GetFreeSoundSource();

	if( m_OneShotSFX.nSourceID != snd::INVALID_SOUNDSOURCE )
	{
		// get a relative position
		math::Vec3 pos( newCustomer->GetPosition() - m_Player.GetPosition() );
		pos.normalise();

		// tweak the pitch for a bit of randomness
		int val = math::RandomNumber( 0, TOTAL_PITCH_TWEAKS );

		if( newCustomer->GetGender() == Customer::CUSTOMER_MALE )
			newCustomer->SetVoicePitchTweak( malePitchTweaks[val] );
		else
			newCustomer->SetVoicePitchTweak( femalePitchTweaks[val] );

		PlayEmitter( newCustomer->GetBoundingSphere().vCenterPoint );
		PlayCustomerHailAudio( *newCustomer );
	}	

	// add new customer
	m_CustomerList.push_back(newCustomer);
				
	// create the destination customer

	newCustomer = new Customer;
	DBG_ASSERT( newCustomer != 0 );

	newCustomer->SetupCustomer( static_cast<Customer::CustomerType>( math::RandomNumber( static_cast<int>(Customer::CustomerType_Male), static_cast<int>(Customer::CustomerType_Female) ) ), false, m_pModelData );

	// get a random spawn position in the area
	randomX = math::RandomNumber( (m_SpawnAreaList[destIndex].boundArea.pos.X-(m_SpawnAreaList[destIndex].boundArea.dim.Width*0.5f)), (m_SpawnAreaList[destIndex].boundArea.pos.X+(m_SpawnAreaList[destIndex].boundArea.dim.Width*0.5f)) );
	pos = math::Vec3( randomX, (m_SpawnAreaList[destIndex].boundArea.pos.Y - m_SpawnAreaList[destIndex].boundArea.dim.Height*0.5f), m_GameData.CUSTOMER_DEPTH );
						
	// create
	newCustomer->SetPosition( pos );
	newCustomer->SetSpawnZone( destIndex );
	newCustomer->SetSpawnNumeric( m_SpawnAreaList[destIndex].boundArea.numeric );
	newCustomer->SetState( Customer::CustomerState_WaitingForCargo );
	newCustomer->SetIdleAnim();
	newCustomer->SetCargoCustomerFlag();
	newCustomer->SetDrawState( false );

	// add new customer
	m_CustomerList.push_back(newCustomer);

	// reset spawn timer
	m_LastSpawnTime = 0.0f;	

	m_ActiveCustomers+=2;

	return(true);
}

/////////////////////////////////////////////////////
/// Method: CreateCustomer
/// Params: [in]freeSlotCount
///
/////////////////////////////////////////////////////
bool CustomerManagement::CreateCustomer( int freeSlotCount )
{
	// spawn a customer on the area
	Customer* newCustomer = 0;
	float randomX = 0.0f;
	int i = 0;
	math::Vec3 pos;

	// pick a rough random slot
	int srcIndex = m_FreeSlots[math::RandomNumber( 0, freeSlotCount-1 )];
			
	if( srcIndex == -1 )
	{
		// check again 1 second from now
		m_LastSpawnTime = m_MaxSpawnTime*0.5f;

		// reset free slots
		for( i=0; i < FREE_SLOTS; ++i )
			m_FreeSlots[i] = -1;

		return(false);
	}

	// set here to make sure the destination isn't the same
	m_SpawnAreaList[srcIndex].inUseCounter++;

	int destIndex = FindDestinationForCustomer( srcIndex );

	// there's no free destination
	if( destIndex == -1 )
	{
		// check again 1 second from now
		m_LastSpawnTime = m_MaxSpawnTime*0.5f;

		// clear out source
		m_SpawnAreaList[srcIndex].inUseCounter--;

		// reset free slots
		for( i=0; i < FREE_SLOTS; ++i )
			m_FreeSlots[i] = -1;

		return(false);
	}

	newCustomer = new Customer;
	DBG_ASSERT( newCustomer != 0 );
		
	newCustomer->SetupCustomer( static_cast<Customer::CustomerType>( math::RandomNumber( static_cast<int>(Customer::CustomerType_Male), static_cast<int>(Customer::CustomerType_Female) ) ), false, m_pModelData );

	// get a random spawn position in the area
	randomX = math::RandomNumber( (m_SpawnAreaList[srcIndex].boundArea.pos.X-(m_SpawnAreaList[srcIndex].boundArea.dim.Width*0.5f)), (m_SpawnAreaList[srcIndex].boundArea.pos.X+(m_SpawnAreaList[srcIndex].boundArea.dim.Width*0.5f)) );
	pos = math::Vec3( randomX, (m_SpawnAreaList[srcIndex].boundArea.pos.Y - m_SpawnAreaList[srcIndex].boundArea.dim.Height*0.5f), m_GameData.CUSTOMER_DEPTH );
		
	// create
	newCustomer->SetPosition( pos );
	newCustomer->SetSpawnZone( srcIndex );
	newCustomer->SetSpawnNumeric( m_SpawnAreaList[srcIndex].boundArea.numeric );
	newCustomer->SetState( Customer::CustomerState_Waiting );
	newCustomer->SetIdleAnim();
	newCustomer->SetDestinationZone( destIndex );
	newCustomer->SetDestinationNumeric( m_SpawnAreaList[destIndex].boundArea.numeric );

	// reset spawn timer
	m_LastSpawnTime = 0.0f;

	// fire a call
	m_OneShotSFX.nSourceID = snd::SoundManager::GetInstance()->GetFreeSoundSource();

	if( m_OneShotSFX.nSourceID != snd::INVALID_SOUNDSOURCE )
	{
		// get a relative position
		math::Vec3 pos( newCustomer->GetPosition() - m_Player.GetPosition() );
		pos.normalise();

		// tweak the pitch for a bit of randomness
		int val = math::RandomNumber( 0, 4 );
		if( newCustomer->GetGender() == Customer::CUSTOMER_MALE )
			newCustomer->SetVoicePitchTweak( malePitchTweaks[val] );
		else
			newCustomer->SetVoicePitchTweak( femalePitchTweaks[val] );

		PlayEmitter( newCustomer->GetBoundingSphere().vCenterPoint );
		PlayCustomerHailAudio( *newCustomer );
	}		

	// add new customer
	m_CustomerList.push_back(newCustomer);

	m_ActiveCustomers++;

	return(true);
}

/////////////////////////////////////////////////////
/// Method: CheckForPlayerLand
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void CustomerManagement::CheckForPlayerLand( float deltaTime )
{
	// see if this is the same landing zone with a customer
	std::vector<Customer*>::iterator it = m_CustomerList.begin();
	
	while( it != m_CustomerList.end() )
	{
		if( (*it)->IsCargoCustomer() )
			CheckForPlayerLandCargo( *(*it), deltaTime );
		else
			CheckForPlayerLandCustomer( *(*it), deltaTime );

		// next customer
		it++;
	}
}

/////////////////////////////////////////////////////
/// Method: CheckForPlayerLandCargo
/// Params: [in]customer, [in]deltaTime
///
/////////////////////////////////////////////////////
void CustomerManagement::CheckForPlayerLandCargo( Customer& customer, float deltaTime )
{
	float playerAngle = math::RadToDeg( m_Player.GetBody()->GetAngle() );

	if( m_Player.GetFuelCount() <= 0 )
		return;

	if( (m_Player.IsInContactWithWorld() && m_Player.IsInLandingZone()) &&
			PhysicsWorld::ContactNormal.y >= 0.9f &&
			(playerAngle > -0.1f && playerAngle < 0.1f ) )
	{
		// customer waiting for cargo pickup
		if( (customer.GetState() == Customer::CustomerState_WaitingWithCargo || customer.GetState() == Customer::CustomerState_WalkToTaxiWithCargo ) &&
			m_Player.GetLandingZoneIndex() == customer.GetSpawnZone() &&
			m_Player.HasPassenger() == false &&
			m_Player.HasCargo() == false )
		{
			// walk to taxi
			customer.SetState( Customer::CustomerState_WalkToTaxiWithCargo );
				
			math::Vec3 pos = customer.GetPosition();
			if( pos.X < m_Player.GetPosition().X )
			{
				pos.X += m_GameData.CUSTOMER_WALK_SPEED*deltaTime;
				customer.SetTargetRotation(90.0f);
			}
			else if( pos.X > m_Player.GetPosition().X )
			{
				pos.X -= m_GameData.CUSTOMER_WALK_SPEED*deltaTime;
				customer.SetTargetRotation(-90.0f);
			}
			
			customer.SetAnimation( CustomerAnim::WALK_WITHPARCEL, 0, true );

			// close enough to taxi
			if( m_Player.IsOnGroundIdle() &&
				std::fabs(pos.X-m_Player.GetPosition().X) < m_GameData.CUSTOMER_CLOSE_TO_TAXI )
			{
				// now walk away
				customer.SetState( Customer::CustomerState_WalkAway );
				customer.SetCargoState( false );

				m_Player.AddCargo( customer.GetDestinationNumeric(), customer );

				m_OneShotSFX.nSourceID = snd::SoundManager::GetInstance()->GetFreeSoundSource();

				if( m_OneShotSFX.nSourceID != snd::INVALID_SOUNDSOURCE )
					AudioSystem::GetInstance()->PlayAudio( m_OneShotSFX.nSourceID, m_DoorOpenId, math::Vec3(0.0f,0.0f,0.0f), AL_TRUE, AL_FALSE );	

				// find a random spot to walk to
				int zoneIndex = customer.GetSpawnZone();
				float customerX = (m_SpawnAreaList[zoneIndex].boundArea.pos.X+(m_SpawnAreaList[zoneIndex].boundArea.dim.Width*0.5f));
				math::Vec3 pos( customerX, (m_SpawnAreaList[zoneIndex].boundArea.pos.Y - m_SpawnAreaList[zoneIndex].boundArea.dim.Height*0.5f), 0.0f );
				
				customer.SetDropOffWalkPoint( pos );
			}

			customer.SetPosition(pos);
		}
		else if( (customer.GetState() == Customer::CustomerState_WaitingForCargo || customer.GetState() == Customer::CustomerState_WalkToTaxiForCargo ) &&
			m_Player.GetLandingZoneIndex() == customer.GetSpawnZone() &&
			m_Player.GetCargoDestination() == customer.GetSpawnNumeric() &&
			m_Player.HasPassenger() == false &&
			m_Player.HasCargo() )
		{
			// walk to taxi
			customer.SetState( Customer::CustomerState_WalkToTaxiForCargo );
				
			math::Vec3 pos = customer.GetPosition();
			if( pos.X < m_Player.GetPosition().X )
			{
				pos.X += m_GameData.CUSTOMER_WALK_SPEED*deltaTime;
				customer.SetTargetRotation(90.0f);
			}
			else if( pos.X > m_Player.GetPosition().X )
			{
				pos.X -= m_GameData.CUSTOMER_WALK_SPEED*deltaTime;
				customer.SetTargetRotation(-90.0f);
			}
			
			customer.SetAnimation( CustomerAnim::WALK, 0, true );

			// close enough to taxi
			if( m_Player.IsOnGroundIdle() &&
				std::fabs(pos.X-m_Player.GetPosition().X) < m_GameData.CUSTOMER_CLOSE_TO_TAXI )
			{
				// now walk away
				customer.SetState( Customer::CustomerState_WalkAwayWithCargo );
				customer.SetCargoState( true, m_Player.GetCargoTextureId() );
				
				m_Player.RemoveCargo();

				m_OneShotSFX.nSourceID = snd::SoundManager::GetInstance()->GetFreeSoundSource();

				if( m_OneShotSFX.nSourceID != snd::INVALID_SOUNDSOURCE )
					AudioSystem::GetInstance()->PlayAudio( m_OneShotSFX.nSourceID, m_DoorOpenId, math::Vec3(0.0f,0.0f,0.0f), AL_TRUE, AL_FALSE );	

				// add money
				m_Player.AddMoney( m_GameData.FARE_COURIER, true );
				ProfileManager::GetInstance()->GetActiveLevelData()->parcelsTransported += 1;

				if( customer.GetPerfectFareFlag() )
					ProfileManager::GetInstance()->GetActiveLevelData()->perfectFares += 1;

				if( customer.GetFareValue() > m_GameData.FARE_MINIMUM )
					PlayCustomerEndAudio( customer, true );

				// find a random spot to walk to
				int zoneIndex = customer.GetSpawnZone();
				float customerX = (m_SpawnAreaList[zoneIndex].boundArea.pos.X+(m_SpawnAreaList[zoneIndex].boundArea.dim.Width*0.5f));
				math::Vec3 pos( customerX, (m_SpawnAreaList[zoneIndex].boundArea.pos.Y - m_SpawnAreaList[zoneIndex].boundArea.dim.Height*0.5f), 0.0f );
				
				customer.SetDropOffWalkPoint( pos );
			}

			customer.SetPosition(pos);
		}

		// turn on the destination customer
		if( (customer.GetState() == Customer::CustomerState_WaitingForCargo) &&
			customer.GetDrawState() == false &&
			m_Player.GetCargoDestination() == customer.GetSpawnNumeric() &&
			m_Player.HasCargo() )
		{
			customer.StartCargoTravelTimer();
			customer.SetDrawState(true);
		}
	}
}

/////////////////////////////////////////////////////
/// Method: CheckForPlayerLandCustomer
/// Params: [in]customer, [in]deltaTime
///
/////////////////////////////////////////////////////
void CustomerManagement::CheckForPlayerLandCustomer( Customer& customer, float deltaTime )
{
	if( m_Player.GetFuelCount() <= 0 )
		return;

	float playerAngle = math::RadToDeg( m_Player.GetBody()->GetAngle() );

	if( (m_Player.IsInContactWithWorld() && m_Player.IsInLandingZone()) &&
			PhysicsWorld::ContactNormal.y >= 0.9f &&
			(playerAngle > -0.1f && playerAngle < 0.1f) )
	{
		// customer waiting for pickup
		if( (customer.GetState() == Customer::CustomerState_Waiting || customer.GetState() == Customer::CustomerState_WalkToTaxi ) &&
			m_Player.GetLandingZoneIndex() == customer.GetSpawnZone() &&
			m_Player.HasPassenger() == false &&
			m_Player.HasCargo() == false  )
		{
			// walk to taxi
			customer.SetState( Customer::CustomerState_WalkToTaxi );
				
			math::Vec3 pos = customer.GetPosition();
			if( pos.X < m_Player.GetPosition().X )
			{
				pos.X += m_GameData.CUSTOMER_WALK_SPEED*deltaTime;
				customer.SetTargetRotation(90.0f);
			}
			else if( pos.X > m_Player.GetPosition().X )
			{
				pos.X -= m_GameData.CUSTOMER_WALK_SPEED*deltaTime;
				customer.SetTargetRotation(-90.0f);
			}
				
			// set walk animation
			customer.SetAnimation( CustomerAnim::WALK, 0, true );

			// close enough to taxi
			if( m_Player.IsOnGroundIdle() &&
				std::fabs(pos.X-m_Player.GetPosition().X) < m_GameData.CUSTOMER_CLOSE_TO_TAXI )
			{
				customer.SetState( Customer::CustomerState_InTaxi );

				// clear the spawn
				m_SpawnAreaList[customer.GetSpawnZone()].inUseCounter--;

				// show the numeric request
				m_Player.AddPassenger( customer.GetDestinationNumeric(), customer );

				m_OneShotSFX.nSourceID = snd::SoundManager::GetInstance()->GetFreeSoundSource();

				if( m_OneShotSFX.nSourceID != snd::INVALID_SOUNDSOURCE )
					AudioSystem::GetInstance()->PlayAudio( m_OneShotSFX.nSourceID, m_DoorOpenId, math::Vec3(0.0f,0.0f,0.0f), AL_TRUE, AL_FALSE );	
			}

			customer.SetPosition(pos);
		}
		else if( customer.GetState() == Customer::CustomerState_InTaxi && 
				m_Player.IsOnGroundIdle() &&
				m_Player.GetLandingZoneIndex() == customer.GetDestinationZone() )
		{
			// customer was in the taxi, has arrived at the destination
			customer.SetState( Customer::CustomerState_DroppedOff );

			m_Player.RemovePassenger();

			// award the fare to the player
			m_Player.AddMoney( customer.GetFareValue(), true );
			ProfileManager::GetInstance()->GetActiveLevelData()->customersTransported += 1;

			if( customer.GetPerfectFareFlag() )
				ProfileManager::GetInstance()->GetActiveLevelData()->perfectFares += 1;

			int index = customer.GetDestinationZone();

			// find a random spot to walk to
			float customerX = (m_SpawnAreaList[index].boundArea.pos.X+(m_SpawnAreaList[index].boundArea.dim.Width*0.5f));
			math::Vec3 pos( customerX, (m_SpawnAreaList[index].boundArea.pos.Y - m_SpawnAreaList[index].boundArea.dim.Height*0.5f), 0.0f );
				
			// start at the taxi
			math::Vec3 taxiPos = m_Player.GetPosition();

			taxiPos.Y = (m_SpawnAreaList[index].boundArea.pos.Y - m_SpawnAreaList[index].boundArea.dim.Height*0.5f);
			taxiPos.Z = m_GameData.CUSTOMER_DROPOFF_DEPTH;
			
			customer.SetPosition( taxiPos );
			customer.SetDropOffWalkPoint( pos );

			// play audio
			m_OneShotSFX.nSourceID = snd::SoundManager::GetInstance()->GetFreeSoundSource();
			
			if( m_OneShotSFX.nSourceID != snd::INVALID_SOUNDSOURCE )
				AudioSystem::GetInstance()->PlayAudio( m_OneShotSFX.nSourceID, m_MoneyBigTipBufferId, math::Vec3(0.0f,0.0f,0.0f), AL_TRUE );
			
			if( customer.GetFareValue() > m_GameData.FARE_MINIMUM )
				PlayCustomerEndAudio( customer );

			// add to customer count
			int oldCount = m_Player.GetCustomersComplete();
			m_Player.SetCustomersComplete( oldCount+1 );
		}			
	}
}

/////////////////////////////////////////////////////
/// Method: FindDestinationForCargo
/// Params: [in]sourceIndex
///
/////////////////////////////////////////////////////
int CustomerManagement::FindDestinationForCargo( int sourceIndex )
{
	// find a free landing zone
	std::vector<CustomerSpawnArea>::iterator it = m_SpawnAreaList.begin();
	int index = 0;

	int lastFreeSlotIndex = 0;
	bool freeSlot = false;
	while( it != m_SpawnAreaList.end() )
	{
		if( index != sourceIndex &&
			(*it).inUseCounter < 1 && // cargo destination cannot have anything there (depth clash)
			(*it).boundArea.type == Level::ePortType_LandingZone )
		{
			freeSlot = true;

			m_FreeSlots[lastFreeSlotIndex] = index;
			lastFreeSlotIndex++;

			if( lastFreeSlotIndex >= FREE_SLOTS )
				break;
		}
			
		index++;
		it++;
	}

	if( freeSlot )
	{
		int freeIndex = m_FreeSlots[math::RandomNumber( 0, lastFreeSlotIndex-1 )];

		return(freeIndex);
	}

	return(-1);
}

/////////////////////////////////////////////////////
/// Method: FindDestinationForPassenger
/// Params: [in]sourceIndex
///
/////////////////////////////////////////////////////
int CustomerManagement::FindDestinationForCustomer( int sourceIndex )
{
	// find a free landing zone
	std::vector<CustomerSpawnArea>::iterator it = m_SpawnAreaList.begin();
	int index = 0;

	int lastFreeSlotIndex = 0;
	bool freeSlot = false;
	while( it != m_SpawnAreaList.end() )
	{
		if( index != sourceIndex &&
			(*it).inUseCounter < 1 && // customer destination cannot have anything there (depth clash)
			(*it).boundArea.type == Level::ePortType_LandingZone )
		{
			freeSlot = true;

			m_FreeSlots[lastFreeSlotIndex] = index;
			lastFreeSlotIndex++;

			if( lastFreeSlotIndex >= FREE_SLOTS )
				break;
		}
			
		index++;
		it++;
	}

	if( freeSlot )
	{
		int freeIndex = m_FreeSlots[math::RandomNumber( 0, lastFreeSlotIndex-1 )];

		return(freeIndex);
	}

	return(-1);
}

/////////////////////////////////////////////////////
/// Method: DrawDebug
/// Params: None
///
/////////////////////////////////////////////////////
void CustomerManagement::DrawDebug()
{	
	bool lightState = renderer::OpenGL::GetInstance()->GetLightingState();
	bool textureState = renderer::OpenGL::GetInstance()->GetTextureState();
	
	if( lightState )
		renderer::OpenGL::GetInstance()->DisableLighting();
	if( textureState )
		renderer::OpenGL::GetInstance()->DisableTexturing();

	std::vector<Customer*>::iterator itCustomers = m_CustomerList.begin();
	while( itCustomers != m_CustomerList.end() )
	{
		const float k_segments = 16.0f;
		const float k_increment = 2.0f * math::PI / k_segments;
		float theta = 0.0f;
		renderer::OpenGL::GetInstance()->SetColour4ub( 255,255,255,255 );
		
		math::Vec3 vaPoints[16];	
		for (int j = 0; j < static_cast<int>(k_segments); ++j)
		{
			math::Vec3 v = (*itCustomers)->GetBoundingSphere().vCenterPoint + (math::Vec3(std::cos(theta), std::sin(theta), 0.0f) * (*itCustomers)->GetBoundingSphere().fRadius );
			vaPoints[j] = v;
			theta += k_increment;
		}
		
		glVertexPointer(3, GL_FLOAT, sizeof(math::Vec3), vaPoints);
		glDrawArrays(GL_LINE_LOOP, 0, 16 );
	
		// next customer
		itCustomers++;
	}	
	
	if( lightState )
		renderer::OpenGL::GetInstance()->EnableLighting();
	if( textureState )
		renderer::OpenGL::GetInstance()->EnableTexturing();
}

/////////////////////////////////////////////////////
/// Method: DrawDebug2D
/// Params: None
///
/////////////////////////////////////////////////////
void CustomerManagement::DrawDebug2D( int lastTextX, int lastTextY )
{
	int offset = 10;
	int xPos = lastTextX;
	int yPos = lastTextY;

	DBGPRINT( xPos, yPos-=offset, "ACTIVE CUST (%d)", m_ActiveCustomers );	

	std::vector<Customer*>::iterator itCustomers = m_CustomerList.begin();
	while( itCustomers != m_CustomerList.end() )
	{
		if( (*itCustomers)->GetState() == Customer::CustomerState_InTaxi )
			DBGPRINT( xPos, yPos-=offset, "CUST - (src:%d  dest:%d)", (*itCustomers)->GetSpawnZone(), (*itCustomers)->GetDestinationZone() );

		// next customer
		itCustomers++;
	}
}

/////////////////////////////////////////////////////
/// Method: Reset
/// Params: None
///
/////////////////////////////////////////////////////
void CustomerManagement::Reset()
{
	int i=0;
	unsigned int j=0;

	for( i=0; i < FREE_SLOTS; ++i )
		m_FreeSlots[i] = -1;

	m_ActiveCustomers = 0;
	m_MaxActiveCustomers = m_TaxiLevel.GetCustomerMaxSpawn();

	m_LastDeltaTime = 0.0f;
	m_LastSpawnTime = m_MaxSpawnTime*0.5f;

	for( j=0; j < m_CustomerList.size(); ++j )
	{
		delete m_CustomerList[j];
		m_CustomerList[j] = 0;
	}	
	m_CustomerList.clear();

	std::vector<CustomerSpawnArea>::iterator spawnIt = m_SpawnAreaList.begin();
	while( spawnIt != m_SpawnAreaList.end() )
	{
		(*spawnIt).inUseCounter = 0;

		// next spawn area
		spawnIt++;
	}

	m_EmitterIndex = 0;
}

/////////////////////////////////////////////////////
/// Method: PlayEmitter
/// Params: [in]pos
///
/////////////////////////////////////////////////////
void CustomerManagement::PlayEmitter( const math::Vec3& pos )
{
	m_EmitterIndex++;
	if( m_EmitterIndex >= MAX_CUSTOMER_EMITTERS )
		m_EmitterIndex = 0;

	m_Emitters[m_EmitterIndex]->SetPos( pos );
	m_Emitters[m_EmitterIndex]->Enable();
	m_Emitters[m_EmitterIndex]->Disable();
}
