/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

// Hmm .. this tool is old, ugly ("interface" and code) but very useful.
#define NO_ODE_SUPPORT
#include "raydium/index.c"

char *version="0.351 Beta";
char *title="Raydium Modler";

#define round(x) (int)((x)>0?(x)+0.5:(x)-0.5)
#define MAX_CONTOUR 512
#define defSUBDIV 10

#define VIEW_XY 0
#define VIEW_ZY 1
#define VIEW_XZ 2

#define CONTOUR_EXTRUDE 0
#define CONTOUR_GATHER 1

////////////////////
GLfloat modl_view=VIEW_XY;
GLfloat modl_zoom=10;
GLfloat modl_grid=1;
GLfloat modl_axes=1;
GLfloat modl_tex=1;
char    modl_light=0;
char    modl_wire=0;
GLfloat pz=0;
//char entry[512];
char arg[16][512];
char cam_file[512];
GLfloat contour_x[MAX_CONTOUR];
GLfloat contour_y[MAX_CONTOUR];
GLfloat contour_z[MAX_CONTOUR];
GLuint contour_index=0;


float xcos(int i)
{
return( (float)cos(i*PI/180) );
}


float xsin(int i)
{
return( (float)sin(i*PI/180) );
}


void screen_to_VIEW(GLfloat sx, GLfloat sy,GLfloat sz,GLfloat *dx, GLfloat *dy, GLfloat *dz)
{
if(modl_view==VIEW_XY) { (*dx)=sx; (*dy)=sy; (*dz)=sz; }
if(modl_view==VIEW_ZY) { (*dx)=sz; (*dy)=sy; (*dz)=sx; }
if(modl_view==VIEW_XZ) { (*dx)=sx; (*dy)=sz; (*dz)=sy; }
}


void screen_to_local(GLfloat *x, GLfloat *y, GLfloat *z, char modl_view)
{
GLfloat lx,ly,lz;
lx=((float)raydium_mouse_x-((float)raydium_window_tx/(float)2)) * ((float)(modl_zoom*(float)2)/(float)raydium_window_tx);
ly=-((float)raydium_mouse_y-((float)raydium_window_ty/(float)2)) * ((float)(modl_zoom*(float)2)/(float)raydium_window_ty);
lz=pz;
//fx=round(x);
//fy=round(y);
if(modl_view==VIEW_XY) { (*x)=lx; (*y)=ly; (*z)=lz; }
if(modl_view==VIEW_ZY) { (*x)=lz; (*y)=ly; (*z)=lx; }
if(modl_view==VIEW_XZ) { (*x)=lx; (*y)=lz; (*z)=ly; }


}



void map_uv(GLfloat x1, GLfloat x2, GLfloat y1, GLfloat y2, GLfloat z1, GLfloat z2)
{
GLuint i;
GLfloat xdiff=x2-x1;
GLfloat ydiff=y2-y1;
GLfloat zdiff=z2-z1;


for(i=0;i<raydium_vertex_index;i++)
  if(raydium_vertex_texture[i]==raydium_texture_current_main)
    {
     if(raydium_vertex_normal_z(i)<-0.5 || raydium_vertex_normal_z(i)>0.5 )
     {
     raydium_vertex_texture_u(i)=(raydium_vertex_x(i)-x1)/xdiff;
     raydium_vertex_texture_v(i)=(raydium_vertex_y(i)-y1)/ydiff;
     }
    else if(raydium_vertex_normal_x(i)<-0.5 || raydium_vertex_normal_x(i)>0.5 )
     {
     raydium_vertex_texture_u(i)=(raydium_vertex_z(i)-z1)/zdiff;
     raydium_vertex_texture_v(i)=(raydium_vertex_y(i)-y1)/ydiff;
     }
    else
     {
     raydium_vertex_texture_u(i)=(raydium_vertex_x(i)-x1)/xdiff;
     raydium_vertex_texture_v(i)=(raydium_vertex_z(i)-z1)/zdiff;
     }
    }

}


