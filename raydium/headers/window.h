#ifndef _WINDOW_H
#define _WINDOW_H
/*=
Window management
600
**/

// Introduction
/**
Some important functions, used for window creation and managment.
**/

__rayapi void raydium_window_close (void);
/**
This function is called by Raydium, do not use.
**/

__rayapi void raydium_window_create (GLuint tx, GLuint ty, signed char rendering, char *name);
/**
You must call this function once in your program, with following arguments:

1. ##tx##, ##ty##: window size, in pixel
2. ##rendering##: window mode: ##RAYDIUM_RENDERING_*## :
##NONE##, ##WINDOW## (resizable), ##WINDOW_FIXED## (unresizable) and ##FULLSCREEN##.
3. ##name##: window's name

Raydium is using MyGLUT for window management, and MyGLUT fullscreen is not
the same between all implementations. For instance, the Linux MyGLUT does
not resize the X screen, therefore doesn't care about ##tx## and ##ty##.
But the win32 implementation did resize screen.

This design choice allows to respect usual behaviors of operating systems.

Note that user can force fullscreen using ##--fullscreen## on the command line.
**/

__rayapi void raydium_window_resize_callback (GLsizei Width, GLsizei Height);
/**
This function is automaticaly called during a window resize,
and resize OpenGL rendering space.

There is almost no reason to call this function by yourself.
**/

__rayapi void raydium_window_view_update (void);
/**
If you've changed 3D window size (clipping: raydium_projection_*),
apply to hardware with this fonction.
**/

__rayapi void raydium_window_projection_update(void);
/**
Subpart of ##raydium_window_view_update()## that does not update the GL viewport.
**/

__rayapi void raydium_window_view_perspective(GLfloat fov, GLfloat fnear, GLfloat ffar);
/**
All-in-one function: sets all "perspective" variables, and updates.
**/

#endif
