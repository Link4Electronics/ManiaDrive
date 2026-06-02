/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

// This is supposed to be the simplest Raydium demo.
// You may have a look to skel.c for a (little bit) more complex example.

#include "raydium/index.c"

void display(void)
{
    if(raydium_key_last==1027)
        exit(0);

    raydium_clear_frame();
    raydium_camera_freemove(RAYDIUM_CAMERA_FREEMOVE_NORMAL);

    raydium_ode_draw_all(RAYDIUM_ODE_DRAW_NORMAL);
    raydium_rendering_finish();
}

int main(int argc, char **argv)
{
    raydium_init_args(argc,argv);
    raydium_init_load("simple.conf");

    raydium_ode_ground_set_name("cocorobix.tri");

    raydium_callback(&display);

    return(0);
}

// EOF
