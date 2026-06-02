#ifndef _RENDER_H
#define _RENDER_H
/*=
Rendering
1300
**/

// Introduction
/**
render.c contains Raydium rendering core, so only "public" and
interesting function will be documented.

A few variable may be very useful here. First, you can see how many frames
were rendered during last second, reading ##raydium_render_fps## (interger,
read only). This variable is refreshed every second. If you need a
instantaneous measure, see below.

You may also read ##raydium_frame_time## (float, read only) since it gives you
the elasped time during the last frame ! (in seconds). This a very easy way
to make framerate independent things. See this example, featuring two different
uses of this variable:
%%(c)
void display(void)
{
static float posx=0;
float speed=10; // our object should move of 10 units per second
static float time_elasped_in_seconds=0;
...
posx = posx + (speed * raydium_frame_time);
time_elasped_in_seconds+=raydium_frame_time;
...
}
%%

Note that you can have instantaneous framerate with, for instance:
%%(c)
float fps=(1.f)/raydium_frame_time;
%%

As a note, I must said that it' obvious for me that many parts of render.c
have to be rewritten (tips: slow, buggy, old, ... :)
**/

__rayapi void raydium_render_lightmap_color(GLfloat *color);
/**
You may force a new lightmap rendering color "filter" anytime with this
function, allowing advanced lighting effects.
HUGE WARNING: You must turn off display lists if you change this value after
first object's render.
See ##raydium_rendering_displaylists_disable()## if needed.
**/

__rayapi void raydium_render_lightmap_color_4f(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
/**
Same as above, using 4 values.
**/

__rayapi int  raydium_rendering_prepare_texture_unit (GLenum tu, GLuint tex);
/**
This function will "prepare" hardawre texture unit ##tu## to render ##tex## texture.
There almost no reason to call this function by yourself.
**/

__rayapi void raydium_rendering_internal_prepare_texture_render (GLuint tex);
/**
Same as above, but for texture unit #0 only.
**/

__rayapi void raydium_rendering_internal_restore_render_state (void);
/**
Internal. Deprecated.
**/

// DO NOT DOCUMENT THIS ... THING !
__rayapi char infov (GLfloat x, GLfloat y);

__rayapi void raydium_rendering_from_to_simple(GLuint from, GLuint to);
/**
Same as ##raydium_rendering_from_to()##, but only with vertices (no
UV, no normals, no textures, no colors, ...).
Mostly used for internal shadow maps creation.
**/

__rayapi void raydium_rendering_from_to (GLuint from, GLuint to);
/**
Renders vertices from ##from## to ##to##.
Using object management functions is a better idea.
**/

__rayapi void raydium_rendering (void);
/**
Renders all vertices (probably useless, now).
**/

__rayapi void raydium_rendering_finish (void);
/**
You must call this function at the end of each frame. This will flush all
commands to hardware, fire a lot off callbacks, and prepare next frame.
**/

__rayapi void raydium_rendering_wireframe (void);
/**
Switch to wireframe rendering.
**/

__rayapi void raydium_rendering_normal (void);
/**
Switch back to standard rendering.
**/

__rayapi void raydium_rendering_rgb_force (GLfloat r, GLfloat g, GLfloat b);
/**
Force all RGB colored vertices to take ##(r,g,b)## color. One example of this
use is for making "team colored" cars : Do not apply textures to some faces
while modelling, and force to team color each time you render a car.
**/

__rayapi void raydium_rendering_rgb_normal (void);
/**
Disable "rgb force" state. See above.
**/

__rayapi void raydium_rendering_displaylists_disable(void);
/**
Disable display lists usage.
Some old video cards and broken drivers may get better performances WITHOUT
display lists (on large objects, mainly).
**/

__rayapi void raydium_rendering_displaylists_enable(void);
/**
Enable display lists usage. default state.
**/

__rayapi void raydium_render_fps_limit(float maxfps);
/**
This function changes the maximum number of frames per second.
Sometimes is wanted to reduce the consumption of cpu cycles by our application.
In this situations we can use a method for delay each frame of the game
until a desired framerate. In that way the residual frames won't be processed
and a "lot" of cpu cycles will be saved.
Also can be used to increase the stability in certains systems.
Set ##maxfps## to 0 if you want to disable this limit (this is the default).
**/

__rayapi void raydium_render_loading(void);
/**
Internal. Display a "low level and cheap" loading screen, useful when
R3S is downloading things from network.
**/

__rayapi void raydium_render_vblank(signed char enable);
/**
Enables or disable VBlank signal waiting, to synchronize rendering on screen
refresh intervals.

There's a lot of issues about vblank waiting with OpenGL (driver, OS, enabling
but no disabling, ...), so consider this function as very experimental.

You should use it only for tests.
**/

__rayapi void raydium_render_cull_blended_set(signed char cull_or_not);
/**
Set ##cull_or_not## to 1 (true) if you want backface culling for transparent
textures (##RAYDIUM_TEXTURE_BLEND_BLENDED##). It solves most issues with
multi-layered transparent rendering (example : seeing a car window thru
another window).

You should probably set true most of the time here, even if the historical
default is false.
**/

#endif
