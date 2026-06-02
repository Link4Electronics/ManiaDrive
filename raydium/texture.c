/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

#ifndef DONT_INCLUDE_HEADERS
#include "index.h"
#else
#include "headers/texture.h"
#endif

#include "live.h"

// proto
int raydium_init_cli_option(char *option, char *value);
FILE *raydium_file_fopen(char *path, char *mode);
void raydium_file_basename(char *dest,char *from);


signed char raydium_texture_size_is_correct(GLuint size)
{
int i;

if(raydium_window_mode==RAYDIUM_RENDERING_NONE)
    return 1;

// must be a power of 2 and <= to raydium_texture_size_max
if(size==1) return 1;
for(i=2;i<=raydium_texture_size_max;i*=2)
    if(i==(int)size) return 1;
return 0;
}

GLuint raydium_texture_load_internal_params(GLuint gl_texture_target, char *filename, char *as, signed char faked, int faked_tx, int faked_ty, int faked_bpp, int or_live_id_fake, GLuint overwrite_id, GLuint cubemap_face)
{
FILE *file=NULL;
unsigned char temp[RAYDIUM_MAX_NAME_LEN];
unsigned char *data=NULL;
GLuint tx=0,ty=0,bpp=0,id;
GLuint i,ii,j,jj,k,GLbpp=0,GLbppi=0;
GLuint texsize=0;
char blended=0,filter=0,cutout=0,simulate=0;
char rgb;
GLfloat r,g,b,rrf;
signed char reflect=0;
GLint compressed=0;
char comp_str[128];
int flipx=0,flipy=0;
char compress=0; // Handle compressed tga texture
GLuint chunkid;
char rle;
char offset;
char filename_base[RAYDIUM_MAX_NAME_LEN];
GLuint gl_teximage_target;

// "as" is duplicated ?
//check if the filename of the texture is already loaded
/*for(i=0;i<raydium_texture_index;i++)
    if(!strcmp(raydium_texture_name[i],as))
    {
    raydium_log("texture: (internal) %s is duplicated",as);
    return i;
    }
*/
// "as" is duplicated ?
//check if the filename of the texture is already loaded
for(i=1;i<RAYDIUM_MAX_TEXTURES;i++)
{
    if(raydium_texture_used[i])
    {
        if (!strcmp(raydium_texture_name[i],as))
        {
            raydium_log("texture: (internal) %s is duplicated",as);
            return i;
        }
    }
}

//detect if there is no rendering window
if(raydium_window_mode==RAYDIUM_RENDERING_NONE)
    simulate=1;

raydium_file_basename(filename_base,filename);

/* is RGB color ? (or texture) */
if(!strncmp("rgb(",filename_base,4)) rgb=1; else rgb=0;

if(!strncmp("ENV",filename_base,3)) reflect=1;

/* is a texture image file ? */
if(!rgb && !faked)
{
 //yes, it's a texture image file
 file=raydium_file_fopen(filename,"rb");
 if(!file)
  {
    //raydium_log("Cannot open %s texture",filename);
    return 0;
  }

 fread(temp,1,12,file);
 offset=temp[0];
 if(temp[2]!=2 && temp[2]!=3 && temp[2]!=10 && temp[2]!=11)
 {
     fclose(file);
     raydium_log("%s is not an TGA RGB or grayscale file (type %i)",filename,temp[2]);
     return 0; //not a:  TGA RGB file
 }
 if (temp[2]==10 || temp[2]==11)
    compress=1;

 fread(temp,1,6,file);

 tx = temp[1] * 256 + temp[0]; // highbyte*256+lowbyte
 ty = temp[3] * 256 + temp[2]; // highbyte*256+lowbyte

 if (temp[5] & 0x10)
     flipx=1;
 else
     flipx=0;

 if (temp[5] & 0x20)
     flipy=1;
 else
     flipy=0;

 // OpenGL cube maps follow the RenderMan specification (opposite scanlines)
 if(cubemap_face)
    flipy=(flipy?0:1);

 //checking if you can use NPOT textures
 if(!raydium_texture_use_npot_textures)
 {
     //no, you can not use NPOT textures
     //check if the texture is valid
      if(!raydium_texture_size_is_correct(tx) || !raydium_texture_size_is_correct(ty))
     {
         //it's not a valid one
         raydium_log("texture: ERROR: cannot load %s: invalid size %ix%i (must be a power of two and inferior to your max hardware size: %i)",filename,tx,ty,raydium_texture_size_max);
         raydium_log("texture: I will fake this texture.");
         tx=ty=1;
     }
 }
 else
 {
     //yes, you can use NPOT textures
     //check if the texture is valid
      if(
        !glutExtensionSupported("GL_ARB_texture_non_power_of_two") &&
        !glutExtensionSupported("GL_ARB_texture_rectangle")  &&
        (   !raydium_texture_size_is_correct(tx) ||
            !raydium_texture_size_is_correct(ty))
        )
     {
         //it's not a valid one
         raydium_log("texture: ERROR: cannot load %s: invalid size %ix%i (must be a power of two and inferior to your max hardware size: %i)",filename,tx,ty,raydium_texture_size_max);
         raydium_log("texture: I will fake this texture.");
         tx=ty=1;
     }
 }

 bpp= temp[4] / 8;
 texsize = tx * ty * bpp;

 data=malloc(texsize);
 if(!data) {
 fclose(file);
 raydium_log("texture: ERROR ! malloc for %s failed ! (%i bytes needed)",filename,tx*ty*bpp);
 return 0; }


     chunkid=0;
     rle=0;

 fread(temp,1,offset,file);
 //reading the image data in the file
 for(jj=0; jj<ty; jj++)
 for(ii=0; ii<tx; ii++)
 {
 if (!compress) // Crossing finger about indent ;o)
     {
     if(fread(temp,1,bpp,file)!=bpp)
     { free(data); fclose(file);
     raydium_log("Invalid data in %s",filename);
     return 0; }
     }
 else
    {
    if (chunkid==0) //Begin of a data chunk
        {
        if(fread(&chunkid,sizeof(GLubyte), 1,file)==0) //Read data chunk identification
        { free(data); fclose(file);
        raydium_log("Not enough data in %s",filename);
        return 0; }
        if (chunkid <128) // Normal raw pixel data
            {
            rle=0; // Not rle
            chunkid++; // Number of chunks datas
            }
        else
            {
            rle=1; //Rle smae pixel repeated
            chunkid=chunkid-127; // Number of pixels
            if(fread(temp,1,bpp,file)!=bpp) // Read rle data
            { free(data); fclose(file);
            raydium_log("Invalid data in %s",filename);
            return 0; }
            }
        }
    if (!rle) // Not compressed data, reading pixel
        {
        if(fread(temp,1,bpp,file)!=bpp)
        { free(data); fclose(file);
        raydium_log("Invalid data in %s",filename);
        return 0; }
        }
    chunkid--;
    }
 if (flipx)
     i=tx-ii-1;
 else
     i=ii;
 if (flipy)
     j=ty-jj-1;
 else
     j=jj;
 // Normal pixel handling.
 k=((j*tx)+i)*bpp;
 if(bpp == 1) data[k]=temp[0];
 else // no greyscale
  {
   if(reflect)
    rrf=RAYDIUM_RENDER_REFLECTION_FACT;
   else
    rrf=1.0f;

   data[k]=temp[2]*rrf;
   data[k+1]=temp[1]*rrf;
   data[k+2]=temp[0]*rrf;
   if(bpp == 4)
    {
    data[k+3]=temp[3];
    if(temp[3]>0 && temp[3]<255)
        blended=RAYDIUM_TEXTURE_BLEND_BLENDED;
    if(temp[3]==0)
        cutout=1;
    }
  }
 }
 fclose(file);
 glPixelStorei(GL_UNPACK_ALIGNMENT,1);
} //end !rgb && !faked


//Have we to overwrite another texture?
if(raydium_texture_to_replace)
{
    id=raydium_texture_to_replace;
    //TODO:WARNING: Not tested!!!!
    raydium_texture_free(raydium_texture_to_replace);
    raydium_texture_to_replace=0;
}
 else
{
 if(!overwrite_id)
    id=raydium_texture_get_next_free_slot_internal();
 else
    id=overwrite_id;
}

//check if there is one texture slot free
if((int)id==-1)
{
    raydium_log("texture: No more texture slots left ! (%i max)", RAYDIUM_MAX_TEXTURES);
    return 0;
}

strcpy(raydium_texture_name[id],as);

if(faked)
    {
    raydium_live_Texture *tex;

    if(or_live_id_fake>=0)
        {
        tex=&raydium_live_texture[or_live_id_fake];
        tx=tex->hardware_tx;
        ty=tex->hardware_ty;
        bpp=tex->bpp/8;
        }
    else
        {
        tx=faked_tx;
        ty=faked_ty;
        bpp=faked_bpp;
        }

    texsize = tx * ty * bpp;
    data=malloc(texsize);
    if(!data)
        {
        fclose(file);
        raydium_log("texture: ERROR ! malloc for %s failed ! (%i bytes needed)",filename,tx*ty*bpp);
        return 0;
        }
    memset(data,0,texsize);
    }

if(!rgb)
{
 if(bpp==1)
    {
    GLbpp=GL_ALPHA;
#ifndef IPHONEOS
    GLbppi=GL_ALPHA8;
#else
    GLbppi=GL_ALPHA;
#endif
    blended=RAYDIUM_TEXTURE_BLEND_BLENDED;
    }

 if(bpp==3)
    {
    GLbpp=GL_RGB;
#ifndef IPHONEOS
    GLbppi=GL_RGBA8; // force RGBA (since ATI cards needs it)
#else
    GLbppi=GL_RGB;
#endif
    texsize += tx * ty;
    }

 if(bpp==4)
    {
    GLbpp=GL_RGBA;
#ifndef IPHONEOS
    GLbppi=GL_RGBA8;
#else
    GLbppi=GL_RGBA;
#endif
    }

 if(raydium_texture_compression_enabled && !faked)
    {
    if(GLbppi==GL_RGBA8)
        GLbppi=GL_COMPRESSED_RGBA;
    if(GLbppi==GL_ALPHA8)
        GLbppi=GL_COMPRESSED_ALPHA;
    }

 raydium_texture_blended[id]=0;
 if(blended)
    raydium_texture_blended[id]=RAYDIUM_TEXTURE_BLEND_BLENDED;

 if(cutout && !blended)
    {
    raydium_texture_blended[id]=RAYDIUM_TEXTURE_BLEND_CUTOUT;
    blended=RAYDIUM_TEXTURE_BLEND_CUTOUT;
    }


 raydium_texture_used_memory+=texsize;
 raydium_texture_memory[id]+=texsize;
 if( (raydium_texture_filter==RAYDIUM_TEXTURE_FILTER_TRILINEAR ||
      raydium_texture_filter==RAYDIUM_TEXTURE_FILTER_ANISO)
     && !faked)
     {
    raydium_texture_used_memory+=(texsize/3); // mipmaps
    raydium_texture_memory[id]+=(texsize/3); // mipmaps
    }

if(!simulate)
 glBindTexture(gl_texture_target,id);


 if(strstr(filename,".tri."))
    raydium_texture_islightmap[id]=1;

 if(!strncmp("ATM",filename_base,3))                                // TEMP !!
    {
    raydium_texture_blended[id]=RAYDIUM_TEXTURE_BLEND_ATM;
    blended=RAYDIUM_TEXTURE_BLEND_ATM;
    }

 if(!simulate)
 {
  if(!strncmp("BOX",filename_base,3) || !strncmp("CUBE",filename_base,4) || faked || blended==RAYDIUM_TEXTURE_BLEND_ATM) // TEMP !!
  {                                                                     // TEMP !!
    glTexParameteri(gl_texture_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);        // TEMP !!
    glTexParameteri(gl_texture_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);        // TEMP !!
    glTexParameteri(gl_texture_target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);        // TEMP !!
  }                                                                     // TEMP !!
  else
  {
    glTexParameteri(gl_texture_target,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(gl_texture_target,GL_TEXTURE_WRAP_T,GL_REPEAT);
    glTexParameteri(gl_texture_target,GL_TEXTURE_WRAP_R,GL_REPEAT);
  }
 }

 if(gl_texture_target==GL_TEXTURE_CUBE_MAP)
    gl_teximage_target=cubemap_face;
 else
    gl_teximage_target=gl_texture_target;

 filter=raydium_texture_filter;

 if(!strncmp("HDR",filename_base,3))                                // TEMP !!
    {
    raydium_texture_hdr[id]=1;
    raydium_texture_nolight[id]=1;
    }

 if(reflect)
    raydium_texture_env[id]=1;

 if(filter==RAYDIUM_TEXTURE_FILTER_TRILINEAR && blended)
    filter=RAYDIUM_TEXTURE_FILTER_BILINEAR;

 if(filter==RAYDIUM_TEXTURE_FILTER_ANISO && blended)
    filter=RAYDIUM_TEXTURE_FILTER_BILINEAR;

 if(faked && filter!=RAYDIUM_TEXTURE_FILTER_NONE)
    filter=RAYDIUM_TEXTURE_FILTER_BILINEAR;

  if(filter==RAYDIUM_TEXTURE_FILTER_NONE && !simulate)
  {
  glTexParameteri(gl_texture_target,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  glTexParameteri(gl_texture_target,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  glTexImage2D(gl_teximage_target,0,GLbppi,tx,ty,0,GLbpp,GL_UNSIGNED_BYTE,data);
  }

  if(filter==RAYDIUM_TEXTURE_FILTER_BILINEAR && !simulate)
  {
  glTexParameterf(gl_texture_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf(gl_texture_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(gl_teximage_target,0,GLbppi,tx,ty,0,GLbpp,GL_UNSIGNED_BYTE,data);
  }

  if(filter==RAYDIUM_TEXTURE_FILTER_TRILINEAR && !simulate)
  {
  glTexParameteri(gl_texture_target,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(gl_texture_target,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR); // Trilinear filtering
  gluBuild2DMipmaps(gl_teximage_target, GLbppi, tx, ty, GLbpp, GL_UNSIGNED_BYTE, data);
  }

  if(filter==RAYDIUM_TEXTURE_FILTER_ANISO && !simulate)
  {
  glTexParameteri(gl_texture_target,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(gl_texture_target,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR); // Trilinear filtering
  glTexParameterf(gl_texture_target, GL_TEXTURE_MAX_ANISOTROPY_EXT, raydium_texture_filter_aniso_levels); // + anisotropy
  gluBuild2DMipmaps(gl_teximage_target, GLbppi, tx, ty, GLbpp, GL_UNSIGNED_BYTE, data);
  }

  if(raydium_texture_compression_enabled)
    glGetTexLevelParameteriv(gl_texture_target, 0, GL_TEXTURE_COMPRESSED_ARB, &compressed);

  if(compressed)
    {
    GLint size;
    float level;
    glGetTexLevelParameteriv(gl_texture_target, 0, GL_TEXTURE_COMPRESSED_IMAGE_SIZE_ARB, &size);
    level=(float)texsize/size;
    raydium_texture_used_memory-=texsize;
    raydium_texture_memory[id]-=texsize;
    raydium_texture_used_memory+=size; // should take care of mipmaps too !
    raydium_texture_memory[id]+=size; // should take care of mipmaps too !
    sprintf(comp_str," compression ratio: %.2f",level);
    }
  else
    strcpy(comp_str,"");

 raydium_log("Tex. num %i (%s) %s: %ix%i, %i Bpp (b%i lm%i hdr%i fx%i fy%i)%s",
             id,raydium_texture_name[id],
             (faked?"FAKED":"loaded"),
             tx,ty,bpp,
             blended,raydium_texture_islightmap[id],
             raydium_texture_hdr[id],flipx,flipy,comp_str);
 free(data);
} else /* is rgb color */
{
 sscanf(filename,"rgb(%f,%f,%f)",&r,&g,&b);
 raydium_texture_rgb[id][0]=r;
 raydium_texture_rgb[id][1]=g;
 raydium_texture_rgb[id][2]=b;
 raydium_texture_rgb[id][3]=1.f;
 if(r<0 && g<0 && b<0)
    {
    raydium_texture_blended[id]=RAYDIUM_TEXTURE_PHANTOM;
    raydium_log("Tex. num %i is Phantom (depth buffer only)",id);
    }
 else
 {
    raydium_texture_blended[id]=0;
    raydium_log("Tex. num %i, rgb(%f,%f,%f) is RGB Color",id,r,g,b);
 }
}
if(id>0)raydium_texture_used[id]=TRUE;
return id;
}


// wrapper to deal with cubemaps
GLuint raydium_texture_load_internal(char *filename, char *as, signed char faked, int faked_tx, int faked_ty, int faked_bpp, int or_live_id_fake)
{
if(!strncmp("CUBE",filename,4))
    {
    int i;

/*    GLenum faces[6] = {
        GL_TEXTURE_CUBE_MAP_POSITIVE_X,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Z
        GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
    };*/

    char *tags[] = {
        "posx",
        "negx",
        "posy",
        "negy",
        "posz",
        "negz",
        NULL
    };

    GLuint id=0;
    char pre[RAYDIUM_MAX_NAME_LEN];
    char post[RAYDIUM_MAX_NAME_LEN];
    char curr_file[RAYDIUM_MAX_NAME_LEN];

    if(!raydium_parser_cut(filename,pre,post,'*'))
        {
        raydium_log("ERROR: invalid cubemap name, ex: CUBE_test_*.tga");
        return 0;
        }

    for(i=0;i<6;i++)
        {
        sprintf(curr_file,"%s%s%s",pre,tags[i],post);
        id=raydium_texture_load_internal_params(GL_TEXTURE_CUBE_MAP,curr_file,curr_file,faked,faked_tx,faked_ty,faked_bpp,or_live_id_fake,id,GL_TEXTURE_CUBE_MAP_POSITIVE_X+i);
        if(!id) { raydium_log("ERROR: cubemap loading failed (%i/6)",i+1); return 0; }
        }

    raydium_log("Tex. num %i (%s) loaded as a cubemap",id,filename);
    strcpy(raydium_texture_name[id],filename);
    raydium_texture_cubemap[id]=1;
    return id;
    }
else
    return raydium_texture_load_internal_params(GL_TEXTURE_2D,filename,as,faked,faked_tx,faked_ty,faked_bpp,or_live_id_fake,0,0);
}


void raydium_texture_npot_enable(void)
{
    raydium_texture_use_npot_textures=1;
}

void raydium_texture_npot_disable(void)
{
    raydium_texture_use_npot_textures=0;
}

GLuint raydium_texture_load(char *filename)
{
int res;
res=raydium_texture_load_internal(filename,filename,0,0,0,0,0);
if((int)res<0)
    {
    raydium_log("texture: faking '%s' with pink color",filename);
    res=raydium_texture_load_internal("rgb(1,0,1)",filename,0,0,0,0,0);
    }
return res;
}


GLuint raydium_texture_load_erase(char *filename, GLuint to_replace)
{
if(to_replace<=0) return 0;
raydium_texture_to_replace=to_replace;
return raydium_texture_load(filename);
}

void raydium_texture_free(int id)
{
GLuint textures[1];
textures[0]=id;
glDeleteTextures( 1, (GLuint *)&textures );

raydium_texture_name[id][0]=0;
raydium_texture_blended[id]=0;
raydium_texture_nolight[id]=0;
raydium_texture_env[id]=0;
raydium_texture_cubemap[id]=0;
raydium_texture_islightmap[id]=0;
raydium_texture_shader[id]=-1;
raydium_texture_rgb[id][0]=-1.f;
raydium_texture_rgb[id][1]=-1.f;
raydium_texture_rgb[id][2]=-1.f;
raydium_texture_rgb[id][3]=1.f;
raydium_texture_hdr[id]=0;
raydium_texture_used[id]=0;

//rest the memory of this texture to the total
raydium_texture_used_memory-=raydium_texture_memory[id];

raydium_texture_memory[id]=0;
}

void raydium_texture_free_name(char *name)
{
int i;
//TODO:re-analise this code to avoid useless passes
for(i=1;i<RAYDIUM_MAX_TEXTURES;i++)
    if(!strcmp(raydium_texture_name[i],name))
        {
        raydium_texture_free(i);
        }
}
int raydium_texture_is_slot_used(int slot)
{
return raydium_texture_used[slot];
}

int raydium_texture_get_next_free_slot_internal(void)
{
int i;

for(i=1;i<RAYDIUM_MAX_TEXTURES;i++)
    if(raydium_texture_used[i]==0)
        return i;
return -1;
}

signed char raydium_texture_current_set(GLuint current)
{
if(current<RAYDIUM_MAX_TEXTURES)
    {
    raydium_texture_current_main=current;
    return current;
    }
return 0;
}

GLuint raydium_texture_find_by_name(char *name)
{
GLuint i;
char flag=0;
GLuint ret=0;

for(i=1;i<RAYDIUM_MAX_TEXTURES;i++)
    if(raydium_texture_used[i] && !strcmp(raydium_texture_name[i],name))
        {
        flag++;
        ret=i;
        }

if(!flag)
    ret=raydium_texture_load(name);

return ret;
}

GLint raydium_texture_exists(char *name)
{
int i;
for(i=1;i<RAYDIUM_MAX_TEXTURES;i++)
    if(raydium_texture_used[i] && !strcmp(raydium_texture_name[i],name))
        return i;
return -1;
}


signed char raydium_texture_current_set_name(char *name)
{
return raydium_texture_current_set(raydium_texture_find_by_name(name));
}


void raydium_texture_filter_change(GLuint filter)
{
//GLuint i;
char force[RAYDIUM_MAX_NAME_LEN];

if(raydium_init_cli_option("filter",force))
    {
    if(!strcmp(force,"none")) filter=RAYDIUM_TEXTURE_FILTER_NONE;
    if(!strcmp(force,"bilinear")) filter=RAYDIUM_TEXTURE_FILTER_BILINEAR;
    if(!strcmp(force,"trilinear")) filter=RAYDIUM_TEXTURE_FILTER_TRILINEAR;
    if(!strcmp(force,"aniso")) filter=RAYDIUM_TEXTURE_FILTER_ANISO;
    }

raydium_texture_filter=filter;

// too ... dangerous
/*for(i=0;i<raydium_texture_index;i++)
    raydium_texture_load_erase(raydium_texture_name[i],i);*/

}


void raydium_texture_compression(signed char enable)
{
if(raydium_texture_compression_available)
    raydium_texture_compression_enabled=(enable?1:0);
}
