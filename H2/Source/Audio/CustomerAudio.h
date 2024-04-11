
/*===================================================================
	File: CustomerAudio.h
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#ifndef __CUSTOMERAUDIO_H__
#define __CUSTOMERAUDIO_H__

#include "Customers/Customer.h"

namespace CustomerAudio
{
	enum eCustomerAccent
	{
		CUSTOMER_AMERICAN=0,
		CUSTOMER_COCKNEY
	};

	enum eAudioIds
	{
		REQUEST_1=0,
		REQUEST_2,
		REQUEST_3,
		REQUEST_4,
		REQUEST_5,
		REQUEST_6,
		REQUEST_7,
		REQUEST_8,
		REQUEST_9,

		REQUEST_NUMBER1,
		REQUEST_NUMBER2,
		REQUEST_NUMBER3,
		REQUEST_NUMBER4,
		REQUEST_NUMBER5,
		REQUEST_NUMBER6,
		REQUEST_NUMBER7,
		REQUEST_NUMBER8,
		REQUEST_NUMBER9,

		HAIL_1,
		HAIL_2,
		HAIL_3,
		HAIL_4,
		HAIL_5,
		HAIL_6,
		HAIL_7,

		END_1,
		END_2
	};
}

void LoadCustomerAudio( );

void SetCustomerAccent( CustomerAudio::eCustomerAccent accent );

CustomerAudio::eCustomerAccent GetCustomerAccent( );

void PlayCustomerRequestAudio( int number, Customer& customer );

void PlayCustomerHailAudio( Customer& customer );

void PlayCustomerEndAudio( Customer& customer, bool isParcel=false );

#endif // __CUSTOMERAUDIO_H__
