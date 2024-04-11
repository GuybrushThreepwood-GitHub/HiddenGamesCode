
/*===================================================================
	File: WinPurchase.cpp
	Library: Support

	(C)Hidden Games
=====================================================================*/

#ifdef BASE_PLATFORM_WINDOWS

#include "CoreBase.h"
#include "Debug/Assertion.h"

#include "WinPurchase.h"

using namespace support::win;

/////////////////////////////////////////////////////
/// Default constructor
/// 
///
/////////////////////////////////////////////////////
WinPurchase::WinPurchase()
{
	m_Callback = 0;
	
	core::app::SetIsStoreAvailable(true);

}

/////////////////////////////////////////////////////
/// Default destructor
/// 
///
/////////////////////////////////////////////////////
WinPurchase::~WinPurchase()
{
    
    
}

/////////////////////////////////////////////////////
/// Method: Initialise
/// 
///
/////////////////////////////////////////////////////
void WinPurchase::Initialise( std::list<support::TPurchase> itemList )
{
	
}

/////////////////////////////////////////////////////
/// Method: PurchaseItem
/// 
///
/////////////////////////////////////////////////////
void WinPurchase::PurchaseItem( const char* purchaseId )
{
	if( m_Callback )
		m_Callback->Success( purchaseId );
}

/////////////////////////////////////////////////////
/// Method: RestorePurchases
/// 
///
/////////////////////////////////////////////////////
void WinPurchase::RestorePurchases()
{
	if( m_Callback )
		m_Callback->RestoreComplete( );
}

/////////////////////////////////////////////////////
/// Method: RestorePurchases
/// 
///
/////////////////////////////////////////////////////
bool WinPurchase::IsContentAlreadyPurchased( const char* purchaseId )
{	
	return false;
}

/////////////////////////////////////////////////////
/// Method: SetCallback
/// 
///
/////////////////////////////////////////////////////
void WinPurchase::SetCallback( support::PurchaseCallback* pCallback )
{
	m_Callback = pCallback;
}

#endif // BASE_PLATFORM_WINDOWS

