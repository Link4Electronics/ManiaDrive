#ifndef _CORE_COMMON_H
#define _CORE_COMMON_H
/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/
//Win32 Build Detail
// Build Raydium Library
//   Define RAYDLL for Dynamic Linking
//   Define nothing for a static library.
// Build Raydium Application
//   Define RAYDLL and FORCE_LIBRAYDIUM to use library and DLL
//   Define FORCE_LIBRAYDIUM to use static library
//   Define nothing for a full compile (application and raydium together).

#define RAYDIUM_MAJOR   0
#define RAYDIUM_MINOR   800

#ifdef WIN32
# ifdef RAYDLL
#   ifndef FORCE_LIBRAYDIUM
#       define __global __declspec(dllexport)
#       define __rayapi __declspec(dllexport)
#   else
#       define __global __declspec(dllimport)
#       define __rayapi __declspec(dllimport)
#   endif
# else
#   ifdef FORCE_LIBRAYDIUM
#       define __global extern
#       define __rayapi
#   else
#       define __global
#       define __rayapi
#   endif
# endif
#else // Not under WIN32
# ifdef LIBRAY // (See Makefile)
#    define __global
#    define __rayapi
# else
#    ifdef MAIN_H
#       define __global extern
#       define __rayapi
#    endif
#    ifdef MAIN_C
#       define __global
#       define __rayapi
#    endif
# endif
# ifdef __APPLE__
#    define APPLE
#    ifndef __arm__
#       define MACOSX
#    else
#       define IPHONEOS
#       define OPENGLES
#       define main(c, v) raydium_main(c, v)
//#       define DEBUG_RENDER_DISABLE_DISPLAYLISTS
#       define DEBUG_RENDER_VERTEXARRAY
#    endif
# endif
#endif

#ifdef SWIG
#define __global
#endif

#ifdef __GNUC__
#define _PACKED __attribute__((packed))
#else
#define _PACKED
#endif

#ifdef WIN32
#include "windows.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <GL/glew.h>
#ifndef IPHONEOS
# ifndef WIN32
#  include <GL/glxew.h>
# else
#  include <GL/wglew.h>
# endif
#endif
#ifdef APPLE
#ifdef MACOSX
#include <OpenGL/gl.h>
#endif
#ifdef IPHONEOS
#include <OpenGLES/ES1/gl.h>
#include "wrapper-opengles.c"
#endif
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#include <math.h>
#include <time.h>
#include <memory.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <signal.h>
#include <stdarg.h>

#ifdef APPLE
#include <OpenAL/al.h>
#else
#include <AL/al.h>
#endif
#include <AL/alut.h>
#include <vorbis/vorbisfile.h>

#ifdef WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

// need to separate LINUX & others, using glut for joystick..
#ifndef WIN32
#ifndef APPLE
#include <linux/joystick.h>
#endif
#include <sys/ioctl.h>
#ifndef APPLE
#include <linux/rtc.h>
#endif
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pwd.h>
#endif

#ifdef linux
#include <linux/if.h>
#include <linux/sockios.h>
#include <X11/Xmd.h>
#endif

#include <errno.h>
#include "config.h"

#ifdef PHP_SUPPORT
#ifdef WIN32
#define PHP_WIN32
#define ZTS
#define ZEND_WIN32
#endif
#include "php.h"
#include "php_version.h"
#include "php_globals.h"
#include "php_variables.h"
#include "zend_modules.h"

#include "SAPI.h"

#include "php.h"
//#include "build-defs.h"
#include "zend.h"
#include "zend_extensions.h"
#include "php_ini.h"
#include "php_globals.h"
#include "php_main.h"
#include "fopen_wrappers.h"
#include "ext/standard/php_standard.h"
#endif

#include <jpeglib.h>

#define PI                                      (3.1415926535f)

// Our turn, let's define all about Raydium ...
#define RAYDIUM_APP_SHORTNAME                   "raydium"

#define RAYDIUM_MAX_VERTICES                    2000000
#define RAYDIUM_MAX_TEXTURES                    256
#define RAYDIUM_MAX_LIGHTS                      8
#define RAYDIUM_MAX_NAME_LEN                    255
#define RAYDIUM_MAX_DIR_LEN                     4096
#define RAYDIUM_MAX_CLI_ARGS                    64
#define RAYDIUM_MAX_OBJECTS                     1024
#define RAYDIUM_MAX_OBJECT_ANIMS                20
#define RAYDIUM_MAX_OBJECT_ANIM_INSTANCES       64
#define RAYDIUM_MAX_TIMECALLS                   16
#define RAYDIUM_MAX_REG_VARIABLES               256
#define RAYDIUM_MAX_REG_FUNCTION                256
#define RAYDIUM_MAX_LOG_FOPEN                   256
#define RAYDIUM_MAX_CAMERA_PATHS                16
#define RAYDIUM_MAX_CAMERA_PATH_STEPS           512
#define RAYDIUM_MAX_VIDEO_DEVICES               4
#define RAYDIUM_MAX_LIVE_TEXTURES               8
#define RAYDIUM_MAX_VIDEOS                      4
#define RAYDIUM_MAX_SHADERS                     32
#define RAYDIUM_MAX_VIEWPORT                    16

#define RAYDIUM_LIGHT_ON                        1
#define RAYDIUM_LIGHT_BLINKING                  2
#define RAYDIUM_LIGHT_OFF                       -1

#define RAYDIUM_TEXTURE_FILTER_NONE             0
#define RAYDIUM_TEXTURE_FILTER_BILINEAR         1
#define RAYDIUM_TEXTURE_FILTER_TRILINEAR        2
#define RAYDIUM_TEXTURE_FILTER_ANISO            3

