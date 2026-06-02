/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

#ifndef DONT_INCLUDE_HEADERS
#include "index.h"
#else
#include "headers/render.h"
#endif

void raydium_callback_image(void);
void raydium_timecall_callback(void);
void raydium_hdr_block(signed char blocking);
signed char raydium_shader_current(int shader);
void raydium_shader_internal_vertex_attributes(int i);
void raydium_osd_draw_name (char *tex, GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);
void raydium_osd_color_change (GLfloat r, GLfloat g, GLfloat b);

// color is a GLfloat[4] (RGBA)
void raydium_render_lightmap_color(GLfloat *color)
{
memcpy(raydium_render_lightmap_color_value,color,raydium_internal_size_vector_float_4);

// Err ... no :/ There's no current color for other texture units than 0 ...
/*
glActiveTextureARB(GL_TEXTURE1_ARB);
glColor4fv(raydium_render_lightmap_color_value);
glActiveTextureARB(GL_TEXTURE0_ARB);
*/
}

void raydium_render_lightmap_color_4f(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
GLfloat col[4];
col[0]=r;
col[1]=g;
col[2]=b;
col[3]=a;
raydium_render_lightmap_color(col);
}

// let's keep this define here, until full tests
int raydium_rendering_prepare_texture_unit(GLenum tu,GLuint tex)
{
// cache state of each texunit
static GLuint texunit_state[RAYDIUM_RENDER_MAX_TEXUNITS];
static int first=1;
int tui;

if(first)
    {
    int i;
    for(i=0;i<RAYDIUM_RENDER_MAX_TEXUNITS;i++)
        texunit_state[i]=0;

    first=0;
    }

tui=tu-GL_TEXTURE0_ARB;


if(tui>=raydium_math_min(RAYDIUM_RENDER_MAX_TEXUNITS,raydium_texture_units) || tui<0)
    {
    raydium_log("render: texture unit %i is invalid (%i max, see RAYDIUM_RENDER_MAX_TEXUNITS or hardware specs)",
    tui,raydium_math_min(RAYDIUM_RENDER_MAX_TEXUNITS,raydium_texture_units));
    return 0;
    }

if(texunit_state[tui]==tex)
    return 0;

//~ raydium_log("tui %i <- tex %s",tui,raydium_texture_name[tex]);

texunit_state[tui]=tex;

#ifdef DEBUG_RENDER_VERTEXARRAY_GLBINDS_DISPLAY
printf("preparing texunit %i with %s\n",tui,raydium_texture_name[tex]);
raydium_render_debug_va_count++;
#endif


// prepare "lightmaps" texture units
if(tui>0)
{
 glEnd(); // ugly, but we must close all shapes, if any
 glActiveTextureARB(tu);

#ifndef RENDER_ALLOW_LIGHTING_FOR_LIGHTMAPS
 if(raydium_texture_islightmap[tex])
 {
 glDisable(GL_LIGHTING);
 }
 else if(raydium_light_enabled_tag)
        glEnable(GL_LIGHTING);
#endif

 glDisable(GL_TEXTURE_GEN_S);
 glDisable(GL_TEXTURE_GEN_T);
 //glDisable(GL_BLEND);


 if(tex)
 {
  if(!raydium_texture_cubemap[tex])
    {
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_TEXTURE_CUBE_MAP);
    glBindTexture(GL_TEXTURE_2D,tex);
    }
  else
    {
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_CUBE_MAP);
    glBindTexture(GL_TEXTURE_CUBE_MAP,tex);
    }

  if(raydium_texture_env[tex])
  {
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
    glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE_EXT, 1);
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE_EXT);
    glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB_EXT,GL_ADD);
  }
  else if(raydium_texture_islightmap[tex])
  {
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
    glColor4fv(raydium_render_lightmap_color_value);
  }
  else
  {
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE_EXT);
    glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB_EXT,GL_MODULATE);
    glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE_EXT, 2);
  }
 }
 else
 {
 glDisable(GL_TEXTURE_2D);
 glDisable(GL_TEXTURE_CUBE_MAP);
// glBindTexture(GL_TEXTURE_2D,0);
 }
 glActiveTextureARB(GL_TEXTURE0_ARB);
}
else // "standard" textunit
{
  // default (according GL specs) DIFFUSE value.
  GLfloat one[]={0.8f, 0.8f, 0.8f, 1.f};
  GLfloat zero[]={0.0,0.0,0.0,0.0};
  GLfloat *rgb;

  if(raydium_texture_nolight[tex])
    {
    glGetBooleanv(GL_LIGHTING,&raydium_render_internal_light_previous_step);
    glDisable(GL_LIGHTING);
    }

  if(raydium_texture_shader[tex]>=0)
    raydium_shader_current(raydium_texture_shader[tex]);
  else
    raydium_shader_current(-1);

  glColor4f(1.f,1.f,1.f,1.f);

  if(raydium_texture_blended[tex]==RAYDIUM_TEXTURE_BLEND_BLENDED)
  {
  glEnable(GL_BLEND);
  glDepthMask(GL_FALSE);
  glDisable(GL_ALPHA_TEST);
//  glDisable(GL_FOG);
  glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
  if(raydium_render_cull_blended)
    glEnable(GL_CULL_FACE);
  }

  if(raydium_texture_blended[tex]==RAYDIUM_TEXTURE_BLEND_CUTOUT)
  {
  glEnable(GL_BLEND);
  glDepthMask(GL_TRUE);
  glAlphaFunc(GL_GREATER,0.78);
  glEnable (GL_ALPHA_TEST);
//  glDisable(GL_FOG);
  glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
  glDisable(GL_CULL_FACE);
  }

  if(raydium_texture_blended[tex]==RAYDIUM_TEXTURE_BLEND_ATM)
  {
  glDisable(GL_BLEND);
  glDepthMask(GL_TRUE);
  glAlphaFunc(GL_GREATER,0.5);
  glEnable (GL_ALPHA_TEST);
  glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
  glDisable(GL_CULL_FACE);
  }

  if(raydium_texture_blended[tex]==RAYDIUM_TEXTURE_BLEND_NONE)
  {
  glDisable(GL_BLEND);
  glDepthMask(GL_TRUE);
  glDisable(GL_ALPHA_TEST);
//  glEnable(GL_FOG);
  glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
  glDisable(GL_CULL_FACE);
  }


  if(raydium_texture_blended[tex]==RAYDIUM_TEXTURE_PHANTOM)
  {
  glDisable(GL_BLEND);
  glDepthMask(GL_TRUE);
  glDisable(GL_ALPHA_TEST);
  glDisable(GL_TEXTURE_2D);
  glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
  glDisable(GL_CULL_FACE);
  }

  raydium_hdr_block(!raydium_texture_hdr[tex]);


  if(raydium_texture_rgb[tex][0]>=0 && raydium_texture_blended[tex]!=RAYDIUM_TEXTURE_PHANTOM)
  {
  if(raydium_render_rgb_force_tag)
    rgb=raydium_render_rgb_force;
  else
    rgb=raydium_texture_rgb[tex];

  glDisable(GL_TEXTURE_2D);
  glColor4f(rgb[0],rgb[1],rgb[2],1.f);
   if(raydium_light_enabled_tag)
   {
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, rgb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, rgb);
   }
  }
  else
  {
  glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, one);
  glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, zero);

  if(!raydium_texture_cubemap[tex])
    {
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_TEXTURE_CUBE_MAP);
    glBindTexture(GL_TEXTURE_2D,tex);
    }
  else
    {
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_CUBE_MAP);
    glBindTexture(GL_TEXTURE_CUBE_MAP,tex);
    }
  //printf("%s\n",raydium_texture_name[tex]);
  }
} // end "standard" texture
return 1;
}


