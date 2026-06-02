/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

#ifndef DONT_INCLUDE_HEADERS
#include "index.h"
#else
#include "headers/init.h"

#endif

// proto
void raydium_ode_init(void);
void raydium_register_api(void);
void raydium_live_init(void);
void raydium_fog_init(void);
void raydium_sky_init(void);
void raydium_video_init(void);
void raydium_internal_live_close(void);
void raydium_shadow_init(void);
void raydium_shadow_object_dl_init(void);
void raydium_shadow_object_dl_delete(void);
void raydium_hdr_init(void);
void raydium_hdr_texture_reset(void);
void raydium_lensflare_init(void);
void raydium_shader_init(void);
void raydium_shader_delete_all(void);
void raydium_web_init(void);
void raydium_path_package_update(void);
void raydium_path_package_register_all_cli(void);
void raydium_camera_init(void);
void raydium_object_render_cache_free_all(void);
void raydium_object_dl_init(void);
void raydium_object_dl_delete(void);
void raydium_ode_delete_all(void);
void raydium_particle_free_all(void);

char *raydium_version(void)
{
static char version[RAYDIUM_MAX_NAME_LEN];
static signed char first=1;

if(first)
    {
    first=0;
    sprintf(version,"%i.%03i",RAYDIUM_MAJOR,RAYDIUM_MINOR);
    }

return version;
}

void raydium_init_lights(void)
{
GLuint i;

for(i=0;i<RAYDIUM_MAX_LIGHTS;i++)
raydium_light_reset(i);
raydium_log("lights: OK");
}

void raydium_init_objects(void)
{
GLuint i;

raydium_object_anim_time_factor=1.f;

for(i=0;i<RAYDIUM_MAX_OBJECTS;i++)
raydium_object_reset(i);
raydium_log("objects: OK");
}


void raydium_init_key(void)
{
if(raydium_window_mode==RAYDIUM_RENDERING_NONE)
    return;
glutIgnoreKeyRepeat(1);
memset(raydium_key,0,RAYDIUM_KEYBOARD_SIZE);
raydium_key_last=0;
raydium_key_trace=0;
raydium_log("keyboard: OK");
}

// Experimental code.
void raydium_init_purgemem(void)
{
int i;

raydium_log("** Start clearing ... **");

// *** heavy things:
// vertices
raydium_vertex_index=0;
raydium_vertex_offset_triangle=0;

// objects, Display Lists, anims
raydium_object_render_cache_free_all();
raydium_object_dl_delete();
raydium_init_objects(); // will delete anim flag, too
raydium_object_index=0;

// textures
for(i=0;i<RAYDIUM_MAX_TEXTURES;i++)
    raydium_texture_free(i);

// ODE
#ifdef ODE_SUPPORT
raydium_ode_delete_all();
#endif
// sound ?

// *** linked things:
// live
raydium_internal_live_close();
raydium_live_init();

// lensflare
raydium_lensflare_init();

// shaders
raydium_shader_delete_all();

// particles2
raydium_particle_free_all();

// shadows
raydium_shadow_object_dl_delete();
raydium_shadow_init();

// OSD
raydium_osd_cursor_texture=0;
for(i=0;i<4;i++)
    raydium_osd_color[i]=1.f;

// GUI
for (i=0;i<RAYDIUM_GUI_MAX_WINDOWS;i++)
    if(raydium_gui_windows[i].state)
        raydium_gui_window_delete(i);
raydium_gui_init();

// *** file things :)
// paths + packages
raydium_path_init();
raydium_path_package_register_all_cli();

// file "fopen cache"
raydium_file_log_fopen_index=0;


// *** and perhaps others ? (probably not !)
// timecalls ?
// camera / camera paths ?
// viewport
raydium_viewport_init();
// regapi / php ?
// network ?
// small things like fog/lights/...

raydium_log("** Engine cleared **");
}

