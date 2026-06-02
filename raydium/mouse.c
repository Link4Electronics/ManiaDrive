/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

#ifndef DONT_INCLUDE_HEADERS
#include "index.h"
#else
#include "headers/mouse.h"
#include "headers/gui.h"
#endif
/*
#define raydium_mouse_hide() glutSetCursor(GLUT_CURSOR_NONE);
#define raydium_mouse_show() glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
*/

void raydium_mouse_move(int x, int y)
{
static signed char first=1;
if(raydium_mouse_x!=x || raydium_mouse_y!=y || first)
    {
    glutWarpPointer(x,y);
    first=0;
    }
}

signed char raydium_mouse_isvisible(void)
{
int ret;
ret=glutGet(GLUT_WINDOW_CURSOR);
return (ret==GLUT_CURSOR_NONE?0:1);
}

void raydium_mouse_init(void)
{
if(raydium_window_mode==RAYDIUM_RENDERING_NONE)
    return;
memset(raydium_mouse_button,0,3);
raydium_mouse_click=0;
raydium_mouse_x=raydium_mouse_y=raydium_window_ty=0;
raydium_mouse_mode_delta=0;
raydium_mouse_cancel_move=0;
raydium_log("mouse: OK");
raydium_mouse_hide();
}


void raydium_mouse_click_callback(int but,int state,int x,int y)
{
unsigned char n=0,s=0;
signed char special=0;


if(but==GLUT_LEFT_BUTTON) n=0;
else if(but==GLUT_RIGHT_BUTTON) n=1;
else if(but==GLUT_MIDDLE_BUTTON) n=2;
else
    {
    special=1;
    n=but;
    }

if(state==GLUT_DOWN) s=1;
if(state==GLUT_UP) s=0;
if(s) raydium_mouse_click=n+1;
if(!special)
    raydium_mouse_button[n]=s;
}


void raydium_mouse_move_callback(int x, int y)
{
if(x>=0 && x<raydium_window_tx) raydium_mouse_x=x;
if(y>=0 && y<raydium_window_ty) raydium_mouse_y=y;
}

int raydium_mouse_button_pressed(int button)
{
return raydium_mouse_button[button];
}

void raydium_mouse_grab_delta(int *x, int *y)
{
static int prev_state=0;
static GLint   old_x;
static GLint   old_y;
#ifndef WIN32
#ifndef APPLE
GLint border;
#endif
#endif

if(raydium_gui_isvisible())
    { *x=0; *y=0; return; }

#ifndef WIN32
#ifndef APPLE
border = raydium_math_max(raydium_window_tx,raydium_window_ty) / 4;
#endif
#endif

raydium_mouse_mode_delta=1;

if(prev_state==0 || raydium_mouse_cancel_move)
{
	*x = 0;
	*y = 0;
}
else
{
	*x = raydium_mouse_x - old_x;
	*y = raydium_mouse_y - old_y;
	old_x = raydium_mouse_x;
	old_y = raydium_mouse_y;
}


//putting the mouse in the middle of the screen, then read the next data of the mouse correctly

#ifndef WIN32
#ifndef APPLE
// xf: small update, we now only move the mouse when it's needed, allow
// a smoother handling of odd screen resolutions
// This "box" model is only enabled for X11, since win32 & OSX seems to have
// issues (somewhere in the message queue, probably)
if(raydium_mouse_x < border || raydium_mouse_x > raydium_window_tx-border ||
   raydium_mouse_y < border || raydium_mouse_y > raydium_window_ty-border ||
   prev_state==0 || raydium_mouse_cancel_move)
#endif
#endif
	{
	old_x=raydium_window_tx/2.0f;
	old_y=raydium_window_ty/2.0f;
	raydium_mouse_move(old_x, old_y);
	}
prev_state=1;
raydium_mouse_cancel_move=0;

}
