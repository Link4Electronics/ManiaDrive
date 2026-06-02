#ifndef _MOUSE_H
#define _MOUSE_H
/*=
Mouse
1100
**/

// Introduction
/**
Mouse API is almost explainded at the top of this guide, but here it
is some other usefull functions (macros, in fact).
**/


#define raydium_mouse_hide() glutSetCursor(GLUT_CURSOR_NONE);
/**
Hides mouse cursor.
**/

#define raydium_mouse_show() glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
/**
Shows mouse cursor.
**/

__rayapi void raydium_mouse_move(int x, int y);
/**
Moves cursor to (##x##,##y##) position (in pixel).
Example if you want to move cursor at window's center:
%%(c)raydium_mouse_move(raydium_window_tx/2, raydium_window_ty/2);%%
**/

__rayapi signed char raydium_mouse_isvisible(void);
/**
Returns true or false (0 or 1), if the mouse is visible or not.
See ##raydium_mouse_show()## and ##raydium_mouse_hide()## above.
**/

__rayapi void raydium_mouse_init (void);
/**
Internal use.
**/

__rayapi void raydium_mouse_click_callback (int but, int state, int x, int y);
/**
Internal callback.
**/

__rayapi void raydium_mouse_move_callback (int x, int y);
/**
Internal callback.
**/

__rayapi int raydium_mouse_button_pressed (int button);
/**
returns ##button## state. (See first part of this document)
**/

// Mouse Wheel
/**
To get the mouse wheel status you have to check directly the variable
##raydium_mouse_click##.
 -Value 4 means "mouse wheel up".
 -Value 5 means "mouse wheel down".
Usage example:
%%(c)
if (raydium_mouse_click==4)
	zoom*=1.1f;
if (raydium_mouse_click==5)
	zoom*=0.9f;
%%
This piece of code will change the value of zoom according mouse wheel.
**/

__rayapi void raydium_mouse_grab_delta(int *x, int *y);
/**
This function will "grab" the mouse and return mouse moves since last call.
Output parameters ##x## and ##y## will gives you the delta.

You can yse this function for any "FPS like" mouse look controls, or any other
situation where you need to known how far the user moves the mouse since
the last frame.

This function now use a box model, so its compliant with all window sizes,
even odd ones.
**/

#endif
