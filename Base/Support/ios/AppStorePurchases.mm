
/*===================================================================
	File: AppStorePurchaseManager.mm
	Library: Support

	(C)Hidden Games
=====================================================================*/

#ifdef BASE_PLATFORM_iOS

#import <StoreKit/StoreKit.h>

#include "CoreBase.h"
#import "Support/ios/AppStorePurchases.h"

namespace
{
	// the following can be used to check the receipts actually came from Apple if running a custom server
	/// NSURL *realStoreURL = [[NSURL alloc] initWithString: @"https://buy.itunes.apple.com/verifyReceipt"];
	/// NSURL *sandboxStoreURL = [[NSURL alloc] initWithString: @"https://sandbox.itunes.apple.com/verifyReceipt"];
}


/// AppStorePurchases
@implementation AppStorePurchases

@synthesize productsRequested;

/////////////////////////////////////////////////////
/// Method: init
/// Returns: None 
///
/////////////////////////////////////////////////////
- (id) init
{
	self = [super init];
    
	return self;
}

/////////////////////////////////////////////////////
/// Method: dealloc
/// Returns: None 
///
/////////////////////////////////////////////////////

/// startManager - starts the store observer and requests the passed product list 
- (void) initialise
{	
	mHasResponseFromStore = false;
	
	mPurchaseList.clear();
	
	mProducts.clear();
	
	// create a store observer 
	[[SKPaymentQueue defaultQueue] addTransactionObserver:self];
}

- (void) requestApplicationProducts: (NSSet *)productRequestList
{
	// pass the request list
	productsRequested = [[SKProductsRequest alloc] initWithProductIdentifiers: productRequestList];
	
	// this class is a delegate to receive the request callbacks
	productsRequested.delegate = self;
	
	// start the request
	[productsRequested start];
}

/// canMakePayments - checks for any restrictions on the user as to whether they can make payments on the store
- (bool) canMakePayments
{
	if ([SKPaymentQueue canMakePayments])
	{
		// Display a store to the user.
		return true;
	}
	else
	{
		// Warn the user that purchases are disabled.
		return false;
	}
}

/// Called when there's an error in a request
- (void)request:(SKRequest *)request didFailWithError:(NSError *)error
{
	
}

/// Called when the Apple App Store responds to the product request. (required)
- (void)productsRequest:(SKProductsRequest *)request didReceiveResponse:(SKProductsResponse *)response
{
	// clear old
	mPurchaseList.clear();

	mProducts.clear();
	
	DBGLOG( "APPSTOREPURCHASES: ****VALID STORE PRODUCTS****\n" );
	
	// populate 
	for (SKProduct *product in response.products)
	{
		NSLog( @"**************" );
			
		// product.localizedDescription
		NSLog( @"product.localizedDescription : %@", product.localizedDescription );
			
		// product.localizedTitle 
		NSLog( @"product.localizedTitle : %@", product.localizedTitle );
			
		// product.price  
		NSNumberFormatter *numberFormatter = [[NSNumberFormatter alloc] init];
		[numberFormatter setFormatterBehavior:NSNumberFormatterBehavior10_4];
		[numberFormatter setNumberStyle:NSNumberFormatterCurrencyStyle];
		[numberFormatter setLocale:product.priceLocale];
		NSString *formattedString = [numberFormatter stringFromNumber:product.price];
			
		NSLog( @"product.price : %@", formattedString );
			
		// product.productIdentifier  		
		NSLog( @"product.productIdentifier : %@", product.productIdentifier );
			
		NSLog( @"**************" );
		
		// store as a valid item
		if( product &&
		   product.productIdentifier != nil )
		{
			support::TPurchase item;
			item.isValid = true;
			snprintf( item.purchaseId, MAX_PURCHASE_ID_LEN, "%s", [product.productIdentifier cStringUsingEncoding:NSASCIIStringEncoding] );
		
			mPurchaseList.push_back(item);
		
			mProducts.push_back(product);
		}
	}
	
	// check if any product failed
	DBGLOG( "APPSTOREPURCHASES: ****INVALID STORE PRODUCTS****\n" );
	
	for( NSString *invalidProductId in response.invalidProductIdentifiers )
	{
		// purchase is invalid
		if( invalidProductId != nil )
		{
			support::TPurchase item;
			item.isValid = false;
			snprintf( item.purchaseId, MAX_PURCHASE_ID_LEN, "%s", [invalidProductId cStringUsingEncoding:NSASCIIStringEncoding] );
			
			mPurchaseList.push_back(item);
			
			DBGLOG("%s\n", item.purchaseId);
		}
	}
	
	mHasResponseFromStore = true;
	
	// clear the list
}

// Tells an observer that one or more transactions have been updated. (required)
- (void)paymentQueue:(SKPaymentQueue *)queue updatedTransactions:(NSArray *)transactions
{
    for (SKPaymentTransaction *transaction in transactions)
    {
        switch (transaction.transactionState)
        {
            case SKPaymentTransactionStatePurchased:
                [self completeTransaction:transaction];
                break;
            case SKPaymentTransactionStateFailed:
                [self failedTransaction:transaction];
                break;
            case SKPaymentTransactionStateRestored:
                [self restoreTransaction:transaction];
				break;
            default:
                break;
        }
    }
}

