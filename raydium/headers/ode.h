#ifndef _ODE_H
#define _ODE_H
#include "../ode.h"

/*=
Integrated Physics (ODE)
3900
**/

// Introduction
/**
Raydium allows you to build applications with full physics, using ODE (Open
Dynamics Engine). ODE is "an open source, high performance library for
simulating rigid body dynamics", and is fully integrated into Raydium, with
the usual abstraction. You can build cars, ragdolls, rockets, ... with
only few lines of code. Physics are linked to sound API, particles engine,
network layer, ... so you've almost nothing else to do but setting up objects.

Raydium's website provides tutorials for building physics ready applications.
**/


// Vocabulary
/**
Raydium physics use a simple vocabulary, with a few entities :
- **Objects:**
Objects are containers, with no direct visual appearance. An object contains
elements and joints (see below). By default, all elements in an object
doesn't collide each others. "Car", "Player", "Crane" are good object examples.

- **Elements:**
Elements are the main thing you will play with. An element is rendered using
an associated 3D mesh, is configured with a geometry, density, a size,
collides with others elements, ...
An element **must** be owned by an object.
For now, there is 3 element types (standard, satic, fixing). Static elements
are unmovable, they just collide with other elements, usefull for very
big elements, or externally controlled elements (motion capture, network,
haptic interface, ...), for example.
Raydium supports boxes and spheres.

- **Joints:**
Joints are dedicated to elements linking. A joint **must** be linked with two
elements or unwanted behaviors may happen.
For now, Raydium supports 4 joint types (hinge, hinge2, universal, fixed), and
you will find more informations with suitable functions documentation, below.
On a joint, you could setup limits (min and max for all axes) and a maximum
force before joint breaks, if needed.
It's now possible to attach a joint to static environnement using the
constant ##RAYDIUM_ODE_JOINT_FIXED## (do not use this value with "_name" joint
functions, since they want a string, not a integer constant).

- **Motors:**
A motor is linked to joints, and may powering an unlimited amount of joints.
For now, 3 motor types are available: engine, angular and rocket.

**Engine** type works the same way as a car's engine: it will try to
make "something" turning, at the desired **speed**. You can link a
gearbox to this type (and only this one).

**Angular** type will try to rotate "something" to the desired **angle**,
usefull for car's front wheels, for example.

**Rocket type** is very simple: give a force and an orientation. Usefull for
creating copters, rockets, and for elements "pushing", for example.
Special rocket is avaiblable for FPS style player controls.
Warning, a rocket is linked to an element ! (not a joint)

- **Explosions:**
Explosions are not directly related to rigid body physics, but consider it
as a high level primitive.
With Raydium, you have two different ways to create an explosion.

First, you can create a "blowing explosion", generating a spherical blow. Any
element in this growing sphere will be ejected.
Use this for very consequent explosions only !

Next, you can create an instantaneous explosion, with a degressive blowing
effect. A force is applied to every body found inside the blowing radius,
proportional to distance from the explosion's center. Usefull for smaller
explosions.

- **Launchers:**
Launchers are not real entities, but "only" tools. Obviously, they are
allowing you to launch an element (you must provice force and orientation)
from another element (relatively). More informations about launchers below.

**/

// About RayODE names
/**
 - Important: All ODE Objects, elements, motors,... **must have unique name** !
So, here is an example of what you **SHOULD NOT DO**:
%%(c)
int car1=raydium_ode_object_create("car");
int car2=raydium_ode_object_create("car");
int car3=raydium_ode_object_create("car");
%%

Ant this is the right way:
%%(c)
int car1=raydium_ode_object_create("car1");
int car2=raydium_ode_object_create("car2");
int car3=raydium_ode_object_create("car3");
%%
**/

// Callbacks
/**
For **advanced** uses, you may want to enter into some "internal" parts of
RayODE. Many callbacks are available for such needs.
To cancel any callback, set its value to ##NULL## (default value).
Here is a quick list:

- ##raydium_ode_StepCallback##
This callback is fired before every ODE callback. Since physcis callback
frequency may change (see ##raydium_ode_time_change##) during slow motion
scenes, for example, this callback is quiet useful.
Callback prototype: ##void f(void);##


- ##raydium_ode_ObjectNearCollide##
When two objects are too near, before lauching "expensive" collision tests,
Raydium is firing this event.

Callback prototype: ##signed char f(int obj1, int obj2);##
##obj1## and ##obj2## are the two objets, and you must return true (1) if
you want to "validate" collision, or false (0) if you don't want that two
objects to collide.


- ##raydium_ode_CollideCallback##
When two objects collides, Raydium will search all collisions between
every elements. For each contact, this callback is fired. For complex
objects, with a lot of elements, this callback may be fired a **very** large
number of times during **one** ODE step ! Do only simple things here.

Callback prototype: ##signed char f(int e1, int e2, dContact *c);##
##e1## and ##e2## are the two colliding elements, and you must return true (1)
if you want to "validate" this contact, or false (0) to cancel this contact
(and only this one !)

See ODE documentation, chapter 7.3.7, for more informations about ##dContact##
structure.


- ##raydium_ode_ExplosionCallback##
At every explosion, of any type, this event is fired. This is the best
place to play suitable sound, create particles and such things.

Callback prototype: ##void f(signed char type, dReal radius, dReal force_or_propag, dReal *pos);##
You can find in callback params:
explosion ##type## (see above), ##radius##, force or propag (depending on
explosion type) and ##pos##, an array of 3 dReal values for explosion position.
The value you will find in ##force_or_propag## is something
like ##RAYDIUM_ODE_NETWORK_EXPLOSION_*## (EXPL or BLOW).


- ##raydium_ode_BeforeElementDrawCallback##
When ##raydium_ode_draw_all(RAYDIUM_ODE_DRAW_NORMAL)## is called, for every
element to draw, this callback is **before** element drawing.

Callback prototype: ##signed char f(int elem);##
##elem## is the element'id. Return true (1) if you want to draw this element,
or false (0) otherwise. This is also the best place to drawn team colors on
cars, for example (see ##raydium_rendering_rgb_force## for this use).


- ##raydium_ode_AfterElementDrawCallback##
Same as the previous callback, but **after** element drawing.

Callback prototype: ##void f(int elem);##
With the previous example (team colors), this is the place to restore
default rendering state (see ##raydium_rendering_rgb_normal##).

- ##raydium_ode_RayCallback##
See ray related functions, below. This callback is used to filter
elements and create contacts during a ray launch.
**/


// Miscallenous
/**
By default, ODE is called 400 times per second, allowing **very** accurate
physics. You may change this using ##raydium_ode_set_physics_freq##,
but most ERP and CFM values must be changed in your
applications. ODE use a lot of cache mechanisms, so 400 Hz is a reasonable value.

Please note RayODE interface is using ##dReal## ODE type for variables.
For now, ##dReal## is an alias to ##float## type. But please use ##sizeof()##.

Raydium provides some other functions for advanced uses, and you can
access directly to ODE API for very experienced users.

See also the ODE documentation: http://opende.sourceforge.net/ode-latest-userguide.html
**/


__rayapi void raydium_ode_name_auto (char *prefix, char *dest);
/**
This function will generate a single name, using ##prefix##. The generated
name is stored at ##dest## address. No memory allocation is done.
Example : ##raydium_ode_name_auto("prefix",str)## may generate something
like ##prefix_ode_0##.
**/

__rayapi void raydium_ode_init_object (int i);
/**
Will initialize (or erase) object ##i##. Mostly for internal uses.
**/

__rayapi void raydium_ode_init_element (int i);
/**
Will initialize (or erase) element ##i##. Mostly for internal uses.
**/

__rayapi void raydium_ode_init_joint (int i);
/**
Will initialize (or erase) joint ##i##. Mostly for internal uses.
**/

__rayapi void raydium_ode_init_motor (int i);
/**
Will initialize (or erase) motor ##i##. Mostly for internal uses.
**/

__rayapi void raydium_ode_init_explosion (int e);
/**
Will initialize (or erase) spherical explosiion ##i##. Mostly for internal uses.
**/

__rayapi void raydium_ode_init (void);
/**
Will initialize all RayODE interface. Never call this function by yourself.
**/

__rayapi void raydium_ode_delete_all(void);
/**
Will delete all entities, including the ground.
**/

__rayapi void raydium_ode_gravity(dReal *vect);
/**
Change world gravity using ##vect##, a 3 floats arrays.
Default value is {0,0,-1}.
**/

__rayapi void raydium_ode_gravity_3f(dReal gx, dReal gy, dReal gz);
/**
Same as above, using 3 float values.
**/

__rayapi signed char raydium_ode_object_isvalid (int i);
/**
Will return 0 (false) if object ##i## is not valid (free slot or out of bounds)
or 1 (true) otherwise.
**/

__rayapi signed char raydium_ode_element_isvalid (int i);
/**
Will return 0 (false) if element ##i## is not valid (free slot or out of bounds)
or 1 (true) otherwise.
**/

__rayapi signed char raydium_ode_joint_isvalid (int i);
/**
Will return 0 (false) if joint ##i## is not valid (free slot or out of bounds)
or 1 (true) otherwise.
**/

__rayapi signed char raydium_ode_motor_isvalid (int i);
/**
Will return 0 (false) if motor ##i## is not valid (free slot or out of bounds)
or 1 (true) otherwise.
**/

__rayapi signed char raydium_ode_explosion_isvalid (int i);
/**
Will return 0 (false) if explosion ##i## is not valid (free slot or out of bounds)
or 1 (true) otherwise.
**/

__rayapi void raydium_ode_ground_dTriArrayCallback (dGeomID TriMesh, dGeomID RefObject, const int *TriIndices, int TriCount);
/**
Internal. Unsupported.
**/

__rayapi int raydium_ode_ground_dTriCallback (dGeomID TriMesh, dGeomID RefObject, int TriangleIndex);
/**
Internal. Unsupported.
**/

