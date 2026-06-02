/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

#ifndef DONT_INCLUDE_HEADERS
#include "index.h"
#else
#include "headers/sprites.h"
#endif

#ifdef ODE_SUPPORT

//returns the first sprite id available
int raydium_sprite_check_available(void)

{
        int id;
        int a;
        id=-1;
        for(a=0;a<RAYDIUM_MAX_SPRITES;a++)
        {
                if(raydium_sprite_used[a]==0)
                {
                        id=a;
                        //automatically reserve the sprite. TESTING
                        raydium_sprite_used[a]=1;
                        break;
                }
        }
        return id;
}

//function to draw directly the sprite. Internal. Don't use
void raydium_sprite_billboard(float x, float y, float z,float ux, float uy, float uz, float rx, float ry, float rz, int textureid, float s0, float s1, float t0, float t1,float size)

{

    GLfloat TSIZE;
    //int texsave;
    TSIZE=size;

    //this could be done faster with a bit operation changing variables with one array?
    //but it could be really ugly and hard to use
    ux*=TSIZE/2;
    uy*=TSIZE/2;
    uz*=TSIZE/2;

    rx*=TSIZE/2;
    ry*=TSIZE/2;
    rz*=TSIZE/2;

    //texsave=raydium_texture_current_main;
    raydium_texture_current_set(textureid);
    raydium_rendering_internal_prepare_texture_render(raydium_texture_current_main);
     glBegin(GL_QUADS); // berk... but i'll switch to triangles one day ;)
         glTexCoord2f(s0, t0);glVertex3f(x + (ux - rx),y + (uy - ry),z + (uz - rz));
         glTexCoord2f(s0, t1);glVertex3f(x + (rx + ux),y + (ry + uy),z + (rz + uz));
         glTexCoord2f(s1, t1);glVertex3f(x + (rx - ux),y + (ry - uy),z + (rz - uz));
         glTexCoord2f(s1, t0);glVertex3f(x + (-rx - ux),y + (-ry - uy),z + (-rz - uz));
    glEnd();
    /*
    //first try using triangle instead of quad (should be at least twice faster)
    //How the hell can i calculate the texture coords?

    // CANNOT BE USED IN THIS CASE CAUSE OF CLAMPING PROBLEM, THE REST OF THE TEXTURE IS VIEWED
    // BUT SHOULD BE WORK FINE WITH PARTICLES

    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
    glBegin(GL_TRIANGLES);
         //deforms the triangle, need to fix, but difficult to guess how
         glTexCoord2f(s0, t0);glVertex3f(x + (ux - rx),Y + (uy - ry),Z + (uz - rz));
          glTexCoord2f(s0, 2*t1);glVertex3f(x + (rx + ux),Y + (2*ry + 2*uy),Z + (rz + uz));
          glTexCoord2f(2*s1, t0);glVertex3f(x + (-2*rx - 2*ux),Y + (-ry - uy),Z + (-rz - uz));
    glEnd();
    */
}


void raydium_sprite_dump_info(int id)
{
        int a;
        raydium_log("SPRITE DUMP: %d",id);
        for(a=0;a<raydium_sprite_total_frames[id];a++)
        {
                raydium_log("\nframe %d:\n   group: %d\n   jump to: %d\n",
                a,
                raydium_sprite_group_frame[id][a],
                raydium_sprite_group_jump[id][raydium_sprite_group_frame[id][a]]);
        }
}

//Load an sprite (filename.sprite) preloading its textures.
//TODO:it should check if the sprite file is already loaded and if it's true
//TODO:clean file loading, ugly.
//then it should ignore the file load and get the texture id.

int raydium_sprite_load(char *fichero,int etiqueta)

