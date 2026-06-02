/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

#include "raydium/index.c"

int sound_plane,sound_touched;

void display(void)
{
    if(!raydium_joy)
        raydium_joy_x=raydium_joy_y=raydium_joy_z=1;
    
    if(raydium_mouse_x<raydium_window_tx/2&&raydium_mouse_y<raydium_window_ty/2)
        raydium_sky_box_name(NULL);
    
    if(raydium_mouse_x>raydium_window_tx/2&&raydium_mouse_y<raydium_window_ty/2)
        raydium_sky_box_name("desert");
    
    if(raydium_mouse_x<raydium_window_tx/2&&raydium_mouse_y>raydium_window_ty/2)
        raydium_sky_box_name("peak");
    
    if(raydium_mouse_x>raydium_window_tx/2&&raydium_mouse_y>raydium_window_ty/2)
        raydium_sky_box_name("siege");
    
    if(raydium_mouse_click>0)
        raydium_sound_SourcePlay(sound_touched);
    
    raydium_clear_frame();
    
    raydium_camera_look_at(raydium_joy_x*5,raydium_joy_y*5,-raydium_joy_z*5,0,0,0);
    raydium_ode_draw_all(RAYDIUM_ODE_DRAW_NORMAL);
    
    raydium_osd_printf(2,98,20,0.525,"font2.tga","^F- %2i FPS - iPhone OS demo for Raydium %s, CQFD Corp.^F",raydium_render_fps,raydium_version());
    raydium_osd_printf(2,2,24,0.6,"font2.tga","^00^11^22^33^44^55^66^77^88^99^AA^BB^CC^DD^EE^FF");
    raydium_osd_logo("logo.tga");
    
    raydium_rendering_finish();
}

int main(int argc, char **argv)
{
    raydium_init_args(argc,argv);
    raydium_window_create(960,640,RAYDIUM_RENDERING_WINDOW_FIXED,"iPhone OS demo");
    
    raydium_texture_filter_change(RAYDIUM_TEXTURE_FILTER_BILINEAR);
    raydium_window_view_perspective(60,1,100);
    
    raydium_fog_disable();
    
    raydium_light_enable();
    raydium_light_on(0);
    raydium_light_conf_7f(0,50,150,200,1000000,1,0.9,0.7);
    
    raydium_background_color_change(1,0.9,0.7,1);
    raydium_sky_box_cache();
    raydium_osd_cursor_set("logo_raydium.tga",14,20);
    raydium_ode_ground_set_name("plane.tri");
    raydium_normal_regenerate_all();
    
    sound_touched=raydium_sound_LoadWav("touched.wav");
    raydium_sound_SetSourceLoop(sound_touched,0);
    sound_plane=raydium_sound_LoadWav("plane.wav");
    raydium_sound_SourcePlay(sound_plane);
    
    /*
        Sample rates higher than 8000 Hz with 16-bit PCM Ogg/Vorbis files
        heavily slow down the system, cause high respond times and stop the
        application on the iPhone OS.
    */
    // raydium_sound_load_music("memak5.ogg");
    
    raydium_render_fps_limit(30);
    
    raydium_callback(&display);
    return 0;
}
