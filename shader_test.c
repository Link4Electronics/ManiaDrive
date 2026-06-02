/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

#include "raydium/index.c"

dReal *lpos;

void display(void)
{
raydium_joy_key_emul();

lpos=raydium_ode_element_pos_get_name("light");
raydium_light_move_3f(0,lpos[0],lpos[1],lpos[2]);

if(raydium_key_last==1027)
    exit(0);

if(raydium_key[GLUT_KEY_F1]) { raydium_projection_fov/=(1.04); raydium_window_view_update(); }
if(raydium_key[GLUT_KEY_F2]) { raydium_projection_fov*=(1.04); raydium_window_view_update(); }

raydium_clear_frame();
raydium_camera_freemove(RAYDIUM_CAMERA_FREEMOVE_NORMAL);
raydium_ode_draw_all(0);
raydium_rendering_finish();
}


int main(int argc, char **argv)
{
int obj;

raydium_init_args(argc,argv);
raydium_window_create(640,480,RAYDIUM_RENDERING_WINDOW,"Small shader test");

raydium_texture_filter_change(RAYDIUM_TEXTURE_FILTER_TRILINEAR);
raydium_window_view_perspective(60,0.01,2500); // fov 60 + near and far planes

raydium_fog_disable();
raydium_light_enable();
raydium_light_on(0);

raydium_light_conf_7f(0,0,0,0,1000000,1,0.9,0.7); // id, pos, intensity and color (RGB)
raydium_background_color_change(1,0.9,0.7,1);

raydium_sky_box_cache();

raydium_camera_freemove_speed=0.01;

raydium_ode_ground_set_name("crate_parabump.tri");
obj=raydium_ode_object_find("GLOBAL");

raydium_ode_object_box_add("coch",obj,1,RAYDIUM_ODE_AUTODETECT,RAYDIUM_ODE_AUTODETECT,RAYDIUM_ODE_AUTODETECT,RAYDIUM_ODE_STANDARD,0,"coch2.tri");
raydium_ode_element_move_name_3f("coch",0,-0.2,-0.1);

raydium_ode_object_box_add("head",obj,1,RAYDIUM_ODE_AUTODETECT,RAYDIUM_ODE_AUTODETECT,RAYDIUM_ODE_AUTODETECT,RAYDIUM_ODE_STANDARD,0,"derhead.tri");
raydium_ode_element_move_name_3f("head",0.3,0.6,-0.25);
raydium_object_tangent_smooth_name("derhead.tri");

raydium_ode_object_box_add("headsmooth",obj,1,RAYDIUM_ODE_AUTODETECT,RAYDIUM_ODE_AUTODETECT,RAYDIUM_ODE_AUTODETECT,RAYDIUM_ODE_STANDARD,0,"derhead_smooth.tri");
raydium_ode_element_move_name_3f("headsmooth",0,0.6,-0.25);

raydium_ode_object_box_add("headflat",obj,1,RAYDIUM_ODE_AUTODETECT,RAYDIUM_ODE_AUTODETECT,RAYDIUM_ODE_AUTODETECT,RAYDIUM_ODE_STANDARD,0,"derhead_flat.tri");
raydium_ode_element_move_name_3f("headflat",-0.3,0.6,-0.25);

raydium_ode_object_sphere_add("light",obj,0.01,RAYDIUM_ODE_AUTODETECT,RAYDIUM_ODE_STANDARD,0,"ballon.tri");
raydium_ode_element_move_name_3f("light",0,0,0.25);
raydium_ode_joint_attach_universal("chand",RAYDIUM_ODE_JOINT_FIXED,raydium_ode_element_find("light"),0,0,0.5, 1,0,0, 0,1,0);
raydium_ode_element_addforce_name_3f("light",10,1,0);

raydium_shader_load("shader1","toon.vert","toon.frag");
raydium_shader_var_f_name("shader1","Edge",0.64);
raydium_shader_var_f_name("shader1","Phong",0.954);
raydium_shader_var_3f_name("shader1","DiffuseColor",0.35,0,0.25);
raydium_shader_var_3f_name("shader1","PhongColor",0.94,0.7,0.85);

raydium_shader_load("shader2","bump.vert","bump.frag");
raydium_shader_var_i_name("shader2","normalMap",0);
raydium_shader_var_i_name("shader2","colorMap",1);
raydium_shader_var_4f_name("shader2","specular",0.1,0.1,0.1,1);
raydium_shader_var_f_name("shader2","shininess",30);

raydium_shader_load("shader3","parabump.vert","parabump.frag");
raydium_shader_var_i_name("shader3","Normal",0);
raydium_shader_var_i_name("shader3","Base_Height",1);
raydium_shader_var_f_name("shader3","u_invRad",0.03);
raydium_shader_var_2f_name("shader3","cBumpSize",0.042,-0.050);
raydium_shader_var_i_name("shader3","base_tex",2);

raydium_shader_attach_texture_name("shader1","rgb(0.94,0.7,0.85)");
raydium_shader_attach_texture_name("shader2","crate_nmap.tga");
raydium_shader_attach_texture_name("shader2","derhead_normal.tga");
raydium_shader_attach_texture_name("shader3","wallNormalMap.tga");

raydium_shadow_enable();
raydium_shadow_camerabox_size(5);

raydium_callback(&display);
return 0;
}
// EOF