__rayapi int raydium_ode_ground_set_name (char *name);
/**
##ground## is a primitive for RayODE interface. You only have to set ground
mesh ##name## (.tri file). You may call this function many times, if needed,
switching from one ground to another on the fly.
**Warning**: triangle normals are very important for ground models !
**/

__rayapi int raydium_ode_object_find (char *name);
/**
Resolves object id from its ##name##.
**/

__rayapi int raydium_ode_element_find (char *name);
/**
Resolves element id from its ##name##.
**/

__rayapi int raydium_ode_joint_find (char *name);
/**
Resolves joint id from its ##name##.
**/

__rayapi int raydium_ode_motor_find (char *name);
/**
Resolves motor id from its ##name##.
**/

__rayapi int raydium_ode_explosion_find (char *name);
/**
Resolves explosion id from its ##name##.
**/

__rayapi int raydium_ode_object_create (char *name);
/**
Will build a new object with ##name##. Returns new object id, or -1 when
it fails.
**/

__rayapi signed char raydium_ode_object_rename (int o, char *newname);
/**
Will rename object ##o## with a ##newname##.
**/

__rayapi signed char raydium_ode_object_rename_name (char *o, char *newname);
/**
Same as above, but from object's name (##o##).
**/

__rayapi signed char raydium_ode_object_colliding (int o, signed char colliding);
/**
By default, all elements from an object are not colliding each others.
The only exception is for ##GLOBAL## object.
If you want to change this behaviour for ##o## object, sets ##colliding##
to 1 (true). 0 (false) sets back to default behaviour (no internal collisions).
**/

__rayapi signed char raydium_ode_object_colliding_name (char *o, signed char colliding);
/**
Same as above, but using object's name.
**/

__rayapi void raydium_ode_object_linearvelocity_set (int o, dReal * vect);
/**
Sets linear velocity for all elements of object ##o##. Velocity is sets thru
##vect##, a 3 x dReal array.
Use with caution, setting an arbitrary linear velocity may cause unwanted
behaviours.
**/

__rayapi void raydium_ode_object_linearvelocity_set_name (char *o, dReal * vect);
/**
Same as above, but using object's name.
**/

__rayapi void raydium_ode_object_linearvelocity_set_name_3f (char *o, dReal vx, dReal vy, dReal vz);
/**
Same as above, but using 3 dReal values.
**/

__rayapi void raydium_ode_object_addforce (int o, dReal *vect);
/**
Add force ##vect## to all elements of object ##o##.
Force is sets thru ##vect##, a 3 x dReal array.
Prefer this method to ##..._linearvelocity_set...## functions.
**/

__rayapi void raydium_ode_object_addforce_3f (int o, dReal vx, dReal vy, dReal vz);
/**
Same as above, but using 3 dReal values.
**/

__rayapi void raydium_ode_object_addforce_name (char *o, dReal * vect);
/**
Same as above, but using object's name.
**/

__rayapi void raydium_ode_object_addforce_name_3f (char *o, dReal vx, dReal vy, dReal vz);
/**
Same as above, but using 3 dReal values.
**/

__rayapi void raydium_ode_element_addforce (int e, dReal *vect);
/**
Adds force ##vect## to element ##e##.
Force is sets thru ##vect##, a 3 x dReal array.
**/

__rayapi void raydium_ode_element_addforce_3f (int e, dReal fx, dReal fu, dReal fz);
/**
Same as above, but using 3 dReal values.
**/

__rayapi void raydium_ode_element_addforce_name (char *e, dReal * vect);
/**
Same as above, but using element's name and one vector.
**/

__rayapi void raydium_ode_element_addforce_name_3f (char *e, dReal vx, dReal vy, dReal vz);
/**
Same as above, but using 3 dReal values.
**/

__rayapi void raydium_ode_element_addrelforce (int e, dReal *vect);
/**
Adds, relative to element orientation, force ##vect## to element ##e##.
Force is sets thru ##vect##, a 3 x dReal array.
**/

__rayapi void raydium_ode_element_addrelforce_3f (int e, dReal fx, dReal fu, dReal fz);
/**
Same as above, but using 3 dReal values.
**/

__rayapi void raydium_ode_element_addrelforce_name (char *e, dReal * vect);
/**
Same as above, but using element's name and one vector.
**/

__rayapi void raydium_ode_element_addrelforce_name_3f (char *e, dReal vx, dReal vy, dReal vz);
/**
Same as above, but using 3 dReal values.
**/

__rayapi dReal * raydium_ode_element_force_get(int e);
/**
Return forces accumulated by a given body;
**/

__rayapi dReal * raydium_ode_element_force_get_name(char * elem);
/**
Same as above with the name.
**/

__rayapi void raydium_ode_element_addtorque (int e, dReal * vect);
/**
Adds torque ##vect## to element ##e##.
Torque is sets thru ##vect##, a 3 x dReal array.
**/

__rayapi void raydium_ode_element_addtorque_3f(int e, dReal vx, dReal vy, dReal vz);
/**
Same as above using 3 dReal values.
**/

__rayapi void raydium_ode_element_addtorque_name (char *e, dReal * vect);
/**
Same as above, but using element's name.
**/

__rayapi void raydium_ode_element_addtorque_name_3f (char *e, dReal vx, dReal vy, dReal vz);
/**
Same as above, but using 3 dReal values.
**/

__rayapi signed char raydium_ode_element_material (int e, dReal erp, dReal cfm);
/**
When two elements collides, there's two important parameters used for
contact point generation : ERP and CFM.
ERP means "Error Reduction Parameter", and its value is between 0 and 1 and
CFM means "Constraint Force Mixing".
Changing ERP and CFM values will change the way this element collides with
other elements, providing a "material" notion.
Raydium provides a few default values, see ##RAYDIUM_ODE_MATERIAL_*## defines
in ##raydium/ode.h## file (hard, medium, soft, soft2, default, ...).

For more informations, see ODE documentation, chapters 3.7 and 3.8.
**/

__rayapi signed char raydium_ode_element_material_name (char *name, dReal erp, dReal cfm);
/**
Same as above, but using element's name.
**/

__rayapi signed char raydium_ode_element_slip (int e, dReal slip);
/**
Slip parameter is a complement of material values (ERP and CFM, see above).
Raydium provides a few default values, see ##RAYDIUM_ODE_SLIP_*## defines
in ##raydium/ode.h## file (ice, player, normal, default).
**/

__rayapi signed char raydium_ode_element_slip_name (char *e, dReal slip);
/**
Same as above, but using element's name.
**/

__rayapi signed char raydium_ode_element_rotfriction (int e, dReal rotfriction);
/**
rotfriction stands for "Rotation Friction", "Rolling Friction",
"Damping Effect", ...
With RayODE, by default, when a sphere element is rolling over a flat ground,
it will roll forever. Applying a rotfriction factor will solve this.
A value of 0 will disable rotation friction.
Example: %%(c)
#define ROTFRICTION     0.0005
raydium_ode_element_rotfriction(elem,ROTFRICTION);%%
**/

__rayapi signed char raydium_ode_element_rotfriction_name (char *e, dReal rotfriction);
/**
Same as above, but using element's name.
**/

__rayapi dReal *raydium_ode_element_linearvelocity_get (int e);
/**
Returns a pointer to element's linear velocity. Linear velocity is an
array of 3 x dReal.
Example: %%(c)
dReal *p;
p=raydium_ode_element_linearvelocity_get(elem);
raydium_log("%f %f %f",p[0],p[1],p[2]);
%%
Returned data is available only for the current frame.
**/

__rayapi dReal *raydium_ode_element_linearvelocity_get_name(char *e);
/**
Same as above using element name.
**/

__rayapi void raydium_ode_element_rellinearvelocity_get (int e,dReal *vel);
/**
Returns element's linear velocity relative to local element axis orientation.
Linear velocity is an array of 3 x dReal.
Example: %%(c)
dReal vel[3];
raydium_ode_element_linearvelocity_get(elem,vel);
raydium_log("%f %f %f",vel[0],vel[1],vel[2]);
%%
Returned data is available only for the current frame.
**/

__rayapi void raydium_ode_element_rellinearvelocity_get_name(char *e, dReal *vel);
/**
Same as above using element name.
**/


__rayapi void raydium_ode_element_linearvelocity_set (int e, dReal *vel);
/**
Set element's linear velocity. Linear velocity is an
array of 3 x dReal.
**/

__rayapi void raydium_ode_element_linearvelocity_set_name (char * e, dReal *vel);
/**
Same as above usign element's name.
**/

__rayapi void raydium_ode_element_linearvelocity_set_3f (int e, dReal velx, dReal vely, dReal velz);
/**
Same as above, using 3 dReal values.
**/

__rayapi void raydium_ode_element_linearvelocity_set_name_3f(char *e, dReal vx, dReal vy, dReal vz);
/**
Same as above using element name.
**/

__rayapi void raydium_ode_element_angularvelocity_set (int e,dReal *avel);
/**
Set element's angular velocity. Angular velocity is an
array of 3 x dReal.
**/

__rayapi void raydium_ode_element_angularvelocity_set_3f (int e,dReal avelx,dReal avely,dReal avelz);
/**
Same as above, 3 dReal.
**/

__rayapi void raydium_ode_element_angularvelocity_set_name (char * e,dReal *avel);
/**
Set element's angular velocity using it's name. Angular velocity is an
array of 3 x dReal.
**/

__rayapi void raydium_ode_element_angularvelocity_set_name_3f (char * e,dReal avelx,dReal avely,dReal avelz);
/**
Same as above, 3 dReal.
**/

__rayapi dReal * raydium_ode_element_angularvelocity_get(int elem);
/**
Return element angular velocity. Return is an array of 3 real.
**/

__rayapi void raydium_ode_element_angularvelocity_get_3f(int elem, dReal * vrx, dReal * vry, dReal * vrz);
/**
Same as above, 3 dReal.
**/

__rayapi dReal * raydium_ode_element_angularvelocity_get_name(char *elem);
/**
Same as above using element name.
**/

