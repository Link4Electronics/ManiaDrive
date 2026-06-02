/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

// ** Volcano **
// Small Raydium demo, mainly featuring shaders (water) and particles. Enjoy !

#include "raydium/index.c"

#define CAM_FILE        "volcano.cam"
#define CAM_FILE2       "volcano1.cam"

GLint water_textures_size;
GLfloat water_height=0;
float water_color[4]={0.5f, 0.6f, 0.8f, 1.0f};

const float kNormalMapScale = 0.25f;
float g_WaterFlow = 0.03f;
float g_WaterUV = 12.0f;

char model[RAYDIUM_MAX_NAME_LEN];

GLfloat light_color[] = {1.0, 0.9, 0.8, 1.0};
GLfloat  *back_color=light_color;

float volcano_center[]= {53.2,47,45};
char *rocks[]={"rock0.tri","rock1.tri","rock2.tri"};

#define ROCK_TAG        10
#define WATER_TAG       11

int sound_water,sound_rock;
float needed_water_volume;

char collide(int e1, int e2, dContact *n)
{
int t1,t2;
int rock,i;
dReal *vel=NULL;
float speed=0;


t1=raydium_ode_element_tag_get(e1);
t2=raydium_ode_element_tag_get(e2);

if( (t1==RAYDIUM_ODE_TAG_GROUND && t2==WATER_TAG) ||
    (t2==RAYDIUM_ODE_TAG_GROUND && t1==WATER_TAG) )
        return 0;


rock=-1;
if(t1==ROCK_TAG)
    rock=e1;
if(t2==ROCK_TAG)
    rock=e2;

if(rock>=0)
    {
    vel=raydium_ode_element_linearvelocity_get(rock);
    speed=0;
    for(i=0;i<3;i++)
        speed+=raydium_math_abs(vel[i]);
    }


if(t1==ROCK_TAG || t2==ROCK_TAG)
    {
    char name[255];

    if(t1==WATER_TAG || t2==WATER_TAG) // rock vs. water
        {
        int i;
        dReal op[3];

        sprintf(name,"water-%i-%i",e1,e2);
        if(raydium_particle_generator_find(name)>=0)
            return 0;

        //printf("%f\n",speed);
        if(speed>4)
            {
            raydium_particle_generator_load("volcano_e_water.prt",name);
            raydium_particle_generator_move_name(name,n->geom.pos);
            }


        for(i=0;i<3;i++)
            op[i]=vel[i]*-1;
        raydium_ode_element_addforce(rock,op);

        needed_water_volume+=speed/100;
/*      if(!raydium_sound_IsPlaying(sound_water) && speed>5)
            {
            raydium_sound_SourcePlay(sound_water);
            raydium_sound_SetSourcePos(sound_water,n->geom.pos);
            }
*/
        return 0;
        }
    else // ground vs. rock
        {
        sprintf(name,"ground-%i-%i",e1,e2);
        if(raydium_particle_generator_find(name)>=0)
            return 1;
        if(n->geom.pos[2]<0)
            return 1;
        raydium_particle_generator_load("volcano_e_collide.prt",name);
        raydium_particle_generator_move_name(name,n->geom.pos);
        if(!raydium_sound_IsPlaying(sound_rock) && speed>5)
            {
            raydium_sound_SourcePlay(sound_rock);
            raydium_sound_SetSourcePos(sound_rock,n->geom.pos);
            }
        }
    }

return 1;
}

