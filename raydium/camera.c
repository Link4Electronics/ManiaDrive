/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

#ifndef DONT_INCLUDE_HEADERS
#include "index.h"
#else
#include "headers/camera.h"
#endif

// res3 is GLfloat[3]
void raydium_camera_vectors(GLfloat *res3)
{
GLfloat m[16];
GLfloat front[3]={1,0,0};
//GLfloat up[3]={0,0,1};
GLfloat res1[3];
GLfloat res2[3];


raydium_math_pos_get_modelview(res1); // get current position
raydium_math_pos_to_matrix(front,m); // create matrix using front
glPushMatrix();
glMultMatrixf(m); // project front
raydium_math_pos_get_modelview(res2); // get new position
glPopMatrix();

// create front vector
res3[1]=-(res1[0]-res2[0]); // x
res3[2]=(res1[1]-res2[1]); // y
res3[0]=(res1[2]-res2[2]); // z

// create up vector

// fallback
res3[3]=res3[4]=0;
res3[5]=1;

/*
raydium_trigo_pos_get_modelview(res1); // get current position
raydium_trigo_pos_to_matrix(up,m); // create matrix using front
glPushMatrix();
glMultMatrixf(m); // project front
raydium_trigo_pos_get_modelview(res2); // get new position
glPopMatrix();
res3[4]=(res1[0]-res2[0]); // x
res3[5]=-(res1[1]-res2[1]); // y
res3[3]=-(res1[2]-res2[2]); // z
*/

//raydium_log("%f %f %f",res3[3],res3[4],res3[5]);
}


void raydium_camera_internal_prepare(void)
{
GLfloat x,y,z;

glLoadIdentity();


if (raydium_viewport_use!=RAYDIUM_VIEWPORT_NONE)
    return;

if(raydium_camera_rumble_remaining>0)
    {
    x=raydium_random_f(-raydium_camera_rumble_amplitude,raydium_camera_rumble_amplitude);
    y=raydium_random_f(-raydium_camera_rumble_amplitude,raydium_camera_rumble_amplitude);
    z=raydium_random_f(-raydium_camera_rumble_amplitude,raydium_camera_rumble_amplitude);

    glRotatef(z,0,0,1);
    glRotatef(x,1,0,0);
    glRotatef(y,0,1,0);

    raydium_camera_rumble_remaining-=raydium_frame_time;
    raydium_camera_rumble_amplitude+=(raydium_camera_rumble_evolution*raydium_frame_time);
    if(raydium_camera_rumble_amplitude<=0)
        {
        raydium_camera_rumble_amplitude=0;
        raydium_camera_rumble_remaining=0;
        }
    }
else raydium_camera_rumble_remaining=0;
}


void raydium_camera_internal(GLfloat x, GLfloat y, GLfloat z)
{
if(raydium_frame_first_camera_pass)
    {
    float pos[3];
    float or[6];
    pos[0]=x;
    pos[1]=y;
    pos[2]=z;
    if(raydium_sound && raydium_viewport_use==RAYDIUM_VIEWPORT_NONE)
        {
        raydium_camera_vectors(or); // get vectors
        raydium_sound_SetListenerPos(pos);
        raydium_sound_SetListenerOr(or);
        }

    if(raydium_sky_atmosphere_check())
        {
        raydium_sky_box_render(x,y,z);
        raydium_sky_atmosphere_render(x,y,z,RAYDIUM_SKY_SPHERE_DEFAULT_DETAIL);
        }
    else
    if(raydium_sky_check())
        {
        raydium_sky_box_render(x,y,z);
        }

    //raydium_sky_box_render(x,y,z);
    //raydium_atmosphere_render(x,y,z,SPHERE_DEFAULT_DETAIL);
    raydium_frame_first_camera_pass=0;
    raydium_light_update_position_all();

    glGetDoublev( GL_MODELVIEW_MATRIX,raydium_camera_gl_modelview);
    glGetDoublev( GL_PROJECTION_MATRIX,raydium_camera_gl_projection);
    glGetIntegerv( GL_VIEWPORT,raydium_camera_gl_viewport);
    }

if(!raydium_camera_pushed)
    {
    raydium_camera_pushed=1;
    raydium_camera_x=x;
    raydium_camera_y=y;
    raydium_camera_z=z;

    glPushMatrix();
    memset(raydium_camera_cursor_place,0,3*sizeof(GLfloat));
    }
else raydium_log("Warning: too many calls to camera_* ! (matrix already pushed)");
}