__rayapi void raydium_ode_element_OnBlow (int e, void *OnBlow);
/**
During an instantaneous explosion, all elements inside the blow's radius may
fire an OnBlow callback (event), if set.
##OnBlow## callback must follow this prototype :
##void blow_touched(int elem, dReal force, dReal max_force)##

##elem## is the element id.
##force## is the amount of force received from explosion.
##max_force## is the amount of force at the core of the explosion.

Sets ##OnBlow## to ##NULL## if you want to disable this callback.
**/

__rayapi void raydium_ode_element_OnBlow_name (char *e, void *OnBlow);
/**
Same as above, but using element's name.
**/

__rayapi void raydium_ode_element_OnDelete (int e, void *OnDelete);
/**
OnDelete callback is fired when someone or something tries to delete an element.
This callback can cancel deletion, if needed.

##OnBlow## callback must follow this prototype :
##int element_delete(int elem)##
##elem## is the element id. Return 1 (true) to confirm deletion, of 0 to cancel.

Sets ##OnDelete## to ##NULL## if you want to disable this callback.
**/

__rayapi void raydium_ode_element_OnDelete_name (char *e, void *OnDelete);
/**
Same as above, but using element's name.
**/

__rayapi void raydium_ode_object_OnDelete (int o, void *OnDelete);
/**
OnDelete callback is fired when someone or something tries to delete an object.
This callback can cancel deletion, if needed.

##OnDelete## callback must follow this prototype :
##int object_delete(int obj)##
##obj## is the object id. Return 1 (true) to confirm deletion, of 0 to cancel.

Sets ##OnDelete## to ##NULL## if you want to disable this callback.
**/

__rayapi void raydium_ode_object_OnDelete_name (char *o, void *OnDelete);
/**
Same as above, but using object's name.
**/

__rayapi void raydium_ode_element_gravity (int e, signed char enable);
/**
By default, gravity applies to every element of the scene. If you want
to disable gravity for element ##e##, set ##enable## to 0 (false).
You can restore gravity with ##enable## sets to 1 (true).
**/

__rayapi void raydium_ode_element_gravity_name (char *e, signed char enable);
/**
Same as above, but using element's name.
**/

__rayapi void raydium_ode_element_ttl_set (int e, int ttl);
/**
TTL means Time To Live. Setting a TTL on an element will automatically
delete this element when TTL expires.

- TTL unit: ##ttl## is given in ODE steps (see example, below).
- TTL deletion may be canceled by OnDelete callback (see above).
- TTL may be changed on the fly, at anytime.
- a ##ttl## value of -1 will disable TTL.

example: %%(c)
raydium_ode_element_ttl_set(elem,raydium_ode_physics_freq*5); // 5 seconds
%%
**/

__rayapi void raydium_ode_element_ttl_set_name (char *e, int ttl);
/**
Same as above, but using element's name.
**/

__rayapi signed char raydium_ode_element_rel2world(int element,dReal *rel,dReal *world);
/**
Utility function that take a point on a ##element## (##rel## is a dReal[3]) and
return  that point's position or velocity in world coordinates (##world## is
a dReal[3] too).

No memory allocation is done here.

This function can be used on normal and static elements.
Note: This is a "Raydium clone" of dBodyGetRelPointPos.
**/

__rayapi signed char raydium_ode_element_world2rel(int element,dReal *world,dReal *rel);
/**
Inverse of ##raydium_ode_element_rel2world.## Input (##world##) is in world
coordinates and ouput (##rel##) is in ##element##'s relative space.

No memory allocation is done here.

This function can be used on normal and static elements.
Note: This is a "Raydium clone" of dBodyGetPosRelPoint.
**/

__rayapi signed char raydium_ode_element_vect2world(int element,dReal *vect,dReal *world);
/**
Given a vector expressed in the ##element## coordinate system (dReal[3]),
rotate it to the world coordinate system (##world##, dReal[3]).

No memory allocation is done here.

This function can be used on normal and static elements.
Note: This is a "Raydium clone" of dBodyVectorToWorld.
**/

__rayapi signed char raydium_ode_element_world2vect(int element,dReal *world,dReal *vect);
/**
Given a vector (world) expressed in the ##world## coordinate system (dReal[3]),
rotate it to the element coordinate system (##vect##, dReal[3]).

No memory allocation is done here.

This function can be used on normal and static elements.
Note: This is a "Raydium clone" of dBodyVectorFromWorld.
**/

__rayapi signed char raydium_ode_element_aabb_get (int element, dReal * aabb);
/**
AABB means Axis-Aligned Bounding Box. This function will return element's
bounding box on X, Y and Z axis.

##aabb## is a pointer to an array of 6 x dReal. The aabb array has
elements (minx, maxx, miny, maxy, minz, maxz).
No memory allocation is done.
Will return 0 (false) in case of failure.
**/

__rayapi signed char raydium_ode_element_aabb_get_name (char *element, dReal * aabb);
/**
Same as above, but using element's name.
**/

__rayapi int raydium_ode_element_touched_get (int e);
/**
Every element provide a "touched" flag. If element ##e## is touching anything,
this function will return 1 (true).
**/

__rayapi int raydium_ode_element_touched_get_name (char *e);
/**
Same as above, but using element's name.
**/

__rayapi signed char raydium_ode_element_player_set (int e, signed char isplayer);
/**
RayODE provides a special behaviour for FPS style players, also
named "standing geoms". The idea is simple : a player element is always
upright, and you can set an arbitrary rotation angle around Z axis anytime.
Sets ##isplayer## to 1 (true) to transform element ##e## into a "player element".
**/

__rayapi signed char raydium_ode_element_player_set_name (char *name, signed char isplayer);
/**
Same as above, but using element's name.
**/

__rayapi signed char raydium_ode_element_player_get (int e);
/**
Returns if element ##e## is a "player element" (1, true) or not (0, false).
See above for more informations about player elements.
**/

__rayapi signed char raydium_ode_element_player_get_name (char *name);
/**
Same as above, but using element's name.
**/

__rayapi signed char raydium_ode_element_player_angle (int e, dReal angle);
/**
Sets "standing geom" Z rotation ##angle## (radian) for element ##e##.
See above for more informations about player elements.
**/

__rayapi signed char raydium_ode_element_player_angle_name (char *e, dReal angle);
/**
Same as above, but using element's name.
**/

__rayapi int raydium_ode_element_ground_texture_get (int e);
/**
Unsupported. Do not use for now.
**/

__rayapi int raydium_ode_element_ground_texture_get_name (char *e);
/**
Unsupported. Do not use for now.
**/

__rayapi int raydium_ode_element_object_get (int e);
/**
Since every element is owned by an object, this function will return
the owner's object id.
**/

__rayapi int raydium_ode_element_object_get_name (char *e);
/**
Same as above, but using element's name.
**/

__rayapi int raydium_ode_object_sphere_add (char *name, int group, dReal mass, dReal radius, signed char type, int tag, char *mesh);
/**
This function will add an new "sphere" element to an object (##group##).
You must provide:
- ##name##: **single** name for this new element.
- ##group##: owner object id.
- ##mass##: density of this new element. Mass will depend on radius.
- ##radius##: radius of the element sphere geometry. Raydium is able to
detect this value with ##RAYDIUM_ODE_AUTODETECT##. Things like
##RAYDIUM_ODE_AUTODETECT*2## are ok, meaning "twice the detected radius".
- ##type##: ##RAYDIUM_ODE_STANDARD## or ##RAYDIUM_ODE_STATIC## (collide only,
no physics).
- ##tag##: use this integer value as you want. The important thing is that
this value is sent to network, and will be available on every connected peer.
This tag must be greater or equal to 0. Suitable functions are available
to read back this value later on an element.
- ##mesh##: 3D model used for rendering this element. Use an empty string to
disable rendering (and not ##NULL## !), and avoid ##RAYDIUM_ODE_AUTODETECT##
int this case.
**/

__rayapi int raydium_ode_object_box_add (char *name, int group, dReal mass, dReal tx, dReal ty, dReal tz, signed char type, int tag, char *mesh);
/**
This function will add an new "box" element to an object (##group##).
Arguments are the same as ##raydium_ode_object_sphere_add## (see above) but
##tx##, ##ty## and ##tz##, used for box sizes. As for spheres, you can
use ##RAYDIUM_ODE_AUTODETECT##. Give this value only for ##tx##, this will
automatically apply to ##ty## and ##tz##.
Again, Things like  ##RAYDIUM_ODE_AUTODETECT*2## are ok, meaning
"twice the detected size".
**/
__rayapi int raydium_ode_object_capsule_add(char *name, int group, dReal mass, dReal radius, dReal length, signed char type, int tag, char *mesh);
/**
This function will add an new "capsule" (capped cylinder) element to an
object (##group##).
You must provide:
- ##name##: **single** name for this new element.
- ##group##: owner object id.
- ##mass##: density of this new element. Mass will depend on radius and
length.
- ##radius##: radius of the internal cylinder and caps. Like boxes and
spheres, you can use ##RAYDIUM_ODE_AUTODETECT##, for radius.
Things like ##RAYDIUM_ODE_AUTODETECT*0.5## are valid, too.
- ##length##: full length from one end to the other (longest side).
- ##type##: ##RAYDIUM_ODE_STANDARD## or ##RAYDIUM_ODE_STATIC## (collide only,
no physics).
- ##tag##: use this integer value as you want. The important thing is that
this value is sent to network, and will be available on every connected peer.
This tag must be greater or equal to 0. Suitable functions are available
to read back this value later on an element.
- ##mesh##: 3D model used for rendering this element. Use an empty string to
disable rendering (and not ##NULL## !), and avoid ##RAYDIUM_ODE_AUTODETECT##
int this case.
##IMPORTANT##: The capsules are ALWAYS CREATED IN Z AXIS. Your meshes should
take this into account.So, the capsule meshes should have the length in Z
axis.
**/

//__rayapi int raydium_ode_object_mesh_add(char *name, int group, dReal mass, char *collimesh, dReal tx, dReal ty, dReal tz, signed char type, int tag, char *mesh);

