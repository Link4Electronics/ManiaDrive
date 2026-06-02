// This is the very first public demo of Raydium Augmented Reality !
// All this need a ****big*** cleanup (I'll do it very soon)

// /!\ Linux only, for now.

// To compile it, you need ARToolKit:
// http://sourceforge.net/projects/artoolkit
// then, export AR_PATH="path to compiled toolkit"
// you can now launch this demo using:
// ./odyncomp.sh ar_new.c (do not use ocomp.sh)

// How to make the demo working:
// Plug you webcam (check focus)
// Print patterns/pattHiro.pdf from the ARToolKit lib
// Launch this demo
// Show the sheet to your camera
// You're now experiencing augmented reality ! ;)

#include "raydium/index.c"
#include "AR/param.h"
#include "AR/ar.h"

GLfloat sun[]={1.0,0.9,0.5,1.0};

GLfloat camx=3.01;
GLfloat camy=3;
GLfloat camz=0;

#define PLAYER_MODEL    "sasc2.tri"

#ifndef WIN32
#define CAMERA_RES_X    320
#define CAMERA_RES_Y    240

#define camera "camera_para.dat"

#else
#define CAMERA_RES_X    640
#define CAMERA_RES_Y    480

#define camera "dvx.dat"
#endif

char            *vconf = "";
int             xsize, ysize;
int             thresh = 100;
int             count = 0;

char           *cparam_name    = camera;
ARParam         cparam;

char           *patt_name      = "patt.hiro";
int             patt_id;
double          patt_width     = 80.0;
double          patt_center[2] = {0.0, 0.0};
double          patt_trans[3][4];
int             rot_ok=0;

double          target_width = 80.0;
double          target_center[2] = {0.0, 0.0};
double          cam_trans[3][4];
double          gl_para[16];

double          gl_cpara[16];

void argConvGlpara( double para[3][4], double gl_para[16] )
{
    int     i, j;

    for( j = 0; j < 3; j++ ) {
        for( i = 0; i < 4; i++ ) {
            gl_para[i*4+j] = para[j][i];
        }
    }
    gl_para[0*4+3] = gl_para[1*4+3] = gl_para[2*4+3] = 0.0;
    gl_para[3*4+3] = 1.0;
}


void ar_camera_start(double patt_trans[3][4])
{
double gl_para[16];

argConvGlpara(patt_trans, gl_para);

glMatrixMode(GL_MODELVIEW);
glPushMatrix();
glLoadMatrixd( gl_para );

glMatrixMode(GL_PROJECTION);
glPushMatrix();
glLoadMatrixd( gl_cpara );

glMatrixMode(GL_MODELVIEW);
}


void ar_camera_stop(void)
{
glMatrixMode(GL_PROJECTION);
glPopMatrix();

glMatrixMode(GL_MODELVIEW);
glPopMatrix();
}


static void argConvGLcpara2( double cparam[3][4], int width, int height, double gnear, double gfar, double m[16] )
{
    double   icpara[3][4];
    double   trans[3][4];
    double   p[3][3], q[4][4];
    int      i, j;

    if( arParamDecompMat(cparam, icpara, trans) < 0 ) {
        printf("gConvGLcpara: Parameter error!!\n");
        exit(0);
    }

    for( i = 0; i < 3; i++ ) {
        for( j = 0; j < 3; j++ ) {
            p[i][j] = icpara[i][j] / icpara[2][2];
        }
    }
    q[0][0] = (2.0 * p[0][0] / width);
    q[0][1] = (2.0 * p[0][1] / width);
    q[0][2] = ((2.0 * p[0][2] / width)  - 1.0);
    q[0][3] = 0.0;

    q[1][0] = 0.0;
    q[1][1] = (2.0 * p[1][1] / height);
    q[1][2] = ((2.0 * p[1][2] / height) - 1.0);
    q[1][3] = 0.0;

    q[2][0] = 0.0;
    q[2][1] = 0.0;
    q[2][2] = (gfar + gnear)/(gfar - gnear);
    q[2][3] = -2.0 * gfar * gnear / (gfar - gnear);

    q[3][0] = 0.0;
    q[3][1] = 0.0;
    q[3][2] = 1.0;
    q[3][3] = 0.0;

    for( i = 0; i < 4; i++ ) {
        for( j = 0; j < 3; j++ ) {
            m[i+j*4] = q[i][0] * trans[0][j]
                     + q[i][1] * trans[1][j]
                     + q[i][2] * trans[2][j];
        }
        m[i+3*4] = q[i][0] * trans[0][3]
                 + q[i][1] * trans[1][3]
                 + q[i][2] * trans[2][3]
                 + q[i][3];
    }
}