/**
This function allows you to load an sprite. It will returns the id(int)
of the new sprite.
**/
{
    FILE *spr;
    int ret,size,i,tmp,sid;
    char var[RAYDIUM_MAX_NAME_LEN];
    char val_s[RAYDIUM_MAX_NAME_LEN];
    GLfloat val_f[RAYDIUM_MAX_NAME_LEN];
    int group;
    char cadena[255];

    //initializing the counter of frames
    i=0;
        //getting the next available sprite id
    sid=raydium_sprite_check_available();

    //resetting the values of the sprite
    for(group=0;group<RAYDIUM_SPRITE_MAX_GROUPS;group++)
    {
        raydium_sprite_group_start_frame[sid][group]=   -1;
        raydium_sprite_group_jump[sid][group]       =   -1;

    }
    raydium_sprite_size[sid]=1;

    //opening the sprite file. TODO:Clean
    if((spr=raydium_file_fopen(fichero,"r")))
    {
                //loop for reading each line of the file
                while( (ret=raydium_parser_read(var,val_s,val_f,&size,spr))!=RAYDIUM_PARSER_TYPE_EOF)
                {
                        //checking the collision box(long, depth, height)
                        if(ret==RAYDIUM_PARSER_TYPE_FLOAT && strcmp(var,"collision")==0 && size==3)
                        {
                                if(RAYDIUM_SPRITE_DEBUG)raydium_log("Collision Box: %f, %f, %f",val_f[0],val_f[1],val_f[2]);
                                raydium_sprite_collision_box[sid][0]=val_f[0];
                                raydium_sprite_collision_box[sid][1]=val_f[1];
                                raydium_sprite_collision_box[sid][2]=val_f[2];
                        }
                        //checking if it's a float array of 4 elements. A frame
                        if(ret==RAYDIUM_PARSER_TYPE_FLOAT && strcmp(var,"coords")==0 && size==4)
                        {
                                if(RAYDIUM_SPRITE_DEBUG)raydium_log("Frame %d readed for \"%s\": %f, %f, %f, %f",i,var,val_f[0],val_f[1],val_f[2],val_f[3]);
                                //inverted values to prevent horizontal flip
                                //something wrong with this or maybe the billboard sprite function is wrong?
                                raydium_sprite_coords[sid][i][0]=val_f[1];
                                raydium_sprite_coords[sid][i][1]=val_f[0];
                                raydium_sprite_coords[sid][i][2]=val_f[2];
                                raydium_sprite_coords[sid][i][3]=val_f[3];
                        }

                        //checking if it's a float array of 1 element
                        /*if(ret==RAYDIUM_PARSER_TYPE_FLOAT && strcmp(var,"jump")==0 && size==1)
                        {
                                if(RAYDIUM_SPRITE_DEBUG)raydium_log("** Group will jump to group %d.",val_f[0]);
                                raydium_sprite_group_jump[sid]=val_f[0];
                        }

                        if(ret==RAYDIUM_PARSER_TYPE_STRING && strcmp(var,"jump")==0 && strcmp(val_s,"stop")==0)
                        {
                                if(RAYDIUM_SPRITE_DEBUG)raydium_log("** The group will stop in its last frame");
                                raydium_sprite_group_jump[sid]=-1;
                        }*/

                        //checking if it's an string (the filename of the texture)
                        if(ret==RAYDIUM_PARSER_TYPE_STRING && strcmp(var,"texture")==0)
                        {
                                if(RAYDIUM_SPRITE_DEBUG)raydium_log("TEXTURE used for frame %d: \"%s\"",i,val_s);
                                //getting the texture ID
                                raydium_texture_filter_change(RAYDIUM_TEXTURE_FILTER_BILINEAR);
                                raydium_sprite_textureid[sid][i]=raydium_texture_find_by_name(val_s);
                                //How can reset the previous texture filter?
                                raydium_texture_filter_change(RAYDIUM_TEXTURE_FILTER_ANISO);
                                //some debug
                                if(RAYDIUM_SPRITE_DEBUG)raydium_log("For sprite %d, the frame %d has the texture ID %d(%s)",sid,i,raydium_sprite_textureid[sid][i],val_s);
                                //as the texture file is the last element for a frame, we increase the counter
                                i++;
                        }

                        //checking if it's an string(the group of the sprite)
                        if(ret==RAYDIUM_PARSER_TYPE_FLOAT && strcmp(var,"group")==0 && size==1)
                        {
                                if(RAYDIUM_SPRITE_DEBUG)raydium_log("Group of the frame %d: %f",i,val_f[0]);
                                //if this is the first frame of a new group, we mark that frame as start_frame of that group.
                                if(raydium_sprite_group_start_frame[sid][(int)val_f[0]]==-1) raydium_sprite_group_start_frame[sid][(int)val_f[0]]=i;
                                raydium_sprite_group_frame[sid][i]=(int)val_f[0];
                        }

                        if(ret==RAYDIUM_PARSER_TYPE_FLOAT && strcmp(var,"group")==0 && size==2)
                        {

                                //if this is the first frame of a new group, we mark that frame as start_frame of that group.
                                if(raydium_sprite_group_start_frame[sid][(int)val_f[0]]==-1) raydium_sprite_group_start_frame[sid][(int)val_f[0]]=i;
                                raydium_sprite_group_frame[sid][i]=(int)val_f[0];
                                raydium_sprite_group_jump[sid][(int)val_f[0]]=(int)val_f[1];
                                if(RAYDIUM_SPRITE_DEBUG)raydium_log("*** Group of the frame %d: %f. Jumping to %f",i,val_f[0],val_f[1]);
                        }
                        if(ret==RAYDIUM_PARSER_TYPE_FLOAT && strcmp(var,"size")==0 )
                        {
                                if(RAYDIUM_SPRITE_DEBUG)raydium_log("*** size of the sprite %d: %f.",sid,val_f);
                                raydium_sprite_size[sid]=val_f[0];
                        }

                }
                //closing the file
                fclose(spr);

                //reset the posision of the sprite
                raydium_sprite_pos[sid][0]=0;
                raydium_sprite_pos[sid][1]=0;
                raydium_sprite_pos[sid][2]=0;

                //adding an standard frametime for the sprite
                raydium_sprite_time[sid]=20;

                //initializing the sprite at its first frame
                raydium_sprite_current_frame[sid]=0;

                //storing the total number of frames
                raydium_sprite_total_frames[sid]=i-1;

                //initializing the current group
                raydium_sprite_group_current[sid]=1;

                //create the ODE object
                sprintf(cadena,"raydium_sprite_%d",sid);
                raydium_ode_object_delete_name(cadena);
                raydium_sprite_collision_box_id[sid]=raydium_ode_object_create(cadena);
                //create the ODE element
                sprintf(cadena,"raydium_sprite_box_%d",sid);
                raydium_sprite_collision_element_id[sid]=raydium_ode_object_box_add(cadena,raydium_sprite_collision_box_id[sid],1,raydium_sprite_collision_box[sid][0],raydium_sprite_collision_box[sid][1],raydium_sprite_collision_box[sid][2], RAYDIUM_ODE_STANDARD,etiqueta,"");
        //raydium_ode_object_cylinder_add(cadena,raydium_sprite_collision_box_id[sid],1,3,6, RAYDIUM_ODE_STATIC, 0,"");
        raydium_ode_element_move_name_3f(cadena,raydium_sprite_pos[sid][0],raydium_sprite_pos[sid][1],raydium_sprite_pos[sid][2]);
                //listing the textures and coordinates
                if(RAYDIUM_SPRITE_DEBUG)
                {
                        raydium_log("Textures used in this sprite:");
                        for(tmp=0;tmp<i;tmp++)
                        {
                                raydium_log("Frame %d use texture ID %d",tmp,raydium_sprite_textureid[sid][tmp]);
                                raydium_log("It's coordinates are: %f %f %f %f ",raydium_sprite_coords[sid][tmp][0],raydium_sprite_coords[sid][tmp][1],raydium_sprite_coords[sid][tmp][2],raydium_sprite_coords[sid][tmp][3]);
                        }
                        for(group=0;group<RAYDIUM_SPRITE_MAX_GROUPS;group++)
                        {
                                raydium_log("Initial frame of group %d: %d",group,raydium_sprite_group_start_frame[sid][group]);
                        }
                }
        }
    //returning the new spriteid or -1 if nothing loaded.
    if(i>0)
    {
        strcpy(raydium_sprite_name[sid],fichero);
        raydium_sprite_type[sid]=-1;
        raydium_sprite_status[sid]=-1;
        raydium_current_sprite++;
        return sid;
    }
    else
    {
        return -1;
    }
}

