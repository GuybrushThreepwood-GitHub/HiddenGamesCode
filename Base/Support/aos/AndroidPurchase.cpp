
/*===================================================================
	File: AndroidPurchase.cpp
	Library: Support

	(C)Hidden Games
=====================================================================*/

#ifdef BASE_PLATFORM_ANDROID

#include "Core/CoreConsts.h"
#include "Core/CoreDefines.h"
#include "Core/FileIO.h"
#include "Core/App.h"
#include "Debug/Assertion.h"

#include "AndroidPurchase.h"

using namespace support::aos;

/////////////////////////////////////////////////////
/// Default constructor
/// 
///
/////////////////////////////////////////////////////
AndroidPurchase::AndroidPurchase()
{
	core::app::SetIsStoreAvailable(false);
}

/////////////////////////////////////////////////////
/// Default destructor
/// 
///
/////////////////////////////////////////////////////
AndroidPurchase::~AndroidPurchase()
{
    
    
}

/////////////////////////////////////////////////////
/// Method: Initialise
/// 
///
/////////////////////////////////////////////////////
void AndroidPurchase::Initialise( std::list<support::TPurchase> itemList )
{

}

/////////////////////////////////////////////////////
/// Method: PurchaseItem
/// 
///
/////////////////////////////////////////////////////
void AndroidPurchase::PurchaseItem( const char* purchaseId )
{

}

/////////////////////////////////////////////////////
/// Method: RestorePurchases
/// 
///
/////////////////////////////////////////////////////
void AndroidPurchase::RestorePurchases()
{

}

/////////////////////////////////////////////////////
/// Method: RestorePurchases
/// 
///
/////////////////////////////////////////////////////
bool AndroidPurchase::IsContentAlreadyPurchased( const char* purchaseId )
{	
	return false;
}

/////////////////////////////////////////////////////
/// Method: SetCallback
/// 
///
/////////////////////////////////////////////////////
void AndroidPurchase::SetCallback( support::PurchaseCallback* pCallback )
{
	m_Callback = pCallback;
}

#endif // BASE_PLATFORM_ANDROID
