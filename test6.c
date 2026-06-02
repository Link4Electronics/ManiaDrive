/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.

    Xfennec - CQFD Corp.
    test & integration program for Raydium - v6
    CQFD Corp.
*/

char *version="version 0.6.17";

#include "raydium/index.c"

GLfloat sun[]={1.0,0.9,0.8,1.0};
GLfloat blood[]={1.0,0.0,0.0,0.2};
GLfloat amb[]={1.0,0.0,0.0,1.0};


#define ROCKET_FIABILITY  5
#define ROCKET_SPEED     70
#define RELOAD_TIME     2.5
#define PLAYER_SPEED    0.2
#define PLAYER_JUMP      20
#define BULLET_SPEED     50
#define BULLET_RATE      50

#define SOUND_MAX_EXPLOS 4
char wav_boom[SOUND_MAX_EXPLOS][128]={"xf_boom1.wav","xf_boom2.wav","xf_boom3.wav","xf_boom4.wav"};
int son_boom[SOUND_MAX_EXPLOS];

#define SOUND_MAX_BONGS 5
char wav_bong[SOUND_MAX_BONGS][128]={"xf_bong1.wav","xf_bong2.wav","xf_bong3.wav","xf_bong4.wav","xf_bong5.wav"};
int son_bong[SOUND_MAX_BONGS];

int son;
int son_rl;
int son_reload;
int son_paf;
int son_gun;
char vue=7;
GLfloat camx=-20;
GLfloat camy=50;
GLfloat camz=40;
char cam[255];
char record=-1;
char draw_debug=-1;
char mouse_grab=1;
char is_explosive_tag=29;
char is_bullet=99;
dReal cam_angle_h=0;
dReal cam_angle_v=90;
int movie_spf=0;
float life;
char already_dead;
float reload_time;
float paf_time=0;
float sensivity=1;

dReal ptmp=0;

dReal gears[]={-0.4,0.4,0.6,0.8,0.9,1.0};
int gear=1;

void create_game(char);


//////////////////////// RayODE Callbacks

void boom(char type, dReal radius, dReal force_or_propag, dReal *pos)
{
int i;
int g1,g2;
char name1[RAYDIUM_MAX_NAME_LEN];
char name2[RAYDIUM_MAX_NAME_LEN];

i=raydium_random_i(0,SOUND_MAX_EXPLOS-1);
//raydium_explosion_add(pos[0],pos[1],pos[2],0,0,0,0.6,3,6,0.1);
raydium_particle_name_auto("explo",name1);
raydium_particle_name_auto("eplo_smoke",name2);
g1=raydium_particle_generator_load("explosion.prt",name1);
g2=raydium_particle_generator_load("explosmoke.prt",name2);

raydium_particle_generator_move(g1,pos);
raydium_particle_generator_move(g2,pos);

raydium_sound_SourcePlay(son_boom[i]);
raydium_sound_SetSourcePos(son_boom[i],pos);

}

void blow_touched(int elem, dReal force, dReal max_force)
{
if(raydium_ode_element_player_get(elem))
    {
    //raydium_log("touched: %.2f perc. of maximum explosion force (%.2f/%.2f)",force/max_force*100,force,max_force);
    life-=(force/2);
    }
else
    {
    // is a rocket !
    raydium_ode_element_OnBlow(elem,NULL);
    raydium_ode_element_delete(elem,1);
    }
}

int rocket_delete(int explo)
{
dReal *pos;
pos=raydium_ode_element_pos_get(explo);
raydium_ode_explosion_blow(4,100,pos);
return 1; // confirm delete
}

char collide(int e1, int e2, dContact *n)
{
char *c1,*c2;
int explo=-1;
int player=-1;
int bullet=-1;

c1=raydium_ode_element_data_get(e1);
c2=raydium_ode_element_data_get(e2);


//if( !raydium_ode_element_isvalid(e1) ||
//    !raydium_ode_element_isvalid(e2) )
//    printf("(%i|%i) (%s|%s)\n",e1,e2,raydium_ode_element[e1].name,raydium_ode_element[e2].name);
//    return 0;

if((c1 && *c1==29))
    explo=e1;
if((c2 && *c2==29))
    explo=e2;

if((c1 && *c1==99))
    bullet=e1;
if((c2 && *c2==99))
    bullet=e2;

if(raydium_ode_element_player_get(e1))
    player=e1;
if(raydium_ode_element_player_get(e2))
    player=e2;

if(player>=0 && n->geom.depth>0.04)
    {
    // Based on small tests:
    life-=(n->geom.depth*100/5);
    }

if(explo>=0)
    {
    raydium_sound_SourceStop(son_rl);
    if(raydium_random_i(0,ROCKET_FIABILITY)==0)
     {
     int i;
     // no luck, this rocket will never explode ;)
     raydium_ode_element_data_set(explo,NULL);
     raydium_ode_element_gravity(explo,1);
     raydium_ode_element_OnBlow(explo,blow_touched);
     // play a random "bong"
     i=raydium_random_i(0,SOUND_MAX_BONGS-1);
     raydium_sound_SourcePlay(son_bong[i]);
     raydium_ode_element_sound_update(explo,son_bong[i]);
     raydium_particle_generator_enable(raydium_ode_element[explo].particle,0);
     }
    else
     {
     raydium_ode_element_delete(explo,1);
     return 1;
     }
    }

if(bullet>=0)
    {
    // play a sound ?
    raydium_ode_element_delete(bullet,1);
    n->surface.soft_erp=0.5;
    n->surface.soft_cfm=1;
    return 1; // generate a contact point anyway
    }
return 1;
}

