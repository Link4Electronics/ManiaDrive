#ifndef _CAMERA_H
#define _CAMERA_H

/*=
Camera
2200
**/

// Introduction
/**
Raydium provides camera management functions, allowing the coder to
move camera with very simple functions, even for complex moves.
You have to place your camera once per frame (not more, not less).

"look_at" style functions can be affected by ##raydium_camera_look_at_roll##
global variable, if needed.

A few words about camera path: Take a look to a .cam file if you want to
understand this simple file format, but you probably only need the ##cam.c##
application, dedicated to camera path creation.

Some camera functions are provided by physics module, see suitable chapter.
**/


__rayapi void raydium_camera_vectors (GLfloat * res3);
/**
This function will return two vectors (2 * 3 * GLfloat), giving the camera
orientation (front vector and up vector). At this day, the up vector is
always the same as the world up vector, even if the camera is rotated
or upside down (and yes, this MUST be corrected :).

Designed for internal uses, before all.
**/

__rayapi void raydium_camera_internal_prepare(void);
/**
Internal use. (pre)
**/

__rayapi void raydium_camera_internal (GLfloat x, GLfloat y, GLfloat z);
/**
Internal use. (post)
**/

__rayapi void raydium_camera_place (GLfloat x, GLfloat y, GLfloat z, GLfloat lacet, GLfloat tangage, GLfloat roulis);
/**
Sets the camera at (x,y,z) position, and using (lacet,tangage,roulis)
as rotation angles.
**/

__rayapi float *raydium_camera_data_get(void);
/**
Function to get the data of the camera in an array of 6 floats.
The first 3 values are the position like x,y,z in universal coordinates.
The next 3 are the rotation angles like r,s,t in degrees and universal orientation.
example:
%%(c)
float *camdata;
...
camdata=raydium_camera_get_data();
raydium_log("pos: %f %f %f rotation: %f %f %f",camdata[0],camdata[1],
 camdata[2],camdata[3],camdata[4],camdata[5]);
...
%%
Returned data is related to the current frame.
**/

__rayapi void raydium_camera_data_set(GLfloat *data);
/**
This will set current camera data (data is copied, no need to keep it
allocated after the call). See above for more information about
this data.
**/

__rayapi void raydium_camera_data_dump(void);
/**
This will display current camera data on the console.
**/


__rayapi void raydium_camera_look_at (GLfloat x, GLfloat y, GLfloat z, GLfloat x_to, GLfloat y_to, GLfloat z_to);
/**
Sets the camera at (x,y,z) position, and looks at (x_to,y_to,z_to).
**/

__rayapi void raydium_camera_replace (void);
/**
You'll need to reset camera position and orientation after each object drawing.
If this is unclear to you, read the "example" section, below.

You will need to make your own 3D transformations (GLRotate, GLTranslate,
...) to draw your objects, or you can use the following function.
**/

__rayapi void raydium_camera_replace_go (GLfloat * pos, GLfloat * R);
/**
This function will replace the camera, as ##raydium_camera_replace()##,
but will place "3D drawing cursor" at position ##pos## (3 GLfloat) with
rotation ##R## (4 GLfloat quaternion).

No eulers (rotx, roty, rotz) version of this function is provided for now..
Do you really need it ?
**/

// Example of camera use
/**
1. place camera
2. move "drawing cursor" to object's place
3. draw object
4. reset camera to initial place (the one given at step 1)
5. move "drawing cursor" to another object's place
6. draw another object
7. [...]

Steps 4 and 5 can be done with raydium_camera_replace_go().
**/

__rayapi void raydium_camera_rumble(GLfloat amplitude, GLfloat ampl_evo, GLfloat secs);
/**
Camera (any type) will rumble for ##secs## seconds, with ##amplitude## (radians).
This ##amplitude## will be incremented of ##ampl_evo## every second (negative
values are allowed for ##ampl_evo##).
An ##amplitude## is always positive.
**/