void raydium_camera_place(GLfloat x, GLfloat y, GLfloat z,
                          GLfloat lacet, GLfloat tangage, GLfloat roulis)
{
raydium_camera_internal_prepare();
glRotatef(roulis,0,0,1);
glRotatef(tangage,1,0,0);
glRotatef(lacet,0,1,0);
glTranslatef(x, y, z);
glRotatef(90,0,0,1);
glRotatef(90,0,1,0);
raydium_camera_internal(z,x,-y);
}

float *raydium_camera_data_get(void)
{
return (float *)raydium_camera_data;
}

void raydium_camera_data_set(GLfloat *data)
{
memcpy(raydium_camera_data,data,sizeof(GLfloat)*6);
}

void raydium_camera_data_dump(void)
{
raydium_log("GLfloat camera_data[] = {%ff,%ff,%ff,%ff,%ff,%ff}",
    raydium_camera_data[0],
    raydium_camera_data[1],
    raydium_camera_data[2],
    raydium_camera_data[3],
    raydium_camera_data[4],
    raydium_camera_data[5]);
}


void raydium_camera_look_at(float x, float y, float z,
                          float x_to, float y_to, float z_to)
{
float delta[3];
float angle[3];

angle[0]=angle[1]=angle[2]=0.0f;

//glLoadIdentity();
raydium_camera_internal_prepare();
glRotatef(raydium_camera_look_at_roll,0,0,1);
raydium_camera_look_at_roll=0;
gluLookAt(x,y,z,z_to,x_to,-y_to,0.,0.,1.);
raydium_camera_internal(x,y,z);

//getting angles for data
delta[0]=z_to-x;
delta[1]=x_to-y;
delta[2]=-y_to-z;

//Corrected spherical angle computing
angle[0]=360.0f-raydium_math_rad2deg(atan2(delta[1],delta[0]));
angle[1]=360.0f-raydium_math_rad2deg(atan2(delta[2],sqrt(delta[0]*delta[0]+delta[1]*delta[1])));

//feeding data
raydium_camera_data[0]=x;
raydium_camera_data[1]=y;
raydium_camera_data[2]=z;
raydium_camera_data[3]=angle[0];
raydium_camera_data[4]=angle[1];
raydium_camera_data[5]=angle[2];//zero
}

void raydium_camera_replace(void)
{
if(raydium_shadow_rendering)
    {
    glLoadIdentity();
    return;
    }

if(!raydium_camera_pushed)
    raydium_log("Warning: no camera to replace (matrix stack's empty)");
else
    {
    glPopMatrix();
    glPushMatrix();
    memset(raydium_camera_cursor_place,0,3*sizeof(GLfloat));
    }
}

void raydium_camera_replace_go(GLfloat *pos, GLfloat *R)
{
// pos[3], R[3*3]

GLfloat matrix[16];

raydium_camera_replace();
matrix[0]=R[0];
matrix[1]=R[4];
matrix[2]=R[8];
matrix[3]=0;
matrix[4]=R[1];
matrix[5]=R[5];
matrix[6]=R[9];
matrix[7]=0;
matrix[8]=R[2];
matrix[9]=R[6];
matrix[10]=R[10];
matrix[11]=0;
matrix[12]=pos[0];
matrix[13]=pos[1];
matrix[14]=pos[2];
matrix[15]=1;
glMultMatrixf(matrix);
memcpy(raydium_camera_cursor_place,pos,3*sizeof(GLfloat));
}


void raydium_camera_rumble(GLfloat amplitude, GLfloat ampl_evo, GLfloat secs)
{
if (raydium_viewport_use!=RAYDIUM_VIEWPORT_NONE)
    return;
raydium_camera_rumble_amplitude=amplitude;
raydium_camera_rumble_evolution=ampl_evo;
raydium_camera_rumble_remaining=secs;
}

void raydium_camera_path_reset(void)
{
raydium_camera_path_reset_flag=1;
}

void raydium_camera_smooth_data_init(GLfloat *data)
{
int i;
for(i=0;i<RAYDIUM_CAMERA_SMOOTH_DATA_SIZE;i++)
    data[i]=0;
data[6]=90; // zoom
}

void raydium_camera_smooth_data_set(GLfloat *data)
{
if(data==NULL) data=raydium_camera_smooth_data_primary;
raydium_camera_smooth_data_ptr=data;
}

