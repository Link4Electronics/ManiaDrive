/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

// This file does not exist. You never see it. ("drawing with a laser with
// a webcam based app")

#include "raydium/index.c"

unsigned char drawing[512*512*4];


void set_pixel(int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char *img)
{
int off;

off=(y*512+x)*4;
img[off+0]=r;
img[off+1]=g;
img[off+2]=b;
img[off+3]=255;
}

int sinal(int res)
{
  if (res<0)
    return -1;
  else if(res==0)
      return 0;
    else
      return 1;
}

void bresenham(int x1, int y1, int x2, int y2, unsigned char r, unsigned char g, unsigned char b, unsigned char *img)
{
  int x,y;
  int s1,s2;
  int dx,dy;
  int e,i;
  int temp, troca;
  
  x=x1;
  y=y1;
  dx=abs(x2-x1);
  dy=abs(y2-y1);
  
  s1=sinal(x2-x1);
  s2=sinal(y2-y1);

  if (dy>dx) 
  {
        temp=dx;
        dx=dy;
        dy=temp;
        troca=1;
  }
  else
  {
        troca=0;
  }

  e=2*dy-dx;

  for (i=1; i<=dx; i++)
  {  
        set_pixel(x,y,r,g,b,img);
  
    while (e>=0) 
    {
        if (troca==1)
          x=x+s1;
        else
          y=y+s2;
  
      e=e-2*dx;
    }
    
    if (troca==1)
        y=y+s2;
    else
        x=x+s1;

  e=e+2*dy;

  }
}



void update_drawing(GLfloat x, GLfloat y)
{
int ix,iy;
static int lx=0,ly=0;

//y=1-y;
//x=1-x;

if(raydium_key_last==1032)
    memset(drawing,0,512*512*4);

ix=x*512;
iy=y*512;
/*
off=(iy*512+ix)*4;
printf("%i\n",off);
drawing[off+0]=0;
drawing[off+1]=255;
drawing[off+2]=0;
drawing[off+3]=255;
*/
bresenham(lx,ly,ix,iy,0,255,0,drawing);
lx=ix;
ly=iy;

raydium_live_texture_refresh_name("drawing.tga");
}



int data_callback(unsigned char *data, int tx, int ty, int bpp)
{
int x,y;
int i,j;
int max;
int where;
GLfloat wherex,wherey;

max=200;
where=-1;
bpp = bpp/8;
for(y=0;y<ty;y++)
  for(x=0;x<tx;x++)
    {
    i=y*tx+x;

    j=(data[i*bpp]*3)-data[i*bpp+1]-data[i*bpp+2];


    if(j>max)
        {
        max=j;
        where=i;
        wherex=x/(float)tx;
        wherey=y/(float)ty;
        }
    }

if(where>=0)
    {
    printf("max = %i\n",max);
    data[where*bpp]=0;
    data[where*bpp+1]=255;
    data[where*bpp+2]=0;
    update_drawing(wherex,wherey);
    }

return 1;
}


GLfloat sun[]={1.0,0.9,0.5,1.0};

GLfloat camx=3.01;
GLfloat camy=3;
GLfloat camz=0;


void display(void)
{
static float rx,ry,rz;
raydium_joy_key_emul();

camx+=raydium_joy_x*0.1;
camy+=raydium_joy_y*0.1;
 
// camx+=0.01;

if(raydium_key[GLUT_KEY_PAGE_DOWN]) camz-=0.1;
if(raydium_key[GLUT_KEY_PAGE_UP]  ) camz+=0.1;

if(raydium_key_last==1027) exit(0);

rx+=5*raydium_frame_time;
ry+=5*raydium_frame_time*0.1;
rz+=5*raydium_frame_time*0.01;

raydium_clear_frame();
raydium_camera_look_at(camx,camy,camz,0,0,0);
//raydium_camera_look_at(0,0.01,3,0,0,0);


raydium_live_texture_mask_name("webcam.tga",1);
raydium_live_texture_mask_name("drawing.tga",1);

//raydium_object_draw_name("plane.tri");

raydium_rendering_finish();
}

int main(int argc, char **argv)
{
int device;

raydium_init_args(argc,argv);
raydium_window_create(640,480,RAYDIUM_RENDERING_WINDOW,"LiveApi test");
raydium_texture_filter_change(RAYDIUM_TEXTURE_FILTER_TRILINEAR);
raydium_projection_near=0.01;
raydium_projection_far=1000;
raydium_projection_fov=60;
raydium_window_view_update();
raydium_light_on(0);
memcpy(raydium_light_color[0],sun,raydium_internal_size_vector_float_4);
raydium_light_intensity[0]=100000;

raydium_light_position[0][0]=50;
raydium_light_position[0][1]=150;
raydium_light_position[0][2]=200;

raydium_light_update_all(0);
raydium_background_color_change(sun[0],sun[1],sun[2],sun[3]);
raydium_fog_disable();

device=raydium_live_video_open_auto();
//device=raydium_live_video_open(RAYDIUM_LIVE_DEVICE_AUTO,640,480);
raydium_live_texture_video(device,"webcam.tga");
raydium_live_texture_refresh_callback_set_name("webcam.tga",data_callback);

//device=raydium_live_video_open("/dev/video1",352,288);
//raydium_live_texture_video(device,"drawing.tga");

memset(drawing,0,512*512*4);
raydium_live_texture_create("drawing.tga",drawing,512,512,32);


raydium_callback(&display);

return 0;
}
