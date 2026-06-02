#ifndef _JOY_H
#define _JOY_H
/*=
Joysticks, pads and force feedback
3100
**/

// Introduction
/**
Raydium supports Joysticks, joypads, steering wheels, force feedback devices,
keyboard emulation.

Please note that this API may change in the future, mainly about force feedback,
that only (barely) works with Linux currently. Some support for "user control
setup" will be added, too.

Interesting variables:
%%(c)
signed char raydium_joy_button[RAYDIUM_BUTTONS_MAX_BUTTONS];
signed char raydium_joy_click;
GLfloat raydium_joy_x;
GLfloat raydium_joy_y;
GLfloat raydium_joy_z;
int raydium_joy;

char raydium_joy_n_axes;
char raydium_joy_n_buttons;
GLfloat raydium_joy_axis[RAYDIUM_JOY_MAX_AXIS]; // "raw" axes data
%%
Buttons are booleans, joy x,y and z are -1 <= (x,y,z) <= 1 and 0 means "center".
**/

/*
#define JS_EVENT_BUTTON         0x01
#define JS_EVENT_AXIS           0x02
#define JS_EVENT_INIT           0x80
extern char number_of_axes, number_of_buttons;
extern int raydium_joy_event_handle;
#ifndef WIN32
struct ff_effect effect_tremble;
#endif
extern struct ff_effect effect_tremble;
extern char effect_tremble_state;
extern clock_t last_event;
*/

__rayapi  void raydium_joy_init_vars (void);

__rayapi  void raydium_joy_key_emul (void);
/**
Emulate joy with keyboard (directional pad), so you can always use
raydium_joy_x and y variables, even if no joystick was detected.
This function has no effect if a joy is detected.

The user itself can also enable joystick emulation by keyboard (or mouse)
using the --joy-emul xxx command line option.
**/

#ifndef APPLE
#ifndef WIN32
__rayapi  int raydium_joy_process_event (struct js_event e);
#endif
#endif
__rayapi  void raydium_joy_callback (void);
__rayapi  void raydium_joy_ff_autocenter (int perc);
/**
Set Force Feedback autocenter factor.
**/

__rayapi  void raydium_joy_init (void);
__rayapi  void raydium_joy_close (void);
__rayapi  void raydium_joy_ff (void);
__rayapi  void raydium_joy_ff_tremble_set (GLfloat period, GLfloat force);
/**
Send tremble effect to Force Feedback device for a determined period,
at a particular force. (no units yet).
**/

#endif