//get the name (char*) of an sprite from its related ODE object id
char *raydium_sprite_get_name_from_object(int obj)
{
        int a;
        for(a=0;a<RAYDIUM_MAX_SPRITES;a++)
        {
                if(raydium_sprite_collision_box_id[a]==obj)
                {
                        return raydium_sprite_name[a];
                }
        }
        return NULL;
}

//get the ODE object id from a certain sprite according its spriteid
int raydium_sprite_object_get(int spriteid)
{
    //raydium_log("you are checking %d=%s",spriteid,raydium_sprite_name[spriteid]);
    return raydium_sprite_collision_box_id[spriteid];
}

//internal. don't use
void sprite_render_frame(float x, float y, float z, int spriteid,int frame,float scalex,float scaley)
{
    GLuint texsave;
    char light;
    GLfloat modmat[16];
    GLfloat ux;
    GLfloat uy;
    GLfloat uz;
    GLfloat rx;
    GLfloat ry;
    GLfloat rz;

        glPushMatrix();

    //storing the current texture, to recover it at last
    texsave=raydium_texture_current_main;
    //storing light state
    light=raydium_light_enabled_tag;
    raydium_light_disable();
    if (raydium_camera_pushed) raydium_camera_replace(); // is it really our job to do it here ?
    //raydium_rendering_internal_restore_render_state();
    //applying plane matrix
    glGetFloatv(GL_MODELVIEW_MATRIX,modmat);

    ux=modmat[0]*scalex;
    uy=modmat[4]*scalex;
    uz=modmat[8]*scalex;//no idea which scale have to use!!!!!
    rx=modmat[1]*scalex;
    ry=modmat[5]*scalex;
    rz=modmat[9]*scalex;//no idea which scale have to use!!!!!
    glDepthMask(GL_FALSE);
    //raydium_billboard_sprite(x,y,z,ux,uy,uz,rx,ry,rz,1);
    raydium_sprite_billboard(x,y,z,ux,uy,uz,rx,ry,rz,raydium_sprite_textureid[spriteid][frame],raydium_sprite_coords[spriteid][frame][0],raydium_sprite_coords[spriteid][frame][1],raydium_sprite_coords[spriteid][frame][2],raydium_sprite_coords[spriteid][frame][3],raydium_sprite_size[spriteid]);
    glDepthMask(GL_TRUE);

    glPopMatrix();

    //restoring previous light and texture.
    if(light) raydium_light_enable();
    raydium_texture_current_set(texsave);
}