// Tells an observer that one or more transactions have been removed from the queue.
/*- (void)paymentQueue:(SKPaymentQueue *)queue removedTransactions:(NSArray *)transactions
 {
 NSLog( @"removedTransactions" );
 
 for (SKPaymentTransaction *transaction in transactions)
 {
 NSLog( @"  transaction: %@", transaction.payment.productIdentifier );
 }
 
 }*/

//  Tells the observer that an error occurred while restoring transactions.
- (void)paymentQueue:(SKPaymentQueue *)queue restoreCompletedTransactionsFailedWithError:(NSError *)error
{
	NSLog( @"restoreCompletedTransactionsFailedWithError - error %@", error );
	
	if( mCallback &&
	   error != nil )
	{
		mCallback->RestoreFailed( error.code );
	}	
}

// Tells the observer that the payment queue has finished sending restored transactions.
- (void)paymentQueueRestoreCompletedTransactionsFinished:(SKPaymentQueue *)queue
{
	DBGLOG( "APPSTOREPURCHASES: paymentQueueRestoreCompletedTransactionsFinished\n" );
	if( mCallback )
	{
		mCallback->RestoreComplete();
	}
}

/// transaction callbacks ///

/// successful transaction
- (void) completeTransaction: (SKPaymentTransaction *)transaction
{
	DBGLOG( "APPSTOREPURCHASES: completeTransaction\n" );
	
	if( transaction &&
	   transaction.payment &&
	   transaction.payment.productIdentifier)
	{
		[self recordTransaction: transaction keyVal: transaction.payment.productIdentifier];
		[self providePurchasedContent: transaction.payment.productIdentifier];
	}
	
	// Remove the transaction from the payment queue.
    [[SKPaymentQueue defaultQueue] finishTransaction: transaction];
}

/// transaction was restored
- (void) restoreTransaction: (SKPaymentTransaction *)transaction
{
	DBGLOG( "APPSTOREPURCHASES: restoreTransaction\n" );
	
	if( transaction &&
	   transaction.originalTransaction &&
	   transaction.originalTransaction.payment &&
	   transaction.originalTransaction.payment.productIdentifier )
	{
		[self recordTransaction: transaction keyVal: transaction.originalTransaction.payment.productIdentifier];
		[self provideRestoredContent: transaction.originalTransaction.payment.productIdentifier];
	}
	
    [[SKPaymentQueue defaultQueue] finishTransaction: transaction];
	
}

/// transaction failed
- (void) failedTransaction: (SKPaymentTransaction *)transaction
{
	DBGLOG( "APPSTOREPURCHASES: failedTransaction\n" );
	
	if( transaction )
	{
		if (transaction.error.code != SKErrorPaymentCancelled)
		{
			// Optionally, display an error here.
            DBGLOG( "APPSTOREPURCHASES: Error code %ld\n", transaction.error.code );
			
			if( mCallback != 0 )
			{
				// convert string
				const char* itemId = [transaction.originalTransaction.payment.productIdentifier cStringUsingEncoding:NSASCIIStringEncoding];
				const char* failureReason = [transaction.error.localizedDescription cStringUsingEncoding:NSASCIIStringEncoding];
				
				if( itemId != 0 &&
				   failureReason != 0 )
					mCallback->Failure(itemId, failureReason, transaction.error.code );
				else
					mCallback->Failure( "", "", -1 );
			}
		}
		else 
		{
			NSLog( @"User cancelled");
			
			if( mCallback != 0 )
			{
				const char* itemId = [transaction.originalTransaction.payment.productIdentifier cStringUsingEncoding:NSASCIIStringEncoding];
				
				if( itemId != 0 )
					mCallback->Failure( itemId, 0, -1 );
				else
					mCallback->Failure( "", 0, -1 );
			}
		}
	}
	
    [[SKPaymentQueue defaultQueue] finishTransaction: transaction];
	
}

/// store the transaction receipt
- (void) recordTransaction: (SKPaymentTransaction *)transaction keyVal:(NSString *)keyVal
{
	DBGLOG( "APPSTOREPURCHASES: recordTransaction\n" );
	
	if( transaction )
	{
		// can store the transaction RECEIPT locally
		[[NSUserDefaults standardUserDefaults] setValue: [NSData dataWithContentsOfURL:[[NSBundle mainBundle] appStoreReceiptURL]] forKey: keyVal];
		[[NSUserDefaults standardUserDefaults] synchronize];
	}
}

/// store the unlocked / enabled content
- (void) providePurchasedContent: (NSString *)productIdentifier
{
	DBGLOG( "APPSTOREPURCHASES: providePurchasedContent\n" );
	
	if( productIdentifier )
	{
		// store the unlock / enabled feature
		[[NSUserDefaults standardUserDefaults] setBool: true forKey: productIdentifier];
		[[NSUserDefaults standardUserDefaults] synchronize];	
		
		if( mCallback != 0 )
		{
			// convert string
			mCallback->Success( [productIdentifier cStringUsingEncoding:NSASCIIStringEncoding] );
		}
	}
}