// NEVER tested as it should be ! (used once only for now)
void raydium_init_reset(void)
{
GLuint i;

/*
for(i=1;i<raydium_texture_index;i++) // free all texture buffers
free(raydium_texture_ptr[i]);
*/

raydium_init_lights();
raydium_fog_init();
raydium_sky_init();
raydium_init_objects();
raydium_object_dl_init();
raydium_network_init();
raydium_timecall_init();
raydium_particle_init();
raydium_camera_init();
raydium_camera_path_init_all();
raydium_osd_fade_init();
raydium_console_init();
raydium_gui_init();
raydium_live_init();
raydium_video_init();
raydium_shadow_init();
raydium_shadow_object_dl_init();
raydium_hdr_init();
raydium_lensflare_init();
raydium_shader_init();
raydium_web_init();

//reset all the textures
for(i=0;i<RAYDIUM_MAX_TEXTURES;i++) // reset all textures
{
    raydium_texture_free(i);
}

raydium_hdr_texture_reset();

raydium_vertex_index=0;
raydium_vertex_offset_triangle=0;

strcpy(raydium_texture_name[0],"dummy.null");

i=0;
raydium_osd_ega[i++]=0.0f;raydium_osd_ega[i++]= 0.0f;raydium_osd_ega[i++]= 0.0f; // 0: black
raydium_osd_ega[i++]=0.0f;raydium_osd_ega[i++]= 0.0f;raydium_osd_ega[i++]= 0.6f; // 1: blue
raydium_osd_ega[i++]=0.0f;raydium_osd_ega[i++]= 0.6f;raydium_osd_ega[i++]= 0.0f; // 2: green
raydium_osd_ega[i++]=0.0f;raydium_osd_ega[i++]= 0.6f;raydium_osd_ega[i++]= 0.6f; // 3: cyan
raydium_osd_ega[i++]=0.6f;raydium_osd_ega[i++]= 0.0f;raydium_osd_ega[i++]= 0.0f; // 4: red
raydium_osd_ega[i++]=0.6f;raydium_osd_ega[i++]= 0.0f;raydium_osd_ega[i++]= 0.6f; // 5: purple
raydium_osd_ega[i++]=0.6f;raydium_osd_ega[i++]= 0.3f;raydium_osd_ega[i++]= 0.0f; // 6: brown
raydium_osd_ega[i++]=0.6f;raydium_osd_ega[i++]= 0.6f;raydium_osd_ega[i++]= 0.6f; // 7: white
raydium_osd_ega[i++]=0.3f;raydium_osd_ega[i++]= 0.3f;raydium_osd_ega[i++]= 0.3f; // 8: grey
raydium_osd_ega[i++]=0.3f;raydium_osd_ega[i++]= 0.3f;raydium_osd_ega[i++]= 1.0f; // 9: light blue
raydium_osd_ega[i++]=0.3f;raydium_osd_ega[i++]= 1.0f;raydium_osd_ega[i++]= 0.3f; // A: light green
raydium_osd_ega[i++]=0.3f;raydium_osd_ega[i++]= 1.0f;raydium_osd_ega[i++]= 1.0f; // B: light cyan
raydium_osd_ega[i++]=1.0f;raydium_osd_ega[i++]= 0.3f;raydium_osd_ega[i++]= 0.3f; // C: light red
raydium_osd_ega[i++]=1.0f;raydium_osd_ega[i++]= 0.3f;raydium_osd_ega[i++]= 1.0f; // D: light purple
raydium_osd_ega[i++]=1.0f;raydium_osd_ega[i++]= 1.0f;raydium_osd_ega[i++]= 0.3f; // E: light yellow
raydium_osd_ega[i++]=1.0f;raydium_osd_ega[i++]= 1.0f;raydium_osd_ega[i++]= 1.0f;  // F: light white

//raydium_texture_index=1; // no more texture loaded (0 is not a texture)
raydium_texture_current_main=0; // sets an "invalid" current texture
raydium_texture_current_multi=0; // sets an "invalid" current texture
raydium_texture_current_multi_u=0;
raydium_texture_current_multi_v=0;
for(i=0;i<(RAYDIUM_RENDER_MAX_TEXUNITS-1);i++)
        raydium_texture_current_env[i]=0; // sets an "invalid" current texture
raydium_texture_to_replace=0; // No texture to erase.. just load it :)
raydium_texture_used_memory=0;
//raydium_texture_filter=RAYDIUM_TEXTURE_FILTER_NONE;
raydium_texture_filter_change(RAYDIUM_TEXTURE_FILTER_NONE);
if(raydium_init_cli_option("npot",NULL))
    raydium_texture_npot_enable();
else
    raydium_texture_npot_disable();
raydium_rendering_rgb_normal();
raydium_rendering_displaylists_enable();
raydium_render_rgb_force_tag=0;
raydium_render_cull_blended=0;
raydium_render_lightmap_color_value[0]=1;
raydium_render_lightmap_color_value[1]=1;
raydium_render_lightmap_color_value[2]=1;
raydium_render_lightmap_color_value[3]=1;
raydium_render_internal_light_previous_step=-1;
raydium_render_max_fps=0;
raydium_vertex_counter=0;
raydium_projection=RAYDIUM_PROJECTION_PERSPECTIVE;
raydium_projection_fov=60;
raydium_projection_near=1;
raydium_projection_far=1000;
raydium_projection_left=0;
raydium_projection_right=0;
raydium_projection_bottom=0;
raydium_projection_top=0;
raydium_window_view_update();
raydium_internal_vertex_next_extras=0;
raydium_sky_force=0;
raydium_osd_logo_angle=0;
raydium_osd_cursor_texture=0;
raydium_osd_cursor_visible=0;
raydium_osd_cursor_offset(0,0);
raydium_register_variable_index=0;
raydium_register_function_index=0;
raydium_frame_time=0;
raydium_capture_asked=RAYDIUM_CAPTURE_NONE;
raydium_sky_prefix_defined=0;
raydium_sky_enabled=1;
raydium_file_log_fopen_index=0;
raydium_log_atexit_dump=1;
raydium_compat_flags=0;

#ifndef IPHONEOS
raydium_ode_timestep = (0.006f);
raydium_ode_physics_freq = 400;
#else
// way less CPU hungry ... but breaks a lot of things (ERP/CFM/...) :/
raydium_ode_timestep = (0.06f);
raydium_ode_physics_freq = 40;
#endif

for(i=0;i<4;i++)
raydium_osd_color[i]=1.f;

raydium_background_color_change(1,1,1,1);

glEnable(GL_TEXTURE_2D);
glEnable(GL_DEPTH_TEST);
glEnable(GL_COLOR_MATERIAL);
glShadeModel(GL_SMOOTH);
glDepthFunc(GL_LEQUAL); // LESS only ? shadow maps loves lequal, so ..
glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);