#define RAYDIUM_TEXTURE_BLEND_NONE              0
#define RAYDIUM_TEXTURE_BLEND_BLENDED           1
#define RAYDIUM_TEXTURE_BLEND_CUTOUT            2
#define RAYDIUM_TEXTURE_PHANTOM                 3
#define RAYDIUM_TEXTURE_BLEND_ATM               4

// Warning, the Linux configure script duplicates these lines !
#define RAYDIUM_RENDERING_WINDOW                0
#define RAYDIUM_RENDERING_FULLSCREEN            1
#define RAYDIUM_RENDERING_NONE                  2
#define RAYDIUM_RENDERING_WINDOW_FIXED          10

#define RAYDIUM_PROJECTION_ORTHO                0
#define RAYDIUM_PROJECTION_PERSPECTIVE          1

#define RAYDIUM_KEYBOARD_SIZE                   256
#define RAYDIUM_JOY_MAX_BUTTONS                 32
#define RAYDIUM_JOY_MAX_AXIS                    8
#define RAYDIUM_JOY_EMUL_NONE                   0
#define RAYDIUM_JOY_EMUL_KEY                    1
#define RAYDIUM_JOY_EMUL_MOUSE                  2

#define RAYDIUM_NETWORK_PORT                    29104
#define RAYDIUM_NETWORK_BEACON_PORT             (RAYDIUM_NETWORK_PORT+1)
#define RAYDIUM_NETWORK_PACKET_SIZE             512
#define RAYDIUM_NETWORK_TIMEOUT                 10
#define RAYDIUM_NETWORK_PACKET_OFFSET           4
#define RAYDIUM_NETWORK_MAX_CLIENTS             8
#define RAYDIUM_NETWORK_MAX_SERVERS             32 // LAN server list
#define RAYDIUM_NETWORK_TX_QUEUE_SIZE           128
#define RAYDIUM_NETWORK_MAX_TRIES               8
#define RAYDIUM_NETWORK_MAX_NETCALLS            32
#define RAYDIUM_NETWORK_MAX_PROPAGS             32
#define RAYDIUM_NETWORK_ACK_DELAY_MAX           2
#define RAYDIUM_NETWORK_PROPAG_HEAD             sizeof(int)
#define RAYDIUM_NETWORK_BEACON_DELAY            5
#define RAYDIUM_NETWORK_BEACON_DEFAULT_TTL      15
#define RAYDIUM_NETWORK_BEACON_INFO_MAX_LEN     100
#define RAYDIUM_NETWORK_MODE_NONE               0
#define RAYDIUM_NETWORK_MODE_CLIENT             1
#define RAYDIUM_NETWORK_MODE_SERVER             2
#define RAYDIUM_NETWORK_MODE_DISCOVER           3
//#define RAYDIUM_NETWORK_TCP                   SOCK_STREAM
#define RAYDIUM_NETWORK_UDP                     SOCK_DGRAM
#define RAYDIUM_NETWORK_DATA_OK                 1
#define RAYDIUM_NETWORK_DATA_NONE               0
#define RAYDIUM_NETWORK_DATA_ERROR              -1

#define RAYDIUM_NETWORK_PACKET_DATA                     1
#define RAYDIUM_NETWORK_PACKET_ERROR_NO_MORE_PLACE      2
#define RAYDIUM_NETWORK_PACKET_ATTRIB_UID               3
#define RAYDIUM_NETWORK_PACKET_REQUEST_UID              4
#define RAYDIUM_NETWORK_PACKET_INFO_NAME                5
#define RAYDIUM_NETWORK_PACKET_ACK                      6
#define RAYDIUM_NETWORK_PACKET_SERVER_BEACON            7
#define RAYDIUM_NETWORK_PACKET_ODE_DATA                 10
#define RAYDIUM_NETWORK_PACKET_ODE_NEWELEM              11
#define RAYDIUM_NETWORK_PACKET_ODE_REMELEM              12
#define RAYDIUM_NETWORK_PACKET_ODE_NIDWHO               13
#define RAYDIUM_NETWORK_PACKET_ODE_EXPLOSION            14
#define RAYDIUM_NETWORK_PACKET_BASE                     20

#define RAYDIUM_SOUND_NUM_BUFFERS               30
#define RAYDIUM_SOUND_NUM_SOURCES               30
#define RAYDIUM_SOUND_VIDEO_SYNC_THRESHOLD      0.1 // 100 ms
#define SOUNDDATASIZE                           (4096*20)
//#define RAYDIUM_SOUND_NUM_ENVIRONMENTS                1

#define RAYDIUM_CONSOLE_MAX_LINES               18
#define RAYDIUM_CONSOLE_MAX_HISTORY             1000
#define RAYDIUM_CONSOLE_FONT_SIZE               16
#define RAYDIUM_CONSOLE_FONT_SPACER             0.5
#define RAYDIUM_CONSOLE_MAX_COMPLETION          10

#define RAYDIUM_TIMECALL_METHOD_NONE            0
#define RAYDIUM_TIMECALL_METHOD_CLOCK           1
#define RAYDIUM_TIMECALL_METHOD_DEVRTC          2

#define RAYDIUM_TIMECALL_FREQ_MIN               100
#define RAYDIUM_TIMECALL_FREQ_PREFERED          8192
#define RAYDIUM_TIMECALL_W32_MODULO_MIN         15

#define RAYDIUM_REGISTER_INT                    1
#define RAYDIUM_REGISTER_FLOAT                  2
#define RAYDIUM_REGISTER_STR                    3
#define RAYDIUM_REGISTER_ICONST                 4
#define RAYDIUM_REGISTER_FCONST                 5
#define RAYDIUM_REGISTER_SCHAR                  6

