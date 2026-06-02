/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

// This file is a default skeleton. Replace all [ ... ] sections with yours.
// Have fun !

#include "raydium/index.c"

int spriteid;
int debug=0;
int spritegravity=0;
float delta_x=0.01;
float delta_y=0.01;

int switchgravity(void)
{
        spritegravity=spritegravity?0:1;
        raydium_ode_element_gravity (raydium_sprite_object_get(spriteid), spritegravity);
        return spritegravity;
}

void display(void)
{
int moving=0;
raydium_clear_frame();  
raydium_joy_key_emul();

if(raydium_key_last==1027)
    exit(0);
    
if(raydium_key_last==1000+'d')
        debug=debug?0:1;
        
if(raydium_key_last==1000+'g')
        switchgravity();
if(raydium_key[GLUT_KEY_UP])
{
        if(raydium_sprite_group_current[spriteid]!=5)raydium_sprite_group_change(spriteid,5);
        raydium_sprite_move_relative(spriteid,-delta_x,0,0);
        moving=1;
}
if(raydium_key[GLUT_KEY_DOWN])
{
        if(raydium_sprite_group_current[spriteid]!=2)raydium_sprite_group_change(spriteid,2);
        raydium_sprite_move_relative(spriteid,delta_x,0,0);
        moving=1;
}
if(raydium_key[GLUT_KEY_LEFT])
{
        if(raydium_sprite_group_current[spriteid]!=3)raydium_sprite_group_change(spriteid,3);
        raydium_sprite_move_relative(spriteid,0,-delta_y,0);
        moving=1;
}
if(raydium_key[GLUT_KEY_RIGHT])
{
        if(raydium_sprite_group_current[spriteid]!=4)raydium_sprite_group_change(spriteid,4);
        raydium_sprite_move_relative(spriteid,0,delta_y,0);
        moving=1;
}
if(!moving) if(raydium_sprite_group_current[spriteid]!=1)raydium_sprite_group_change(spriteid,1);


raydium_camera_look_at(10,-2,1,0,0,0);

raydium_ode_draw_all(RAYDIUM_ODE_DRAW_NORMAL);
if(debug) raydium_ode_draw_all(RAYDIUM_ODE_DRAW_DEBUG);
raydium_sprite_display(spriteid);
raydium_rendering_finish();
}


int main(int argc, char **argv)
{
debug=0;
raydium_init_args(argc,argv);
raydium_window_create(800,600,RAYDIUM_RENDERING_WINDOW,"Sprite test application");

raydium_texture_filter_change(RAYDIUM_TEXTURE_FILTER_ANISO);
raydium_window_view_perspective(60,0.01,2500); // fov 60 + near and far planes

raydium_fog_disable();    
raydium_light_enable();
raydium_light_on(0);

raydium_light_conf_7f(0,50,150,200,1000000,1,0.9,0.7); // id, pos, intensity and color (RGB)
raydium_background_color_change(1,0.9,0.7,1);

raydium_sky_box_cache();
spriteid=raydium_sprite_load("sprite1-test.sprite",0);
raydium_sprite_move(spriteid,0,0,-0.75);
raydium_ode_ground_set_name("cocorobix.tri");

raydium_callback(&display);
return(0);
}

// EOF
