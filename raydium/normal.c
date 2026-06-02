/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

#ifndef DONT_INCLUDE_HEADERS
#include "index.h"
#else
#include "headers/normal.h"
#endif

void raydium_normal_generate_lastest_triangle(int default_visu)
{
GLfloat vect[2][3];
GLfloat norm[3];
GLfloat len;
int i;
GLuint index=raydium_vertex_index;

// let's find "vectors" of the triangle's plane ...
vect[0][0]=
 raydium_vertex_x(index-1) -
 raydium_vertex_x(index-2) ;
vect[0][1]=
 raydium_vertex_y(index-1) -
 raydium_vertex_y(index-2) ;
vect[0][2]=
 raydium_vertex_z(index-1) -
 raydium_vertex_z(index-2) ;

vect[1][0]=
 raydium_vertex_x(index-1) -
 raydium_vertex_x(index-3) ;
vect[1][1]=
 raydium_vertex_y(index-1) -
 raydium_vertex_y(index-3) ;
vect[1][2]=
 raydium_vertex_z(index-1) -
 raydium_vertex_z(index-3) ;

// ... and now, the normal ...

norm[0]=vect[0][1]*vect[1][2] - vect[0][2]*vect[1][1];
norm[1]=vect[0][2]*vect[1][0] - vect[0][0]*vect[1][2];
norm[2]=vect[0][0]*vect[1][1] - vect[0][1]*vect[1][0];

// ... which we normalize.

len=sqrt(norm[0]*norm[0] + norm[1]*norm[1] + norm[2]*norm[2]);

for(i=1;i<=3;i++)
{
raydium_vertex_normal_x(index-i)=-norm[0]/len;
if(default_visu) raydium_vertex_normal_visu_x(index-i)=raydium_vertex_normal_x(index-i);
}

for(i=1;i<=3;i++)
{
raydium_vertex_normal_y(index-i)=-norm[1]/len;
if(default_visu) raydium_vertex_normal_visu_y(index-i)=raydium_vertex_normal_y(index-i);
}

for(i=1;i<=3;i++)
{
raydium_vertex_normal_z(index-i)=-norm[2]/len;
if(default_visu) raydium_vertex_normal_visu_z(index-i)=raydium_vertex_normal_z(index-i);
}

//raydium_log("norm [%f,%f,%f]/%f",norm[0],norm[1],norm[2],len);
}


void raydium_normal_generate_lastest_tangent(void)
{
GLuint index=raydium_vertex_index;
GLfloat v01x,v01y,v01z;
GLfloat v02x,v02y,v02z;
GLfloat du01,dv01;
GLfloat du02,dv02;
GLfloat tx,ty,tz;
float s;
int i;

// index | -1 -2 -3
// vert  |  2  1  0

// vector from vert0 to vert1 (3D)
v01x=raydium_vertex_x(index-2) - raydium_vertex_x(index-3);
v01y=raydium_vertex_y(index-2) - raydium_vertex_y(index-3);
v01z=raydium_vertex_z(index-2) - raydium_vertex_z(index-3);

// vector from vert0 to vert2 (3D)
v02x=raydium_vertex_x(index-1) - raydium_vertex_x(index-3);
v02y=raydium_vertex_y(index-1) - raydium_vertex_y(index-3);
v02z=raydium_vertex_z(index-1) - raydium_vertex_z(index-3);

// vector from uv0 to uv1 (2D)
du01=raydium_vertex_texture_u(index-2) - raydium_vertex_texture_u(index-3);
dv01=raydium_vertex_texture_v(index-2) - raydium_vertex_texture_v(index-3);

// vector from uv0 to uv2 (2D)
du02=raydium_vertex_texture_u(index-1) - raydium_vertex_texture_u(index-3);
dv02=raydium_vertex_texture_v(index-1) - raydium_vertex_texture_v(index-3);

// 1/(u*v' - u'*v)
s=1/(du01*dv02 - du02*dv01);

// tangent ...
tx=s*(dv02*v01x + (-dv01)*v02x);
ty=s*(dv02*v01y + (-dv01)*v02y);
tz=s*(dv02*v01z + (-dv01)*v02z);

// ... and normalize:
s=sqrt(tx*tx + ty*ty + tz*tz);
tx/=s;
ty/=s;
tz/=s;

// printf("tangent = [%f %f %f]\n",tx,ty,tz);

for(i=1;i<=3;i++)
    {
    raydium_vertex_tangent_x(index-i)=tx;
    raydium_vertex_tangent_y(index-i)=ty;
    raydium_vertex_tangent_z(index-i)=tz;
    }
}


