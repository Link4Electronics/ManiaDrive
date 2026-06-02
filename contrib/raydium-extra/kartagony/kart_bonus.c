// Kartagony - Bonus management

#define MAX_BONUS	7

typedef struct Bonus
{
    int  id;
    char icon[RAYDIUM_MAX_NAME_LEN];
    char auto_fire;
    int  priority;
} Bonus;

Bonus bonus[MAX_BONUS];


void bonus_init(void)
{
int i;

bonus_to_delete=-1;

strcpy(bonus[0].icon,"BOXhud_boost.tga");
bonus[0].auto_fire=0;
bonus[0].priority=6;

strcpy(bonus[1].icon,"BOXhud_oil.tga");
bonus[1].auto_fire=0;
bonus[1].priority=6;

strcpy(bonus[2].icon,"BOXhud_jump.tga");
bonus[2].auto_fire=0;
bonus[2].priority=6;

strcpy(bonus[3].icon,"BOXhud_life.tga");
bonus[3].auto_fire=1;
bonus[3].priority=1;

strcpy(bonus[4].icon,"BOXhud_rocket.tga");
bonus[4].auto_fire=0;
bonus[4].priority=4;

strcpy(bonus[5].icon,"BOXhud_weight.tga");
bonus[5].auto_fire=1;
bonus[5].priority=4;

strcpy(bonus[6].icon,"BOXhud_mine.tga");
bonus[6].auto_fire=0;
bonus[6].priority=6;

/*
strcpy(bonus[7].icon,"BOXhud_smoke.tga");
bonus[7].auto_fire=1;
bonus[7].priority=4;
*/

for(i=0;i<MAX_BONUS;i++)
    {
    bonus[i].id=i;
    raydium_texture_find_by_name(bonus[i].icon); // force loading
    }
}

int explo_OnDelete(int explo)
{
dReal *pos;
pos=raydium_ode_element_pos_get(explo);
raydium_ode_explosion_blow(1,80,pos);
return 1;
}


void drop_mine(void)
{
char name[RAYDIUM_MAX_NAME_LEN];
int from;
dReal *pos;

raydium_ode_name_auto("mine",name);
from=raydium_ode_object_find("KART");
raydium_ode_object_box_add(name,from,0.05,RAYDIUM_ODE_AUTODETECT,0,0,RAYDIUM_ODE_STANDARD,TAG_MINE,"mine.tri");
raydium_ode_element_moveto_name(name,"GLOBAL",1);
pos=raydium_ode_element_pos_get_name("kart_corps");
raydium_ode_element_move_name(name,pos);
raydium_ode_element_OnDelete_name(name,explo_OnDelete);
}

void drop_oil(void)
{
char name[RAYDIUM_MAX_NAME_LEN];
int from;
dReal *pos;

raydium_ode_name_auto("oil",name);
from=raydium_ode_object_find("KART");
raydium_ode_object_box_add(name,from,1,RAYDIUM_ODE_AUTODETECT,0,0,RAYDIUM_ODE_STANDARD,TAG_OIL,"oil.tri");
raydium_ode_element_moveto_name(name,"GLOBAL",1);
pos=raydium_ode_element_pos_get_name("kart_corps");
raydium_ode_element_move_name(name,pos);
}


int drop_bonus(int dummy)
{
// get AABB from ground
dReal aabb[6];
dReal pos[3];
dReal *get;
char name[64];
int a,i,tag;

printf("****************\n");

if(!in_game) return 1;

// get "world" (all karts) AABB
memset(aabb,0,sizeof(dReal)*6);
for(i=0;i<RAYDIUM_ODE_MAX_ELEMENTS;i++)
    if(raydium_ode_element_isvalid(i))
	{	
	tag=raydium_ode_element_tag_get(i);
	if(!(tag>=TAG_KART && tag<TAG_KART_END))
	    continue;
	
	get=raydium_ode_element_pos_get(i);
//	raydium_log("%s: %f %f %f",raydium_ode_element[i].name,get[0],get[1],get[2]);
	if(get[0]<aabb[0]) aabb[0]=get[0];
	if(get[0]>aabb[1]) aabb[1]=get[0];

	if(get[1]<aabb[2]) aabb[2]=get[1];
	if(get[1]>aabb[3]) aabb[3]=get[1];

	if(get[2]<aabb[4]) aabb[4]=get[2];
	if(get[2]>aabb[5]) aabb[5]=get[2];
	}

//raydium_log("%f %f | %f %f | %f %f",aabb[0],aabb[1],aabb[2],aabb[3],aabb[4],aabb[5]);
pos[0]=raydium_random_f(aabb[0],aabb[1]);
pos[1]=raydium_random_f(aabb[2],aabb[3]);
pos[2]=aabb[5]+5;
//raydium_log("%f %f %f",pos[0],pos[1],pos[2]);
//raydium_log("---");

a=raydium_ode_object_find("GLOBAL");
raydium_ode_name_auto("bonus",name);
raydium_ode_object_sphere_add(name,a,0.1,RAYDIUM_ODE_AUTODETECT,RAYDIUM_ODE_STANDARD,TAG_BONUS,"bonus.tri");
raydium_ode_element_ttl_set_name(name,raydium_ode_get_physics_freq()*BONUS_TTL);
raydium_ode_element_OnDelete_name(name,drop_bonus);
raydium_ode_element_move_name(name,pos);
//printf("Bonus at: %f %f %f\n",pos[0],pos[1],pos[2]);
return 1; // confirm delete
}