void frame_step(GLfloat step)
{
reload_time-=step;
paf_time+=step;
}


void step(void)
{
static int cpt=0;
if(raydium_mouse_button[1] && cpt>BULLET_RATE)
    {
    // fire !
    int a;
    char name[RAYDIUM_MAX_NAME_LEN];
    cpt=0;
    a=raydium_ode_object_find("PLAYER");
    raydium_ode_name_auto("bullet",name);
    strcpy(cam,name);
    raydium_ode_object_box_add(name,a,0.01,RAYDIUM_ODE_AUTODETECT,0,0,RAYDIUM_ODE_STANDARD,0,"bullet.tri");
    raydium_ode_launcher_simple_name_3f(name,"player",0,cam_angle_v+5,0,BULLET_SPEED);
    raydium_ode_element_rotate_direction_name(name,0);
    raydium_ode_element_data_set_name(name,&is_bullet);
    raydium_ode_element_gravity_name(name,0);
//    raydium_ode_element_OnDelete_name(name,rocket_delete);
    raydium_ode_element_ttl_set_name(name,raydium_ode_get_physics_freq()*5); // 5 seconds
//    raydium_sound_SourcePlay(son_rl);
//    raydium_ode_element_sound_update_name("player",son_rl);
//    reload_time=RELOAD_TIME;
    raydium_sound_SourcePlay(son_gun);
    raydium_ode_element_sound_update_name("player",son_gun);
    }
cpt++;
}


void kill_player(void)
{
already_dead=1;
create_game(0);
raydium_log("Killed");
} PHP_v_v(kill_player);

void kill_php(void)
{
life=0;
} PHP_v_v(kill_php);

/////////////////////// Actors creation