void raydium_normal_restore_all(void)
{
GLuint i;
for(i=0;i<raydium_vertex_index;i++)
 {
 raydium_vertex_normal_visu_x(i)=raydium_vertex_normal_x(i);
 raydium_vertex_normal_visu_y(i)=raydium_vertex_normal_y(i);
 raydium_vertex_normal_visu_z(i)=raydium_vertex_normal_z(i);
 }
raydium_log("normal: Normals restored.");
}

void raydium_normal_regenerate_all(void)
{
GLuint save=raydium_vertex_index;

if(save<3) return;

for(raydium_vertex_index=3;raydium_vertex_index<=save;raydium_vertex_index+=3)
    raydium_normal_generate_lastest_triangle(1);

raydium_vertex_index=save;
raydium_log("normal: Normals regenerated.");
}

void raydium_normal_internal_smooth_generic(char *type,GLuint from, GLuint to,GLfloat *in,GLfloat *out)
{
GLuint i,j;
GLfloat x,y,z,l;
GLfloat sum_x;
GLfloat sum_y;
GLfloat sum_z;
GLuint debug_time;
char *tag; // 1 means "already done", 2 means "used for last search"
GLuint total;

total=to-from;

tag=malloc(total);
if(!tag) { raydium_log("normal: Not enought memory for %s smoothing, giving up.",type); return; }
memset(tag,0,total);

debug_time=0;

for(i=from;i<to;i++)
{
if(tag[i-from]) continue;
x=raydium_vertex_x(i);
y=raydium_vertex_y(i);
z=raydium_vertex_z(i);
sum_x=0;
sum_y=0;
sum_z=0;

for(j=from;j<to;j++)
 {
 if(raydium_vertex_x(j)==x && raydium_vertex_y(j)==y && raydium_vertex_z(j)==z)
  {
  sum_x+=in[i*3+0];
  sum_y+=in[i*3+1];
  sum_z+=in[i*3+2];
  tag[j-from]=2;
  }
 }
l=sqrt(sum_x*sum_x + sum_y*sum_y + sum_z*sum_z);
sum_x/=l;
sum_y/=l;
sum_z/=l;

for(j=from;j<to;j++)
if(tag[j-from]==2)
 {
 out[j*3+0]=sum_x;
 out[j*3+1]=sum_y;
 out[j*3+2]=sum_z;
 tag[j-from]=1;
 }

debug_time++;
//if(debug_time>100) { raydium_log("normal: smoothing: %i/%i",i-from,total); debug_time=0; }
}

free(tag);
raydium_log("normal: %s smoothing done.",type);
}

void raydium_normal_smooth_from_to(GLuint from, GLuint to)
{
raydium_normal_internal_smooth_generic("normal",from,to,raydium_vertex_normal_arr,
                                        raydium_vertex_normal_visu_arr);
}


void raydium_normal_smooth_all(void)
{
raydium_normal_smooth_from_to(0,raydium_vertex_index);
}

void raydium_normal_tangent_smooth_from_to(GLuint from, GLuint to)
{
raydium_normal_internal_smooth_generic("normal",from,to,raydium_vertex_tangent_arr,
                                       raydium_vertex_tangent_arr);
}

void raydium_normal_tangent_smooth_all(void)
{
raydium_normal_tangent_smooth_from_to(0,raydium_vertex_index);
}