void raydium_rendering_internal_prepare_texture_render(GLuint tex)
{
raydium_rendering_prepare_texture_unit(GL_TEXTURE0_ARB,tex);
}

void raydium_rendering_internal_restore_render_state(void)
{
//#define ONE 0.8 // default DIFFUSE value.
GLfloat one[]={0.8f, 0.8f, 0.8f, 1.f};

//return; // make no sens to restore state since next texture will reset it

glDisable(GL_BLEND);
glDepthMask(GL_TRUE);
glEnable(GL_TEXTURE_2D);
glDisable(GL_TEXTURE_CUBE_MAP);
glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, one);
}

// 2D quick 'n ugly clipping test
char infov(GLfloat x, GLfloat y)
{
#ifdef RENDER_DEBUG_NO_CLIP
return 1;
#endif
if((x+raydium_camera_cursor_place[0])>(raydium_camera_x-raydium_projection_far) &&
   (x+raydium_camera_cursor_place[0])<(raydium_camera_x+raydium_projection_far) &&
   (y+raydium_camera_cursor_place[1])>(raydium_camera_y-raydium_projection_far) &&
   (y+raydium_camera_cursor_place[1])<(raydium_camera_y+raydium_projection_far) ) return 1; else return 0;
}

// used by shadows
void raydium_rendering_from_to_simple(GLuint from, GLuint to)
{
GLuint i;
//printf("%i -> %i\n",from,to);
glBegin(GL_TRIANGLES);
  for(i=from;i<to;i+=3)
    {
    glVertex3f(raydium_vertex_x(i+0), raydium_vertex_y(i+0), raydium_vertex_z(i+0));
    glVertex3f(raydium_vertex_x(i+1), raydium_vertex_y(i+1), raydium_vertex_z(i+1));
    glVertex3f(raydium_vertex_x(i+2), raydium_vertex_y(i+2), raydium_vertex_z(i+2));
    }
glEnd();

glBegin(GL_POINTS);
    glVertex3f(raydium_vertex_x(from), raydium_vertex_y(from), raydium_vertex_z(from));
glEnd();
}

