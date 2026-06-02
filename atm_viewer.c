/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

#include "raydium/index.c"

#define ATM_MAX	16
int atm_textures[ATM_MAX];
char atm_texnames[ATM_MAX][RAYDIUM_MAX_NAME_LEN];
int atm_count;
float ratio=1;
float zoom=5;

void display(void)
{
float rzoom;
int i;

raydium_joy_key_emul();

if(raydium_key_last==1027)
    exit(0);

raydium_clear_frame();
raydium_camera_look_at(10,-2,2,0,0,0);
raydium_ode_draw_all(0);

if(raydium_key[GLUT_KEY_F1])
	zoom-=(raydium_frame_time*15);
if(raydium_key[GLUT_KEY_F2])
	zoom+=(raydium_frame_time*15);

if(zoom<0) zoom=0;

if(raydium_key[GLUT_KEY_F3])
	ratio-=(raydium_frame_time*30);
if(raydium_key[GLUT_KEY_F4])
	ratio+=(raydium_frame_time*30);

if(zoom<0) zoom=0;

rzoom=zoom*zoom;

for(i=0;i<atm_count;i++)
    raydium_osd_draw(atm_textures[i],50-rzoom-ratio,50-rzoom,50+rzoom+ratio,50+rzoom);

for(i=0;i<atm_count;i++)
    raydium_osd_printf(5,95-(i*3),16,0.4,"font2.tga","layer %i: %s",i,atm_texnames[i]);


raydium_osd_printf(5,5,16,0.4,"font2.tga","^cF1^f: unzoom ^cF2^f: zoom | ^cF3/F4^f: change ratio");

raydium_rendering_finish();
}


int main(int argc, char **argv)
{
char file[RAYDIUM_MAX_NAME_LEN];
char opt[32];
int tex;

raydium_init_args(argc,argv);
raydium_window_create(640,480,RAYDIUM_RENDERING_FULLSCREEN,"My app");

raydium_texture_filter_change(RAYDIUM_TEXTURE_FILTER_TRILINEAR);
raydium_window_view_perspective(60,0.01,2500); // fov 60 + near and far planes

// ATM loading
if(!raydium_init_cli_option("file",file))
    {
    raydium_log_atexit_quiet();
    printf("\"Improved Alpha-Tested Magnification\" distance field texture viewer\n");
    printf("Usage: %s --file ATM_xxx.tga [--file1 ATM_xxx.tga [--file2 ATM_xxx.tga [...]]]\n",argv[0]);
    printf("Sample (working) usage: %s --file ATM_rlogo3.tga --file1 ATM_rlogo4.tga\n",argv[0]);
    return 1;
    }

tex=raydium_texture_find_by_name(file);
if(!tex)
    {
    raydium_log_atexit_quiet();
    printf("Error: invalid texture '%s'\n",file);
    }
strcpy(atm_texnames[atm_count],file);
atm_textures[atm_count++]=tex;

do {
    tex=0;
    sprintf(opt,"file%i",atm_count);
	if(raydium_init_cli_option(opt,file))
	{
	tex=raydium_texture_find_by_name(file);
	if(tex)
	    {
	    strcpy(atm_texnames[atm_count],file);
	    atm_textures[atm_count++]=tex;
	    }
	}
} while(tex);

raydium_fog_disable();
raydium_light_enable();
raydium_light_on(0);

raydium_light_conf_7f(0,50,150,200,1000000,1,0.9,0.7); // id, pos, intensity and color (RGB)
raydium_background_color_change(1,0.9,0.7,1);

raydium_sky_box_cache();

raydium_ode_ground_set_name("cocorobix.tri");

raydium_callback(&display);
return(0);
}

// EOF
