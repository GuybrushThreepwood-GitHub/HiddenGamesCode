
/*===================================================================
	File: MacPurchase.cpp
	Library: Support

	(C)Hidden Games
=====================================================================*/

#ifdef BASE_PLATFORM_MAC

#include "Core/CoreDefines.h"
#include "Core/CoreConsts.h"
#include "Core/FileIO.h"
#include "Core/App.h"
#include "Debug/Assertion.h"

#include "MacPurchase.h"

using namespace support::mac;

/////////////////////////////////////////////////////
/// Default constructor
/// 
///
/////////////////////////////////////////////////////
MacPurchase::MacPurchase()
{
	m_Callback = 0;
	
	core::app::SetIsStoreAvailable(true);

}

/////////////////////////////////////////////////////
/// Default destructor
/// 
///
/////////////////////////////////////////////////////
MacPurchase::~MacPurchase()
{
    
    
}

/////////////////////////////////////////////////////
/// Method: Initialise
/// 
///
/////////////////////////////////////////////////////
void MacPurchase::Initialise( std::list<support::TPurchase> itemList )
{
	
}

/////////////////////////////////////////////////////
/// Method: PurchaseItem
/// 
///
/////////////////////////////////////////////////////
void MacPurchase::PurchaseItem( const char* purchaseId )
{
	if( m_Callback )
		m_Callback->Success( purchaseId );
}

/////////////////////////////////////////////////////
/// Method: RestorePurchases
/// 
///
/////////////////////////////////////////////////////
void MacPurchase::RestorePurchases()
{
	if( m_Callback )
		m_Callback->RestoreComplete( );
}

/////////////////////////////////////////////////////
/// Method: RestorePurchases
/// 
///
/////////////////////////////////////////////////////
bool MacPurchase::IsContentAlreadyPurchased( const char* purchaseId )
{	
	return false;
}

/////////////////////////////////////////////////////
/// Method: SetCallback
/// 
///
/////////////////////////////////////////////////////
void MacPurchase::SetCallback( support::PurchaseCallback* pCallback )
{
	m_Callback = pCallback;
}

#endif // BASE_PLATFORM_MAC

