/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

// avoid "real GLUT"
#ifndef GLUT_API_VERSION
#ifndef MYGLUT_H
#define MYGLUT_H

#ifdef APPLE
#ifdef MACOSX
#include <OpenGL/gl.h>
#endif
#ifdef IPHONEOS
#include <OpenGLES/ES1/gl.h>
#endif
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#define GLUT_LEFT_BUTTON                0
#define GLUT_MIDDLE_BUTTON              1
#define GLUT_RIGHT_BUTTON               2

#define GLUT_DOWN                       0
#define GLUT_UP                         1

#define GLUT_KEY_F1                     1
#define GLUT_KEY_F2                     2
#define GLUT_KEY_F3                     3
#define GLUT_KEY_F4                     4
#define GLUT_KEY_F5                     5
#define GLUT_KEY_F6                     6
#define GLUT_KEY_F7                     7
#define GLUT_KEY_F8                     8
#define GLUT_KEY_F9                     9
#define GLUT_KEY_F10                    10
#define GLUT_KEY_F11                    11
#define GLUT_KEY_F12                    12
#define GLUT_KEY_LEFT                   100
#define GLUT_KEY_UP                     101
#define GLUT_KEY_RIGHT                  102
#define GLUT_KEY_DOWN                   103
#define GLUT_KEY_PAGE_UP                104
#define GLUT_KEY_PAGE_DOWN              105
#define GLUT_KEY_HOME                   106
#define GLUT_KEY_END                    107
#define GLUT_KEY_INSERT                 108

#define GLUT_WINDOW_WIDTH               102
#define GLUT_WINDOW_HEIGHT              103
#define GLUT_WINDOW_DEPTH_SIZE          106
#define GLUT_WINDOW_CURSOR              122
#define GLUT_WINDOW_INVRATIO		256

#define GLUT_CURSOR_LEFT_ARROW          1
#define GLUT_CURSOR_NONE                101

#define GLUT_RGB                        0
#define GLUT_DOUBLE                     2
#define GLUT_DEPTH                      16

#define GLUT_GAME_MODE_POSSIBLE         1

// ------------------- variables
__global int _glutWindowSize[2];
__global signed char _glutWindowInverseRatio;
__global signed char _glutMouseVisible;

// callbacks:
__global void (*glutReshapeFuncCB)(int width, int height);
__global void (*glutKeyboardFuncCB)(unsigned char key, int x, int y);
__global void (*glutSpecialUpFuncCB)(int key, int x, int y);
__global void (*glutSpecialFuncCB)(int key, int x, int y);
__global void (*glutMotionFuncCB)(int x, int y);
__global void (*glutPassiveMotionFuncCB)(int x, int y);
__global void (*glutMouseFuncCB)(int button, int state, int x, int y);
__global void (*glutDisplayFuncCB)(void);
__global void (*glutIdleFuncCB)(void);

#endif
#endif
