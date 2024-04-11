
#ifndef __PURCHASECOMMON_H__
#define __PURCHASECOMMON_H__

const int MAX_PURCHASE_ID_LEN = 256;

namespace support
{	
	class PurchaseCallback
	{
		public:
			PurchaseCallback() {}
			virtual ~PurchaseCallback() {}

			virtual void Success( const char* purchaseId ) = 0;
			virtual void Failure( const char* purchaseId, const char* errorString, int errorId ) = 0;
			virtual void Restored( const char* purchaseId ) = 0;
			virtual void RestoreComplete() = 0;
			virtual void RestoreFailed( int errorId ) = 0;
		
		private:
	};
	
	struct TPurchase
	{
		bool isValid;
		char purchaseId[MAX_PURCHASE_ID_LEN];
	};
	
} // namespace support

#endif // __PURCHASECOMMON_H__

