/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

#include "raydium/index.c"

// Reset to initial state.
void reset(void)
{
int lf;

for(lf=0;lf<RAYDIUM_MAX_LENSFLARES;lf++)
    raydium_lensflare_reset(lf);

if(raydium_ode_object_isvalid(raydium_ode_object_find("UFO")))
    raydium_ode_object_delete_name("UFO");

if(raydium_ode_object_isvalid(raydium_ode_object_find("CAR")))
    raydium_ode_object_delete_name("CAR");

raydium_sky_box_name(NULL);
raydium_sky_box_cache();
}

// Toggle the specified FX group of all lens flare effects.
void toggle(int fx)
{
int lf;

for(lf=0;lf<RAYDIUM_MAX_LENSFLARES;lf++)
    raydium_lensflare_fx_switch(lf,fx);
}

// Print values from all used lens flares.
void dump(void)
{
int lf,fx;

raydium_log("Lens flare dump:");
raydium_log("----------------");

for(lf=0;lf<RAYDIUM_MAX_LENSFLARES;lf++)
    if(raydium_lensflare_internal_state[lf])
    {
    raydium_log("<*> lf%i, x%.3f, y%.3f, z%.3f",lf,
        raydium_lensflare_position[lf][0],
        raydium_lensflare_position[lf][1],
        raydium_lensflare_position[lf][2]);
    for(fx=0;fx<RAYDIUM_LENSFLARE_MAX_FX;fx++)
        if(raydium_lensflare_fx_internal_state[lf][fx])
        raydium_log(" +  fx%i, s%.1f, v%.1f, r%.1f, g%.1f, b%.1f, a%.1f",fx,
            raydium_lensflare_fx_size[lf][fx],
            raydium_lensflare_fx_velocity[lf][fx],
            raydium_lensflare_fx_color[lf][fx][0],
            raydium_lensflare_fx_color[lf][fx][1],
            raydium_lensflare_fx_color[lf][fx][2],
            raydium_lensflare_fx_color[lf][fx][3]);
    }
}

