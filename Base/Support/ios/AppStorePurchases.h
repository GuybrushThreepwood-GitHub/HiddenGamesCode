
#ifdef BASE_PLATFORM_iOS

#import <Foundation/Foundation.h>
#import <StoreKit/StoreKit.h>
#include <vector>
#include "Support/PurchaseCommon.h"

/// AppStorePurchases - Manages the app store requests for purchasable products
@interface AppStorePurchases : NSObject <SKProductsRequestDelegate, SKPaymentTransactionObserver>
{
	std::vector<SKProduct*> mProducts;
	SKProductsRequest* productsRequested;
	
	support::PurchaseCallback* mCallback;
	bool mHasResponseFromStore;
	std::vector<support::TPurchase> mPurchaseList;
}

- (void) initialise;
- (void) requestApplicationProducts: (NSSet *)productRequestList;
- (bool) canMakePayments;

// product requests
- (void) productsRequest:(SKProductsRequest *)request didReceiveResponse:(SKProductsResponse *)response;

// store observer
- (void)paymentQueue:(SKPaymentQueue *)queue updatedTransactions:(NSArray *)transactions;
//- (void)paymentQueue:(SKPaymentQueue *)queue removedTransactions:(NSArray *)transactions;

- (void)paymentQueue:(SKPaymentQueue *)queue restoreCompletedTransactionsFailedWithError:(NSError *)error;
- (void)paymentQueueRestoreCompletedTransactionsFinished:(SKPaymentQueue *)queue;

// callbacks
- (void) completeTransaction: (SKPaymentTransaction *)transaction;
- (void) failedTransaction: (SKPaymentTransaction *)transaction;
- (void) restoreTransaction: (SKPaymentTransaction *)transaction;

- (void) finishTransaction: (SKPaymentTransaction *)transaction wasSuccessful:(bool)wasSuccessful;
- (void) recordTransaction: (SKPaymentTransaction *)transaction keyVal:(NSString *)keyVal;
- (void) providePurchasedContent: (NSString *)productIdentifier;
- (void) provideRestoredContent: (NSString *)productIdentifier;

// user calls
- (void) setPurchaseCallback: (support::PurchaseCallback *)callback;
- (void) restoreNonConsumables;

- (void) purchaseContent: (NSString *)productIdentifier;
- (bool) hasPurchasedContent: (const char *)productIdentifier;
- (bool) isValidToPurchase: (const char *)productIdentifier;

- (bool) hasStoreResponse;

@property (nonatomic, strong) SKProductsRequest* productsRequested;

@end // AppStorePurchases


//////////////////
bool IsAppStoreAvailable();

#endif // BASE_PLATFORM_iOS