void get_bonus(void)
{
have_bonus=-1;
bonusroll_time=BONUSROLL_TIME;
}


void bonus_fire(void)
{

if(have_bonus<=0) return;

if(have_bonus==1) // boost
    boost_time=BOOST_TIME;    

if(have_bonus==2) // oil
    drop_oil();

if(have_bonus==3) // jump
    raydium_ode_launcher_name_3f("kart_corps","kart_corps",0,0,0,JUMP_FORCE);    

if(have_bonus==4) // life
    {
    game_state.scores[menu_team]++;
    raydium_network_propag_refresh(NET_GAME_STATE_BASE+menu_team);
    }

if(have_bonus==5) // rocket
    {
    // launch projectile
    int a;
    char name[255];
    a=raydium_ode_object_find("KART");
    raydium_ode_name_auto("rocket",name);
    raydium_ode_object_box_add(name,a,0.1,RAYDIUM_ODE_AUTODETECT,0,0,RAYDIUM_ODE_STANDARD,TAG_ROCKET,"rocket.tri");
    if(raydium_key[GLUT_KEY_END])
	raydium_ode_launcher_simple_name_3f(name,"kart_corps",0,-90,0,ROCKET_SPEED);
    else
	raydium_ode_launcher_simple_name_3f(name,"kart_corps",0,90,0,ROCKET_SPEED);
    //raydium_ode_element_rotate_direction_name(name,0);
    raydium_ode_element_gravity_name(name,0);
    raydium_ode_element_OnDelete_name(name,explo_OnDelete);
    raydium_ode_element_ttl_set_name(name,raydium_ode_get_physics_freq()*10); // 10 seconds
//    raydium_sound_SourcePlay(son_rl);
//    raydium_ode_element_sound_update_name("tank_tour",son_rl);
    }

if(have_bonus==6) // weight
    weight_time=WEIGHT_TIME;    

if(have_bonus==7) // mine
    drop_mine();    

have_bonus=0;
}


void draw_bonus(int n,float fact)
{
    raydium_osd_color_ega('f');
    raydium_osd_draw_name(bonus[n].icon,50-(5*fact),90-(5*fact),50+(5*fact),90+(5*fact));
}

void manage_bonus(void)
{

// quiet a hack, I admit :)
if(bonus_to_delete!=-1)
    {
    raydium_ode_element_delete(bonus_to_delete,1);
    bonus_to_delete=-1;
    }

// we must "take" a bonus ?
if(have_bonus==-1 && bonusroll_time<=0)
    {
    int *tableau;
    int total=0;
    int i,j,k;

    for(i=0;i<MAX_BONUS;i++)
	total+=bonus[i].priority;
    
    tableau=malloc(total*sizeof(int));

    for(i=0,k=0;i<MAX_BONUS;i++)
	for(j=0;j<bonus[i].priority;j++,k++)
	    tableau[k]=i;
    
    // must moderate random
    have_bonus=raydium_random_0_x(total);
    have_bonus=tableau[have_bonus]+1;
//    have_bonus=5;
    free(tableau);
    }

if(have_bonus==-1)
    draw_bonus( ((int)(bonusroll_time*10)) % MAX_BONUS ,1);

if(have_bonus>0) 
    {
    static float s=1;
    static char fact=BONUSBLINK_FACT;

    s+=frame_time*fact;
    if(s>2) fact=-BONUSBLINK_FACT;
    if(s<1) fact= BONUSBLINK_FACT;

    if(bonusroll_time<-BONUSBLINK_TIME) 
	{
	s=1;
	if(bonus[have_bonus-1].auto_fire)
	    {
	    bonus_fire();
	    return;
	    }
	}
    
    draw_bonus(have_bonus-1,s);
    }
}
