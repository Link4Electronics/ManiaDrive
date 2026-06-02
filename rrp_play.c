/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

// RRP records player. See raydium_ode_capture_record() for creating a replay.

#include "raydium/index.c"

signed char help_show=1;
signed char attached=0; // is camera attached to something ?
int attached_id; // if yes, which element is it ?
dReal attached_pos[3]; // and where the camera is ? (relative to this element)
dReal attached_lookat[3]; // and what are we looking at ? (relative, too)


void display(void)
{
static signed char playing=1;

raydium_joy_key_emul();

if(raydium_key_last==1027)
    exit(0);

if(raydium_key_last==1032)
    playing=(playing?0:1);

if(raydium_key_last==1000+'h')
    help_show=(help_show?0:1);

if(raydium_key[GLUT_KEY_F2])
    raydium_ode_capture_speed(10);
else if(raydium_key[GLUT_KEY_F1])
    raydium_ode_capture_speed(-10);
else if(raydium_key[GLUT_KEY_F4])
    raydium_ode_capture_speed(0.1);
else if(raydium_key[GLUT_KEY_F3])
    raydium_ode_capture_speed(-0.1);
else
    raydium_ode_capture_speed( (float)playing );


if(raydium_mouse_click==1)
  {
  if(!attached)
    {
    int id;
    dReal pos[3];
    dReal cam[3];
    dReal dist;

    id = raydium_ode_mouse_pick(100,pos,&dist);

    if(id>=0)
        {
        // attached_pos (camera pos to element's space)
        cam[0]=raydium_camera_x;
        cam[1]=raydium_camera_y;
        cam[2]=raydium_camera_z;
        raydium_ode_element_world2rel(id,cam,attached_pos);
        // attached_lookat (impact pos to element's space)
        raydium_ode_element_world2rel(id,pos,attached_lookat);
        attached_id=id;
        attached=1;
        }
    }
  else // is alread attached
    {
    attached=0;
    }
  }

if(attached && !raydium_ode_element_isvalid(attached_id))
    attached=0;

raydium_clear_frame();

if(!attached)
    raydium_camera_freemove(RAYDIUM_CAMERA_FREEMOVE_NORMAL);
else
    raydium_ode_element_camera_inboard(attached_id,
                    attached_pos[0],attached_pos[1],attached_pos[2],
                    attached_lookat[0],attached_lookat[1],attached_lookat[2]);


raydium_ode_draw_all(0);
if(help_show)
    {
    raydium_osd_printf(2,98,18,0.5,"font2.tga","^CSPACE^F: start/stop  ^CF1/F2^F: -10x/10x  ^CF3/F4^F: -0.1x/0.1x");
    raydium_osd_printf(2,94,18,0.5,"font2.tga","^CLeft Click^F: Attach/detach camera to targeted element");
    raydium_osd_printf(2,90,18,0.5,"font2.tga","^CH^F: Show/hide this message");
    }
raydium_rendering_finish();
}


int main(int argc, char **argv)
{
char file[RAYDIUM_MAX_NAME_LEN];

raydium_init_args(argc,argv);

if(!raydium_init_cli_option("file",file))
    {
    printf("******* Usage: rrp_play --file file.rrp\n");
    printf("******* Demo file (auto-downloaded): demo.rrp\n");
    exit(0);
    }

raydium_window_create(640,480,RAYDIUM_RENDERING_WINDOW,"RRP player");

raydium_texture_filter_change(RAYDIUM_TEXTURE_FILTER_TRILINEAR);
raydium_window_view_perspective(60,0.01,2500); // fov 60 + near and far planes

raydium_fog_disable();
raydium_light_enable();
raydium_light_on(0);

raydium_light_conf_7f(0,50,150,200,1000000,1,0.9,0.7); // id, pos, intensity and color (RGB)
raydium_background_color_change(1,0.9,0.7,1);

raydium_sky_box_cache();
raydium_shadow_enable();



raydium_ode_capture_play(file,1);

raydium_callback(&display);
return(0);
}

// EOF
