
/*===================================================================
	File: InventoryManagement.h
	Game: H4

	(C)Hidden Games
=====================================================================*/

#ifndef __INVENTORYMANAGEMENT_H__
#define __INVENTORYMANAGEMENT_H__

#include <vector>

class InventoryManagement
{
	public:
		InventoryManagement();
		~InventoryManagement();

		void AddItem( int index );
		bool HasItem( int index );
		void RemoveItem( int index );

		const std::vector<int>& GetItemList()	{ return m_ItemList; }

	private:
		std::vector<int> m_ItemList;
};

#endif // __INVENTORYMANAGEMENT_H__