// if step is <= 0, moves will be instaneous
// camera will be placed only if step is >=0 (negative steps are used
// only to change internal vars)
void raydium_camera_smooth(GLfloat px, GLfloat py, GLfloat pz,
                           GLfloat lx, GLfloat ly, GLfloat lz,
                           GLfloat zoom, GLfloat roll, GLfloat step)
{
GLfloat opx,opy,opz;
GLfloat olx,oly,olz;
GLfloat ozoom=90;
GLfloat oroll=0;
float delta[3];
float angle[3];
angle[0]=angle[1]=angle[2]=0;

// see RAYDIUM_CAMERA_SMOOTH_DATA_SIZE
opx=raydium_camera_smooth_data_ptr[0];
opy=raydium_camera_smooth_data_ptr[1];
opz=raydium_camera_smooth_data_ptr[2];
olx=raydium_camera_smooth_data_ptr[3];
oly=raydium_camera_smooth_data_ptr[4];
olz=raydium_camera_smooth_data_ptr[5];
ozoom=raydium_camera_smooth_data_ptr[6];
oroll=raydium_camera_smooth_data_ptr[7];

//raydium_log("camera smooth (org): %.2f %.2f %.2f | %.2f %.2f %.2f | %.2f %.2f",opx,opy,opz,olx,oly,olz,ozoom,step);
//raydium_log("camera smooth (asked): %.2f %.2f %.2f | %.2f %.2f %.2f | %.2f %.2f",px,py,pz,lx,ly,lz,zoom,step);

if(step<=0 || // wow.. smells inf, do a instantaneous step. (and don't place cam)
   raydium_camera_path_reset_flag)
{
    opx=px;
    opy=py;
    opz=pz;
    olx=lx;
    oly=ly;
    olz=lz;
    ozoom=zoom;
    oroll=roll;
    raydium_camera_path_reset_flag=0;
}
else
{
    opx+=(px-opx)*step;
    opy+=(py-opy)*step;
    opz+=(pz-opz)*step;
    olx+=(lx-olx)*step;
    oly+=(ly-oly)*step;
    olz+=(lz-olz)*step;
    ozoom+=(zoom-ozoom)*step;
    oroll+=(roll-oroll)*step;

    if(ozoom<0) ozoom=0;
    if(ozoom>180) ozoom=270;

    if(ozoom!=raydium_projection_fov)
        {
        raydium_projection_fov=ozoom;
        raydium_window_projection_update();
        }
}

raydium_camera_look_at_roll=oroll;

if(step>=0)
    raydium_camera_look_at(opx,opy,opz,olx,oly,olz);
//raydium_log("camera smooth (result): %.2f %.2f %.2f | %.2f %.2f %.2f | %.2f | %.2f",opx,opy,opz,olx,oly,olz,ozoom,step);
//overwriting camera_data. The data stored into
//camera_look_at is not well aligned
delta[0]=olz-opx;
delta[1]=olx-opy;
delta[2]=-oly-opz;

//Corrected spherical angle computing
angle[0]=360.0f-raydium_math_rad2deg(atan2(delta[1],delta[0]));
angle[1]=360.0f-raydium_math_rad2deg(atan2(delta[2],sqrt(delta[0]*delta[0]+delta[1]*delta[1])));
//feeding data
raydium_camera_data[0]=opx;
raydium_camera_data[1]=opy;
raydium_camera_data[2]=opz;
raydium_camera_data[3]=angle[0];
raydium_camera_data[4]=angle[1];
raydium_camera_data[5]=angle[2];//zero

raydium_camera_smooth_data_ptr[0]=opx;
raydium_camera_smooth_data_ptr[1]=opy;
raydium_camera_smooth_data_ptr[2]=opz;
raydium_camera_smooth_data_ptr[3]=olx;
raydium_camera_smooth_data_ptr[4]=oly;
raydium_camera_smooth_data_ptr[5]=olz;
raydium_camera_smooth_data_ptr[6]=ozoom;
raydium_camera_smooth_data_ptr[7]=oroll;
}


void raydium_camera_path_init(int p)
{
int i;
raydium_camera_path[p].name[0]=0;
raydium_camera_path[p].steps=-1;
for(i=0;i<RAYDIUM_MAX_CAMERA_PATH_STEPS;i++)
    {
    raydium_camera_path[p].x[i]=0;
    raydium_camera_path[p].y[i]=0;
    raydium_camera_path[p].z[i]=0;
    raydium_camera_path[p].zoom[i]=0;
    raydium_camera_path[p].roll[i]=0;
    }
}

void raydium_camera_path_init_all(void)
{
int i;
for(i=0;i<RAYDIUM_MAX_CAMERA_PATHS;i++)
    raydium_camera_path_init(i);
}


int raydium_camera_path_find(char *name)
{
int i;
for(i=0;i<RAYDIUM_MAX_CAMERA_PATHS;i++)
    if(!strcmp(raydium_camera_path[i].name,name) && raydium_camera_path[i].steps>-1)
        return i;
return -1;
}


