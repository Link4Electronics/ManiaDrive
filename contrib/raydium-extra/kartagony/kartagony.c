// Kartagony - CQFD Corp.
// Initial release: 2004/Apr/17
// License: GPL - GNU General Public License, see "raydium/gpl.txt" file.

char *version="version 0.2";

#include "raydium/index.c"

GLfloat sun[]={1.0,0.9,0.5,1.0};
GLfloat amb[]={1.0,0.0,0.0,1.0};

GLfloat frame_time;
GLfloat invin_time;
GLfloat bonusroll_time;
GLfloat boost_time;
GLfloat weight_time;
GLfloat oil_time;
GLfloat replace_time;
GLfloat smoke_time;
int bonus_to_delete;
int have_bonus;
char in_game;
char game_over;

void blow_touched(int elem, dReal force, dReal max_force);
void rocket_del(int rocket);

#include "kart_conf.c"
#include "kart_obj.c"
#include "kart_menu.c"
#include "kart_sound.c"
#include "kart_ode.c"
#include "kart_net.c"
#include "kart_bonus.c"

void blow_touched(int elem, dReal force, dReal max_force)
{
if(invin_time>0) return;
game_state.scores[menu_team]--;
raydium_network_propag_refresh(NET_GAME_STATE_BASE+menu_team);
invin_time=INVINCIBLE_TIME;
}


void explo(char type, dReal radius, dReal force_or_propag, dReal *pos)
{
int g1,g2;
//int i;
//i=raydium_random_i(0,SOUND_MAX_EXPLOS-1);

// raydium_explosion_add(pos[0],pos[1],pos[2],0,0,0,0.6,3,6,0.1);
g1=raydium_particle_generator_load("explosion.prt","explo");
g2=raydium_particle_generator_load("explosmoke.prt","explo_smoke");

raydium_particle_generator_move(g1,pos);
raydium_particle_generator_move(g2,pos);

//raydium_sound_SourcePlay(son_boom[i]);
//raydium_sound_SetSourcePos(son_boom[i],pos);
}

void touch_mine(int mine, int kart)
{
dReal *pos;

pos=raydium_ode_element_pos_get(mine);

if(raydium_ode_network_element_isdistant(mine))
	delete_request(mine);
    else
	raydium_ode_element_delete(mine,1);
}


void touch_oil(int oil, int kart)
{
oil_time=OIL_TIME;

if(raydium_ode_network_element_isdistant(oil))
	delete_request(oil);
    else
	raydium_ode_element_delete(oil,1);
}


void touch_bonus(int bonus, int kart)
{
get_bonus();

if(raydium_ode_network_element_isdistant(bonus))
	delete_request(bonus);
    else
//	raydium_ode_element_delete(bonus,1);
	bonus_to_delete=bonus;
}

void rocket_del(int rocket)
{
if(raydium_ode_network_element_isdistant(rocket))
	delete_request(rocket);
    else
	raydium_ode_element_delete(rocket,1);
}

char is_kart(int elem)
{
int tag;

tag=raydium_ode_element_tag_get(elem);
if(tag>=TAG_KART && tag<TAG_KART_END)
    return 1;
return 0;
}

