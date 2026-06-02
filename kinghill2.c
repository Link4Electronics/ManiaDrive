// TODO :
// timelimit (sync server side ?)
// spectate mode ? (raydium modif : "no element" client keep alive)

#include "raydium/index.c"

void create_car(void);

GLfloat sun[]={1.0,0.9,0.9,1.0};
int son_moteur;
int game_state;
#define FALL_SOUNDS 3
int son_fall[FALL_SOUNDS];

//#define DEFAULT_RESPAWN_ONLY
#define MAX_RESPAWNS            16
GLfloat respawns[MAX_RESPAWNS][6];
int respawn_index=0;
GLfloat ground_z=0;
GLfloat static_camera[3];
GLfloat partytime=0;
short scores[RAYDIUM_NETWORK_MAX_CLIENTS];
char camera_file[128];

#define ATM_TEXTURE_1           "ATM_rlogo3.tga"
#define ATM_TEXTURE_2           "ATM_rlogo4.tga"
float atm_zoom=-1;

#define GAME_MENU               1
#define GAME_GAME               2
#define GAME_END                3
#define GAME_OUT                4

#define TYPE_BALANCIER          6
#define TYPE_CORPS              7

#define NET_REMAINING   (RAYDIUM_NETWORK_PACKET_BASE)


void read_world(char *filename)
{
FILE *fp;
int v;
dReal p[6];
char ent[128];

respawn_index=0;
fp=raydium_file_fopen(filename,"rt");
if(!fp) return;
fscanf(fp,"%i\n",&v);
while(fscanf(fp,"%f %f %f %f %f %f %s\n",&p[0],&p[1],&p[2],&p[3],&p[4],&p[5],ent)!=EOF)
    {
    switch(ent[0])
        {
        case 'r':
            memcpy(respawns[respawn_index],p,sizeof(GLfloat)*6);
            respawn_index++;
            break;
        case 'g':
            ground_z=p[2];
            break;

        }
    }

fclose(fp);
}

void set_world(char *root)
{
char f[256];

sprintf(f,"%s.tri",root);
raydium_ode_ground_set_name(f);

sprintf(f,"%s.ent",root);
read_world(f);

sprintf(camera_file,"%s.cam",root);
} PHP_v_s(set_world)


void atm_zoom_fx(void)
{
float rzoom;

if(atm_zoom<0)
    return;

rzoom=atm_zoom*atm_zoom*atm_zoom;

if(rzoom>800)
    atm_zoom=-1;

raydium_osd_draw_name(ATM_TEXTURE_1,50-rzoom,50-rzoom,50+rzoom,50+rzoom);
raydium_osd_draw_name(ATM_TEXTURE_2,50-rzoom,50-rzoom,50+rzoom,50+rzoom);

atm_zoom+=(raydium_frame_time*15);
}


void game_state_change(int state)
{
int i;

game_state=state;
switch(state)
    {
    case GAME_MENU:
        raydium_gui_show();
        break;
    case GAME_GAME:
        raydium_gui_hide();
        create_car();
        partytime=60;
        break;
    case GAME_END:
        raydium_ode_object_delete_name("WATURE");
        break;
    case GAME_OUT:
        // "tu tombes !"
    i=raydium_random_i(0,2);
    raydium_sound_SourcePlay(son_fall[i]);
    raydium_sound_SetSourcePosCamera(i);

        i=raydium_network_uid;
        if(i<0) i=0;
        scores[i]++;
        raydium_network_propag_refresh(RAYDIUM_NETWORK_PACKET_BASE+i+1);
        break;
    }
}


char collide(int e1, int e2, dContact *c)
{
int i1,i2;
i1=raydium_ode_element_tag_get(e1);
i2=raydium_ode_element_tag_get(e2);

if(i1==TYPE_BALANCIER || i2==TYPE_BALANCIER)
    return 0;

return 1;
}


char draw_element_after(int elem)
{
int type;
dReal *pos;

type=raydium_ode_element_tag_get(elem);
if(type==TYPE_CORPS && raydium_ode_network_element_isdistant(elem))
    {
    int player;
    int his_score;
    int our_score;
    char color;
    pos=raydium_ode_element_pos_get(elem);
    player=raydium_ode_network_element_distantowner(elem);

    his_score=scores[player];
    our_score=scores[raydium_network_uid];
    color=(our_score>=his_score?'c':'a');

    raydium_osd_printf_3D(pos[0],pos[1],pos[2]+0.4,12,0.5,"font2.tga","^f%s^f (^%c%i^f)",raydium_network_name[player],color,his_score);
    }
return 1;
}


