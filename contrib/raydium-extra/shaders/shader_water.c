/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

#define NO_ODE_SUPPORT
#include "raydium/index.c"

//#define CAM_FILE "water_cam1.cam"
#define CAM_FILE "cam.cam"

GLint water_textures_size;
GLfloat water_height=-0.5;
float water_color[4]={0.5f, 0.6f, 0.6f, 1.0f};

const float kNormalMapScale = 0.25f;
float g_WaterFlow = 0.12f;
float g_WaterUV = 35.0f;

char model[RAYDIUM_MAX_NAME_LEN];

GLfloat light_color[] = {1.0, 0.9, 0.8, 1.0};
GLfloat  *back_color=light_color;

void display(void)
{
    double plane_reflection[4] = {0.0, 0.0, 1.0, -water_height};
    static GLfloat secs=0;

    raydium_joy_key_emul();

    if(raydium_key_last==1027)
	exit(0);

//    if(raydium_key_last==1032)
//        raydium_capture_frame_auto();

    if(raydium_key[GLUT_KEY_F1]) { raydium_projection_fov/=(1.04); raydium_window_view_update(); }
    if(raydium_key[GLUT_KEY_F2]) { raydium_projection_fov*=(1.04); raydium_window_view_update(); }

    if(raydium_key_last==3)
	raydium_light_disable();
    if(raydium_key_last==4)
	raydium_light_enable();

    
    if(raydium_key_last==5)
        raydium_light_position[0][2]=100;
    if(raydium_key_last==6)
        raydium_light_position[0][2]=200;
    if(raydium_key_last==7)
        raydium_light_position[0][2]=300;
    
    secs+=raydium_frame_time/2;

    raydium_clear_frame();
    
//    raydium_camera_place(cam_pos_x,cam_pos_y,cam_pos_z,cam_angle_x,cam_angle_y,0);
    raydium_camera_freemove(RAYDIUM_CAMERA_FREEMOVE_NORMAL);
//    raydium_camera_smooth_path_to_path(CAM_FILE,secs,CAM_FILE,secs+1,raydium_frame_time*2);
    raydium_shader_var_4f_name("water","cameraPos",raydium_camera_x,raydium_camera_y,raydium_camera_z,1.0f);
    raydium_shader_var_4f_name("water","lightPos",raydium_light_position[0][0],raydium_light_position[0][1],raydium_light_position[0][2],1.0f);

/////////////////////////////////////////////
// create reflection
glViewport(0,0, water_textures_size, water_textures_size);
glScalef(1.0, 1.0, -1.0);
raydium_sky_box_render(raydium_camera_x,raydium_camera_y,raydium_camera_z);// glClear ok ...
raydium_camera_replace(); // cancels the glScalef ...
glTranslatef(0.0f, 0.0f, water_height*2.0f);
glScalef(1.0, 1.0, -1.0);
glPushMatrix();
{
float a,b,c;

a=raydium_light_position[0][0];
b=raydium_light_position[0][1];
c=raydium_light_position[0][2];
raydium_light_position[0][0]*=-1;
raydium_light_position[0][1]*=-1;
raydium_light_update_position_all();

glEnable(GL_CLIP_PLANE0);
glClipPlane(GL_CLIP_PLANE0, plane_reflection);
raydium_object_draw_name(model);
raydium_light_position[0][0]=a;
raydium_light_position[0][1]=b;
raydium_light_position[0][2]=c;
}
raydium_particle_draw_all();
glDisable(GL_CLIP_PLANE0);
glPopMatrix();
glBindTexture(GL_TEXTURE_2D,raydium_texture_find_by_name("reflection"));
glCopyTexSubImage2D(GL_TEXTURE_2D,0, 0,0, 0,0, water_textures_size, water_textures_size);
raydium_rendering_internal_restore_render_state();


/////////////////////////////////////////////
// create refraction
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
glViewport(0,0, water_textures_size, water_textures_size);
raydium_camera_replace();
raydium_light_update_position_all();

{
float zero[]={0,0,0,0};
glEnable(GL_FOG);
glFogf(GL_FOG_START,0.1);
glFogf(GL_FOG_END,2);
glFogi(GL_FOG_MODE,GL_LINEAR);
glFogfv(GL_FOG_COLOR,zero);
raydium_fog_volumetric_enable();
}

raydium_object_draw_name(model);
glBindTexture(GL_TEXTURE_2D,raydium_texture_find_by_name("refraction"));
glCopyTexSubImage2D(GL_TEXTURE_2D,0, 0,0, 0,0, water_textures_size, water_textures_size);
raydium_fog_volumetric_disable();
raydium_fog_apply();
raydium_rendering_internal_restore_render_state();


/////////////////////////////////////////////
// regular rendering
glViewport(0, 0, raydium_window_tx, raydium_window_ty);
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
raydium_sky_box_render(raydium_camera_x,raydium_camera_y,raydium_camera_z);
raydium_camera_replace();
raydium_object_draw_name(model);
raydium_rendering_internal_restore_render_state();


/////////////////////////////////////////////
// rendering water
// prepare texunits
raydium_rendering_prepare_texture_unit(GL_TEXTURE0_ARB,raydium_texture_find_by_name("reflection"));
raydium_rendering_prepare_texture_unit(GL_TEXTURE1_ARB,raydium_texture_find_by_name("refraction"));
raydium_rendering_prepare_texture_unit(GL_TEXTURE2_ARB,raydium_texture_find_by_name("water_nmap_0.tga"));
raydium_rendering_prepare_texture_unit(GL_TEXTURE3_ARB,raydium_texture_find_by_name("water_dudvmap_0.tga"));
// activate shader
raydium_shader_current_name("water");
// draw quad
{
// Create a static variable for the movement of the water
static float move = 0.0f;

// Use this variable for the normal map and make it slower
// than the refraction map's speed.  We want the refraction
// map to be jittery, but not the normal map's waviness.
float move2 = move * kNormalMapScale;

// Set the refraction map's UV coordinates to our global g_WaterUV
float refrUV = g_WaterUV;

// Set our normal map's UV scale and shrink it by kNormalMapScale
float normalUV = g_WaterUV * kNormalMapScale;

float low=-100;
float high=100;

// Move the water by our global speed
//printf("%f\n",raydium_frame_time);
move += (g_WaterFlow*raydium_frame_time);
        
glBegin(GL_QUADS);

// The back left vertice for the water
 glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0.0f, g_WaterUV);				// Reflection texture				
 glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0.0f, refrUV - move);			// Refraction texture
 glMultiTexCoord2fARB(GL_TEXTURE2_ARB, 0.0f, normalUV + move2);		// Normal map texture
 glMultiTexCoord2fARB(GL_TEXTURE3_ARB, 0, 0);						// DUDV map texture
 glMultiTexCoord2fARB(GL_TEXTURE4_ARB, 0, 0);						// Depth texture
 glVertex3f(low, low, water_height);

