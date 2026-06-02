/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

// Quick experimentation with wiimotes ...
// You'll need libwiimote, available at http://libwiimote.sourceforge.net/

// Compilation:
// # export LIBWII_INCLUDES=/usr/local/include/libcwiimote-0.4.0/libcwiimote/
// # ./odyncomp.sh wii_test.c

#include "raydium/index.c"

#define SMOOTH_SIZE	10
#define DEG2RAD (PI/180.0)

#define WII_MAX	4
#include "wiimote.h"
#include "wiimote_api.h"

wiimote_t wiimote[WII_MAX];
int nmotes;

void wiimote_refresh(void)
{
int i;

for(i=0;i<nmotes;i++)
    if(wiimote_is_open(&wiimote[i]))
	{
	wiimote[i].mode.acc = 1;

	if (wiimote_update(&wiimote[i]) < 0)
		wiimote_disconnect(&wiimote[i]);
	}
}


signed char wiimote_autoconnect(void)
{
signed char connected=0;
int i;

raydium_log("-----------------------------------------");
raydium_log("Press buttons 1 and 2 of your wiimote ...");
do {
    nmotes = wiimote_discover(wiimote, WII_MAX);
    
    if(nmotes <= 0)
	continue;

    raydium_log("found %i wiimote(s) : ",nmotes);

    for (i=0; i<nmotes; i++) 
	raydium_log("#%i: %s", i+1, wiimote[i].link.r_addr);
	
    raydium_log("connecting to first wiimote ...");

    i=0;
    if (wiimote_connect(&wiimote[i], wiimote[i].link.r_addr) < 0) 
	{
	raydium_log("unable to open wiimote: %s", wiimote_get_error());
	}
    else
	{
	raydium_log("wiimote %i connected",i+1);
	wiimote[i].led.bits  = 1 << i;
	connected++;
	}

}while(!connected);

wiimote_refresh();
return 1;
}


void create_world(void)
{
int a;
a=raydium_ode_object_find("GLOBAL");
raydium_ode_object_box_add("box",a,0.1,RAYDIUM_ODE_AUTODETECT,0,0,RAYDIUM_ODE_STATIC,0,"wiimote.tri");
//raydium_ode_element_material_name("box",0,10);
}


float smooth(float *array, int size, float newval)
{
float f,res,total_factor;
int i;

// 1 - move the oldest value out
for(i=size-2;i>=0;i--)
    array[i+1]=array[i];

// 2 - add current value    
array[0]=newval;

// 3 - do the average
res=0;
total_factor=0;
for(i=0;i<size;i++)
	{
	f=size-i;
	res+=(array[i]*f);
	total_factor+=f;
	}

res/=total_factor;
return res;
}

void display(void)
{
raydium_joy_key_emul();

if(raydium_key_last==1027)
    exit(0);

while(wiimote_pending(&wiimote[0]))
    {
    float rx,ry,rz;
    static float orx,ory;
    static float rx_array[SMOOTH_SIZE];
    static float ry_array[SMOOTH_SIZE];

    wiimote_refresh();
    rx=wiimote[0].tilt.x;
    ry=wiimote[0].tilt.y;
//    rz=wiimote[0].tilt.z;
    rz=180;

    if(isnan(rx))
	rx=orx;
    else
	orx=rx;

    if(isnan(ry))
	ry=ory;
    else
	ory=ry;

    rx*=DEG2RAD;
    ry*=DEG2RAD;
    rz*=DEG2RAD;

    rx=smooth(rx_array,SMOOTH_SIZE,rx);
    ry=smooth(ry_array,SMOOTH_SIZE,ry);

    raydium_ode_element_rotate_name_3f("box",rx,ry,rz);
    }

raydium_clear_frame();
raydium_camera_freemove(RAYDIUM_CAMERA_FREEMOVE_NORMAL);
raydium_ode_draw_all(0);
raydium_rendering_finish();
}


int main(int argc, char **argv)
{
raydium_init_args(argc,argv);

wiimote_autoconnect();

raydium_window_create(640,480,RAYDIUM_RENDERING_WINDOW,"My app");

raydium_texture_filter_change(RAYDIUM_TEXTURE_FILTER_TRILINEAR);
raydium_window_view_perspective(60,0.01,2500); // fov 60 + near and far planes

raydium_fog_disable();    
raydium_light_enable();
raydium_light_on(0);

raydium_light_conf_7f(0,2,0,1,100,1,1,1); // id, pos, intensity and color (RGB)
raydium_background_color_change(1,1,1,1);

raydium_sky_box_cache();

raydium_ode_ground_set_name("showroom_l.tri");
raydium_shadow_enable();
create_world();

raydium_callback(&display);
return(0);
}

// EOF
