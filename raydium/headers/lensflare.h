#ifndef _LENSFLARE_H
#define _LENSFLARE_H

/*=
Lens flare effects
4350
**/

// Introduction
/**
Lens flare effects can be used to emulate various reflection and scattering
visuals, for example and probably the most common type is the sunlight.
Lens flare effects inside the engine are designed to be highly customizable.
You should be able to create everything with them you can think of. Lamps,
starry skies, headlights for vehicles, explosions, UFO invasions and much more.

Lets show an example scene of a basic sunlight lens flare effect:
[[http://ftp.cqfd-corp.org/lens-flare-scene_s.jpg lensflare]]

Currently, lens flare effects support eight different effect layers, so called
FX groups. Each group has size, velocity, color and alpha attributes. You're
also able to use custom textures, see the ##raydium_lensflare_texture_name()##
function for details.

The simplest way to create a lens flare effect is to load a lens flare effect
configuration file, usually .lf extension text files.
%%(c)
// Skeleton lens flare effect configuration file.

// Optional custom set of textures.
// When no texture name was assigned the default set of textures will be used.
// e. g. texture="hexagon" will load LF_hexagon_glow.tga instead of LFglow.tga.
texture=""

// Lens flare FX groups.
// To deactivate a FX group simply do not declare it or comment it out.
// Available FX groups: main, rays, streak, halo, orbs, glow, star, anam.
// fxgrp = { size, velocity, red, green, blue, alpha }
main   = { 1.0, 1.0, 1.0, 1.0, 1.0, 1.0 }
rays   = { 1.0, 1.0, 1.0, 1.0, 1.0, 1.0 }
streak = { 1.0, 1.0, 1.0, 1.0, 1.0, 1.0 }
halo   = { 1.0, 1.0, 1.0, 1.0, 1.0, 1.0 }
orbs   = { 1.0, 1.0, 1.0, 1.0, 1.0, 1.0 }
glow   = { 1.0, 1.0, 1.0, 1.0, 1.0, 1.0 }
star   = { 1.0, 1.0, 1.0, 1.0, 1.0, 1.0 }
anam   = { 1.0, 1.0, 1.0, 1.0, 1.0, 1.0 }
%%
After creation of the lens flare you can change the position calling
##raydium_lensflare_move()## using the lens flare id returned by
##raydium_lensflare_create()##.

If you want to use lens flare effects together with HDR, be sure to enable HDR
calling ##raydium_hdr_enable()## before creating a lens flare effect, so the
textures will be automagically set to be HDR able.

This effect can be quiet heavy on the framerate, especially when using multiple
big blended layers. So please be careful, you have been warned. ;)
**/

__rayapi void raydium_lensflare_init(void);
/**
Internal use.
**/

__rayapi void raydium_lensflare_enable(void);
/**
Enables the lens flare effect system.
**/

__rayapi void raydium_lensflare_disable(void);
/**
Disables the lens flare effect system.
**/

__rayapi signed char raydium_lensflare_isvalid(int lf);
/**
Internal use, but you can call this function to verify if ##lf## lens flare
has a valid id inside array range, see ##RAYDIUM_MAX_LENSFLARES##.
**/

__rayapi void raydium_lensflare_reset(int lf);
/**
Deactivates and resets ##lf## lens flare to the initial state.
**/

__rayapi void raydium_lensflare_on(int lf);
/**
Turns ##lf## lens flare on.
**/

__rayapi void raydium_lensflare_off(int lf);
/**
Turns ##lf## lens flare off.
**/

__rayapi void raydium_lensflare_switch(int lf);
/**
Toggles ##lf## lens flare state.
**/

__rayapi void raydium_lensflare_texture_name(int lf, char *name);
/**
This function allows to load custom name lens flare textures.
By default the names of the lens flare textures are:
##LFglow.tga##, ##LFstar.tga##, ##LFstreak.tga##, ##LFhalo.tga##,
##LFray.tga##, ##LFanam.tga##.

However with this function you can define your own set of lens flare textures,
with the following name format:
##LF_<name>_glow.tga##, ##LF_<name>_star.tga##, ...

Example:
%%(c)
raydium_lensflare_texture_name(raydium_lensflare_find("myname"),"mytex");
// Lens flare textures will be LF_mytex_glow.tga, LF_mytex_star.tga, ...
%%

If you want to use lens flare effects together with HDR, be sure to enable HDR
calling ##raydium_hdr_enable()## before using this function, so the textures
will be automagically set to be HDR able.
**/

__rayapi void raydium_lensflare_move(int lf, float *pos);
/**
This function will move ##lf## lens flare to position ##pos##.
##pos## is a float array of three values (x,y,z).
**/

__rayapi void raydium_lensflare_move_3f(int lf, float x, float y, float z);
/**
Same as above, but using three float values.
**/

__rayapi void raydium_lensflare_move_relative_3f(int lf, float x, float y, float z);
/**
Same as above, but using relative coordinates. Useful for ingame displacements.
**/

__rayapi signed char raydium_lensflare_fx_isvalid(int fx);
/**
Internal use, but you can call this function to verify if ##fx## FX group
has a valid id inside array range, see ##RAYDIUM_LENSFLARE_MAX_FX##.
**/

__rayapi void raydium_lensflare_fx_on(int lf, int fx);
/**
Turns ##fx## FX group on.
**/

__rayapi void raydium_lensflare_fx_off(int lf, int fx);
/**
Turns ##fx## FX group off.
**/

__rayapi void raydium_lensflare_fx_switch(int lf, int fx);
/**
Toggles ##fx## FX group state.
**/

__rayapi void raydium_lensflare_fx_size_change(int lf, int fx, float s);
/**
Sets ##s## size of ##fx## FX group within ##lf## lens flare effect.
**/

__rayapi void raydium_lensflare_fx_color_change(int lf, int fx, float r, float g, float b);
/**
Sets color of ##fx## FX group within ##lf## lens flare effect.
**/

__rayapi void raydium_lensflare_fx_alpha_change(int lf, int fx, float a);
/**
Sets alpha transparency of ##fx## FX group within ##lf## lens flare effect.
**/

__rayapi void raydium_lensflare_fx_color_rgba(int lf, int fx, float r, float g, float b, float a);
/**
Sets color and alpha of ##fx## FX group within ##lf## lens flare effect.
**/

__rayapi void raydium_lensflare_fx_velocity_change(int lf, int fx, float v);
/**
Sets ##v## velocity of ##fx## FX group within ##lf## lens flare effect.
**/

__rayapi signed char raydium_lensflare_internal_load(int lf, char *filename);
/**
Internal use, load ##lf## lens flare values from ##filename## configuration.
**/

__rayapi int raydium_lensflare_find(char *name);
/**
Resolves lens flare id from its ##name##.
**/

__rayapi int raydium_lensflare_create(char *name, char *filename);
/**
Builds a new lens flare effect with ##name## using the values from the
##filename## configuration and returns the new lens flare id or -1 on error.

You can also call this function several times with the same ##name## to
overwrite the lens flare values with different ##filename## configurations.

There are ##RAYDIUM_MAX_LENSFLARES## slots available at the same time.
**/

__rayapi signed char raydium_lensflare_internal_point_is_occluded(float x, float y, float z);
/**
Internal use.
**/

__rayapi void raydium_lensflare_fx_internal_draw(int lf, int fx, int id, int tex, float d, float cx, float cy, float vx, float vy, float g, float pt);
/**
Internal use.
**/

__rayapi void raydium_lensflare_draw(int lf);
/**
Internal use.
**/

__rayapi void raydium_lensflare_draw_all(void);
/**
Internal use.
**/

#endif