#define RAYDIUM_PARSER_TYPE_EOF                 0
#define RAYDIUM_PARSER_TYPE_FLOAT               1
#define RAYDIUM_PARSER_TYPE_STRING              2
#define RAYDIUM_PARSER_TYPE_DEBUG               3
#define RAYDIUM_PARSER_TYPE_RAWDATA             4

#define RAYDIUM_OSD_FONT_SIZE_FACTOR            (12.f)

#define RAYDIUM_RENDER_MULTITEX_AUTO_UV_FACT    (50.f)
#define RAYDIUM_RENDER_REFLECTION_FACT          (0.1f)
#ifdef IPHONEOS
#define RAYDIUM_RENDER_MAX_TEXUNITS             2
#else
#define RAYDIUM_RENDER_MAX_TEXUNITS             4
#endif

#define RAYDIUM_SKYBOX_TYPE_2D                  1
#define RAYDIUM_SKYBOX_TYPE_CUBEMAP             6

#define RAYDIUM_SKY_SPHERE_MAX_DETAIL           30
#define RAYDIUM_SKY_SPHERE_DEFAULT_DETAIL       25

#define RAYDIUM_CAPTURE_NONE                    0
#define RAYDIUM_CAPTURE_TGA                     1
#define RAYDIUM_CAPTURE_JPG                     2

#define RAYDIUM_DB_FILENAME                     raydium_file_home_path("raydium.db")
#define RAYDIUM_DB_TEMP                         raydium_file_home_path("raydium.db.temp")
#define RAYDIUM_DB_SEPARATOR                    ';'

#define RAYDIUM_HDR_SIZE                        64
#define RAYDIUM_HDR_PASS                        8
#define RAYDIUM_HDR_EYE_SPEED_DEFAULT           0.1f

#define RAYDIUM_MAX_LENSFLARES                  32
#define RAYDIUM_LENSFLARE_MAX_TEXTURES          6
#define RAYDIUM_LENSFLARE_MAX_FX                8

#define RAYDIUM_FOG_MODE_LINEAR                 GL_LINEAR
#define RAYDIUM_FOG_MODE_EXP                    GL_EXP
#define RAYDIUM_FOG_MODE_EXP2                   GL_EXP2

#define RAYDIUM_MAX_SPRITES                     64
#define RAYDIUM_MAX_FRAMES_PER_SPRITE           256
#define RAYDIUM_SPRITE_DEBUG                    0
#define RAYDIUM_SPRITE_MAX_GROUPS               64

#define RAYDIUM_FILE_NOT_FOUND                  0
#define RAYDIUM_FILE_FOUND                      1

#define RAYDIUM_LOG_MODE_STDOUT                 1
#define RAYDIUM_LOG_MODE_STERR                  2
#define RAYDIUM_LOG_MODE_NONE                   255

#define RAYDIUM_CAMERA_FREEMOVE_FIXED           0
#define RAYDIUM_CAMERA_FREEMOVE_NORMAL          1
#define RAYDIUM_CAMERA_FREEMOVE_MOUSE           2

#define RAYDIUM_CAMERA_PUSH_NONE                0
#define RAYDIUM_CAMERA_PUSH_FREEMOVE_ABS        1
#define RAYDIUM_CAMERA_PUSH_FREEMOVE_REL        2

#define RAYDIUM_VIEWPORT_NONE                   -1
#define RAYDIUM_VIEWPORT_DIRECT                 -10

#define RAYDIUM_CAMERA_SMOOTH_DATA_SIZE         8

#define RAYDIUM_COMPAT_ODE_OBJECT_MOVE_R1099    2

__global int     raydium_init_argc;
__global char  **raydium_init_argv;
__global char    raydium_init_wd[RAYDIUM_MAX_DIR_LEN];
__global char    raydium_homedir[RAYDIUM_MAX_DIR_LEN];

__global int     raydium_key_last;
__global signed char    raydium_key[RAYDIUM_KEYBOARD_SIZE];
__global signed char    raydium_key_trace; // change this from ingame console !
__global signed char    raydium_mouse_click;
__global signed char    raydium_mouse_button[3];
__global GLint  raydium_mouse_x;
__global GLint  raydium_mouse_y;
__global signed char    raydium_mouse_mode_delta; // captured for mouseview camera ?
__global signed char    raydium_mouse_cancel_move; // cancel next mouseview call
__global signed char    raydium_joy_button[RAYDIUM_JOY_MAX_BUTTONS];
__global GLfloat        raydium_joy_axis[RAYDIUM_JOY_MAX_AXIS];
__global signed char    raydium_joy_click;
__global GLfloat raydium_joy_x;
__global GLfloat raydium_joy_y;
__global GLfloat raydium_joy_z;
__global int     raydium_joy;
__global char    raydium_joy_name[RAYDIUM_MAX_NAME_LEN];
__global char    raydium_joy_n_axes;     // read only
__global char    raydium_joy_n_buttons;  // read only
__global char    raydium_joy_emul_type;


__global GLuint   raydium_texture_index;
__global GLuint   raydium_texture_current_main;
__global GLuint   raydium_texture_current_multi;
__global GLfloat  raydium_texture_current_multi_u;
__global GLfloat  raydium_texture_current_multi_v;
__global GLuint   raydium_texture_current_env[RAYDIUM_RENDER_MAX_TEXUNITS-1]; // do not count first texunit
__global signed char     raydium_texture_filter;
__global signed char     raydium_texture_compression_available;
__global signed char     raydium_texture_compression_enabled;
__global GLint    raydium_texture_size_max;
__global GLint    raydium_texture_units;
__global GLfloat  raydium_texture_filter_aniso_levels;
__global GLuint   raydium_internal_size_vector_float_4;
__global GLuint   raydium_texture_to_replace;
__global GLfloat  raydium_texture_used_memory;
__global int    raydium_texture_use_npot_textures;

