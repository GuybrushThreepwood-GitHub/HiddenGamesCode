

/*===================================================================
	File: Endian.cpp
	Library: Core

	(C)Hidden Games
=====================================================================*/

#include "Core/CoreDefines.h"

#include "Debug/Assertion.h"
#include "Debug/DebugLogging.h"
#include "Core/Endian.h"

core::EMachineEndian core::eMachineEndian = core::MACHINE_LITTLE_ENDIAN;

/////////////////////////////////////////////////////
/// Function: EndianCheck
/// Params: None
///
/////////////////////////////////////////////////////
void core::EndianCheck( void )
{
	unsigned char	swaptest[2] = {1,0};

	DBGLOG( "ENDIANCHECK: BOOL Size:					%lu\n", sizeof(bool) );
	DBGLOG( "ENDIANCHECK: CHAR Size:					%lu\n", sizeof(char) );
	DBGLOG( "ENDIANCHECK: UNSIGNED CHAR Size:		%lu\n", sizeof(unsigned char) );
	DBGLOG( "ENDIANCHECK: SIGNED CHAR Size:			%lu\n", sizeof(signed char) );
	DBGLOG( "ENDIANCHECK: INT Size:					%lu\n", sizeof(int) );
	DBGLOG( "ENDIANCHECK: UNSIGNED INT Size:			%lu\n", sizeof(unsigned int) );
	DBGLOG( "ENDIANCHECK: SIGNED INT Size:			%lu\n", sizeof(signed int) );
	DBGLOG( "ENDIANCHECK: SHORT INT Size:			%lu\n", sizeof(short int) );
	DBGLOG( "ENDIANCHECK: UNSIGNED SHORT INT Size:	%lu\n", sizeof(unsigned short int) );
	DBGLOG( "ENDIANCHECK: SIGNED SHORT INT Size:		%lu\n", sizeof(signed short int) );
	DBGLOG( "ENDIANCHECK: UNSIGNED LONG:				%lu\n", sizeof(unsigned long) );
	DBGLOG( "ENDIANCHECK: LONG INT Size:				%lu\n", sizeof(long int) );
	DBGLOG( "ENDIANCHECK: SIGNED LONG INT Size:		%lu\n", sizeof(signed long int) );
	DBGLOG( "ENDIANCHECK: UNSIGNED LONG INT Size:	%lu\n", sizeof(unsigned long int) );
	DBGLOG( "ENDIANCHECK: FLOAT Size:				%lu\n", sizeof(float) );
	DBGLOG( "ENDIANCHECK: DOUBLE Size:				%lu\n", sizeof(double) );
	DBGLOG( "ENDIANCHECK: LONG DOUBLE Size:			%lu\n", sizeof(long double) );

	// see how this number is stored
	if( *(short *)swaptest == 1 )
	{
		// x86
		eMachineEndian = core::MACHINE_LITTLE_ENDIAN;
		DBGLOG( "ENDIANCHECK: Machine is little endian\n" );
	}
	else
	{
		// powerpc
		eMachineEndian = core::MACHINE_BIG_ENDIAN;
		DBGLOG( "ENDIANCHECK: Machine is big endian\n" );
	}
}

/////////////////////////////////////////////////////
/// Function: LittleToBigEndianShort
/// Params: [in]littleEndian
///
/////////////////////////////////////////////////////
unsigned short core::LittleToBigEndianShort( unsigned short littleEndian )
{
	if( eMachineEndian == MACHINE_BIG_ENDIAN )
	{
		unsigned char b1, b2;

		b1 = littleEndian & 255;
		b2 = (littleEndian >> 8) & 255;

		return( (b1 << 8) + b2 );
	}
	else
		return( littleEndian );
}

/////////////////////////////////////////////////////
/// Function: LittleToBigEndianInt
/// Params: [in]littleEndian
///
/////////////////////////////////////////////////////
unsigned int core::LittleToBigEndianInt( unsigned int littleEndian )
{
	if( core::eMachineEndian == core::MACHINE_BIG_ENDIAN )
	{
		unsigned char b1, b2, b3, b4;

		b1 = littleEndian & 255;
		b2 = (littleEndian>>8) & 255;
		b3 = (littleEndian>>16) & 255;
		b4 = (littleEndian>>24) & 255;

		return( ((int)b1<<24) + ((int)b2<<16) + ((int)b3<<8) + b4 );
	}
	else
		return( littleEndian );
}

/////////////////////////////////////////////////////
/// Function: LittleToBigEndianFloat
/// Params: [in]LittleToBigEndianFloat
///
/////////////////////////////////////////////////////
float core::LittleToBigEndianFloat( float littleEndian )
{
	if( core::eMachineEndian == core::MACHINE_BIG_ENDIAN )
	{
		union
		{
			float f;
			unsigned char b[4];
		} dat1, dat2;

		dat1.f = littleEndian;
		dat2.b[0] = dat1.b[3];
		dat2.b[1] = dat1.b[2];
		dat2.b[2] = dat1.b[1];
		dat2.b[3] = dat1.b[0];

		return( dat2.f );
	}
	else
		return( littleEndian );
}

/////////////////////////////////////////////////////
/// Function: EndianSwapShort
/// Params: [in]nEndianValue, [in]eEndian
///
/////////////////////////////////////////////////////
unsigned short core::EndianSwapShort( unsigned short nEndianValue, core::EMachineEndian eEndian )
{
	if( core::eMachineEndian == eEndian )
	{
		// value should be in the correct layout
		return( nEndianValue );
	}
	else
	{
		// do a conversion
		unsigned char b1, b2;

		b1 = nEndianValue & 255;
		b2 = (nEndianValue >> 8) & 255;

		return( (b1 << 8) + b2 );
	}	
}

/////////////////////////////////////////////////////
/// Function: EndianSwapInt
/// Params: [in]nEndianValue, [in]eEndian
///
/////////////////////////////////////////////////////
unsigned int core::EndianSwapInt( unsigned int nEndianValue, core::EMachineEndian eEndian )
{
	if( core::eMachineEndian == eEndian )
	{
		// value should be in the correct layout
		return( nEndianValue );
	}
	else
	{
		// do a conversion
		unsigned char b1, b2, b3, b4;

		b1 = nEndianValue & 255;
		b2 = (nEndianValue>>8) & 255;
		b3 = (nEndianValue>>16) & 255;
		b4 = (nEndianValue>>24) & 255;

		return( ((int)b1<<24) + ((int)b2<<16) + ((int)b3<<8) + b4 );
	}
}

/////////////////////////////////////////////////////
/// Function: EndianSwapFloat
/// Params: [in]fEndianValue, [in]eEndian
///
/////////////////////////////////////////////////////
float core::EndianSwapFloat( float fEndianValue, core::EMachineEndian eEndian )
{
	if( core::eMachineEndian == eEndian )
	{
		// value should be in the correct layout
		return( fEndianValue );
	}
	else
	{
		// do a conversion
		union
		{
			float f;
			unsigned char b[4];
		} dat1, dat2;

		dat1.f = fEndianValue;
		dat2.b[0] = dat1.b[3];
		dat2.b[1] = dat1.b[2];
		dat2.b[2] = dat1.b[1];
		dat2.b[3] = dat1.b[0];

		return( dat2.f );
	}
}

