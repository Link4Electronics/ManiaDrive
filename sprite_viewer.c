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
char spritename[255];
float depth=10.0f;
int debug=0;

void display(void)
{
    raydium_joy_key_emul();

    if(raydium_key_last==1027)      exit(0);

    //if(raydium_key_last==1000+'s') raydium_sprite_dump_info(spriteid);

    if(raydium_key_last==1000+'1') raydium_sprite_group_change(spriteid,1);
    if(raydium_key_last==1000+'2') raydium_sprite_group_change(spriteid,2);
    if(raydium_key_last==1000+'3') raydium_sprite_group_change(spriteid,3);
    if(raydium_key_last==1000+'4') raydium_sprite_group_change(spriteid,4);
    if(raydium_key_last==1000+'5') raydium_sprite_group_change(spriteid,5);
    if(raydium_key_last==1000+'n') raydium_sprite_group_change(spriteid,raydium_sprite_get_current_group(spriteid)+1);
    if(raydium_key_last==1000+'p') raydium_sprite_group_change(spriteid,raydium_sprite_get_current_group(spriteid)-1);


    if(raydium_key_last==1000+'+') raydium_sprite_change_sprite_time_relative(spriteid,1);
    if(raydium_key_last==1000+'-') raydium_sprite_change_sprite_time_relative(spriteid,-1);

    if(raydium_key[GLUT_KEY_F1])    depth+=1.5*raydium_frame_time;
    if(raydium_key[GLUT_KEY_F2])    depth+=-3*raydium_frame_time;
    //if the move is ommited, the bounding box will fall. Need fix????
    raydium_sprite_move_relative(spriteid,0,0,0);
    raydium_clear_frame();
    raydium_camera_look_at(depth,0,0,0,0,0);
    raydium_ode_draw_all(RAYDIUM_ODE_DRAW_NORMAL);
    if(debug)raydium_ode_draw_all(RAYDIUM_ODE_DRAW_DEBUG);
    //OSD
    raydium_osd_color_rgba(1,1,1,1);
    raydium_osd_printf(3,95,15,0.5,"font2.tga","Showing sprite: \"%s\"",spritename);
    raydium_osd_printf(3,92,15,0.5,"font2.tga","Current animation group: %d",raydium_sprite_group_current[spriteid]);
    raydium_osd_printf(3,89,15,0.5,"font2.tga","Current frame: %d",raydium_sprite_current_frame[spriteid]);
    raydium_osd_printf(3,86,15,0.5,"font2.tga","Status of the sprite: %s",((raydium_sprite_is_stopped(spriteid))?"stopped":"playing"));
    raydium_osd_printf(3,83,15,0.5,"font2.tga","Time between frames: %.2f",raydium_sprite_time[spriteid]);
    raydium_osd_printf(3,80,15,0.5,"font2.tga","Press F1 and F2 to move the camera");
    raydium_osd_printf(3,77,15,0.5,"font2.tga","Press d key to see ODE debug");
    raydium_osd_printf(3,74,15,0.5,"font2.tga","Press 1 to 5 to change animation group");
    raydium_osd_printf(3,71,15,0.5,"font2.tga","Press + or - to increase/decrese the time betwen frames");
    raydium_osd_printf(3,68,15,0.5,"font2.tga","Press n(next) or p(previous) to change to the next/previous animation group");

    raydium_osd_logo("logo.tga");
    raydium_sprite_display(spriteid);
    //switch debug
    if(raydium_key_last==1000+'d') debug=debug?0:1;


    raydium_rendering_finish();
}

int main(int argc, char **argv)
{
    raydium_init_args(argc,argv);
    if(!argv[1])
        {
        raydium_log("\n\n\nERROR!!!!!\n You must indicate a valid sprite file in the command line. Example:\n./odyncomp.sh sprite_viewer.c sprite1-test.sprite\n\n\n");
        return 0;
        }

    raydium_init_load("sprite_viewer.config");
    raydium_log("Loading sprite: %s",argv[1]);

    if((spriteid=raydium_sprite_load((char*)argv[1],0))==-1)
        {
        raydium_log("\n\n\nERROR!!!!!\n You must indicate a valid sprite file in the command line. Example:\n./odyncomp.sh sprite_viewer.c sprite1-test.sprite\n\n\n");
        return 0;
        }

    strcpy(spritename,argv[1]);
    raydium_ode_element_gravity (raydium_sprite_object_get(spriteid), 1);
    raydium_callback(&display);
    return 0;
}

// EOF