char collide(int e1, int e2, dContact *c)
{

if( raydium_ode_element_tag_get(e1)==TAG_ROCKET &&
    !raydium_ode_network_element_isdistant(e1) )
	rocket_del(e1);
if( raydium_ode_element_tag_get(e2)==TAG_ROCKET &&
    !raydium_ode_network_element_isdistant(e2) )
	rocket_del(e2);

if( raydium_ode_element_tag_get(e1)==TAG_MINE &&
    is_kart(e2) &&
    !raydium_ode_network_element_isdistant(e2) )
	    {
	    touch_mine(e1,e2);
	    return 0;
	    }

if( raydium_ode_element_tag_get(e2)==TAG_MINE &&
    is_kart(e1) &&
    !raydium_ode_network_element_isdistant(e1) )
	    {
	    touch_mine(e2,e1);
	    return 0;
	    }

if( raydium_ode_element_tag_get(e1)==TAG_BONUS &&
    is_kart(e2) &&
    !have_bonus &&
    !raydium_ode_network_element_isdistant(e2) )
	    {
	    touch_bonus(e1,e2);
	    return 0;
	    }

if( raydium_ode_element_tag_get(e2)==TAG_BONUS &&
    is_kart(e1) &&
    !have_bonus &&
    !raydium_ode_network_element_isdistant(e1) )
	    {
	    touch_bonus(e2,e1);
	    return 0;
	    }

if( raydium_ode_element_tag_get(e1)==TAG_OIL &&
    is_kart(e2) &&
    !raydium_ode_network_element_isdistant(e2) )
	    {
	    touch_oil(e1,e2);
	    return 0;
	    }

if( raydium_ode_element_tag_get(e2)==TAG_OIL &&
    is_kart(e1) &&
    !raydium_ode_network_element_isdistant(e1) )
	    {
	    touch_oil(e2,e1);
	    return 0;
	    }

return 1;
}

void frame_step(GLfloat step)
{
// update timers here
frame_time=step;
if(invin_time>0) invin_time-=step;
bonusroll_time-=step;
boost_time-=step;
weight_time-=step;
oil_time-=step;
if(raydium_key[GLUT_KEY_HOME])
    replace_time+=step;
else
    replace_time=0;
}

void start_game(void)
{
int i;
FILE *fp;

if(menu_networked_game)
    {
    if(!raydium_network_client_connect_to(menu_server_ip))
	{
	menu_main(0);
	return;
	}
    }

fp=fopen("server.last","wt");
if(fp)
    {
    fputs(menu_server_ip,fp);
    fclose(fp);
    }

for(i=0;i<4;i++)
    game_state.scores[i]=DEFAULT_SCORE;

create_kart();
invin_time=0;
have_bonus=0;
boost_time=0;
weight_time=0;
oil_time=0;
replace_time=0;
in_game=1;
smoke_time=0;
game_over=0;
drop_bonus(0);
}


char draw_element_before(int elem)
{
int team;

if(is_kart(elem))
    {
    team=raydium_ode_element_tag_get(elem)-TAG_KART;
    raydium_rendering_rgb_force(TEAM_COL(team));
    }

return 1; // draw element
}

void draw_element_after(int elem)
{
raydium_rendering_rgb_normal();
}


void draw_game(void)
{
dReal speed,direct,accel,slip;
int i;

if(raydium_key_last==1027)
    exit(0); // too "hard"

if(raydium_ode_object_find("KART")<0)
    {
    in_game=0;
    raydium_camera_look_at(0.1,0.1,0,0,1,0);
    raydium_osd_color_ega('f');
    raydium_osd_draw_name("BOXfront.tga",0,100,100,0);
    }
else
    {
    in_game=1;
    
    raydium_joy_key_emul();

    if(game_over)
	{
	raydium_joy_x=raydium_joy_y=raydium_key_last=0;
	}
        
    if(raydium_key_last==1032)
	bonus_fire();

    if(replace_time>REPLACE_TIME)
	{
	create_kart();
	replace_time=0;
	}

    speed=direct=0;
    accel=0.03;

    if(oil_time>0)
	slip=RAYDIUM_ODE_SLIP_ICE;
    else
	slip=RAYDIUM_ODE_SLIP_DEFAULT;

    raydium_ode_element_slip_name("kart_pneu_ag",slip);
    raydium_ode_element_slip_name("kart_pneu_ad",slip);
    raydium_ode_element_slip_name("kart_pneu_rg",slip);
    raydium_ode_element_slip_name("kart_pneu_rd",slip);
    
    if(raydium_joy_y>0.3 || raydium_key[GLUT_KEY_INSERT]) // "insert" is a hack
	{
        speed=raydium_joy_y*25;
	if(boost_time>0) 
	    {
	    speed*=2;
	    accel*=2;
	    }

	if(weight_time>0) 
	    {
	    speed/=2;
	    accel/=2;
	    }
	raydium_ode_motor_power_max_name("kart_moteur",accel);
	raydium_ode_motor_power_max_name("kart_frein",0);
	}
    else
	{
	raydium_ode_motor_power_max_name("kart_moteur",0);
	raydium_ode_motor_power_max_name("kart_frein",0.1 * -raydium_joy_y);
	}

    direct=raydium_joy_x*0.3;
    
    if(raydium_key[GLUT_KEY_END])
	raydium_ode_element_camera_inboard_name("kart_corps",-0.15,0,0.07,-2,0,0);
    else
	raydium_ode_element_camera_inboard_name("kart_corps",0,0,0.15,2,0,0);

    raydium_ode_motor_speed_name("kart_moteur",-speed);
    raydium_ode_motor_angle_name("kart_direction",direct);
    speed=raydium_ode_motor_speed_get_name("kart_moteur",1);
    speed*=0.1; // wheel radius
    speed/=2;
    speed+=0.5; // lower speed
    speed=raydium_trigo_abs(speed);
    if(speed>2) speed=2;
    raydium_sound_SetSourcePitch(sounds[SOUND_ENGINE],speed);
    raydium_ode_element_sound_update_name("kart_corps",sounds[SOUND_ENGINE]);
    
    raydium_ode_draw_all(0);
    //raydium_ode_draw_all(1);
    manage_bonus();
    
    // Here, we test scores <=0 !
    for(i=0;i<4;i++)
	{
	raydium_osd_color_change(TEAM_COL(i));
	if(game_state.scores[i]<=0)
	    {
	    raydium_osd_printf(30,60-(i*5),30,0.5,"font2.tga","%s looses !",teams[i].name);
	    game_over=1;
	    }
	}	
    }
raydium_ode_network_element_send_iterative(RAYDIUM_ODE_NETWORK_OPTIMAL);
}

