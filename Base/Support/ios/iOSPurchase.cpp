
/*===================================================================
	File: iOSPurchase.cpp
	Library: Support

	(C)Hidden Games
=====================================================================*/

#ifdef BASE_PLATFORM_iOS

#include "Core/CoreConsts.h"
#include "Core/CoreDefines.h"
#include "Core/FileIO.h"
#include "Core/App.h"
#include "Debug/Assertion.h"

#include "iOSPurchase.h"

using namespace support::ios;

/////////////////////////////////////////////////////
/// Default constructor
/// 
///
/////////////////////////////////////////////////////
iOSPurchase::iOSPurchase()
{
    m_AppStoreSupport = IsAppStoreAvailable();
	m_Callback = 0;
	
	core::app::SetIsStoreAvailable(m_AppStoreSupport);
	
    if( m_AppStoreSupport )
    {
		m_AppPurchases = [[AppStorePurchases alloc] init];		
		[m_AppPurchases initialise];
    }	
}

/////////////////////////////////////////////////////
/// Default destructor
/// 
///
/////////////////////////////////////////////////////
iOSPurchase::~iOSPurchase()
{
    if( m_AppPurchases != nil )
	{
		[AppStorePurchases dealloc];
		m_AppPurchases = nil;
	}
}

/////////////////////////////////////////////////////
/// Method: Initialise
/// 
///
/////////////////////////////////////////////////////
void iOSPurchase::Initialise( std::list<support::TPurchase> itemList )
{
	if( m_AppStoreSupport )
	{
		// convert the purchase list to a NSSet
		if( !itemList.empty() )
		{
			NSSet* purchaseSet = [[NSSet alloc] init];
			std::list<support::TPurchase>::iterator it = itemList.begin();
			
			while( it != itemList.end() )
			{		
				NSString* convertedString = [NSString stringWithUTF8String: it->purchaseId];
				
				purchaseSet = [purchaseSet setByAddingObject:convertedString];
				
				// next
				++it;
			}
			
			//NSSet *testSet = [NSSet setWithObject:@"uk.co.hiddengames.purchasetest" ];
			
			// initialise with the new purchase set
			[m_AppPurchases requestApplicationProducts: purchaseSet];
		}			
	}
}

/////////////////////////////////////////////////////
/// Method: PurchaseItem
/// 
///
/////////////////////////////////////////////////////
void iOSPurchase::PurchaseItem( const char* purchaseId )
{
	if( m_AppStoreSupport )
	{
		NSString* convertedString = [NSString stringWithUTF8String: purchaseId];
		
		[m_AppPurchases purchaseContent: convertedString];	
	}
}

/////////////////////////////////////////////////////
/// Method: RestorePurchases
/// 
///
/////////////////////////////////////////////////////
void iOSPurchase::RestorePurchases()
{
	if( m_AppStoreSupport )
	{
		[m_AppPurchases restoreNonConsumables];
	}
}

/////////////////////////////////////////////////////
/// Method: RestorePurchases
/// 
///
/////////////////////////////////////////////////////
bool iOSPurchase::IsContentAlreadyPurchased( const char* purchaseId )
{
	if( m_AppStoreSupport )
	{
		return [m_AppPurchases hasPurchasedContent: purchaseId];
	}	
	
	return false;
}

/////////////////////////////////////////////////////
/// Method: SetCallback
/// 
///
/////////////////////////////////////////////////////
void iOSPurchase::SetCallback( support::PurchaseCallback* pCallback )
{
	m_Callback = pCallback;
	if( m_AppStoreSupport )
	{
		[m_AppPurchases setPurchaseCallback:pCallback];
	}
}

#endif // BASE_PLATFORM_iOS