raydium_light_enable();
glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);  // mmmmm...
glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
raydium_log("Raydium engine reseted to original state");
}



void raydium_init_engine(void)
{
GLenum err;
int i;
GLfloat aniso_hard;
GLfloat aniso_user;
#ifdef PHP_SUPPORT
char autoexec[RAYDIUM_MAX_NAME_LEN];
#endif
char str[RAYDIUM_MAX_NAME_LEN];

raydium_signal_install_trap();
err=glewInit();
if(err==GLEW_OK)
    raydium_log("OpenGL extensions: OK");
else
    raydium_log("OpenGL extensions: FAILED");

#ifdef IPHONEOS
if(!glActiveTexture || !glClientActiveTexture || !glActiveTextureARB || !glClientActiveTextureARB)
    {
    raydium_log("ERROR: glActiveTexture(ARB) glClientActiveTexture(ARB) not available !");
    raydium_log("GLEW probably overwrite default existing functions, please update glew.h accordingly until we found a better solution ...");
    exit(200);
    }
#endif

raydium_internal_size_vector_float_4=sizeof(GLfloat)*4;
raydium_log("Platform \"4xfloat\" vector size is: %i byte(s) long",raydium_internal_size_vector_float_4);
glGetIntegerv(GL_MAX_TEXTURE_SIZE, &raydium_texture_size_max);
raydium_log("OpenGL implementation maximum texture size: %ix%i",raydium_texture_size_max,raydium_texture_size_max);
glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &raydium_texture_units);
raydium_log("OpenGL hardware providing %i fixed texture unit(s), Raydium deals with %i",raydium_texture_units,RAYDIUM_RENDER_MAX_TEXUNITS);
glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso_hard);
if(raydium_init_cli_option("max-aniso",str))
    aniso_user=atof(str);
else
    aniso_user=999;
raydium_texture_filter_aniso_levels=raydium_math_min(aniso_user,aniso_hard);
raydium_log("OpenGL anisotropy max level is %.2f",raydium_texture_filter_aniso_levels);

if(raydium_init_cli_option("compress",NULL))
    raydium_texture_compression_enabled=1;
else
    raydium_texture_compression_enabled=0;

raydium_texture_compression_available=glutExtensionSupported("GL_ARB_texture_compression");
if(raydium_texture_compression_available)
    {
    glHint(GL_TEXTURE_COMPRESSION_HINT_ARB, GL_NICEST);
    raydium_log("OpenGL texture compression available");
    }
else
    raydium_texture_compression_enabled=0;

