/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

#ifndef SHADOW_H
#define SHADOW_H

#define RAYDIUM_SHADOW_OPACITY          0.5
#define RAYDIUM_SHADOW_TEXTURE          "shadowmap"
#define RAYDIUM_SHADOW_BOX_SIZE         10

#define RAYDIUM_SHADOW_MODE_FULLSCENE   10
#define RAYDIUM_SHADOW_MODE_CAMERABOX   20

__global char raydium_shadow_mode_current;
__global GLuint raydium_shadow_map_size;
__global GLuint raydium_shadow_texture;
__global int raydium_shadow_light;
__global GLfloat raydium_shadow_ground_modelsize;
__global GLfloat raydium_shadow_ground_center_factor_x;
__global GLfloat raydium_shadow_ground_center_factor_y;
__global GLfloat raydium_shadow_camerabox_size_current;
__global GLuint raydium_shadow_object_dl[RAYDIUM_MAX_OBJECTS];
__global char raydium_shadow_object_dl_state[RAYDIUM_MAX_OBJECTS];

// proto
void raydium_camera_replace(void);
signed char raydium_object_isvalid(int obj);
void raydium_ode_draw_all(signed char names);
void raydium_object_find_minmax(GLuint obj, GLfloat *min, GLfloat *max);
void raydium_object_find_center_factors(GLuint obj, GLfloat *tx, GLfloat *ty, GLfloat *tz);
void raydium_object_find_center(GLuint obj, GLfloat *tx, GLfloat *ty, GLfloat *tz);
void raydium_rendering_from_to_simple(GLuint from, GLuint to);
void raydium_object_render_va(int obj, signed char simple);

#endif