/// store the unlocked / enabled content
- (void) provideRestoredContent: (NSString *)productIdentifier
{
	DBGLOG( "APPSTOREPURCHASES: provideRestoredContent\n" );
	
	if( productIdentifier )
	{
		// store the unlock / enabled feature
		[[NSUserDefaults standardUserDefaults] setBool: true forKey: productIdentifier];
		[[NSUserDefaults standardUserDefaults] synchronize];	
		
		if( mCallback != 0 )
		{
			// convert string
			mCallback->Restored( [productIdentifier cStringUsingEncoding:NSASCIIStringEncoding] );
		}
	}
}

////////////
- (void) setPurchaseCallback: (support::PurchaseCallback *)callback
{
	mCallback = callback;
}

/// request previously completed transactions
- (void) restoreNonConsumables
{
	DBGLOG( "APPSTOREPURCHASES: restoreNonConsumables\n" );
	
	[[SKPaymentQueue defaultQueue] restoreCompletedTransactions];
}

- (SKProduct*) findProductById: (NSString *)productIdentifier
{
	std::vector<SKProduct*>::iterator it = mProducts.begin();
	while( it != mProducts.end() )
	{
		if( [(*it).productIdentifier isEqualToString:productIdentifier] )
		{
			return *it;
		}
		it++;
	}
	
	return nil;
}

/// request a product purchase
- (void) purchaseContent: (NSString *)productIdentifier
{
#if (!TARGET_IPHONE_SIMULATOR)
	
	if( productIdentifier != nil )
	{
		// this will not work in the simulator
		SKProduct* product = [self findProductById:productIdentifier];
		if( product )
		{
			SKPayment* payment = [SKPayment paymentWithProduct:product];
			[[SKPaymentQueue defaultQueue] addPayment:payment];
		}
	}
	
	/*if( activityView )
	{
		UIWindow* window = [UIApplication sharedApplication].keyWindow;
		if (!window) 
			window = [[UIApplication sharedApplication].windows objectAtIndex:0];
		
		[window addSubview:activityView];
	
		[activityView setCenter: window.center ];
		
		//activityView.transform = CGAffineTransformScale(CGAffineTransformIdentity, 0.001, 0.001);
		[UIView beginAnimations:nil context:nil];
		[UIView setAnimationDuration:0.3/1.5];
		[UIView setAnimationDelegate:self];
		//[UIView setAnimationDidStopSelector:@selector(bounce1AnimationStopped)];
		//activityView.transform = CGAffineTransformScale(CGAffineTransformIdentity, 1.1, 1.1);
		[UIView commitAnimations];
		
		showingWaitView = true;
	}*/
#else	
	// just fail
	/*if( pPurchaseCallback != 0 )
	{
		// convert string
		const char* failureReason = "Purchasing does not work in the simulator";
		
		if( failureReason )
		{
			std::string failString( failureReason );
			pPurchaseCallback->purchaseFailed( failString, -1 );
		}
	}*/
#endif
}

/// removes the transaction from the queue and posts a notification with the transaction result
- (void)finishTransaction:(SKPaymentTransaction *)transaction wasSuccessful:(bool)wasSuccessful
{
    // remove the transaction from the payment queue.
    [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
} 

/// check the local dictionary of the app for stored keys
- (bool) hasPurchasedContent: (const char*)productIdentifier
{
	if( productIdentifier == 0 )
		return false;
	
	NSString* convertedString = [NSString stringWithUTF8String: productIdentifier];
	
	return( [[NSUserDefaults standardUserDefaults] boolForKey:convertedString] );
}

/// check the local dictionary of the app for stored keys
- (bool) isValidToPurchase: (const char*)productIdentifier
{
	if( productIdentifier == 0 )
		return false;
	
	// no contact from store
	if( !mHasResponseFromStore || mPurchaseList.size() == 0 )
		return false;
	
	std::vector<support::TPurchase>::const_iterator it = mPurchaseList.begin();
	while( it != mPurchaseList.end() )
	{
		if( (*it).isValid )
		{
			if( strcmp( (*it).purchaseId, productIdentifier ) == 0 )
				return true;
		}
		
		// next
		++it;
	}
	
	return false;
	
}

/// has the store given the purchases back
- (bool) hasStoreResponse
{
	return mHasResponseFromStore;
}

@end // AppStorePurchaseManager

/////////////////////////////////////////////////////
/// Function: IsAppStoreAvailable
/// Params: None
///
/////////////////////////////////////////////////////
bool IsAppStoreAvailable()
{
	// check for presence of SKProduct API
	Class productClass = (NSClassFromString(@"SKProduct"));
	
	// check if the device is running iOS 3.0 or later
	NSString *reqSysVer = @"3.0";
	NSString *currSysVer = [[UIDevice currentDevice] systemVersion];
	bool osVersionSupported = ([currSysVer compare:reqSysVer options:NSNumericSearch] != NSOrderedAscending);
	
	return(productClass && osVersionSupported);	
}

#endif // BASE_PLATFORM_iOS

