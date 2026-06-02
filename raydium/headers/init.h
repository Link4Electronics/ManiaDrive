#ifndef _INIT_H
#define _INIT_H
/*=
Initialization
2400
**/

// Introduction
/**
This file is mainly designed for internal uses, but there's anyway
some interesting functions.
**/

__rayapi char *raydium_version(void);
/**
Return Raydium Engine version as a static string. Format is "x.yyy".
You can also find defines for this, named ##RAYDIUM_MAJOR## (x)
and ##RAYDIUM_MINOR## (yyy).
**/

__rayapi  void raydium_init_lights (void);
/**
Internal use. Must be moved to light.c.
**/

__rayapi  void raydium_init_objects (void);
/**
Internal use. Must be moved to object.c.
**/

__rayapi  void raydium_init_key (void);
/**
Internal use. Must be moved to key.c.
**/

__rayapi void raydium_init_purgemem(void);
/**
The goal of this function is to free and unloads all heavy things in the engine.
Main targets are textures, objects, vertices and all RayODE entities,
but dependencies includes live textures, lensflares, shaders, particles,
shadows, GUI and OSD.

Please, note that the goal of this function is not to reset or restart the
engine, a lot of things will not be changed (lights, cameras, timecalls,
viewport, regapi, PHP, network, ... The idea is to free memory, nothing else.

In other words, consider this function as almost useless and only needed for
some very special cases, when you need to load entire new scenes, many
times. Raydium is supposed to be able to deal without this function.

Also, the code is still very experimental, and calling context should be
friendly ! Do not try this in the middle of a callback, for instance.
**/

__rayapi  void raydium_init_reset (void);
/**
Internal, part of engine initialisation. At first, this function was supposed
to be able to help restart the engine. It failed.
**/

__rayapi  void raydium_init_engine (void);
/**
Internal use. **Never** call this function by yourself, it may cause
huge memory leaks.
**/

__rayapi int raydium_init_load(char *filename);
/**
This function is used to load a configuration file, ##filename##, and then it
will do automatically all initialization of the application (window size,
title, lights, skybox, ...)

The ##filename## config file must follow the "variable=value;" pattern, one
variable per line. It allows comments (with double slash). See chapter about
text file parsing for more information about syntax.

If the file does not exists (local or R3S servers), Raydium will create a
default one.

A list of all settings will follow here, soon.

It returns 1 if the load process ends correctly, or 0 is something was wrong.
**/

#endif