void contour_circle(GLfloat ray,GLuint subdiv)
{
GLfloat inc=360.0/(float)subdiv;
GLfloat x,y,z;
GLfloat mx,my,mz;
GLfloat i;

screen_to_local(&mx,&my,&mz,VIEW_XY);

for(i=0;i<360;i+=inc)
{
x=xcos(i)*ray+round(mx);
y=xsin(i)*ray+round(my);
z=pz;
screen_to_VIEW(x,y,z,&x,&y,&z);
contour_x[contour_index]=x;
contour_y[contour_index]=y;
contour_z[contour_index]=z;
contour_index++;
}


}


void generate_contour_between(GLuint a, GLuint b, char type)
{
GLfloat ax,ay,az;
GLfloat bx,by,bz;
GLfloat lx,ly,lz;


ax=contour_x[a];
ay=contour_y[a];
az=contour_z[a];

bx=contour_x[b];
by=contour_y[b];
bz=contour_z[b];

if(type==CONTOUR_EXTRUDE)
{
 if(modl_view==VIEW_XY)
 {
 raydium_vertex_add(ax,ay,az);
 raydium_vertex_add(ax,ay,pz);
 raydium_vertex_add(bx,by,bz);

 raydium_vertex_add(bx,by,bz);
 raydium_vertex_add(ax,ay,pz);
 raydium_vertex_add(bx,by,pz);
 }

 if(modl_view==VIEW_ZY)
 {
 raydium_vertex_add(ax,ay,az);
 raydium_vertex_add(pz,ay,az);
 raydium_vertex_add(bx,by,bz);

 raydium_vertex_add(bx,by,bz);
 raydium_vertex_add(pz,ay,az);
 raydium_vertex_add(pz,by,bz);
 }

 if(modl_view==VIEW_XZ)
 {
 raydium_vertex_add(ax,ay,az);
 raydium_vertex_add(ax,pz,az);
 raydium_vertex_add(bx,by,bz);

 raydium_vertex_add(bx,by,bz);
 raydium_vertex_add(ax,pz,az);
 raydium_vertex_add(bx,pz,bz);
 }
}

if(type==CONTOUR_GATHER)
{
 screen_to_local(&lx,&ly,&lz,modl_view);
 raydium_vertex_add(ax,ay,az);
 raydium_vertex_add(lx,ly,lz);
 raydium_vertex_add(bx,by,bz);
}


}

void generate_contour(char type)
{
GLuint i;

if(!contour_index)
{ printf("No contour to extrude. Aborting.\n"); return; }

for(i=0;i<contour_index-1;i++)
generate_contour_between(i,i+1,type);

generate_contour_between(contour_index-1,0,type);


contour_index=0;
}


void resize(float xfact,float yfact,float zfact)
{
int i;

screen_to_VIEW(xfact,yfact,zfact,&xfact,&yfact,&zfact);

for(i=0;i<raydium_vertex_index;i++)
{
raydium_vertex_x(i)*=xfact;
raydium_vertex_y(i)*=yfact;
raydium_vertex_z(i)*=zfact;
}

}

void mirror(void)
{
int i;

for(i=0;i<raydium_vertex_index;i++)
{
//raydium_vertex_x[i]*=fact;
//raydium_vertex_y[i]*=fact;
raydium_vertex_z(i)*=-1;
}

}

void rotatez(GLfloat angle)
{
int i;
GLfloat x,y;

for(i=0;i<raydium_vertex_index;i++)
{
x=raydium_vertex_x(i);
y=raydium_vertex_y(i);

raydium_vertex_x(i)=x*raydium_math_cos(angle) - y*raydium_math_sin(angle);
raydium_vertex_y(i)=x*raydium_math_sin(angle) + y*raydium_math_cos(angle);
//raydium_vertex_z[i]*=-1;
}

}