__rayapi void raydium_camera_smooth (GLfloat px, GLfloat py, GLfloat pz, GLfloat lx, GLfloat ly, GLfloat lz, GLfloat zoom, GLfloat roll, GLfloat step);
/**
Smooth style clone of ##raydium_camera_look_at##.
Roll is given by ##roll## and not global variable ##raydium_camera_look_at_roll##
as for regular look_at function.
##zoom## is the requested FOV.
Play with step to modify smoothing level of the movement. A good way to use
this function is the following usage :
%%(c) raydium_camera_smooth(cam[0],cam[1],cam[2],pos[1],-pos[2],pos[0],70,0,raydium_frame_time*3); %%
**/

__rayapi void raydium_camera_smooth_data_set(GLfloat *data);
/**
This function will allows you to use your own array for smooth camera. The
main purpose of this is to use multiple smooth camera at the same time, for
instance with viewports.

To get back to the "default/primary" internal array of Raydium, use ##NULL##.

##data## is a GLfloat array of ##RAYDIUM_CAMERA_SMOOTH_DATA_SIZE## elements.
**/

__rayapi void raydium_camera_smooth_data_init(GLfloat *data);
/**
This function will initialize a custom smooth camera array.
See ##raydium_camera_smooth_data_set()## above for more informations.
**/

__rayapi void raydium_camera_path_init (int p);
/**
Internal use.
**/

__rayapi void raydium_camera_path_init_all (void);
/**
Internal use.
**/

__rayapi int raydium_camera_path_find (char *name);
/**
Lookups path's id using filename ##name##.
This function will not try to load a camera path if it's not found, and
will return -1.
**/

__rayapi int raydium_camera_path_load (char *filename);
/**
Obvious : use this function to load a camera path.
**/

__rayapi void raydium_camera_path_draw (int p);
/**
Draws ##p## camera path, as red lines. This must be done at each frame.
**/

__rayapi void raydium_camera_path_draw_name (char *path);
/**
Same as above, but using camera path's name.
**/

__rayapi signed char raydium_camera_smooth_path (char *path, GLfloat step, GLfloat * x, GLfloat * y, GLfloat * z, GLfloat * zoom, GLfloat * roll);
/**
Returns the (##x,y,z##) point of the camera path for step ##step##, using
provided ##zoom## (FOV) and ##roll## angle.
It's important to note that ##step## is a float.
Mostly for internal use.
**/

__rayapi void raydium_camera_path_reset(void);
/**
Next smooth call will be instantaneous.
**/

__rayapi void raydium_camera_smooth_path_to_pos (char *path, GLfloat lx, GLfloat ly, GLfloat lz, GLfloat path_step, GLfloat smooth_step);
/**
"Camera on path looking at a point".
Simple ##raydium_camera_smooth## version: give a path name, a "look_at"
point (##lx,ly,lz##), a current ##step##, anda ##smooth_step## time
factor (see ##raydium_camera_smooth## example above).
**/

__rayapi void raydium_camera_smooth_pos_to_path (GLfloat lx, GLfloat ly, GLfloat lz, char *path, GLfloat path_step, GLfloat smooth_step);
/**
"Camera on point looking at a path".
Same style as previous function.
**/

__rayapi void raydium_camera_smooth_path_to_path (char *path_from, GLfloat path_step_from, char *path_to, GLfloat path_step_to, GLfloat smooth_step);
/**
"Camera on a path looking at another path".
Same style as previous functions.
**/

__rayapi void raydium_viewport_init(void);
/**
Init of raydium_viewport array to support up to
##RAYDIUM_VIEWPORT_MAX## viewport
**/

__rayapi void raydium_viewport_create (char * name,int tx,int ty);
/**
Create a texture for saving viewport display
Texture size ##tx## and ##ty## must be related with final displayed
viewport size.
**/

__rayapi void raydium_viewport_enable(char * name);
/**
Direct all render operations to dedicated viewport.
Advanced camera feature as sound/rumble are desactivated during viewport render.
**/

__rayapi void raydium_viewport_save(void);
/**
Copy viewport rendering to texture buffer.
Render operations return to normal state, with normal camera behavior.
**/

