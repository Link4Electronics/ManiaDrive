/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

#ifndef DONT_INCLUDE_HEADERS
#include "index.h"
#else
#include "headers/file_tri.h"
#endif

// WARNING: most functions of this file are not part of Raydium yet !
// So, be carefull with functions without "raydium_file" prefix.


#define DONT_SAVE_DUMMY_TEXTURE

void dump_vertex_to(char *filename)
{
FILE *fp;
//GLuint tex;
GLuint i;
char text[256];
char bl;

fp=raydium_file_fopen(filename,"wt");
if(!fp) { printf("cannot write to file \"%s\", fopen() failed\n",filename); return; }

fprintf(fp,"1\n");

/*
for(tex=0;tex<raydium_texture_index;tex++)
{
// fprintf(fp,"%s\n",raydium_texture_name[tex]);
// fprintf(fp,"%i\n",j);
 for(i=0;i<raydium_vertex_index;i++)
  if(raydium_vertex_texture[i]==tex)
  fprintf(fp,"%f %f %f %f %f %s\n",
  raydium_vertex_x[i],raydium_vertex_y[i],raydium_vertex_z[i],
  raydium_vertex_texture_u[i],raydium_vertex_texture_v[i],raydium_texture_name[tex]);
}
*/
for(bl=0;bl<2;bl++)
{
 for(i=0;i<raydium_vertex_index;i++)
 if( (raydium_texture_blended[raydium_vertex_texture[i]]?1:0) == bl)
 {
  if(raydium_vertex_texture_multi[i])
  {
   sprintf(text,"%s;%f|%f|%s",raydium_texture_name[raydium_vertex_texture[i]],
                              raydium_vertex_texture_multi_u(i),
                              raydium_vertex_texture_multi_v(i),
                              raydium_texture_name[raydium_vertex_texture_multi[i]]);
  }
  else
  strcpy(text,raydium_texture_name[raydium_vertex_texture[i]]);

#ifdef DONT_SAVE_DUMMY_TEXTURE
  if(raydium_vertex_texture[i])
#endif
  fprintf(fp,"%f %f %f %f %f %f %f %f %s\n",
  raydium_vertex_x(i),raydium_vertex_y(i),raydium_vertex_z(i),
  raydium_vertex_normal_visu_x(i), raydium_vertex_normal_visu_y(i), raydium_vertex_normal_visu_z(i),
  raydium_vertex_texture_u(i),raydium_vertex_texture_v(i),
  text);
 }
}

fclose(fp);
printf("saved.\n");
}

// sorts alpha textures
void dump_vertex_to_alpha(char *filename)
{
FILE *fp;
GLuint tex;
GLuint i;
char text[256];
int bl;


raydium_log("WARNING: 'dump_vertex_to_alpha' function is deprecated, since regular 'dump_vertex_to' function now sorts alpha textures");


fp=raydium_file_fopen(filename,"wt");
if(!fp) { printf("cannot write to file \"%s\", fopen() failed\n",filename); return; }
fprintf(fp,"1\n");

for(bl=0;bl<2;bl++)
{
//for(tex=0;tex<raydium_texture_index;tex++)
for(tex=1;tex<RAYDIUM_MAX_TEXTURES;tex++)
{
    if(raydium_texture_used[tex])
    {
        if( (raydium_texture_blended[tex]?1:0) == bl)
        {
          printf("%s\n",raydium_texture_name[tex]);
          strcpy(text,raydium_texture_name[tex]);
        // fprintf(fp,"%i\n",j);
         for(i=0;i<raydium_vertex_index;i++)
          if(raydium_vertex_texture[i]==tex )
          fprintf(fp,"%f %f %f %f %f %f %f %f %s\n",
          raydium_vertex_x(i),raydium_vertex_y(i),raydium_vertex_z(i),
          raydium_vertex_normal_visu_x(i), raydium_vertex_normal_visu_y(i), raydium_vertex_normal_visu_z(i),
          raydium_vertex_texture_u(i),raydium_vertex_texture_v(i),
          text);
        }
    }
}
printf("----\n");
}


fclose(fp);
printf("saved.\n");
}


#define MULTI_SEP ';'
#define MULTI_UVSEP '|'
#define ENVMAP_SEP '#'
int raydium_file_set_textures(char *name)
{
char *sep;
char *sep2=NULL;
char *sep_env;
char texname[RAYDIUM_MAX_NAME_LEN];
int i;

sep = strchr(name,MULTI_SEP);
sep_env = strchr(name,ENVMAP_SEP);
if(sep) sep2 = strchr(sep+1,MULTI_UVSEP);

for(i=0;i<(RAYDIUM_RENDER_MAX_TEXUNITS-1);i++)
        raydium_texture_current_env[i]=0;

if(sep_env)
{
  int count=0;
  char todo[RAYDIUM_MAX_NAME_LEN];
  char part1[RAYDIUM_MAX_NAME_LEN];
  char part2[RAYDIUM_MAX_NAME_LEN];

  for(i=0;name[i];i++)
        if(name[i]==ENVMAP_SEP)
           count++;

  if(count>=RAYDIUM_RENDER_MAX_TEXUNITS)
        {
        raydium_log("ERROR: file_tri: this file request %i texunits, only %i are available here",count+1,RAYDIUM_RENDER_MAX_TEXUNITS);
        return 0;
        }

  count=0;
  strcpy(todo,name);

  raydium_parser_cut(todo,part1,part2,'#');
  raydium_texture_current_set_name(part1); // main texture
  strcpy(todo,part2);

  while(raydium_parser_cut(todo,part1,part2,'#'))
        {
        raydium_texture_current_env[count++]=raydium_texture_find_by_name(part1);
        strcpy(todo,part2);
        }
  raydium_texture_current_env[count++]=raydium_texture_find_by_name(todo);

  return 3;
}



// 2 textures + 1 uv
if(sep && sep2)
{
  sscanf(sep+1,"%f|%f|%s\n", &raydium_texture_current_multi_u,
                            &raydium_texture_current_multi_v,
                            texname);
  raydium_texture_current_multi=raydium_texture_find_by_name(texname);
  *sep=0;
  raydium_texture_current_set_name(name);
  *sep=MULTI_SEP;
  return 2;
}


// 2 textures, but 0 uv
if(sep && !sep2)
{
  raydium_texture_current_multi=raydium_texture_find_by_name(sep+1);
  *sep=0;
  raydium_texture_current_set_name(name);
  *sep=MULTI_SEP;
  raydium_texture_current_multi_u=-99999;
  raydium_texture_current_multi_v=-99999;
  return 1;
}

// 1 texture and 0 uv
if(!sep && !sep2)
{
  raydium_texture_current_multi=0;
  raydium_texture_current_set_name(name);
  return 0;
}

return -1; // should never reach this
}