void btnErrorOkClick(raydium_gui_Object *w)
{
raydium_gui_window_delete_name("error");
}


void btnTrainingClick(raydium_gui_Object *w)
{
game_state_change(GAME_GAME);
}

void btnConnect(raydium_gui_Object *w)
{
char server[512];

raydium_gui_read_name("menu","edtServer",server);
raydium_gui_read_name("menu","edtPlayerName",raydium_network_name_local);

if(!raydium_network_client_connect_to(server))
    {
    int handle;

    handle=raydium_gui_window_create("error",25,45,50,15);
    raydium_gui_widget_sizes(0,0,18);
    raydium_gui_label_create("lblError",handle,50,80,"Cannot connect to server ...",0,0,0);
    raydium_gui_widget_sizes(15,5,18);
    raydium_gui_button_create("btnErrorOk",handle,35,15,"OK",btnErrorOkClick);

    return;
    }

raydium_parser_db_set("KingHill2-LastServer",server);
game_state_change(GAME_GAME);
}


void create_menu(void)
{
int handle;
char lastserver[RAYDIUM_MAX_NAME_LEN];
char defaultip[RAYDIUM_MAX_NAME_LEN];

raydium_init_cli_option_default("ip",defaultip,"127.0.0.1");
raydium_parser_db_get("KingHill2-LastServer",lastserver,defaultip);

raydium_gui_theme_load("theme-raydium2.gui");

handle=raydium_gui_window_create("menu",45,75,50,20);
raydium_gui_widget_sizes(0,0,18);
raydium_gui_label_create("lblPlayerName",handle,25,85,"Player Name :",0,0,0);
raydium_gui_widget_sizes(25,4,18);
raydium_gui_edit_create("edtPlayerName",handle,47,75,raydium_network_name_local);

raydium_gui_widget_sizes(0,0,18);
raydium_gui_label_create("lblServer",handle,32.5,55,"Server :",0,0,0);
raydium_gui_widget_sizes(25,4,18);
raydium_gui_edit_create("edtServer",handle,47,45,lastserver);

raydium_gui_widget_sizes(15,5,18);
raydium_gui_button_create("btnTraining",handle,5,15,"Training",btnTrainingClick);
raydium_gui_widget_sizes(20,5,18);
raydium_gui_button_create("btnMulti",handle,50,15,"Multiplayer",btnConnect);

}


