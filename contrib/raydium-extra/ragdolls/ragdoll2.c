#include "raydium/index.c"

GLfloat cam_angle_x = 0;
GLfloat cam_angle_y = 90;

GLfloat cam_pos_x = 0;
GLfloat cam_pos_y = 0;
GLfloat cam_pos_z = 0;

GLfloat speed = 0.1;
GLint sensibilite = 3;

GLint lacet = 0;

GLfloat light_color[] = {1.0, 0.9, 0.8, 1.0};
//    GLfloat back_color[] = {1, 1, 1, 1};
GLfloat  *back_color=light_color;
//    GLfloat light_color[] = {0, 0, 0, 1};





int create_ragdoll(void)
{
#define BONE_BREAK 0

//dReal pos[3];
int obj;
int head,body,u_leg,u_leg2,l_leg,l_leg2,u_arm,u_arm2,l_arm,l_arm2;
int jneck,jfemur,jfemur2,jknee,jknee2,jshoulder,jshoulder2,jelbow,jelbow2;
int mknees,mtorso,mneck;
char name[128];


//    raydium_ode_object_delete_name("RAGDOLL");
    raydium_ode_name_auto("RAGDOLL",name);
    obj=raydium_ode_object_create(name);
    raydium_ode_name_auto("head",name);
    head=raydium_ode_object_sphere_add(name,obj,0.1,RAYDIUM_ODE_AUTODETECT,RAYDIUM_ODE_STANDARD,0,"p_head.tri");
    raydium_ode_element_move_3f(head,5,0,1);

    raydium_ode_name_auto("body",name);
    body=raydium_ode_object_box_add(name,obj,0.01,RAYDIUM_ODE_AUTODETECT,0,0,RAYDIUM_ODE_STANDARD,0,"p_body.tri");
    raydium_ode_element_move_3f(body,5,0,0.66);
    raydium_ode_name_auto("neck",name);
    jneck=raydium_ode_joint_attach_hinge(name,body,head,5,0,0.95,RAYDIUM_ODE_JOINT_AXE_X);
    raydium_ode_joint_hinge_limits(jneck,-1,1);
    raydium_ode_joint_break_force(jneck,BONE_BREAK);
//    raydium_ode_joint_delete_callback_name("neck",bone_break);

    raydium_ode_name_auto("u_leg",name);
    u_leg=raydium_ode_object_box_add(name,obj,0.1,RAYDIUM_ODE_AUTODETECT,0,0,RAYDIUM_ODE_STANDARD,0,"p_u_leg.tri");
    raydium_ode_element_move_3f(u_leg,4.9,0,0.26);
    raydium_ode_element_rotate_3f(u_leg,0,-0.2,0);
    raydium_ode_name_auto("femur",name);
    jfemur=raydium_ode_joint_attach_hinge(name,body,u_leg,4.9,0,0.42,RAYDIUM_ODE_JOINT_AXE_X);
    raydium_ode_joint_hinge_limits(jfemur,-1,1);
    raydium_ode_joint_break_force(jfemur,BONE_BREAK);
//    raydium_ode_joint_delete_callback_name("femur",bone_break);

    raydium_ode_name_auto("u_leg2",name);
    u_leg2=raydium_ode_object_box_add(name,obj,0.1,RAYDIUM_ODE_AUTODETECT,0,0,RAYDIUM_ODE_STANDARD,0,"p_u_leg2.tri");
    raydium_ode_element_move_3f(u_leg2,5.1,0,0.26);
    raydium_ode_element_rotate_3f(u_leg2,0,0.2,0);
    raydium_ode_name_auto("femur2",name);
    jfemur2=raydium_ode_joint_attach_hinge(name,body,u_leg2,5.1,0,0.42,RAYDIUM_ODE_JOINT_AXE_X);
    raydium_ode_joint_hinge_limits(jfemur2,-1,1);
    raydium_ode_joint_break_force(jfemur2,BONE_BREAK);
//    raydium_ode_joint_delete_callback_name("femur2",bone_break);


    raydium_ode_name_auto("l_leg",name);
    l_leg=raydium_ode_object_box_add(name,obj,0.1,RAYDIUM_ODE_AUTODETECT,0,0,RAYDIUM_ODE_STANDARD,0,"p_l_leg.tri");
    raydium_ode_element_move_3f(l_leg,4.82,-0.02,-0.14);
    raydium_ode_element_rotate_3f(l_leg,0,-0.1,0);
    raydium_ode_element_slip(l_leg,RAYDIUM_ODE_SLIP_ICE);
    raydium_ode_name_auto("knee",name);
    jknee=raydium_ode_joint_attach_hinge(name,u_leg,l_leg,4.85,0,0.05,RAYDIUM_ODE_JOINT_AXE_X);
    raydium_ode_joint_hinge_limits(jknee,-2,0);
    raydium_ode_joint_break_force(jknee,BONE_BREAK);
//    raydium_ode_joint_delete_callback_name("knee",bone_break);

    raydium_ode_name_auto("u_leg2",name);
    l_leg2=raydium_ode_object_box_add(name,obj,0.1,RAYDIUM_ODE_AUTODETECT,0,0,RAYDIUM_ODE_STANDARD,0,"p_l_leg2.tri");
    raydium_ode_element_move_3f(l_leg2,5.18,-0.02,-0.14);
    raydium_ode_element_rotate_3f(l_leg2,0,0.1,0);
    raydium_ode_element_slip(l_leg2,RAYDIUM_ODE_SLIP_ICE);
    raydium_ode_name_auto("knee2",name);
    jknee2=raydium_ode_joint_attach_hinge(name,u_leg2,l_leg2,5.15,0,0.05,RAYDIUM_ODE_JOINT_AXE_X);
    raydium_ode_joint_hinge_limits(jknee2,-2,0);
    raydium_ode_joint_break_force(jknee2,BONE_BREAK);
//    raydium_ode_joint_delete_callback_name("knee2",bone_break);

    raydium_ode_name_auto("u_arm",name);
    u_arm=raydium_ode_object_box_add(name,obj,0.1,RAYDIUM_ODE_AUTODETECT,0,0,RAYDIUM_ODE_STANDARD,0,"p_u_arm.tri");
    raydium_ode_element_move_3f(u_arm,4.83,0,0.73);
    raydium_ode_element_rotate_3f(u_arm,-0.1,-0.1,0);
    raydium_ode_name_auto("shoulder",name);
    jshoulder=raydium_ode_joint_attach_hinge(name,body,u_arm,4.85,0,0.84,RAYDIUM_ODE_JOINT_AXE_X);
    raydium_ode_joint_break_force(jshoulder,BONE_BREAK);
//    raydium_ode_joint_delete_callback_name("shoulder",bone_break);
//    raydium_ode_joint_hinge_limits_name("shoulder",-2,2);

    raydium_ode_name_auto("u_arm2",name);
    u_arm2=raydium_ode_object_box_add(name,obj,0.1,RAYDIUM_ODE_AUTODETECT,0,0,RAYDIUM_ODE_STANDARD,0,"p_u_arm2.tri");
    raydium_ode_element_move_3f(u_arm2,5.17,0,0.73);
    raydium_ode_element_rotate_3f(u_arm2,-0.1,0.1,0);
    raydium_ode_name_auto("shoulder2",name);
    jshoulder2=raydium_ode_joint_attach_hinge(name,body,u_arm2,5.15,0,0.84,RAYDIUM_ODE_JOINT_AXE_X);
    raydium_ode_joint_break_force(jshoulder2,BONE_BREAK);
//    raydium_ode_joint_delete_callback_name("shoulder2",bone_break);
//    raydium_ode_joint_hinge_limits_name("shoulder2",-2,2);

    raydium_ode_name_auto("l_arm",name);
    l_arm=raydium_ode_object_box_add(name,obj,0.1,RAYDIUM_ODE_AUTODETECT,0,0,RAYDIUM_ODE_STANDARD,0,"p_l_arm.tri");
    raydium_ode_element_move_3f(l_arm,4.80,-0.05,0.45);
    raydium_ode_element_rotate_3f(l_arm,0.3,0,0);
    raydium_ode_name_auto("elbow",name);
    jelbow=raydium_ode_joint_attach_hinge(name,u_arm,l_arm,4.80,0,0.60,RAYDIUM_ODE_JOINT_AXE_X);
    raydium_ode_joint_hinge_limits(jelbow,0,2);
    raydium_ode_joint_break_force(jelbow,BONE_BREAK);
//    raydium_ode_joint_delete_callback_name("elbow",bone_break);

    raydium_ode_name_auto("l_arm2",name);
    l_arm2=raydium_ode_object_box_add(name,obj,0.1,RAYDIUM_ODE_AUTODETECT,0,0,RAYDIUM_ODE_STANDARD,0,"p_l_arm2.tri");
    raydium_ode_element_move_3f(l_arm2,5.20,-0.05,0.45);
    raydium_ode_element_rotate_3f(l_arm2,0.3,0,0);
    raydium_ode_name_auto("elbow2",name);
    jelbow2=raydium_ode_joint_attach_hinge(name,u_arm2,l_arm2,5.20,0,0.60,RAYDIUM_ODE_JOINT_AXE_X);
    raydium_ode_joint_hinge_limits(jelbow2,0,2);
    raydium_ode_joint_break_force(jelbow2,BONE_BREAK);
//    raydium_ode_joint_delete_callback_name("elbow2",bone_break);


    raydium_ode_name_auto("knees",name);
    mknees=raydium_ode_motor_create(name,obj,RAYDIUM_ODE_MOTOR_ANGULAR);
    raydium_ode_motor_attach(mknees,jknee,0);
    raydium_ode_motor_attach(mknees,jknee2,0);

    raydium_ode_name_auto("torso",name);
    mtorso=raydium_ode_motor_create(name,obj,RAYDIUM_ODE_MOTOR_ANGULAR);
    raydium_ode_motor_attach(mtorso,jfemur,0);
    raydium_ode_motor_attach(mtorso,jfemur2,0);

    raydium_ode_name_auto("neck",name);
    mneck=raydium_ode_motor_create(name,obj,RAYDIUM_ODE_MOTOR_ANGULAR);
    raydium_ode_motor_attach(mneck,jneck,0);

#define MUSCLE_FORCE 0.02
    raydium_ode_motor_angle(mknees,0);
    raydium_ode_motor_angle(mtorso,0);
    raydium_ode_motor_angle(mneck,0);
    raydium_ode_motor_power_max(mknees,MUSCLE_FORCE);
    raydium_ode_motor_power_max(mtorso,MUSCLE_FORCE);
    raydium_ode_motor_power_max(mneck,MUSCLE_FORCE);

//    pos[0]=x;
//    pos[1]=y;
//    pos[2]=-0.4;
//    raydium_ode_object_move(obj,pos);

return obj;
}


