//!NOBINDINGS
/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

#ifndef MYGLUT_IPHONEOS_H
#define MYGLUT_IPHONEOS_H

#include "config.h"

// Activate the context with the surface.
int myglutMakeCurrent(void);

// Swap buffers to show the drawing.
int myglutSwapBuffers(void);

// Let the event cycle run to prevent abrupt termination.
void myglutPumpEvents(void);

// Get acceleration value of the device.
float myglutGetAcceleration(int pivot);

// Get touch value of the screen.
int myglutGetTouch(int index);

// Emulate a warp pointer event to the given coordinates.
void myglutWarpPointer(int x, int y);

#endif