void display(void)
{
    double plane_reflection[4] = {0.0, 0.0, 1.0, -water_height};
    static GLfloat secs=0;
    static GLfloat countdown=0;

    if(raydium_key_last==1027)
        exit(0);

//    if(raydium_key_last==1032)
if(countdown<=0 && raydium_random_i(0,15)==0)
        {
        int a,r;
        char name[255];
        float x,y,z;

        a=raydium_ode_object_find("GLOBAL");
        raydium_ode_name_auto("rock",name);
        r=raydium_random_i(0,2);
        raydium_ode_object_sphere_add(name,a,1,RAYDIUM_ODE_AUTODETECT,RAYDIUM_ODE_STANDARD,ROCK_TAG,rocks[r]);
        raydium_ode_element_move_name(name,volcano_center);

        x=-raydium_random_pos_1()*500;
        y=-raydium_random_pos_1()*500;
        raydium_ode_element_addforce_name_3f(name,x,y,1000);

        x=raydium_random_neg_pos_1()*100;
        y=raydium_random_neg_pos_1()*100;
        z=raydium_random_neg_pos_1()*100;
        raydium_ode_element_addtorque_name_3f(name,x,y,z);

        raydium_ode_element_particle_name(name,"volcano_e_fire.prt");
        raydium_ode_element_ttl_set_name(name,raydium_ode_get_physics_freq()*25);

        raydium_camera_rumble(0.3,-0.1,3);
        countdown=0.3;
        }

    secs+=raydium_frame_time/2;
    countdown-=raydium_frame_time;
    needed_water_volume=0;

    if(raydium_key_last==5)
        raydium_light_position[0][2]=100;
    if(raydium_key_last==6)
        raydium_light_position[0][2]=200;
    if(raydium_key_last==7)
        raydium_light_position[0][2]=300;


    raydium_clear_frame();

//    raydium_camera_place(cam_pos_x,cam_pos_y,cam_pos_z,cam_angle_x,cam_angle_y,0);
    raydium_camera_smooth_path_to_path(CAM_FILE,secs,CAM_FILE2,secs,raydium_frame_time*2);
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
raydium_ode_draw_all(RAYDIUM_ODE_DRAW_NORMAL_NO_POST);
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

//glEnable(GL_CLIP_PLANE0);
//glClipPlane(GL_CLIP_PLANE0, plane_refraction);
raydium_ode_draw_all(RAYDIUM_ODE_DRAW_NORMAL_NO_POST);
//glDisable(GL_CLIP_PLANE0);
glBindTexture(GL_TEXTURE_2D,raydium_texture_find_by_name("refraction"));
glCopyTexSubImage2D(GL_TEXTURE_2D,0, 0,0, 0,0, water_textures_size, water_textures_size);
raydium_fog_volumetric_disable();
raydium_fog_apply();
raydium_rendering_internal_restore_render_state();



/////////////////////////////////////////////
// regular rendering
glViewport(0, 0, raydium_window_tx, raydium_window_ty);
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
raydium_camera_replace();
raydium_sky_box_render(raydium_camera_x,raydium_camera_y,raydium_camera_z);
raydium_ode_draw_all(RAYDIUM_ODE_DRAW_NORMAL_NO_POST);
//raydium_ode_draw_all(RAYDIUM_ODE_DRAW_DEBUG);
raydium_rendering_internal_restore_render_state();


/////////////////////////////////////////////
// rendering water
raydium_camera_replace();
// prepare texunits
raydium_rendering_prepare_texture_unit(GL_TEXTURE0_ARB,raydium_texture_find_by_name("reflection"));
raydium_rendering_prepare_texture_unit(GL_TEXTURE1_ARB,raydium_texture_find_by_name("refraction"));
raydium_rendering_prepare_texture_unit(GL_TEXTURE2_ARB,raydium_texture_find_by_name("water_nmap.tga"));
raydium_rendering_prepare_texture_unit(GL_TEXTURE3_ARB,raydium_texture_find_by_name("water_dudvmap.tga"));
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
 glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0.0f, g_WaterUV);                                // Reflection texture
 glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0.0f, refrUV - move);                    // Refraction texture
 glMultiTexCoord2fARB(GL_TEXTURE2_ARB, 0.0f, normalUV + move2);         // Normal map texture
 glMultiTexCoord2fARB(GL_TEXTURE3_ARB, 0, 0);                                           // DUDV map texture
 glMultiTexCoord2fARB(GL_TEXTURE4_ARB, 0, 0);                                           // Depth texture
 glVertex3f(low, low, water_height);

// The front left vertice for the water
 glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0.0f, 0.0f);                                     // Reflection texture
 glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0.0f, 0.0f - move);                      // Refraction texture
 glMultiTexCoord2fARB(GL_TEXTURE2_ARB, 0.0f, 0.0f + move2);                     // Normal map texture
 glMultiTexCoord2fARB(GL_TEXTURE3_ARB, 0, 0);                                           // DUDV map texture
 glMultiTexCoord2fARB(GL_TEXTURE4_ARB, 0, 0);                                           // Depth texture
 glVertex3f(low, high, water_height);