void rotatey(GLfloat angle)
{
int i;
GLfloat x,z;

for(i=0;i<raydium_vertex_index;i++)
{
x=raydium_vertex_x(i);
z=raydium_vertex_z(i);

raydium_vertex_x(i)=x*raydium_math_cos(angle) - z*raydium_math_sin(angle);
raydium_vertex_z(i)=x*raydium_math_sin(angle) + z*raydium_math_cos(angle);
//raydium_vertex_z[i]*=-1;
}

}

void rotatex(GLfloat angle)
{
int i;
GLfloat y,z;

for(i=0;i<raydium_vertex_index;i++)
{
y=raydium_vertex_y(i);
z=raydium_vertex_z(i);

raydium_vertex_y(i)=y*raydium_math_cos(angle) - z*raydium_math_sin(angle);
raydium_vertex_z(i)=y*raydium_math_sin(angle) + z*raydium_math_cos(angle);
//raydium_vertex_z[i]*=-1;
}

}


void size_n_(int n, GLfloat *min, GLfloat *max, int start, int end)
{
int i;

if(!raydium_vertex_index) return;


(*max)=(*min)=raydium_vertex_arr[start*3+n];

for(i=start+1;i<end;i++)
    {
    if(raydium_vertex_arr[i*3+n]>(*max)) (*max)=raydium_vertex_arr[i*3+n];
    if(raydium_vertex_arr[i*3+n]<(*min)) (*min)=raydium_vertex_arr[i*3+n];
    }

}

void size_n(int n, GLfloat *min, GLfloat *max)
{
size_n_(n,min,max,0,raydium_vertex_index);
}


void size(void)
{
GLfloat min,max;

size_n(0,&min,&max);
raydium_log("%f < x < %f, diff= %f\n",min,max,raydium_math_abs(max-min));
size_n(1,&min,&max);
raydium_log("%f < y < %f, diff= %f\n",min,max,raydium_math_abs(max-min));
size_n(2,&min,&max);
raydium_log("%f < z < %f, diff= %f\n",min,max,raydium_math_abs(max-min));
}


void move(void)
{
int i;
GLfloat mx,my,mz;

screen_to_local(&mx,&my,&mz,modl_view);

for(i=0;i<raydium_vertex_index;i++)
{
raydium_vertex_x(i)+=round(mx);
raydium_vertex_y(i)+=round(my);
raydium_vertex_z(i)+=round(mz);
}
}

void move_by_(GLfloat x, GLfloat y, GLfloat z,int start, int end)
{
int i;
for(i=start;i<end;i++)
{
raydium_vertex_x(i)+=x;
raydium_vertex_y(i)+=y;
raydium_vertex_z(i)+=z;
}

}

void move_by(GLfloat x, GLfloat y, GLfloat z)
{
move_by_(x,y,z,0,raydium_vertex_index);
}


void center_(int start, int end)
{
GLfloat offx, offy, offz;
GLfloat min,max;

size_n_(0,&min,&max,start,end);
offx=-(max-((max-min)/2));

size_n_(1,&min,&max,start,end);
offy=-(max-((max-min)/2));

size_n_(2,&min,&max,start,end);
offz=-(max-((max-min)/2));

move_by_(offx,offy,offz,start,end);
}


void center(void)
{
// look if first object is animated
if(raydium_object_anims[0]>0)
    {
    int i,j,cpt,len;

    raydium_log("This is an animated object, I'll center frame by frame");
    raydium_log("Use \"*\" char to allow an anim to be centered. example: *jump");
    len=raydium_object_anim_len[0];
    cpt=raydium_object_anim_len[0];
    for(i=0;i<raydium_object_anims[0];i++)
      for(j=0;j<raydium_object_anim_end[0][i]-raydium_object_anim_start[0][i];j++)
        {
        if(raydium_object_anim_names[0][i][0]=='*')
            center_(cpt,cpt+len);
        cpt+=len;
        }
    }
else
    center_(0,raydium_vertex_index);
}