__rayapi signed char raydium_ode_element_ray_attach(int element, dReal length, dReal dirx, dReal diry, dReal dirz);
/**
This function will attach a new ray to ##element##. This may be used as a
sensor, "hitscan" line, intersection test, ...
Then you can get from this ray things like distance between the start
of the ray (element's center) and the first "touched" element. You will also
find wich element was touched, and where. The same applies for the last touched
element.
Do not try to retrieve informations until next frame.

You must provide ray's length (the ray won't detect "things" over that point),
and direction vector (relative to element).

You can set up to ##RAYDIUM_ODE_MAX_RAYS## rays per element.

Warning, ray are linked to GLOBAL object, so they will detect EVERY element,
even if owned by the same object ! (only ##element## is never reported).

If you want to filter wich elements are used to generate rays'informations,
you can use ##raydium_ode_RayCallback##. This callback is following the
same prototype as ##raydium_ode_CollideCallback## (see at the top of
this chapter). In this callback, you can use 3 different return values:
- ##RAYDIUM_ODE_RAY_CONTACT_IGNORE## if you don't want this "contact" for
ray informations,
- ##RAYDIUM_ODE_RAY_CONTACT_REPORT## if you want to report the contact: it will
update informations for ##raydium_ode_element_ray_get()##.
- ##RAYDIUM_ODE_RAY_CONTACT_CREATE## will report the contact, and collide !

Return 0 if you don't want this "contact" for ray informations,
or 1 if you want normal behaviour. The first element in this callback is always
the one with the ray.

This functions returns the ray id for this element of -1 when it fails.
**/

__rayapi signed char raydium_ode_element_ray_attach_name(char *element, dReal length, dReal dirx, dReal diry, dReal dirz);
/**
Same as above, but using element's name.
**/

__rayapi signed char raydium_ode_element_ray_delete(int element, int ray_id);
/**
Delete ray ##ray_id## from ##element##. No more ray "reports" will be available
after this call.
**/

__rayapi signed char raydium_ode_element_ray_delete_name(char *element, int ray_id);
/**
Same as above, but using element's name.
**/

__rayapi signed char raydium_ode_element_ray_get(int element, int ray_id, raydium_ode_Ray *result);
/**
This function allows you to retrieve informations about ray.

Here you are a sample of ##raydium_ode_Ray## structure with
interesting fields:
%%(c)
typedef struct raydium_ode_Ray
{
    signed char state; // is this ray active ?
    dReal   rel_dir[3];
    dReal   rel_pos[3];
    // farest contact
    dReal   max_dist;
    int     max_elem;   // touched element, -1 if no element was touched
    dReal   max_pos[3];
    // nearest contact
    dReal   min_dist;
    int     min_elem;   // touched element, -1 if no element was touched
    dReal   min_pos[3];
} raydium_ode_Ray;
%%

Obviously, this function won't allocate any memory, you must provide a
valid pointer to a ##raydium_ode_Ray## struct.
**/

__rayapi signed char raydium_ode_element_ray_get_name(char *element, int ray_id, raydium_ode_Ray *result);
/**
Same as above, but using element's name.
**/

__rayapi signed char raydium_ode_element_ray_set_length(int element, int ray_id, dReal length);
/**
Change ray length to choose range detection length.
**/


__rayapi signed char raydium_ode_element_ray_set_length_name(char *element, int ray_id, dReal length);
/**
Same as above, but using element's name.
**/

__rayapi signed char raydium_ode_element_ray_pos(int element, int ray_id, dReal *pos);
/**
Moves ##ray_id## ray of ##element## to relative ##pos##.
**/

__rayapi signed char raydium_ode_element_ray_pos_3f(int element, int ray_id, dReal px, dReal py, dReal pz);
/**
Same as above, but using 3 float values.
**/

__rayapi signed char raydium_ode_element_ray_pos_name(char *element, int ray_id, dReal *pos);
/**
Same as above, but using element's name.
**/

__rayapi signed char raydium_ode_element_ray_pos_name_3f(char *element, int ray_id, dReal px, dReal py, dReal pz);
/**
Same as above, but using 3 float values.
**/

__rayapi int raydium_ode_element_fix (char *name, int *elem, int nelems, signed char keepgeoms);
/**
Experimental code.

The idea here is to create a bounding single element for a group of elements.
You must provide:
- ##name##: the new bounding element's name.
- ##elems##: an array of all elements to fix (id array).
- ##nelems##: the number of elements in ##elems## array.
- ##keepgeoms##: set to 0.

You can only fix standard elements (no statics) and all elements must be
owned by the same object.
**/

__rayapi void raydium_ode_element_unfix (int e);
/**
Experimental code. Unimplemented, yet.
Symmetric function, see ##raydium_ode_element_fix##.
**/

__rayapi void raydium_ode_element_mass(int elem, dReal mass);
/**
Change ##mass## the the ##elem## element. Obviously, you can't change the
mass of static element.
**/

__rayapi void raydium_ode_element_mass_name(char *elem, dReal mass);
/**
Same as above, but using element's name.
**/

__rayapi void raydium_ode_element_mass_set(int elem,dReal mass);
/**
Alias for ##raydium_ode_element_mass()##.
**/

__rayapi void raydium_ode_element_mass_set_name(char *elem, dReal mass);
/**
Alias for ##raydium_ode_element_mass_name()##.
**/

__rayapi dReal raydium_ode_element_mass_get(int elem);
/**
Return ##mass## of the ##elem## element.
Return only the mass of the element not the inertia matrix.
**/

__rayapi dReal raydium_ode_element_mass_get_name(char * elem);
/**
Same as above using element name.
**/

__rayapi void raydium_ode_element_move (int elem, dReal * pos);
/**
This function will move element ##elem## to ##pos##.
##pos## is a dReal array of 3 values (x,y,z).
Warning: arbitrary moves may lead to unwanted behaviours.
**/

__rayapi void raydium_ode_element_move_name (char *name, dReal * pos);
/**
Same as above, but using element's name.
**/

__rayapi void raydium_ode_element_move_3f(int elem, dReal x,dReal y, dReal z);
/**
Same as ##raydium_ode_element_move##, but using 3 dReal values.
**/


__rayapi void raydium_ode_element_move_name_3f (char *name, dReal x, dReal y, dReal z);
/**
Same as above, but using element's name.
**/

__rayapi void raydium_ode_element_rotate (int elem, dReal * rot);
/**
This function will rotate element ##elem## using ##rot##.
##rot## is a dReal array of 3 values (rx,ry,rz), in radians.
Warning: arbitrary rotations may lead to unwanted behaviours.
**/

__rayapi void raydium_ode_element_rotate_3f (int elem, dReal rx, dReal ry, dReal rz);
/**
Same as ##raydium_ode_element_rotate##, but using 3 dReal values.
**/

__rayapi void raydium_ode_element_rotate_name (char *name, dReal * rot);
/**
Same as ##raydium_ode_element_rotate##, but using element's name.
**/

__rayapi void raydium_ode_element_rotateq (int elem, dReal * rot);
/**
This function will rotate element ##elem## using ##rot## quaternion.
##rot## is a dReal array of 4 values.
Warning: arbitrary rotations may lead to unwanted behaviours.
**/

__rayapi void raydium_ode_element_rotateq_name (char *name, dReal * rot);
/**
Same as ##raydium_ode_element_rotateq##, but using element's name.
**/

__rayapi void raydium_ode_element_rotate_name_3f (char *name, dReal rx, dReal ry, dReal rz);
/**
Same as ##raydium_ode_element_rotate_name##, but using 3 dReal values.
**/

__rayapi void raydium_ode_object_rotate(int obj, dReal *rot);
/**
This function will try to rotate object ##obj##.
For now, rotation is done around the first element of the object.
##rot## is a dReal array of 3 values (rx,ry,rz), in radians.
Warning: arbitrary rotations may lead to unwanted behaviours.
**/

__rayapi void raydium_ode_object_rotate_name(char *obj, dReal *rot);
/**
Same as above, but using object's name.
**/

__rayapi void raydium_ode_object_rotate_name_3f(char *obj, dReal rx, dReal ry, dReal rz);
/**
Same as above, but using 3 dReal values.
**/

__rayapi void raydium_ode_object_rotq_set(int obj, dReal *qrot);
/**
Set absolute rotation Quaternion of object ##obj##.

**/

__rayapi void raydium_ode_object_eulerzyx_get_3f(int obj,dReal * rx, dReal *ry, dReal * rz);
/**
Return euler angle of first element (référence) of object ##obj##.
Euler angles are computed using Rotation matrix R=Rz.Ry.Rx.
**/

__rayapi void raydium_ode_object_eulerzyx_get(int obj,dReal * rotxyz);
/**
Same as above using ##rotxyz## vector of 3 elements.
**/

__rayapi void raydium_ode_object_eulerzyx_set_3f(int obj,dReal rx, dReal ry, dReal rz);
/**
Set euler angle of object ##obj##, center of rotation is the first element (référence) of the objet.
Euler angles are computed using Rotation matrix R=Rz.Ry.Rx.
**/

__rayapi void raydium_ode_object_rotate_rotq(int obj, dReal *rotq);
/**
Rotate object ##obj## according to ##rotq## quaternion rotation.
This is a relative rotation.
**/

__rayapi void raydium_ode_object_move (int obj, dReal * pos);
/**
This function will move object ##obj## to ##pos##.
Obviously, every element of object will be moved.
##pos## is a dReal array of 3 values (x,y,z).
Warning: arbitrary moves may lead to unwanted behaviours.
**/

__rayapi void raydium_ode_object_move_3f (int obj, dReal x, dReal y, dReal z);
/**
Same as above, but using 3 dReal values.
**/

__rayapi void raydium_ode_object_move_name (char *name, dReal * pos);
/**
Same as above, but using object's name.
**/

__rayapi void raydium_ode_object_move_name_3f (char *name, dReal x, dReal y, dReal z);
/**
Same as above, but using 3 dReal values.
**/

