
/*===================================================================
	File: CustomerAudio.cpp
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "SoundBase.h"
#include "CollisionBase.h"
#include "Model/ModelCommon.h"
#include "ModelBase.h"
#include "InputBase.h"
#include "ScriptBase.h"

#include "Audio/AudioSystem.h"
#include "Audio/CustomerAudio.h"

namespace
{
	struct CustomerAudioData
	{
		unsigned int soundEnum;
		ALuint bufferId;	
		int resId;

		//const char* filename;
	};

	CustomerAudioData CustomerAudio_MaleAmericanData[] = 
	{
		{ CustomerAudio::REQUEST_1,			snd::INVALID_SOUNDBUFFER, 300 },
		{ CustomerAudio::REQUEST_2,			snd::INVALID_SOUNDBUFFER, 301 },
		{ CustomerAudio::REQUEST_3,			snd::INVALID_SOUNDBUFFER, 302 },
		{ CustomerAudio::REQUEST_4,			snd::INVALID_SOUNDBUFFER, 303 },
		{ CustomerAudio::REQUEST_5,			snd::INVALID_SOUNDBUFFER, 304 },
		{ CustomerAudio::REQUEST_6,			snd::INVALID_SOUNDBUFFER, 305 },
		{ CustomerAudio::REQUEST_7,			snd::INVALID_SOUNDBUFFER, 306 },
		{ CustomerAudio::REQUEST_8,			snd::INVALID_SOUNDBUFFER, 307 },
		{ CustomerAudio::REQUEST_9,			snd::INVALID_SOUNDBUFFER, 308 },

		{ CustomerAudio::REQUEST_NUMBER1,		snd::INVALID_SOUNDBUFFER, 309 },
		{ CustomerAudio::REQUEST_NUMBER2,		snd::INVALID_SOUNDBUFFER, 310 },
		{ CustomerAudio::REQUEST_NUMBER3,		snd::INVALID_SOUNDBUFFER, 311 },
		{ CustomerAudio::REQUEST_NUMBER4,		snd::INVALID_SOUNDBUFFER, 312 },
		{ CustomerAudio::REQUEST_NUMBER5,		snd::INVALID_SOUNDBUFFER, 313 },
		{ CustomerAudio::REQUEST_NUMBER6,		snd::INVALID_SOUNDBUFFER, 314 },
		{ CustomerAudio::REQUEST_NUMBER7,		snd::INVALID_SOUNDBUFFER, 315 },
		{ CustomerAudio::REQUEST_NUMBER8,		snd::INVALID_SOUNDBUFFER, 316 },
		{ CustomerAudio::REQUEST_NUMBER9,		snd::INVALID_SOUNDBUFFER, 317 },

		{ CustomerAudio::HAIL_1,		snd::INVALID_SOUNDBUFFER, 320 },
		{ CustomerAudio::HAIL_2,		snd::INVALID_SOUNDBUFFER, 321 },
		{ CustomerAudio::HAIL_3,		snd::INVALID_SOUNDBUFFER, 322 },
		{ CustomerAudio::HAIL_4,		snd::INVALID_SOUNDBUFFER, 323 },
		{ CustomerAudio::HAIL_5,		snd::INVALID_SOUNDBUFFER, 324 },
		{ CustomerAudio::HAIL_6,		snd::INVALID_SOUNDBUFFER, 325 },
		{ CustomerAudio::HAIL_7,		snd::INVALID_SOUNDBUFFER, 326 },

		{ CustomerAudio::END_1,			snd::INVALID_SOUNDBUFFER, 318 },
		{ CustomerAudio::END_2,			snd::INVALID_SOUNDBUFFER, 319 }

	};

	CustomerAudioData CustomerAudio_MaleCockneyData[] = 
	{
		{ CustomerAudio::REQUEST_1,			snd::INVALID_SOUNDBUFFER, 350 },
		{ CustomerAudio::REQUEST_2,			snd::INVALID_SOUNDBUFFER, 351 },
		{ CustomerAudio::REQUEST_3,			snd::INVALID_SOUNDBUFFER, 352 },
		{ CustomerAudio::REQUEST_4,			snd::INVALID_SOUNDBUFFER, 353 },
		{ CustomerAudio::REQUEST_5,			snd::INVALID_SOUNDBUFFER, 354 },
		{ CustomerAudio::REQUEST_6,			snd::INVALID_SOUNDBUFFER, 355 },
		{ CustomerAudio::REQUEST_7,			snd::INVALID_SOUNDBUFFER, 356 },
		{ CustomerAudio::REQUEST_8,			snd::INVALID_SOUNDBUFFER, 357 },
		{ CustomerAudio::REQUEST_9,			snd::INVALID_SOUNDBUFFER, 358 },

		{ CustomerAudio::REQUEST_NUMBER1,		snd::INVALID_SOUNDBUFFER, 359 },
		{ CustomerAudio::REQUEST_NUMBER2,		snd::INVALID_SOUNDBUFFER, 360 },
		{ CustomerAudio::REQUEST_NUMBER3,		snd::INVALID_SOUNDBUFFER, 361 },
		{ CustomerAudio::REQUEST_NUMBER4,		snd::INVALID_SOUNDBUFFER, 362 },
		{ CustomerAudio::REQUEST_NUMBER5,		snd::INVALID_SOUNDBUFFER, 363 },
		{ CustomerAudio::REQUEST_NUMBER6,		snd::INVALID_SOUNDBUFFER, 364 },
		{ CustomerAudio::REQUEST_NUMBER7,		snd::INVALID_SOUNDBUFFER, 365 },
		{ CustomerAudio::REQUEST_NUMBER8,		snd::INVALID_SOUNDBUFFER, 366 },
		{ CustomerAudio::REQUEST_NUMBER9,		snd::INVALID_SOUNDBUFFER, 367 },

		{ CustomerAudio::HAIL_1,		snd::INVALID_SOUNDBUFFER, 370 },
		{ CustomerAudio::HAIL_2,		snd::INVALID_SOUNDBUFFER, 371 },
		{ CustomerAudio::HAIL_3,		snd::INVALID_SOUNDBUFFER, 372 },
		{ CustomerAudio::HAIL_4,		snd::INVALID_SOUNDBUFFER, 373 },
		{ CustomerAudio::HAIL_5,		snd::INVALID_SOUNDBUFFER, 374 },
		{ CustomerAudio::HAIL_6,		snd::INVALID_SOUNDBUFFER, 375 },
		{ CustomerAudio::HAIL_7,		snd::INVALID_SOUNDBUFFER, 376 },

		{ CustomerAudio::END_1,			snd::INVALID_SOUNDBUFFER, 368 },
		{ CustomerAudio::END_2,			snd::INVALID_SOUNDBUFFER, 369 }
	};

	CustomerAudioData CustomerAudio_FemaleAmericanData[] = 
	{
		{ CustomerAudio::REQUEST_1,			snd::INVALID_SOUNDBUFFER, 200 },
		{ CustomerAudio::REQUEST_2,			snd::INVALID_SOUNDBUFFER, 201 },
		{ CustomerAudio::REQUEST_3,			snd::INVALID_SOUNDBUFFER, 202 },
		{ CustomerAudio::REQUEST_4,			snd::INVALID_SOUNDBUFFER, 203 },
		{ CustomerAudio::REQUEST_5,			snd::INVALID_SOUNDBUFFER, 204 },
		{ CustomerAudio::REQUEST_6,			snd::INVALID_SOUNDBUFFER, 205 },
		{ CustomerAudio::REQUEST_7,			snd::INVALID_SOUNDBUFFER, 206 },
		{ CustomerAudio::REQUEST_8,			snd::INVALID_SOUNDBUFFER, 207 },
		{ CustomerAudio::REQUEST_9,			snd::INVALID_SOUNDBUFFER, 208 },

		{ CustomerAudio::REQUEST_NUMBER1,		snd::INVALID_SOUNDBUFFER, 209 },
		{ CustomerAudio::REQUEST_NUMBER2,		snd::INVALID_SOUNDBUFFER, 210 },
		{ CustomerAudio::REQUEST_NUMBER3,		snd::INVALID_SOUNDBUFFER, 211 },
		{ CustomerAudio::REQUEST_NUMBER4,		snd::INVALID_SOUNDBUFFER, 212 },
		{ CustomerAudio::REQUEST_NUMBER5,		snd::INVALID_SOUNDBUFFER, 213 },
		{ CustomerAudio::REQUEST_NUMBER6,		snd::INVALID_SOUNDBUFFER, 214 },
		{ CustomerAudio::REQUEST_NUMBER7,		snd::INVALID_SOUNDBUFFER, 215 },
		{ CustomerAudio::REQUEST_NUMBER8,		snd::INVALID_SOUNDBUFFER, 216 },
		{ CustomerAudio::REQUEST_NUMBER9,		snd::INVALID_SOUNDBUFFER, 217 },

		{ CustomerAudio::HAIL_1,		snd::INVALID_SOUNDBUFFER, 220 },
		{ CustomerAudio::HAIL_2,		snd::INVALID_SOUNDBUFFER, 221 },
		{ CustomerAudio::HAIL_3,		snd::INVALID_SOUNDBUFFER, 222 },
		{ CustomerAudio::HAIL_4,		snd::INVALID_SOUNDBUFFER, 223 },
		{ CustomerAudio::HAIL_5,		snd::INVALID_SOUNDBUFFER, 224 },
		{ CustomerAudio::HAIL_6,		snd::INVALID_SOUNDBUFFER, 225 },
		{ CustomerAudio::HAIL_7,		snd::INVALID_SOUNDBUFFER, 226 },

		{ CustomerAudio::END_1,				snd::INVALID_SOUNDBUFFER, 218 },
		{ CustomerAudio::END_2,				snd::INVALID_SOUNDBUFFER, 219 }
	};

	CustomerAudioData CustomerAudio_FemaleCockneyData[] = 
	{
		{ CustomerAudio::REQUEST_1,			snd::INVALID_SOUNDBUFFER, 250 },
		{ CustomerAudio::REQUEST_2,			snd::INVALID_SOUNDBUFFER, 251 },
		{ CustomerAudio::REQUEST_3,			snd::INVALID_SOUNDBUFFER, 252 },
		{ CustomerAudio::REQUEST_4,			snd::INVALID_SOUNDBUFFER, 253 },
		{ CustomerAudio::REQUEST_5,			snd::INVALID_SOUNDBUFFER, 254 },
		{ CustomerAudio::REQUEST_6,			snd::INVALID_SOUNDBUFFER, 255 },
		{ CustomerAudio::REQUEST_7,			snd::INVALID_SOUNDBUFFER, 256 },
		{ CustomerAudio::REQUEST_8,			snd::INVALID_SOUNDBUFFER, 257 },
		{ CustomerAudio::REQUEST_9,			snd::INVALID_SOUNDBUFFER, 258 },

		{ CustomerAudio::REQUEST_NUMBER1,		snd::INVALID_SOUNDBUFFER, 259 },
		{ CustomerAudio::REQUEST_NUMBER2,		snd::INVALID_SOUNDBUFFER, 260 },
		{ CustomerAudio::REQUEST_NUMBER3,		snd::INVALID_SOUNDBUFFER, 261 },
		{ CustomerAudio::REQUEST_NUMBER4,		snd::INVALID_SOUNDBUFFER, 262 },
		{ CustomerAudio::REQUEST_NUMBER5,		snd::INVALID_SOUNDBUFFER, 263 },
		{ CustomerAudio::REQUEST_NUMBER6,		snd::INVALID_SOUNDBUFFER, 264 },
		{ CustomerAudio::REQUEST_NUMBER7,		snd::INVALID_SOUNDBUFFER, 265 },
		{ CustomerAudio::REQUEST_NUMBER8,		snd::INVALID_SOUNDBUFFER, 266 },
		{ CustomerAudio::REQUEST_NUMBER9,		snd::INVALID_SOUNDBUFFER, 267 },

		{ CustomerAudio::HAIL_1,		snd::INVALID_SOUNDBUFFER, 270 },
		{ CustomerAudio::HAIL_2,		snd::INVALID_SOUNDBUFFER, 271 },
		{ CustomerAudio::HAIL_3,		snd::INVALID_SOUNDBUFFER, 272 },
		{ CustomerAudio::HAIL_4,		snd::INVALID_SOUNDBUFFER, 273 },
		{ CustomerAudio::HAIL_5,		snd::INVALID_SOUNDBUFFER, 274 },
		{ CustomerAudio::HAIL_6,		snd::INVALID_SOUNDBUFFER, 275 },
		{ CustomerAudio::HAIL_7,		snd::INVALID_SOUNDBUFFER, 276 },

		{ CustomerAudio::END_1,			snd::INVALID_SOUNDBUFFER, 268 },
		{ CustomerAudio::END_2,			snd::INVALID_SOUNDBUFFER, 269 }
	};

	// active accent
	CustomerAudio::eCustomerAccent CurrentCustomerAccent = CustomerAudio::CUSTOMER_AMERICAN;
}

/////////////////////////////////////////////////////
/// Function: LoadCustomerAudio
/// Params: None
///
/////////////////////////////////////////////////////
void LoadCustomerAudio()
{
	int i=0;

	// male american
	for( i=0; i < (sizeof(CustomerAudio_MaleAmericanData)/sizeof(CustomerAudioData)); ++i )
	{
		CustomerAudio_MaleAmericanData[i].bufferId = AudioSystem::GetInstance()->AddAudioFile( CustomerAudio_MaleAmericanData[i].resId );
	}
	// male cockney
	for( i=0; i < (sizeof(CustomerAudio_MaleCockneyData)/sizeof(CustomerAudioData)); ++i )
	{
		CustomerAudio_MaleCockneyData[i].bufferId = AudioSystem::GetInstance()->AddAudioFile( CustomerAudio_MaleCockneyData[i].resId );
	}

	// female american
	for( i=0; i < (sizeof(CustomerAudio_FemaleAmericanData)/sizeof(CustomerAudioData)); ++i )
	{
		CustomerAudio_FemaleAmericanData[i].bufferId = AudioSystem::GetInstance()->AddAudioFile( CustomerAudio_FemaleAmericanData[i].resId );
	}

	// female cockney
	for( i=0; i < (sizeof(CustomerAudio_FemaleCockneyData)/sizeof(CustomerAudioData)); ++i )
	{
		CustomerAudio_FemaleCockneyData[i].bufferId = AudioSystem::GetInstance()->AddAudioFile( CustomerAudio_FemaleCockneyData[i].resId );
	}
}

/////////////////////////////////////////////////////
/// Function: SetCustomerAccent
/// Params: [in]accent
///
/////////////////////////////////////////////////////
void SetCustomerAccent( CustomerAudio::eCustomerAccent accent )
{
	CurrentCustomerAccent = accent;
}

/////////////////////////////////////////////////////
/// Function: GetCustomerAccent
/// Params: None
///
/////////////////////////////////////////////////////
CustomerAudio::eCustomerAccent GetCustomerAccent( )
{
	return(CurrentCustomerAccent);
}

/////////////////////////////////////////////////////
/// Function: PlayCustomerRequestAudio
/// Params: [in]number, [in]customer
///
/////////////////////////////////////////////////////
void PlayCustomerRequestAudio( int number, Customer& customer )
{
	DBG_ASSERT( (number >= 1 && number <= 9) );

	int actualIndex = number-1;

	ALuint soundId = snd::INVALID_SOUNDSOURCE;
	ALuint bufferId = snd::INVALID_SOUNDBUFFER;

	soundId = snd::SoundManager::GetInstance()->GetFreeSoundSource();
	if( soundId != snd::INVALID_SOUNDSOURCE )
	{
		// find the correct buffer
		int randLookup = math::RandomNumber( 0, 1 );
		int whichRequest = CustomerAudio::REQUEST_1;

		// pick a random type
		if( randLookup == 0 )
		{
			whichRequest = CustomerAudio::REQUEST_1+actualIndex;
		}
		else
		{
			whichRequest = CustomerAudio::REQUEST_NUMBER1+actualIndex;
		}

		if( customer.GetGender() == Customer::CUSTOMER_MALE )
		{
			// male
			if( CurrentCustomerAccent == CustomerAudio::CUSTOMER_AMERICAN )
				bufferId = CustomerAudio_MaleAmericanData[whichRequest].bufferId;
			else
				bufferId = CustomerAudio_MaleCockneyData[whichRequest].bufferId;
		}
		else
		{
			// female
			if( CurrentCustomerAccent == CustomerAudio::CUSTOMER_AMERICAN )
				bufferId = CustomerAudio_FemaleAmericanData[whichRequest].bufferId;
			else
				bufferId = CustomerAudio_FemaleCockneyData[whichRequest].bufferId;
		}

		if( bufferId != snd::INVALID_SOUNDBUFFER )
		{
			AudioSystem::GetInstance()->PlayAudio( soundId, bufferId, math::Vec3(0.0f,0.0f,0.0f), AL_TRUE, AL_FALSE, 1.0f+customer.GetVoicePitchTweak(), 1.0f );
		}
	}
}

/////////////////////////////////////////////////////
/// Function: PlayCustomerHailAudio
/// Params: [in]customer
///
/////////////////////////////////////////////////////
void PlayCustomerHailAudio( Customer& customer )
{
	ALuint soundId = snd::INVALID_SOUNDSOURCE;
	ALuint bufferId = snd::INVALID_SOUNDBUFFER;

	soundId = snd::SoundManager::GetInstance()->GetFreeSoundSource();
	if( soundId != snd::INVALID_SOUNDSOURCE )
	{
		// find the correct buffer
		int randLookup = math::RandomNumber( CustomerAudio::HAIL_1, CustomerAudio::HAIL_7 );

		if( customer.GetGender() == Customer::CUSTOMER_MALE )
		{
			// male
			if( CurrentCustomerAccent == CustomerAudio::CUSTOMER_AMERICAN )
				bufferId = CustomerAudio_MaleAmericanData[randLookup].bufferId;
			else
				bufferId = CustomerAudio_MaleCockneyData[randLookup].bufferId;
		}
		else
		{
			// female
			if( CurrentCustomerAccent == CustomerAudio::CUSTOMER_AMERICAN )
				bufferId = CustomerAudio_FemaleAmericanData[randLookup].bufferId;
			else
				bufferId = CustomerAudio_FemaleCockneyData[randLookup].bufferId;
		}

		if( bufferId != snd::INVALID_SOUNDBUFFER )
		{
			AudioSystem::GetInstance()->PlayAudio( soundId, bufferId, math::Vec3(0.0f,0.0f,0.0f), AL_TRUE, AL_FALSE, 1.0f+customer.GetVoicePitchTweak(), 1.0f );
		}
	}
}

/////////////////////////////////////////////////////
/// Function: PlayCustomerEndAudio
/// Params: [in]customer
///
/////////////////////////////////////////////////////
void PlayCustomerEndAudio( Customer& customer, bool isParcel )
{
	ALuint soundId = snd::INVALID_SOUNDSOURCE;
	ALuint bufferId = snd::INVALID_SOUNDBUFFER;

	soundId = snd::SoundManager::GetInstance()->GetFreeSoundSource();
	if( soundId != snd::INVALID_SOUNDSOURCE )
	{
		// find the correct buffer
		int randLookup = CustomerAudio::END_1;

		if( !isParcel )
			randLookup = math::RandomNumber( CustomerAudio::END_1, CustomerAudio::END_2 );

		if( customer.GetGender() == Customer::CUSTOMER_MALE )
		{
			// male
			if( CurrentCustomerAccent == CustomerAudio::CUSTOMER_AMERICAN )
				bufferId = CustomerAudio_MaleAmericanData[randLookup].bufferId;
			else
				bufferId = CustomerAudio_MaleCockneyData[randLookup].bufferId;
		}
		else
		{
			// female
			if( CurrentCustomerAccent == CustomerAudio::CUSTOMER_AMERICAN )
				bufferId = CustomerAudio_FemaleAmericanData[randLookup].bufferId;
			else
				bufferId = CustomerAudio_FemaleCockneyData[randLookup].bufferId;
		}

		if( bufferId != snd::INVALID_SOUNDBUFFER )
		{
			AudioSystem::GetInstance()->PlayAudio( soundId, bufferId, math::Vec3(0.0f,0.0f,0.0f), AL_TRUE, AL_FALSE, 1.0f+customer.GetVoicePitchTweak(), 1.0f );
		}
	}
}