int raydium_camera_path_load(char *filename)
{
FILE *fp;
int i=0;
GLfloat x,y,z,zoom,roll;
int p;

fp=raydium_file_fopen(filename,"rt");
if(!fp)
    {
    raydium_log("camera: cannot open camera path '%s'",filename);
    return -1;
    }

for(p=0;p<RAYDIUM_MAX_CAMERA_PATHS;p++)
    if(raydium_camera_path[p].steps==-1)
        break;

if(p==RAYDIUM_MAX_CAMERA_PATHS)
    {
    raydium_log("camera: cannot find any free slot !",filename);
    return -1;
    }

strcpy(raydium_camera_path[p].name,filename);

while( fscanf(fp,"%f %f %f %f %f\n",&x,&y,&z,&zoom,&roll)!=EOF )
    {
    raydium_camera_path[p].x[i]=x;
    raydium_camera_path[p].y[i]=y;
    raydium_camera_path[p].z[i]=z;
    raydium_camera_path[p].zoom[i]=zoom;
    raydium_camera_path[p].roll[i]=roll;
    i++;
    }

raydium_camera_path[p].steps=i;
raydium_log("camera path '%s' loaded (slot %i, %i steps)",filename,p,i);
return p;
}


void raydium_camera_path_draw(int p)
{
int i;

if(p>=0 && p<RAYDIUM_MAX_CAMERA_PATHS)
    {
    glDisable(GL_LIGHTING);
    raydium_texture_current_set_name("rgb(1,0,0)");
    raydium_rendering_internal_prepare_texture_render(raydium_texture_current_main);
    glLineWidth(1.f);
    glBegin(GL_LINE_LOOP);

    for(i=0;i<raydium_camera_path[p].steps;i++)
        {
        glVertex3f(
                raydium_camera_path[p].x[i],
                raydium_camera_path[p].y[i],
                raydium_camera_path[p].z[i]);
        }

    glEnd();
    if(raydium_light_enabled_tag)
        glEnable(GL_LIGHTING);

    return;
    }
raydium_log("camera path draw failed : invalid index");
}

void raydium_camera_path_draw_name(char *path)
{
int p;

p=raydium_camera_path_find(path);
if(p==-1) p=raydium_camera_path_load(path);

raydium_camera_path_draw(p);
}


signed char raydium_camera_smooth_path(char *path, GLfloat step, GLfloat *x, GLfloat *y, GLfloat *z, GLfloat *zoom, GLfloat *roll)
{
int p;
int step1,step2;
GLfloat vx,vy,vz,vzoom,vroll;
GLfloat diff;

p=raydium_camera_path_find(path);

if(p==-1)
    p=raydium_camera_path_load(path);

if(p==-1)
    return 0;

step1=(int)step;
step2=step1+1;
diff=step-step1;

while(step1>=raydium_camera_path[p].steps)
    {
    step1-=raydium_camera_path[p].steps;
    }

while(step2>=raydium_camera_path[p].steps)
    {
    step2-=raydium_camera_path[p].steps;
    }

vx=raydium_camera_path[p].x[step2]-raydium_camera_path[p].x[step1];
vy=raydium_camera_path[p].y[step2]-raydium_camera_path[p].y[step1];
vz=raydium_camera_path[p].z[step2]-raydium_camera_path[p].z[step1];
vzoom=raydium_camera_path[p].zoom[step2]-raydium_camera_path[p].zoom[step1];
vroll=raydium_camera_path[p].roll[step2]-raydium_camera_path[p].roll[step1];

*x=raydium_camera_path[p].x[step1]+(vx*diff);
*y=raydium_camera_path[p].y[step1]+(vy*diff);
*z=raydium_camera_path[p].z[step1]+(vz*diff);
*zoom=raydium_camera_path[p].zoom[step1]+(vzoom*diff);
*roll=raydium_camera_path[p].roll[step1]+(vroll*diff);

return 1;
}


void raydium_camera_smooth_path_to_pos(char *path, GLfloat lx, GLfloat ly, GLfloat lz, GLfloat path_step, GLfloat smooth_step)
{
GLfloat x=0,y=0,z=0,zoom=0,roll=0;

if(raydium_camera_smooth_path(path,path_step,&x,&y,&z,&zoom,&roll)==-1)
    raydium_log("camera path error with '%s'",path);

raydium_camera_smooth(x,y,z,ly,-lz,lx,zoom,roll,smooth_step);
}

