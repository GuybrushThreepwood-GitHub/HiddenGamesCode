
#ifndef __RESOURCEHELPER_H__
#define __RESOURCEHELPER_H__

namespace res
{
	GLuint LoadTextureName( const char* name, bool genMipMaps=false, GLenum magFilter=GL_LINEAR, GLenum minFilter=GL_LINEAR, bool lowPower=false );

	ALuint LoadSoundName( int resId );
}

#endif // __RESOURCEHELPER_H__