void argConvGLcpara( ARParam *param, double gnear, double gfar, double m[16] )
{
    int i;

    for( i = 0; i < 4; i++ ) {
        param->mat[1][i] = (param->ysize-1)*(param->mat[2][i]) - param->mat[1][i];
    }

    argConvGLcpara2( param->mat, param->xsize, param->ysize, gnear, gfar, m );
}


int data_callback(unsigned char *data, int tx, int ty, int bpp)
{
    ARUint8         *dataPtr;
    ARMarkerInfo    *marker_info;
    int             marker_num;
    int             j, k;


    dataPtr=data;

    if( arDetectMarker(dataPtr, thresh, &marker_info, &marker_num) < 0 ) {
        //cleanup();
        raydium_log("arDetectMarker error");
        exit(0);
    }


    k = -1;
    for( j = 0; j < marker_num; j++ ) {
        if( patt_id == marker_info[j].id ) {
            if( k == -1 ) k = j;
            else if( marker_info[k].cf < marker_info[j].cf ) k = j;
        }
    }
    if( k == -1 ) {
        raydium_log("no marker available %i",raydium_random_i(0,10));
        return 1;
    }
    else
        raydium_log("Marker %f",marker_info[k].cf);

//    arGetTransMat(&marker_info[k], patt_center, patt_width, patt_trans);
    arGetTransMatCont(&marker_info[k], patt_trans,patt_center, patt_width, patt_trans);
    arUtilMatInv(patt_trans, cam_trans);
    argConvGlpara(patt_trans, gl_para);

    rot_ok=1;

return 1;
}


void create_perso(void)
{
int a;

    raydium_ode_object_delete_name("PLAYER");
    a=raydium_ode_object_create("PLAYER");
    raydium_ode_object_sphere_add("player",a,0.1,RAYDIUM_ODE_AUTODETECT*0.8,RAYDIUM_ODE_STANDARD,0,PLAYER_MODEL);
    raydium_ode_element_material_name("player",RAYDIUM_ODE_MATERIAL_SOFT2);
    raydium_ode_element_player_set_name("player",1);
    raydium_ode_motor_create("player_react",a,RAYDIUM_ODE_MOTOR_ROCKET);
    raydium_ode_motor_rocket_set_name("player_react","player",0,0,0);
    raydium_ode_motor_rocket_playermovement_name("player_react",1);
    raydium_ode_element_slip_name("ground",RAYDIUM_ODE_SLIP_ICE/2.f);

    raydium_ode_object_move_name_3f("PLAYER",0,0,1);
}


void create_car(void)
{
int a;
 
#define BREAK_FORCE     130
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
raydium_ode_motor_power_max_name("moteur",0.2);
 
raydium_ode_motor_create("direction",a,RAYDIUM_ODE_MOTOR_ANGULAR);
raydium_ode_motor_attach_name("direction","suspet_ag",0);
raydium_ode_motor_attach_name("direction","suspet_ad",0);
raydium_ode_motor_power_max_name("direction",0.2);

raydium_ode_object_move_name_3f("WATURE",0,0,1);
}


