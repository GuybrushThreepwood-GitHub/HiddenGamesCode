
/*===================================================================
	File: RaspberryPiPurchase.cpp
	Library: Support

	(C)Hidden Games
=====================================================================*/

#ifdef BASE_PLATFORM_RASPBERRYPI

#include "Core/CoreConsts.h"
#include "Core/CoreDefines.h"
#include "Core/FileIO.h"
#include "Core/App.h"
#include "Debug/Assertion.h"

#include "RaspberryPiPurchase.h"

using namespace support::rpi;

/////////////////////////////////////////////////////
/// Default constructor
/// 
///
/////////////////////////////////////////////////////
RaspberryPiPurchase::RaspberryPiPurchase()
{
	core::app::SetIsStoreAvailable(false);
}

/////////////////////////////////////////////////////
/// Default destructor
/// 
///
/////////////////////////////////////////////////////
RaspberryPiPurchase::~RaspberryPiPurchase()
{
    
    
}

/////////////////////////////////////////////////////
/// Method: Initialise
/// 
///
/////////////////////////////////////////////////////
void RaspberryPiPurchase::Initialise( std::list<support::TPurchase> itemList )
{

}

/////////////////////////////////////////////////////
/// Method: PurchaseItem
/// 
///
/////////////////////////////////////////////////////
void RaspberryPiPurchase::PurchaseItem( const char* purchaseId )
{

}

/////////////////////////////////////////////////////
/// Method: RestorePurchases
/// 
///
/////////////////////////////////////////////////////
void RaspberryPiPurchase::RestorePurchases()
{

}

/////////////////////////////////////////////////////
/// Method: RestorePurchases
/// 
///
/////////////////////////////////////////////////////
bool RaspberryPiPurchase::IsContentAlreadyPurchased( const char* purchaseId )
{	
	return false;
}

/////////////////////////////////////////////////////
/// Method: SetCallback
/// 
///
/////////////////////////////////////////////////////
void RaspberryPiPurchase::SetCallback( support::PurchaseCallback* pCallback )
{
	m_Callback = pCallback;
}

#endif // BASE_PLATFORM_RASPBERRYPI
