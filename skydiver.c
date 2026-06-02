/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

// TODO:
// - Blood (red particules)
// - Update this game to use all last Raydium ODE integrations

/*
    Xfennec - CQFD Corp.
    NewSkyDiver (Atari "Sky Diver" clone)
    CQFD Corp.
*/

char *version="version 0.96";

//#define ODE_MANUAL_CALLBACK

#include "raydium/index.c"


#define DEFAULT_ZOOM            20
#define DEFAULT_CAMX            0
#define DEFAULT_CAMY            -100
#define DEFAULT_CAMZ            5
#define PLANE_PLACE             30
#define PARACHUTE_PORTANCE      0.1
#define MAX_TOUCH_TIME          5


GLfloat sun[]={1.0,0.9,0.5,1.0};
GLfloat amb[]={1.0,0.0,0.0,1.0};
GLfloat tmp2[]={1.0,1.0,1.0,1.0};
GLfloat zero[]={0.0,0.0,0.0,0.0};

GLfloat camx=DEFAULT_CAMX;
GLfloat camy=DEFAULT_CAMY;
GLfloat camz=DEFAULT_CAMZ;
GLfloat camzoom=DEFAULT_ZOOM;
dReal wind;
dReal move=0;
char para_open;
char touching;
int touching_steps;
float fly_time;
float dist_from_center;
int breaked_bones;
int best_score=0;
int plane_touched;
char cam[RAYDIUM_MAX_NAME_LEN];
GLfloat last_frame_step;
char tips[6][128]={"^dPress ^fSPACE^d to jump, ^fBACKSPACE^d to restart a game, ^fF1^d for dynamic view.","^cPress ^fSPACE^c to open parachute !","^aUse ^fLEFT^a and ^fRIGHT^a arrows (or joystick/joypad) to move ...","^eUse ^fBACKSPACE^e to try again !","^dBye ! ...",""};
char player_name[RAYDIUM_MAX_NAME_LEN];
int tip=0;
dReal score_timer;
char fp_view=-1;

#define GAME_PART_PLANE    0
#define GAME_PART_JUMP     1
#define GAME_PART_PARA     2
#define GAME_PART_TOUCHED  3
#define GAME_PART_SCORE    4
#define GAME_PART_QUIT     5
int game_part=-1;

int S_plane;
int S_hurt[4];
int S_plane_fall;

// type_* are used as address tag
char type_lander;
char type_para;
char type_plane;

int music_change(char *newfile)
{
raydium_register_variable(newfile,RAYDIUM_REGISTER_STR,"raydium_ogg_file");
raydium_php_exec("playlist.php");
raydium_register_variable_unregister_last();
return strlen(newfile)!=0;
}


int get_score(char growing_fx)
{
// - Scoring (breaked bones, flying time, lander-touching time, wind)
GLfloat score,tmp,w;

w=wind;

if(breaked_bones) return 0;
if(!touching_steps) return 0;
if(w>0) w*=2; // positive wind is harder
score=touching_steps*raydium_math_abs(w); // 2000 max

tmp=3.f-fly_time; // fly time
if(tmp<0) tmp=0;
score+=400*tmp;

tmp=0.5f-dist_from_center; // fly time
if(tmp<0) tmp=0;
score+=4000*tmp;

tmp=score_timer/2;
if(tmp>1 || !growing_fx) tmp=1;
if(score>best_score) best_score=score;
return score*tmp;
}


void post_score(void)
{
int score=get_score(0);
char lversion[RAYDIUM_MAX_NAME_LEN];

if(!score) return;

strcpy(lversion,version); // since PHP will write to variables ..

raydium_register_variable(&wind,RAYDIUM_REGISTER_FLOAT,"wind");
raydium_register_variable(&breaked_bones,RAYDIUM_REGISTER_INT,"breaked_bones");
raydium_register_variable(&fly_time,RAYDIUM_REGISTER_FLOAT,"fly_time");
raydium_register_variable(&touching_steps,RAYDIUM_REGISTER_INT,"touching_steps");
raydium_register_variable(&score,RAYDIUM_REGISTER_INT,"score");
raydium_register_variable(player_name,RAYDIUM_REGISTER_STR,"name");
raydium_register_variable(lversion,RAYDIUM_REGISTER_STR,"version");

raydium_php_exec("score.php");

raydium_register_variable_unregister_last();
raydium_register_variable_unregister_last();
raydium_register_variable_unregister_last();
raydium_register_variable_unregister_last();
raydium_register_variable_unregister_last();
raydium_register_variable_unregister_last();
raydium_register_variable_unregister_last();
}