__rayapi void raydium_viewport_draw(char * name, GLfloat tx,GLfloat ty,GLfloat sx,GLfloat sy);
/**
Draw contents of ##name## viewport to screen
##tx##,##ty## are lower left corner in screen percents.
##sx##,##sy## are size in screen percents.
You can create mirrors by using a negative ##sx## (and update ##tx##).
Example:
%%(c)
// this is the mirror:
raydium_viewport_draw("camera2",80,70,-60,30);

// ... of this:
raydium_viewport_draw("camera2",20,70,60,30);
%%
**/

__rayapi void raydium_viewport_direct_open(int x, int y, int sizex, int sizey);
/**
Starts a "direct viewport". Direct viewports are not stored in a texture, but
are directly rendered on the screen. It's a bit less costly than classic
viewports, match the screen pixel-by-pixel, and use a custom width/height ratio.

Direct viewports should be opened when needed, most of the time at the end of
the display loop, just before raydium_rendering_finish() and all OSD
things.

All parameters are given in pixels.

An interesting note is that ##raydium_mouse_x## and y positions are translated
into the viewport, in the ##raydium_window_x## and y "real" screen size, meaning
that any code already using these variables will continue to work as-is in
viewports.
**/

__rayapi void raydium_viewport_direct_open_4f(GLfloat x, GLfloat y, GLfloat sizex, GLfloat sizey);
/**
Same as ##raydium_viewport_direct_open()##, but arguments are given in percent
like all OSD functions, with the same lower-left origin.
**/

__rayapi void raydium_viewport_direct_close(void);
/**
Close the direct viewport rendering. This functions tries to restore all
the camera context, but you should be aware that a few things could break
when called after a direct viewport.
**/

__rayapi signed char raydium_viewport_direct_is_mouse_in(void);
/**
Reports true (##1##) if mouse in in the current direct viewport.
Useful to detect if the user clicked the viewport, for instance.
**/

__rayapi void raydium_camera_freemove(int move);
/**
This function is a fast & easy way to create a working camera that can move
with keys and can be rotated with the mouse, like a usual camera in a
First Person Shooter(FPS) game.
Its sources could be also interesting as an example of implementation of this
kind of camera, since this function can be a bit limited if you want to do
complex things.
You can modify the global variables ##raydium_camera_freemove_sensibility##
and ##raydium_camera_freemove_speed##, mouse sensibility (3 default) and
movement speed (0.1 default) respectively.
If ##move## is ##RAYDIUM_CAMERA_FREEMOVE_FIXED##, camera is only placed and fixed.
Else if ##move## is ##RAYDIUM_CAMERA_FREEMOVE_NORMAL## camera position and
orientation are updated with Mouse/Keyboard.
Last, ##RAYDIUM_CAMERA_FREEMOVE_MOUSE## only allow mouse view (and no move).
**/

__rayapi void raydium_camera_orbitmove(float x_to, float y_to, float z_to);
/**
EXPERIMENTAL: This functions allows to rotate (in fact orbit) around a
certain point (x_to,y_to,z_to). The distance of the point to the camera
 can be changed with the up/down keys and with the mouse wheel.
**/

__rayapi void raydium_camera_data_reset(void);
/**
Resets camera internal data. Useful for some camera switches.
**/

__rayapi void raydium_camera_push(int type, GLfloat *vect, GLfloat *slowness);
/**
This function will push (or "punch", if you give high values :) the camera.

The ##vect## parameter is the vector to the "destination", and higer is
the ##slowness## value, the slower you'll get there (quick sample values for
slowness: 1 will look slow, 0.1 will look quick)

Both parameters needs dReal[3] arrays, ##slowness## is always positive.

Here's the currently supported modes:
 - ##RAYDIUM_CAMERA_PUSH_FREEMOVE_ABS## for freemove camera, in world coords
 - ##RAYDIUM_CAMERA_PUSH_FREEMOVE_REL## for freemove camera, using current camera vector
**/

__rayapi void raydium_camera_init(void);
__rayapi GLfloat *raydium_camera_push_internal_step(void);

#endif