void display(void)
{
int i;

raydium_background_color_change(sun[0],sun[1],sun[2],sun[3]);
raydium_light_position[0][0]=50;
raydium_light_position[0][1]=150;
raydium_light_position[0][2]=200;
raydium_light_position[0][3]=1.0;

raydium_clear_frame();
glLoadIdentity();

draw_game();

if(menu) MenuItem_draw();

for(i=0;i<4;i++)
    {
    raydium_osd_color_change(TEAM_COL(i));
    raydium_osd_printf(2,90-(i*5),20,0.5,"font2.tga","%s: %i",teams[i].name,game_state.scores[i]);
    }    

raydium_osd_printf(90,2,20,0.4,"font2.tga","^c%i FPS",raydium_render_fps);
//raydium_osd_network_stat_draw(5,5,10);
raydium_osd_logo("logo.tga");
raydium_rendering_finish();
}

int main(int argc, char **argv)
{
raydium_init_args(argc,argv);
raydium_window_create(800,600,RAYDIUM_RENDERING_WINDOW,version);
raydium_texture_filter_change(RAYDIUM_TEXTURE_FILTER_TRILINEAR);
raydium_projection_near=0.05;
raydium_projection_far=1000;
raydium_projection_fov=70;
raydium_fog_disable();
raydium_light_on(0);
memcpy(raydium_light_color[0],sun,raydium_internal_size_vector_float_4);
raydium_light_intensity[0]=1000000;
raydium_light_update_all(0);

memcpy(raydium_light_color[1],amb,raydium_internal_size_vector_float_4);
raydium_light_intensity[1]=10000;
raydium_light_update_all(1);
raydium_window_view_update();

strcpy(raydium_console_config_texture,"logo_console.tga");
raydium_osd_cursor_set("BOXcursor.tga",4,4);
raydium_sky_box_cache();

conf_init();
bonus_init();
sound_init();
menu_main(0);
net_init();

raydium_timecall_add(frame_step,-1);
raydium_ode_ground_set_name("laby1.tri");
raydium_ode_CollideCallback=collide;
raydium_ode_ExplosionCallback=explo;
raydium_ode_BeforeElementDrawCallback=draw_element_before;
raydium_ode_AfterElementDrawCallback=draw_element_after;

//raydium_ode_element_slip_name("ground",RAYDIUM_ODE_SLIP_ICE/5.f);

raydium_callback(&display);
return 0;
}