void play_hurt_sound(void)
{
raydium_sound_SourcePlay(S_hurt[rand()%4]);
}

void frame_step(GLfloat step)
{
last_frame_step=step;
if(game_part==GAME_PART_SCORE)
    score_timer+=(step/60);
}

void bone_break(int j)
{
play_hurt_sound();
breaked_bones++;
}

void camera(GLfloat px, GLfloat py, GLfloat pz, GLfloat lx, GLfloat ly, GLfloat lz, GLfloat zoom)
{
#define SLOW_FACT 30
static GLfloat opx,opy,opz;
static GLfloat olx,oly,olz;
static GLfloat ozoom=DEFAULT_ZOOM;
ALfloat pos[3];
ALfloat or[6];

//raydium_log("<%f (asked=%f real=%f step=%f)",ozoom,zoom,raydium_projection_fov,last_frame_step);

opx+=(px-opx)/SLOW_FACT*last_frame_step;
opy+=(py-opy)/SLOW_FACT*last_frame_step;
opz+=(pz-opz)/SLOW_FACT*last_frame_step;
olx+=(lx-olx)/SLOW_FACT*last_frame_step;
oly+=(ly-oly)/SLOW_FACT*last_frame_step;
olz+=(lz-olz)/SLOW_FACT*last_frame_step;
ozoom+=(zoom-ozoom)/SLOW_FACT*last_frame_step;

//raydium_log(">%f",ozoom);

if(ozoom!=raydium_projection_fov)
    {
    raydium_projection_fov=ozoom;
    raydium_window_view_update();
    }

raydium_camera_look_at(opx,opy,opz,olx,oly,olz);
pos[0]=opx;
pos[1]=opy;
pos[2]=opz;
raydium_sound_SetListenerPos(pos);
or[0]=olx-opx;
or[1]=oly-opy;
or[2]=olz-opz;
or[3]=or[4]=0;
or[5]=1;
raydium_sound_SetListenerOr(or);
}


char collide(int e1, int e2, dContact *n)
{
char *c1,*c2;

c1=raydium_ode_element_data_get(e1);
c2=raydium_ode_element_data_get(e2);

    if(((c1==&type_para && c2==&type_lander) ||
        (c2==&type_para && c1==&type_lander)  ) && game_part<GAME_PART_SCORE)
        {
        touching=1;
        }

if((c1==&type_para || c2==&type_para) && (game_part==GAME_PART_PARA || game_part==GAME_PART_JUMP))
{
dReal *pos;
para_open=0;
game_part=GAME_PART_TOUCHED;
pos=raydium_ode_element_pos_get_name("body");
dist_from_center=raydium_math_abs(pos[0]);
}

if(c1==&type_plane || c2==&type_plane)
    {
     if(!plane_touched)
        {
        raydium_sound_SourceStop(S_plane);
        play_hurt_sound();
        plane_touched++;
        }
    }

return 1;
}

void step(void)
{

if(touching) touching_steps++;
touching=0;
// if parachute is open... :
if(para_open)
 raydium_ode_object_addforce_name_3f("RAGDOLL",move+(wind*0.1),0,PARACHUTE_PORTANCE);

if(game_part>=GAME_PART_JUMP && game_part<GAME_PART_TOUCHED)
    fly_time+=(1/400.f);

{
int i;
dJointID joint;

i=raydium_ode_joint_find("shoulder");
if(i<0)
    {
    //raydium_log("not found");
    return;
    }
joint=raydium_ode_joint[i].joint;
if(raydium_key[GLUT_KEY_F2])
    {
    dJointSetHingeParam(joint,dParamVel,10);
    dJointSetHingeParam(joint,dParamFMax,0.2);
    }
else
    {
    dJointSetHingeParam(joint,dParamVel, 0);
    dJointSetHingeParam(joint,dParamFMax,0);
    }
}

}

