/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

// A joystick device test to verify the correct behavior on all supported
// target platforms.

#include "raydium/index.c"

void display(void)
{
if(raydium_key_last==1027)
    exit(0);

raydium_clear_frame();

if(raydium_joy)
    {
    int a,b;
    int x,y;

    // General device information.
    raydium_osd_printf(2,98,16,0.5,"font2.tga","^F%s^F",raydium_joy_name);
    raydium_osd_printf(2,95,16,0.5,"font2.tga","^F%02i axes^F",raydium_joy_n_axes);
    raydium_osd_printf(2,92,16,0.5,"font2.tga","^F%02i buttons^F",raydium_joy_n_buttons);

    // Show axes zones.
    raydium_osd_printf(2,80,16,0.5,"font2.tga","^FA X E S^F");
    raydium_osd_printf(2,77,16,0.5,"font2.tga","^F[^8-----------------------------------^7|^8-----------------------------------^F]");
    raydium_osd_printf(50+47*raydium_joy_x,77,16,0.5,"font2.tga","^CX^F");
    raydium_osd_printf(2,71,16,0.5,"font2.tga","^F[^8-----------------------------------^7|^8-----------------------------------^F]");
    raydium_osd_printf(50+47*raydium_joy_y,71,16,0.5,"font2.tga","^EY^F");
    raydium_osd_printf(2,65,16,0.5,"font2.tga","^F[^8-----------------------------------^7|^8-----------------------------------^F]");
    raydium_osd_printf(50+47*raydium_joy_z,65,16,0.5,"font2.tga","^AZ^F");

    // Show values of all available axes.
    raydium_osd_printf(2,50,16,0.5,"font2.tga","^FR A W   D A T A^F");
    for(a=0;a<RAYDIUM_JOY_MAX_AXIS;a++)
        raydium_osd_printf(2,47-a*3,16,0.5,"font2.tga","^%cA%02i %+.10f^F",(raydium_joy_n_axes>a)?'F':'8',a,raydium_joy_axis[a]);

    // Show button states of all available buttons.
    raydium_osd_printf(2,11,16,0.5,"font2.tga","^FB U T T O N S^F");
    raydium_osd_printf(2,8,16,0.5,"font2.tga","^F#%02i click value^F",raydium_joy_click);
    for(b=0,x=2,y=5;b<RAYDIUM_JOY_MAX_BUTTONS;b++)
        {
        raydium_osd_printf(x,y,16,0.5,"font2.tga","^%cB%02i^F",(raydium_joy_button[b])?(char)raydium_random_i('0','F'):(raydium_joy_n_buttons>b)?'F':'8',b);
        x+=5;
        if(x>95) { x=2; y-=3; }
        }
    }
else
    {
    raydium_osd_printf(2,98,16,0.5,"font2.tga","^FNo device found!^F");
    raydium_osd_printf(2,2,16,0.5,"font2.tga","^FPlease try to specify a device using the --joydev CLI option.^F");
    }

raydium_rendering_finish();
}

int main(int argc,char **argv)
{
raydium_init_args(argc,argv);
raydium_window_create(640,480,RAYDIUM_RENDERING_WINDOW,"JoyDev Test");

raydium_texture_filter_change(RAYDIUM_TEXTURE_FILTER_TRILINEAR);
raydium_window_view_perspective(60,1,100);
raydium_background_color_change(0.2,0.2,0.2,1);
raydium_render_fps_limit(60);

raydium_osd_cursor_set("BOXcursor.tga",4,4);

raydium_callback(&display);
return(0);
}

// EOF