//put an sprite in a space position using universal coordinates
void raydium_sprite_move(int sprite,float x, float y, float z)
{
    raydium_sprite_pos[sprite][0]=x;
    raydium_sprite_pos[sprite][1]=y;
    raydium_sprite_pos[sprite][2]=z;
    raydium_ode_object_move_3f(raydium_sprite_collision_box_id[sprite],raydium_sprite_pos[sprite][0],raydium_sprite_pos[sprite][1],raydium_sprite_pos[sprite][2]);
}

void raydium_sprite_move_relative(int sprite, float deltax, float deltay, float deltaz)
//same that before, but using relative coordinates
{
    raydium_sprite_pos[sprite][0]+=deltax;
    raydium_sprite_pos[sprite][1]+=deltay;
    raydium_sprite_pos[sprite][2]+=deltaz;
    raydium_ode_object_move_3f(raydium_sprite_collision_box_id[sprite],raydium_sprite_pos[sprite][0],raydium_sprite_pos[sprite][1],raydium_sprite_pos[sprite][2]);
}

//show a certain sprite
void raydium_sprite_display(int id)
{
        raydium_camera_replace();
    int grp;
    int lastone=0;

    grp = raydium_sprite_group_current[id];
    sprite_render_frame(raydium_sprite_pos[id][0],raydium_sprite_pos[id][1],raydium_sprite_pos[id][2],id,raydium_sprite_current_frame[id],1,1);
    //adding the frametime to the loop...Hmmm, not sure about this...
    raydium_sprite_timer[id]+=raydium_frame_time*200;
    //Is it time to draw a new frame?
    if(raydium_sprite_timer[id]>=raydium_sprite_time[id])
    {

        //erase the counter
        raydium_sprite_timer[id]=0;
        raydium_sprite_stopped[id]=0;
        //if this is the last frame of the sprite...
                if (raydium_sprite_total_frames[id]==raydium_sprite_current_frame[id]) //this is the last frame
                {
                        //raydium_log("last frame!!! total: %d number: %d",raydium_sprite_total_frames[id],raydium_sprite_current_frame[id]);
                        lastone=1;
                }
                else
                {
                        lastone=0;
                }

        //if not the last frame of the sprite, check if the next frame and the current one have the same group ID
        if(!lastone && raydium_sprite_group_frame[id][raydium_sprite_current_frame[id]+1]==raydium_sprite_group_frame[id][raydium_sprite_current_frame[id]] )
        {
            //if they have the same group ID, then just advance one frame
            raydium_sprite_current_frame[id]++;
        }
        else //if they are different or this is the last frame of the sprite
        {
                //raydium_log("nextjump: %d",raydium_sprite_group_jump[id][raydium_sprite_group_current[id]]);
                //if the group has an "stop" tag
            if(raydium_sprite_group_jump[id][raydium_sprite_group_current[id]]==-1)
            {
                if(RAYDIUM_SPRITE_DEBUG)raydium_log("Group %d has an STOP tag(-1)",grp);
                raydium_sprite_stopped[id]=1;
                //do nothing. PAUSE THE ANIMATION
            }
            /*
            else
            {
                raydium_sprite_stopped[id]=0;
                        }
                        */



            //if the group has LOOP.
            if (raydium_sprite_group_jump[id][raydium_sprite_group_current[id]]==-2)
            {
                raydium_sprite_current_frame[id]=raydium_sprite_group_start_frame[id][raydium_sprite_group_current[id]];
            }
            //if the group has a regular jump
            if (raydium_sprite_group_jump[id][raydium_sprite_group_current[id]]>=0)
            {
                //jumping to a new group
                //BADD!!!!!!!!!!!!!
                raydium_sprite_current_frame[id]=raydium_sprite_group_start_frame[id][raydium_sprite_group_jump[id][raydium_sprite_group_current[id]]];
                //if(RAYDIUM_SPRITE_DEBUG)raydium_log("grupo actual:%d grupo de salto:%d ",grp,raydium_sprite_group_jump[id][raydium_sprite_current_frame[id]]);
            }
            if (raydium_sprite_group_jump[id][raydium_sprite_group_current[id]]==-999)
            {
                //do nothing
                        }

            //if the groups has a unkonw jump, then restart the group and notice
            /*
            if (raydium_sprite_group_jump[id][raydium_sprite_current_frame[id]]<-2 && raydium_sprite_group_jump[id][raydium_sprite_current_frame[id]]!=-999)
            {
                raydium_log("UNKNOWK JUMP TYPE FOR SPRITE %d IN FRAME %d",id,raydium_sprite_current_frame[id]);
                raydium_sprite_current_frame[id]=raydium_sprite_group_start_frame[id][raydium_sprite_group_current[id]];
            }
            */
        }
        /*
        //If the animation comes to the end of the frames, we have to reset it (sending it to the begining of the group)
        if(raydium_sprite_current_frame[id]>=raydium_sprite_total_frames[id])
        {
            raydium_sprite_current_frame[id]=raydium_sprite_group_start_frame[id][raydium_sprite_group_current[id]];
        }
        * */
    }

}