void create_landing(dReal px, dReal py, dReal pz)
{
int a;
a=raydium_ode_object_create("LANDER");
  raydium_ode_object_box_add("lander",a,1,RAYDIUM_ODE_AUTODETECT,0,0,RAYDIUM_ODE_STATIC,0,"landing.tri");
  raydium_ode_element_move_name_3f("lander",pz,py,pz);
  raydium_ode_element_data_set_name("lander",&type_lander);
}


void create_plane(int num)
{
int a;
char obj[128];
char ele[128];

sprintf(obj,"PLANE %i",num);
sprintf(ele,"plane %i",num);
a=raydium_ode_object_create(obj);
  raydium_ode_object_box_add(ele,a,1,RAYDIUM_ODE_AUTODETECT,0,0,RAYDIUM_ODE_STANDARD,0,"plane.tri");
  raydium_ode_element_move_name_3f(ele,-PLANE_PLACE,0,10);
  raydium_ode_element_data_set_name(ele,&type_plane);
  // hey ... we shouldn't call ODE directly !
  raydium_ode_element_addforce_name_3f(ele,500,0,0);
  //dBodyAddForce(raydium_ode_element[raydium_ode_element_find(ele)].body,500,0,0);
  dBodySetGravityMode(raydium_ode_element[raydium_ode_element_find(ele)].body,0);
  raydium_sound_SourcePlay(S_plane);
}