void create_game(char alive)
{
#define BREAK_FORCE     130
#define ROTFRICTION     0.0005
int a;


if(!alive)
{
dQuaternion q;
dReal pos[3];
dReal vel[3];
dReal accum[3];

raydium_ode_element_rotq_get_name("player",q);
memcpy(pos,raydium_ode_element_pos_get_name("player"),sizeof(dReal)*3);
// get vel and accums
memcpy(vel,dBodyGetLinearVel(raydium_ode_element[raydium_ode_element_find("player")].body),sizeof(dReal)*3);
memcpy(accum,dBodyGetForce(raydium_ode_element[raydium_ode_element_find("player")].body),sizeof(dReal)*3);

raydium_ode_object_delete_name("PLAYER");

a=raydium_ode_object_create("PLAYER");
raydium_ode_object_box_add("player",a,0.1,RAYDIUM_ODE_AUTODETECT,0,0,RAYDIUM_ODE_STANDARD,0,"lego.tri");
// raydium_ode_element_material_name("player",RAYDIUM_ODE_MATERIAL_SOFT2);
raydium_ode_element_move_name("player",pos);
raydium_ode_element_rotateq_name("player",q);
dBodySetLinearVel(raydium_ode_element[raydium_ode_element_find("player")].body,vel[0],vel[1],vel[2]);
dBodySetForce(raydium_ode_element[raydium_ode_element_find("player")].body,accum[0],accum[1],accum[2]);
dBodySetTorque(raydium_ode_element[raydium_ode_element_find("player")].body,0.1,0.1,0.1);

return;
}

raydium_ode_object_delete_name("BUGGY");

  a=raydium_ode_object_create("BUGGY");
    raydium_ode_object_box_add("buggy_corps",a,1,RAYDIUM_ODE_AUTODETECT,0,0,RAYDIUM_ODE_STANDARD,0,"buggy.tri");


    raydium_ode_object_sphere_add("buggy_pneu_ag",a,0.5,RAYDIUM_ODE_AUTODETECT,RAYDIUM_ODE_STANDARD,0,"buggy_r.tri");
    raydium_ode_element_rotfriction_name("buggy_pneu_ag",ROTFRICTION);
    raydium_ode_element_move_name_3f("buggy_pneu_ag",0.42,0.253,-0.280);
    raydium_ode_joint_attach_hinge2_name("buggy_suspet_ag","buggy_corps","buggy_pneu_ag",RAYDIUM_ODE_JOINT_SUSP_DEFAULT_AXES);
    raydium_ode_joint_break_force_name("buggy_suspet_ag",BREAK_FORCE);

    raydium_ode_object_sphere_add("buggy_pneu_ad",a,0.5,RAYDIUM_ODE_AUTODETECT,RAYDIUM_ODE_STANDARD,0,"buggy_r.tri");
    raydium_ode_element_rotfriction_name("buggy_pneu_ad",ROTFRICTION);
    raydium_ode_element_move_name_3f("buggy_pneu_ad",0.42,-0.253,-0.280);
    raydium_ode_joint_attach_hinge2_name("buggy_suspet_ad","buggy_corps","buggy_pneu_ad",RAYDIUM_ODE_JOINT_SUSP_DEFAULT_AXES);
    raydium_ode_joint_break_force_name("buggy_suspet_ad",BREAK_FORCE);

    raydium_ode_object_sphere_add("buggy_pneu_rg",a,0.5,RAYDIUM_ODE_AUTODETECT,RAYDIUM_ODE_STANDARD,0,"buggy_r.tri");
    raydium_ode_element_rotfriction_name("buggy_pneu_rg",ROTFRICTION);
    raydium_ode_element_move_name_3f("buggy_pneu_rg",-0.444,0.3,-0.260);
    raydium_ode_joint_attach_hinge2_name("buggy_suspet_rg","buggy_corps","buggy_pneu_rg",RAYDIUM_ODE_JOINT_SUSP_DEFAULT_AXES);
    raydium_ode_joint_hinge2_block_name("buggy_suspet_rg",1);
    raydium_ode_joint_break_force_name("buggy_suspet_rg",BREAK_FORCE);

    raydium_ode_object_sphere_add("buggy_pneu_rd",a,0.5,RAYDIUM_ODE_AUTODETECT,RAYDIUM_ODE_STANDARD,0,"buggy_r.tri");
    raydium_ode_element_rotfriction_name("buggy_pneu_rd",ROTFRICTION);
    raydium_ode_element_move_name_3f("buggy_pneu_rd",-0.444,-0.3,-0.260);
    raydium_ode_joint_attach_hinge2_name("buggy_suspet_rd","buggy_corps","buggy_pneu_rd",RAYDIUM_ODE_JOINT_SUSP_DEFAULT_AXES);
    raydium_ode_joint_hinge2_block_name("buggy_suspet_rd",1);
    raydium_ode_joint_break_force_name("buggy_suspet_rd",BREAK_FORCE);

    raydium_ode_motor_create("buggy_moteur",a,RAYDIUM_ODE_MOTOR_ENGINE);
    raydium_ode_motor_attach_name("buggy_moteur","buggy_suspet_ag",1);
    raydium_ode_motor_attach_name("buggy_moteur","buggy_suspet_ad",1);
    raydium_ode_motor_power_max_name("buggy_moteur",0.05);
    raydium_ode_motor_gears_set_name("buggy_moteur",gears,6);
    raydium_ode_motor_gear_change_name("buggy_moteur",gear);


    raydium_ode_motor_create("buggy_direction",a,RAYDIUM_ODE_MOTOR_ANGULAR);
    raydium_ode_motor_attach_name("buggy_direction","buggy_suspet_ag",0);
    raydium_ode_motor_attach_name("buggy_direction","buggy_suspet_ad",0);
    raydium_ode_motor_power_max_name("buggy_direction",0.2);

    raydium_ode_element_ray_attach_name("buggy_corps",10,1,0,0);
    raydium_ode_element_ray_pos_name_3f("buggy_corps",0,0.2,0,0.225);
    {
    //dReal pos[3]={2,2,2};
    //raydium_ode_object_move(a,pos);
    }
    raydium_ode_element_particle_offset_name_3f("buggy_corps","smoke.prt",-0.5,0,0);
//    raydium_ode_element_particle_offset_name_3f("buggy_corps","dust.prt",-0.5,0,-0.3);

/*
raydium_ode_object_delete_name("TANK");

  a=raydium_ode_object_create("TANK");
    raydium_ode_object_box_add("tank_corps",a,1,RAYDIUM_ODE_AUTODETECT,0,0,RAYDIUM_ODE_STANDARD,0,"t80b.tri");
    raydium_ode_element_move_name_3f("tank_corps",-2,0,0);

    raydium_ode_object_box_add("tank_tour",a,1,RAYDIUM_ODE_AUTODETECT,0,0,RAYDIUM_ODE_STANDARD,0,"t80t.tri");
    raydium_ode_element_move_name_3f("tank_tour",-2,0,0.4);

    raydium_ode_joint_attach_hinge_name("pivot","tank_corps","tank_tour",-2,0,0.4,RAYDIUM_ODE_JOINT_AXE_Z);
    raydium_ode_joint_hinge_limits_name("pivot",-2.5,2.5);
    raydium_ode_motor_create("pivot_motor",a,RAYDIUM_ODE_MOTOR_ENGINE);
    raydium_ode_motor_attach_name("pivot_motor","pivot",0);
    raydium_ode_motor_power_max_name("pivot_motor",0.1);

    raydium_ode_object_box_add("tank_canon",a,1,RAYDIUM_ODE_AUTODETECT,0,0,RAYDIUM_ODE_STANDARD,0,"t80c.tri");
    raydium_ode_element_move_name_3f("tank_canon",-1,0,0.30);

    raydium_ode_joint_attach_hinge_name("trukatach","tank_tour","tank_canon",-1.8,0,0.3,RAYDIUM_ODE_JOINT_AXE_Y);
    raydium_ode_joint_hinge_limits_name("trukatach",0,0.5);
    raydium_ode_motor_create("truk_motor",a,RAYDIUM_ODE_MOTOR_ANGULAR);
    raydium_ode_motor_attach_name("truk_motor","trukatach",0);
    raydium_ode_motor_power_max_name("truk_motor",1);
*/

    life=100;
    already_dead=0;
    reload_time=RELOAD_TIME;
    raydium_ode_object_delete_name("PLAYER");
    a=raydium_ode_object_create("PLAYER");
    raydium_ode_object_sphere_add("player",a,0.1,RAYDIUM_ODE_AUTODETECT,RAYDIUM_ODE_STANDARD,0,"lego.tri");
    raydium_ode_element_material_name("player",RAYDIUM_ODE_MATERIAL_SOFT2);
    raydium_ode_element_player_set_name("player",1);
    raydium_ode_motor_create("player_react",a,RAYDIUM_ODE_MOTOR_ROCKET);
    raydium_ode_motor_rocket_set_name("player_react","player",0,0,0);
    raydium_ode_motor_rocket_playermovement_name("player_react",1);
    raydium_ode_element_slip_name("ground",RAYDIUM_ODE_SLIP_ICE/2.f);
    raydium_ode_element_OnBlow_name("player",blow_touched);
//    raydium_ode_element_move_name_3f("player",3,3,1);
#define DEFAULT_RAND_DEC raydium_random_f(-3,3)
#define DEFAULT_DEC DEFAULT_RAND_DEC,DEFAULT_RAND_DEC,0
raydium_ode_object_move_name_3f("BUGGY", DEFAULT_DEC);
raydium_ode_object_move_name_3f("PLAYER",DEFAULT_DEC);

}