void read_world(char *filename)
{
FILE *fp;
int v;
dReal p[3],r[3];
char ent[128];
int id;

fp=raydium_file_fopen(filename,"rt");
fscanf(fp,"%i\n",&v);
while(fscanf(fp,"%f %f %f %f %f %f %s\n",&p[0],&p[1],&p[2],&r[0],&r[1],&r[2],ent)!=EOF)
    {
    switch(ent[0])
	{
	case 'r':
	    raydium_log("+ ragdoll");
	    id=create_ragdoll();
	    raydium_ode_object_move(id,p);
	    //raydium_ode_object_rotate(id,r);
	    break;
	case 'b':
	    raydium_log("+ crate");
	    raydium_ode_name_auto("crate",ent);
	    id=raydium_ode_object_box_add(ent,raydium_ode_object_find("GLOBAL"),0.1,RAYDIUM_ODE_AUTODETECT,0,0,RAYDIUM_ODE_STANDARD,0,"crate.tri");
	    raydium_ode_element_move(id,p);
	    //raydium_ode_object_rotate(id,r);
	    break;
	case 'g':
	    raydium_log("+ gaz");
	    raydium_ode_name_auto("gaz",ent);
	    id=raydium_ode_object_box_add(ent,raydium_ode_object_find("GLOBAL"),0.1,RAYDIUM_ODE_AUTODETECT,0,0,RAYDIUM_ODE_STANDARD,0,"bombone_gaz.tri");
	    raydium_ode_element_move(id,p);
	    //raydium_ode_object_rotate(id,r);
	    break;

	}
    }

fclose(fp);
}