void create_car(void)
{
int a;
int respawn;

//#define BREAK_FORCE     130
#define BREAK_FORCE     0
#define ROTFRICTION     0.0005

#ifndef IPHONEOS
#define ERP_CFM         0.1,1.0
#else
#define ERP_CFM         (0.1*10),1.0
#endif

raydium_ode_object_delete_name("WATURE");

a=raydium_ode_object_create("WATURE");
raydium_ode_object_box_add("corps",a,0.5,RAYDIUM_ODE_AUTODETECT,0,0,RAYDIUM_ODE_STANDARD,TYPE_CORPS,"jeepch.tri");
raydium_ode_element_slip_name("corps",RAYDIUM_ODE_SLIP_ICE);

raydium_ode_network_next_local_only=1;
raydium_ode_object_box_add("balancier",a,1.8,0.1,0.1,0.1,RAYDIUM_ODE_STANDARD,TYPE_BALANCIER,"");
raydium_ode_element_move_name_3f("balancier",0,0,-0.5);
raydium_ode_joint_attach_fixed_name("balanfixed","corps","balancier");

raydium_ode_object_sphere_add("pneu_ag",a,0.5,RAYDIUM_ODE_AUTODETECT,RAYDIUM_ODE_STANDARD,0,"roue_barbare.tri");
//raydium_ode_element_rotfriction_name("pneu_ag",ROTFRICTION);
raydium_ode_element_move_name_3f("pneu_ag",0.42,0.453,-0.180);
raydium_ode_joint_attach_hinge2_name("suspet_ag","corps","pneu_ag",RAYDIUM_ODE_JOINT_SUSP_DEFAULT_AXES);
raydium_ode_joint_break_force_name("suspet_ag",BREAK_FORCE);
raydium_ode_joint_suspension_name("suspet_ag",ERP_CFM);

raydium_ode_object_sphere_add("pneu_ad",a,0.5,RAYDIUM_ODE_AUTODETECT,RAYDIUM_ODE_STANDARD,0,"roue_barbare.tri");
//raydium_ode_element_rotfriction_name("pneu_ad",ROTFRICTION);
raydium_ode_element_move_name_3f("pneu_ad",0.42,-0.453,-0.180);
raydium_ode_joint_attach_hinge2_name("suspet_ad","corps","pneu_ad",RAYDIUM_ODE_JOINT_SUSP_DEFAULT_AXES);
raydium_ode_joint_break_force_name("suspet_ad",BREAK_FORCE);
raydium_ode_joint_suspension_name("suspet_ad",ERP_CFM);

raydium_ode_object_sphere_add("pneu_rg",a,0.5,RAYDIUM_ODE_AUTODETECT,RAYDIUM_ODE_STANDARD,0,"roue_barbare.tri");
//raydium_ode_element_rotfriction_name("pneu_rg",ROTFRICTION);
raydium_ode_element_move_name_3f("pneu_rg",-0.444,0.453,-0.180);
raydium_ode_joint_attach_hinge2_name("suspet_rg","corps","pneu_rg",RAYDIUM_ODE_JOINT_SUSP_DEFAULT_AXES);
raydium_ode_joint_hinge2_block_name("suspet_rg",1);
raydium_ode_joint_break_force_name("suspet_rg",BREAK_FORCE);
raydium_ode_joint_suspension_name("suspet_rg",ERP_CFM);

raydium_ode_object_sphere_add("pneu_rd",a,0.5,RAYDIUM_ODE_AUTODETECT,RAYDIUM_ODE_STANDARD,0,"roue_barbare.tri");
//raydium_ode_element_rotfriction_name("pneu_rd",ROTFRICTION);
raydium_ode_element_move_name_3f("pneu_rd",-0.444,-0.453,-0.180);
raydium_ode_joint_attach_hinge2_name("suspet_rd","corps","pneu_rd",RAYDIUM_ODE_JOINT_SUSP_DEFAULT_AXES);
raydium_ode_joint_hinge2_block_name("suspet_rd",1);
raydium_ode_joint_break_force_name("suspet_rd",BREAK_FORCE);
raydium_ode_joint_suspension_name("suspet_rd",ERP_CFM);

raydium_ode_motor_create("moteur",a,RAYDIUM_ODE_MOTOR_ENGINE);
raydium_ode_motor_attach_name("moteur","suspet_ag",1);
raydium_ode_motor_attach_name("moteur","suspet_ad",1);
raydium_ode_motor_attach_name("moteur","suspet_rg",1);
raydium_ode_motor_attach_name("moteur","suspet_rd",1);
//raydium_ode_motor_power_max_name("moteur",0.2);

raydium_ode_motor_create("direction",a,RAYDIUM_ODE_MOTOR_ANGULAR);
raydium_ode_motor_attach_name("direction","suspet_ag",0);
raydium_ode_motor_attach_name("direction","suspet_ad",0);
raydium_ode_motor_power_max_name("direction",0.2);

raydium_sound_SourcePlay(son_moteur);

respawn=raydium_random_i(0,respawn_index-1);
#ifndef DEFAULT_RESPAWN_ONLY
raydium_ode_object_move(a,respawns[respawn]);
raydium_ode_object_rotate(a,respawns[respawn]+3);
#endif

//raydium_log("%i (%i)",respawn,respawn_index);
/*raydium_log("%i | %f %f %f | %f %f %f",respawn,
                                  respawns[respawn][0],
                                  respawns[respawn][1],
                                  respawns[respawn][2],
                                  respawns[respawn][3],
                                  respawns[respawn][4],
                                  respawns[respawn][5]);*/
atm_zoom=1;
}


void hms(GLfloat t,int *h, int *m, int *s, int *ms)
{
*h=*m=0;
*ms=(t-(int)t)*1000;
*s=t;
while(*s>=60)
    {
    (*m)++;
    (*s)-=60;
    }

while(*m>=60)
    {
    (*h)++;
    (*m)-=60;
    }
}

void draw_timer(void)
{
int h2,m2,s2,ms2;


partytime-=raydium_frame_time;
if(partytime<0)
    partytime=0;

if(partytime==0)
    game_state_change(GAME_END);

hms(partytime,&h2,&m2,&s2,&ms2);
raydium_osd_printf(74,97,18,0.5,"font2.tga","^fParty %i:%02i:%02i:%03i",h2,m2,s2,ms2);
}


void display_all(void)
{
raydium_ode_draw_all(0);
if(raydium_key[GLUT_KEY_F1]) raydium_ode_draw_all(1);
}

