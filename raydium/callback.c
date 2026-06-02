/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

#ifndef DONT_INCLUDE_HEADERS
#include "index.h"
#else
#include "headers/callback.h"
#endif

void raydium_osd_cursor_draw(void);
void raydium_console_draw(void);
void raydium_gui_draw(void);
void raydium_osd_fade_callback(void);
#ifdef PHP_SUPPORT
int  raydium_php_exec(char *);
#endif
#ifdef ODE_SUPPORT
void raydium_ode_network_read(void);
void raydium_ode_capture_play_callback(void);
#endif
void raydium_network_read_faked(void);
void raydium_video_callback(void);
void raydium_internal_live_video_callback(void);
void raydium_object_callback(void);
void raydium_web_callback(void);
void raydium_hdr_map(void);
void raydium_hdr_map_apply(void);


void raydium_callback_image(void)
{
raydium_timecall_callback();
raydium_light_callback();
//raydium_particle_draw_all();
raydium_hdr_map_apply();
raydium_particle_callback();
raydium_osd_fade_callback();
raydium_gui_draw();
raydium_console_draw();
raydium_osd_cursor_draw();
raydium_joy_callback();
raydium_sound_callback();
#ifdef ODE_SUPPORT
// 0hz ODE callback workaround
raydium_ode_network_read();
raydium_ode_capture_play_callback();
#endif

if(raydium_network_mode==RAYDIUM_NETWORK_MODE_DISCOVER)
    raydium_network_read_faked();

//#ifndef WIN32
raydium_internal_live_video_callback();
//#endif
raydium_video_callback();
raydium_web_callback();
raydium_object_callback();
}


void raydium_callback_set(void)
{
if(raydium_window_mode==RAYDIUM_RENDERING_NONE)
    return;
glutReshapeFunc(&raydium_window_resize_callback);
glutKeyboardFunc((void *)raydium_key_normal_callback);
glutSpecialUpFunc((void *)raydium_key_special_up_callback);
glutSpecialFunc((void *)raydium_key_special_callback);
glutMotionFunc((void *)raydium_mouse_move_callback);
glutPassiveMotionFunc((void *)raydium_mouse_move_callback);
glutMouseFunc((void *)raydium_mouse_click_callback);
}

void raydium_callback_internal_loop_wrapper(void)
{
//FPS LIMITER variables
static unsigned long time_previous;
unsigned long curr_time;
float max_fps;

//if(raydium_render_max_fps!=0)
if(raydium_render_max_fps>0)
    {
    curr_time=raydium_timecall_clock();
    // 2 is an experimental offset
    //max_fps=raydium_timecall_clocks_per_sec/(raydium_render_max_fps+2);
    max_fps=raydium_timecall_clocks_per_sec/(raydium_render_max_fps);

    if((curr_time-time_previous) < max_fps)
        {
#ifndef WIN32
        usleep(1);
#else
        Sleep(1);
#endif
        return;
        }

    time_previous=curr_time;
    }

raydium_callback_loop();
}

void raydium_callback_display_set(void (*loop)){
#ifdef PHP_SUPPORT
char autoexec[RAYDIUM_MAX_NAME_LEN];

if(raydium_init_cli_option("autoexec2",autoexec))
    raydium_php_exec(autoexec);
#endif
raydium_callback_loop=loop;
glutDisplayFunc(raydium_callback_internal_loop_wrapper);
glutIdleFunc(raydium_callback_internal_loop_wrapper);
}

void raydium_loop(void){
    glutManualLoop();
}

void raydium_callback(void (*loop) )
{
raydium_callback_display_set(loop);
glutMainLoop();
}
