/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

#include "headers/myglut.h"
#include "myglut-iphoneos.h"

void glutInit(int *argc, char **argv)
{
    glutReshapeFuncCB=NULL;
    glutKeyboardFuncCB=NULL;
    glutSpecialUpFuncCB=NULL;
    glutSpecialFuncCB=NULL;
    glutMotionFuncCB=NULL;
    glutPassiveMotionFuncCB=NULL;
    glutMouseFuncCB=NULL;
    glutDisplayFuncCB=NULL;
    glutIdleFuncCB=NULL;
}

void pwInit(int x, int y, int w, int h, int multisample, char *title, int border, int num_samples, int window_style)
{
    myglutMakeCurrent();
}

void glutSetCursor(int c)
{
    _glutMouseVisible=0;
}

void glutWarpPointer(int x, int y)
{
    myglutWarpPointer(x,y);
}

void glutSwapBuffers(void)
{
    myglutSwapBuffers();
}

// glutMainLoop is generic (myglut.c)

void myglutGetEvents(void)
{
    myglutPumpEvents();
    
    switch(myglutGetTouch(0))
    {
        // Press
        case 1:
            if(glutPassiveMotionFuncCB)
                glutPassiveMotionFuncCB(myglutGetTouch(1),myglutGetTouch(2));
            if(glutMouseFuncCB)
                glutMouseFuncCB(GLUT_LEFT_BUTTON,GLUT_DOWN,myglutGetTouch(1),myglutGetTouch(2));
            break;
        // Move
        case 2:
            if(glutPassiveMotionFuncCB)
                glutPassiveMotionFuncCB(myglutGetTouch(1),myglutGetTouch(2));
            break;
        // Release
        case 3:
            if(glutPassiveMotionFuncCB)
                glutPassiveMotionFuncCB(myglutGetTouch(1),myglutGetTouch(2));
            if(glutMouseFuncCB)
                glutMouseFuncCB(GLUT_LEFT_BUTTON,GLUT_UP,myglutGetTouch(1),myglutGetTouch(2));
            break;
        default:
            break;
    }
}

void myglutCloseWindow(void)
{
    exit(0);
}