// The front left vertice for the water
 glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0.0f, 0.0f);					// Reflection texture
 glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0.0f, 0.0f - move);			// Refraction texture
 glMultiTexCoord2fARB(GL_TEXTURE2_ARB, 0.0f, 0.0f + move2);			// Normal map texture
 glMultiTexCoord2fARB(GL_TEXTURE3_ARB, 0, 0);						// DUDV map texture
 glMultiTexCoord2fARB(GL_TEXTURE4_ARB, 0, 0);						// Depth texture
 glVertex3f(low, high, water_height);

// The front right vertice for the water
 glMultiTexCoord2fARB(GL_TEXTURE0_ARB, g_WaterUV, 0.0f);				// Reflection texture
 glMultiTexCoord2fARB(GL_TEXTURE1_ARB, refrUV, 0.0f - move);			// Refraction texture
 glMultiTexCoord2fARB(GL_TEXTURE2_ARB, normalUV, 0.0f + move2);		// Normal map texture
 glMultiTexCoord2fARB(GL_TEXTURE3_ARB, 0, 0);						// DUDV map texture
 glMultiTexCoord2fARB(GL_TEXTURE4_ARB, 0, 0);						// Depth texture
 glVertex3f(high, high, water_height);

// The back right vertice for the water
 glMultiTexCoord2fARB(GL_TEXTURE0_ARB, g_WaterUV, g_WaterUV);		// Reflection texture
 glMultiTexCoord2fARB(GL_TEXTURE1_ARB, refrUV, refrUV - move);		// Refraction texture
 glMultiTexCoord2fARB(GL_TEXTURE2_ARB, normalUV, normalUV + move2);	// Normal map texture
 glMultiTexCoord2fARB(GL_TEXTURE3_ARB, 0, 0);						// DUDV map texture
 glMultiTexCoord2fARB(GL_TEXTURE4_ARB, 0, 0);						// Depth texture
 glVertex3f(high, low, water_height);