__global signed char    raydium_projection;
__global GLfloat raydium_projection_fov;        // perspective only
__global GLfloat raydium_projection_near;       // perspective & ortho
__global GLfloat raydium_projection_far;        // perspective & ortho
__global GLfloat raydium_projection_left;       // ortho only
__global GLfloat raydium_projection_right;      // ortho only
__global GLfloat raydium_projection_bottom;     // ortho only
__global GLfloat raydium_projection_top;        // ortho only
__global GLfloat raydium_background_color[4];

__global signed char raydium_sky_force;
__global signed char raydium_sky_atmosphere_enable_tag;
__global GLfloat     raydium_sky_sphere_angle_orbit_u;
__global GLfloat     raydium_sky_sphere_angle_orbit_v;
__global GLfloat     raydium_sky_sphere_x_vel;
__global GLfloat     raydium_sky_sphere_y_vel;
__global GLfloat     raydium_sky_sphere_x_pos;
__global GLfloat     raydium_sky_sphere_y_pos;
__global GLfloat     raydium_sky_sphere_quality;
__global GLfloat     raydium_sky_sphere_heigth;
__global signed char raydium_sky_sphere_generated;
__global char raydium_sky_texture_skybox_type;
__global char raydium_sky_texture_skybox_front[RAYDIUM_MAX_NAME_LEN]; // will be used as the only texture name for cubemaps
__global char raydium_sky_texture_skybox_back[RAYDIUM_MAX_NAME_LEN];
__global char raydium_sky_texture_skybox_left[RAYDIUM_MAX_NAME_LEN];
__global char raydium_sky_texture_skybox_right[RAYDIUM_MAX_NAME_LEN];
__global char raydium_sky_texture_skybox_bottom[RAYDIUM_MAX_NAME_LEN];
__global char raydium_sky_texture_skybox_top[RAYDIUM_MAX_NAME_LEN];
__global int raydium_sky_prefix_defined;
__global int raydium_sky_enabled;


__global GLsizei raydium_window_tx;
__global GLsizei raydium_window_ty;
__global GLfloat raydium_window_ratio;
__global signed char     raydium_window_mode;

__global GLuint  raydium_vertex_index;
__global GLuint  raydium_vertex_counter;
__global signed char    raydium_vertex_offset_triangle;

#define raydium_vertex_x(a) raydium_vertex_arr[(a)*3+0]
#define raydium_vertex_y(a) raydium_vertex_arr[(a)*3+1]
#define raydium_vertex_z(a) raydium_vertex_arr[(a)*3+2]
__global GLfloat *raydium_vertex_arr;

#define raydium_vertex_normal_x(a) raydium_vertex_normal_arr[(a)*3+0]
#define raydium_vertex_normal_y(a) raydium_vertex_normal_arr[(a)*3+1]
#define raydium_vertex_normal_z(a) raydium_vertex_normal_arr[(a)*3+2]
__global GLfloat *raydium_vertex_normal_arr;

#define raydium_vertex_tangent_x(a) raydium_vertex_tangent_arr[(a)*3+0]
#define raydium_vertex_tangent_y(a) raydium_vertex_tangent_arr[(a)*3+1]
#define raydium_vertex_tangent_z(a) raydium_vertex_tangent_arr[(a)*3+2]
__global GLfloat *raydium_vertex_tangent_arr;

#define raydium_vertex_normal_visu_x(a) raydium_vertex_normal_visu_arr[(a)*3+0]
#define raydium_vertex_normal_visu_y(a) raydium_vertex_normal_visu_arr[(a)*3+1]
#define raydium_vertex_normal_visu_z(a) raydium_vertex_normal_visu_arr[(a)*3+2]
__global GLfloat *raydium_vertex_normal_visu_arr; // used for smoothing

#define raydium_vertex_texture_u(a) raydium_vertex_texture_uv_arr[(a)*2+0]
#define raydium_vertex_texture_v(a) raydium_vertex_texture_uv_arr[(a)*2+1]
__global GLfloat *raydium_vertex_texture_uv_arr;

__global GLuint  *raydium_vertex_texture;
__global GLuint  *raydium_vertex_texture_multi;

#define raydium_vertex_texture_multi_u(a) raydium_vertex_texture_multi_uv_arr[(a)*2+0]
#define raydium_vertex_texture_multi_v(a) raydium_vertex_texture_multi_uv_arr[(a)*2+1]
__global GLfloat *raydium_vertex_texture_multi_uv_arr;

__global GLuint  *raydium_vertex_texture_env[RAYDIUM_RENDER_MAX_TEXUNITS];
__global signed char     *raydium_vertex_tag;

__global signed char     raydium_texture_islightmap[RAYDIUM_MAX_TEXTURES];
__global signed char     raydium_texture_nolight[RAYDIUM_MAX_TEXTURES];
__global signed char     raydium_texture_blended[RAYDIUM_MAX_TEXTURES];
__global signed char     raydium_texture_hdr[RAYDIUM_MAX_TEXTURES];
__global signed char     raydium_texture_env[RAYDIUM_MAX_TEXTURES];
__global signed char     raydium_texture_cubemap[RAYDIUM_MAX_TEXTURES];
__global GLint    raydium_texture_shader[RAYDIUM_MAX_TEXTURES];
__global char     raydium_texture_name[RAYDIUM_MAX_TEXTURES][RAYDIUM_MAX_NAME_LEN];
__global GLfloat  raydium_texture_rgb[RAYDIUM_MAX_TEXTURES][4];
__global int      raydium_texture_used[RAYDIUM_MAX_TEXTURES];
__global float    raydium_texture_memory[RAYDIUM_MAX_TEXTURES];

