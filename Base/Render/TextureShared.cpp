
/*===================================================================
	File: TextureShared.cpp
	Library: Render

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"

#include <cstring>
#include <cmath>

#include "Math/Vectors.h"

#include "Render/OpenGLCommon.h"

#include "Render/TextureShared.h"
#include "Render/Texture.h"

/////////////////////////////////////////////////////
/// Function: IsPowerOfTwo
/// Params: [in]nValue
///
/////////////////////////////////////////////////////
bool renderer::IsPowerOfTwo( int nValue )
{
	const int POW2_TABLE_SIZE = 16;

	int i = 0;
	int nTable[POW2_TABLE_SIZE];

	for( i = 0; i < POW2_TABLE_SIZE; i++ )
		nTable[i] = static_cast<int>( std::pow( 2.0, (double)i ) );

	for( i = 0; i < POW2_TABLE_SIZE; i++ )
	{
		if( nValue == nTable[i] )
			return(true);
	}

	return(false);
}

/////////////////////////////////////////////////////
/// Function: GetPreviousPowerOfTwo
/// Params: [in]nValue, [in]nMaxSize
///
/////////////////////////////////////////////////////
int renderer::GetPreviousPowerOfTwo( int nValue, int nMaxSize )
{
    int result = nMaxSize;

	// the loop below will never be low enough so set it here
	if( nValue > nMaxSize )
		return( nMaxSize );

	// shift until the loop fails
    while(result >= nValue)
    {
        result >>= 1;
    }

    return result;
}

/////////////////////////////////////////////////////
/// Function: GetNextPowerOfTwo
/// Params: [in]nValue, [in]nMaxSize
///
/////////////////////////////////////////////////////
int renderer::GetNextPowerOfTwo( int nValue, int nMaxSize )
{
    int result = 1;

	// don't go higher than supported
	if( nValue > nMaxSize )
		return( nMaxSize );

	if( nValue <= result )
		return( 2 );

	// shift until the loop fails
    while(result <= nValue)
    {
        result <<= 1;
    }

    return result;
}

/////////////////////////////////////////////////////
/// Function: GetUVCoord
/// Params: [in]nPixelCoord, [in]nTextureSize
///
/////////////////////////////////////////////////////
float renderer::GetUVCoord( int nPixelCoord, int nTextureSize )
{
	if( nPixelCoord > nTextureSize )
	{
		DBGLOG( "*WARNING* texture coord request for pixel bigger than texture size\n" );
		return(0.0f);
	}

	return(static_cast<float>(nPixelCoord) / static_cast<float>(nTextureSize));
}

#ifdef BASE_SUPPORT_DDS
/////////////////////////////////////////////////////
/// Function: FlipTexture_DXTC1
/// Params: [in]ptr, [in]numBlocks
///
/////////////////////////////////////////////////////
void renderer::FlipTexture_DXTC1( unsigned char *ptr, unsigned int numBlocks )
{
	renderer::TDXTColBlock *curblock = reinterpret_cast<renderer::TDXTColBlock *>(ptr);
	unsigned char temp;

	for( unsigned int i = 0; i < numBlocks; i++ ) 
	{
		temp = curblock->row[0];
		curblock->row[0] = curblock->row[3];
		curblock->row[3] = temp;
		temp = curblock->row[1];
		curblock->row[1] = curblock->row[2];
		curblock->row[2] = temp;

		curblock++;
	}
}

/////////////////////////////////////////////////////
/// Function: FlipTexture_DXTC3
/// Params: [in]ptr, [in]numBlocks
///
/////////////////////////////////////////////////////
void renderer::FlipTexture_DXTC3( unsigned char *ptr, unsigned int numBlocks )
{
	renderer::TDXTColBlock *curblock = reinterpret_cast<renderer::TDXTColBlock*>(ptr);
	renderer::TDXT3AlphaBlock *alphablock = 0;
	unsigned short tempS;
	unsigned char tempB;

	for( unsigned int i = 0; i < numBlocks; i++ )
	{
		alphablock = (renderer::TDXT3AlphaBlock *)curblock;

		tempS = alphablock->row[0];
		alphablock->row[0] = alphablock->row[3];
		alphablock->row[3] = tempS;
		tempS = alphablock->row[1];
		alphablock->row[1] = alphablock->row[2];
		alphablock->row[2] = tempS;

		curblock++;

		tempB = curblock->row[0];
		curblock->row[0] = curblock->row[3];
		curblock->row[3] = tempB;
		tempB = curblock->row[1];
		curblock->row[1] = curblock->row[2];
		curblock->row[2] = tempB;

		curblock++;
	}
}

/////////////////////////////////////////////////////
/// Function: FlipTexture_DXT5_Alpha
/// Params: [in]block
///
/////////////////////////////////////////////////////
void renderer::FlipTexture_DXT5_Alpha( renderer::TDXT5AlphaBlock *block )
{
	unsigned char gBits[4][4];
	    
	//const unsigned long mask = 0x00000007;          // bits = 00 00 01 11
	const unsigned long mask = core::LittleToBigEndianInt( 0x00000007 );          // bits = 00 00 01 11
	unsigned long bits = 0;
	std::memcpy(&bits, &block->row[0], sizeof(unsigned char) * 3);

	gBits[0][0] = (unsigned char)(bits & mask);
	bits >>= 3;
	gBits[0][1] = (unsigned char)(bits & mask);
	bits >>= 3;
	gBits[0][2] = (unsigned char)(bits & mask);
	bits >>= 3;
	gBits[0][3] = (unsigned char)(bits & mask);
	bits >>= 3;
	gBits[1][0] = (unsigned char)(bits & mask);
	bits >>= 3;
	gBits[1][1] = (unsigned char)(bits & mask);
	bits >>= 3;
	gBits[1][2] = (unsigned char)(bits & mask);
	bits >>= 3;
	gBits[1][3] = (unsigned char)(bits & mask);

	bits = 0;
	std::memcpy(&bits, &block->row[3], sizeof(unsigned char) * 3);

	gBits[2][0] = (unsigned char)(bits & mask);
	bits >>= 3;
	gBits[2][1] = (unsigned char)(bits & mask);
	bits >>= 3;
	gBits[2][2] = (unsigned char)(bits & mask);
	bits >>= 3;
	gBits[2][3] = (unsigned char)(bits & mask);
	bits >>= 3;
	gBits[3][0] = (unsigned char)(bits & mask);
	bits >>= 3;
	gBits[3][1] = (unsigned char)(bits & mask);
	bits >>= 3;
	gBits[3][2] = (unsigned char)(bits & mask);
	bits >>= 3;
	gBits[3][3] = (unsigned char)(bits & mask);

	// clear existing alpha bits
	std::memset(block->row, 0, sizeof(unsigned char) * 6);

	unsigned long *pBits = ((unsigned long*) &(block->row[0]));

	*pBits = *pBits | (gBits[3][0] << 0);
	*pBits = *pBits | (gBits[3][1] << 3);
	*pBits = *pBits | (gBits[3][2] << 6);
	*pBits = *pBits | (gBits[3][3] << 9);

	*pBits = *pBits | (gBits[2][0] << 12);
	*pBits = *pBits | (gBits[2][1] << 15);
	*pBits = *pBits | (gBits[2][2] << 18);
	*pBits = *pBits | (gBits[2][3] << 21);

	pBits = ((unsigned long*) &(block->row[3]));

	*pBits = *pBits | (gBits[1][0] << 0);
	*pBits = *pBits | (gBits[1][1] << 3);
	*pBits = *pBits | (gBits[1][2] << 6);
	*pBits = *pBits | (gBits[1][3] << 9);

	*pBits = *pBits | (gBits[0][0] << 12);
	*pBits = *pBits | (gBits[0][1] << 15);
	*pBits = *pBits | (gBits[0][2] << 18);
	*pBits = *pBits | (gBits[0][3] << 21);
}

/////////////////////////////////////////////////////
/// Function: FlipTexture_DXTC5
/// Params: [in]ptr, [in]numBlocks
///
/////////////////////////////////////////////////////
void renderer::FlipTexture_DXTC5( unsigned char *ptr, unsigned int numBlocks )
{
	renderer::TDXTColBlock *curblock = reinterpret_cast<renderer::TDXTColBlock*>(ptr);
	renderer::TDXT5AlphaBlock *alphablock = 0;
	unsigned char temp;
	    
	for( unsigned int i = 0; i < numBlocks; i++ )
	{
		alphablock = (renderer::TDXT5AlphaBlock *)curblock;
	        
		FlipTexture_DXT5_Alpha( alphablock );

		curblock++;

		temp = curblock->row[0];
		curblock->row[0] = curblock->row[3];
		curblock->row[3] = temp;
		temp = curblock->row[1];
		curblock->row[1] = curblock->row[2];
		curblock->row[2] = temp;

		curblock++;
	}
}
#endif // BASE_SUPPORT_DDS


