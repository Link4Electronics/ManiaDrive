// avoid "real GLUT"
#ifndef GLUT_API_VERSION
#ifndef _MYGLUT_H
#define _MYGLUT_H

#include "../myglut.h"

/*=
MyGLUT
4700
**/

// Details
/**
MyGLUT is an alternative to "real" GLUT, providing a small subset of the
original API, up to Raydium needs, and tries to enhances some GLUT features,
such as window and keyboard management.

While staying portable on all Raydium targets, MyGLUT is able to use some
platform-specific features, such as Xinerama for Linux.

MyGLUT is heavily based on PW, the PLIB windowing library
( http://plib.sourceforge.net/ ). Huge thanks to Steve Baker for his great work.
**/

// protos
__rayapi void glutInit(int *argc, char **argv);
__rayapi int  glutGet(int enu);
__rayapi void glutSetCursor(int cursor);
__rayapi void glutWarpPointer(int x, int y);
__rayapi void glutSwapBuffers(void);
__rayapi void glutIgnoreKeyRepeat(int ignore);
__rayapi void glutReshapeFunc(void *func);
__rayapi void glutKeyboardFunc(void *func);
__rayapi void glutSpecialUpFunc(void *func);
__rayapi void glutSpecialFunc(void *func);
__rayapi void glutMotionFunc(void *func);
__rayapi void glutPassiveMotionFunc(void *func);
__rayapi void glutMouseFunc(void *func);
__rayapi void glutDisplayFunc(void *func);
__rayapi void glutIdleFunc(void *func);
__rayapi int  glutExtensionSupported(const char *name);
__rayapi void glutManualLoop(void);
__rayapi void glutMainLoop(void);
__rayapi void glutWireSphere(GLdouble radius, GLint slices, GLint stacks);
__rayapi void myglutCreateWindow(GLuint tx, GLuint ty, signed char rendering, char *name);

#endif
#endif
