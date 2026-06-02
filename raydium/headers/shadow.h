#ifndef _SHADOW__H
#define _SHADOW__H
#include "../shadow.h"

/*=
Shadows
4600
**/

// Introduction
/**
Raydium provides a simple support for shadowing, on top of the RayODE API.

This shadowing system will draw all RayODE elements to a shadow map, from
light point of vue. This map will then be projected to the RayODE ground.

This method have some drawbacks (shadows are projected all over
Z axis [floor-and-ceiling shadows], one frame lag, ...) but have the huge
advantage to be very fast, even on lew-end hardware, and requires no
particular tuning or setup.

Short story: see ##raydium_shadow_enable()## (available in the console).
**/


__rayapi void raydium_shadow_init(void);
/**
Internal use.
**/

__rayapi void raydium_shadow_mode(char mode);
/**
Raydium now features more accurate shadowmaps, rendering shadows only
around the camera. See ##raydium_shadow_camerabox_size()## if you want to
change the size of the bounding box.
This mode is now the default (##RAYDIUM_SHADOW_MODE_CAMERABOX##), but the old
behavior is available with ##RAYDIUM_SHADOW_MODE_FULLSCENE##, and you can
switch from one mode to the other anytime.
Please note that, currently, camerabox shadow model can't work with viewports,
(and would be very slow) so you should then switch to fullscene model.
**/

__rayapi void raydium_shadow_camerabox_size(GLfloat size);
/**
Defines the size of the box around the camera where the shadows are generated
and rendered.
Of course, current shadow mode should be ##RAYDIUM_SHADOW_MODE_CAMERABOX##.
Default value is ##RAYDIUM_SHADOW_BOX_SIZE## (see shadow.h)
**/

__rayapi void raydium_shadow_enable(void);
/**
Enable shadows. Shadows are disabled by default.
**/

__rayapi void raydium_shadow_disable(void);
/**
Disable shadows.
**/

__rayapi signed char raydium_shadow_isenabled(void);
/**
Will return true (1) if shadows are enabled, false (0) otherwise.
**/

__rayapi void raydium_shadow_light_main(GLuint l);
/**
Defines wich light is use to generate shadows. Light #0 is the default.
**/

__rayapi void raydium_shadow_ground_change(int object);
/**
Internal use. Defines which object is the ground (where shadows are projected).
**/

__rayapi void raydium_shadow_map_generate(void);
/**
Internal use. Will generate shadow map texture.
**/

__rayapi void raydium_shadow_map_render(void);
/**
Internal use. Will apply shadow map to the ground.
**/

__rayapi void raydium_shadow_object_draw(GLuint o);
/**
Internal use. Draws an object to the shadow map.
**/


#endif