void create_para(char *plane)
{
#define BONE_BREAK 80
dReal *pos;
dReal mypos[3];
int a;

    a=raydium_ode_object_create("RAGDOLL");
    raydium_ode_object_sphere_add("head",a,0.1,RAYDIUM_ODE_AUTODETECT,RAYDIUM_ODE_STANDARD,0,"p_head.tri");
    raydium_ode_element_data_set_name("head",&type_para);
    raydium_ode_element_move_name_3f("head",5,0,1);

    raydium_ode_object_box_add("body",a,0.1,RAYDIUM_ODE_AUTODETECT,0,0,RAYDIUM_ODE_STANDARD,0,"p_body.tri");
    raydium_ode_element_data_set_name("body",&type_para);
    raydium_ode_element_move_name_3f("body",5,0,0.66);
    raydium_ode_joint_attach_hinge_name("neck","body","head",5,0,0.95,RAYDIUM_ODE_JOINT_AXE_X);
    raydium_ode_joint_hinge_limits_name("neck",-1,1);
    raydium_ode_joint_break_force_name("neck",BONE_BREAK);
    raydium_ode_joint_delete_callback_name("neck",bone_break);

    raydium_ode_object_box_add("u_leg",a,0.1,RAYDIUM_ODE_AUTODETECT,0,0,RAYDIUM_ODE_STANDARD,0,"p_u_leg.tri");
    raydium_ode_element_data_set_name("u_leg",&type_para);
    raydium_ode_element_move_name_3f("u_leg",4.9,0,0.26);
    raydium_ode_element_rotate_name_3f("u_leg",0,-0.2,0);
    raydium_ode_joint_attach_hinge_name("femur","body","u_leg",4.9,0,0.42,RAYDIUM_ODE_JOINT_AXE_X);
    raydium_ode_joint_hinge_limits_name("femur",-1,1);
    raydium_ode_joint_break_force_name("femur",BONE_BREAK);
    raydium_ode_joint_delete_callback_name("femur",bone_break);

    raydium_ode_object_box_add("u_leg2",a,0.1,RAYDIUM_ODE_AUTODETECT,0,0,RAYDIUM_ODE_STANDARD,0,"p_u_leg2.tri");
    raydium_ode_element_data_set_name("u_leg2",&type_para);
    raydium_ode_element_move_name_3f("u_leg2",5.1,0,0.26);
    raydium_ode_element_rotate_name_3f("u_leg2",0,0.2,0);
    raydium_ode_joint_attach_hinge_name("femur2","body","u_leg2",5.1,0,0.42,RAYDIUM_ODE_JOINT_AXE_X);
    raydium_ode_joint_hinge_limits_name("femur2",-1,1);
    raydium_ode_joint_break_force_name("femur2",BONE_BREAK);
    raydium_ode_joint_delete_callback_name("femur2",bone_break);


    raydium_ode_object_box_add("l_leg",a,0.1,RAYDIUM_ODE_AUTODETECT,0,0,RAYDIUM_ODE_STANDARD,0,"p_l_leg.tri");
    raydium_ode_element_data_set_name("l_leg",&type_para);
    raydium_ode_element_move_name_3f("l_leg",4.82,-0.02,-0.14);
    raydium_ode_element_rotate_name_3f("l_leg",0,-0.1,0);
    raydium_ode_joint_attach_hinge_name("knee","u_leg","l_leg",4.85,0,0.05,RAYDIUM_ODE_JOINT_AXE_X);
    raydium_ode_joint_hinge_limits_name("knee",-2,0);
    raydium_ode_joint_break_force_name("knee",BONE_BREAK);
    raydium_ode_joint_delete_callback_name("knee",bone_break);

    raydium_ode_object_box_add("l_leg2",a,0.1,RAYDIUM_ODE_AUTODETECT,0,0,RAYDIUM_ODE_STANDARD,0,"p_l_leg2.tri");
    raydium_ode_element_data_set_name("l_leg2",&type_para);
    raydium_ode_element_move_name_3f("l_leg2",5.18,-0.02,-0.14);
    raydium_ode_element_rotate_name_3f("l_leg2",0,0.1,0);
    raydium_ode_joint_attach_hinge_name("knee2","u_leg2","l_leg2",5.15,0,0.05,RAYDIUM_ODE_JOINT_AXE_X);
    raydium_ode_joint_hinge_limits_name("knee2",-2,0);
    raydium_ode_joint_break_force_name("knee2",BONE_BREAK);
    raydium_ode_joint_delete_callback_name("knee2",bone_break);

    raydium_ode_object_box_add("u_arm",a,0.1,RAYDIUM_ODE_AUTODETECT,0,0,RAYDIUM_ODE_STANDARD,0,"p_u_arm.tri");
    raydium_ode_element_data_set_name("u_arm",&type_para);
    raydium_ode_element_move_name_3f("u_arm",4.83,0,0.73);
    raydium_ode_element_rotate_name_3f("u_arm",-0.1,-0.1,0);
    raydium_ode_joint_attach_hinge_name("shoulder","body","u_arm",4.85,0,0.84,RAYDIUM_ODE_JOINT_AXE_X);
    raydium_ode_joint_break_force_name("shoulder",BONE_BREAK);
    raydium_ode_joint_delete_callback_name("shoulder",bone_break);
//    raydium_ode_joint_hinge_limits_name("shoulder",-2,2);

    raydium_ode_object_box_add("u_arm2",a,0.1,RAYDIUM_ODE_AUTODETECT,0,0,RAYDIUM_ODE_STANDARD,0,"p_u_arm2.tri");
    raydium_ode_element_data_set_name("u_arm2",&type_para);
    raydium_ode_element_move_name_3f("u_arm2",5.17,0,0.73);
    raydium_ode_element_rotate_name_3f("u_arm2",-0.1,0.1,0);
    raydium_ode_joint_attach_hinge_name("shoulder2","body","u_arm2",5.15,0,0.84,RAYDIUM_ODE_JOINT_AXE_X);
    raydium_ode_joint_break_force_name("shoulder2",BONE_BREAK);
    raydium_ode_joint_delete_callback_name("shoulder2",bone_break);
//    raydium_ode_joint_hinge_limits_name("shoulder2",-2,2);

    raydium_ode_object_box_add("l_arm",a,0.1,RAYDIUM_ODE_AUTODETECT,0,0,RAYDIUM_ODE_STANDARD,0,"p_l_arm.tri");
    raydium_ode_element_data_set_name("l_arm",&type_para);
    raydium_ode_element_move_name_3f("l_arm",4.80,-0.05,0.45);
    raydium_ode_element_rotate_name_3f("l_arm",0.3,0,0);
    raydium_ode_joint_attach_hinge_name("elbow","u_arm","l_arm",4.80,0,0.60,RAYDIUM_ODE_JOINT_AXE_X);
    raydium_ode_joint_hinge_limits_name("elbow",0,2);
    raydium_ode_joint_break_force_name("elbow",BONE_BREAK);
    raydium_ode_joint_delete_callback_name("elbow",bone_break);

    raydium_ode_object_box_add("l_arm2",a,0.1,RAYDIUM_ODE_AUTODETECT,0,0,RAYDIUM_ODE_STANDARD,0,"p_l_arm2.tri");
    raydium_ode_element_data_set_name("l_arm2",&type_para);
    raydium_ode_element_move_name_3f("l_arm2",5.20,-0.05,0.45);
    raydium_ode_element_rotate_name_3f("l_arm2",0.3,0,0);
    raydium_ode_joint_attach_hinge_name("elbow2","u_arm2","l_arm2",5.20,0,0.60,RAYDIUM_ODE_JOINT_AXE_X);
    raydium_ode_joint_hinge_limits_name("elbow2",0,2);
    raydium_ode_joint_break_force_name("elbow2",BONE_BREAK);
    raydium_ode_joint_delete_callback_name("elbow2",bone_break);

    pos=raydium_ode_element_pos_get_name(plane);
    memcpy(mypos,pos,sizeof(dReal)*3);
    mypos[2]-=2;
    raydium_ode_object_move_name("RAGDOLL",mypos);
    pos = (dReal *) raydium_ode_element_linearvelocity_get_name(plane);
    //pos=(dReal *)dBodyGetLinearVel(raydium_ode_element[raydium_ode_element_find(plane)].body);
    raydium_ode_object_linearvelocity_set_name("RAGDOLL",pos);
}


