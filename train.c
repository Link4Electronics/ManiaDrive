/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

// Small demo of Raydium embedded PHP scripting, building a train
// using train.php script.

#include "raydium/index.c"

float force=0;

GLfloat light_color[] = {1.0, 0.9, 0.8, 1.0};


void display(void)
{
    
raydium_joy_key_emul();

if(raydium_key_last==1027)
    exit(0);

{
float speed=0;
if(raydium_key[GLUT_KEY_HOME]) speed=force;
if(raydium_key[GLUT_KEY_END]) speed=-force;
raydium_ode_motor_speed_name("train_moteur",speed);
}

if(raydium_key_last==1000+'a') raydium_ode_time_change(0);
if(raydium_key_last==1000+'z') raydium_ode_time_change(10);
if(raydium_key_last==1000+'e') raydium_ode_time_change(100);


if(raydium_key_last==1032)
    {
//    raydium_particle_generator_load("smoke.prt","smoke");
//    raydium_particle_generator_load("clouds.prt","clouds");
//    raydium_particle_generator_load("test.prt","durt_smoke");
//    raydium_particle_generator_load("fountain.prt","fountain");
//    raydium_particle_generator_load("explosion.prt","explo");
//    raydium_particle_generator_load("explosmoke.prt","explo_smoke");

//    raydium_particle_generator_move_name_3f("explo",1,0,0);
    raydium_php_exec("train.php");
    }

    
raydium_light_position[0][0]=100;
raydium_light_position[0][1]=100;
raydium_light_position[0][2]=100;
raydium_light_position[0][3]=1.0;
raydium_light_update_position(0); 

raydium_background_color_change(light_color[0],light_color[1],light_color[2],light_color[3]);

    
raydium_clear_frame();

//raydium_ode_element_camera_inboard_name("train_train",0,1,0,-3,0,-0.5);
//raydium_ode_element_camera_inboard_name("train_train",0,0.3,-0.3,-3,0,-0.5);
raydium_camera_freemove(RAYDIUM_CAMERA_FREEMOVE_NORMAL);
//raydium_object_draw_name("rail_world_background.tri");

raydium_ode_draw_all(0);
if(raydium_key[GLUT_KEY_F1])
    raydium_ode_draw_all(1);
//keys explanation on OSD
raydium_osd_color_rgba(1,1,1,1);
raydium_osd_printf(3,83,16,0.5,"font2.tga","Use keys 'a','z','e' to change the time callback");
raydium_osd_printf(3,87,16,0.5,"font2.tga","Use keys 'HOME','END' to move the train");
raydium_osd_printf(3,91,16,0.5,"font2.tga","Use key 'F1' to show debug ODE collision boxes");
raydium_osd_printf(3,95,16,0.5,"font2.tga","Use cursors and mouse to move the camera");
raydium_osd_logo("logo.tga");

raydium_rendering_finish();
}




int main(int argc, char **argv)
{
    raydium_init_args(argc,argv);
    
    raydium_window_create(640,480,RAYDIUM_RENDERING_WINDOW,"train's test");
    raydium_texture_filter_change(RAYDIUM_TEXTURE_FILTER_TRILINEAR);
    raydium_projection_near=0.01;
    raydium_projection_far=2500;
    raydium_projection_fov=60;
    raydium_fog_disable();
    raydium_window_view_update();
    
//    raydium_light_disable();
    raydium_light_on(0);
    memcpy(raydium_light_color[0],light_color,raydium_internal_size_vector_float_4);
    raydium_light_intensity[0] = 1000000;
    raydium_light_update_all(0);
    
//    raydium_osd_cursor_set("BOXcursor.tga",4,4);

    raydium_window_view_update();

    raydium_ode_ground_set_name("rail_world_full.tri");
    raydium_register_variable(&force,RAYDIUM_REGISTER_FLOAT,"force");

    // preload tri files and textures
    raydium_object_find_load("train1.tri");
    raydium_object_find_load("train_bw.tri");
    raydium_object_find_load("wagon.tri");
    raydium_object_find_load("train_a.tri");

    raydium_php_exec("train.php");

    raydium_shadow_enable();

    raydium_callback(&display);
    return(0);
}
// EOF
