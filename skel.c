/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

// This file is a default skeleton. Replace all [ ... ] sections with yours.
// Have fun !

#include "raydium/index.c"


void display(void)
{
raydium_joy_key_emul();

if(raydium_key_last==1027)
    exit(0);

raydium_clear_frame();
/* [ place your camera here ] */ raydium_camera_look_at(10,-2,2,0,0,0);
/* [ draw here ] */ raydium_ode_draw_all(0);
raydium_rendering_finish();
}


int main(int argc, char **argv)
{
raydium_init_args(argc,argv);
raydium_window_create(640,480,RAYDIUM_RENDERING_WINDOW,"My app");

raydium_texture_filter_change(RAYDIUM_TEXTURE_FILTER_TRILINEAR);
raydium_window_view_perspective(60,0.01,2500); // fov 60 + near and far planes

raydium_fog_disable();    
raydium_light_enable();
raydium_light_on(0);

raydium_light_conf_7f(0,50,150,200,1000000,1,0.9,0.7); // id, pos, intensity and color (RGB)
raydium_background_color_change(1,0.9,0.7,1);

raydium_sky_box_cache();

/* [ place base scene here ] */ raydium_ode_ground_set_name("cocorobix.tri");

raydium_callback(&display);
return(0);
}

// EOF
