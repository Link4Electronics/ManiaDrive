/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

#ifndef DONT_INCLUDE_HEADERS
#include "index.h"
#else
#include "headers/internal.h"
#endif 

void raydium_file_log_fopen_display(void);
void raydium_network_internal_dump(void);

void raydium_internal_dump(void)
{
GLuint i,j,a;
int si;

if(!raydium_log_atexit_dump)
    return;

if(raydium_init_cli_option("regs",NULL))
    raydium_register_dump();

raydium_log("Internal buffers:");
raydium_log("-----------------");
raydium_log("Total of %i vertex(s) loaded:",raydium_vertex_index);

//for(i=0;i<raydium_texture_index;i++)
for(i=1;i<RAYDIUM_MAX_TEXTURES;i++)
    {
    if(raydium_texture_used[i])
    {
        for(j=0,a=0;j<raydium_vertex_index;j++)
            if(raydium_vertex_texture[j]==i) a++;
                raydium_log("Texture %i: %i vert, as \"%s\" (%.2f MB)",i,a,raydium_texture_name[i],raydium_texture_memory[i]/1024.f/1024.f);
    }
    }
raydium_log("Estimated total: %.2f MB used for textures.",raydium_texture_used_memory/1024.f/1024.f);

raydium_log("Using %i object(s):",raydium_object_index);
for(si=0;si<raydium_object_index;si++)
    {
    a=raydium_object_end[si]-raydium_object_start[si];
    raydium_log("Object %i: %i vert, as \"%s\"",si,a,raydium_object_name[si]);
    }

if(raydium_network_mode!=RAYDIUM_NETWORK_MODE_NONE)
    raydium_network_internal_dump();

if(raydium_init_cli_option("files",NULL)) raydium_file_log_fopen_display();

// raydium_log(".end of dump");
}

// 0 : Projection
// 1 : Model
void raydium_internal_dump_matrix(int n)
{
GLfloat tmp[16];
char str[80];
char str2[80];
int i,j;

if(n==0) glGetFloatv(GL_PROJECTION_MATRIX,tmp);
if(n==1) glGetFloatv(GL_MODELVIEW_MATRIX,tmp);

raydium_log("Matrix [4x4] :");
str[0]=0;
for(i=j=0;i<16;i++,j++)
 {
  sprintf(str2,"| % 10.2f ",tmp[i]);
  strcat(str,str2);
  if(j>=3)
   {
   j=-1;
   raydium_log("%s",str);
   str[0]=0;
   }
 }
raydium_log(".end");
}