__rayapi void raydium_ode_object_pos_get(int obj,dReal *pos);
/**
Get object ##obj## position, retrun position of first element of the object (reference).
**/

__rayapi void raydium_ode_object_pos_set(int obj, dReal *new_pos);
/**
Set object ##obj## position. Move first element (reference) of object to the new position ##new_pos##.
And move all others elements keeping original relative position to reference element.
**/

__rayapi void raydium_ode_object_rotateq (int obj, dReal * rot);
/**
This function will try to rotate object ##obj## using ##rot## quaternion.
For now, rotation is done around the last element of the object.
##rot## is a dReal array of 4 values.
Warning: arbitrary rotations may lead to unwanted behaviours.
**/

__rayapi void raydium_ode_object_rotateq_name (char *obj, dReal * rot);
/**
Same as above, but using object's name.
**/

__rayapi void raydium_ode_element_rotate_direction (int elem, signed char Force0OrVel1);
/**
This function will rotate element ##elem## from its force or velocity.
Sets ##Force0OrVel1## to ##0## if you want to align element using its
force or ##1## using its linear velocity.
Warning: arbitrary rotations may lead to unwanted behaviours.
**/

__rayapi void raydium_ode_element_rotate_direction_name (char *e, signed char Force0OrVel1);
/**
Same as above, but using element's name.
**/


__rayapi void raydium_ode_element_auto_rotate_direction (int elem, signed char Force0OrVel1);
/**
Will flag ##elem## for automatic orientation according to it's force or velocity.
Sets ##Force0OrVel1## to ##0## if you want to align element using its
force or ##1## using its linear velocity.
Use ##-1## to disable automatic orientation.
**/

__rayapi void raydium_ode_element_auto_rotate_direction_name (char *e, signed char Force0OrVel1);
/**
Same as above, but using element's name.
**/

__rayapi void raydium_ode_element_rotate_rotq(int elem, dReal *rotq);
/**
Rotate element ##elem## relative to element rotation.
This is a relative rotation.
**/

__rayapi void raydium_ode_element_mesh_scale(int elem, float scale_factor);
/**
Allows to rescale the mesh of element ##elem##. This only applies to the mesh,
the physical properties are not modified (geometry size, mass, ...)
Shadows are rescaled, too.
**/

__rayapi void raydium_ode_element_mesh_scale_name(char *e, float scale_factor);
/**
Same as above, but using element's name.
**/

__rayapi void raydium_ode_element_data_set (int e, void *data);
/**
You may want to link some data to any element. If so, use this function
and provide a pointer to your ##data## for element ##e##.
**/

__rayapi void raydium_ode_element_data_set_name (char *e, void *data);
/**
Same as above, but using element's name.
**/

__rayapi void *raydium_ode_element_data_get (int e);
/**
This function will return a pointer to your linked data, if any (see above).
**/

__rayapi void *raydium_ode_element_data_get_name (char *e);
/**
Same as above, but using element's name.
**/

__rayapi int raydium_ode_element_tag_get (int e);
/**
When you create a new element, you must provide a "tag". This function
allows you to get back the tag's value, even on "distant" elements.
**/

__rayapi int raydium_ode_element_tag_get_name (char *e);
/**
Same as above, but using element's name.
**/

__rayapi void raydium_ode_object_data_set (int o, void *data);
/**
You may want to link some data to any object. If so, use this function
and provide a pointer to your ##data## for element ##e##.
**/

__rayapi void raydium_ode_object_data_set_name (char *o, void *data);
/**
Same as above, but using object's name.
**/

__rayapi void *raydium_ode_object_data_get (int e);
/**
This function will return a pointer to your linked data, if any (see above).
**/

__rayapi void *raydium_ode_object_data_get_name (char *e);
/**
Same as above, but using object's name.
**/

__rayapi void raydium_ode_joint_suspension (int j, dReal erp, dReal cfm);
/**
ERP means "Error Reduction Parameter", and its value is between 0 and 1 and
CFM means "Constraint Force Mixing".
Changing ERP and CFM values will change joint energy absorption and restitution.

For more informations, see ODE documentation, chapters 3.7 and 3.8.

Notice: Consider this function as advanced.

Warning: On a universal joint, this function will change ERP and CFM for
limits, and not for the joint itself ! (experimental feature)
**/

__rayapi void raydium_ode_joint_suspension_name (char *j, dReal erp, dReal cfm);
/**
Same as above, but using element's name.
**/

__rayapi int raydium_ode_joint_attach_hinge2 (char *name, int elem1, int elem2, dReal axe1x, dReal axe1y, dReal axe1z, dReal axe2x, dReal axe2y, dReal axe2z);
/**
Will create a new joint between two elements (##elem1## and ##elem2##).
Hinge2 is a very specialized joint, perfect for car wheel's for example.

[[http://ode.org/pix/hinge2.jpg hinge2]]

"Axis 1 is specified relative to body 1 (this would be the steering
axis if body 1 is the chassis). Axis 2 is specified relative to body 2
(this would be the wheel axis if body 2 is the wheel)."

You must also provide joint's ##name##.

Raydium provides ##RAYDIUM_ODE_JOINT_SUSP_DEFAULT_AXES## define, useful for
most chassis-wheel joints, and ##RAYDIUM_ODE_JOINT_AXE_X##, Y and Z for
other cases.

You cannot attach a static element.
**/

__rayapi int raydium_ode_joint_attach_hinge2_name (char *name, char *elem1, char *elem2, dReal axe1x, dReal axe1y, dReal axe1z, dReal axe2x, dReal axe2y, dReal axe2z);
/**
Same as above, but using elements's names.
**/

__rayapi int raydium_ode_joint_attach_universal (char *name, int elem1, int elem2, dReal posx, dReal posy, dReal posz, dReal axe1x, dReal axe1y, dReal axe1z, dReal axe2x, dReal axe2y, dReal axe2z);
/**
Will create a new joint between two elements (##elem1## and ##elem2##).

[[http://ode.org/pix/universal.jpg universal]]

"Given axis 1 on body 1, and axis 2 on body 2 that is perpendicular to
axis 1, it keeps them perpendicular. In other words, rotation of the two
bodies about the direction perpendicular to the two axes will be equal."

"Axis 1 and axis 2 should be perpendicular to each other."

You must also provide joint's ##name##, and joint position (##posx##, ##posy##,
##posz##) in world coordinates.

Raydium provides ##RAYDIUM_ODE_JOINT_AXE_X##, ##RAYDIUM_ODE_JOINT_AXE_Y##
and ##RAYDIUM_ODE_JOINT_AXE_Z## defines, that may help.

You cannot attach a static element.
**/

__rayapi int raydium_ode_joint_attach_universal_name (char *name, char *elem1, char *elem2, dReal posx, dReal posy, dReal posz, dReal axe1x, dReal axe1y, dReal axe1z, dReal axe2x, dReal axe2y, dReal axe2z);
/**
Same as above, but using elements's names.
**/

__rayapi int raydium_ode_joint_attach_hinge (char *name, int elem1, int elem2, dReal posx, dReal posy, dReal posz, dReal axe1x, dReal axe1y, dReal axe1z);
/**
Will create a new joint between two elements (##elem1## and ##elem2##).

[[http://ode.org/pix/hinge.jpg hinge]]

You must provide joint's ##name##, and joint position (##posx##, ##posy##,
##posz##) in world coordinates.

Raydium provides ##RAYDIUM_ODE_JOINT_AXE_X##, ##RAYDIUM_ODE_JOINT_AXE_Y##
and ##RAYDIUM_ODE_JOINT_AXE_Z## defines, that may help for setting axis.

You cannot attach a static element.
**/

__rayapi int raydium_ode_joint_attach_hinge_name (char *name, char *elem1, char *elem2, dReal posx, dReal posy, dReal posz, dReal axe1x, dReal axe1y, dReal axe1z);
/**
Same as above, but using elements's names.
**/

__rayapi int raydium_ode_joint_attach_fixed (char *name, int elem1, int elem2);
/**
Will create a new joint between two elements (##elem1## and ##elem2##).

Fixed joint is more a hack than a real joint. Use only when it's absolutely
necessary, and have a look to ##raydium_ode_element_fix##.

You must provide joint's ##name##
You cannot attach a static element.
**/

__rayapi int raydium_ode_joint_attach_fixed_name (char *name, char *elem1, char *elem2);
/**
Same as above, but using elements's names.
**/

__rayapi void raydium_ode_joint_hinge_limits (int j, dReal lo, dReal hi);
/**
Sets low (##lo##) and high (##hi##) limits for hinge joint ##j##.
**/

__rayapi void raydium_ode_joint_hinge_limits_name (char *j, dReal lo, dReal hi);
/**
Same as above, but using joint's name.
**/

__rayapi void raydium_ode_joint_universal_limits (int j, dReal lo1, dReal hi1, dReal lo2, dReal hi2);
/**
Sets low and hight limits for axe 1 (##lo1##, ##hi1##) and axe 2 (##lo2##,
##hi2##) for universal joint ##j##. See ##raydium_ode_joint_attach_universal##
for more informations about universal joint axes.
**/

__rayapi void raydium_ode_joint_universal_limits_name (char *j, dReal lo1, dReal hi1, dReal lo2, dReal hi2);
/**
Same as above, but using joint's name.
**/

__rayapi void raydium_ode_joint_hinge2_block (int j, signed char block);
/**
Sometime, you may need to block rotation for first axe of hinge2 joints, for
example with rear wheels of a car. If so, set ##block## to 1 (true).
Setting ##block## back to 0 (false) will restore standard rotation behaviour.
**/

__rayapi void raydium_ode_joint_hinge2_block_name (char *name, signed char block);
/**
Same as above, but using joint's name.
**/

__rayapi void raydium_ode_joint_hinge2_limits (int j, dReal lo, dReal hi);
/**
Sets low (##lo##) and high (##hi##) limits for hinge2 (aka suspet) joint ##j##.
**/

