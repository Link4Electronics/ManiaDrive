/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

// This small app will allow you to generate camera paths.
// Using the console, you can change speed using "$speed=0.01", for example.

#include "raydium/index.c"

GLfloat cam_angle_x = 0;
GLfloat cam_angle_y = 90;

GLfloat cam_pos_x = 0;
GLfloat cam_pos_y = 0;
GLfloat cam_pos_z = 0;
GLfloat roll = 0;

GLfloat speed = 0.1;
GLint sensibilite = 3;

char model[RAYDIUM_MAX_NAME_LEN];

GLfloat light_color[] = {1.0, 0.9, 0.8, 1.0};

FILE *out,*out1;

void display(void)
{
    
int delta_x, delta_y;

raydium_joy_key_emul();

cam_pos_z += (raydium_math_sin(cam_angle_x+90)*raydium_joy_y*speed*raydium_math_sin(90-cam_angle_y));
cam_pos_x += (raydium_math_cos(cam_angle_x+90)*raydium_joy_y*speed*raydium_math_sin(90-cam_angle_y));
cam_pos_y += (raydium_math_cos(90-cam_angle_y)*speed*raydium_joy_y);
    
cam_pos_x -= (raydium_math_cos(cam_angle_x)*raydium_joy_x*speed);
cam_pos_z -= (raydium_math_sin(cam_angle_x)*raydium_joy_x*speed);
    
if(raydium_key_last==1027)
    exit(0);

if(raydium_key[GLUT_KEY_F1]) { raydium_projection_fov/=(1.04); raydium_window_view_update(); }
if(raydium_key[GLUT_KEY_F2]) { raydium_projection_fov*=(1.04); raydium_window_view_update(); }

if(raydium_key[GLUT_KEY_F3]) roll--;
if(raydium_key[GLUT_KEY_F4]) roll++;

delta_x = raydium_mouse_x - (raydium_window_tx/2);
cam_angle_x += (delta_x*sensibilite*0.1f); 

delta_y = raydium_mouse_y - (raydium_window_ty/2);
cam_angle_y += (delta_y*sensibilite*0.1f); 

raydium_mouse_move(raydium_window_tx/2, raydium_window_ty/2);


raydium_light_position[0][0]=100;
raydium_light_position[0][1]=100;
raydium_light_position[0][2]=100;
raydium_light_position[0][3]=1.0;

raydium_light_update_position(0); 

raydium_background_color_change(light_color[0],light_color[1],light_color[2],light_color[3]);
    
raydium_clear_frame();
raydium_camera_place(cam_pos_x,cam_pos_y,cam_pos_z,cam_angle_x,cam_angle_y,roll);

raydium_camera_replace();
raydium_object_draw_name(model);

if(raydium_key_last==1032)
    {
    GLfloat from[]={1,1,1,1};
    GLfloat to[]={1,1,1,0};

    GLfloat pos[3];
    GLfloat pos2[3];

    fprintf(out,"%f %f %f %f %f\n",cam_pos_z,cam_pos_x,-cam_pos_y,raydium_projection_fov,roll);
    
    pos[0]=10;
    pos[1]=0;
    pos[2]=0;
    raydium_math_rotate(pos,0,cam_angle_y,cam_angle_x,pos2);
    
    fprintf(out1,"%f %f %f %f %f\n",cam_pos_z+pos2[0],cam_pos_x+pos2[1],-cam_pos_y+pos2[2],raydium_projection_fov,roll);
    raydium_osd_fade_from(from,to,0.2,NULL);
    }

raydium_osd_printf(2,98,16,0.48,"font2.tga","^fF1/F2: Zoom - F3/F4: Roll - Space: add path point - see cam.cam / cam1.cam");
    
raydium_rendering_finish();
}




int main(int argc, char **argv)
{
    raydium_init_args(argc,argv);
    
    raydium_window_create(640,480,RAYDIUM_RENDERING_WINDOW,"Camera path maker (willou.c based)");
    raydium_texture_filter_change(RAYDIUM_TEXTURE_FILTER_TRILINEAR);
    raydium_projection_near=0.01;
    raydium_projection_far=2500;
    raydium_projection_fov=60;
    raydium_fog_disable();
    raydium_window_view_update();
    
    raydium_light_on(0);
    memcpy(raydium_light_color[0],light_color,raydium_internal_size_vector_float_4);
    raydium_light_intensity[0] = 1000000;
    raydium_light_update_all(0);


    raydium_window_view_update();


    strcpy(model,"a.tri");
    raydium_init_cli_option("model",model);

    out=fopen("cam.cam","wt");
    out1=fopen("cam1.cam","wt");

    if(!out)
        {
        raydium_log("cannot open cam.cam (w)");
        return 1;
        }
    if(!out1)
        {
        raydium_log("cannot open cam1.cam (w)");
        return 1;
        }

    raydium_register_variable(&speed, RAYDIUM_REGISTER_FLOAT, "speed");
    raydium_callback(&display);
    return(0);
}
// EOF