void raydium_camera_smooth_pos_to_path(GLfloat lx, GLfloat ly, GLfloat lz, char *path, GLfloat path_step, GLfloat smooth_step)
{
GLfloat x=0,y=0,z=0,zoom=0,roll=0;

if(raydium_camera_smooth_path(path,path_step,&x,&y,&z,&zoom,&roll)==-1)
    raydium_log("camera path error with '%s'",path);

raydium_camera_smooth(lx,ly,lz,y,-z,x,zoom,roll,smooth_step);
}

void raydium_camera_smooth_path_to_path(char *path_from, GLfloat path_step_from, char *path_to, GLfloat path_step_to, GLfloat smooth_step)
{
GLfloat fx=0,fy=0,fz=0,fzoom=0,froll=0;
GLfloat tx=0,ty=0,tz=0,tzoom=0,troll=0;

if(raydium_camera_smooth_path(path_from,path_step_from,&fx,&fy,&fz,&fzoom,&froll)==-1)
    raydium_log("camera path error with '%s' (from)",path_from);

if(raydium_camera_smooth_path(path_to,path_step_to,&tx,&ty,&tz,&tzoom,&troll)==-1)
    raydium_log("camera path error with '%s' (to)",path_to);

raydium_camera_smooth(fx,fy,fz, ty,-tz,tx,fzoom,froll,smooth_step);
}

void raydium_viewport_init(void)
{
raydium_viewport_nb=0;
raydium_viewport_use=RAYDIUM_VIEWPORT_NONE;
}

void raydium_viewport_direct_open(int x, int y, int sizex, int sizey)
{
signed char clamped=0;

if (raydium_viewport_use!=RAYDIUM_VIEWPORT_NONE)
    {
    raydium_log ("An other viewport is already enabled");
    return;
    }

raydium_viewport_use=RAYDIUM_VIEWPORT_DIRECT;
raydium_viewport_direct_values[0]=x;
raydium_viewport_direct_values[1]=y;
raydium_viewport_direct_values[2]=sizex;
raydium_viewport_direct_values[3]=sizey;

raydium_viewport_saved_mouse[0]=raydium_mouse_x;
raydium_viewport_saved_mouse[1]=raydium_mouse_y;
// translate mouse_coords to viewport
raydium_mouse_x=((raydium_mouse_x-x)*((float)raydium_window_tx/sizex));
raydium_mouse_y=raydium_window_ty-(((raydium_window_ty-raydium_mouse_y-y)*((float)raydium_window_ty/sizey)));
// and clamp it
if(raydium_mouse_x<0) { raydium_mouse_x=0; clamped=1; }
if(raydium_mouse_y<0) { raydium_mouse_y=0; clamped=1; }
if(raydium_mouse_x>=raydium_window_tx) { raydium_mouse_x=raydium_window_tx-1; clamped=1; }
if(raydium_mouse_y>=raydium_window_ty) { raydium_mouse_y=raydium_window_ty-1; clamped=1; }
raydium_viewport_direct_mouse_clamped=clamped;

if(raydium_camera_pushed)
    {
    raydium_viewport_saved_context=1;
    glGetDoublev(GL_MODELVIEW_MATRIX,raydium_viewport_saved_modelview_data);
    raydium_viewport_saved_camera_pos[0]=raydium_camera_x;
    raydium_viewport_saved_camera_pos[1]=raydium_camera_y;
    raydium_viewport_saved_camera_pos[2]=raydium_camera_z;

    memcpy(raydium_viewport_saved_gl_viewport,raydium_camera_gl_viewport,sizeof(GLint)*4);
    memcpy(raydium_viewport_saved_gl_projection,raydium_camera_gl_projection,sizeof(GLdouble)*16);
    memcpy(raydium_viewport_saved_gl_modelview,raydium_camera_gl_modelview,sizeof(GLdouble)*16);

    glPopMatrix();
    raydium_camera_pushed=0;
    }
else raydium_viewport_saved_context=0;

raydium_frame_first_camera_pass=1;

glViewport(x, y, sizex, sizey);
glScissor(x, y, sizex, sizey);
glEnable(GL_SCISSOR_TEST);
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
glDisable(GL_SCISSOR_TEST);
glEnable(GL_STENCIL_TEST); // HDR

raydium_window_projection_update();
}

void raydium_viewport_direct_open_4f(GLfloat x, GLfloat y, GLfloat sizex, GLfloat sizey)
{
raydium_viewport_direct_open(
    (x/100.f)*raydium_window_tx,
    (y/100.f)*raydium_window_ty,
    (sizex/100.f)*raydium_window_tx,
    (sizey/100.f)*raydium_window_ty);
}

