/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

// Small JPGS movie player.

#include "raydium/index.c"


#define ALPHA_SECONDS   3
float alpha=0;
float unzoom;

float ratio_str2float(char *str)
{
char p1[RAYDIUM_MAX_NAME_LEN];
char p2[RAYDIUM_MAX_NAME_LEN];
int i1,i2;

raydium_parser_cut(str,p1,p2,'/');
i1=atoi(p1);
i2=atoi(p2);
return i1/(float)i2;
}

void alpha_set(void)
{
alpha=ALPHA_SECONDS;
}

void hms(float t,int *h, int *m, int *s, int *ms)
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


void display(void)
{
int h1,m1,s1,ms1;
int h2,m2,s2,ms2;

raydium_joy_key_emul();

if(raydium_key_last==1027)
    exit(0);

if(raydium_key_last==100)
    {
    raydium_video_seek_rel_name("video",-10);
    alpha_set();
    }
if(raydium_key_last==102)
    {
    raydium_video_seek_rel_name("video",10);
    alpha_set();
    }
if(raydium_key_last==103)
    {
    raydium_video_seek_rel_name("video",-60);
    alpha_set();
    }
if(raydium_key_last==101)
    {
    raydium_video_seek_rel_name("video",60);
    alpha_set();
    }
if(raydium_key_last==1032)
    alpha_set();


raydium_clear_frame();
raydium_osd_draw_name("video",0,100-unzoom,100,unzoom);

if(alpha>0)
    {
    raydium_osd_color_rgba(1,1,1,alpha);
    hms(raydium_video_elapsed_name("video"),&h1,&m1,&s1,&ms1);
    hms(raydium_video_duration_name("video"),&h2,&m2,&s2,&ms2);
    raydium_osd_printf(50,95,32,0.5,"font2.tga","%i:%02i:%02i / %i:%02i:%02i",h1,m1,s1,h2,m2,s2);
    }

alpha-=raydium_frame_time;
if(alpha<0)
    alpha=0;



raydium_osd_color_rgba(1,1,1,1);
raydium_rendering_finish();
}


int main(int argc, char **argv)
{
char video[RAYDIUM_MAX_NAME_LEN];
char sound[RAYDIUM_MAX_NAME_LEN];
char ratio[RAYDIUM_MAX_NAME_LEN];
float screen_ratio;
float video_ratio;

raydium_init_args(argc,argv);

if(!raydium_init_cli_option("file",video))
    {
    printf("******* Usage: jpgs_play --file file.jpgs [--sound file.ogg] [--ratio 4/3]\n");
    printf("******* Ratio is video ratio, not screen one (square screen pixels required for this to work)\n");
    exit(0);
    }

raydium_init_cli_option_default("sound",sound,"");
raydium_init_cli_option_default("ratio",ratio,"4/3");

raydium_window_create(640,480,RAYDIUM_RENDERING_WINDOW,"JPGS movie player");
raydium_texture_filter_change(RAYDIUM_TEXTURE_FILTER_TRILINEAR);
raydium_background_color_change(0,0,0,1);

if(raydium_video_open_with_sound(video,"video",sound) < 0)
    {
    printf("******* Unable to open '%s'\n",video);
    exit(0);
    }

// compute some boring stuff about screen and video ratio ...
screen_ratio=raydium_window_tx/(float)raydium_window_ty;
video_ratio=ratio_str2float(ratio);
unzoom=(100.f-(100.f/(video_ratio/screen_ratio)))/2.f;

raydium_render_fps_limit(120);
raydium_callback(&display);
return(0);
}

// EOF
