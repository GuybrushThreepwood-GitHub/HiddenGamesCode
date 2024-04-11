
/*===================================================================
	File: RandomTables.cpp
	Library: Math

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"

#include <cstdlib>
#include <cmath>
#include <ctime>

#include "Math/RandomTables.h"

/////////////////////////////////////////////////////
/// Function: InitRandomSeed
/// Params: None
///
/////////////////////////////////////////////////////
void math::InitRandomSeed( void )
{
	//int i = 0;

	std::srand( static_cast<unsigned int>( time(0) ) );
}

/////////////////////////////////////////////////////
/// Function: RandomNumber
/// Params: [in]min, [in]max
///
/////////////////////////////////////////////////////
int math::RandomNumber(int iMin, int iMax)
{
	if (iMin == iMax) 
		return(iMin);
	
	return((std::rand() % (abs(iMax-iMin)+1))+iMin);
}

/////////////////////////////////////////////////////
/// Function: RandomNumber
/// Params: [in]min, [in]max
///
/////////////////////////////////////////////////////
float math::RandomNumber(float fMin, float fMax)
{
	if (fMin == fMax) 
		return(fMin);

	float fRandom = (float)std::rand() / (float)RAND_MAX;
	
	return((fRandom * std::abs(fMax-fMin))+fMin);
}