/*
void generate_contour_to_point(void)
{

GLuint i;

if(!contour_index)
{ printf("No contour to extrude. Aborting.\n"); return; }


for(i=0;i<contour_index-1;i++)
{
printf("contour: %f %f %f\n",contour_x[i],contour_y[i],contour_z[i]);
generate_contour_between(i,i+1);
}


}
*/


void modl_exit(void)
{
printf("Exit order received !\nI must go out :)\n");
//raydium_internal_dump();
exit(0);
}


void modl_help(void)
{
printf("Raydium Modler - CQFD Corp.\n");
printf("---------------------------\n");
printf("Usage: /command arguments");
printf("help: this screen\n");
printf("quit: exit\n");
printf("key-below-esc: gives prompt\n");
printf("F5 - F6 keys: previous - next texture\n");
printf("z (a): sets current depth to a value (you can use PGUP/PGDOWN or mouse wheel)\n");
printf("axe[s] [on|off]: enable/disable axes\n");
printf("wire [on|off]: enable/disable wireframe rendering\n");
printf("grid [on|off]: enable/disable grid drawing\n");
printf("tex[ture] [on|off]: enable/disable OS texture preview\n");
printf("contour: extrude current profile (made with middle mouse button)\n");
printf("contour [circle (ray) [sub]|gather|reset]: see doc\n");
printf("bind [erase] (file.tga): loads [or erase current texture whith] file.tga\n");
printf("load (file.tri): loads from .tri file\n");
printf("save (file.tri): saves to .tri file\n");
printf("saveup (file.tri): saves to .tri file and upload to repository\n");
printf("saveupall (file.tri): saves to .tri file and uploads to repository with dependencies\n");
printf("upload (file): uploads file to repository\n");
printf("uploadall (file): uploads file to repository with dependencies\n");
printf("list [filter]: list files from repository\n");
printf("savea (file.tri): saves to .tri file, sorting alpha textures at the end of file\n");
printf("move: move object by cursor position\n");
printf("move x y z: move object by x, y and z\n");
printf("rotate(x|y|z) angle: rotate object on x, y or z of angle\n");
printf("resize [a | (x y z)] resize all by a or by x,y,z (scale factor)\n");
printf("line [n]: change/reset line size\n");
printf("mirror: mirror object\n");
printf("size: return object sizes\n");
printf("cam (cam.cam) draw cam path file\n");
printf("center: center object (each frame if it's an anim)\n");
}


void draw_texture(void)
{
//glBindTexture(GL_TEXTURE_2D,raydium_texture_current);
//glColor4f(1.f,1.f,1.f,1.f);
raydium_rendering_internal_prepare_texture_render(raydium_texture_current_main);
glBegin(GL_QUADS);
glTexCoord2f(0,1);
glVertex3f(-modl_zoom,modl_zoom,0);
glTexCoord2f(0,0);
glVertex3f(-modl_zoom,modl_zoom-1,0);
glTexCoord2f(1,0);
glVertex3f(-modl_zoom+1,modl_zoom-1,0);
glTexCoord2f(1,1);
glVertex3f(-modl_zoom+1,modl_zoom,0);
glEnd();
raydium_rendering_internal_restore_render_state();
}

void draw_axes(void)
{
GLfloat size=modl_zoom/3;
GLfloat font=modl_zoom/30;

glBegin(GL_LINES);

glVertex3f(0,0,0);
glVertex3f(size,0,0);
glVertex3f(size+font,-font,0);
glVertex3f(size+font+font,font,0);
glVertex3f(size+font,font,0);
glVertex3f(size+font+font,-font,0);

glVertex3f(0,0,0);
glVertex3f(0,size,0);
glVertex3f(0,size+font+(font/2),0);
glVertex3f(0,size+font,0);
glVertex3f(0,size+font+(font/2),0);
glVertex3f(-font/2,size+font+font+(font/2),0);
glVertex3f(0,size+font+(font/2),0);
glVertex3f(font/2,size+font+font+(font/2),0);

glVertex3f(0,0,0);
glVertex3f(0,0,size);
glVertex3f(0,font,size+font-(font/2));
glVertex3f(0,font,size+font+(font/2));
glVertex3f(0,-font,size+font-(font/2));
glVertex3f(0,-font,size+font+(font/2));
glVertex3f(0,font,size+font-(font/2));
glVertex3f(0,-font,size+font+(font/2));


glEnd();

}


