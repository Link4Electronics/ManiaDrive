/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

#include "raydium/index.c"

char type='1';
float mx=0.f;
float my=0.f;
float mz=0.f;



void display(void)
{
raydium_joy_key_emul();

if(raydium_key_last==1027)
    exit(0);

if((raydium_key_last-1000)>='1' && (raydium_key_last-1000)<='8')
{
	type=(raydium_key_last-1000);
	mx=my=mz=0;
}

if(raydium_key[GLUT_KEY_UP])	my+=0.1;
if(raydium_key[GLUT_KEY_RIGHT]) mx-=0.1;
if(raydium_key[GLUT_KEY_DOWN])	my-=0.1;
if(raydium_key[GLUT_KEY_LEFT])	mx+=0.1;

if(raydium_key_last==1000+' ')
    {
    GLfloat vect[3]={10,0,0};
    GLfloat slowness[3]={0.3,0,0};
    type='6';
    raydium_camera_push(RAYDIUM_CAMERA_PUSH_FREEMOVE_REL,vect,slowness);
    }

raydium_clear_frame();

switch(type)
{
	case '1':
		raydium_camera_look_at(10,-2,2,0,0,0);

	break;
	case '2':
		raydium_camera_look_at(2,10,3,0,0,0);

	break;
	case '3':
		raydium_camera_look_at(2,2,10,0,0,0);

	break;
	case '4':
		raydium_camera_smooth(10+mx,-2+my,2+mz,0,0,0,60,0,raydium_frame_time*3);


	break;
	case '5':
		raydium_camera_freemove(RAYDIUM_CAMERA_FREEMOVE_FIXED);

	break;
	case '6':
		raydium_camera_freemove(RAYDIUM_CAMERA_FREEMOVE_NORMAL);

	break;
	case '7':
		raydium_camera_orbitmove(12,2,2);

	break;
	case '8':
		raydium_camera_orbitmove(0,0,0);

	break;

}
raydium_osd_cursor_draw();
/* [ draw here ] */ raydium_ode_draw_all(0);
raydium_texture_current_set(0);
glLineWidth(5);
glBegin(GL_LINES);
//x-red
glColor4f(1,0,0,1);
glVertex3f(0,0,0);
glVertex3f(1,0,0);
//y-blue
glColor4f(0,0,1,1);
glVertex3f(0,0,0);
glVertex3f(0,1,0);
//z-green
glColor4f(0,1,0,1);
glVertex3f(0,0,0);
glVertex3f(0,0,1);
glEnd();
raydium_osd_color_rgba(1,0,0,1);
raydium_osd_printf_3D(0.8,0,0.3,20,0.5,"bitstream.tga","X");
raydium_osd_color_rgba(0,0,1,1);
raydium_osd_printf_3D(0,0.8,0.3,20,0.5,"bitstream.tga","Y");
raydium_osd_color_rgba(0,1,0,1);
raydium_osd_printf_3D(0.3,0.3,0.8,20,0.5,"bitstream.tga","Z");
raydium_osd_color_rgba(1,1,1,1);
float *pos;
pos=raydium_camera_data_get();
raydium_osd_printf(5,95,18,0.42f,"bitstream.tga","Camera position: %3.3f, %3.3f, %3.3f",pos[0],pos[1],pos[2]);
raydium_osd_printf(5,90,18,0.42f,"bitstream.tga","Camera angles: %3.3f, %3.3f, %3.3f",fmodf(pos[3],360),fmodf(pos[4],360),fmodf(pos[5],360));
raydium_osd_printf(5,5,22,0.42f,"bitstream.tga","Press 1,2,3,4,5,6,7,8 to change camera.");

switch(type)
{
	case '1':
		raydium_osd_printf(5,85,18,0.42f,"bitstream.tga","Camera: raydium_camera_look_at(10,-2,2,0,0,0)");

	break;
	case '2':
		raydium_osd_printf(5,85,18,0.42f,"bitstream.tga","Camera: raydium_camera_look_at(2,10,3,0,0,0)");

	break;
	case '3':
		raydium_osd_printf(5,85,18,0.42f,"bitstream.tga","Camera: raydium_camera_look_at(2,2,10,0,0,0)");

	break;
	case '4':
		raydium_osd_printf(5,85,18,0.42f,"bitstream.tga","Camera: raydium_camera_smooth(10+mx,-2+my,2+mz,0,0,0,60,0,raydium_frame_time*3)");

	break;
	case '5':
		raydium_osd_printf(5,85,18,0.42f,"bitstream.tga","Camera: raydium_camera_freemove(RAYDIUM_CAMERA_FREEMOVE_FIXED)");

	break;
	case '6':
		raydium_osd_printf(5,85,18,0.42f,"bitstream.tga","Camera: raydium_camera_freemove(RAYDIUM_CAMERA_FREEMOVE_NORMAL)");

	break;
	case '7':
		raydium_osd_printf(5,85,18,0.42f,"bitstream.tga","Camera: raydium_camera_orbitmove(12,2,2)");

	break;
	case '8':
		raydium_osd_printf(5,85,18,0.42f,"bitstream.tga","Camera: raydium_camera_orbitmove(0,0,0)");

	break;
}
raydium_rendering_finish();
}


int main(int argc, char **argv)
{
raydium_init_args(argc,argv);
raydium_window_create(640,480,RAYDIUM_RENDERING_WINDOW,"Camera test");

raydium_texture_filter_change(RAYDIUM_TEXTURE_FILTER_TRILINEAR);
raydium_window_view_perspective(60,0.01,2500); // fov 60 + near and far planes

raydium_fog_disable();
raydium_light_enable();
raydium_light_on(0);

raydium_light_conf_7f(0,50,150,200,1000000,1,0.9,0.7); // id, pos, intensity and color (RGB)
raydium_background_color_change(1,0.9,0.7,1);

raydium_sky_box_cache();

/* [ place base scene here ] */ raydium_ode_ground_set_name("cocorobix.tri");

raydium_callback(&display);
return(0);
}

// EOF
