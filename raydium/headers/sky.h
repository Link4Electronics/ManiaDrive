#ifndef _SKY_H
#define _SKY_H

/*=
Sky and environement boxes
1900
**/

// Introduction
/**
Skyboxes are mostly automated.

Currently, Raydium will use ##BOXfront.tga##, ##BOXback.tga##, ##BOXleft.tga##,
##BOXright.tga##, ##BOXbottom.tga## and ##BOXtop.tga## as default skybox
and will draw a skybox only if fog is disabled (this is not for technical
reasons, but only for realism, just think about it ;)... but you can force
skybox with fog using ##raydium_sky_force## if you really want).

See ##raydium_sky_box_name()## if you want to change default skybox.
**/

__rayapi void raydium_sky_box_cache (void);
/**
Internal use. Will init default skybox.
**/

__rayapi void raydium_sky_box_cache (void);
/**
As skybox texture are sometimes large files, you can pre-load skybox
with this function. If you don't do it, Raydium will load textures
during the first frame of your application.
Calling this function will automatically define sky as a HDR emitter.
See HDR chapter for more information.
**/

__rayapi void raydium_sky_box_render (GLfloat x, GLfloat y, GLfloat z);
/**
Internal use. (internal rendering functions for 2D and cubemaps skyboxes
are used, too)
**/

__rayapi void raydium_sky_sphere_render(GLfloat x, GLfloat y, GLfloat z, int detail);
/**
Internal use.
Calculates and draw the sphere. Also rotate it according the angles or orbit.
**/

__rayapi void raydium_sky_enable(void);
/**
Allows to render the sky at background.
**/


__rayapi void raydium_sky_disable(void);
/**
Disable the render of sky at background.
**/

__rayapi int raydium_sky_check(void);
/**
Returns 1 if the sky is going to be drawn at background or 0 if not.
**/

//Atmosphere
/**
Atmosphere are series of effects that intend to make the sky and the atmosphere
of the game more realistic. As this is quite-beta state, only a orbital sky
effect is available right now.
To activate/deactivate this series of effects, you should use:
##raydium_sky_atmosphere_enable## and ##raydium_sky_atmosphere_disable##
respectively.
If you need to check if the atmosphere is activated or not, use
##raydium_sky_atmosphere_check##. The rest of the functions are internal
and should not used by normal programs.
**/

__rayapi void raydium_sky_atmosphere_enable(void);
/**
turn on the use of atmosphere effects.
This one and _disable function a program should use, the other
##raydium_sky_atmosphere_## are internal ones.
**/

__rayapi void raydium_sky_atmosphere_disable(void);
/**
turn off the use of atmosphere effects.
**/

__rayapi void raydium_sky_atmosphere_render(GLfloat x, GLfloat y, GLfloat z,int detail);
/**
Internal use. This internal function draws the atmosphere effects. Right
now only draws a rotating sphere with a gradient of color (from black to white).
In a future, it will draw multiples layers of sky (with and without textures),
stars, satellites... Maybe rain and snow could be included here also.
**/

__rayapi signed char raydium_sky_atmosphere_check(void);
/**
This functions only check if the atmosphere features are been used.
Returns 1 if they are used, else 0.
**/

__rayapi void raydium_sky_box_name(char *name);
/**
This function allows to load custom name sky textures.
By default the names of the sky texture are:
##BOXfront.tga##, ##BOXback.tga##, ##BOXleft.tga##, ##BOXright.tga##,
##BOXbottom.tga## and ##BOXtop.tga##.

However with this function you can define your own skybox textures, with the
following name format:
##BOX_<name>_front.tga##, ##BOX_<name>_front.tga##, ...

Example:
%%(c)
raydium_sky_box_name("mybox");
// Skybox textures will be BOX_mybox_top.tga, BOX_mybox_left.tga and so on ...
%%

This function can be called anytime, but will cancel ##raydium_sky_box_cache()##
effect: new textures will be loaded during this function call, causing a
small freeze, and new textures will not have HDR tag.
Note that you can call ##raydium_sky_box_cache()## again to restore HDR tags.
**/

#endif