void display(void)
{
dReal speed;
dReal direct;
dReal *tmp;

raydium_joy_key_emul();

if(raydium_key[GLUT_KEY_F1]) { raydium_projection_fov/=(1.04); raydium_window_view_update(); }
if(raydium_key[GLUT_KEY_F2]) { raydium_projection_fov*=(1.04); raydium_window_view_update(); }

// sets motor speed
if((vue==4 || vue==5 || vue==6) && life>0)
{
speed=0;

// keyboard or joy settings
if(raydium_joy_y>0) speed=raydium_joy_y*55;
//if(raydium_joy_z<0) speed=(raydium_joy_z)*-55;


direct=raydium_joy_x*0.3; // 0.2 is "perfect" for clio stability

raydium_ode_motor_speed_name("buggy_moteur",-speed);
raydium_ode_motor_angle_name("buggy_direction",direct);

if( (raydium_key_last==1113 || raydium_joy_click==6) && gear<5)
 gear++;

if( (raydium_key_last==1119 || raydium_joy_click==5) && gear>0)
 gear--;

}

if(vue==8 && life>0 && 0)
{
static dReal angle=0;
speed=raydium_joy_x;
angle+=raydium_joy_y*0.01;
raydium_ode_motor_speed_name("pivot_motor",speed);
raydium_ode_motor_angle_name("truk_motor",angle);

if(raydium_mouse_click==1 && reload_time<0)
    {
    // launch projectile
    int a;
    char name[255];
    a=raydium_ode_object_find("TANK");
    raydium_ode_name_auto("ob",name);
    strcpy(cam,name);
    raydium_ode_object_box_add(name,a,0.02,RAYDIUM_ODE_AUTODETECT,0,0,RAYDIUM_ODE_STANDARD,0,"rocket.tri");
    raydium_ode_launcher_simple_name_3f(name,"tank_canon",0,90,0,ROCKET_SPEED);
    raydium_ode_element_rotate_direction_name(name,0);
    raydium_ode_element_data_set_name(name,&is_explosive_tag);
//    raydium_ode_element_gravity_name(name,0);
    raydium_ode_element_OnDelete_name(name,rocket_delete);
    raydium_ode_element_ttl_set_name(name,raydium_ode_get_physics_freq()*10); // 10 seconds
    raydium_sound_SourcePlay(son_rl);
    raydium_ode_element_sound_update_name("tank_tour",son_rl);
    reload_time=RELOAD_TIME;
    }
}

if(vue==7 && life>0)
{
int dir=5;
speed=PLAYER_SPEED;
ptmp=0;

// directions indices:
// 1    2    3
//      ^
// 4 <- 5 -> 6
//      v
// 7    8    9

if(raydium_key[GLUT_KEY_LEFT])  dir=4;
if(raydium_key[GLUT_KEY_RIGHT]) dir=6;
if(raydium_key[GLUT_KEY_UP]) dir=2;
if(raydium_key[GLUT_KEY_DOWN]) dir=8;

if(raydium_key[GLUT_KEY_LEFT] && raydium_key[GLUT_KEY_UP]) dir=1;
if(raydium_key[GLUT_KEY_RIGHT] && raydium_key[GLUT_KEY_UP]) dir=3;

if(raydium_key[GLUT_KEY_LEFT] && raydium_key[GLUT_KEY_DOWN]) dir=7;
if(raydium_key[GLUT_KEY_RIGHT] && raydium_key[GLUT_KEY_DOWN]) dir=9;

switch(dir)
    {
    case 1:
        ptmp=-45;
        break;
    case 2:
        ptmp=0;
        break;
    case 3:
        ptmp=45;
        break;
    case 4:
        ptmp=-90;
        break;
    case 5:
        speed=0; // stops player
        break;
    case 6:
        ptmp=90;
        break;
    case 7:
        ptmp=-135;
        break;
    case 8:
        ptmp=180;
        break;
    case 9:
        ptmp=135;
        break;
    }

raydium_ode_motor_speed_name("player_react",speed);
raydium_ode_motor_rocket_orientation_name("player_react",0,90,ptmp);
if(raydium_mouse_click==3 && raydium_ode_element_touched_get_name("player")) raydium_ode_launcher_name_3f("player","player",0,0,0,PLAYER_JUMP);
if(raydium_mouse_click==1 && reload_time<0)
    {
    // launch rocket
    int a;
    char name[RAYDIUM_MAX_NAME_LEN];
    a=raydium_ode_object_find("PLAYER");
    raydium_ode_name_auto("mav",name);
    strcpy(cam,name);
    raydium_ode_object_box_add(name,a,0.1,RAYDIUM_ODE_AUTODETECT,0,0,RAYDIUM_ODE_STANDARD,0,"rocket.tri");
    raydium_ode_launcher_simple_name_3f(name,"player",0,cam_angle_v+5,0,ROCKET_SPEED);
    raydium_ode_element_rotate_direction_name(name,0);
    raydium_ode_element_data_set_name(name,&is_explosive_tag);
    raydium_ode_element_gravity_name(name,0);
    raydium_ode_element_OnDelete_name(name,rocket_delete);
    raydium_ode_element_ttl_set_name(name,raydium_ode_get_physics_freq()*10); // 10 seconds
    raydium_ode_element_particle_name(name,"smoke.prt");
    raydium_sound_SourcePlay(son_rl);
    raydium_ode_element_sound_update_name("player",son_rl);
    reload_time=RELOAD_TIME;
    }

}

if(life<=0 && !already_dead)
    kill_player();

if(raydium_mouse_click==1 && life<=0)
    create_game(1);


if(raydium_key_last==1027) exit(0);
if(raydium_key_last==1114) record*=-1;
if(raydium_key_last==1116) draw_debug*=-1;
if(raydium_key_last==1109) mouse_grab*=-1;

if(raydium_key_last==1032)
    {

    int a;
    char name[RAYDIUM_MAX_NAME_LEN];
    a=raydium_ode_object_find("GLOBAL");
    raydium_ode_name_auto("box",name);
    raydium_ode_object_box_add(name,a,0.1,RAYDIUM_ODE_AUTODETECT,0,0,RAYDIUM_ODE_STANDARD,0,"crate.tri");
    raydium_ode_element_move_name_3f(name,raydium_random_neg_pos_1(),raydium_random_neg_pos_1(),2);

    //raydium_php_exec("test.php");
    }

if(raydium_key_last==1097) { movie_spf=0; raydium_ode_time_change(0); }
if(raydium_key_last==1122) { movie_spf=3; raydium_ode_time_change(10); }
if(raydium_key_last==1101) { movie_spf=16; raydium_ode_time_change(100); }
if (raydium_key_last=='d'+1000)
    raydium_ode_autodisable_set(1);
if (raydium_key_last=='D'+1000)
    raydium_ode_autodisable_set(0);


if(raydium_key_last==1109 && 0)  // disabled
 {
 int elems[10];
 elems[0]=raydium_ode_element_find("buggy_corps");
 elems[1]=raydium_ode_element_find("buggy_pneu_ag");
 elems[2]=raydium_ode_element_find("buggy_pneu_ad");
 elems[3]=raydium_ode_element_find("buggy_pneu_rd");
 elems[4]=raydium_ode_element_find("buggy_pneu_rg");
 raydium_ode_element_fix("merged_elem",elems,5,0);
 }


//if(raydium_key_last==1098)

if(raydium_key_last==5)
{
vue=5;
raydium_projection_near=10;
raydium_projection_fov=2;
raydium_window_view_update();
}

if(raydium_key_last==6)
{
vue=6;
raydium_projection_near=0.05 ;
raydium_projection_fov=60;
raydium_window_view_update();

}

if(raydium_key_last==4)
{
vue=4;
raydium_projection_near=0.05;
raydium_projection_fov=40;
raydium_window_view_update();

}

if(raydium_key_last==7)
{
vue=7;
raydium_projection_near=0.05;
raydium_projection_fov=70;
raydium_window_view_update();
}


if(raydium_key_last==8)
{
vue=8;
raydium_projection_near=0.05;
raydium_projection_fov=70;
raydium_window_view_update();
}
if(raydium_key_last==9)
{
vue=9;
raydium_projection_near=0.05;
raydium_projection_fov=70;
raydium_window_view_update();
}

if(raydium_key_last==3)
{
vue=3;
raydium_projection_near=0.05;
raydium_projection_fov=120;
raydium_window_view_update();
}

//if(raydium_key_last==1100)
//    raydium_ode_element_moveto_name("buggy_pneu_ag","GLOBAL",1);


if(mouse_grab==1 && vue!=9)
{
 float delta_x,delta_y;
 int x,y;
 float zoomfact=1;
 zoomfact=raydium_projection_fov/70;
 raydium_mouse_grab_delta(&x,&y);
 delta_x=x*sensivity*zoomfact;
 delta_y=y*sensivity*zoomfact;
 cam_angle_h += (delta_x*0.008f); // rads
 if(life>0)
  cam_angle_v += (delta_y*0.458f); // degs

 raydium_ode_element_player_angle_name("player",cam_angle_h);
 if(cam_angle_v<1) cam_angle_v=1;
 if(cam_angle_v>179) cam_angle_v=179;
}



{
static int reload_done=1;
if(reload_time<1 && !reload_done)
    {
    reload_done=1;
    raydium_sound_SourcePlay(son_reload);
    raydium_ode_element_sound_update_name("player",son_reload);
    }
if(reload_time>1) reload_done=0;
}


//raydium_light_position[0][0]=0.1;
//raydium_light_position[0][1]=0.1;
raydium_light_position[0][0]=50;
raydium_light_position[0][1]=150;
raydium_light_position[0][2]=200;
raydium_light_position[0][3]=1.0;
//raydium_light_update_all(0);


{
/*dReal *pos;
pos=raydium_ode_element_pos_get_name("buggy_corps");
if(pos)
    raydium_particle_generator_move_name("smoke",pos);*/
}


raydium_clear_frame();
glLoadIdentity();

// camera + drawing
if(raydium_ode_element_find(cam)<0) strcpy(cam,"player");
tmp=raydium_ode_element_pos_get_name(cam);
if(vue==5) raydium_camera_look_at(camx,camy,camz,tmp[1],-tmp[2],tmp[0]);
if(vue==6) raydium_ode_element_camera_inboard_name("buggy_corps",-0.15,0,0.1,2,0,0);
if(vue==4) raydium_ode_element_camera_inboard_name("buggy_corps",0,0.35,-0.2,2,0,-0.2);
if(vue==7) raydium_ode_element_camera_inboard_name("player",0,0,0.1, raydium_math_sin(cam_angle_v),0,raydium_math_cos(cam_angle_v));
if(vue==8) raydium_camera_freemove(RAYDIUM_CAMERA_FREEMOVE_NORMAL);
if(vue==3) raydium_ode_element_camera_inboard_name(cam,0,0,0.1, 1,0,0);
if(vue==9) raydium_camera_freemove(RAYDIUM_CAMERA_FREEMOVE_FIXED);


raydium_ode_draw_all(RAYDIUM_ODE_DRAW_NORMAL);
raydium_ode_draw_all(RAYDIUM_ODE_DRAW_RAY);

if(draw_debug==1)
    {
    raydium_ode_draw_all(RAYDIUM_ODE_DRAW_DEBUG);
    raydium_ode_draw_all(RAYDIUM_ODE_DRAW_AABB);
    raydium_osd_network_stat_draw(5,30,20);
    }


speed=raydium_ode_motor_speed_get_name("buggy_moteur",0);
raydium_osd_printf(2,4,12,0.5,"font2.tga","^fcar speed: %f",speed);
raydium_joy_ff_autocenter(speed/2.f);

speed=raydium_ode_motor_speed_get_name("buggy_moteur",1);
if(vue!=7)
    {
    raydium_osd_printf(10,10,30,0.5,"font2.tga","%i",gear);
    raydium_osd_printf(13,10,15,0.5,"font2.tga","%.1f",speed);
    }

if(paf_time>0.04 && speed>1 && !raydium_key[GLUT_KEY_UP])
    {
    paf_time=0;
    if(raydium_random_i(0,10)==0) raydium_sound_SourcePlay(son_paf);
    }

//if(speed<10 && gear>1) gear--;
//if(speed>45 && gear<5) gear++;
raydium_ode_motor_gear_change_name("buggy_moteur",gear);

speed*=0.1; // wheel radius
speed/=4;
speed+=0.5;
speed+=(raydium_random_neg_pos_1()/15);
raydium_sound_SetSourcePitch(son,raydium_math_abs(speed));
raydium_ode_element_sound_update_name("buggy_corps",son);
raydium_ode_element_sound_update_name("buggy_corps",son_paf);

raydium_osd_printf(2,98,16,0.5,"font2.tga","- %3i FPS - tech demo %s for Raydium %s, CQFD Corp.",raydium_render_fps,version,raydium_version());
if(vue==9 || vue==8) {
    dReal pos[3];
    dReal dist;
    int id;

    id = raydium_ode_mouse_pick(100,pos,&dist);
    raydium_osd_printf((100.0f*raydium_mouse_x)/raydium_window_tx,100-(100.0f*raydium_mouse_y)/raydium_window_ty,16,0.5,"font2.tga","+ %d %.3f %.3f %.3f %.3f",id,pos[0],pos[1],pos[2],dist);
    if (raydium_mouse_button[0] && id>0){
        if (raydium_ode_element[id].state == RAYDIUM_ODE_STANDARD){
            pos[1]=pos[0]=0;
            pos[2]=5;
            raydium_ode_element_addforce(id,pos);
        }
    }
}

{
char c='2';
if(life<50) c='e';
if(life<25) c='c';
raydium_osd_printf(2,15,20,0.5,"font2.tga","Health: ^%c%.1f %%^f",c,life);
}


{
raydium_ode_Ray r;
if(raydium_ode_element_ray_get_name("buggy_corps",0,&r) && r.min_elem>=0)
    raydium_osd_printf(2,30,18,0.5,"font2.tga","%.3f %s",r.min_dist,raydium_ode_element[r.min_elem].name);
}


//raydium_osd_printf(2,80,18,0.5,"font2.tga","^f Lag-O-meter: %.2f ms",raydium_netwok_queue_ack_delay/(double)raydium_timecall_clocks_per_sec*1000);
//raydium_osd_printf(2,80,18,0.5,"font2.tga","AF: %i",raydium_ode_element_ground_texture_get_name("buggy_pneu_ad"));
{
//dReal rx,ry,rz;
//raydium_ode_element_rot_get_name("buggy_corps",&rx,&ry,&rz);
//raydium_osd_printf(2,80,18,0.5,"font2.tga","^fRot %.2f %.2f %.2f",rx,ry,rz);
}

if(reload_time>0)
 raydium_osd_printf(7,20,15,0.5,"font2.tga","^c[RELOADING]^f");

if(life<=0)
    raydium_osd_printf(10,50,30,0.5,"font2.tga","Push FIRE button to restart game.");
else
    if(vue==7) raydium_osd_draw_name("BOXcross.tga",49,49,51,51);

if(life<=0)
 raydium_osd_mask(blood);


//raydium_osd_mask_texture_name("shadowmap",1);

//raydium_osd_network_stat_draw(5,30,20);
raydium_osd_logo("logo6.tga");
memset(raydium_vertex_tag,0,RAYDIUM_MAX_VERTICES);
raydium_rendering_finish();

//raydium_ode_network_element_send_random(4);
//raydium_ode_network_element_send_all();
raydium_ode_network_element_send_iterative(RAYDIUM_ODE_NETWORK_OPTIMAL);
}