void raydium_viewport_direct_close(void)
{
if(raydium_viewport_use!=RAYDIUM_VIEWPORT_DIRECT)
    return;
raydium_viewport_use=RAYDIUM_VIEWPORT_NONE;
raydium_rendering_internal_restore_render_state();
if (raydium_camera_pushed){
    glPopMatrix();
    raydium_camera_pushed=0;
}
raydium_viewport_use=RAYDIUM_VIEWPORT_NONE;
glViewport(0,0, raydium_window_tx, raydium_window_ty);
if(raydium_projection==RAYDIUM_PROJECTION_PERSPECTIVE)
    {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(raydium_projection_fov,(GLfloat)raydium_window_tx/(GLfloat)raydium_window_ty,
                    raydium_projection_near,raydium_projection_far);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    }

raydium_mouse_x=raydium_viewport_saved_mouse[0];
raydium_mouse_y=raydium_viewport_saved_mouse[1];

// we try to restore everything about the camera
if(raydium_viewport_saved_context)
    {
    glLoadMatrixd(raydium_viewport_saved_modelview_data);
    raydium_camera_x=raydium_viewport_saved_camera_pos[0];
    raydium_camera_y=raydium_viewport_saved_camera_pos[1];
    raydium_camera_z=raydium_viewport_saved_camera_pos[2];
    glPushMatrix();

    memcpy(raydium_camera_gl_viewport,raydium_viewport_saved_gl_viewport,sizeof(GLint)*4);
    memcpy(raydium_camera_gl_projection,raydium_viewport_saved_gl_projection,sizeof(GLdouble)*16);
    memcpy(raydium_camera_gl_modelview,raydium_viewport_saved_gl_modelview,sizeof(GLdouble)*16);

    raydium_light_update_position_all();
    raydium_camera_pushed=1;
    raydium_viewport_saved_context=0;
    }

}

signed char raydium_viewport_direct_is_mouse_in(void)
{
if(raydium_viewport_use!=RAYDIUM_VIEWPORT_DIRECT)
    return 1;

return !raydium_viewport_direct_mouse_clamped;
}


void raydium_viewport_create (char * name,int tx,int ty)
{
int i;
if (raydium_viewport_nb < RAYDIUM_MAX_VIEWPORT)
    {
    for(i=0;i<raydium_viewport_nb;i++)
        if(!strcmp(name,raydium_viewport[i].name))
            {
            raydium_log ("ERROR: viewport %s already exist",name);
            return;
            }

    if(raydium_texture_load_internal("",name,1,tx,ty,3,-1))
        {
        strcpy(raydium_viewport[raydium_viewport_nb].name,name);
        raydium_viewport[raydium_viewport_nb].tx=tx;
        raydium_viewport[raydium_viewport_nb].ty=ty;
        raydium_viewport_nb++;
        }
    }
    else
    raydium_log ("ERROR: No more viewports (%i max)",RAYDIUM_MAX_VIEWPORT);
}

void raydium_viewport_enable(char * name)
{
int i;

if (raydium_viewport_use!=RAYDIUM_VIEWPORT_NONE)
{
    raydium_log ("An other viewport is already enabled");
    return;
}

for(i=0;i<raydium_viewport_nb;i++)
   if(!strcmp(name,raydium_viewport[i].name))
   {
        glViewport(0,0, raydium_viewport[i].tx,raydium_viewport[i].ty);
        raydium_viewport_use=i;
        return;
   }
raydium_log("Viewport %s not found.",name);
}

void raydium_viewport_save(void)
{
if (raydium_viewport_use==RAYDIUM_VIEWPORT_NONE || raydium_viewport_use==RAYDIUM_VIEWPORT_DIRECT)
{
    raydium_log("No viewport enabled.");
    return;
}

glBindTexture(GL_TEXTURE_2D,raydium_texture_find_by_name(raydium_viewport[raydium_viewport_use].name));
glCopyTexSubImage2D(GL_TEXTURE_2D,0, 0,0, 0,0, raydium_viewport[raydium_viewport_use].tx, raydium_viewport[raydium_viewport_use].ty);
raydium_rendering_internal_restore_render_state();
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
glEnable(GL_STENCIL_TEST); // HDR
raydium_frame_first_camera_pass=1;
if (raydium_camera_pushed){
    glPopMatrix();
    raydium_camera_pushed=0;
}
raydium_viewport_use=RAYDIUM_VIEWPORT_NONE;
glViewport(0,0, raydium_window_tx, raydium_window_ty);
}