glEnd();
}

raydium_shader_current(-1);

raydium_rendering_prepare_texture_unit(GL_TEXTURE0_ARB,0);
raydium_rendering_prepare_texture_unit(GL_TEXTURE1_ARB,0);
raydium_rendering_prepare_texture_unit(GL_TEXTURE2_ARB,0);
raydium_rendering_prepare_texture_unit(GL_TEXTURE3_ARB,0);

if(raydium_key[GLUT_KEY_F12])
    {
    raydium_osd_draw_name("reflection",0,0,20,20);
    raydium_osd_draw_name("refraction",80,0,100,20);
    }


raydium_rendering_finish();
}




int main(int argc, char **argv)
{
    int tmp;
    raydium_init_args(argc,argv);
    
    // window creation
    raydium_window_create(800,600,RAYDIUM_RENDERING_WINDOW,"willou.c based water shader effect");
    raydium_texture_filter_change(RAYDIUM_TEXTURE_FILTER_TRILINEAR);
    raydium_projection_near=0.01;
    raydium_projection_far=2500;
    raydium_projection_fov=60;
    raydium_fog_disable();
    raydium_fog_volumetric_support();
    raydium_window_view_update();
    
    raydium_light_enable();
    raydium_light_on(0);
    memcpy(raydium_light_color[0],light_color,raydium_internal_size_vector_float_4);
    raydium_light_intensity[0] = 10000000;
    raydium_light_position[0][0]=350;
    raydium_light_position[0][1]=10;
    raydium_light_position[0][2]=200;
    raydium_light_position[0][3]=1.0;
    raydium_light_update_all(0);
    
    raydium_sky_box_cache();

    raydium_window_view_update();
    raydium_background_color_change(back_color[0],back_color[1],back_color[2],back_color[3]);

    tmp=1;
    while(tmp<=raydium_window_tx && 
           tmp<=raydium_window_ty && 
           tmp<=raydium_texture_size_max)
                {
                tmp*=2;
                }

    water_textures_size=tmp/2;
    raydium_texture_load_internal("","reflection",1,water_textures_size,water_textures_size,4,-1);
    raydium_texture_load_internal("","refraction",1,water_textures_size,water_textures_size,4,-1);
//    raydium_shader_load("water","water.vert","water_brico.frag");
    raydium_shader_load("water","water.vert","water.frag");
    raydium_shader_var_i_name("water","reflection",0);
    raydium_shader_var_i_name("water","refraction",1);
    raydium_shader_var_i_name("water","normalMap",2);
    raydium_shader_var_i_name("water","dudvMap",3);
    raydium_shader_var_4f_name("water","waterColor", water_color[0], water_color[1], water_color[2], water_color[3]);
    strcpy(model,"cocorobix.tri");
    raydium_init_cli_option("model",model);

    raydium_callback(&display);
    return(0);
}
// EOF