raydium_vertex_arr=malloc(RAYDIUM_MAX_VERTICES*sizeof(GLfloat)*3);
raydium_vertex_normal_arr=malloc(RAYDIUM_MAX_VERTICES*sizeof(GLfloat)*3);
raydium_vertex_tangent_arr=malloc(RAYDIUM_MAX_VERTICES*sizeof(GLfloat)*3);
raydium_vertex_normal_visu_arr=malloc(RAYDIUM_MAX_VERTICES*sizeof(GLfloat)*3);
raydium_vertex_texture_uv_arr=malloc(RAYDIUM_MAX_VERTICES*sizeof(GLfloat)*2);
raydium_vertex_texture=malloc(RAYDIUM_MAX_VERTICES*sizeof(GLuint));
raydium_vertex_texture_multi=malloc(RAYDIUM_MAX_VERTICES*sizeof(GLuint));
raydium_vertex_texture_multi_uv_arr=malloc(RAYDIUM_MAX_VERTICES*sizeof(GLfloat)*2);

for(i=0;i<(RAYDIUM_RENDER_MAX_TEXUNITS-1);i++)
        raydium_vertex_texture_env[i]=malloc(RAYDIUM_MAX_VERTICES*sizeof(GLuint));
raydium_vertex_tag=malloc(RAYDIUM_MAX_VERTICES);
// must test more than just the last "big" malloc result..
if(!raydium_vertex_texture) { raydium_log("Out of memory..."); exit(29); }
raydium_log("vertex arrays memory: OK");
for(i=0;i<RAYDIUM_MAX_TEXTURES;i++) // reset all textures
    raydium_texture_memory[i]=0;
raydium_path_init(); // do this ASAP, before any file is opened
raydium_random_randomize();
raydium_init_key();
raydium_mouse_init();
raydium_joy_init(); // and not init_joy, since defined in joy.c, not init.c
raydium_sound=0;
raydium_sound_init();
raydium_viewport_init();
raydium_callback_set();
#ifdef PHP_SUPPORT
raydium_php_init();
#endif

raydium_atexit(raydium_path_dump);
raydium_atexit(raydium_path_package_update); // As atexit are called in inverse order must be called before disabling php
raydium_atexit(raydium_sound_close);
raydium_atexit(raydium_joy_close);
raydium_atexit(raydium_network_close);
raydium_atexit(raydium_internal_dump);
raydium_atexit(raydium_console_history_save);
raydium_atexit(raydium_internal_live_close);
raydium_log("atexit functions: OK");
raydium_init_reset();
#ifdef ODE_SUPPORT
raydium_ode_init();
#endif
raydium_register_api();
raydium_log("Engine is now ready.\n\t -----------------------------------------------------------");

#ifdef PHP_SUPPORT
if(raydium_init_cli_option("autoexec",autoexec))
    raydium_php_exec(autoexec);
#endif
raydium_path_package_register_all_cli();
}