void display(void)
{

    int delta_x, delta_y;
    raydium_joy_key_emul();

    cam_pos_z += (raydium_trigo_sin(cam_angle_x+90)*raydium_joy_y*speed*raydium_trigo_sin(90-cam_angle_y));
    cam_pos_x += (raydium_trigo_cos(cam_angle_x+90)*raydium_joy_y*speed*raydium_trigo_sin(90-cam_angle_y));
    cam_pos_y += (raydium_trigo_cos(90-cam_angle_y)*speed*raydium_joy_y);

    cam_pos_x -= (raydium_trigo_cos(cam_angle_x)*raydium_joy_x*speed);
    cam_pos_z -= (raydium_trigo_sin(cam_angle_x)*raydium_joy_x*speed);

    if(raydium_key_last==1027)
	exit(0);


    if(raydium_key_last==1)
	raydium_ode_time_change(10);
    if(raydium_key_last==2)
	raydium_ode_time_change(100);
    if(raydium_key_last==3)
	{
	dReal pos[3];
	char name1[128];
	char name2[128];
	int g1,g2;

	pos[0]=0;
	pos[1]=0;
	pos[2]=-0.6;
	raydium_ode_explosion_blow(4,80,pos);

	raydium_particle_name_auto("explo",name1);
	raydium_particle_name_auto("eplo_smoke",name2);
	g1=raydium_particle_generator_load("explosion.prt",name1);
	g2=raydium_particle_generator_load("explosmoke.prt",name2);

	raydium_particle_generator_move(g1,pos);
	raydium_particle_generator_move(g2,pos);
	}

//    if(raydium_key_last==1032)
//	create_ragdoll(0,1);


    delta_x = raydium_mouse_x - (raydium_window_tx/2);
    cam_angle_x += (delta_x*sensibilite*0.1f);

    delta_y = raydium_mouse_y - (raydium_window_ty/2);
    cam_angle_y += (delta_y*sensibilite*0.1f);

    raydium_mouse_move(raydium_window_tx/2, raydium_window_ty/2);

    raydium_light_position[0][0]=50;
    raydium_light_position[0][1]=150;
    raydium_light_position[0][2]=200;
    raydium_light_position[0][3]=1.0;

    raydium_clear_frame();
    raydium_camera_place(cam_pos_x,cam_pos_y,cam_pos_z,cam_angle_x,cam_angle_y,0);
    raydium_camera_replace();

    raydium_ode_draw_all(0);
//    raydium_ode_draw_all(1);

//    raydium_osd_logo("logo.tga");
    raydium_rendering_finish();
    raydium_ode_network_element_send_iterative(RAYDIUM_ODE_NETWORK_OPTIMAL);
}




int main(int argc, char **argv)
{
    char server[128];

    raydium_init_args(argc,argv);

    // creation de la fenetre et de la scene
    raydium_window_create(640,480,RAYDIUM_RENDERING_WINDOW,"ragdoll test");
    raydium_texture_filter_change(RAYDIUM_TEXTURE_FILTER_TRILINEAR);
    raydium_compat_add(RAYDIUM_COMPAT_ODE_OBJECT_MOVE_R1099);
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

    raydium_shadow_enable();
    if(raydium_init_cli_option("server",server))
	if(!raydium_network_client_connect_to(server))
	    exit(1);

//    raydium_osd_cursor_set("BOXcursor.tga",4,4);

    raydium_window_view_update();
    raydium_background_color_change(back_color[0],back_color[1],back_color[2],back_color[3]);

    raydium_register_variable(&speed, RAYDIUM_REGISTER_FLOAT, "speed");

    raydium_ode_ground_set_name("hangarl.tri");
    read_world("hangarl.ent");
    raydium_ode_time_change(0);
    raydium_callback(&display);
    return(0);
}
// EOF
