/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

char *version="version 0.1";

#include "raydium/index.c"


#define GROUND_SLIP		0.1f
#define TANK_TRACK_SLIP 	0.1f

GLfloat cam_angle_x = 0;
GLfloat cam_angle_y = 90;

GLfloat cam_pos_x = 0;
GLfloat cam_pos_y = 0;
GLfloat cam_pos_z = 0;


GLfloat light_color[] = {1.0, 0.9, 0.8, 1.0};

int sound_car;


void simul_create()
{

int a;
char c[4];
int i;

#define BREAK_FORCE     200

#define ERP_CFM         0.8,0.1 //#define ERP_CFM         0.1,1.0
#define ROTFRICTION     0.001 //#define ROTFRICTION     0.05



    raydium_ode_object_delete_name("TANK");

    a=raydium_ode_object_create("TANK");
    raydium_ode_object_box_add("tank_corps",a,10,RAYDIUM_ODE_AUTODETECT,0,0,RAYDIUM_ODE_STANDARD,0,"t80b.tri");
    //raydium_ode_element_move_name_3f("tank_corps",-2,0,0+0.5);

    raydium_ode_motor_create("tank_moteurG",a,RAYDIUM_ODE_MOTOR_ENGINE);
    raydium_ode_motor_power_max_name("tank_moteurG",1);

    raydium_ode_motor_create("tank_moteurD",a,RAYDIUM_ODE_MOTOR_ENGINE);
    raydium_ode_motor_power_max_name("tank_moteurD",1);

    c[0]='H';
    c[1]='G';
    c[3]=0;
    for (i=0;i<6;i++) {
        c[2]='0'+i;
        raydium_ode_object_sphere_add(&c[1],a,1,0.140,RAYDIUM_ODE_STANDARD,0,"");
        raydium_ode_element_rotfriction_name(&c[1],ROTFRICTION);
        raydium_ode_element_move_name_3f(&c[1],0.930-((1.030+0.760)/6.0)*i,0.425,-0.240);
        raydium_ode_joint_attach_hinge_name(c,"tank_corps",&c[1],0.930-((1.030+0.760)/6.0)*i,0.425,-0.240,RAYDIUM_ODE_JOINT_AXE_Y);
        raydium_ode_motor_attach_name("tank_moteurG",c,0);
        raydium_ode_element_slip_name (&c[1],TANK_TRACK_SLIP);
    }

    c[0]='H';
    c[1]='D';
    c[3]=0;
    for (i=0;i<6;i++) {
        c[2]='0'+i;
        raydium_ode_object_sphere_add(&c[1],a,1,0.140,RAYDIUM_ODE_STANDARD,0,"");
        raydium_ode_element_rotfriction_name(&c[1],ROTFRICTION);
        raydium_ode_element_move_name_3f(&c[1],0.930-((1.030+0.760)/6.0)*i,-0.425,-0.240);
        raydium_ode_joint_attach_hinge_name(c,"tank_corps",&c[1],0.930-((1.030+0.760)/6.0)*i,-0.425,-0.240,RAYDIUM_ODE_JOINT_AXE_Y);
        raydium_ode_motor_attach_name("tank_moteurD",c,0);
        raydium_ode_element_slip_name (&c[1],TANK_TRACK_SLIP);
    }

    raydium_ode_object_box_add("tank_tour",a,1,RAYDIUM_ODE_AUTODETECT,0,0,RAYDIUM_ODE_STANDARD,0,"t80t.tri");
    raydium_ode_element_move_name_3f("tank_tour",0,0,0.4);

    raydium_ode_joint_attach_hinge_name("pivot","tank_corps","tank_tour",0,0,0.4,RAYDIUM_ODE_JOINT_AXE_Z);
    raydium_ode_joint_hinge_limits_name("pivot",-2.5,2.5);
    raydium_ode_motor_create("pivot_motor",a,RAYDIUM_ODE_MOTOR_ANGULAR);
    raydium_ode_motor_attach_name("pivot_motor","pivot",0);
    raydium_ode_motor_power_max_name("pivot_motor",10);

    raydium_ode_object_box_add("tank_canon",a,1,RAYDIUM_ODE_AUTODETECT,0,0,RAYDIUM_ODE_STANDARD,0,"t80c.tri");
    raydium_ode_element_move_name_3f("tank_canon",1,0,0.30);

    raydium_ode_joint_attach_hinge_name("trukatach","tank_tour","tank_canon",0.2,0,0.3,RAYDIUM_ODE_JOINT_AXE_Y);
    raydium_ode_joint_hinge_limits_name("trukatach",0,0.5);
    raydium_ode_motor_create("truk_motor",a,RAYDIUM_ODE_MOTOR_ANGULAR);
    raydium_ode_motor_attach_name("truk_motor","trukatach",0);
    raydium_ode_motor_power_max_name("truk_motor",1);

    raydium_ode_element_particle_offset_name_3f("tank_corps","smoke.prt",-1,0.5,0);

    raydium_ode_object_move_name_3f("TANK",1,0,0.2);
}




