/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

// Early animation support demo for Raydium

#include "raydium/index.c"

GLfloat sun[]={1.0,0.9,0.8,1.0};


char model[RAYDIUM_MAX_NAME_LEN];

void display(void)
{
static float anm=0;
static int anmid=0;
static enum {automatic,step} mode = automatic;

raydium_joy_key_emul();

if(raydium_key_last==1027) exit(0);


raydium_clear_frame();


//raydium_camera_look_at(0.4,-0.4,0, 0,-0.02,0);
raydium_camera_freemove(RAYDIUM_CAMERA_FREEMOVE_NORMAL);
if (mode == automatic)
    anm+=(raydium_frame_time*10);

if (raydium_key_last == 1000+' ')
    mode=(mode==automatic)?step:automatic;

if (raydium_key_last == 1000 + 'f')
    {
    anm+=1.0f;
    mode=step;
    }
if (raydium_key_last == 1000 + 'b')
    {
    anm=raydium_math_max(0,anm+1.0f);
    mode=step;
    }


if (raydium_key_last == 1000 + 'F')
    {
    anm+=0.1f;
    mode=step;
    }

if (raydium_key_last == 1000 + 'B')
    {
    anm=raydium_math_max(0,anm-0.1f);
    mode=step;
    }


if(raydium_key_last==1000+'p')
    {
    anmid=raydium_math_max(0,anmid-1);
    anm=0;
    }

if(raydium_key_last==1000+'n')
    {
    anmid=raydium_math_min(anmid+1,raydium_object_anims[raydium_object_find(model)]-1);
    raydium_log("%d",raydium_object_anims[raydium_object_find(model)]);
    anm=0;
    }

raydium_object_anim(raydium_object_find(model),0,anmid);
raydium_object_anim_frame_name(model,0,anm);

raydium_object_anim_instance_name(model,0);
raydium_object_draw_name(model);


raydium_osd_printf(7,20,15,0.5,"font2.tga","%s %s(%d) - Use 'n'/'p' to switch to next / previous animation",model\
                   ,raydium_object_anim_names[raydium_object_find(model)][anmid],anmid);

raydium_osd_printf(7,18,15,0.5,"font2.tga"," -> %3.2f - Use 'f'/'b' to switch to forward / backward frame",anm);
raydium_osd_printf(7,16,15,0.5,"font2.tga"," - Use 'F'/'B' to switch to forward /  backward frame with 1/10 step");
raydium_osd_printf(7,14,15,0.5,"font2.tga"," - Use space to return to normal animation speed.");

raydium_rendering_finish();
}


int main(int argc, char **argv)
{
raydium_init_args(argc,argv);
raydium_window_create(640,480,RAYDIUM_RENDERING_WINDOW,"Animation test");
raydium_texture_filter_change(RAYDIUM_TEXTURE_FILTER_TRILINEAR);
raydium_projection_near=0.01;
raydium_projection_far=1000;
raydium_projection_fov=60;
raydium_window_view_update();

raydium_light_on(0);
memcpy(raydium_light_color[0],sun,raydium_internal_size_vector_float_4);
raydium_light_intensity[0]=100000;

raydium_light_position[0][0]=50;
raydium_light_position[0][1]=150;
raydium_light_position[0][2]=200;

raydium_light_update_all(0);
raydium_background_color_change(sun[0],sun[1],sun[2],sun[3]);
raydium_fog_disable();

raydium_init_cli_option_default("model",model,"sas.tri");
//strcpy(model,"testa.tri");

raydium_object_load(model);

raydium_callback(&display);

return 0;
}