void raydium_viewport_draw(char * name, GLfloat tx,GLfloat ty,GLfloat sx,GLfloat sy)
{
int i;

for(i=0;i<raydium_viewport_nb;i++)
    if(!strcmp(name,raydium_viewport[i].name)){
        raydium_osd_color_ega('f');
        raydium_osd_draw_name(name,tx,ty,tx+sx,ty+sy);
        return;
    }
raydium_log("Viewport %s not found.",name);
}

void raydium_camera_freemove(int move)
{
float dir_x, dir_y;
float *data;
GLfloat *push;

GLfloat rffp_cam_angle_x, rffp_cam_angle_y, rffp_cam_angle_z;
GLfloat rffp_cam_pos_x, rffp_cam_pos_y, rffp_cam_pos_z;

GLint rffp_delta_x=0;
GLint rffp_delta_y=0;

dir_x=0;
dir_y=0;

data=raydium_camera_data_get();
rffp_cam_pos_x=data[1];
rffp_cam_pos_y=-data[2];
rffp_cam_pos_z=data[0];
rffp_cam_angle_x=data[3];
rffp_cam_angle_y=data[4];
rffp_cam_angle_z=data[5];

if(raydium_camera_push_type==RAYDIUM_CAMERA_PUSH_FREEMOVE_ABS)
    {
    push=raydium_camera_push_internal_step();
    rffp_cam_pos_x+=push[0];
    rffp_cam_pos_y+=push[1];
    rffp_cam_pos_z+=push[2];
    }

if(!raydium_gui_isvisible())
    {
    switch(move)
        {
        //RAYDIUM_CAMERA_FREEMOVE_NORMAL
        case RAYDIUM_CAMERA_FREEMOVE_NORMAL:
            //get input
            if(raydium_key[GLUT_KEY_DOWN])  dir_y=-1;
            if(raydium_key[GLUT_KEY_UP])    dir_y=1;
            if(raydium_key[GLUT_KEY_LEFT])  dir_x=-1;
            if(raydium_key[GLUT_KEY_RIGHT]) dir_x=1;

            if(raydium_camera_push_type==RAYDIUM_CAMERA_PUSH_FREEMOVE_REL && raydium_frame_time)
                {
                push=raydium_camera_push_internal_step();
                dir_y+=push[0]/(raydium_frame_time*6.f);
                dir_x+=push[1]/(raydium_frame_time*6.f);
                rffp_cam_pos_y-=push[2];
                }

            // "5 lines" mouse wheel scrool
            if(raydium_mouse_click==4) dir_y=5;
            if(raydium_mouse_click==5) dir_y=-5;

        case RAYDIUM_CAMERA_FREEMOVE_MOUSE:
            //60=experimental value
            dir_x *= (raydium_frame_time*60.0f);
            dir_y *= (raydium_frame_time*60.0f);

            raydium_mouse_grab_delta(&rffp_delta_x,&rffp_delta_y);

            //calculating the position (x,y,z) of the camera
            rffp_cam_pos_z += (raydium_math_sin(rffp_cam_angle_x+90)*dir_y*raydium_camera_freemove_speed*raydium_math_sin(90-rffp_cam_angle_y));
            rffp_cam_pos_x += (raydium_math_cos(rffp_cam_angle_x+90)*dir_y*raydium_camera_freemove_speed*raydium_math_sin(90-rffp_cam_angle_y));
            rffp_cam_pos_y += (raydium_math_cos(90-rffp_cam_angle_y)*raydium_camera_freemove_speed*dir_y);

            rffp_cam_pos_x -= (raydium_math_cos(rffp_cam_angle_x)*dir_x*raydium_camera_freemove_speed);
            rffp_cam_pos_z -= (raydium_math_sin(rffp_cam_angle_x)*dir_x*raydium_camera_freemove_speed);

            //looking where the mouse points
            rffp_cam_angle_x += (rffp_delta_x*raydium_camera_freemove_sensibility*0.1f);
            rffp_cam_angle_y += (rffp_delta_y*raydium_camera_freemove_sensibility*0.1f);
            break;

        //RAYDIUM_CAMERA_FREEMOVE_FIXED
        case RAYDIUM_CAMERA_FREEMOVE_FIXED:
            //Nothing to do camera won't move.
            break;
        }
    }

raydium_camera_place(rffp_cam_pos_x,rffp_cam_pos_y,rffp_cam_pos_z,rffp_cam_angle_x,rffp_cam_angle_y,rffp_cam_angle_z);
raydium_camera_data[1]=rffp_cam_pos_x;
raydium_camera_data[2]=-rffp_cam_pos_y;
raydium_camera_data[0]=rffp_cam_pos_z;
raydium_camera_data[3]=rffp_cam_angle_x;
raydium_camera_data[4]=rffp_cam_angle_y;
raydium_camera_data[5]=rffp_cam_angle_z;
}


