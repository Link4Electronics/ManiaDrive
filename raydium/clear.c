/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

#ifndef DONT_INCLUDE_HEADERS
#include "index.h"
#else
#include "headers/clear.h"
#endif

void raydium_clear_frame(void)
{
//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
glEnable(GL_STENCIL_TEST); // HDR

raydium_frame_first_camera_pass=1;
raydium_vertex_counter=0;
raydium_viewport_use=-1;
raydium_mouse_mode_delta=0;
#ifdef DEBUG_RENDER_VERTEXARRAY_GLBINDS_DISPLAY
raydium_render_debug_va_count=0;
#endif
}

void raydium_clear_color_update(void)
{
glClearColor(raydium_background_color[0],
             raydium_background_color[1],
             raydium_background_color[2],
             raydium_background_color[3]);
}
