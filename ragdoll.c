/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

// Small and DIRTY ragdoll demo - This file must be cleaned. A lot.

#include "raydium/index.c"

GLfloat light_color[] = {1.0, 0.9, 0.8, 1.0};
GLfloat  *back_color=light_color;

void create_ragdoll(void)
{
#define BONE_BREAK 0

dReal pos[3];
int a;

    raydium_ode_object_delete_name("RAGDOLL");
    a=raydium_ode_object_create("RAGDOLL");
    raydium_ode_object_sphere_add("head",a,0.1,RAYDIUM_ODE_AUTODETECT,RAYDIUM_ODE_STANDARD,0,"p_head.tri");
    raydium_ode_element_move_name_3f("head",5,0,1);

    raydium_ode_object_box_add("body",a,0.1,RAYDIUM_ODE_AUTODETECT,0,0,RAYDIUM_ODE_STANDARD,0,"p_body.tri");
    raydium_ode_element_move_name_3f("body",5,0,0.66);
    raydium_ode_joint_attach_hinge_name("neck","body","head",5,0,0.95,RAYDIUM_ODE_JOINT_AXE_X);
    raydium_ode_joint_hinge_limits_name("neck",-1,1);
    raydium_ode_joint_break_force_name("neck",BONE_BREAK);

    raydium_ode_object_box_add("u_leg",a,0.1,RAYDIUM_ODE_AUTODETECT,0,0,RAYDIUM_ODE_STANDARD,0,"p_u_leg.tri");
    raydium_ode_element_move_name_3f("u_leg",4.9,0,0.26);
    raydium_ode_element_rotate_name_3f("u_leg",0,-0.2,0);
    raydium_ode_joint_attach_hinge_name("femur","body","u_leg",4.9,0,0.42,RAYDIUM_ODE_JOINT_AXE_X);
    raydium_ode_joint_hinge_limits_name("femur",-1,1);
    raydium_ode_joint_break_force_name("femur",BONE_BREAK);

    raydium_ode_object_box_add("u_leg2",a,0.1,RAYDIUM_ODE_AUTODETECT,0,0,RAYDIUM_ODE_STANDARD,0,"p_u_leg2.tri");
    raydium_ode_element_move_name_3f("u_leg2",5.1,0,0.26);
    raydium_ode_element_rotate_name_3f("u_leg2",0,0.2,0);
    raydium_ode_joint_attach_hinge_name("femur2","body","u_leg2",5.1,0,0.42,RAYDIUM_ODE_JOINT_AXE_X);
    raydium_ode_joint_hinge_limits_name("femur2",-1,1);
    raydium_ode_joint_break_force_name("femur2",BONE_BREAK);


    raydium_ode_object_box_add("l_leg",a,0.1,RAYDIUM_ODE_AUTODETECT,0,0,RAYDIUM_ODE_STANDARD,0,"p_l_leg.tri");
    raydium_ode_element_move_name_3f("l_leg",4.82,-0.02,-0.14);
    raydium_ode_element_rotate_name_3f("l_leg",0,-0.1,0);
    raydium_ode_joint_attach_hinge_name("knee","u_leg","l_leg",4.85,0,0.05,RAYDIUM_ODE_JOINT_AXE_X);
    raydium_ode_joint_hinge_limits_name("knee",-2,0);
    raydium_ode_joint_break_force_name("knee",BONE_BREAK);

    raydium_ode_object_box_add("l_leg2",a,0.1,RAYDIUM_ODE_AUTODETECT,0,0,RAYDIUM_ODE_STANDARD,0,"p_l_leg2.tri");
    raydium_ode_element_move_name_3f("l_leg2",5.18,-0.02,-0.14);
    raydium_ode_element_rotate_name_3f("l_leg2",0,0.1,0);
    raydium_ode_joint_attach_hinge_name("knee2","u_leg2","l_leg2",5.15,0,0.05,RAYDIUM_ODE_JOINT_AXE_X);
    raydium_ode_joint_hinge_limits_name("knee2",-2,0);
    raydium_ode_joint_break_force_name("knee2",BONE_BREAK);

    raydium_ode_object_box_add("u_arm",a,0.1,RAYDIUM_ODE_AUTODETECT,0,0,RAYDIUM_ODE_STANDARD,0,"p_u_arm.tri");
    raydium_ode_element_move_name_3f("u_arm",4.83,0,0.73);
    raydium_ode_element_rotate_name_3f("u_arm",-0.1,-0.1,0);
    raydium_ode_joint_attach_hinge_name("shoulder","body","u_arm",4.85,0,0.84,RAYDIUM_ODE_JOINT_AXE_X);
    raydium_ode_joint_break_force_name("shoulder",BONE_BREAK);
//    raydium_ode_joint_hinge_limits_name("shoulder",-2,2);

    raydium_ode_object_box_add("u_arm2",a,0.1,RAYDIUM_ODE_AUTODETECT,0,0,RAYDIUM_ODE_STANDARD,0,"p_u_arm2.tri");
    raydium_ode_element_move_name_3f("u_arm2",5.17,0,0.73);
    raydium_ode_element_rotate_name_3f("u_arm2",-0.1,0.1,0);
    raydium_ode_joint_attach_hinge_name("shoulder2","body","u_arm2",5.15,0,0.84,RAYDIUM_ODE_JOINT_AXE_X);
    raydium_ode_joint_break_force_name("shoulder2",BONE_BREAK);
//    raydium_ode_joint_hinge_limits_name("shoulder2",-2,2);

    raydium_ode_object_box_add("l_arm",a,0.1,RAYDIUM_ODE_AUTODETECT,0,0,RAYDIUM_ODE_STANDARD,0,"p_l_arm.tri");
    raydium_ode_element_move_name_3f("l_arm",4.80,-0.05,0.45);
    raydium_ode_element_rotate_name_3f("l_arm",0.3,0,0);
    raydium_ode_joint_attach_hinge_name("elbow","u_arm","l_arm",4.80,0,0.60,RAYDIUM_ODE_JOINT_AXE_X);
    raydium_ode_joint_hinge_limits_name("elbow",0,2);
    raydium_ode_joint_break_force_name("elbow",BONE_BREAK);

    raydium_ode_object_box_add("l_arm2",a,0.1,RAYDIUM_ODE_AUTODETECT,0,0,RAYDIUM_ODE_STANDARD,0,"p_l_arm2.tri");
    raydium_ode_element_move_name_3f("l_arm2",5.20,-0.05,0.45);
    raydium_ode_element_rotate_name_3f("l_arm2",0.3,0,0);
    raydium_ode_joint_attach_hinge_name("elbow2","u_arm2","l_arm2",5.20,0,0.60,RAYDIUM_ODE_JOINT_AXE_X);
    raydium_ode_joint_hinge_limits_name("elbow2",0,2);
    raydium_ode_joint_break_force_name("elbow2",BONE_BREAK);


    raydium_ode_motor_create("knees",a,RAYDIUM_ODE_MOTOR_ANGULAR);
    raydium_ode_motor_attach_name("knees","knee",0);
    raydium_ode_motor_attach_name("knees","knee2",0);

    raydium_ode_motor_create("torso",a,RAYDIUM_ODE_MOTOR_ANGULAR);
    raydium_ode_motor_attach_name("torso","femur",0);
    raydium_ode_motor_attach_name("torso","femur2",0);

    pos[0]=0;
    pos[1]=0;
    pos[2]=0.7;
    raydium_ode_object_move_name("RAGDOLL",pos);

}