void raydium_camera_orbitmove( float x_to, float y_to, float z_to)
{
static float delta_x,delta_y;
int x,y;
float mult=0.05f;
static float zoom=10.0f; //really is the distance, not zoom
float Matrix[16];

raydium_mouse_grab_delta(&x,&y);
delta_x+=x;
delta_y+=y;

if(raydium_key[GLUT_KEY_UP])if(zoom>0)zoom-=0.1;
if(raydium_key[GLUT_KEY_DOWN])zoom+=0.1;
if(raydium_mouse_click==4)zoom-=1;
if(raydium_mouse_click==5)zoom+=1;
if(zoom<=0)zoom=0.001;

raydium_camera_internal_prepare();
glRotatef(raydium_camera_look_at_roll,0,0,1);
raydium_camera_look_at_roll=0;

gluLookAt(1*zoom,0,0,0,0,0,0.,0.,1.);

glRotatef(delta_y*mult,0,1,0);
glRotatef(delta_x*mult,0,0,1);

glTranslatef(-x_to,-y_to,-z_to);
glGetFloatv(GL_MODELVIEW_MATRIX, Matrix);

//feeding camera data
raydium_camera_internal(Matrix[2]*zoom+x_to,Matrix[6]*zoom+y_to,Matrix[10]*zoom+z_to);
raydium_camera_data[0]=Matrix[2]*zoom+x_to;
raydium_camera_data[1]=Matrix[6]*zoom+y_to;
raydium_camera_data[2]=Matrix[10]*zoom+z_to;
raydium_camera_data[3]=180+delta_x*mult;
raydium_camera_data[4]=delta_y*mult;
raydium_camera_data[5]=0;
}

void raydium_camera_data_reset(void)
{
raydium_camera_data[0]=0;
raydium_camera_data[1]=0;
raydium_camera_data[2]=0;
raydium_camera_data[3]=0;
raydium_camera_data[4]=0;
raydium_camera_data[5]=0;
}

void raydium_camera_init(void)
{
int i;
raydium_camera_pushed=0;
raydium_camera_look_at_roll=0;
raydium_camera_path_reset_flag=1;
raydium_camera_rumble_amplitude=0;
raydium_camera_rumble_evolution=0;
raydium_camera_rumble_remaining=0;
//INITIALIZING freemove global variables
raydium_camera_freemove_speed = 0.1;
raydium_camera_freemove_sensibility = 3;
// push
raydium_camera_push_type=RAYDIUM_CAMERA_PUSH_NONE;
for(i=0;i<3;i++)
    {
    raydium_camera_push_current[i]=0;
    raydium_camera_push_end[i]=0;
    raydium_camera_push_slowness[i]=0;
    }
raydium_camera_data_reset();
raydium_camera_smooth_data_init(raydium_camera_smooth_data_primary);
raydium_camera_smooth_data_set(NULL); // same as raydium_camera_smooth_data_primary
}

void raydium_camera_push(int type, GLfloat *vect, GLfloat *slowness)
{
int i;

if(raydium_camera_push_type)
    raydium_log("warning: camera already pushed.");

raydium_camera_push_type=type;

for(i=0;i<3;i++)
    {
    raydium_camera_push_current[i]=0;
    raydium_camera_push_end[i]=vect[i];
    raydium_camera_push_slowness[i]=raydium_math_abs(slowness[i]);
    }
}

GLfloat *raydium_camera_push_internal_step(void)
{
int i;
signed char end=0;
signed char sign;
GLfloat dist;
static GLfloat fact[3];

for(i=0;i<3;i++)
    fact[i]=0;

if(!raydium_camera_push_type)
    return fact;

for(i=0;i<3;i++)
    {
    sign=(raydium_camera_push_end[i]>0?1:-1);
    dist=raydium_math_abs(raydium_camera_push_end[i])-raydium_camera_push_current[i];
    if(raydium_camera_push_slowness[i])
        fact[i]=dist/raydium_camera_push_slowness[i];
    fact[i]*=raydium_frame_time;
    raydium_camera_push_current[i]+=fact[i];
    if(fact[i]<0.00001) // quite arbitrary, no ? ... (FLT_EPSILON is too long to get)
        end++;
    fact[i]*=sign;
    }

if(end==3)
    raydium_camera_push_type=RAYDIUM_CAMERA_PUSH_NONE;

return fact;
}