int read_vertex_from(char *filename)
{
GLfloat x,y,z,nx,ny,nz,u,v;
int i,ii;
unsigned int ui;
GLuint save;
GLint visu;
FILE *fp;
char name[RAYDIUM_MAX_NAME_LEN];

fp=raydium_file_fopen(filename,"rt");
if(!fp) { printf("cannot read from file \"%s\", fopen() failed\n",filename); return 0; }
fscanf(fp,"%i\n",&visu);


raydium_log("Object: loading \"%s\", version %i",filename,visu);

// test here version 2 (anims)
if(visu==2)
    {
    int j,k;
    fscanf(fp,"%i %i\n",&j,&k);

    if(j>RAYDIUM_MAX_OBJECT_ANIMS)
        {
        raydium_log("object: too much anims for this file ! (%i max)",RAYDIUM_MAX_OBJECT_ANIMS);
        j=RAYDIUM_MAX_OBJECT_ANIMS; // will no work ;) (fixme)
        }

    raydium_object_anims[raydium_object_index]=j;
    raydium_object_anim_len[raydium_object_index]=k;
    raydium_object_anim_instance_current[raydium_object_index]=0;
    raydium_object_anim_default_anim[raydium_object_index]=0;

    for(ii=0;ii<RAYDIUM_MAX_OBJECT_ANIM_INSTANCES;ii++)
        {
        raydium_object_anim_current[raydium_object_index][ii]=0;
        raydium_object_anim_frame_current[raydium_object_index][ii]=0;
        raydium_object_anim_previous[raydium_object_index][ii]=-1;
        raydium_object_anim_frame_previous[raydium_object_index][ii]=0;
        raydium_object_anim_frame_previous_timeout[raydium_object_index][ii]=0;
        raydium_object_anim_punctually_flag[raydium_object_index][ii]=-1;
        }


    for(i=0;i<raydium_object_anims[raydium_object_index];i++)
        {
        fscanf(fp,"%i %i %s\n",&j,&k,name);
        raydium_object_anim_start[raydium_object_index][i]=j;
        raydium_object_anim_end[raydium_object_index][i]=k;
        raydium_object_anim_automatic_factor[raydium_object_index][i]=0;
        strcpy(raydium_object_anim_names[raydium_object_index][i],name);
        }

    // build "current transformed model" space
    for(ui=0;ui<raydium_object_anim_len[raydium_object_index];ui++)
        {
        raydium_vertex_add(0,0,0);
        raydium_vertex_texture[raydium_vertex_index-1]=0;
        }

    fscanf(fp,"%i\n",&visu);
    raydium_log("object: anim: %i frame(s) with %i vertice per frame (ver %i)",raydium_object_anims[raydium_object_index],raydium_object_anim_len[raydium_object_index],visu);
    }
// ...

save=raydium_texture_current_main;
i=0;

if(visu>0)
{
 while( fscanf(fp,"%f %f %f %f %f %f %f %f %s\n",&x,&y,&z,&nx,&ny,&nz,&u,&v,name)!=EOF )
 {
  raydium_file_set_textures(name);
  raydium_vertex_uv_normals_add(x,y,z,nx,ny,nz,u,v);
  i++;
 }
}
else if(visu==0)
{
 while( fscanf(fp,"%f %f %f %f %f %s\n",&x,&y,&z,&u,&v,name)!=EOF )
 {
  raydium_file_set_textures(name);
  raydium_vertex_uv_add(x,y,z,u,v);
  i++;
 }
}
else if(visu<0)
{
 while( fscanf(fp,"%f %f %f %s\n",&x,&y,&z,name)!=EOF )
 {
  raydium_file_set_textures(name);
  raydium_vertex_add(x,y,z);
  i++;
 }

}

if(i%3)
    {
    raydium_log("ERROR: file: tri: object %s ... must be *3 !",filename);
    raydium_log("This object is invalid, and current engine state undefined.");
	raydium_log("***********************************************************");
    // and generate dummy vertices ?
    }

fclose(fp);

raydium_texture_current_multi=0;
raydium_texture_current_set(save);
//printf("loaded.\n");
return 1;
}