void raydium_rendering_from_to(GLuint from, GLuint to)
{
GLuint tex,i,j;
int multi_prepared=0;
signed char env=0;

if(raydium_shadow_rendering)
    {
    raydium_rendering_from_to_simple(from,to);
    return;
    }

//for(tex=1;tex<raydium_texture_index;tex++)
for(tex=1;tex<RAYDIUM_MAX_TEXTURES;tex++)
if(raydium_texture_used[tex])
{

  // prepare first texture unit
  raydium_rendering_prepare_texture_unit(GL_TEXTURE0_ARB,tex);

  // ... and reset next ones
  for(j=0;j<(RAYDIUM_RENDER_MAX_TEXUNITS-1);j++)
        raydium_rendering_prepare_texture_unit(GL_TEXTURE1_ARB+j,0);

  glBegin(GL_TRIANGLES);


  for(i=from,j=0;i<to;i+=3)
  if(raydium_vertex_texture[i]==tex)
  {
#ifdef DEBUG_RENDER_DISABLE_DISPLAYLISTS
   if(!infov(raydium_vertex_x(i  ),raydium_vertex_y(i  )) &&
      !infov(raydium_vertex_x(i+1),raydium_vertex_y(i+1)) &&
      !infov(raydium_vertex_x(i+2),raydium_vertex_y(i+2)) )
        continue;
#endif


#ifdef RENDER_DEBUG_TAG
    if(raydium_vertex_tag[i  ] ||
       raydium_vertex_tag[i+1] ||
       raydium_vertex_tag[i+2] )
        glColor4f(1.f,0.f,1.f,1.f);
    else
        glColor4f(1.f,1.f,1.f,1.f);
#endif

// TODO: Find out what exactly causes the application to terminate on the iPhone OS.
#ifndef IPHONEOS
    for(j=0;j<(RAYDIUM_RENDER_MAX_TEXUNITS-1);j++)
            if(raydium_vertex_texture_env[j][i])
                {
                env=1;
                break;
                }

    if(env || raydium_vertex_texture_multi[i])
    {
        if(raydium_vertex_texture_multi[i] && raydium_rendering_prepare_texture_unit(GL_TEXTURE1_ARB,raydium_vertex_texture_multi[i]))
            {
            //glEnd(); // done by "prepare_texture_multi"
            glBegin(GL_TRIANGLES);
            multi_prepared=1;
            }

        if(env)
            {
            for(j=0;j<(RAYDIUM_RENDER_MAX_TEXUNITS-1);j++)
                raydium_rendering_prepare_texture_unit(GL_TEXTURE1_ARB+j,raydium_vertex_texture_env[j][i]);
            //glEnd(); // done by "prepare_texture_multi"
            glBegin(GL_TRIANGLES);
            multi_prepared=1;
            }

        // THIS CODE IS DUPLICATED FOR SPEED REASON (1) (but not vertex attributes !)
        for(j=0;j<3;j++)
        {
        glNormal3f(raydium_vertex_normal_visu_x(i+j),raydium_vertex_normal_visu_y(i+j),raydium_vertex_normal_visu_z(i+j));
        raydium_shader_internal_vertex_attributes(i+j);
        glMultiTexCoord2fARB(GL_TEXTURE0_ARB,raydium_vertex_texture_u(i+j),raydium_vertex_texture_v(i+j));
        glMultiTexCoord2fARB(GL_TEXTURE1_ARB,raydium_vertex_texture_multi_u(i+j),raydium_vertex_texture_multi_v(i+j));
        //printf("%f %f\n",raydium_vertex_texture_multi_u[i+j],raydium_vertex_texture_multi_v[i+j]);
        if(raydium_fog_volumetric_enabled_tag)
#ifndef RENDER_VOLUMETRIC_FOG_AXIS_REVERSE
            glFogCoordfEXT( raydium_fog_volumetric_array(i+j));
#else
            glFogCoordfEXT(-raydium_fog_volumetric_array(i+j));
#endif
        glVertex3f(raydium_vertex_x(i+j), raydium_vertex_y(i+j), raydium_vertex_z(i+j));
        raydium_vertex_counter++;
        }
    }
    else
#endif
    {
        // cancel previous multitexturing settings
        if(multi_prepared)
            {
            for(j=0;j<(RAYDIUM_RENDER_MAX_TEXUNITS-1);j++)
                raydium_rendering_prepare_texture_unit(GL_TEXTURE1_ARB+j,0);
            multi_prepared=0;
            glBegin(GL_TRIANGLES);
            }

        // THIS CODE IS DUPLICATED FOR SPEED REASON (2) (but not vertex attributes !)
        for(j=0;j<3;j++)
        {
        glNormal3f(raydium_vertex_normal_visu_x(i+j),raydium_vertex_normal_visu_y(i+j),raydium_vertex_normal_visu_z(i+j));
        glMultiTexCoord2fARB(GL_TEXTURE0_ARB,raydium_vertex_texture_u(i+j),raydium_vertex_texture_v(i+j));
        if(raydium_fog_volumetric_enabled_tag)
#ifndef RENDER_VOLUMETRIC_FOG_AXIS_REVERSE
            glFogCoordfEXT( raydium_fog_volumetric_array(i+j));
#else
            glFogCoordfEXT(-raydium_fog_volumetric_array(i+j));
#endif
        glVertex3f(raydium_vertex_x(i+j), raydium_vertex_y(i+j), raydium_vertex_z(i+j));
        raydium_vertex_counter++;
        }
    }
  }
  glEnd();
  if(raydium_render_internal_light_previous_step==GL_TRUE)
    {
    glEnable(GL_LIGHTING);
    raydium_render_internal_light_previous_step=-1;
    }
} // end for "all textures"

for(j=0;j<(RAYDIUM_RENDER_MAX_TEXUNITS-1);j++)
        raydium_rendering_prepare_texture_unit(GL_TEXTURE1_ARB+j,0);
}