void draw_grid(void)
{
GLfloat x;
GLfloat y;
GLfloat z;



glBegin(GL_POINTS);
for(x=-modl_zoom;x<=modl_zoom;x++)
for(y=-modl_zoom;y<=modl_zoom;y++)
for(z=-modl_zoom;z<=modl_zoom;z++)
 {
  if( (modl_view==VIEW_XY && z==pz) ||
      (modl_view==VIEW_XZ && y==pz) ||
      (modl_view==VIEW_ZY && x==pz) ) glColor3f(1,0,0); else glColor3f(0.5,0.5,0.5);
 glVertex3f(x,y,z);
 }

glColor3f(0.7,0.7,0.7);
//glColor3f(0.0,0.0,0.0);
glEnd();
}


int cutcut(char *entry)
{
int i,j,n;

for(i=0,j=0,n=0;i<=strlen(entry);i++)
{
if(entry[i]==' ' || entry[i]==0) { arg[n][j]=0; j=0; if(n<16) n++; }
else arg[n][j++]=entry[i];
}
if(arg[0][0])
return n; else return 0;
}

void prompt(char *entry)
{
int argc;

//entry[strlen(entry)-1]=0;
argc=cutcut(entry);
if(!argc) return;


if(!strcmp(arg[0],"quit")) modl_exit();
if(!strcmp(arg[0],"help")) modl_help();
/*
if(!strcmp(arg[0],"map") && argc>=2)
    if(!strcmp(arg[1],"u") && argc>=4) map_u(atof(arg[2]),atof(arg[3]));
    else if(!strcmp(arg[1],"v") && argc>=4) map_v(atof(arg[2]),atof(arg[3]));
*/
if(!strcmp(arg[0],"contour") && argc==1) generate_contour(CONTOUR_EXTRUDE);
if(!strcmp(arg[0],"contour") && argc>=2)
      if(!strcmp(arg[1],"gather")) generate_contour(CONTOUR_GATHER);
if(!strcmp(arg[0],"contour") && argc>=2)
      if(!strcmp(arg[1],"reset")) contour_index=0;
if(!strcmp(arg[0],"contour") && argc>=2)
      if(!strcmp(arg[1],"circle") && argc>=3)
      {
      if(argc==3) contour_circle(atof(arg[2]),defSUBDIV);
      if(argc==4) contour_circle(atof(arg[2]),atof(arg[3]));
      }
if(!strcmp(arg[0],"light"))
      { if(argc>=2 && !strcmp(arg[1],"on")) modl_light=1; else modl_light=0; }
if(!strcmp(arg[0],"wire"))
      { if(argc>=2 && !strcmp(arg[1],"on")) modl_wire=1; else modl_wire=0; }
if(!strcmp(arg[0],"grid"))
      { if(argc>=2 && !strcmp(arg[1],"off")) modl_grid=0; else modl_grid=1; }
if(!strcmp(arg[0],"axe") || !strcmp(arg[0],"axes"))
      { if(argc>=2 && !strcmp(arg[1],"off")) modl_axes=0; else modl_axes=1; }
if(!strcmp(arg[0],"texture") || !strcmp(arg[0],"tex"))
      { if(argc>=2 && !strcmp(arg[1],"off")) modl_tex=0; else modl_tex=1; }
if(!strcmp(arg[0],"z") && argc==2) pz=atof(arg[1]);
if(!strcmp(arg[0],"resize") && argc==2) resize(atof(arg[1]),atof(arg[1]),atof(arg[1]));
if(!strcmp(arg[0],"resize") && argc==4) resize(atof(arg[1]),atof(arg[2]),atof(arg[3]));
if(!strcmp(arg[0],"move") && argc==1) move();
if(!strcmp(arg[0],"move") && argc==4) move_by(atof(arg[1]),atof(arg[2]),atof(arg[3]));
if(!strcmp(arg[0],"size")) size();
if(!strcmp(arg[0],"rotatex") && argc==2) rotatex(atof(arg[1]));
if(!strcmp(arg[0],"rotatez") && argc==2) rotatez(atof(arg[1]));
if(!strcmp(arg[0],"rotatey") && argc==2) rotatey(atof(arg[1]));
if(!strcmp(arg[0],"mirror")) mirror();
if(!strcmp(arg[0],"line")  && argc==1) glLineWidth(1.0);
if(!strcmp(arg[0],"line")  && argc==2) glLineWidth(atof(arg[1]));
if(!strcmp(arg[0],"save")  && argc==2) dump_vertex_to(arg[1]);
if(!strcmp(arg[0],"saveup")&& argc==2) { dump_vertex_to(arg[1]); raydium_rayphp_repository_file_put(arg[1],0); }
if(!strcmp(arg[0],"saveupall")&& argc==2) { dump_vertex_to(arg[1]); raydium_rayphp_repository_file_put(arg[1],1); }
if(!strcmp(arg[0],"upload")&& argc==2) raydium_rayphp_repository_file_put(arg[1],0);
if(!strcmp(arg[0],"uploadall")&& argc==2) raydium_rayphp_repository_file_put(arg[1],1);
if(!strcmp(arg[0],"list") && argc==1) raydium_rayphp_repository_file_list("");
if(!strcmp(arg[0],"list") && argc==2) raydium_rayphp_repository_file_list(arg[1]);
if(!strcmp(arg[0],"savea") && argc==2) dump_vertex_to_alpha(arg[1]);
if(!strcmp(arg[0],"load")  && argc==2) read_vertex_from(arg[1]);
if(!strcmp(arg[0],"bind")  && argc==2) raydium_texture_load(arg[1]);
if(!strcmp(arg[0],"bind")  && argc==3 && !strcmp(arg[1],"erase")) raydium_texture_load_erase(arg[2],raydium_texture_current_main);
if(!strcmp(arg[0],"cam") && argc==2) strcpy(cam_file,arg[1]);
if(!strcmp(arg[0],"center")) center();
}



