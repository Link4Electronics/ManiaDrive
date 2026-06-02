/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

// Small RayODE car creation and explosion. Use SPACE and BACKSPACE.

#include "raydium/index.c"

GLfloat sun[]={1.0,0.9,0.5,1.0};
int sound_boom;
int sound_car;

void create_car(void)
{
int a;

#define BREAK_FORCE     200
#define ROTFRICTION     0.0005
#define ERP_CFM         0.3,0.8

raydium_ode_object_delete_name("WATURE");

a=raydium_ode_object_create("WATURE");
raydium_ode_object_box_add("corps",a,1,1.2,0.6,0.4,RAYDIUM_ODE_STANDARD,0,"clio.tri");
raydium_ode_element_slip_name("corps",RAYDIUM_ODE_SLIP_ICE);

raydium_ode_object_sphere_add("pneu_ag",a,0.5,RAYDIUM_ODE_AUTODETECT,RAYDIUM_ODE_STANDARD,0,"roue5.tri");
raydium_ode_element_rotfriction_name("pneu_ag",ROTFRICTION);
raydium_ode_element_move_name_3f("pneu_ag",0.42,0.253,-0.180);
raydium_ode_joint_attach_hinge2_name("suspet_ag","corps","pneu_ag",RAYDIUM_ODE_JOINT_SUSP_DEFAULT_AXES);
raydium_ode_joint_break_force_name("suspet_ag",BREAK_FORCE);
raydium_ode_joint_suspension_name("suspet_ag",ERP_CFM);

raydium_ode_object_sphere_add("pneu_ad",a,0.5,RAYDIUM_ODE_AUTODETECT,RAYDIUM_ODE_STANDARD,0,"roue5.tri");
raydium_ode_element_rotfriction_name("pneu_ad",ROTFRICTION);
raydium_ode_element_move_name_3f("pneu_ad",0.42,-0.253,-0.180);
raydium_ode_joint_attach_hinge2_name("suspet_ad","corps","pneu_ad",RAYDIUM_ODE_JOINT_SUSP_DEFAULT_AXES);
raydium_ode_joint_break_force_name("suspet_ad",BREAK_FORCE);
raydium_ode_joint_suspension_name("suspet_ad",ERP_CFM);

raydium_ode_object_sphere_add("pneu_rg",a,0.5,RAYDIUM_ODE_AUTODETECT,RAYDIUM_ODE_STANDARD,0,"roue5.tri");
raydium_ode_element_rotfriction_name("pneu_rg",ROTFRICTION);
raydium_ode_element_move_name_3f("pneu_rg",-0.444,0.253,-0.180);
raydium_ode_joint_attach_hinge2_name("suspet_rg","corps","pneu_rg",RAYDIUM_ODE_JOINT_SUSP_DEFAULT_AXES);
raydium_ode_joint_hinge2_block_name("suspet_rg",1);
raydium_ode_joint_break_force_name("suspet_rg",BREAK_FORCE);
raydium_ode_joint_suspension_name("suspet_rg",ERP_CFM);

raydium_ode_object_sphere_add("pneu_rd",a,0.5,RAYDIUM_ODE_AUTODETECT,RAYDIUM_ODE_STANDARD,0,"roue5.tri");
raydium_ode_element_rotfriction_name("pneu_rd",ROTFRICTION);
raydium_ode_element_move_name_3f("pneu_rd",-0.444,-0.253,-0.180);
raydium_ode_joint_attach_hinge2_name("suspet_rd","corps","pneu_rd",RAYDIUM_ODE_JOINT_SUSP_DEFAULT_AXES);
raydium_ode_joint_hinge2_block_name("suspet_rd",1);
raydium_ode_joint_break_force_name("suspet_rd",BREAK_FORCE);
raydium_ode_joint_suspension_name("suspet_rd",ERP_CFM);

raydium_ode_motor_create("moteur",a,RAYDIUM_ODE_MOTOR_ENGINE);
raydium_ode_motor_attach_name("moteur","suspet_ag",1);
raydium_ode_motor_attach_name("moteur","suspet_ad",1);
raydium_ode_motor_power_max_name("moteur",0.1);
//raydium_ode_motor_gears_set_name("moteur",gears,7);

raydium_ode_motor_create("direction",a,RAYDIUM_ODE_MOTOR_ANGULAR);
raydium_ode_motor_attach_name("direction","suspet_ag",0);
raydium_ode_motor_attach_name("direction","suspet_ad",0);
raydium_ode_motor_power_max_name("direction",0.2);

//raydium_ode_object_move_name_3f("WATURE",0,0,1);
raydium_sound_SourcePlay(sound_car);
}