void raydium_rendering(void)
{
raydium_rendering_from_to(0,raydium_vertex_index);
}

void raydium_rendering_finish(void)
{
static int fps=0;
static unsigned long last=0;
fps++;
if(!last) last=raydium_timecall_clock();

raydium_callback_image();
glFlush();
raydium_rendering_internal_restore_render_state();
//glutPostRedisplay();

#ifdef DEBUG_RENDER_VERTEXARRAY_GLBINDS_DISPLAY
raydium_log("DEBUG_RENDER_VERTEXARRAY_GLBINDS_DISPLAY: %i",raydium_render_debug_va_count);
#endif


#ifdef DEBUG_MOVIE
raydium_timecall_debug_movie++;

if(raydium_key[GLUT_KEY_F11])
{
char name[128];
static int frame;
sprintf(name,"movie/frame%04d.jpg",frame);
raydium_capture_frame_jpeg_now(name);
frame++;
}
#endif

switch(raydium_capture_asked)
    {
    case RAYDIUM_CAPTURE_TGA:
        raydium_capture_frame_now(raydium_capture_filename);
        break;
    case RAYDIUM_CAPTURE_JPG:
        raydium_capture_frame_jpeg_now(raydium_capture_filename);
        break;
}
raydium_capture_asked=RAYDIUM_CAPTURE_NONE;

#ifndef DEBUG_SHADOW_MAP_VIEW
glutSwapBuffers();
#endif

raydium_shadow_map_generate();
raydium_key_last=0;
raydium_mouse_click=0;
raydium_camera_pushed=0;
glPopMatrix();
if(raydium_timecall_clock() > last + raydium_timecall_clocks_per_sec)
    {
    last=raydium_timecall_clock();
    raydium_render_fps=fps;
    fps=0;
    }
}