void display(void)
{
int ufo;
char name[RAYDIUM_MAX_NAME_LEN];

raydium_joy_key_emul();

switch(raydium_key_last)
    {
    case 1027:exit(0);break;
    case GLUT_KEY_F12:reset();break;
    case 1000+'1':toggle(0);break;
    case 1000+'2':toggle(1);break;
    case 1000+'3':toggle(2);break;
    case 1000+'4':toggle(3);break;
    case 1000+'5':toggle(4);break;
    case 1000+'6':toggle(5);break;
    case 1000+'7':toggle(6);break;
    case 1000+'8':toggle(7);break;
    case 1009:dump();break;
    case GLUT_KEY_PAGE_UP:raydium_lensflare_enable();break;
    case GLUT_KEY_PAGE_DOWN:raydium_lensflare_disable();break;
    }

// Siege sunlight
if(raydium_key_last==GLUT_KEY_F2)
    {
    int sun;

    reset();

    sun=raydium_lensflare_create("sunlight","sun.lf");
    raydium_lensflare_move_3f(sun,1000,1750,800);

    raydium_sky_box_name("siege");
    raydium_sky_box_cache();
    }

// Desert sunlight
if(raydium_key_last==GLUT_KEY_F3)
    {
    int sun;

    reset();

    sun=raydium_lensflare_create("sunlight","sun.lf");
    raydium_lensflare_move_3f(sun,1000,-1750,1300);

    raydium_sky_box_name("desert");
    raydium_sky_box_cache();
    }

// Peak sunlight
if(raydium_key_last==GLUT_KEY_F4)
    {
    int sun;

    reset();

    sun=raydium_lensflare_create("sunlight","6-bladed.lf");
    raydium_lensflare_move_3f(sun,-50,1750,1250);

    raydium_sky_box_name("peak");
    raydium_sky_box_cache();
    }

// Headlights
if(raydium_key_last==GLUT_KEY_F5)
    {
    int obj;

    reset();

    obj=raydium_ode_object_create("CAR");
    raydium_ode_object_box_add("fiesta",obj,0.2,1.2,0.6,0.4,RAYDIUM_ODE_STATIC,0,"fiesta.tri");
    raydium_ode_object_move_3f(obj,0,0,-1.05);

    raydium_ode_element_lensflare_offset_name_3f("fiesta","headlight_left","lamp.lf",0.55,-0.15,-0.05);
    raydium_ode_element_lensflare_offset_name_3f("fiesta","headlight_right","lamp.lf",0.55,0.15,-0.05);
    }

// Starry sky
if(raydium_key_last==GLUT_KEY_F6)
    {
    int star;

    reset();

    for(star=0;star<RAYDIUM_MAX_LENSFLARES;star++)
        {
        raydium_lensflare_on(star);
        raydium_lensflare_texture_name(star,"");
        raydium_lensflare_fx_on(star,2);
        raydium_lensflare_fx_size_change(star,2,0.4);
        raydium_lensflare_fx_velocity_change(star,2,1.2);
        raydium_lensflare_fx_color_rgba(star,2,1.0,1.0,1.0,0.6);
        raydium_lensflare_move_3f(star,
            raydium_random_f(-30.0,30.0),
            raydium_random_f(-30.0,30.0),
            raydium_random_f(25.0,50.0));
        }

    raydium_sky_box_name("starfield");
    raydium_sky_box_cache();
    }

// UFO invasion
#define UFO_SPEED 50
#define UFO_REL_SPEED 5
if(raydium_key_last==GLUT_KEY_F7)
    {
    int obj;
    float vect[3];

    reset();

    vect[0]=raydium_random_f(-UFO_SPEED,UFO_SPEED);
    vect[1]=raydium_random_f(-UFO_SPEED,UFO_SPEED);
    vect[2]=0;

    obj=raydium_ode_object_create("UFO");
    for(ufo=0;ufo<3;ufo++)
        {
        sprintf(name,"ufo%i",ufo);
        raydium_ode_object_box_add(name,obj,1,RAYDIUM_ODE_AUTODETECT,0,0,RAYDIUM_ODE_STANDARD,0,"ufo.tri");
        raydium_ode_element_gravity_name(name,0);
        raydium_ode_element_addforce_name_3f(name,
            vect[0]+raydium_random_f(-UFO_REL_SPEED,UFO_REL_SPEED),
            vect[1]+raydium_random_f(-UFO_REL_SPEED,UFO_REL_SPEED),
            0);

        raydium_ode_element_move_name_3f(name,
            raydium_random_f(-ufo*3,ufo*3),
            raydium_random_f(-ufo*3,ufo*3),
            raydium_random_f(3+ufo*3,5+ufo*5));
        raydium_ode_element_lensflare_offset_name_3f(name,name,"6-bladed.lf",0,0,-0.3);
        }

    raydium_sky_box_name("starfield");
    raydium_sky_box_cache();
    }

// Rotate UFOs
for(ufo=0;ufo<3;ufo++)
    {
    sprintf(name,"ufo%i",ufo);
    if(raydium_ode_object_isvalid(raydium_ode_object_find(name)))
        {
        static float rot[3]={0,0,0};

        rot[2]+=raydium_frame_time/5;
        if(rot[2]>2*M_PI)rot[2]=0;
        raydium_ode_object_rotate_name(name,rot);
        }
    }

raydium_clear_frame();
raydium_camera_freemove(RAYDIUM_CAMERA_FREEMOVE_NORMAL);
raydium_ode_draw_all(RAYDIUM_ODE_DRAW_NORMAL);

raydium_osd_printf(2,98,16,0.5,"font2.tga","^F- %2i FPS - Lens flare effect demo for Raydium %s, CQFD Corp.^F",raydium_render_fps,raydium_version());

if(raydium_key[GLUT_KEY_F1])
    {
    raydium_osd_printf(2,32,16,0.5,"font2.tga","^BF2   Siege sunlight^F");
    raydium_osd_printf(2,29,16,0.5,"font2.tga","^BF3   Desert sunlight^F");
    raydium_osd_printf(2,26,16,0.5,"font2.tga","^BF4   Peak sunlight^F");
    raydium_osd_printf(2,23,16,0.5,"font2.tga","^DF5   Car headlights^F");
    raydium_osd_printf(2,20,16,0.5,"font2.tga","^FF6   Starry sky^F");
    raydium_osd_printf(2,17,16,0.5,"font2.tga","^FF7   UFO invasion^F");
    raydium_osd_printf(2,14,16,0.5,"font2.tga","^EF12  Reset to initial state^F");
    raydium_osd_printf(2,11,16,0.5,"font2.tga","^E1..8 Toggle FX groups^F");
    raydium_osd_printf(2, 8,16,0.5,"font2.tga","^ETAB  Print values^F");
    raydium_osd_printf(2, 5,16,0.5,"font2.tga","^APgUp Activate^F");
    raydium_osd_printf(2, 2,16,0.5,"font2.tga","^CPgDn Deactivate^F");
    }
else
    raydium_osd_printf(2,2,16,0.5,"font2.tga","^FPress F1 for help^F");

raydium_osd_logo("logo.tga");
raydium_rendering_finish();
}

int main(int argc, char **argv)
{
raydium_init_args(argc,argv);
raydium_window_create(640,480,RAYDIUM_RENDERING_WINDOW,"Lens flare effect demo");

raydium_texture_filter_change(RAYDIUM_TEXTURE_FILTER_TRILINEAR);
raydium_window_view_perspective(60,0.01,2500);

strcpy(raydium_console_config_texture,"logo_console.tga");
raydium_background_color_change(1,0.9,0.7,1);
raydium_sky_box_cache();

raydium_fog_disable();

raydium_light_enable();
raydium_light_on(0);
raydium_light_conf_7f(0,50,150,200,1000000,1,0.9,0.7);

raydium_lensflare_enable();

raydium_ode_ground_set_name("cocorobix.tri");

raydium_callback(&display);

return(0);
}