void display(void)
{
float speed;
static float speedD=0,speedG=0;
int delta_x, delta_y;

unsigned char dir;
static int inboard=1;

    raydium_joy_key_emul();

    speedD=speedG=0;
    dir=0;
    if(raydium_key[GLUT_KEY_UP]) dir|=1;
    if(raydium_key[GLUT_KEY_RIGHT]) dir|=2;
    if(raydium_key[GLUT_KEY_DOWN]) dir|=4;
    if(raydium_key[GLUT_KEY_LEFT]) dir|=8;

    switch (dir) {
        case 1: speedG=1;speedD=1;break;
        case 2: speedG=1;speedD=-1;break;
        case 3: speedG=1;speedD=0;break;
        case 4: speedG=-1;speedD=-1;break;
        case 6: speedG=-1;speedD=0;break;
        case 8: speedG=-1;speedD=1;break;
        case 9: speedG=0;speedD=1;break;
        case 12: speedG=0;speedD=-1;break;
        default: speedD=speedG=0;
    }
    speedD*=10;
    speedG*=10;
    raydium_ode_motor_speed_name("tank_moteurG",-speedG);
    raydium_ode_motor_speed_name("tank_moteurD",-speedD);


    speed=raydium_math_abs(speedD)+raydium_math_abs(speedG);
    speed*=0.025;
    speed+=0.5;
    speed+=(raydium_random_neg_pos_1()/15);
    raydium_ode_element_sound_update_name("tank_corps",sound_car);
    raydium_sound_SetSourcePitch(sound_car,raydium_math_abs(speed));

    if(raydium_key_last==1027)
        exit(0);

    raydium_light_position[0][0]=100;
    raydium_light_position[0][1]=100;
    raydium_light_position[0][2]=100;
    raydium_light_position[0][3]=1.0;
    raydium_light_update_position(0);

    raydium_background_color_change(light_color[0],light_color[1],light_color[2],light_color[3]);

    raydium_clear_frame();

    if(!raydium_key[GLUT_KEY_F2]){

        raydium_viewport_enable("camera2");

        raydium_ode_element_camera_inboard_name("tank_tour",0.2,0,0.1,2,0,0);

        raydium_ode_draw_all(RAYDIUM_ODE_DRAW_NORMAL);
        raydium_osd_printf(10,10,40,0.5,"font2.tga","standard viewport");
        raydium_osd_logo("logo.tga");
        raydium_viewport_save();
        raydium_clear_frame();
    }


    delta_x = raydium_mouse_x - (raydium_window_tx/2);
    delta_y = raydium_mouse_y - (raydium_window_ty/2);
    raydium_mouse_move(raydium_window_tx/2, raydium_window_ty/2);

    if (inboard){
        static GLfloat speed=0;

        if (raydium_mouse_button[2]) {
            if (speed==0)
                speed=0.1;
            speed+=0.01;
        }
            else
        speed=0;

        if (raydium_mouse_click==4)
            speed=0.05;
        if (raydium_mouse_click==5)
            speed=-0.05;

        cam_angle_x += (delta_x*3*0.1f);
        cam_angle_y += (delta_y*3*0.1f);

        cam_pos_z += (raydium_math_sin(cam_angle_x+90)*speed*raydium_math_sin(90-cam_angle_y));
        cam_pos_x += (raydium_math_cos(cam_angle_x+90)*speed*raydium_math_sin(90-cam_angle_y));
        cam_pos_y += (raydium_math_cos(90-cam_angle_y)*speed);
        raydium_camera_place(cam_pos_x,cam_pos_y,cam_pos_z,cam_angle_x,cam_angle_y,0);

    }
    else {
        raydium_ode_motor_angle_name("pivot_motor",raydium_ode_motor_angle_get_name("pivot_motor",0)+delta_x*10.0f);
        raydium_ode_motor_angle_name("truk_motor",raydium_ode_motor_angle_get_name("truk_motor",0)-delta_y*10.0f);

        raydium_ode_element_camera_inboard_name("tank_corps",0.7,0,0.2,2,0,0);
    }


    raydium_ode_draw_all(0);
    raydium_ode_draw_all(RAYDIUM_ODE_DRAW_RAY);

    if(raydium_key[GLUT_KEY_F1])
        raydium_ode_draw_all(1);

    if(!raydium_key[GLUT_KEY_F2])
        raydium_viewport_draw("camera2",20,70,60,30);

    raydium_osd_printf(2,98,16,0.5,"font2.tga","- %3i FPS - Viewport demo %s for Raydium %s, CQFD Corp.",raydium_render_fps,version,raydium_version());

    if (raydium_key_last==1000+'m') inboard^=1;

    if(!raydium_key[GLUT_KEY_F3]){
        raydium_viewport_direct_open_4f(20,40,60,30);

        raydium_ode_element_camera_inboard_name("tank_tour",0.2,0,0.1,2,0,0);

        raydium_ode_draw_all(RAYDIUM_ODE_DRAW_NORMAL);
        raydium_osd_printf(10,10,40,0.5,"font2.tga","direct viewport");
        raydium_osd_logo("logo.tga");
        raydium_viewport_direct_close();
    }

    raydium_osd_logo("logo.tga");

    raydium_rendering_finish();
}


int main(int argc, char **argv)
{
    raydium_init_args(argc,argv);

    raydium_window_create(640,480,RAYDIUM_RENDERING_WINDOW,"test Viewports");
    raydium_texture_filter_change(RAYDIUM_TEXTURE_FILTER_TRILINEAR);
    raydium_projection_near=0.01;
    raydium_projection_far=2500;
    raydium_projection_fov=60;
    raydium_fog_disable();
    raydium_window_view_update();
    raydium_shadow_enable();
//    raydium_shadow_disable();

    raydium_light_on(0);
    memcpy(raydium_light_color[0],light_color,raydium_internal_size_vector_float_4);
    raydium_light_intensity[0] = 1000000;
    raydium_light_update_all(0);


    raydium_ode_ground_set_name("cocorobix.tri");
    raydium_ode_element_slip_name("ground",GROUND_SLIP);

    raydium_sound_DefaultReferenceDistance=4.f;
    sound_car=raydium_sound_LoadWav("murcielago.wav");
    raydium_sound_SetSourcePitch(sound_car,0);
    raydium_sound_SetSourceGain(sound_car,0.1);  // Engine Gain

    raydium_viewport_create ("camera2",256,256);

    simul_create();

    raydium_callback(&display);
    return(0);
}
// EOF