// The front right vertice for the water
 glMultiTexCoord2fARB(GL_TEXTURE0_ARB, g_WaterUV, 0.0f);                                // Reflection texture
 glMultiTexCoord2fARB(GL_TEXTURE1_ARB, refrUV, 0.0f - move);                    // Refraction texture
 glMultiTexCoord2fARB(GL_TEXTURE2_ARB, normalUV, 0.0f + move2);         // Normal map texture
 glMultiTexCoord2fARB(GL_TEXTURE3_ARB, 0, 0);                                           // DUDV map texture
 glMultiTexCoord2fARB(GL_TEXTURE4_ARB, 0, 0);                                           // Depth texture
 glVertex3f(high, high, water_height);

// The back right vertice for the water
 glMultiTexCoord2fARB(GL_TEXTURE0_ARB, g_WaterUV, g_WaterUV);           // Reflection texture
 glMultiTexCoord2fARB(GL_TEXTURE1_ARB, refrUV, refrUV - move);          // Refraction texture
 glMultiTexCoord2fARB(GL_TEXTURE2_ARB, normalUV, normalUV + move2);     // Normal map texture
 glMultiTexCoord2fARB(GL_TEXTURE3_ARB, 0, 0);                                           // DUDV map texture
 glMultiTexCoord2fARB(GL_TEXTURE4_ARB, 0, 0);                                           // Depth texture
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
    raydium_osd_draw_name("reflection",0,0,40,40);
    raydium_osd_draw_name("refraction",80,0,100,20);
    }

raydium_ode_draw_all_post();
raydium_osd_draw_name("logo_raydium.tga",85,5,95,20);
raydium_rendering_finish();

if(needed_water_volume>1)
    needed_water_volume=1;
#ifdef WIN32
raydium_sound_SetSourceGain(sound_water,needed_water_volume*5);
#else
raydium_sound_SetSourceGain(sound_water,needed_water_volume);
#endif
}




int main(int argc, char **argv)
{
    int tmp;
    raydium_init_args(argc,argv);

    // window creation
    raydium_window_create(800,600,RAYDIUM_RENDERING_WINDOW,"Volcano - Shaders and Particles");
    raydium_texture_filter_change(RAYDIUM_TEXTURE_FILTER_TRILINEAR);
    raydium_projection_near=0.1;
    raydium_projection_far=600;
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
    raydium_shadow_enable();
    raydium_shadow_mode(RAYDIUM_SHADOW_MODE_FULLSCENE);

    raydium_window_view_update();
    raydium_background_color_change(back_color[0],back_color[1],back_color[2],back_color[3]);

    raydium_ode_CollideCallback=collide;

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
    raydium_hdr_texture_name("reflection",1);
    raydium_shader_load("water","water.vert","water.frag");
    raydium_shader_var_i_name("water","reflection",0);
    raydium_shader_var_i_name("water","refraction",1);
    raydium_shader_var_i_name("water","normalMap",2);
    raydium_shader_var_i_name("water","dudvMap",3);
    raydium_shader_var_4f_name("water","waterColor", water_color[0], water_color[1], water_color[2], water_color[3]);
    strcpy(model,"paradise.tri");
    raydium_init_cli_option("model",model);
    raydium_ode_ground_set_name(model);
    raydium_particle_generator_load("volcano_dust.prt","dust");
    raydium_particle_generator_move_name("dust",volcano_center);
    raydium_ode_object_box_add("water",raydium_ode_object_find("GLOBAL"),0,200,200,20,RAYDIUM_ODE_STATIC,WATER_TAG,"");
    raydium_ode_element_move_name_3f("water",0,0,-10);
#ifdef WIN32
    raydium_sound_DefaultReferenceDistance=50.f;
#else
    raydium_sound_DefaultReferenceDistance=10.f;
#endif
    raydium_sound_load_music("volcano.ogg");
    sound_water=raydium_sound_LoadWav("splash.wav");
    sound_rock=raydium_sound_LoadWav("rockbounce1.wav");
    raydium_sound_SourcePlay(sound_water);
    raydium_sound_SetSourceGain(sound_water,0);
    raydium_sound_SetSourceLoop(sound_rock,0);


    raydium_callback(&display);
    return(0);
}
// EOF
