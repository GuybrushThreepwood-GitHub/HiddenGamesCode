
#ifndef __MICROGAMEINVENTORY_H__
#define __MICROGAMEINVENTORY_H__

#include "MicroGame.h"
#include "GameStates/UI/UIFileLoader.h"

class MicroGameInventory : public MicroGame
{
	public:
		MicroGameInventory( );
		virtual ~MicroGameInventory();

		void Initialise();
		void Release();

		void OnEnter();
		void OnExit();

		void Update( float deltaTime );
		void Draw();

		bool IsComplete()		{ return m_Complete; }
		bool ReturnCode()		{ return m_ReturnCode; }

		void SetupInventory();
		void SetupCollectables();

	private:
		mdl::ModelHGM* m_InventoryModel;
		UIFileLoader m_InventoryUI;

		ALuint m_PistolEquipAudio;
		ALuint m_ShotgunEquipAudio;
		ALuint m_UnEquipAudio;

		int m_PageIndex;
		int m_SelectedItem;
		char m_ValString[32];

		int m_Clicks;

};

#endif // __MICROGAMEINVENTORY_H__