void display(void)
{
float speed,direct;
static float scale=50;
 
raydium_joy_key_emul();
 
direct=raydium_joy_x*0.3;
speed=raydium_joy_y*55;
 
raydium_ode_motor_speed_name("moteur",-speed);
raydium_ode_motor_angle_name("direction",direct);

if(raydium_key_last==1027) exit(0);

if(raydium_key[GLUT_KEY_F1]) scale++;
if(raydium_key[GLUT_KEY_F2]) scale--;

if(raydium_key_last==1000+'c') 
    {
    char dummy[128];
    raydium_ode_name_auto("caisse",dummy);
    raydium_ode_object_box_add(dummy,raydium_ode_object_find("GLOBAL"),1,RAYDIUM_ODE_AUTODETECT,RAYDIUM_ODE_AUTODETECT,RAYDIUM_ODE_AUTODETECT,RAYDIUM_ODE_STANDARD,0,"crate.tri");
    raydium_ode_element_move_name_3f(dummy,0,0,2);
    raydium_ode_element_ttl_set_name(dummy,raydium_ode_get_physics_freq()*60);
    }
                                
if(raydium_key_last==1032) 
    {
    raydium_ode_object_delete_name("WATURE");
    create_car();
    }


raydium_clear_frame();


// useless :)
raydium_camera_look_at(0,0.01,3,0,0,0); 

raydium_live_texture_mask_name("webcam.tga",1);
ar_camera_start(patt_trans);
raydium_light_update_position_all();
glScalef(scale,scale,scale);
glPushMatrix();

// because of glScale ...
glEnable(GL_NORMALIZE);
raydium_ode_draw_all(0);
//raydium_ode_draw_all(1);
glDisable(GL_NORMALIZE);

glPopMatrix();
ar_camera_stop();

raydium_osd_printf(2,4,20,0.5,"font2.tga","%f",scale);

raydium_ode_network_element_send_iterative(RAYDIUM_ODE_NETWORK_OPTIMAL);
raydium_rendering_finish();
}


int main(int argc, char **argv)
{
int device;
ARParam  wparam;
FILE *fp;
char server[128];

raydium_init_args(argc,argv);
raydium_window_create(800,600,RAYDIUM_RENDERING_WINDOW,"Augmented Reality - libAR test");
raydium_texture_filter_change(RAYDIUM_TEXTURE_FILTER_TRILINEAR);
raydium_projection_near=1;
raydium_projection_far=10000;
raydium_projection_fov=60; // useless
raydium_window_view_update();

raydium_light_on(0);
memcpy(raydium_light_color[0],sun,raydium_internal_size_vector_float_4);
raydium_light_intensity[0]=100000;

raydium_light_position[0][0]=50;
raydium_light_position[0][1]=150;
raydium_light_position[0][2]=200;

raydium_light_update_all(0);
raydium_background_color_change(sun[0],sun[1],sun[2],sun[3]);
raydium_fog_disable();

    fp=raydium_file_fopen(cparam_name,"rb");
    if(!fp)
        exit(1);
    fclose(fp);
    fp=raydium_file_fopen(patt_name,"rb");
    if(!fp)
        exit(1);
    fclose(fp);

    /* set the initial camera parameters */
    if( arParamLoad(cparam_name, 1, &wparam) < 0 ) {
        printf("Camera parameter load error !!\n");
        exit(0);
    }
    arParamChangeSize( &wparam, CAMERA_RES_X, CAMERA_RES_Y, &cparam );
    arInitCparam( &cparam );
    printf("*** Camera Parameter ***\n");
    arParamDisp( &cparam );

    if( (patt_id=arLoadPatt(patt_name)) < 0 ) {
        printf("pattern load error !!\n");
        exit(0);
    }


argConvGLcpara( &cparam, AR_GL_CLIP_NEAR, AR_GL_CLIP_FAR, gl_cpara );
arImageProcMode = AR_IMAGE_PROC_IN_FULL;

//device=raydium_live_video_open_auto();
device=raydium_live_video_open(RAYDIUM_LIVE_DEVICE_AUTO,CAMERA_RES_X,CAMERA_RES_Y);
raydium_live_texture_video(device,"webcam.tga");
//raydium_video_open("s80.jpgs","webcam.tga");
raydium_live_texture_refresh_callback_set_name("webcam.tga",data_callback);

raydium_shadow_enable();

if(raydium_init_cli_option("server",server))
     if(!raydium_network_client_connect_to(server)) 
        exit(1);

raydium_ode_ground_set_name("area.tri");
create_car();

//create_perso();
//raydium_object_anim_automatic_name(PLAYER_MODEL,"stand",10);

raydium_callback(&display);

return 0;
}