void display(void)
{
float speed=0,direct=0;
float accel=0.2;
char tag=0;
dReal *pos;
dReal cam[3];
static float secs=0;
int i,z;


if(raydium_key_last==1027) exit(0);

raydium_clear_frame();


switch(game_state)
    {
    case GAME_GAME:

        raydium_joy_key_emul();

        direct=raydium_joy_x*0.3;

        if(raydium_joy_y>0.3)
            {
            speed=raydium_joy_y*10;
            raydium_ode_motor_power_max_name("moteur",accel);
            tag=1;
            }
        else
        if(raydium_joy_y<-0.3)
            {
            speed=raydium_joy_y*5;
            raydium_ode_motor_power_max_name("moteur",0.2 * -raydium_joy_y);
            tag=1;
            }

        if(!tag)
            {
                speed=0;
                raydium_ode_motor_power_max_name("moteur",0.05);
            }

        raydium_ode_motor_speed_name("moteur",-speed);
        raydium_ode_motor_angle_name("direction",direct);

        // get element position
        pos=raydium_ode_element_pos_get_name("corps");

        // raydium_log("%f %f",pos[2],ground_z);
#ifndef DEFAULT_RESPAWN_ONLY
        if(pos[2]<ground_z)
            {
            // raydium_log("toto");
            game_state_change(GAME_OUT);
            }
#endif


        // get position of camera in world coords
        raydium_ode_element_RelPointPos_name("corps",-1,0,0,cam);

        // correct z pos (always at top of the car, for example)
        cam[2]=pos[2]+0.5;

        // standard smooth lookat camera
        raydium_camera_smooth(cam[0],cam[1],cam[2],pos[1],-pos[2],pos[0],
                              70,0,raydium_frame_time*3);


        static_camera[0]=cam[0];
        static_camera[1]=cam[1];
        static_camera[2]=cam[2];

        display_all();
        raydium_osd_printf(3.4,96.6,35,0.5,"font2.tga","^0- %s -",raydium_network_name_local);
        raydium_osd_printf(3,97,35,0.5,"font2.tga","^f- %s -",raydium_network_name_local);


        i=raydium_network_uid;
        if(i<0) i=0;
        raydium_osd_printf(2,2,15,0.5,"font2.tga","^fscore: %i",scores[i]);

        raydium_ode_element_sound_update_name("corps",son_moteur);


        speed=raydium_ode_motor_speed_get_name("moteur",1);
        speed/=12;
        speed+=0.5;
        speed+=(raydium_random_neg_pos_1()/15);
        raydium_sound_SetSourcePitch(son_moteur,raydium_math_abs(speed));

        atm_zoom_fx();
    break;


    case GAME_OUT:
        raydium_ode_element_slip_name("corps",RAYDIUM_ODE_SLIP_DEFAULT);
        raydium_ode_motor_power_max_name("moteur",10);
        raydium_ode_motor_speed_name("moteur",0);
        raydium_ode_motor_angle_name("direction",0);
        raydium_ode_element_sound_update_name("corps",son_moteur);
        raydium_sound_SetSourcePitch(son_moteur,0.2);

        if(raydium_key_last==1032 || raydium_mouse_click==1)
            game_state_change(GAME_GAME);


        pos=raydium_ode_element_pos_get_name("corps");
        raydium_camera_smooth(static_camera[0],
                              static_camera[1],
                              static_camera[2],
                              pos[1],-pos[2],pos[0],
                              90,0,raydium_frame_time*3);

        display_all();
        raydium_osd_printf(47,53,15,0.5,"font2.tga","^f[ ^cOUT!^f ]");
    break;

    case GAME_MENU:
        secs+=raydium_frame_time;
        raydium_camera_smooth_path_to_pos(camera_file,0,0,0,secs,2*raydium_frame_time);
        display_all();
    break;

    case GAME_END:
        secs+=raydium_frame_time;
        raydium_camera_smooth_path_to_pos(camera_file,0,0,0,secs,2*raydium_frame_time);
        display_all();
        raydium_osd_printf(47,93,15,0.5,"font2.tga","^f[ ^cSCORES^f ]");
        z=85;

        for(i=0;i<RAYDIUM_NETWORK_MAX_CLIENTS;i++)
        {
            if(raydium_network_name[i][0])
            {
            raydium_osd_printf(40,z,25,0.5,"font2.tga","^f%s %i",raydium_network_name[i],scores[i]);
            z-=10;
            }
        }
    break;

    }


if(game_state==GAME_GAME || game_state==GAME_OUT)
        {
        static dReal max=0;
        dReal *torque;
        dReal *pos;
        dReal posc[3];
        dReal t;


        pos=raydium_ode_element_pos_get_name("corps");
        torque = raydium_ode_element_angularvelocity_get_name("corps");
        t=(torque[0]*torque[0]+torque[1]*torque[1]+torque[2]*torque[2]);
        if(t>max)
            max=t;
        //raydium_log("%f",max);
        // problem !
        if(t>300)
            {
            raydium_log("LIMITS !!!! %f %f %f",pos[0],pos[1],pos[2]);
//          dBodySetAngularVel(raydium_ode_element[raydium_ode_element_find("corps")].body,0,0,0);
//          dBodySetAngularVel(raydium_ode_element[raydium_ode_element_find("pneu_ag")].body,0,0,0);
//          dBodySetAngularVel(raydium_ode_element[raydium_ode_element_find("pneu_ad")].body,0,0,0);
//          dBodySetAngularVel(raydium_ode_element[raydium_ode_element_find("pneu_rg")].body,0,0,0);
//          dBodySetAngularVel(raydium_ode_element[raydium_ode_element_find("pneu_rd")].body,0,0,0);
            memcpy(posc,pos,sizeof(GLfloat)*3);
            create_car();
            raydium_ode_object_move_name("WATURE",posc);
//          pos=raydium_ode_element_pos_get_name("corps");
//          raydium_log("%f %f %f",pos[0],pos[1],pos[2]);
            }

        if(raydium_network_uid>=0)
            draw_timer();
        }

raydium_rendering_finish();

raydium_ode_network_element_send_iterative(RAYDIUM_ODE_NETWORK_OPTIMAL);
}


