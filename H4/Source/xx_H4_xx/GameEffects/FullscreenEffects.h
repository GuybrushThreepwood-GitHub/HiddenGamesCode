
#ifndef __FULLSCREENEFFECTS_H__
#define __FULLSCREENEFFECTS_H__

void LoadNoiseTextures();

void RemoveNoiseTextures();

void DrawNoise( float zOffset, float deltaTime );

void DrawFullscreenQuad( float zOffset, int alpha );

void DrawFullscreenQuad( const math::Vec4Lite& col );

void SetNoiseAlpha( int alpha );

#endif // __FULLSCREENEFFECTS_H__