//Still needs a check of void parameters.
//Ie. it should returns error when certain parameters are not given in the conf file.
int raydium_init_load(char *filename)
{
    FILE *fp;
    int ret;
    char var[255],val_s[255];
    float val_f[255];
    int size;
    int tmp_width=0,tmp_height=0,tmp_windowtype=0,tmp_filter=0,tmp_fog=0,tmp_lighting=0,tmp_hdr=0,tmp_sky=0;
    float tmp_light[8],tmp_background[4],tmp_fov=0,tmp_far=0,tmp_near=0;
    char tmp_title[255];

    //flags
    int flag_width,flag_height,flag_title,flag_windowtype,flag_filter,flag_fov,flag_near,flag_far,flag_fog,flag_lighting,flag_light,flag_background,flag_paths,flag_hdr,flag_sky;

     //initializing flags
     flag_width=flag_height=flag_title=flag_windowtype=flag_filter=flag_fov=flag_near=flag_far=flag_fog=flag_lighting=flag_light=flag_background=flag_paths=flag_hdr=flag_sky=0;


    // Needed here as accessing file
    raydium_path_init();
     //opening the config file
    fp=raydium_file_fopen(filename,"rtl"); // As php isn't initialised need to work on local

    if(!fp) //conf file not found, creating one
        {
        //we can not load the configuration. ERROR
        raydium_log("ERROR loading configuration file.");
        if (filename!=NULL)
            {
            raydium_file_cache_flush();
            fp=raydium_file_fopen(filename,"w");
            if (fp)
                {
                raydium_log("Generating default configuration file %s",filename);
                fprintf (fp,"%s","\
// Automatically generated, default, config file. You can custom it.\n\
\n\
// Window settings\n\
width=800;\n\
height=600;\n\
\n\
// window or fullscreen\n\
windowtype=\"window\";\n\
title=\"Raydium application 0.1\";\n\
\n\
// Texture filter: \"none\", \"bilinear\", \"trilinear\", \"aniso\"\n\
filter=\"trilinear\";\n\
fov=60;\n\
near=0.001;\n\
far=2500;\n\
\n\
fog=\"off\";\n\
lighting=\"on\";\n\
\n\
// Light 0 parameters (id, pos [3], intensity and color [3: RGB])\n\
light=\"0,50,150,200,1000000,1,0.9,0.7\";\n\
\n\
// Background color (RGBA)\n\
background=\"1,0.9,0.7,1\";\n\
hdr=\"off\";\n\
\n\
// \"default\", \"foldered\" (currently)\n\
paths=\"default\";\n\
\n\
//Sky type: \"box\" or \"none\" (currently)\n\
sky=\"box\";\n\
");
                fclose(fp);
                }
            }
        fp=raydium_file_fopen(filename,"rtl"); // Trying to open new created conf file
        }



    if(fp)
    {
        raydium_log("Reading configuration file...");
        while( (ret=raydium_parser_read(var,val_s,val_f,&size,fp))!=RAYDIUM_PARSER_TYPE_EOF)
        {
            if(strcmp(var,"width")==0)
                {
                    tmp_width=atoi(val_s);
                    flag_width=1;
                }
            if(strcmp(var,"height")==0)
                {
                    tmp_height=atoi(val_s);
                    flag_height=1;
                }
            if(strcmp(var,"windowtype")==0)
            {
                raydium_parser_trim(val_s);
                tmp_windowtype=RAYDIUM_RENDERING_WINDOW;
                if(strcmp(val_s,"fullscreen")==0) tmp_windowtype=RAYDIUM_RENDERING_FULLSCREEN;
                if(strcmp(val_s,"fixed")==0) tmp_windowtype=RAYDIUM_RENDERING_WINDOW_FIXED;
                flag_windowtype=1;
            }
            if(strcmp(var,"title")==0)
            {
                raydium_parser_trim(val_s);
                strcpy(tmp_title,val_s);
                flag_title=1;
            }
            if(strcmp(var,"filter")==0)
            {
                raydium_parser_trim(val_s);
                tmp_filter=2;
                if(strcmp(val_s,"trilinear")==0)tmp_filter=2;
                if(strcmp(val_s,"bilinear")==0)tmp_filter=1;
                if(strcmp(val_s,"none")==0)tmp_filter=0;
                if(strcmp(val_s,"aniso")==0)tmp_filter=3;
                flag_filter=1;
            }
            if(strcmp(var,"fov")==0)
            {
                tmp_fov=atof(val_s);
                flag_fov=1;
            }
            if(strcmp(var,"near")==0)
            {
                raydium_parser_trim(val_s);
                tmp_near=atof(val_s);
                flag_near=1;
            }
            if(strcmp(var,"far")==0)
            {
                raydium_parser_trim(val_s);
                tmp_far=atof(val_s);
                flag_far=1;
            }
            if(strcmp(var,"fog")==0)
            {
                raydium_parser_trim(val_s);
                tmp_fog=((strcmp(val_s,"on")==0) || (strcmp(val_s,"enable")==0))?1:0;
                flag_fog=1;
            }
            if(strcmp(var,"lighting")==0)
            {
                raydium_parser_trim(val_s);
                tmp_lighting=((strcmp(val_s,"on")==0) || (strcmp(val_s,"enable")==0))?1:0;
                flag_lighting=1;
            }
            if(strcmp(var,"light")==0)
            {
                raydium_parser_trim(val_s);
                sscanf(val_s, "%f,%f,%f,%f,%f,%f,%f,%f", &tmp_light[0], &tmp_light[1],&tmp_light[2],&tmp_light[3],&tmp_light[4],&tmp_light[5],&tmp_light[6],&tmp_light[7]);
                flag_light=1;
            }
             if(strcmp(var,"background")==0)
            {
                raydium_parser_trim(val_s);
                sscanf( val_s, "%f,%f,%f,%f", &tmp_background[0], &tmp_background[1],&tmp_background[2],&tmp_background[3]);
                flag_background=1;
            }
            if(strcmp(var,"paths")==0)
            {
                raydium_parser_trim(val_s);
                if(strcmp(val_s,"foldered")==0)
                {
                        flag_paths=1;
                 }
             }
            if(strcmp(var,"hdr")==0)
            {
                raydium_parser_trim(val_s);
                tmp_hdr=((strcmp(val_s,"on")==0) || (strcmp(val_s,"enable")==0))?1:0;
                flag_hdr=1;
            }
            if(strcmp(var,"sky")==0)
            {
                raydium_parser_trim(val_s);
                tmp_sky=0;
                tmp_sky=((strcmp(val_s,"none")==0))?-1:tmp_sky;
                flag_sky=1;
            }
        }

        //Here, we process all the data achieved and make the raydium calls
        if(flag_width && flag_height && flag_windowtype && flag_title)
            raydium_window_create(tmp_width,tmp_height,tmp_windowtype,tmp_title);

        if(flag_filter)
            raydium_texture_filter_change(tmp_filter);
        else
            raydium_texture_filter_change(RAYDIUM_TEXTURE_FILTER_TRILINEAR);

        if (flag_fov && flag_near && flag_far)
            raydium_window_view_perspective(tmp_fov,tmp_near,tmp_far);

        if(flag_fog)
            {
            if(tmp_fog)
                raydium_fog_enable();
            else
                raydium_fog_disable();
            }
        else raydium_fog_disable();

        //light
        if(flag_lighting)
            {
            if(tmp_lighting)
                raydium_light_enable();
            else
                raydium_light_disable();
            }

        if(flag_light && flag_lighting && tmp_lighting)
            {
            raydium_light_on(0);
            raydium_light_conf_7f(tmp_light[0],tmp_light[1],tmp_light[2],tmp_light[3],tmp_light[4],tmp_light[5],tmp_light[6],tmp_light[7]);
            }
        //background
        if(flag_background)
            raydium_background_color_change(tmp_background[0], tmp_background[1],tmp_background[2],tmp_background[3]);

        //This must be placed after paths processing: Textures involved.
        // Allow skybox load from local foldered path

        if(flag_paths)
            {
            if(flag_paths==1)
                {
                //TODO Allow to specify base folder.
                raydium_path_ext_priority("./data/textures/","tga",7);
                raydium_path_ext_priority("./data/fonts/","tga",7);
                raydium_path_ext_priority("./data/shaders/","vert",7);
                raydium_path_ext_priority("./data/shaders/","frag",7);
                raydium_path_ext_priority("./data/meshes/","tri",7);
                raydium_path_ext_priority("./data/themes/","gui",7);
                raydium_path_ext_priority("./data/particles/","prt",7);
                raydium_path_ext_priority("./data/lensflares/","lf",7);
                raydium_path_ext_priority("./data/cams/","cam",7);
                raydium_path_ext_priority("./data/sprites/","sprite",7);
                raydium_path_ext_priority("./data/music/","wav",7);
                raydium_path_ext_priority("./data/music/","ogg",7);
                raydium_path_ext_priority("./","tga",7);
                }
            }
        //sky type
        if(flag_sky)
            switch(tmp_sky)
                {
                case -1:
                    raydium_sky_disable();
                    break;
                case 0:
                    break;
                }
        //This must be placed after paths processing: Textures involved.
        if(flag_hdr && tmp_hdr)
            {
            raydium_hdr_init();
            raydium_hdr_enable();
            }
    }
    // Return only if a well formed conf file found.
    if(flag_width && flag_height && flag_windowtype && flag_title) // Minimum flag used to create render window
        {
        if (fp) fclose(fp);
        return 1; //ending load of configuration
        }

    if (filename==NULL || fp==NULL){
        raydium_log("Loading a default fail-safe configuration.");
        //raydium_init_args(argc,argv);
        raydium_window_create(640,480,RAYDIUM_RENDERING_WINDOW,"My app");

        raydium_texture_filter_change(RAYDIUM_TEXTURE_FILTER_TRILINEAR);
        raydium_window_view_perspective(60,0.01,2500);

        raydium_fog_disable();
        raydium_light_enable();
        raydium_light_on(0);

        raydium_light_conf_7f(0,50,150,200,1000000,1,0.9,0.7);
        raydium_background_color_change(1,0.9,0.7,1);

        raydium_sky_box_cache();
    }
    if (fp) fclose(fp);
    return 0;
}



