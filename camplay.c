#include "raydium/index.c"

char cam_file[RAYDIUM_MAX_NAME_LEN];
char cam1_file[RAYDIUM_MAX_NAME_LEN];
char tri_file[RAYDIUM_MAX_NAME_LEN];

GLfloat sun[]={1.0,0.9,0.9,1.0};
GLfloat speed=1;
GLfloat smooth=2;

void display(void)
{
static GLfloat secs=0;
 
raydium_joy_key_emul();

if(raydium_key_last==1027)
    exit(0);

raydium_clear_frame();

secs+=raydium_frame_time*speed;
raydium_camera_smooth_path_to_path(cam_file,secs,cam1_file,secs,raydium_frame_time*smooth);

if(raydium_key_last==GLUT_KEY_F3 && speed>=0.25)
    speed-=0.25;
if(raydium_key_last==GLUT_KEY_F4)
    speed+=0.25;

if(raydium_key_last==GLUT_KEY_F5 && smooth>1)
    smooth--;
if(raydium_key_last==GLUT_KEY_F6)
    smooth++;

raydium_object_draw_name(tri_file);
if(raydium_key[GLUT_KEY_F1])
    raydium_camera_path_draw_name(cam_file);
if(raydium_key[GLUT_KEY_F2])
    raydium_camera_path_draw_name(cam1_file);

raydium_osd_printf(3,98,16,0.5,"font2.tga","Camera Path Demo");
raydium_osd_printf(3,94,16,0.5,"font2.tga","speed=%.2f (adjust with F3 and F4)",speed);
raydium_osd_printf(3,90,16,0.5,"font2.tga","max speed limit=%.2f (smoothing, adjust with F5 and F6)",smooth);
raydium_osd_printf(3,86,16,0.5,"font2.tga","F1: show camera path / F2: show 'look at' path");

raydium_rendering_finish();
}

int main(int argc, char **argv)
{
raydium_init_args(argc,argv);
raydium_window_create(640,480,RAYDIUM_RENDERING_WINDOW,"Small .cam file player");
raydium_texture_filter_change(RAYDIUM_TEXTURE_FILTER_TRILINEAR);
raydium_projection_near=0.01;
raydium_projection_far=1000;
raydium_projection_fov=60;
raydium_window_view_update();
raydium_light_on(0);
memcpy(raydium_light_color[0],sun,raydium_internal_size_vector_float_4);
raydium_light_intensity[0]=1000000;

raydium_light_position[0][0]=50;
raydium_light_position[0][1]=150;
raydium_light_position[0][2]=200;

raydium_light_update_all(0);
raydium_background_color_change(sun[0],sun[1],sun[2],sun[3]);
raydium_fog_disable();

raydium_log("---------");
raydium_log("Options:");
raydium_log("--cam file.cam : change the camera path");
raydium_log("--cam1 file1.cam : change the 'look at' camera path");
raydium_log("--tri file.tri : change the rendered model");
raydium_log("Note: You can use the same file for cam and cam1");
raydium_log("---------");
if(raydium_init_cli_option("help",NULL))
    exit(1);

strcpy(tri_file,"quiris.tri");
strcpy(cam_file,"quiris.cam");
strcpy(cam1_file,"quiris1.cam");
raydium_init_cli_option("tri",tri_file);
raydium_init_cli_option("cam",cam_file);
raydium_init_cli_option("cam1",cam1_file);

raydium_callback(&display);

return 0;
}

