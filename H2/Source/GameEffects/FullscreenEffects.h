
#ifndef __FULLSCREENEFFECTS_H__
#define __FULLSCREENEFFECTS_H__

void DrawFullscreenQuad( float zOffset, int alpha );

void DrawFullscreenQuad( const math::Vec4Lite& col );

void DrawFullscreenQuad(float zOffset, GLuint textureId);

#endif // __FULLSCREENEFFECTS_H__

