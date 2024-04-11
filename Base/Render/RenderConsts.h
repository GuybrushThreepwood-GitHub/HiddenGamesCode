
#ifndef __RENDERCONSTS_H__
#define __RENDERCONSTS_H__

namespace
{
	const int _R_ = 0;
	const int _G_ = 1;
	const int _B_ = 2;
	const int _A_ = 3;

	const float FXFACTOR = (1.0f / 65536.0f);
	const float FXINV = 65536.0f;

	inline int f2x( float f ) 
	{ 
		return static_cast<int>( f * FXINV ); 
	}

	inline float x2f( int x )
	{ 
		return static_cast<float>( static_cast<float>(x) * FXFACTOR ); 
	}
};

#endif // __RENDERCONSTS_H__