void create_game(void)
{
if(game_part==GAME_PART_SCORE) post_score();
raydium_ode_object_delete_name("RAGDOLL");
raydium_ode_object_delete_name("PLANE 1");
raydium_ode_object_delete_name("LANDER");
para_open=0;
touching=0;
touching_steps=0;
if(game_part!=GAME_PART_PLANE)
    wind=raydium_random_neg_pos_1();
game_part=GAME_PART_PLANE;
fly_time=0;
dist_from_center=0;
breaked_bones=0;
plane_touched=0;
create_plane(1);
create_landing(0,0,0);
}

void name_change(char *name)
{
name[strlen(name)-1]=0;
strcpy(player_name,name);
raydium_log("Name changed to \"%s\"",player_name);
raydium_console_event();
}


void display(void)
{
static GLfloat angle;
dReal *tmp;

angle+=last_frame_step/5;

raydium_joy_key_emul();

if(raydium_key_last==1027)
    game_part=GAME_PART_QUIT;

if(raydium_key_last==1)
    fp_view*=-1;

//if(raydium_key[GLUT_KEY_F1]) camzoom/=(1.04);
//if(raydium_key[GLUT_KEY_F2]) camzoom*=(1.04);

move=raydium_joy_x*0.1;

if(raydium_key_last==1008 && game_part!=GAME_PART_SCORE)
    {
    game_part=GAME_PART_SCORE;
    score_timer=0;
    raydium_key_last=0; // disable the following test
    raydium_sound_SourcePlay(S_plane_fall);
    }

if((score_timer>30 || raydium_key_last==1008) && game_part==GAME_PART_SCORE)
    create_game();

if(raydium_key_last==1032)
    {
    if(game_part==GAME_PART_PLANE)
        {
        create_para("plane 1");
        game_part=GAME_PART_JUMP;
        }
    else
    if(game_part==GAME_PART_JUMP)
        {
        para_open=1;
        game_part=GAME_PART_PARA;
        }
    }

if(touching_steps>=400*MAX_TOUCH_TIME && game_part==GAME_PART_TOUCHED)
    {
    touching_steps=400*MAX_TOUCH_TIME;
    game_part=GAME_PART_SCORE;
    raydium_sound_SourcePlay(S_plane_fall);
    score_timer=0;
    }


raydium_background_color_change(sun[0],sun[1],sun[2],sun[3]);

raydium_light_position[0][0]=50;
raydium_light_position[0][1]=-150;
raydium_light_position[0][2]=200;
raydium_light_position[0][3]=1.0;

raydium_clear_frame();

switch(game_part)
    {
    case GAME_PART_PLANE:
        camx=DEFAULT_CAMX;
        camy=DEFAULT_CAMY;
        camz=DEFAULT_CAMZ;
        camzoom=20;
        strcpy(cam,"ground");
        tip=0;
        break;
    case GAME_PART_JUMP:
        camzoom=5;
        strcpy(cam,"body");
        tip=1;
        break;
    case GAME_PART_PARA:
        camzoom=15;
        strcpy(cam,"body");
        tip=2;
        break;
    case GAME_PART_TOUCHED:
        camzoom=6;
        camz=50;
        camx=raydium_math_cos(angle)*100;
        camy=raydium_math_sin(angle)*100;
        strcpy(cam,"head");
        tip=3;
        break;
    case GAME_PART_QUIT:
        camx=camy=1;
        camz+=(camz*last_frame_step)/60;
        camzoom=70;
        strcpy(cam,"ground");
        tip=4;
        if(camz>500) exit(0);
        break;
    case GAME_PART_SCORE:
        if(raydium_ode_object_find("PLANE 1")<0)
            create_plane(1);
        dBodySetGravityMode(raydium_ode_element[raydium_ode_element_find("plane 1")].body,1);
        camzoom=4;
        camz=30;
        camx=raydium_math_cos(-angle)*200;
        camy=raydium_math_sin(-angle)*200;
        strcpy(cam,"plane 1");
        tip=3;
        break;
    }


// test if plane is not too far ... : (and adjust his sound, too)
if(raydium_ode_object_find("PLANE 1")>=0 && game_part<GAME_PART_SCORE)
    {
    ALfloat pos[3];
    tmp=raydium_ode_element_pos_get_name("plane 1");
    pos[0]=tmp[0];
    pos[1]=tmp[1];
    pos[2]=tmp[2];
    raydium_sound_SetSourcePos(S_plane,pos);
    raydium_sound_SetSourcePos(S_plane_fall,pos);
    if(tmp[0]>250)
        {
        score_timer=0;
        game_part=GAME_PART_SCORE;
        return;
        }
    }


if(raydium_ode_element_find(cam)<0)
    {
    //raydium_log("Camera cannot find \"%s\", now looking ground...",cam);
    strcpy(cam,"ground");
    }

// Test is "First Person" view is used
if(fp_view>0 && game_part<GAME_PART_SCORE)
{
    int from,to;
    dReal *from_p,*to_p;

    raydium_projection_near=0.1;
    raydium_projection_far=1000;

    from=raydium_ode_element_find("body");
    to=raydium_ode_element_find("ground");
    if(from<0) from=raydium_ode_element_find("plane 1");

    from_p=raydium_ode_element_pos_get(from);
    to_p=raydium_ode_element_pos_get(to);

    if(game_part>=GAME_PART_TOUCHED)
        camera(to_p[0]-0.3,to_p[1]-0.3,to_p[2]+0.5,from_p[1],-from_p[2],from_p[0],80);
    else
        camera(from_p[0],from_p[1]-2,from_p[2]+1,to_p[0],to_p[1],to_p[2],80);
//    raydium_projection_fov=90;
//    raydium_window_view_update();
//    raydium_ode_element_camera_inboard(from,0,-1,0,0,1,0);
}
else
{
    raydium_projection_near=5;
    raydium_projection_far=3000;
    tmp=raydium_ode_element_pos_get_name(cam);
    camera(camx,camy,camz,tmp[1],-tmp[2],tmp[0],camzoom);
}

raydium_ode_draw_all(0);
//raydium_ode_draw_all(1);

if(para_open)
    {
    // All this to draw a simple static parachute...
    dMatrix3 R;
    dReal *pos;
    dReal my_pos[3];
    dRSetIdentity(R);
    pos=raydium_ode_element_pos_get_name("head");
    memcpy(my_pos,pos,sizeof(dReal)*3);
    my_pos[2]+=1;
    raydium_camera_replace_go(my_pos,R);
    raydium_object_draw_name("parachute.tri");
    }

if(game_part==GAME_PART_SCORE)
    {
    raydium_osd_printf(20,45,40,0.8,"font2.tga","Score: %i",get_score(1));
    }
//else
    {
    raydium_osd_printf(5,13,16,0.5,"font2.tga","^cWind: %s %02.1f %% | Fly-Time: %.2f | Touch Time: %.2f",(wind<0?"<-":"->"),raydium_math_abs(wind*100),fly_time,touching_steps/400.f);
    raydium_osd_printf(5,10,16,0.5,"font2.tga","^cCenter Distance: %.2f | Breaked Bones: %i",dist_from_center,breaked_bones);
    }

raydium_osd_printf(75,95,16,0.5,"font2.tga","^fBest score: ^9% 5i",best_score);
raydium_osd_printf(2,2,16,0.5,"font2.tga","%s",tips[tip]);
raydium_osd_printf(2,98,16,0.5,"font2.tga","^f- %3i FPS - NewSkyDiver %s (Raydium %s) - CQFD Corp.",raydium_render_fps,version,raydium_version());
raydium_osd_logo("logoc.tga");
raydium_rendering_finish();

raydium_ode_network_element_send_iterative(RAYDIUM_ODE_NETWORK_OPTIMAL);
}