void create_car2(void)
{
int a;
dQuaternion rot;
dReal *pos;

if(raydium_ode_element_find("corps")<0) return;

a=raydium_ode_object_create("WATURE2");

raydium_ode_object_box_add("body",a,1,RAYDIUM_ODE_AUTODETECT,0,0,RAYDIUM_ODE_STANDARD,0,"clio_xp_body.tri");

raydium_ode_object_box_add("top",a,1,RAYDIUM_ODE_AUTODETECT,0,0,RAYDIUM_ODE_STANDARD,0,"clio_xp_top.tri");
raydium_ode_element_move_name_3f("top",-0.1,0,0.25);

raydium_ode_object_box_add("front",a,1,RAYDIUM_ODE_AUTODETECT,0,0,RAYDIUM_ODE_STANDARD,0,"clio_xp_front.tri");
raydium_ode_element_move_name_3f("front",0.45,0,0.08);

raydium_ode_object_box_add("back",a,1,RAYDIUM_ODE_AUTODETECT,0,0,RAYDIUM_ODE_STANDARD,0,"clio_xp_back.tri");
raydium_ode_element_move_name_3f("back",-0.52,0,0.1);

raydium_ode_object_box_add("left",a,1,RAYDIUM_ODE_AUTODETECT,0,0,RAYDIUM_ODE_STANDARD,0,"clio_xp_left.tri");
raydium_ode_element_move_name_3f("left",0.05,0.23,0.1);

raydium_ode_object_box_add("right",a,1,RAYDIUM_ODE_AUTODETECT,0,0,RAYDIUM_ODE_STANDARD,0,"clio_xp_right.tri");
raydium_ode_element_move_name_3f("right",0.05,-0.23,0.1);

raydium_ode_object_sphere_add("wheel_ag",a,0.5,RAYDIUM_ODE_AUTODETECT,RAYDIUM_ODE_STANDARD,0,"roue5.tri");
raydium_ode_element_move_name_3f("wheel_ag",0.42,0.253,-0.1);

raydium_ode_object_sphere_add("wheel_ad",a,0.5,RAYDIUM_ODE_AUTODETECT,RAYDIUM_ODE_STANDARD,0,"roue5.tri");
raydium_ode_element_move_name_3f("wheel_ad",0.42,-0.253,-0.1);

raydium_ode_object_sphere_add("wheel_rg",a,0.5,RAYDIUM_ODE_AUTODETECT,RAYDIUM_ODE_STANDARD,0,"roue5.tri");
raydium_ode_element_move_name_3f("wheel_rg",-0.444,0.253,-0.1);

raydium_ode_object_sphere_add("wheel_rd",a,0.5,RAYDIUM_ODE_AUTODETECT,RAYDIUM_ODE_STANDARD,0,"roue5.tri");
raydium_ode_element_move_name_3f("wheel_rd",-0.444,-0.253,-0.1);


raydium_ode_element_rotq_get_name("corps",rot);
pos=raydium_ode_element_pos_get_name("corps");

raydium_ode_object_move_name_3f("WATURE2",pos[0],pos[1],pos[2]);
raydium_ode_object_rotateq_name("WATURE2",rot);

// Get linear velocity and replicate it to WATURE2
pos=raydium_ode_element_linearvelocity_get_name("corps");
raydium_ode_object_linearvelocity_set_name("WATURE2",pos);

raydium_ode_object_delete_name("WATURE");
raydium_ode_object_rename_name("WATURE2","WATURE");

pos=raydium_ode_element_pos_get_name("body");
raydium_ode_explosion_blow_rand_3f(5,200,0.12,pos[0]+0.01,pos[1]+0.01,pos[2]); // bonne explosion
raydium_ode_element_particle_point_name("body","explosion.prt");
raydium_ode_element_particle_point_name("body","explosmoke.prt");
raydium_ode_element_particle_point_name("body","exploparts.prt");

raydium_sound_SourceStop(sound_car);
raydium_sound_SourcePlay(sound_boom);
raydium_ode_element_sound_update_name("body",sound_boom);
}