int main(int argc, char **argv)
{
char server[128];
char model[RAYDIUM_MAX_NAME_LEN];
int i;

raydium_init_args(argc,argv);
raydium_window_create(640,480,RAYDIUM_RENDERING_WINDOW,version);
raydium_texture_filter_change(RAYDIUM_TEXTURE_FILTER_TRILINEAR);
raydium_projection_near=0.05;
raydium_projection_far=1000;
raydium_projection_fov=70;
//raydium_light_disable();
raydium_fog_disable();
raydium_light_on(0);
//raydium_light_on(1);
memcpy(raydium_light_color[0],sun,raydium_internal_size_vector_float_4);
//raydium_light_intensity[0]=400;
raydium_light_intensity[0]=1000000;
raydium_light_update_all(0);

raydium_shadow_enable();

if(raydium_init_cli_option("server",server))
     if(!raydium_network_client_connect_to(server))
        exit(1);

raydium_sound_DefaultReferenceDistance=4.f;

//son=raydium_sound_LoadWav("../wav/clio/scooter.wav");
son=raydium_sound_LoadWav("murcielago2.wav");
raydium_sound_SetSourcePitch(son,0);
//raydium_sound_SetSourceGain(son,0.05);  // Engine Gain
raydium_sound_SourcePlay(son);

son_rl=raydium_sound_LoadWav("xf_launch1.wav");
raydium_sound_SetSourceLoop(son_rl,0);

son_reload=raydium_sound_LoadWav("xf_reload.wav");
raydium_sound_SetSourceLoop(son_reload,0);

son_paf=raydium_sound_LoadWav("xf_paf.wav");
raydium_sound_SetSourceLoop(son_paf,0);

son_gun=raydium_sound_LoadWav("xf_gun1.wav");
raydium_sound_SetSourceLoop(son_gun,0);

for(i=0;i<SOUND_MAX_EXPLOS;i++)
    {
    son_boom[i]=raydium_sound_LoadWav(wav_boom[i]);
    raydium_sound_SetSourceLoop(son_boom[i],0);
    }

for(i=0;i<SOUND_MAX_BONGS;i++)
    {
    son_bong[i]=raydium_sound_LoadWav(wav_bong[i]);
    raydium_sound_SetSourceLoop(son_bong[i],0);
    }

//raydium_osd_cursor_set("BOXcursor.tga",4,4);
strcpy(raydium_console_config_texture,"logo_console.tga");
raydium_sky_box_cache();


strcpy(cam,"invalid_name");
raydium_register_variable(&sensivity,RAYDIUM_REGISTER_FLOAT,"sensivity");
raydium_register_variable(&camx,RAYDIUM_REGISTER_FLOAT,"camx");
raydium_register_variable(&camy,RAYDIUM_REGISTER_FLOAT,"camy");
raydium_register_variable(&camz,RAYDIUM_REGISTER_FLOAT,"camz");
raydium_register_variable(cam,RAYDIUM_REGISTER_STR,"cam");

//raydium_register_variable(&ptmp,RAYDIUM_REGISTER_FLOAT,"ptmp");
raydium_register_variable(&vue,RAYDIUM_REGISTER_INT,"vue");
raydium_register_function(C2PHP(kill_php),"kill");
raydium_timecall_add(frame_step,-1);


raydium_init_cli_option_default("ground",model,"a.tri");
raydium_ode_ground_set_name(model);

create_game(1);
raydium_ode_CollideCallback=collide;
//raydium_ode_RayCallback=collide; // funny: it will kill player when crossing the ray.
raydium_ode_ExplosionCallback=boom;
raydium_ode_StepCallback=step;

//raydium_ode_time_change(0);

raydium_php_exec("test6.php");
//raydium_sound_load_music("memak5.ogg");
raydium_sound_load_music("memak5loud.ogg");

raydium_callback(&display);
return 0;
}