//function to change the animation group of one sprite
void raydium_sprite_group_change(int sprite,int group)
{
        if(raydium_sprite_group_start_frame[sprite][group]!=-1)
        {
                raydium_sprite_group_current[sprite]=group;
                raydium_sprite_current_frame[sprite]=raydium_sprite_group_start_frame[sprite][group];
                //raydium_log("Changing sprite group %d to: %d",sprite,group);
        }
}

//frees an sprite
void raydium_sprite_free(int sprite)
{
        //pending to free really the content of the sprite
        raydium_sprite_pos[sprite][0]=0;
        raydium_sprite_pos[sprite][1]=0;
        raydium_sprite_pos[sprite][2]=0;
        raydium_sprite_used[sprite]=0;
        raydium_sprite_current_frame[sprite]=0;
        raydium_sprite_total_frames[sprite]=0;
        raydium_ode_object_delete(raydium_sprite_collision_box_id[sprite]);

}
//frees an sprite
void raydium_sprite_free_name(char *name)
{
    int a,sprite;
    sprite=0;
    for (a=0;a<RAYDIUM_MAX_SPRITES;a++)
    {
        if(raydium_sprite_used[a]==1 && strcmp(raydium_sprite_name[a],name)==0)
        {
            sprite=a;
            //pending to free really the content of the sprite
            raydium_sprite_pos[sprite][0]=0;
            raydium_sprite_pos[sprite][1]=0;
            raydium_sprite_pos[sprite][2]=0;
            raydium_sprite_used[sprite]=0;
            raydium_sprite_current_frame[sprite]=0;
            raydium_sprite_total_frames[sprite]=0;
            raydium_ode_object_delete(raydium_sprite_collision_box_id[sprite]);
            raydium_sprite_name[sprite][0]=0;
            break;
        }
    }


}


