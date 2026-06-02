#ifndef _HDR_H
#define _HDR_H

/*=
Pseudo HDR
4300
**/

// Introduction
/**
Currently, HDR features of Raydium are highly experimental, because of
performance issues with some hardware.

The basic idea behind Raydium's HDR rendering is to use a few tricks to
allow such rendering even on low-end hardware.

Let's take a example scene:
[[http://ftp.cqfd-corp.org/pseudo-hdr-scene_s.jpg base]]

During this regular rendering, Raydium will use the (boolean) "HDR tag" of
each texture to know if the texture is bright or not. This tag is toggled
using the ##raydium_hdr_texture()## function. The result is a very contrasted
version of the rendered image, like this:
[[http://ftp.cqfd-corp.org/pseudo-hdr-stencil_s.png stencil]]

Raydium will then downscale this image, and apply a heavy blur effect:
[[http://ftp.cqfd-corp.org/pseudo-hdr-stencil-64-blur.png blur]]

This blurred texture is uploaded back to the video card, upscaled, and added
to the already rendered color buffer:
[[http://ftp.cqfd-corp.org/pseudo-hdr-scene-final_s.jpg final]]

**/

__rayapi void raydium_hdr_init(void);
/**
Internal use.
**/

__rayapi void raydium_hdr_enable(void);
/**
Enables HDR.
**/

__rayapi void raydium_hdr_disable(void);
/**
Disables HDR.
**/

__rayapi void raydium_hdr_internal_window_malloc(void);
/**
Internal use.
**/

__rayapi void raydium_hdr_block(signed char blocking);
/**
Internal use. Not currenlty used, in facts.
**/

__rayapi void raydium_hdr_blur(unsigned char *in, unsigned char *out);
/**
Internal use. Will blur ##in## to ##out##.
**/

__rayapi void raydium_hdr_map(void);
/**
Internal use. Will create HDR texture.
**/

__rayapi void raydium_hdr_map_apply(void);
/**
Internal use. Will apply HDR texture on the screen.
**/

__rayapi void raydium_hdr_settings_color_local(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
/**
You can define the color of the HDR effect. This is the "accurate" HDR color,
not the ambient HDR color. (see ##raydium_hdr_settings_color_ambient()##).

Alpha channel (##a## is not currently used). Default is full white, but
it's quiet a good idea to match your background scene color.
**/

__rayapi void raydium_hdr_settings_color_ambient(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
/**
Same as raydium_hdr_settings_color_local(), but for the ambient HDR mask. This
mask is a zoomed copy of the local HDR mask, and should be a subtile, darker
color of ##raydium_hdr_settings_color_local()##.

You may have to play a bit with this color to find the correct one for your
scene, but it can enhance greatly the HDR rendering.

You can set ##r##, ##g## and ##b## to ##0## to disable the ambient HDR.
**/

__rayapi void raydium_hdr_settings_eye(float speed, float alpha_max);
/**
Defines the eye settings, where ##speed## will define how quickly the eye
will react to a bright scene.
Default is currently ##RAYDIUM_HDR_EYE_SPEED_DEFAULT## (##0.1##).

The other setting, ##alpha_max##, will define how visible the HDR effect will
be. The default is ##1.0##.
**/

__rayapi void raydium_hdr_settings(GLfloat *color_local, GLfloat *color_ambient, float eye_speed, float alpha_max);
/**
See ##raydium_hdr_settings_color_local()##, ##raydium_hdr_settings_color_ambient()##
and ##raydium_hdr_settings_eye()##.
**/

__rayapi signed char raydium_hdr_texture(int texture, signed char hdr);
/**

This function will set the HDR tag on ##texture##. If ##hdr## is set to
true (##1##), this texture will be used as "bright emitter" by the HDR rendering.

By default, all textures are loaded with a HDR tag of ##0##, except if texture
filename starts with ##HDR## letters (example: ##HDR_lamp_buld.tga##). Skybox
is also loaded with HDR tag sets to ##1##.

WARNING: Because of display lists, HDR tagging should be done **before**
drawing objects for the first time !
**/

__rayapi signed char raydium_hdr_texture_name(char *texture, signed char hdr);
/**
Same as above, but using ##texture## name.
**/

__rayapi void raydium_hdr_texture_reset(void);
/**
Will reset all HDR tag for textures to ##0##.
See note about display lists in ##raydium_hdr_texture()##, above.
**/

#endif
