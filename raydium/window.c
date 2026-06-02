/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

#ifndef DONT_INCLUDE_HEADERS
#include "index.h"
#else
#include "headers/window.h"
#endif

// proto
void raydium_init_engine(void);
int raydium_init_cli_option(char *option, char *value);
void raydium_hdr_internal_window_malloc(void);

void raydium_window_close(void)
{
//glutDestroyWindow(...);
}

// TODO: need to exit form game mode if enabled !
void raydium_window_create(GLuint tx, GLuint ty, signed char rendering, char *name)
{
// There is only one possible resolution available for the iPhone OS.
// The content will be rotated to landscape mode.
#ifdef IPHONEOS
tx=glutGet(GLUT_WINDOW_WIDTH);
ty=glutGet(GLUT_WINDOW_HEIGHT);
#endif
char mode[RAYDIUM_MAX_NAME_LEN];
#ifndef MYGLUT_H
GLuint gtx,gty;
#endif

glutInit(&raydium_init_argc, raydium_init_argv);
if(raydium_init_cli_option("window",NULL) && rendering!=RAYDIUM_RENDERING_NONE)
    rendering=RAYDIUM_RENDERING_WINDOW;
if(raydium_init_cli_option("window-fixed",NULL) && rendering!=RAYDIUM_RENDERING_NONE)
    rendering=RAYDIUM_RENDERING_WINDOW_FIXED;
if(raydium_init_cli_option("fullscreen",NULL) && rendering!=RAYDIUM_RENDERING_NONE)
    rendering=RAYDIUM_RENDERING_FULLSCREEN;

raydium_window_mode=rendering;
if(rendering==RAYDIUM_RENDERING_NONE)
    {
    raydium_init_engine();
    return;
    }

sprintf(mode,"%ix%i:32",tx,ty);
raydium_log("Requesting %s mode",mode);

#ifndef MYGLUT_H
#ifndef WIN32
// prefered under Linux
glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
#else
// seems to be the only way to get 24b zbuffer under win32
glutInitDisplayString("rgb>=4 double depth>=16");
#endif


switch(rendering)
    {
    case RAYDIUM_RENDERING_FULLSCREEN:

        glutGameModeString(mode);
        if(!glutGameModeGet(GLUT_GAME_MODE_POSSIBLE))
            raydium_log("cannot fullscreen to %s mode",mode);
        glutEnterGameMode(); // GLUT will use a "fake" Fullscreen if real one's not possible
        break;

    case RAYDIUM_RENDERING_WINDOW:
    case RAYDIUM_RENDERING_WINDOW_FIXED: // not supported yet with GLUT
        glutInitWindowSize(tx,ty);
        glutCreateWindow(name);
        break;

    default:
        raydium_log("Invalid rendering mode (windowed or fullscreen only)");
        exit(46);
        break;
    }

gtx=glutGet(GLUT_WINDOW_WIDTH);
gty=glutGet(GLUT_WINDOW_HEIGHT);

// (some version of) freeglut + fullscreen = high failure ratio :)
if(gtx==0 && gty==0)
    {
    glutInitWindowSize(tx,ty);
    glutCreateWindow(name);
    gtx=glutGet(GLUT_WINDOW_WIDTH);
    gty=glutGet(GLUT_WINDOW_HEIGHT);
    }

tx=gtx;
ty=gty;

raydium_log("Got %ix%i:%i mode",tx,ty,glutGet(GLUT_WINDOW_DEPTH_SIZE));
#else
myglutCreateWindow(tx,ty,rendering,name);
#endif

raydium_atexit(raydium_window_close);
raydium_log("using %s, from %s (version %s)",glGetString(GL_RENDERER),glGetString(GL_VENDOR),glGetString(GL_VERSION));
raydium_init_engine();
#ifndef MYGLUT_H
raydium_window_tx=tx;
raydium_window_ty=ty;
#else
raydium_window_tx=glutGet(GLUT_WINDOW_WIDTH);
raydium_window_ty=glutGet(GLUT_WINDOW_HEIGHT);
#endif
raydium_mouse_x=tx/2;
raydium_mouse_y=ty/2;
}

void raydium_window_projection_update(void)
{
int tx,ty;

if (raydium_viewport_use!=-1 && raydium_viewport_use!=RAYDIUM_VIEWPORT_DIRECT)
    {
    tx=raydium_viewport[raydium_viewport_use].tx;
    ty=raydium_viewport[raydium_viewport_use].ty;
    }
else if(raydium_viewport_use==RAYDIUM_VIEWPORT_DIRECT)
    {
    tx=raydium_viewport_direct_values[2];
    ty=raydium_viewport_direct_values[3];
    }
else
    {
    tx=raydium_window_tx;
    ty=raydium_window_ty;
    }

raydium_window_ratio=tx/(float)ty;
if(glutGet(GLUT_WINDOW_INVRATIO))
    raydium_window_ratio=1.f/raydium_window_ratio;

glMatrixMode(GL_PROJECTION);
glLoadIdentity();

if(raydium_projection==RAYDIUM_PROJECTION_ORTHO)
glOrtho(raydium_projection_left,raydium_projection_right,
        raydium_projection_bottom,raydium_projection_top,
        raydium_projection_near,raydium_projection_far);

if(raydium_projection==RAYDIUM_PROJECTION_PERSPECTIVE)
gluPerspective(raydium_projection_fov,raydium_window_ratio,
               raydium_projection_near,raydium_projection_far);

// Rotate the content to fit the landscape mode on the iPhone OS.
#ifdef IPHONEOS
#ifndef IPHONEOS_ORIENTATION_PORTRAIT
glRotatef(-90,0,0,1);
#endif
#endif
glMatrixMode(GL_MODELVIEW);
glLoadIdentity();
}

void raydium_window_resize_callback(GLsizei Width, GLsizei Height)
{
if(!Height) Height=1; // height=0 IS possible

#ifndef IPHONEOS
raydium_window_tx=Width;
raydium_window_ty=Height;
#else
#ifndef IPHONEOS_ORIENTATION_PORTRAIT
{
GLsizei swap;
swap=Width;
Width=Height;
Height=swap;
}
#endif
#endif

// called each frame !!
//raydium_log("resized to %i %i\n",Width,Height);

glViewport(0, 0, Width, Height);
raydium_window_projection_update();

// update HDR memory size
raydium_hdr_internal_window_malloc();
}



void raydium_window_view_update(void)
{
raydium_window_resize_callback(raydium_window_tx,raydium_window_ty);
raydium_fog_apply();
}

void raydium_window_view_perspective(GLfloat fov, GLfloat fnear, GLfloat ffar)
{
raydium_projection=RAYDIUM_PROJECTION_PERSPECTIVE;
if(fov>=0)  raydium_projection_fov =fov;
if(fnear>=0) raydium_projection_near=fnear;
if(ffar>=0)  raydium_projection_far =ffar;
raydium_window_view_update();
}