void raydium_rendering_wireframe(void)
{
glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void raydium_rendering_normal(void)
{
glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void raydium_rendering_rgb_force(GLfloat r, GLfloat g, GLfloat b)
{
raydium_render_rgb_force_tag=1;
raydium_render_rgb_force[0]=r;
raydium_render_rgb_force[1]=g;
raydium_render_rgb_force[2]=b;
raydium_render_rgb_force[3]=1.0;
}

void raydium_rendering_rgb_normal(void)
{
raydium_render_rgb_force_tag=0;
}


void raydium_rendering_displaylists_enable(void)
{
raydium_render_displaylists_tag=1;
}

void raydium_rendering_displaylists_disable(void)
{
raydium_render_displaylists_tag=0;
}

void raydium_render_fps_limit(float maxfps)
{
   if(maxfps<0)
    maxfps=0;
   raydium_render_max_fps=maxfps;
}

void raydium_render_vblank(signed char enable)
{
enable=(enable?1:0);
#ifndef IPHONEOS
# ifndef WIN32
if(glxewIsSupported("GLX_SGI_swap_control"))
    {
    glXSwapIntervalSGI(enable);
    return;
    }
# endif
# ifdef WIN32
if(wglewIsSupported("WGL_EXT_swap_control"))
    {
    wglSwapIntervalEXT(enable);
    return;
    }
# endif
#endif
raydium_log("Error: swap control not available");
}

void raydium_render_cull_blended_set(signed char cull_or_not)
{
raydium_render_cull_blended=(cull_or_not?1:0); // !!cull_or_not should work, too, no ? :)
}

// are faked textures package compliant ?
void raydium_render_loading(void)
{
// Sad and dirty, but we *must* use hard-coded image data, since we need
// this texture even when NO data is available yet.
// Current font: Sans Bold. Costs something like 2 KB.
// Tried with ATM filter first, but it looks better as-is at 128x16.
static unsigned char texture_data[] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,18,117,166,179,158,92,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,91,255,255,255,255,255,235,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,54,58,0,0,48,255,255,190,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,26,188,181,181,177,161,124,44,0,0,0,0,0,
45,173,226,230,189,75,0,0,0,0,122,181,186,42,0,46,
186,181,115,0,0,0,175,181,153,0,0,101,181,178,0,0,
177,181,145,0,0,0,108,202,233,219,154,13,0,0,0,147,
228,220,121,65,181,174,0,0,0,129,225,221,118,91,181,182,
1,0,146,181,192,0,0,158,181,124,0,0,108,181,180,0,
0,0,80,173,165,51,160,254,234,0,0,57,181,179,0,0,
0,179,181,39,0,0,157,181,139,0,0,0,0,0,0,0,
0,0,0,36,255,254,255,255,255,255,255,181,0,0,0,53,
255,254,255,251,255,255,112,0,0,0,218,255,255,117,0,127,
255,255,214,0,0,0,246,255,215,0,0,142,255,250,0,0,
249,255,204,0,0,193,255,255,241,255,254,255,21,0,151,255,
255,214,241,232,255,244,0,0,144,255,255,248,255,222,255,255,
2,0,206,255,255,0,0,222,255,174,0,0,152,255,253,0,
0,118,255,255,255,255,197,255,237,0,0,80,255,252,0,0,
0,252,255,55,0,0,220,255,195,0,0,0,0,0,0,0,
0,0,0,36,255,255,169,56,87,199,255,255,198,0,0,255,
254,236,0,0,189,255,255,14,0,0,255,255,252,194,0,199,
253,255,255,0,0,0,246,255,215,0,0,142,255,250,0,0,
249,255,204,0,38,255,255,98,0,11,248,254,163,0,217,255,
187,0,0,214,255,244,0,15,255,255,189,0,0,254,255,255,
2,0,206,255,255,0,0,222,255,174,0,0,152,255,253,0,
9,255,255,197,9,50,255,255,237,0,0,79,253,251,0,0,
0,251,253,54,0,0,219,253,194,0,0,0,0,0,0,0,
0,0,0,36,255,255,145,0,0,0,215,255,254,2,1,254,
255,95,0,0,34,255,255,43,0,92,255,255,166,255,0,255,
161,255,255,87,0,0,246,255,215,0,0,142,255,250,0,0,
249,255,204,0,147,255,255,14,0,0,191,255,239,0,181,254,
240,77,54,140,255,244,0,37,255,255,36,0,0,154,255,255,
2,0,206,255,255,0,0,222,255,174,0,0,152,255,253,0,
69,255,255,50,0,0,168,255,237,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,36,255,255,145,0,0,0,40,255,255,90,1,254,
255,85,0,0,24,255,255,45,0,186,255,184,93,255,112,255,
87,193,255,178,0,0,246,255,218,0,0,145,255,250,0,0,
249,255,204,0,154,255,255,11,0,0,187,255,245,0,18,205,
255,255,255,255,255,244,0,39,255,255,26,0,0,144,255,255,
2,0,206,255,255,0,0,222,255,186,0,0,154,255,253,0,
77,255,255,35,0,0,145,255,237,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,36,255,255,145,0,0,0,20,255,255,118,0,255,
254,212,0,0,153,255,255,23,0,232,255,98,13,255,241,255,
13,110,255,229,0,0,246,255,245,0,0,203,254,250,0,0,
249,255,204,0,66,255,255,53,0,0,237,254,184,0,4,0,
0,31,31,140,255,235,0,22,255,255,154,0,0,242,255,255,
2,0,206,255,255,0,0,222,255,255,5,0,181,255,243,0,
38,255,255,133,0,0,242,255,237,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,36,255,255,145,0,0,0,64,254,255,69,0,101,
255,255,211,195,255,255,164,0,11,255,255,43,0,232,255,231,
0,48,255,255,10,0,246,255,254,255,207,255,255,210,0,0,
249,255,204,0,0,232,255,255,186,240,255,255,44,0,21,255,
171,146,168,255,255,181,0,0,194,254,255,193,218,247,255,255,
2,0,206,255,255,0,0,222,255,247,251,209,255,254,178,0,
0,204,254,255,191,225,249,255,237,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,36,255,255,145,0,0,0,247,254,254,0,0,0,
97,227,255,255,242,131,0,0,111,238,246,0,0,157,235,150,
0,0,246,238,106,0,227,235,198,137,255,255,213,2,0,0,
249,255,204,0,0,0,162,255,255,255,207,49,0,0,14,226,
255,255,255,252,159,0,0,0,1,184,255,255,170,126,255,255,
2,0,190,235,249,0,0,205,235,151,126,255,255,217,18,0,
0,8,192,255,255,159,117,235,219,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,36,255,255,200,128,158,255,254,255,128,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
249,255,204,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,134,255,255,
2,0,6,8,8,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,36,255,255,255,255,255,255,255,110,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
249,255,204,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,134,255,255,
2,0,216,255,255,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,15,114,110,109,105,89,53,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
230,235,188,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,123,235,236,
2,0,190,235,249,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

static char *fake_texture="_loading.tga";
static int fake_sx=128;
static int fake_sy=16;

static signed char first=1;
static float color=1;
static float color_evo=-0.2;
int texsave;

if(first && raydium_texture_exists(fake_texture)==-1)
    {
    // create the "loading" texture:
    raydium_texture_load_internal(fake_texture,fake_texture,
                                  1,fake_sx,fake_sy,1,-1); // fake, size, bpp

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D,0,GL_ALPHA,fake_sx,fake_sy,0,
                 GL_ALPHA,GL_UNSIGNED_BYTE,texture_data);

    first=0;
    // bug: message is not displayed the first time during the very first frame
    // on early init with some applications (test6, shader_test, ...)
    // strange fix: need to call this here, even if osd_draw function
    // will do the same right after ... (?)
    raydium_window_resize_callback(raydium_window_tx, raydium_window_ty);
    }

// render a black screen with fake_texture in the bottom right:
glClearColor(0,0,0,1);
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
raydium_osd_color_change(color,color,color); // should restore here too ?

// draw with our texture, without breaking too much things (display lists, texture units, ...)
texsave=raydium_texture_current_main;
raydium_osd_draw_name(fake_texture,75,0,100,5);
raydium_texture_current_set(texsave);
raydium_rendering_internal_prepare_texture_render(raydium_texture_current_main);

glutSwapBuffers();

color+=color_evo;
if(color>1) { color=1; color_evo*=-1; }
if(color<0.41) { color=0.4; color_evo*=-1; }

raydium_clear_color_update(); // restore clear color
}