void mouse_n_keys_event(void)
{
GLfloat rotx,roty;
GLfloat x,y,z;
if(raydium_key_last==1032) { map_uv(-modl_zoom,modl_zoom,-modl_zoom,modl_zoom,-modl_zoom,modl_zoom); }
if(raydium_key_last==1045)  modl_zoom--;
if(raydium_key_last==1043)  modl_zoom++;
if(raydium_key_last==1027)  modl_exit();
/*if(raydium_key_last==1)*/
//prompt();
if(raydium_key_last==108) modl_view=VIEW_XY;
if(raydium_key_last==106) modl_view=VIEW_ZY;
if(raydium_key_last==1127) modl_view=VIEW_XZ;
if(raydium_key_last==104) pz++;
if(raydium_key_last==105) pz--;
if(raydium_key_last==5 && raydium_texture_current_main>1) raydium_texture_current_main--;
if(raydium_key_last==6 && raydium_texture_current_main<raydium_texture_index-1) raydium_texture_current_main++;
if(raydium_key_last==1008 && raydium_vertex_index>=3) raydium_vertex_index-=3;
if(raydium_key_last==8) raydium_normal_smooth_all();
if(raydium_key_last==9) raydium_normal_regenerate_all();
if(raydium_key_last==10) raydium_normal_restore_all();

if(raydium_mouse_button[0])
{
rotx=((float)raydium_mouse_x-((float)raydium_window_tx/(float)2)) * ((float)360/(float)raydium_window_tx);
roty=((float)raydium_mouse_y-((float)raydium_window_ty/(float)2)) * ((float)360/(float)raydium_window_ty);
 if(modl_view==VIEW_XY)
 {
 glRotatef(roty,1,0,0);
 glRotatef(rotx,0,1,0);
 }

 if(modl_view==VIEW_ZY)
 {
 glRotatef(roty,0,0,1);
 glRotatef(rotx,0,1,0);
 }

 if(modl_view==VIEW_XZ)
 {
 glRotatef(roty,1,0,0);
 glRotatef(rotx,0,0,1);
 }

//printf("-- %f %f %i\n",(raydium_mouse_x-(window_tx/2)),(window_tx/2),raydium_mouse_x);
//printf("%i %i\n",raydium_mouse_x,mousey);
}


if(raydium_mouse_click==2)
{
screen_to_local(&x,&y,&z,modl_view);
printf("vertex add at round(%f,%f,%f)\n",x,y,z);
raydium_vertex_add(round(x),round(y),round(z));
}

if(raydium_mouse_click==3)
{
screen_to_local(&x,&y,&z,modl_view);
printf("contour(%i) add at round(%f,%f,%f)\n",contour_index,x,y,z);
contour_x[contour_index]=round(x);
contour_y[contour_index]=round(y);
contour_z[contour_index++]=round(z);
}


if(modl_zoom<1) modl_zoom=1;
}



