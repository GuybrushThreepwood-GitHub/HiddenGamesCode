
#ifndef __MICROGAMEUSEITEM_H__
#define __MICROGAMEUSEITEM_H__

#include "MicroGame.h"
#include "GameStates/UI/UIFileLoader.h"

class MicroGameUseItem : public MicroGame
{
	public:
		MicroGameUseItem( int correctSlot, const char* giveupFunc );
		virtual ~MicroGameUseItem();

		void Initialise();
		void Release();

		void OnEnter();
		void OnExit();

		void Update( float deltaTime );
		void Draw();

		bool IsComplete()		{ return m_Complete; }
		bool ReturnCode()		{ return m_ReturnCode; }

		void SetupInventory();

	private:
		mdl::ModelHGM* m_InventoryModel;
		UIFileLoader m_InventoryUI;

		bool m_HasItem;

		int m_CorrectSlot;
		const char* m_GiveUpFunc;

		int m_SelectedItem;
		char m_ValString[32];
};

#endif // __MICROGAMEUSEITEM_H__