__global GLint  raydium_object_index;
__global GLuint raydium_object_start[RAYDIUM_MAX_OBJECTS];
__global GLuint raydium_object_end[RAYDIUM_MAX_OBJECTS];
__global char   raydium_object_name[RAYDIUM_MAX_OBJECTS][RAYDIUM_MAX_NAME_LEN];
__global GLuint raydium_object_dl[RAYDIUM_MAX_OBJECTS];
__global char   raydium_object_dl_state[RAYDIUM_MAX_OBJECTS];
__global signed char raydium_object_anims[RAYDIUM_MAX_OBJECTS]; // number of anims
__global GLuint raydium_object_anim_len[RAYDIUM_MAX_OBJECTS]; // len of each anim
__global GLuint raydium_object_anim_start[RAYDIUM_MAX_OBJECTS][RAYDIUM_MAX_OBJECT_ANIMS];
__global GLuint raydium_object_anim_end[RAYDIUM_MAX_OBJECTS][RAYDIUM_MAX_OBJECT_ANIMS];
#ifndef SWIG
__global char   raydium_object_anim_names[RAYDIUM_MAX_OBJECTS][RAYDIUM_MAX_OBJECT_ANIMS][RAYDIUM_MAX_NAME_LEN]; // anims name array
#endif
__global GLuint  raydium_object_anim_default_anim[RAYDIUM_MAX_OBJECTS];
__global GLuint  raydium_object_anim_instance_current[RAYDIUM_MAX_OBJECTS]; // current "rendering" instance
__global GLfloat raydium_object_anim_automatic_factor[RAYDIUM_MAX_OBJECTS][RAYDIUM_MAX_OBJECT_ANIMS]; // frame automatic factor
__global GLfloat raydium_object_anim_time_factor;
// states
__global int     raydium_object_anim_current[RAYDIUM_MAX_OBJECTS][RAYDIUM_MAX_OBJECT_ANIM_INSTANCES]; // current anim
__global GLfloat raydium_object_anim_frame_current[RAYDIUM_MAX_OBJECTS][RAYDIUM_MAX_OBJECT_ANIM_INSTANCES]; // current frame
__global int     raydium_object_anim_previous[RAYDIUM_MAX_OBJECTS][RAYDIUM_MAX_OBJECT_ANIM_INSTANCES]; // anim switch blending (last anim)
__global GLfloat raydium_object_anim_frame_previous[RAYDIUM_MAX_OBJECTS][RAYDIUM_MAX_OBJECT_ANIM_INSTANCES]; // anim switch blending (last frame)
__global GLfloat raydium_object_anim_frame_previous_timeout[RAYDIUM_MAX_OBJECTS][RAYDIUM_MAX_OBJECT_ANIM_INSTANCES]; // "timer" for anim switch blending
__global int     raydium_object_anim_punctually_flag[RAYDIUM_MAX_OBJECTS][RAYDIUM_MAX_OBJECT_ANIM_INSTANCES];

__global int     raydium_render_fps;
__global GLfloat raydium_render_rgb_force[4];
__global GLfloat raydium_render_lightmap_color_value[4];
__global signed char     raydium_render_rgb_force_tag;
__global signed char     raydium_render_cull_blended;
__global char    raydium_render_displaylists_tag;
__global GLboolean raydium_render_internal_light_previous_step;
__global float   raydium_render_max_fps;
#ifdef DEBUG_RENDER_VERTEXARRAY_GLBINDS_DISPLAY
__global unsigned int raydium_render_debug_va_count;
#endif

__global void (*raydium_callback_loop)(void); // for glutMainLoop wrapper

__global signed char     raydium_fog_enabled_tag;
__global signed char     raydium_fog_volumetric_enabled_tag;
__global int             raydium_fog_mode_value;
__global float           raydium_fog_far_value;
__global float           raydium_fog_near_value;
__global float           raydium_fog_density_value;
__global signed char     raydium_fog_volumetric_axis;
#define raydium_fog_volumetric_array(a) raydium_vertex_arr[(a)*3+raydium_fog_volumetric_axis]

__global signed char    raydium_light_enabled_tag;
__global signed char    raydium_light_internal_state[RAYDIUM_MAX_LIGHTS];
__global GLfloat raydium_light_position[RAYDIUM_MAX_LIGHTS][4];
__global GLfloat raydium_light_color[RAYDIUM_MAX_LIGHTS][4];
__global GLfloat raydium_light_intensity[RAYDIUM_MAX_LIGHTS];
__global GLfloat raydium_light_blink_low[RAYDIUM_MAX_LIGHTS];
__global GLfloat raydium_light_blink_high[RAYDIUM_MAX_LIGHTS];
__global GLfloat raydium_light_blink_increment[RAYDIUM_MAX_LIGHTS];
// TODO: light_spot

__global signed char    raydium_internal_vertex_next_extras;
__global GLfloat raydium_internal_vertex_next_u;
__global GLfloat raydium_internal_vertex_next_v;
__global GLfloat raydium_internal_vertex_next_nx;
__global GLfloat raydium_internal_vertex_next_ny;
__global GLfloat raydium_internal_vertex_next_nz;

