/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

// This client application will allow you to spectate any Raydium server (and
// spawns a wood box into the networked world to avoid timeouts). 

// Usage example: ./odyncomp.sh willt5.c --server 192.168.1.10

// willou.c based.

#include "raydium/index.c"

GLfloat light_color[] = {1.0, 0.9, 0.8, 1.0};

char draw_debug=1;

void display(void)
{
    
    raydium_joy_key_emul();

    if(raydium_key_last==1027)
        exit(0);

    if(raydium_key_last==1032)
        raydium_php_exec("test.php");

    if(raydium_key_last==1116) draw_debug*=-1;
    
    raydium_light_position[0][0]=100;
    raydium_light_position[0][1]=100;
    raydium_light_position[0][2]=100;
    raydium_light_position[0][3]=1.0;

    raydium_background_color_change(light_color[0],light_color[1],light_color[2],light_color[3]);

    
    raydium_clear_frame();

    raydium_camera_freemove(RAYDIUM_CAMERA_FREEMOVE_NORMAL);
    
    raydium_ode_draw_all(0);
    if(draw_debug>0)
       raydium_ode_draw_all(1);

    raydium_osd_logo("logo.tga");
    
    raydium_rendering_finish();
    raydium_ode_network_element_send_iterative(5);
}




int main(int argc, char **argv)
{
    char server[128];
    char ground[128];
    raydium_init_args(argc,argv);
    
    raydium_window_create(640,480,RAYDIUM_RENDERING_WINDOW,"Test 5 network viewer");
    raydium_texture_filter_change(RAYDIUM_TEXTURE_FILTER_TRILINEAR);
    raydium_projection_near=0.01;
    raydium_projection_far=2500;
    raydium_projection_fov=60;
    raydium_fog_disable();
    raydium_window_view_update();
    
    raydium_light_on(0);
    memcpy(raydium_light_color[0],light_color,raydium_internal_size_vector_float_4);
    raydium_light_intensity[0] = 1000000;
    raydium_light_update_all(0);
    
//    raydium_osd_cursor_set("BOXcursor.tga",4,4);

    raydium_window_view_update();

if(raydium_init_cli_option("server",server))
     if(!raydium_network_client_connect_to(server)) 
        exit(1);

if(raydium_init_cli_option("ground",ground))
    raydium_ode_ground_set_name(ground);
else
    raydium_ode_ground_set_name("a.tri");

    raydium_sky_box_cache();
    raydium_ode_object_box_add("boite",0,0.1,RAYDIUM_ODE_AUTODETECT,0,0,RAYDIUM_ODE_STANDARD,0,"crate.tri");
    raydium_ode_element_move_name_3f("boite",10,10,10);

    raydium_callback(&display);
    return(0);
}
// EOF