void display(void)
{
float speed,direct;
dReal *pos;
dReal def[]={0,0,0};
raydium_joy_key_emul();

direct=raydium_joy_x*0.3;
speed=raydium_joy_y*55;


if(raydium_ode_element_find("corps")>=0)
    {
    raydium_ode_motor_speed_name("moteur",-speed);
    raydium_ode_motor_angle_name("direction",direct);

    speed=raydium_ode_motor_speed_get_name("moteur",1);
    speed*=0.1; // wheel radius
    speed/=4;
    speed+=0.5;
    speed+=(raydium_random_neg_pos_1()/15);
    raydium_sound_SetSourcePitch(sound_car,raydium_math_abs(speed));
    raydium_ode_element_sound_update_name("corps",sound_car);
    }


if(raydium_key_last==1032) create_car2();
if(raydium_key_last==1008) create_car();


if(raydium_key_last==1097) raydium_ode_time_change(0);
if(raydium_key_last==1122) raydium_ode_time_change(10);
if(raydium_key_last==1101) raydium_ode_time_change(100);

if(raydium_key_last==1027) exit(0);


raydium_clear_frame();

if(raydium_ode_element_find("body")>=0)
    pos=raydium_ode_element_pos_get_name("body");
else if(raydium_ode_element_find("corps")>=0)
    pos=raydium_ode_element_pos_get_name("corps");
else
    pos=def;

raydium_camera_look_at(1,1,1,pos[1],-pos[2],pos[0]);
raydium_ode_draw_all(0);

raydium_rendering_finish();
}


int main(int argc, char **argv)
{
raydium_init_args(argc,argv);
raydium_window_create(512,384,RAYDIUM_RENDERING_WINDOW,"RayODE car explosion demo");
raydium_texture_filter_change(RAYDIUM_TEXTURE_FILTER_TRILINEAR);
raydium_projection_near=0.1;
raydium_projection_far=10000;
raydium_projection_fov=70;
raydium_window_view_update();

raydium_light_on(0);
memcpy(raydium_light_color[0],sun,raydium_internal_size_vector_float_4);
raydium_light_intensity[0]=1000000;

raydium_light_position[0][0]=50;
raydium_light_position[0][1]=150;
raydium_light_position[0][2]=200;

raydium_light_update_all(0);
raydium_background_color_change(sun[0],sun[1],sun[2],sun[3]);

raydium_fog_disable();
raydium_shadow_enable();

raydium_sound_DefaultReferenceDistance=4.f;
sound_car=raydium_sound_LoadWav("murcielago.wav");
raydium_sound_SetSourcePitch(sound_car,0);
raydium_sound_SetSourceGain(sound_car,0.05);  // Engine Gain

sound_boom=raydium_sound_LoadWav("explosion1.wav");
raydium_sound_SetSourceLoop(sound_boom,0);

raydium_ode_ground_set_name("cocorobix.tri");
create_car();

raydium_callback(&display);

return 0;
}