__global signed char    raydium_frame_first_camera_pass;
__global float   raydium_frame_time;
__global GLfloat raydium_camera_x;
__global GLfloat raydium_camera_y; // read only, undocumented.
__global GLfloat raydium_camera_z;
__global signed char    raydium_camera_pushed;
__global GLfloat  raydium_camera_cursor_place[3];
__global GLfloat  raydium_camera_look_at_roll;
__global GLfloat  raydium_camera_rumble_amplitude;
__global GLfloat  raydium_camera_rumble_evolution;
__global GLfloat  raydium_camera_rumble_remaining;
__global GLdouble raydium_camera_gl_modelview[16];
__global GLdouble raydium_camera_gl_projection[16];
__global GLint    raydium_camera_gl_viewport[4];
__global GLfloat  raydium_camera_freemove_sensibility;
__global GLfloat  raydium_camera_freemove_speed;
__global GLfloat  raydium_camera_push_type;
__global GLfloat  raydium_camera_push_current[3];
__global GLfloat  raydium_camera_push_end[3];
__global GLfloat  raydium_camera_push_slowness[3];
__global GLfloat  raydium_camera_data[6];
__global GLfloat  raydium_camera_smooth_data_primary[RAYDIUM_CAMERA_SMOOTH_DATA_SIZE];
__global GLfloat  *raydium_camera_smooth_data_ptr;

__global GLfloat  raydium_ode_timestep;
__global GLfloat  raydium_ode_physics_freq;

typedef struct raydium_camera_Path
    {
    char    name[RAYDIUM_MAX_NAME_LEN];
    GLfloat x[RAYDIUM_MAX_CAMERA_PATH_STEPS];
    GLfloat y[RAYDIUM_MAX_CAMERA_PATH_STEPS];
    GLfloat z[RAYDIUM_MAX_CAMERA_PATH_STEPS];
    GLfloat zoom[RAYDIUM_MAX_CAMERA_PATH_STEPS];
    GLfloat roll[RAYDIUM_MAX_CAMERA_PATH_STEPS];
    int steps;
    } raydium_camera_Path;

__global raydium_camera_Path raydium_camera_path[RAYDIUM_MAX_CAMERA_PATHS];
__global signed char raydium_camera_path_reset_flag;


typedef struct raydium_Viewport
{
    char    name[RAYDIUM_MAX_NAME_LEN];
    int     tx;
    int     ty;
} raydium_Viewport ;

__global raydium_Viewport raydium_viewport[RAYDIUM_MAX_VIEWPORT];
__global int raydium_viewport_nb;
__global int raydium_viewport_use;
__global signed char raydium_viewport_saved_context;
__global GLdouble    raydium_viewport_saved_modelview_data[16];
__global GLdouble    raydium_viewport_saved_gl_modelview[16];
__global GLdouble    raydium_viewport_saved_gl_projection[16];
__global GLint       raydium_viewport_saved_gl_viewport[4];
__global GLint       raydium_viewport_saved_mouse[2];
__global GLfloat     raydium_viewport_saved_camera_pos[3];
__global GLfloat     raydium_viewport_direct_values[4];
__global signed char raydium_viewport_direct_mouse_clamped;

__global int    raydium_network_socket;
__global int    raydium_network_uid;
__global signed char raydium_network_mode;
__global signed char raydium_network_client[RAYDIUM_NETWORK_MAX_CLIENTS];
__global time_t raydium_network_start;
__global struct sockaddr raydium_network_client_addr[RAYDIUM_NETWORK_MAX_CLIENTS];
__global time_t raydium_network_keepalive[RAYDIUM_NETWORK_MAX_CLIENTS];
__global char   raydium_network_name_local[RAYDIUM_MAX_NAME_LEN];
__global char   raydium_network_name[RAYDIUM_NETWORK_MAX_CLIENTS][RAYDIUM_MAX_NAME_LEN];
__global char   raydium_network_connected_server[RAYDIUM_MAX_NAME_LEN];
__global int    raydium_network_netcall_type[RAYDIUM_NETWORK_MAX_NETCALLS];
__global void * raydium_network_netcall_func[RAYDIUM_NETWORK_MAX_NETCALLS];
__global signed char raydium_network_netcall_tcp[RAYDIUM_NETWORK_MAX_NETCALLS];
__global void * raydium_network_on_connect;
__global void * raydium_network_on_disconnect;
__global int    raydium_network_stat_rx;
__global int    raydium_network_stat_tx;
__global int    raydium_network_stat_reemitted;
__global int    raydium_network_stat_double;
__global int    raydium_network_stat_lost;
__global int    raydium_network_stat_bogus_ack;

typedef struct raydium_network_Tcp
    {
    signed char state;
    unsigned short tcpid;
    char packet[RAYDIUM_NETWORK_PACKET_SIZE];
    unsigned long time;
    unsigned short retries_left;
    struct sockaddr to;
    int to_player;
    } raydium_network_Tcp;

__global raydium_network_Tcp raydium_network_queue[RAYDIUM_NETWORK_TX_QUEUE_SIZE];
__global int raydium_network_queue_index;

__global unsigned short raydium_network_tcpid_i[RAYDIUM_NETWORK_TX_QUEUE_SIZE]; // ID
__global int            raydium_network_tcpid_p[RAYDIUM_NETWORK_TX_QUEUE_SIZE]; // Player
__global int            raydium_network_tcpid_index;

__global unsigned long raydium_netwok_queue_ack_delay_client;
__global unsigned long raydium_netwok_queue_ack_delay_server[RAYDIUM_NETWORK_MAX_CLIENTS];
__global signed char   raydium_network_write_notcp;

typedef struct raydium_network_Propag
    {
    signed char state;
    int type;
    unsigned short size;
    unsigned int version;
    void *data;
    } raydium_network_Propag;

__global raydium_network_Propag raydium_network_propag[RAYDIUM_NETWORK_MAX_PROPAGS];