void display(void)
{

    raydium_joy_key_emul();

    if(raydium_key_last==1027)
        exit(0);

// won't work until you disable display lists
    if(raydium_key_last==3)
        raydium_light_disable();
    if(raydium_key_last==4)
        raydium_light_enable();

    if(raydium_key_last==1032)
        create_ragdoll();

if(raydium_ode_object_find("RAGDOLL")>=0)
{

#define MUSCLE_FORCE 0.6
//#define MUSCLE_FORCE 2
    if(raydium_key[GLUT_KEY_F1])
        {
        raydium_ode_motor_angle_name("knees",0);
        raydium_ode_motor_power_max_name("knees",MUSCLE_FORCE);
        }
        else
        {
        raydium_ode_motor_power_max_name("knees",0);
        }

    if(raydium_key[GLUT_KEY_F2])
        {
        raydium_ode_motor_angle_name("torso",0);
        raydium_ode_motor_power_max_name("torso",MUSCLE_FORCE);
        }
        else
        {
        raydium_ode_motor_power_max_name("torso",0);
        }

}


    raydium_light_position[0][0]=50;
    raydium_light_position[0][1]=150;
    raydium_light_position[0][2]=200;
    raydium_light_position[0][3]=1.0;

    raydium_clear_frame();

    raydium_camera_freemove(RAYDIUM_CAMERA_FREEMOVE_NORMAL);

    raydium_ode_draw_all(0);

//    raydium_osd_logo("logo.tga");
    raydium_osd_printf(2,98,16,0.5,"font2.tga","- %3i FPS - Ragdoll - Press ^cSPACE^f to spawn, and ^cF1^f/^cF2^f to control",raydium_render_fps);
    raydium_rendering_finish();
}




int main(int argc, char **argv)
{
    raydium_init_args(argc,argv);

    raydium_window_create(640,480,RAYDIUM_RENDERING_WINDOW,"Small dirty ragdoll demo");
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

//    raydium_osd_cursor_set("BOXcursor.tga",4,4);

    raydium_window_view_update();
    raydium_shadow_enable();
    raydium_background_color_change(back_color[0],back_color[1],back_color[2],back_color[3]);

    raydium_ode_ground_set_name("cocorobix.tri");
    raydium_callback(&display);
    return(0);
}
// EOF