__rayapi void raydium_ode_joint_hinge2_limits_name (char *j, dReal lo, dReal hi);
/**
Same as above, but using joint's name.
**/

__rayapi void raydium_ode_joint_delete_callback (int j, void (*f) (int));
/**
Since joints may break (see ##raydium_ode_joint_break_force##), it may be
useful to get a callback on joint deletion.
This callback must this prototype:
##void joint_delete(int jid)##
##jid## is the deleted joint id. You can't cancel joint deletion (yet).
**/

__rayapi void raydium_ode_joint_delete_callback_name (char *name, void (*f) (int));
/**
Same as above, but using joint's name.
**/

__rayapi void raydium_ode_joint_break_force (int j, dReal maxforce);
/**
Setting a non-zero ##maxforce## on a joint will transform this joint into
a "breakable joint". There's no unit for ##maxforce##, you'll probably have
to find the suitable value empirically.
**/

__rayapi void raydium_ode_joint_break_force_name (char *name, dReal maxforce);
/**
Same as above, but using joint's name.
**/

__rayapi void raydium_ode_joint_elements_get (int j, int *e1, int *e2);
/**
Will return elements (##e1## and ##e2##) linked to joint ##j##.
**/

__rayapi void raydium_ode_joint_elements_get_name (char *j, int *e1, int *e2);
/**
Same as above, but using joint's name.
**/

__rayapi void raydium_ode_motor_update_joints_data_internal (int j);
/**
Internal function.
**/

__rayapi void raydium_ode_motor_speed (int j, dReal force);
/**
Sets motor ##j## speed parameter. This is only suitable for "engine"
and "rocket" type motors. There's no special unit for ##force##.
**/

__rayapi void raydium_ode_motor_speed_name (char *name, dReal force);
/**
Same as above, but using motor's name.
**/

__rayapi void raydium_ode_motor_power_max (int j, dReal power);
/**
Sets motor ##j## max power parameter. This is only suitable for "engine"
and "angular" motors. There's no special unit for ##power##.
**/

__rayapi void raydium_ode_motor_power_max_name (char *name, dReal power);
/**
Same as above, but using motor's name.
**/

__rayapi void raydium_ode_motor_angle (int j, dReal angle);
/**
Sets motor ##j## angle parameter. This is only suitable for "angular" motors.
##angle## has the units of radians.
**/

__rayapi void raydium_ode_motor_angle_name (char *motor, dReal angle);
/**
Same as above, but using motor's name.
**/

__rayapi void raydium_ode_motor_gears_set (int m, dReal * gears, int n_gears);
/**
Sets a gearbox for motor ##m## (only suitable for "engine" motors).
##gears## is an array of dReal values, with all gears factors).
##n_gears## is the array length (total number of gears for this gearbox).
example:
%%(c)
// rear,1,2,3,4,5
dReal gears[]={-0.4,0.4,0.6,0.8,0.9,1.0};
...
raydium_ode_motor_gears_set(main_engine,gears,6);
%%
If you want to cancel a gearbox, set a gearbox with only one gear with 1.0
factor value.

Raydium gearboxes implementation is very naive, with 100% output.
For example, a 0.5 gear factor will divide maximum speed by two, but will
provide twice the normal torque.
**/

__rayapi void raydium_ode_motor_gears_set_name (char *m, dReal * gears, int n_gears);
/**
Same as above, but using motor's name.
**/

__rayapi void raydium_ode_motor_gear_change (int m, int gear);
/**
Switch motor ##m## to ##gear##.
**/

__rayapi void raydium_ode_motor_gear_change_name (char *m, int gear);
/**
Same as above, but using motor's name.
**/

__rayapi dReal raydium_ode_motor_gear_ratio(int m);
/**
Returns the current gear ratio for motor ##m##.
**/

__rayapi dReal raydium_ode_motor_gear_ratio_name(char *m);
/**
Same as above, but using motor's name.
**/

__rayapi dReal *raydium_ode_element_pos_get (int j);
/**
This function will return element ##j##'s current position, as an array of
3 dReal values.
example:
%%(c)
dReal *pos;
dReal pos_copy;
...
pos=raydium_ode_element_pos_get(my_element);
raydium_log("%f %f %f",pos[0],pos[1],pos[2]);
memcpy(pos_copy,pos,sizeof(dReal)*3);
...
%%
Returned data is available only for the current frame.
**/

__rayapi dReal *raydium_ode_element_pos_get_name (char *name);
/**
Same as above, but using element's name.
**/

__rayapi signed char raydium_ode_element_rotq_get (int j, dReal * quat);
/**
This function will return element ##j##'s current rotation, as an array of
4 dReal values (quaternion), thru ##quat##.
No memory allocation will be done.
**/

__rayapi signed char raydium_ode_element_rotq_get_name (char *name, dReal * quat);
/**
Same as above, but using element's name.
**/

__rayapi signed char raydium_ode_element_rotq_set (int j, dReal * quat);
/**
This function will set element ##j##'s current rotation, as an array of
4 dReal values (quaternion), thru ##quat##.
**/

__rayapi signed char raydium_ode_element_rotq_set_name (char *name, dReal * quat);
/**
Same as above, but using element's name.
**/

__rayapi signed char raydium_ode_element_rot_get (int e, dReal * phi, dReal * theta, dReal * psi);
/**
This code is experimental. It should returns element ##e##'s current rotation
using 3 dReal angles, in radians.
Compute euler angles of element ##e## using R = Transpose (Rz.Ry.Rx) Rotation matrix order.
This Function is ode compliant with internal ode Euler to Rotation Matrix compute.
Y axis is associated with theta

**/

__rayapi signed char raydium_ode_element_rot_get_name (char * name, dReal * phi, dReal * theta, dReal * psi);
/**
Same as above, but using element's name.
**/

__rayapi signed char raydium_ode_element_rot_set (int e, dReal phi, dReal theta, dReal psi);
/**
This code returns element ##e##'s current rotation using 3 dReal angles, in radians.
Compute euler angles of element ##e## using R = Transpose (Rz.Ry.Rx) Rotation matrix order.
This Function is ode compliant with internal ode Euler to Rotation Matrix compute.
**/

__rayapi signed char raydium_ode_element_rot_set_name (char * name, dReal phi, dReal theta, dReal psi);
/**
Same as above, but using element's name.
**/

__rayapi signed char raydium_ode_element_eulerxyz_get_name(char * name, dReal *rx, dReal *ry, dReal *rz);
/** Compute euler angles of element ##name## using R = Rx.Ry.Rz Rotation matrix order.
**/

__rayapi signed char raydium_ode_element_eulerxyz_get(int e, dReal *rx, dReal *ry, dReal *rz);
/**
Same as above but using element number.
**/

__rayapi signed char raydium_ode_element_eulerzyx_get_name(char * name, dReal *rx, dReal *ry, dReal *rz);
/** Compute euler angles of element ##name## using R = Rz.Ry.Rz Rotation matrix order.
This function is usefull to find object site (local orientation), as Z rotation is applied first
X and Y rotation does not depend on word element orientation (plane, ...)
**/

__rayapi signed char raydium_ode_element_eulerzyx_get(int e, dReal *rx, dReal *ry, dReal *rz);
/**
Same as above but using element number.
**/

__rayapi signed char raydium_ode_element_euler_set_name(char * name, dReal yaw, dReal pitch, dReal roll);
/** Set rotation matrix of element ##name## using euler angles and matrix R = Rx.Ry.Rz Rotation matrix order.
Yaw is X axis rotation, Pitch Y axis, and Roll Z axis.
**/

__rayapi signed char raydium_ode_element_euler_set(int e, dReal yaw, dReal pitch, dReal roll);
/**
Same as above but using element number.
**/

__rayapi void raydium_ode_element_sound_update (int e, int source);
/**
This function is a small bridge between RayODE and sound API, updating sound
##source## using element ##e##'s position.
**/

__rayapi void raydium_ode_element_sound_update_name (char *e, int source);
/**
Same as above, but using element's name.
**/

__rayapi void raydium_ode_element_RelPointPos (int e, dReal px, dReal py, dReal pz, dReal * res);
/**
Give a point (##px##, ##py## and ##pz##) on element ##e## to this function,
and il will return this point in global coordinates (##res##).
Returned data is available only for the current frame.
**/

__rayapi void raydium_ode_element_RelPointPos_name (char *e, dReal px, dReal py, dReal pz, dReal * res);
/**
Same as above, but using element's name.
**/

__rayapi int raydium_ode_motor_create (char *name, int obj, signed char type);
/**
This function will create a new motor, using ##name## (single), for
object ##obj##, with ##type##. As said before, available types are
##RAYDIUM_ODE_MOTOR_ENGINE##, ##RAYDIUM_ODE_MOTOR_ANGULAR## and
##RAYDIUM_ODE_MOTOR_ROCKET##. See the first part of this chapter for more
informations about motor types.
**/

__rayapi void raydium_ode_motor_attach (int motor, int joint, int joint_axe);
/**
This function will link ##motor## to ##joint##, on axe ##joint_axe## (first axe
is axe ##0## and so on ...). This is only suitable for engine and angular motors.
**/

__rayapi void raydium_ode_motor_attach_name (char *motor, char *joint, int joint_axe);
/**
Same as above, but using motor's name and joint's name.
**/

__rayapi dReal raydium_ode_motor_speed_get (int m, int gears);
/**
Will return current motor speed.
For engine style motors, if ##gears## is sets to 1 (true), returned speed
will be relative to current motor's gear. Useless for other types.
**/

__rayapi dReal raydium_ode_motor_speed_get_name (char *name, int gears);
/**
same as above, but using motor's name.
**/

__rayapi dReal raydium_ode_motor_angle_get(int m, int axe);
/**
Will return current motor angle on ##axe## axe. Avaible only for angular motor.
**/

__rayapi dReal raydium_ode_motor_angle_get_name(char *name, int axe);
/**
same as above, but using motor's name.
**/