typedef struct raydium_network_Beacon
    {
    int id;
    char name[RAYDIUM_MAX_NAME_LEN];
    char ip[RAYDIUM_MAX_NAME_LEN];
    char info[RAYDIUM_NETWORK_BEACON_INFO_MAX_LEN];
    int player_count;
    int player_max;
    unsigned long when; // 0 means "free"
    } raydium_network_Beacon;

typedef struct raydium_network_BeaconSearch
    {
    signed char active;
    char app_or_mod[RAYDIUM_MAX_NAME_LEN];
    int version;
    } raydium_network_BeaconSearch;

__global raydium_network_Beacon raydium_network_server_list[RAYDIUM_NETWORK_MAX_SERVERS];
__global char raydium_network_beacon[RAYDIUM_NETWORK_PACKET_SIZE];
__global int  raydium_network_beacon_info_offset;
__global raydium_network_BeaconSearch raydium_network_beacon_search;

#ifdef linux
#define RAYDIUM_NETWORK_BROADCAST_INTERFACE_MAX 8
__global int raydium_network_broadcast_interface_index;
__global struct sockaddr_in raydium_network_broadcast_interfaces[RAYDIUM_NETWORK_BROADCAST_INTERFACE_MAX];
#endif

__global ALuint  raydium_sound_buffer[RAYDIUM_SOUND_NUM_BUFFERS];
__global ALuint  raydium_sound_source[RAYDIUM_SOUND_NUM_SOURCES];
__global ALfloat raydium_sound_source_fade_factor[RAYDIUM_SOUND_NUM_SOURCES];
__global char    raydium_sound_source_fade_tofile[RAYDIUM_SOUND_NUM_SOURCES][RAYDIUM_MAX_NAME_LEN];
__global ALfloat raydium_sound_DefaultReferenceDistance;
__global int     raydium_sound;
__global int     raydium_sound_top_buffer;
__global char    raydium_sound_music_buf[SOUNDDATASIZE];
__global FILE   *raydium_sound_music_file;
__global OggVorbis_File raydium_sound_vf;
__global vorbis_info   *raydium_sound_ogginfo;
__global int    (*raydium_sound_music_eof_callback)(char *);
__global void   (*raydium_sound_music_changed_callback)(void);

typedef struct raydium_sound_music_Info {
    char artist[RAYDIUM_MAX_NAME_LEN];
    char title [RAYDIUM_MAX_NAME_LEN];
    char album [RAYDIUM_MAX_NAME_LEN];
} raydium_sound_music_Info;

__global raydium_sound_music_Info raydium_sound_music_info;


//__global float  raydium_spritecoord[RAYDIUM_MAX_SPRITES][RAYDIUM_MAX_FRAMES_PER_SPRITE][8];
__global int    raydium_sprite_frames[RAYDIUM_MAX_SPRITES];
__global int    raydium_current_sprite;
__global float  raydium_sprite_size[RAYDIUM_MAX_SPRITES];
__global int    raydium_sprite_used[RAYDIUM_MAX_SPRITES];
__global float  raydium_sprite_pos[RAYDIUM_MAX_SPRITES][3];
__global int    raydium_sprite_textureid[RAYDIUM_MAX_SPRITES][RAYDIUM_MAX_FRAMES_PER_SPRITE];
#ifndef SWIG
__global float  raydium_sprite_coords[RAYDIUM_MAX_SPRITES][RAYDIUM_MAX_FRAMES_PER_SPRITE][4];
#endif
__global float  raydium_sprite_time[RAYDIUM_MAX_SPRITES];
__global float  raydium_sprite_timer[RAYDIUM_MAX_SPRITES];
__global int    raydium_sprite_current_frame[RAYDIUM_MAX_SPRITES];
__global int    raydium_sprite_total_frames[RAYDIUM_MAX_SPRITES];
__global int    raydium_sprite_group_frame[RAYDIUM_MAX_SPRITES][RAYDIUM_MAX_FRAMES_PER_SPRITE];
__global int    raydium_sprite_group_current[RAYDIUM_MAX_SPRITES];
__global int    raydium_sprite_group_start_frame[RAYDIUM_MAX_SPRITES][RAYDIUM_SPRITE_MAX_GROUPS];
__global float  raydium_sprite_collision_box[RAYDIUM_MAX_SPRITES][3];
__global int    raydium_sprite_collision_box_id[RAYDIUM_MAX_SPRITES];
__global int    raydium_sprite_collision_element_id[RAYDIUM_MAX_SPRITES];
__global int    raydium_sprite_group_jump[RAYDIUM_MAX_SPRITES][RAYDIUM_SPRITE_MAX_GROUPS]; //-2=no jump, -1=stop, number=group to jump
__global int    raydium_sprite_type[RAYDIUM_MAX_SPRITES];
__global int    raydium_sprite_status[RAYDIUM_MAX_SPRITES];
__global char   raydium_sprite_name[RAYDIUM_MAX_SPRITES][RAYDIUM_MAX_NAME_LEN];
__global char   raydium_sprite_stopped[RAYDIUM_MAX_SPRITES];


__global GLfloat raydium_osd_logo_angle;
__global GLfloat raydium_osd_color[4];

__global signed char  raydium_osd_cursor_visible;
__global GLuint  raydium_osd_cursor_texture;
__global GLfloat raydium_osd_cursor_xsize;
__global GLfloat raydium_osd_cursor_ysize;
__global GLfloat raydium_osd_cursor_xoffset;
__global GLfloat raydium_osd_cursor_yoffset;

__global GLfloat raydium_osd_ega[48];

__global GLfloat raydium_osd_fade_color_timeleft;
__global GLfloat raydium_osd_fade_color_increment[4];
__global GLfloat raydium_osd_fade_color_current[4];
__global void *  raydium_osd_fade_OnFadeEnd;