void netcall_remaining_time(int type, char *buff)
{
float f;
memcpy(&f,buff+RAYDIUM_NETWORK_PACKET_OFFSET,sizeof(float));

partytime=f;
//raydium_log("time synced");
}


int main(int argc, char **argv)
{
int i;
char world[256];

raydium_init_args(argc,argv);
raydium_window_create(640,480,RAYDIUM_RENDERING_FULLSCREEN,"King of the Hill 2");
raydium_compat_add(RAYDIUM_COMPAT_ODE_OBJECT_MOVE_R1099);
raydium_texture_filter_change(RAYDIUM_TEXTURE_FILTER_TRILINEAR);
raydium_projection_near=0.01;
raydium_projection_far=1000;
raydium_projection_fov=60;
raydium_window_view_update();
raydium_light_on(0);
raydium_shadow_enable();
memcpy(raydium_light_color[0],sun,raydium_internal_size_vector_float_4);
raydium_light_intensity[0]=1000000;

raydium_light_position[0][0]=50;
raydium_light_position[0][1]=150;
raydium_light_position[0][2]=200;

raydium_light_update_all(0);
raydium_background_color_change(sun[0],sun[1],sun[2],sun[3]);
raydium_fog_disable();

raydium_ode_CollideCallback=collide;
raydium_ode_AfterElementDrawCallback=draw_element_after;

raydium_register_function(C2PHP(set_world),"set_world");

if(raydium_init_cli_option("world",world))
    set_world(world);
else
    set_world("jeep_hangarl");
//set_world("jeep_hangarl");

raydium_texture_find_by_name(ATM_TEXTURE_1);
raydium_texture_find_by_name(ATM_TEXTURE_2);

raydium_sound_DefaultReferenceDistance=4.f;
son_moteur=raydium_sound_LoadWav("jeepch.wav");
create_menu();

son_fall[0]=raydium_sound_LoadWav("tutombes.wav");
son_fall[1]=raydium_sound_LoadWav("tutombes2.wav");
son_fall[2]=raydium_sound_LoadWav("tutombes3.wav");

for(i=0;i<FALL_SOUNDS;i++)
    raydium_sound_SetSourceLoop(son_fall[i],0);


// scores init
for(i=0;i<RAYDIUM_NETWORK_MAX_CLIENTS;i++)
    {
    scores[i]=0;
    raydium_network_propag_add(RAYDIUM_NETWORK_PACKET_BASE+i+1,scores+i,sizeof(short));
    }
raydium_network_netcall_add(netcall_remaining_time,NET_REMAINING,1);


game_state_change(GAME_MENU);

raydium_callback(&display);

return 0;
}

