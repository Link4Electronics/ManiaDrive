#include "raydium/index.c"

// Small and probably useless test for various Raydium timers.

int fake_load=10;

int timer_0=0;
int timer_1=0;
GLfloat timer_2=0;

void timer_0_callback(void)
{
timer_0++;
}

void timer_1_callback(void)
{
timer_1++;
}

void timer_2_callback(GLfloat step)
{
timer_2+=step;
}


void draw_timers(void)
{
static time_t start;
time_t a;

a=time(NULL);
if(start==0) start=a;

raydium_osd_printf(13,90,16,0.5,"font2.tga","fake load: renders %i times each frame (use +/- to change)",fake_load);

raydium_osd_printf(13,10,20,0.5,"font2.tga","timer 0=%i (400)",timer_0);
raydium_osd_printf(13,20,20,0.5,"font2.tga","timer 1=%f (1)",timer_1/400.f);
raydium_osd_printf(13,30,20,0.5,"font2.tga","timer 2=%f (soft)",timer_2);
raydium_osd_printf(13,40,20,0.5,"font2.tga","timer r=%i (real)",a-start);
}


void display(void)
{
int i;

raydium_joy_key_emul();

if(raydium_key_last==1027) exit(0);

if(raydium_key_last==1000+'+') fake_load++;
if(raydium_key_last==1000+'-' && fake_load>1) fake_load--;

raydium_clear_frame();
raydium_camera_freemove(RAYDIUM_CAMERA_FREEMOVE_NORMAL);

for(i=0;i<fake_load;i++)
    raydium_object_draw_name("cocorobix.tri");

draw_timers();
raydium_rendering_finish();
}



int main(int argc, char **argv)
{
raydium_init_args(argc,argv);
raydium_init_load("simple.conf");

raydium_timecall_add(timer_0_callback,1);
raydium_timecall_add(timer_1_callback,400);
raydium_timecall_add(timer_2_callback,-1);

raydium_callback(&display);

return 0;
}