/*
//DON'T USE!!!
int raydium_sprite_copy(int other)
{
        int a=0;
        int sid;
        raydium_log("Preparando copia %d. Su frames es %d",other,raydium_sprite_total_frames[other]);
        if(raydium_sprite_total_frames[other])
        //there is such  sprite
        {
                //returning the new id
                //bad way, i need a function.
                raydium_log("copying sprite");
                //sid=raydium_current_sprite + 1;
                sid=raydium_sprite_check_available();
                //raydium_sprite_used[sid]=1;

                //copying all the parameters:
                //the frames
                raydium_sprite_total_frames[sid]=raydium_sprite_total_frames[other];
                //the coords of the frames
                for(a=0;a<raydium_sprite_total_frames[sid];a++)
                {
                        spritecoord[sid][a][0]=spritecoord[other][a][0];
                        spritecoord[sid][a][1]=spritecoord[other][a][1];
                        spritecoord[sid][a][2]=spritecoord[other][a][2];
                        spritecoord[sid][a][3]=spritecoord[other][a][3];
                        spritecoord[sid][a][4]=spritecoord[other][a][4];
                        spritecoord[sid][a][5]=spritecoord[other][a][5];
                        spritecoord[sid][a][6]=spritecoord[other][a][6];
                        spritecoord[sid][a][7]=spritecoord[other][a][7];
                }
                //the position
                raydium_sprite_pos[sid][0]=raydium_sprite_pos[other][0];
                raydium_sprite_pos[sid][1]=raydium_sprite_pos[other][1];
                raydium_sprite_pos[sid][2]=raydium_sprite_pos[other][2];
                //id texture per frame
                for(a=0;a<raydium_sprite_total_frames[sid];a++)
                {
                        raydium_sprite_textureid[sid][a]=raydium_sprite_textureid[other][a];
                }
                //sprite coords per frame
                for(a=0;a<raydium_sprite_total_frames[sid];a++)
                {
                        raydium_sprite_coords[sid][a][0]=raydium_sprite_coords[other][a][0];
                        raydium_sprite_coords[sid][a][1]=raydium_sprite_coords[other][a][1];
                        raydium_sprite_coords[sid][a][2]=raydium_sprite_coords[other][a][2];
                        raydium_sprite_coords[sid][a][3]=raydium_sprite_coords[other][a][3];

                }
                //the time
                raydium_sprite_time[sid]=raydium_sprite_time[other];
                //the timer
                raydium_sprite_timer[sid]=raydium_sprite_timer[other];
                //current frame
                raydium_sprite_current_frame[sid]=raydium_sprite_current_frame[other];
                //total frames
                raydium_sprite_total_frames[sid]=raydium_sprite_total_frames[other];
                //group of each frame
                for(a=0;a<raydium_sprite_total_frames[sid];a++)
                {
                        raydium_sprite_group_frame[sid][a]=raydium_sprite_group_frame[other][a];
                }
                //current group
                raydium_sprite_group_current[sid]=raydium_sprite_group_current[other];
                //start frame per group
                for(a=0;a<RAYDIUM_SPRITE_MAX_GROUPS;a++)
                {
                        if(raydium_sprite_group_start_frame[other][a]!=-1)
                                raydium_sprite_group_start_frame[sid][a]=raydium_sprite_group_start_frame[other][a];
                }
                //collision box
                raydium_sprite_collision_box[sid][0]=raydium_sprite_collision_box[other][0];
                raydium_sprite_collision_box[sid][1]=raydium_sprite_collision_box[other][1];
                raydium_sprite_collision_box[sid][2]=raydium_sprite_collision_box[other][2];
                //collision box id
                raydium_sprite_collision_box_id[sid]=raydium_sprite_collision_box_id[other];
                //raydium_sprite_collision_box_id[sid]=raydium_ode_object_create(cadena);
                //sprintf(cadena,"raydium_sprite_box_%d",sid);
                //raydium_ode_object_box_add(cadena,raydium_sprite_collision_box_id[sid],1,raydium_sprite_collision_box[sid][0],raydium_sprite_collision_box[sid][1],raydium_sprite_collision_box[sid][2], RAYDIUM_ODE_STATIC,0,"");
                //raydium_ode_element_move_name_3f(cadena,raydium_sprite_pos[sid][0],raydium_sprite_pos[sid][1],raydium_sprite_pos[sid][2]);
                //group jump
                for(a=0;a<RAYDIUM_SPRITE_MAX_GROUPS;a++)
                {
                        if(raydium_sprite_group_jump[other]!=0)
                                raydium_sprite_group_jump[sid][a]=raydium_sprite_group_jump[other][a]; //-2=no jump, -1=stop, number=group to jump
                }

                raydium_current_sprite++;
                //creating the new object
                char *cadena;
                sprintf(cadena,"raydium_sprite_%d",sid);
                raydium_ode_object_delete_name(cadena);
                raydium_sprite_collision_box_id[sid]=raydium_ode_object_create(cadena);
                sprintf(cadena,"raydium_sprite_box_%d",sid);
                raydium_ode_object_box_add(cadena,raydium_sprite_collision_box_id[sid],1,raydium_sprite_collision_box[sid][0],raydium_sprite_collision_box[sid][1],raydium_sprite_collision_box[sid][2], RAYDIUM_ODE_STATIC,0,"");
                raydium_ode_element_move_name_3f(cadena,raydium_sprite_pos[sid][0],raydium_sprite_pos[sid][1],raydium_sprite_pos[sid][2]);

                return sid;
        }
        else
        //no such sprite, error
        {
                return -1;
        }
}
*/

