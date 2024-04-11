
/*===================================================================
	File: CustomerManagement.h
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#ifndef __CUSTOMERMANAGEMENT_H__
#define __CUSTOMERMANAGEMENT_H__

#include "Audio/AudioSystem.h"
#include "Customers/Customer.h"

const int FREE_SLOTS = 10;
const int MAX_CUSTOMER_EMITTERS = 4;

class CustomerManagement
{
	public:
		struct CustomerSpawnArea
		{
			int levelAreaIndex;

			int inUseCounter;
			//float lastTime;

			Level::PortArea boundArea;
		};

	public:
		CustomerManagement( Player& player, Level& taxiLevel );
		~CustomerManagement();

		void Draw( bool pauseFlag );
		void Update( float deltaTime );

		void DrawDebug();
		void DrawDebug2D( int lastTextX, int lastTextY );

		void Reset();

		std::vector<Customer*>& GetCustomerList()			{ return m_CustomerList; }
		std::vector<CustomerSpawnArea>& GetSpawnAreas()		{ return m_SpawnAreaList; }

	private:

		void UpdateCargo( Customer& customer, float deltaTime );
		void UpdateCustomer( Customer& customer, float deltaTime );

		bool CreateCargo( int freeSlotCount );
		bool CreateCustomer( int freeSlotCount );

		void CheckForCustomerSpawn();
		void CheckForPlayerLand( float deltaTime );

		void CheckForPlayerLandCargo( Customer& customer, float deltaTime );
		void CheckForPlayerLandCustomer( Customer& customer, float deltaTime );

		int FindDestinationForCargo( int sourceIndex );
		int FindDestinationForCustomer( int sourceIndex );

		math::Vec2& GetSafeIconPosition( math::Vec2& iconPos, float oriWidth, float oriHeight );

		void PlayEmitter( const math::Vec3& pos );

	private:
		int m_FreeSlots[FREE_SLOTS];
		int m_ActiveCustomers;
		int m_MaxActiveCustomers;
		
		float m_LastDeltaTime;
		float m_MaxSpawnTime;
		float m_LastSpawnTime;

		Player& m_Player;
		Level& m_TaxiLevel;	
		mdl::ModelHGA* m_pModelData;
		efx::Emitter* m_Emitters[MAX_CUSTOMER_EMITTERS];
		int m_EmitterIndex;

		ScriptDataHolder::DevScriptData m_DevData;
		GameData m_GameData;

		std::vector<Customer*> m_CustomerList;
		std::vector<CustomerSpawnArea> m_SpawnAreaList;
		std::vector<GLuint> m_CustomerTextures;

		Sfx m_OneShotSFX;

		ALuint m_MoneyBigTipBufferId;
		ALuint m_DoorOpenId;
};

#endif // __CUSTOMERMANAGEMENT_H__