void draw_contour(void)
{
GLuint i;

glColor3f(0,1,0);
glBegin(GL_LINE_LOOP);
for(i=0;i<contour_index;i++)
glVertex3f(contour_x[i],contour_y[i],contour_z[i]);

glEnd();
glColor3f(1,1,1);

}


void update_vars(void)
{
raydium_projection=RAYDIUM_PROJECTION_ORTHO;
raydium_projection_near=-modl_zoom;
raydium_projection_far=modl_zoom;
raydium_projection_left=-modl_zoom;
raydium_projection_right=modl_zoom;
raydium_projection_bottom=-modl_zoom;
raydium_projection_top=modl_zoom;
raydium_window_resize_callback(raydium_window_tx,raydium_window_ty);
//glutWarpPointer(0,0);
}

void light_update(void)
{
GLfloat lx,ly,lz;
screen_to_local(&lx,&ly,&lz,modl_view);
raydium_light_position[0][0]=lx;
raydium_light_position[0][1]=ly;
raydium_light_position[0][2]=lz;
raydium_light_update_position(0);
}



void display(void)
{
raydium_clear_frame();

glLoadIdentity();
update_vars();
light_update();

if(modl_tex) draw_texture();
if(modl_view==VIEW_ZY) glRotatef(90,0,1,0);
if(modl_view==VIEW_XZ) glRotatef(-90,1,0,0);

mouse_n_keys_event();
//glRotatef(90,0,1,0);

glDisable(GL_TEXTURE_2D);
if(modl_grid) draw_grid();
if(modl_axes) draw_axes();
glColor3f(1.0,1.0,1.0);
draw_contour();
glEnable(GL_TEXTURE_2D);

//glutSolidTeapot(1);
if(modl_light) raydium_light_enable();
if(modl_wire) { glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); }
else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//glEdgeFlag(GL_TRUE);
raydium_rendering();

if(cam_file[0]!=0)
    raydium_camera_path_draw_name(cam_file);

raydium_rendering_finish();
raydium_light_disable();
}



int main(int argc, char **argv)
{
char window[256];
sprintf(window,"%s - %s",title,version);

cam_file[0]=0;

raydium_init_args(argc,argv);
raydium_window_create(600,600,RAYDIUM_RENDERING_WINDOW,window);
raydium_background_color_change(0,0,0,1);
raydium_console_gets_callback=prompt;

raydium_mouse_show();
raydium_texture_filter_change(RAYDIUM_TEXTURE_FILTER_TRILINEAR);
//raydium_texture_current=1;

raydium_light_on(0);
printf("Tip: Press key-below-esc to get prompt.\n");
raydium_callback(&display);
return(0);
}