__rayapi void raydium_ode_motor_rocket_set (int m, int element, dReal x, dReal y, dReal z);
/**
This function will configure rocket motor ##m## on ##element## at position
(##x##,##y##,##z##). Rocket motors are unusable until this function is called.
**/

__rayapi void raydium_ode_motor_rocket_set_name (char *motor, char *element, dReal x, dReal y, dReal z);
/**
same as above, but using motor's name.
**/

__rayapi void raydium_ode_motor_rocket_orientation (int m, dReal rx, dReal ry, dReal rz);
/**
This function will rotate rocket ##m## using ##rx##,##ry## and ##rz## angles
in degrees. Base orientation is z up.
**/

__rayapi void raydium_ode_motor_rocket_orientation_name (char *name, dReal rx, dReal ry, dReal rz);
/**
same as above, but using motor's name.
**/

__rayapi void raydium_ode_motor_rocket_playermovement (int m, signed char isplayermovement);
/**
Will configure rocket ##m## for player movements. This type of rocket will be
automatically disabled when linked element is not touched by
anything (ground in most cases).
**/

__rayapi void raydium_ode_motor_rocket_playermovement_name (char *m, signed char isplayermovement);
/**
same as above, but using motor's name.
**/

__rayapi signed char raydium_ode_motor_delete (int e);
/**
Will obviously delete motor ##e##.
**/

__rayapi signed char raydium_ode_motor_delete_name (char *name);
/**
same as above, but using motor's name.
**/

__rayapi signed char raydium_ode_joint_delete (int joint);
/**
Will obviously delete ##joint##.
**/

__rayapi signed char raydium_ode_joint_delete_name (char *name);
/**
same as above, but using joint's name.
**/

__rayapi signed char raydium_ode_element_delete (int e, signed char deletejoints);
/**
Will obviously delete element ##e##. Deletion may me queued for some reason,
for a very short time (current collide loop). For now, you **must** set
##deletejoints## to 1 (true), since joints without 2 linked elements
are invalid.
Linked rocket engines will be deleted, too.
**/

__rayapi signed char raydium_ode_element_delete_name (char *name, signed char deletejoints);
/**
Same as above, but using element's name.
**/

__rayapi signed char raydium_ode_object_delete (int obj);
/**
Will obviously delete object ##obj##. All elements, joints and motors will
be deleted with object.
**/

__rayapi signed char raydium_ode_object_delete_name (char *name);
/**
Same as above, but using object's name.
**/

__rayapi signed char raydium_ode_explosion_delete (int e);
/**
Will delete ##RAYDIUM_ODE_NETWORK_EXPLOSION_EXPL## type explosion ##e##.
**/

__rayapi signed char raydium_ode_element_moveto (int element, int object, signed char deletejoints);
/**
This function will move ##element## from his owner object to another ##object##.
This "migration" will not be completed until ##element## is not touching
anymore his previous owner.
For now, you **must** set ##deletejoints## to 1 (true), deleting linked joints.
**/

__rayapi signed char raydium_ode_element_moveto_name (char *element, char *object, signed char deletejoints);
/**
Same as above, but using element's name and object's name.
**/

__rayapi void raydium_ode_joint_break (int j);
/**
Internal joint testing function.
**/

__rayapi signed char raydium_ode_launcher (int element, int from_element, dReal * rot, dReal force);
/**
This function will launch an ##element## from ##from_element##.
You must provide ##rot##, an array of 3 dReal angles in degreees, relative
to ##from_element## current orientation.
You must also provide a ##force##, with no particular unit.
**/

__rayapi signed char raydium_ode_launcher_name (char *element, char *from_element, dReal * rot, dReal force);
/**
Same as above, using ##element## and ##from_element## names.
**/

__rayapi signed char raydium_ode_launcher_name_3f (char *element, char *from_element, dReal rx, dReal ry, dReal rz, dReal force);
/**
Same as above, but using 3 dReal values for rotation.
**/

__rayapi signed char raydium_ode_launcher_simple (int element, int from_element, dReal * lrot, dReal force);
/**
This function will act the same as previous ones, adding a few things:
- ##element## will be aligned with ##from_element## (position and rotation).
- ##element## will be "migrated" to GLOBAL object during launch.
**/

__rayapi signed char raydium_ode_launcher_simple_name (char *element, char *from_element, dReal * rot, dReal force);
/**
Same as above, using ##element## and ##from_element## names.
**/

__rayapi signed char raydium_ode_launcher_simple_name_3f (char *element, char *from_element, dReal rx, dReal ry, dReal rz, dReal force);
/**
Same as above, but using 3 dReal values for rotation.
**/

__rayapi void raydium_ode_explosion_blow (dReal radius, dReal max_force, dReal * pos);
/**
This function will create an instantaneous explosion, generating a degressive
blowing effect.
You must provide a ##radius## (normal world units), a maximum force
(##max_force##), and a position (##pos##, 3 x dReal array).
**/

__rayapi void raydium_ode_explosion_blow_3f (dReal radius, dReal max_force, dReal px, dReal py, dReal pz);
/**
Same as above, but using 3 dReal values for position.
**/

__rayapi void raydium_ode_explosion_blow_rand(dReal radius, dReal max_force, dReal rand_factor, dReal *pos);
/**
Same as ##raydium_ode_explosion_blow_rand()##, but introducing a random factor
on resulting torque for blowed objects, for a more realistic effect.
**/

__rayapi void raydium_ode_explosion_blow_rand_3f(dReal radius, dReal max_force, dReal rand_factor, dReal px, dReal py, dReal pz);
/**
Same as above, but using 3 dReal values for position.
**/

__rayapi int raydium_ode_explosion_create (char *name, dReal final_radius, dReal propag, dReal * pos);
/**
This function will create an spherical growing explosion. Any element in the
explosion will be ejected.
As said before: "Use this for very consequent explosions only !".
You must provide ##final_radius##, ##propag## (growing size) and a
position (##pos##, 3 x dReal array).
When an explosion reach its final radius, it will be deleted.
**/

__rayapi void raydium_ode_element_camera_inboard (int e, dReal px, dReal py, dReal pz, dReal lookx, dReal looky, dReal lookz);
/**
RayODE to camera API bridge.
Sets the camera on element ##e## at relative position (##px##,##py##,##pz##),
and looking at (##lookx##,##looky##,##lookz##) relative point.
Works with normal and static elements.
**/

__rayapi void raydium_ode_element_camera_inboard_name (char *name, dReal px, dReal py, dReal pz, dReal lookx, dReal looky, dReal lookz);
/**
Same as above, but using element's name.
**/

__rayapi void raydium_ode_draw_all_post(void);
/**
This function is called automatically when you render the frame with RayODE
using ##raydium_ode_draw_all(RAYDIUM_ODE_DRAW_NORMAL)##.
Currently, it will generate and apply HDR map, render particles and lensflares.
**/

__rayapi void raydium_ode_draw_all(signed char names);
/**
This function will draw all RayODE scene. You must call this function
by yourself.
Set ##names## to ##RAYDIUM_ODE_DRAW_NORMAL## for normal rendering.
Other ##names## values will:
- draw only elements, joints and motors names and elements bounding boxes
with ##RAYDIUM_ODE_DRAW_DEBUG##
- draw only objets AABB (Axis-Aligned Bounding Box) with ##RAYDIUM_ODE_DRAW_AABB##
- draw only element rays (if any) with ##RAYDIUM_ODE_DRAW_RAY##
- do the same as regular rendering, but will not apply post-rendering to the
frame with the value ##RAYDIUM_ODE_DRAW_NORMAL_NO_POST## (see previous function)
... so you may need multiple call to this function each frame.
**/

__rayapi void raydium_ode_near_callback (void *data, dGeomID o1, dGeomID o2);
/**
Internal callback.
**/

__rayapi void raydium_ode_callback (void);
/**
Internal frame callback.
**/

__rayapi void raydium_ode_time_change (GLfloat perc);
/**
This function will change RayODE timecall frequency, allowing slow motion
effects, for example, where ##perc## is the percentage of the normal time base.
Since this function obviously do not change physics accuracy, be careful
with ##perc## > 100, wich will generate a big load for the CPU.
This function also change particles and mesh animations time.
**/

__rayapi void raydium_ode_element_particle (int elem, char *filename);
/**
This function will "fix" a particle generator on element ##elem##. You must
provide particle generator's ##filename##.
**/

__rayapi void raydium_ode_element_particle_name (char *elem, char *filename);
/**
Same as above, using element's name.
**/

__rayapi void raydium_ode_element_particle_offset (int elem, char *filename, dReal * offset);
/**
Same as ##raydium_ode_element_particle##, but with an ##offset##, relative
to element. ##offset## is an array of 3 dReal values.
**/

__rayapi void raydium_ode_element_particle_offset_3f(int elem,char *filename,dReal ox,dReal oy, dReal oz);
/**
Same as above, but using 3 dReal values for offset.
**/

__rayapi void raydium_ode_element_particle_offset_name (char *elem, char *filename, dReal * offset);
/**
Same as above, using element's name.
**/

__rayapi void raydium_ode_element_particle_offset_name_3f (char *elem, char *filename, dReal ox, dReal oy, dReal oz);
/**
Same as above, but using 3 dReal values for offset.
**/

__rayapi void raydium_ode_element_particle_point (int elem, char *filename);
/**
Same as ##raydium_ode_element_particle##, but generator will not be linked
with element, only positioned at current element's position.
**/

__rayapi void raydium_ode_element_particle_point_name (char *elem, char *filename);
/**
Same as above, using element's name.
**/

void raydium_ode_internal_particle_genetator_deleted_callback(int gen);
/**
Internal callback.
**/

__rayapi int raydium_ode_element_lensflare_offset(int elem, char *flare_name, char *filename, dReal *offset);
/**
This function will attach a lensflare to element ##elem## at desired ##offset##.
Even if this function will load the lensflare by itslef, you should have a look
to ##raydium_lensflare_create()## for more informations about lensflares.
**/

