
#ifndef __IAPLIST_H__
#define __IAPLIST_H__

#include <list>
#include "Support/PurchaseCommon.h"

namespace res
{
	void CreateIAPList();

	void ClearIAPList();

	const char* GetIAPString( int index );
	
	int GetIAPIndex( const char* purchaseId );
	
	std::list<support::TPurchase> GetIAPAsList();
}

#endif // __IAPLIST_H__