//get a pointer to the position(float[3]) of an sprite. Need more testing.
float *raydium_sprite_get_pos(int number)
{
        if(raydium_sprite_used[number]!=0)
                return (float *) raydium_sprite_pos[number];
        else
                return NULL;
}

int raydium_sprite_get_id_from_element(int element)
{
        int a;
        for(a=0;a<raydium_current_sprite;a++)
        {
                if(raydium_sprite_collision_element_id[a]==element)
                return a;
        }
        return -1;
}

//function to change the type of one sprite
int raydium_sprite_set_type(int id,int value)
{
        if(raydium_sprite_used[id]>-1)
        {
                if((raydium_sprite_type[id]=value))
                {
                        return 1;
                }
        }
        raydium_log("Can not change the type of the sprite.");
        return 0;
}

//function to change the name of one sprite
int raydium_sprite_set_name(int id,char *cadena)
{
        if(raydium_sprite_used[id]>-1)
        {
                if(strcpy(raydium_sprite_name[id],cadena))
                {
                        return 1;
                }
        }
        raydium_log("Can not change the name of the sprite.");
        return 0;
}


//function to return the id of one sprite according its name.
//NOTE: the default name of one sprite is the filename of its sprite.
int raydium_sprite_find(char *name)
{
        int a;
        for(a=0;a<raydium_current_sprite;a++)
        {
                if(strcmp(raydium_sprite_name[a],name)==0)
                return a;
        }
        return -1;
}

int raydium_sprite_is_stopped(int id)
{
        return raydium_sprite_stopped[id];
}

float raydium_sprite_change_sprite_time(int id,float time)
{
        raydium_sprite_time[id]=time;
        if(raydium_sprite_time[id]<0)raydium_sprite_time[id]=0;
        return raydium_sprite_time[id];
}

float raydium_sprite_change_sprite_time_relative(int id,float time)
{
        raydium_sprite_time[id]+=time;
        if(raydium_sprite_time[id]<0)raydium_sprite_time[id]=0;
        return raydium_sprite_time[id];
}

int raydium_sprite_get_current_group(int id)
{
        return raydium_sprite_group_current[id];
}

int raydium_sprite_get_current_frame(int id)
{
        return raydium_sprite_current_frame[id];
}

// ODE_SUPPORT
#endif