__rayapi int raydium_ode_element_lensflare_offset_name(char *elem, char *flare_name, char *filename, dReal *offset);
/**
Same as above, using element's name.
**/

__rayapi int raydium_ode_element_lensflare_offset_name_3f(char *elem, char *flare_name, char *filename, dReal offx, dReal offy, dReal offz);
/**
Same as above, but using 3 dReal values for offset.
**/


__rayapi void raydium_camera_smooth_path_to_element (char *path, int element, GLfloat path_step, GLfloat smooth_step);
/**
This function is a clone of ##raydium_camera_smooth_path_to_pos## dedicated to
RayODE, looking at ##element## from path.
You may look at suitable chapter for more informations about ##path##,
##path_step## and ##smooth_step##.
**/

__rayapi void raydium_camera_smooth_path_to_element_name (char *path, char *element, GLfloat path_step, GLfloat smooth_step);
/**
Same as above, using element's name.
**/

__rayapi void raydium_camera_smooth_element_to_path_name (char *element, char *path, GLfloat path_step, GLfloat smooth_step);
/**
This function is a clone of ##raydium_camera_smooth_pos_to_path## dedicated to
RayODE, looking at path, from ##element##.
Here, you must provide element's name.
You may look at suitable chapter for more informations about ##path##,
##path_step## and ##smooth_step##.
**/

__rayapi void raydium_camera_smooth_element_to_path_offset (int element, GLfloat offset_x, GLfloat offset_y, GLfloat offset_z, char *path, GLfloat path_step, GLfloat smooth_step);
/**
This function is a clone of ##raydium_camera_smooth_pos_to_path## dedicated to
RayODE and providing an offset (for ##element##), looking at path, from
##element##.
You may look at suitable chapter for more informations about ##path##,
##path_step## and ##smooth_step##.
**/

__rayapi void raydium_camera_smooth_element_to_path_offset_name (char *element, GLfloat offset_x, GLfloat offset_y, GLfloat offset_z, char *path, GLfloat path_step, GLfloat smooth_step);
/**
Same as above, using element's name.
**/


__rayapi int raydium_ode_capture_3d(char *filename);
/**
This function is provided "for fun" only. The main idea is to dump all scene
to a .tri file (##filename##). A .sprt file will also be created, wich is a
special file format with all particles found during the dump. You can reload
.sprt files with ##raydium_particle_state_restore##.
Note from source code:
%%(c)
// This function is provided "for fun" only. Not all effects are dumped:
// Missing : shadows, forced colors, before/after callbacks,
// fixed elements, ...
// Some code is pasted from file.c (and this is BAD ! :)
%%
**/

__rayapi int raydium_ode_orphans_check(void);
/**
Search orphans in all objects. An orphan is a geometry that exists into ODE
but is not managed by RayODE.
This function will print object with orphans and return total orphan count.
**/

__rayapi int raydium_ode_ray_launch(dReal *from, dReal *to, dReal max_dist, dReal *touched_dist, dReal *touched_pos, signed char (*filter)(int,int, dContact *));
/**
This function will launch a ray from any position to any other on, and will
report touched element (-1 if none) and the touched position in world coords,
found in ##touched_pos## (3*dReal array, or NULL).

You must give a maximum distance for the ray, and the function will return
the distance to the touched element. (##touched_dist##, pointer to dReal)

Parameters ##from## and ##to## are 3*dReal arrays.

You can set a ##filter##, if needed. (or use NULL)

Warning, this function could be a bit costly, since it used a full ODE scene
collision detection.
**/

__rayapi int raydium_ode_mouse_pick(dReal dist,dReal pos[3],dReal *depth);
/**
Mouse picking function. Return raydium_element pointed by mouse on the screen.
##dist## is maximal detection distance (range).
##pos## Global 3D position of pointed point. ##depth## distance from point of view.
Use ##raydium_ode_PickCallback## (as signed char f(int,int,dContact*)) to filter
contacts points. Uses as other collide callback but is specific to Pick function.
**/

__rayapi void raydium_ode_set_physics_freq (GLfloat freq);
/**
You can use ##raydium_ode_set_physics_freq## to change the frequence of physical engine call. 400Hz is default.
This will change physical behaviour.
**/

__rayapi GLfloat raydium_ode_get_physics_freq(void);
/**
Return the actual value of physical engine calling frequency.
**/

__rayapi void raydium_ode_set_timestep(GLfloat tstep);
/**
Set physical engine time step (which is 0.006f by default).
This value is related to ##raydium_ode_physics_freq##.
**/

__rayapi GLfloat raydium_ode_get_timestep(void);
/**
Return physical engine iteration timestep.
**/


__rayapi void raydium_ode_capture_internal_create(int type, int id, dReal *sizes, char *mesh);
/**
Internal. Add entity creation to the replay.
**/

__rayapi void raydium_ode_capture_internal_create_all(void);
/**
Internal. Add all entities to the replay.
**/

__rayapi void raydium_ode_capture_internal_delete(int id);
/**
Internal. Add entity deletion to the replay.
**/

__rayapi void raydium_ode_capture_record(char *rrp_filename);
/**
Records to a RRP file all ODE events to create a replay.
Recording is done at ##RAYDIUM_ODE_RECORD_RATE_DEFAULT## rate.
You can play another RRP file while recording.

WARNING: experimental feature ! API may change.
**/

__rayapi void raydium_ode_capture_record_rate(char *rrp_filename, int rate_hz);
/**
Same as above, but allowing you to select the recording rate.
Valid range is [1,400] Hz.
**/

__rayapi void raydium_ode_capture_record_stop(void);
/**
Stops the recording. Not needed to get a valid record.
**/

__rayapi void raydium_ode_capture_play_internal_index_build(void);
/**
Internal. Builds index, needed for RRP playing.
**/

__rayapi void raydium_ode_record_play_ghost(signed char ghost);
/**
Set ##ghost## to true (1) if you want to avoid all collisions between "your"
elements and replay ones. Default is false (0).
**/

__rayapi void raydium_ode_capture_play(char *rrp_filename, signed char change_ground);
/**
This function will play ##rrp_filename## file, and default speed.
If ##change_ground## is true (1), this function will set/change the
current RayODE ground (like raydium_ode_ground_set_name()) with the one
used in the replay file.
You can start recording during a replay, if you want.
**/

__rayapi void raydium_ode_capture_stop(void);
/**
Stops the replay.
**/

__rayapi signed char raydium_ode_capture_seek(double time);
/**
This function will seek into the replay to jump at ##time## seconds.
The CPU cost of this function is proportional to the size of the jump
from the current position to ##time##.
Returns false (0) on failure (ex:##time## out of range)
**/

__rayapi signed char raydium_ode_capture_seek_rel(double time);
/**
Same as above, but using a relative ##time##.
Example: ##raydium_ode_capture_seek_rel(-2.5)## will rewind the replay of
two and half a second from the current time.
**/

__rayapi void raydium_ode_capture_speed(GLfloat factor);
/**
Change the speed of the capture replay. The default factor is ##1##, and a value
of ##2## will play the record twice the normal speed, for example.
**You can use negative values** to play the record backwards.
The value ##0## will pause the replay.
**/

__rayapi void raydium_ode_capture_internal_read_event(signed char sense);
/**
Internal. Reads "special" events at the current file position (sense=1 means
forward and sens=-1 means backward)
**/

__rayapi void raydium_ode_capture_internal_read_move(signed char pass);
/**
Internal. Reads a "move" event at the current file position.
**/

__rayapi void raydium_ode_capture_play_callback(void);
/**
Internal frame callback for capture playing.
**/

__rayapi void raydium_ode_autodisable_set(signed char autod);
/**
RayODE can automatically disable resting elements.
This is useful for physical solver. It don't have to reserve space in
the solver matrix and compute solution for this element.

This can greatly speed up the program, but may cause some strange
behaviors, like elements resting in mid-air.

This feature is disabled by default.
**/

__rayapi signed char raydium_ode_autodisable_get(void);
/**
Is RayODE autodisable feature is enabled ? This function will
return ##1## (true) if yes, ##0## (false) otherwise.
**/

__rayapi signed char raydium_ode_element_disable_get(int elem);
/**
Return whatever or not an element is disabled.
In this case it is not treated by the solver.
Will return ##1## if element is disabled, ##0## if not.
**/

__rayapi signed char raydium_ode_element_disable_get_name (char *e);
/**
Same as above using element name.
**/

__rayapi void raydium_ode_element_disable_set(int elem, signed char disable_state);
/**
Allow user to disable or reactivate an element manually.
if ##disable_state## is ##1## element will be desactived,
##0## will re-activate the element.
**/

__rayapi void raydium_ode_element_disable_set_name (char *e, signed char disable_state);
/**
Same as above with element name.
**/

__rayapi void raydium_ode_contact_feedback_save(int custom_id);
/**
During your ##raydium_ode_CollideCallback##, you may want to save forces
and torques that **will** be generated by the contact you're creating.

This function allows you to do this, simply by providing a ##custom_id##, a
unique identifier, in the range [0,##RAYDIUM_ODE_CONTACTS_FEEDBACK_MAX##].
Most of the time, this value is the "current" element id, like the wheel id
during a road/wheel contact, for instance. It's then very easy to get the saved
forces and torques during the next physics iteration using the same id (the
wheel id is the same from one iteration to another).
**/

__rayapi dJointFeedback *raydium_ode_contact_feedback_get(int custom_id);
/**
This function will return a pointer to a ##dJointFeedback## structure, where
you'll find the forces and torques saved with
##raydium_ode_contact_feedback_save()## during **the previous physics iteration**.
See above for more informations about ##custom_id##.
%%(c)
typedef struct dJointFeedback {
    dVector3 f1; // force that joint applies to body 1
    dVector3 t1; // torque that joint applies to body 1
    dVector3 f2; // force that joint applies to body 2
    dVector3 t2; // torque that joint applies to body 2
} dJointFeedback;
%%
**/

#include "ode_net.h"
#endif