int main(int argc, char **argv)
{
struct passwd *pn;
char server[128];

#ifndef WIN32
pn=getpwuid(geteuid());
strcpy(player_name,pn->pw_name);
#else
{
DWORD s;
s=RAYDIUM_MAX_NAME_LEN-1;
GetUserName(player_name,&s);
if(!strlen(player_name))
    gethostname(player_name,RAYDIUM_MAX_NAME_LEN-1);
}
#endif

raydium_init_args(argc,argv);
raydium_window_create(640,480,RAYDIUM_RENDERING_WINDOW,version);
raydium_texture_filter_change(RAYDIUM_TEXTURE_FILTER_TRILINEAR);
raydium_projection_near=5;
raydium_projection_far=3000;
raydium_projection_fov=DEFAULT_ZOOM;
//raydium_light_disable();
raydium_fog_disable();
raydium_light_on(0);
//raydium_light_on(1);
memcpy(raydium_light_color[0],sun,raydium_internal_size_vector_float_4);
//raydium_light_intensity[0]=400;
raydium_light_intensity[0]=1000000;
raydium_light_update_all(0);
memcpy(raydium_light_color[1],amb,raydium_internal_size_vector_float_4);
raydium_light_intensity[1]=10000;
raydium_light_update_all(1);
raydium_window_view_update();


glMaterialfv( GL_FRONT, GL_DIFFUSE, tmp2);
glMaterialfv( GL_FRONT, GL_AMBIENT, zero);
glMaterialfv( GL_FRONT, GL_EMISSION, zero);
glMaterialf( GL_FRONT, GL_SHININESS, 128.);


// sounds loading :
raydium_sound_DefaultReferenceDistance=100.f;

S_plane=raydium_sound_LoadWav("plane.wav");
raydium_sound_SetSourcePitch(S_plane,2);
raydium_sound_SetSourceGain(S_plane,0.5);

S_plane_fall=raydium_sound_LoadWav("planefall.wav");
raydium_sound_SetSourceLoop(S_plane_fall,0);

S_hurt[0]=raydium_sound_LoadWav("hurt4.wav");
raydium_sound_SetSourceLoop(S_hurt[0],0);

S_hurt[1]=raydium_sound_LoadWav("hurt2.wav");
raydium_sound_SetSourceLoop(S_hurt[1],0);

S_hurt[2]=raydium_sound_LoadWav("hurt3.wav");
raydium_sound_SetSourceLoop(S_hurt[2],0);

S_hurt[3]=raydium_sound_LoadWav("hurt1.wav");
raydium_sound_SetSourceLoop(S_hurt[3],0);

raydium_osd_cursor_set("BOXcursor.tga",4,4);
strcpy(raydium_console_config_texture,"logo_console.tga");
raydium_sky_box_cache();

raydium_register_variable(&camx,RAYDIUM_REGISTER_FLOAT,"camx");
raydium_register_variable(&camy,RAYDIUM_REGISTER_FLOAT,"camy");
raydium_register_variable(&camz,RAYDIUM_REGISTER_FLOAT,"camz");
raydium_register_variable(&camzoom,RAYDIUM_REGISTER_FLOAT,"camzoom");
raydium_register_variable(&cam,RAYDIUM_REGISTER_STR,"cam");
raydium_timecall_add(frame_step,-60);

raydium_ode_ground_set_name("hills.tri");
raydium_ode_CollideCallback=collide;
raydium_ode_StepCallback=step;
raydium_console_gets_callback=name_change;
raydium_sound_music_eof_callback=music_change;

{
char file[RAYDIUM_MAX_NAME_LEN];
music_change(file);
raydium_sound_load_music(file);
}

raydium_log("Found player name: %s",player_name);
raydium_log("use /xxx to change your player name");
// cache data ... :
create_plane(1);
create_landing(0,0,0);
create_para("plane 1");
raydium_object_load("parachute.tri");
// ... and start a new game :

if(raydium_init_cli_option("server",server))
     if(!raydium_network_client_connect_to(server))
        exit(1);

create_game();
raydium_callback(&display);
return 0;
}
