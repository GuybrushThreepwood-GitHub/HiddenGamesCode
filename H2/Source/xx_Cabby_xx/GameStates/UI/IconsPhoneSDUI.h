

/*===================================================================
	File: IconsPhoneSDUI.h
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#ifndef __ICONSPHONESDUI_H__
#define __ICONSPHONESDUI_H__

// forward declare
#include "GameStates/UI/UIBaseState.h"
#include "GameStates/IBaseGameState.h"

#include "GameStates/UI/UIFileLoader.h"

class IconsPhoneSDUI : public UIBaseState, public IBaseGameState
{
	public:
		IconsPhoneSDUI( StateManager& stateManager, InputSystem& inputSystem );
		virtual ~IconsPhoneSDUI();

		virtual void Enter();
		virtual void Exit();
		virtual void PrepareTransitionIn();
		virtual int TransitionIn();
		virtual void PrepareTransitionOut();
		virtual int TransitionOut();

		virtual void Update( float deltaTime );
		virtual void Draw();

	private:
		math::Vec3& GetSafeIconPosition( math::Vec3& iconPos, float oriWidth, float oriHeight );

	protected:
		float m_LastDeltaTime;

		mdl::ModelHGM* m_UIMesh;
		UIFileLoader m_Elements;

		Player* m_Player;
		CustomerManagement* m_Customers;
		GameData m_GameData;

		std::vector<Level::PortArea> m_PortAreaList;

		std::vector<Customer*> m_CustomerList;
		std::vector<CustomerManagement::CustomerSpawnArea> m_SpawnAreaList;

		float m_HalfWidth;
		float m_HalfHeight;
	
		float m_OriWidth;
		float m_OriHeight;	

		int m_ParcelIconIndex;
		int m_CustomerIconIndex;

};

#endif // __ICONSPHONESDUI_H__