__global GLfloat raydium_console_pos;
__global GLfloat raydium_console_inc;
__global GLfloat raydium_console_config_max;
__global GLfloat raydium_console_config_speed;
__global char    raydium_console_config_texture[RAYDIUM_MAX_NAME_LEN];
__global char    raydium_console_config_font[RAYDIUM_MAX_NAME_LEN];
__global GLfloat raydium_console_cursor_blink;
__global char    raydium_console_lines[RAYDIUM_CONSOLE_MAX_LINES][RAYDIUM_MAX_NAME_LEN];
__global int     raydium_console_line_last;
__global char    raydium_console_get_string[RAYDIUM_MAX_NAME_LEN];
__global char    raydium_console_get_string_last[RAYDIUM_MAX_NAME_LEN];
__global void *  raydium_console_gets_callback;
__global char    raydium_console_history[RAYDIUM_CONSOLE_MAX_HISTORY][RAYDIUM_MAX_NAME_LEN];
__global int     raydium_console_history_index; // store
__global int     raydium_console_history_index_current; // user
__global char    raydium_console_history_filename[RAYDIUM_MAX_NAME_LEN];

__global int            raydium_timecall_index;
__global signed char    raydium_timecall_method;
__global unsigned long  raydium_timecall_max_frequency;
__global unsigned long  raydium_timecall_clocks_per_sec;
__global unsigned long  raydium_timecall_offset;
__global unsigned long  raydium_timecall_stopped_stamp;
__global int            raydium_timecall_devrtc_handle;
__global unsigned long  raydium_timecall_devrtc_clocks;
__global void *         raydium_timecall_funct[RAYDIUM_MAX_TIMECALLS];
__global GLint          raydium_timecall_soft_call[RAYDIUM_MAX_TIMECALLS];
__global unsigned long  raydium_timecall_interval[RAYDIUM_MAX_TIMECALLS];
__global unsigned long  raydium_timecall_next[RAYDIUM_MAX_TIMECALLS];
__global int            raydium_timecall_w32_divmodulo;
__global unsigned long  raydium_timecall_debug_movie;

__global signed char raydium_shadow_tag;
__global signed char raydium_shadow_rendering;
__global int raydium_shadow_ground_mesh;

__global signed char raydium_capture_asked;
__global char raydium_capture_filename[RAYDIUM_MAX_NAME_LEN];

__global signed char raydium_hdr_state;
__global signed char raydium_hdr_generated;
__global int   raydium_hdr_texture_id;
__global float raydium_hdr_eye;
__global float raydium_hdr_eye_speed;
__global float raydium_hdr_alpha_max;
__global GLfloat raydium_hdr_color_local[4];
__global GLfloat raydium_hdr_color_ambient[4];
__global unsigned char *raydium_hdr_mem;
__global unsigned char *raydium_hdr_mem_hdr;
__global unsigned char *raydium_hdr_mem_hdr2;
__global unsigned char *raydium_hdr_mem_hdr3;

#ifndef SWIG
__global signed char raydium_lensflare_enabled_tag;
__global char        raydium_lensflare_name[RAYDIUM_MAX_LENSFLARES][RAYDIUM_MAX_NAME_LEN];
__global signed char raydium_lensflare_internal_created[RAYDIUM_MAX_LENSFLARES];
__global signed char raydium_lensflare_internal_state[RAYDIUM_MAX_LENSFLARES];
__global int         raydium_lensflare_texture_id[RAYDIUM_MAX_LENSFLARES][RAYDIUM_LENSFLARE_MAX_TEXTURES];
__global float       raydium_lensflare_position[RAYDIUM_MAX_LENSFLARES][6];
__global float       raydium_lensflare_appearance[RAYDIUM_MAX_LENSFLARES];
__global signed char raydium_lensflare_fx_internal_state[RAYDIUM_MAX_LENSFLARES][RAYDIUM_LENSFLARE_MAX_FX];
__global float       raydium_lensflare_fx_size[RAYDIUM_MAX_LENSFLARES][RAYDIUM_LENSFLARE_MAX_FX];
__global float       raydium_lensflare_fx_color[RAYDIUM_MAX_LENSFLARES][RAYDIUM_LENSFLARE_MAX_FX][4];
__global float       raydium_lensflare_fx_velocity[RAYDIUM_MAX_LENSFLARES][RAYDIUM_LENSFLARE_MAX_FX];
#endif

__global int    raydium_register_variable_index;
__global int    raydium_register_function_index;
__global char   raydium_register_variable_name[RAYDIUM_MAX_REG_VARIABLES][RAYDIUM_MAX_NAME_LEN];
__global void * raydium_register_variable_addr[RAYDIUM_MAX_REG_VARIABLES];
__global int    raydium_register_variable_type[RAYDIUM_MAX_REG_VARIABLES];
#ifdef PHP_SUPPORT
#define ZFE zend_function_entry
#else
#define ZFE void *
#endif
#ifndef SWIG
__global ZFE raydium_register_function_list[RAYDIUM_MAX_REG_FUNCTION];
#endif

__global char raydium_php_rayphp_path[RAYDIUM_MAX_NAME_LEN];

__global FILE *raydium_log_file;
__global signed char raydium_log_atexit_dump;

__global char  raydium_file_log_fopen[RAYDIUM_MAX_LOG_FOPEN][RAYDIUM_MAX_NAME_LEN];
__global char  raydium_file_log_fopen_status[RAYDIUM_MAX_LOG_FOPEN];
__global int   raydium_file_log_fopen_index;
__global int   raydium_file_log_mode;
__global signed char raydium_file_log_time;

__global unsigned long raydium_profile_timer;

__global unsigned int raydium_compat_flags;
#endif
// EOF
