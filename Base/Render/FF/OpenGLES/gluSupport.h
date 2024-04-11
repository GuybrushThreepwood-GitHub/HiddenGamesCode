
#ifndef __GLUSUPPORT_H__
#define __GLUSUPPORT_H__

#ifdef BASE_SUPPORT_OPENGLES

GLint gluBuild2DMipmapLevels (GLenum target, GLint internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type, GLint level, GLint base, GLint max, const void *data);
GLint gluBuild2DMipmaps (GLenum target, GLint internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *data);

const GLubyte * gluErrorString (GLenum error);

void gluLookAt (GLfloat eyeX, GLfloat eyeY, GLfloat eyeZ, GLfloat centerX, GLfloat centerY, GLfloat centerZ, GLfloat upX, GLfloat upY, GLfloat upZ);
void gluOrtho2D (GLfloat left, GLfloat right, GLfloat bottom, GLfloat top);
void gluPerspective (GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar);

GLint gluScaleImage (GLenum format, GLsizei wIn, GLsizei hIn, GLenum typeIn, const void *dataIn, GLsizei wOut, GLsizei hOut, GLenum typeOut, GLvoid* dataOut);

#endif // BASE_SUPPORT_OPENGLES

#endif // __GLUSUPPORT_H__

