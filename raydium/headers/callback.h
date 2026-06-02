#ifndef _CALLBACK_H
#define _CALLBACK_H

/*=
Callbacks
1500
**/

// Introduction
/**
This file contains many initializations, a few internal callbacks, but 
will provides a very important function for end-user, wich will 
gives user display function to Raydium: see below
**/


__rayapi void raydium_callback_image (void);
/**
Internal use.
**/

__rayapi void raydium_callback_set (void);
/**
Internal use.
**/

__rayapi void raydium_callback (void (*loop));
/**
This function will loop over the provided display function, indefinitely.
"loop" must be: 
%%(c) void loop(void) %%
**/

__rayapi void raydium_callback_display_set(void (*fdisplay));
/**
This function will install display callback but don't enter in an infinite loop.
fdisplay is user display function.
"fdisplay" must be: 
%%(c) void fdisplay(void) %%
**/

__rayapi void raydium_loop(void);
/**
Run raydium once. This function needs to be called periodically.
This function return after loop completion.
It is usefull to integrate raydium loop in another program.
**/
#endif
