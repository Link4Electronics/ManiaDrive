#ifndef _SPRITES_H
#define _SPRITES_H
#ifdef ODE_SUPPORT
/*=
Sprites (viewer axis aligned 2D billboards)
2900
**/

// Introduction
/**
Raydium provides its own sprite system.
Each sprite needs a .sprite file. This file is as this:
%%collision={0.8,0.7,0.8};
size=1;

coords={0.0,0.25,0.0,0.25};
group=1;
texture="sprite1-test.tga";

coords={0,0.25,0.5,0.75};
group=2;
texture="sprite1-test.tga";

...%%
The ##collision## variable defines the size of a RayODE box element.

##size## is the size of the displayed sprite. It won't affect the ODE object,
just the graphical stuff.

Then you'll have to define each frame of the sprite. Each frame must be
defined by ##coords## (interval 0 to 1) in a ##texture## file.

It's important to have in mind that the sprite can have "groups".
Those groups are used to team up some related sprites. For example we can
have 3 sprites of a forward movement, those could be in one specific group.
In that way you can change from one group to another with a Raydium function easily.

Even more, the frames of a sprite group will be animated automatically
and when the animation comes to the end of the group then you can chose
what will be the next action:
1) You can stop animation, you can restart the group animation or you can
jump to a new group.
2) you can indicate a "jump" to another group with something like:
%%group={7,11}; That would jump to the group 11
group={5,-1}; -1 means STOP THE ANIMATION
group={3,-2}; -2 means LOOP IN THE SAME GROUP
%%

Raydium provides a sprite viewer (sprite_viewer.c) that will download
a sample sprite file, very useful to understand how sprite file are built.
**/

//returns the first sprite id available
__rayapi int raydium_sprite_check_available(void);
/**
Internal use.
**/

//function to draw directly the sprite. Internal. Don't use
__rayapi void raydium_sprite_billboard(float x, float y, float z,float ux, float uy, float uz, float rx, float ry, float rz, int textureid, float s0, float s1, float t0, float t1,float size);
/**
Internal use.
**/
//Load an sprite (filename.sprite) preloading its textures.
//TODO:it should check if the sprite file is already loaded and if it's true
//TODO:clean file loading, ugly.
//then it should ignore the file load and get the texture id.

__rayapi int raydium_sprite_load(char *fichero,int etiqueta);

/**
This function allows you to load an sprite. It will returns the id(int)
of the new sprite.
**/

//get the name (char*) of an sprite from its related ODE object id
__rayapi char *raydium_sprite_get_name_from_object(int obj);
/**
You can get the name of an sprite with this function.
**/

//get the ODE object id from a certain sprite according its spriteid
__rayapi int raydium_sprite_object_get(int spriteid);
/**
Function to get the ODE object linked to the sprite.
**/

__rayapi void sprite_render_frame(float x, float y, float z, int spriteid,int frame,float scalex,float scaley);
/**
Internal use.
**/

__rayapi void raydium_sprite_move(int sprite,float x, float y, float z);
/**
With this function you can move (in univsrese coordinates) an sprite
giving the 3 coordinates.
**/

__rayapi void raydium_sprite_move_relative(int sprite, float deltax, float deltay, float deltaz);
/**
Same than previous but it uses relative coordinates. Usefull for ingame
displacements.
**/

__rayapi void raydium_sprite_display(int id);
/**
This function will display the given sprite. Usually should be called in
the display callback.
**/

//function to change the animation group of one sprite
__rayapi void raydium_sprite_group_change(int sprite,int group);
/**
Function to change the animation group of a certain sprite.
**/

__rayapi void raydium_sprite_free(int sprite);
/**
Deletes an sprite.
**/

__rayapi void raydium_sprite_free_name(char *name);
/**
Function to delete all the sprites using a certain filename.
**/

__rayapi float *raydium_sprite_get_pos(int number);
/**
Returns a 3float array with the position (universe coordinates) of a
frame
**/

__rayapi int raydium_sprite_get_id_from_element(int element);
/**
Returns the id of an sprite from the element id given.
**/

//function to change the type of one sprite
__rayapi int raydium_sprite_set_type(int id,int value);
/**
Internal use.
**/

//function to change the name of one sprite
__rayapi int raydium_sprite_set_name(int id,char *cadena);
/**
With this function you can change the name of an sprite
**/

//function to return the id of one sprite according its name.
//NOTE: the default name of one sprite is the filename of its sprite.
__rayapi int raydium_sprite_find(char *name);
/**
This function will return the sprite id(int) of an sprite according its
name
**/

__rayapi int raydium_sprite_is_stopped(int id);
/**
This functions returns 1 if the animation of the sprite id has been
stoppped and 0 if it's playing.
**/

__rayapi void raydium_sprite_dump_info(int id);
/**
Dump of jumps for a certain sprite.
**/

__rayapi float raydium_sprite_change_sprite_time(int id,float time);
/**
Function to change the time betwen frames in the animation of an id sprite.
**/


__rayapi float raydium_sprite_change_sprite_time_relative(int id,float time);
/**
Same than previous but you can add a quantity of time to the previous
stored time, ie relative.
**/

__rayapi int raydium_sprite_get_current_group(int id);
/**
Function to get the current group of an sprite.
**/

__rayapi int raydium_sprite_get_current_frame(int id);
/**
Function to get the current frame of an sprite.
**/

// ODE_SUPPORT
#endif
#endif
