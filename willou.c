/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

// Small "in-game" model viewer, with mouse look. Use --model option.

#define NO_ODE_SUPPORT
#include "raydium/index.c"

char model[RAYDIUM_MAX_NAME_LEN];

GLfloat light_color[] = {1.0, 0.9, 0.8, 1.0};
//    GLfloat back_color[] = {1, 1, 1, 1};
GLfloat  *back_color=light_color;
//    GLfloat light_color[] = {0, 0, 0, 1};


void display(void)
{

    raydium_joy_key_emul();

    if(raydium_key_last==1027)
        exit(0);

    if(raydium_key[GLUT_KEY_F1]) { raydium_projection_fov/=(1.04); raydium_window_view_update(); }
    if(raydium_key[GLUT_KEY_F2]) { raydium_projection_fov*=(1.04); raydium_window_view_update(); }

    if(raydium_key_last==3)
        raydium_light_disable();
    if(raydium_key_last==4)
        raydium_light_enable();

    raydium_light_position[0][0]=50;
    raydium_light_position[0][1]=150;
    raydium_light_position[0][2]=200;
    raydium_light_position[0][3]=1.0;

    raydium_clear_frame();

    raydium_camera_freemove(RAYDIUM_CAMERA_FREEMOVE_NORMAL);

    raydium_object_draw_name(model);
    raydium_particle_draw_all(); // if raydium_particle_state_restore() is used ...

//    raydium_osd_logo("logo.tga");

    raydium_rendering_finish();
}

int main(int argc, char **argv)
{
    raydium_init_args(argc,argv);

    // window creation
    raydium_window_create(640,480,RAYDIUM_RENDERING_WINDOW,"Willou's test");
    raydium_texture_filter_change(RAYDIUM_TEXTURE_FILTER_TRILINEAR);
    raydium_projection_near=0.01;
    raydium_projection_far=2500;
    raydium_projection_fov=60;
    raydium_fog_disable();
    raydium_window_view_update();

    raydium_light_enable();
    raydium_light_on(0);
    memcpy(raydium_light_color[0],light_color,raydium_internal_size_vector_float_4);
    raydium_light_intensity[0] = 10000000;
    raydium_light_update_all(0);

    raydium_sky_box_name("CUBE_clouds_*.tga");
    raydium_sky_box_cache();

    raydium_window_view_update();
    raydium_background_color_change(back_color[0],back_color[1],back_color[2],back_color[3]);

    strcpy(model,"a.tri");
    raydium_init_cli_option("model",model);

    raydium_callback(&display);
    return(0);
}
// EOF
