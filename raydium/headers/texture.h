#ifndef _TEXTURE_H
#define _TEXTURE_H
/*=
Textures
1200
**/

// Introduction
/**
For now, Raydium only handles TGA textures.
As explainded in the first part of this guide, Raydium provides four
texture filters (none, bilinear, trilinear using MipMaps, and anisotropic ).

Texture must be 8 (alpha mask), 24 (RGB) or 32 (RGBA) bits, and sizes should
be a power of two (so all hardware can handle it). Compression is available.

Raydium supports simple color materials,  using a "rgb(r,g,b)" string
as texture name, where r, g and b are 0 <= x <= 1 (floats).
With 3 negative values, you will generate a "phantom texture". Phantom textures
are only drawn into the z-buffer (and not color buffer).
Texture clamping and advanced multitexturing effects are supported by Raydium,
but not fully documented here for now.
A few quick tips:
 - "BOX" filename prefix (ex: BOX_foo.tga) is used as a clamp-to-edge attribute.
 - "HDR" prefix is used to set a texture as a "light emitter" (sky, lamp, ...)
 - "ENV" prefix is used for environment (sphere) mapping.
 - "ATM" prefix is used for "Alpha-Tested Magnification" vector textures.
 - ";" operator is used for basic multitexturing in TRI files.
 - "|" operator for UV coords with multitextured lines in TRI files.
 - "#" operator is used for environnement mapped multitexturing in TRI files.

A few things about the last operator, "#":
Effective environment mapping (one pass, two texture units) is available using
a special filename separator for texture field in TRI files : #
See this example:
##0.232258 0.225387 -0.149804 0.012198 -0.274925 0.961388 0.731411 0.980236 fiesta_diffuse.tga#ENV_map.tga##
Environment texture name must start with "ENV" to allow spherical mapping, wich
is needed for such effect. See also ##RAYDIUM_RENDER_REFLECTION_FACT## in
file ##common.h## if you want reflection to be more or less visible.

This separator can also be used for shaders, to load multiple textures in the
hardware (up to RAYDIUM_RENDER_MAX_TEXUNITS on the same line of the tri file).

For more informations about other operators and prefixes, you may have a look
at the Wiki or at engine's source code.

Some informations about textures with alpha channel, since it can be to root of
a few rendering issues. Translucent textures are not Z-buffer compliant, so
Raydium will always render it last. But it's a "per-object" behavior, so it
can still "fight" with other objects, or even with other translucent textures
from the same object. Think about rendering order (textures and objects) when
possible, it will help avoiding such troubles.

Sometime, alpha channel is only needed to create "holes" in textures. If you
can't handle this directly in the geometry of the object (slower but better
quality), the easy option is ##RAYDIUM_TEXTURE_BLEND_CUTOUT## textures. Raydium
will detect such textures when alpha channel shows only perfectly opaque pixels
and prefectly transparent ones. Any intermediate value will make the engine
falling back to the regular ##RAYDIUM_TEXTURE_BLEND_BLENDED## mode.

Keep this in mind when you create textures like trees, for instance, where you
should have only 0 or 100% alpha values. You can do this with software
like The GIMP, using tools like "Threshold Alpha". Note that the engine will
not apply advanced filters (trilinear, aniso, ...) on such textures to avoid
rendering artifacts.
**/

__rayapi signed char raydium_texture_size_is_correct (GLuint size);
/**
Returns true if ##size## is a correct texture size, depending of
hardware capacities and "power of 2" constraint.
**/

__rayapi GLuint raydium_texture_load_internal(char *filename, char *as, signed char faked, int faked_tx, int faked_ty, int faked_bpp, int or_live_id_fake);
/**
Internal use. (and there's also some other internal wrappers)
**/

__rayapi GLuint raydium_texture_load (char *filename);
/**
Loads "filename" texture into hardware memory. Function results
texture index, but in most cases, you can identify later a texture
by his name, without providing his index, so you can probably ignore
this value.

0 is returned if texture loading have failed.
**/

__rayapi GLuint raydium_texture_load_erase (char *filename, GLuint to_replace);
/**
Same as above, but ##to_replace## texture (index) is erased with ##filename##.
**/

__rayapi void raydium_texture_free(int number);
/**
experimental function to free textures
**/

__rayapi void raydium_texture_free_name(char *name);
/**
experimental function to free textures by its name
**/

__rayapi int raydium_texture_is_slot_used(int slot);
/**
Returns true (1) if the texture ##slot## is used for a texture, and
therefore, valid.
**/

__rayapi int raydium_texture_get_next_free_slot_internal(void);
/**
Internal use. Will search a new free texture slot.
**/

__rayapi signed char raydium_texture_current_set (GLuint current);
/**
Switch active texture to "current" index. Mostly used for runtime object
creation:
"set current texture, add vertices, set another texture,
add vertices, ... and save all to an objet"
(see below for vertices management).
**/

__rayapi signed char raydium_texture_current_set_name (char *name);
/**
Same as above, but using texture name. This function will load ##name##
if not alread done.
**/

__rayapi GLuint raydium_texture_find_by_name (char *name);
/**
Returns index for texture "name", and load it if not already done.
**/

__rayapi GLint raydium_texture_exists(char *name);
/**
Same as above, but don't load texture if ##name## isn't already loaded and
then returns -1. Returns texture id otherwise.
**/

__rayapi void raydium_texture_npot_enable(void);
/**
You can allow the load of Non-power-of-two textures with this function.
**/

__rayapi void raydium_texture_npot_disable(void);
/**
Function to disabled the previous behaviour. By default Raydium already
has this behaviour disabled.
**/

__rayapi void raydium_texture_filter_change (GLuint filter);
/**

Change texture filter. The new filter will apply on all "next" textures,
but will not change already loaded ones (this was the case in old Raydium
releases), since it may generate strange bugs with dynamic (aka "faked")
textures, and it was very slow.

%%(c)
// will switch to bilinear filter for next textures
raydium_texture_filter_change(RAYDIUM_TEXTURE_FILTER_BILINEAR)%%
**/

__rayapi void raydium_texture_compression(signed char enable);
/**
This function allows Raydium to compress textures if hardware supports this
feature. You can also use the "--compress" command line switch.
The default is ##0## ("no").
**/

#endif
