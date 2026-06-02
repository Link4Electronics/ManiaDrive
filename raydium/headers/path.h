#ifndef _PATH__H
#define _PATH__H
#include "../path.h"

/*=
File path
2110
**/

// Introduction
/**
When ##raydium_file_fopen()## is called, by Raydium or by an application,
the Path API is used to search the file.
When the file is open for writing, Raydium will check if the current working
directory is writable. If this is not the case, Raydium will try to write the
file in the user home directory (~/.appname/data/), and will create it if
needed.
For reading, Raydium will also use the current directory **first**, and then
will search in a list of directories of your choice (see example below).
The user home directory is registered by default in this list.

Now, the search patch system is able to work even if the '/' character is
present in the requested filename, so you can use subdirectories if needed.

Each path can have a ##priority## (lower is better).
Application local directory have highest structural priority.
Path add by user have default 2 priority.
Packages have a priority of 5. Packages files are searched just after local directory.
~/.appname/data have lowest priority of 7

Raydium now features a data package system, so you can store some files in
a regular ZIP file (with subdirectories if you want) and then register this
package in your application. Raydium will then automatically search files
in the ZIP.
**/

// Example of directory registering:
/**
%%(c)
raydium_path_ext("./media/textures/","tga");
raydium_path_ext("./media/fonts/","tga");
raydium_path_ext("./media/shaders/","vert");
raydium_path_ext("./media/shaders/","frag");
raydium_path_ext("./media/meshes/","tri");
raydium_path_ext("./media/themes/","gui");
raydium_path_ext("./media/particles/","prt");
raydium_path_ext("./media/cars/","car");
raydium_path_ext("./media/cams/","cam");
raydium_path_add("./media/"); // "unsorted" files
%%
**/

__rayapi int raydium_path_find_free(void);
/**
Internal (search a free path slot).
**/

__rayapi signed char raydium_path_ext(char *dir, char *ext);
/**
Register ##dir## directory for files with ##ext## extension.
Return 0 when it fails.
**/

__rayapi signed char raydium_path_ext_priority(char *dir,char *ext,int priority);
/**
Register ##dir## directory for files with ##ext## extension.
Can specify path ##priority## (##1## is ##highest## priority).
Return 0 when if fails.
**/

__rayapi signed char raydium_path_add(char *dir);
/**
Register ##dir## directory.
Return 0 when it fails.
**/

__rayapi signed char raydium_path_find(char *pathfolder);
/**
Returns 1 if the indicated folder is already in the path.
**/


__rayapi signed char raydium_path_add_priority(char *dir, int priority);
/**
Register ##dir## directory.
Can specify ##priority## (##1## is ##highest## priority).
Return 0 when it fails.
**/

__rayapi signed char raydium_path_write(char *dir);
/**
Change the writing directory to ##dir## directory. You should probably also
register this new directory, using ##raydium_path_add()##.
**/

__rayapi signed char raydium_path_string_from(char *str);
/**
Reset all registrations (only current directory stays) and replace it with the
provided formated string. Here it is an example for such string:

%%/home/xfennec/.myapp/data:./media/textures/X.tga:./media/fonts/X.tga:
./media/shaders/X.vert:./media/shaders/X.frag:./media/meshes/X.tri:
./media/themes/X.gui:./media/particles/X.prt:./media/cars/X.car:
./media/cams/X.cam:./media%%
(do not include line feeds and replace 'X' by '*')

This string is based on the example at the top of this chapter.
**/

__rayapi int raydium_path_string_to(char *out);
/**
Dumps all registrations to ##out## string.
**/

__rayapi void raydium_path_resolv(char *in, char *out, char mode);
/**
Internal (find the best absolute path for the requested file).
**/

__rayapi void raydium_path_dump(void);
/**
Dumps read and write paths to console.
**/

__rayapi void raydium_path_reset(void);
/**
Reset all registrations.
You should probably better use ##raydium_path_string_from()##.
**/

__rayapi void raydium_path_write_local_deny(signed char deny);
/**
By default, Raydium always tries to write in the local directory (binary's
directory, in other words), and if it's not possible, did it in the registered
writing directory. Using this function (with deny=1) will force Raydium to use
the second option each time. Then Raydium will never write to local directory.
**/

__rayapi void raydium_path_init(void);
/**
Internal.
**/

__rayapi int raydium_path_package_find(char *name);
/**
Return path index associated with package name ##name##.
**/

__rayapi int raydium_path_package_find_free(void);
/**
Return free package slot.
Return -1 if ##RAYDIUM_MAX_PACKAGES_FILES## already registered.
**/

__rayapi signed char raydium_path_package_register(char *file);
/**
Call this function to register a new ZIP package (note that you can change
the file extension if you want). All files in the archive will be available
to the application with no other change !

This feature is R3S compliant, so the ZIP file may be downloaded automatically.

There's no persistence, you should register your packages at every run. A cache
system makes the proccess very quick, as file access. Any change made to the
registered ZIP file discards the cache automatically.

Package have a default 'Middle' priority of ##5## -> File are first searched
in local directory, then in path added explicitly by user and then in ##package## file.

**/

__rayapi signed char raydium_path_package_internal_add(char * file);
/**
Internal. Add a package to list of registered package.
**/

__rayapi void raydium_path_package_mode(char * name,unsigned char mode);
/**
Define package mode: ##RAYDIUM_PACKAGE_READONLY## / ##RAYDIUM_PACKAGE_READWRITE##
With ##RAYDIUM_PACKAGE_READWRITE## all opened files are automaticaly added/refreshed
in package at application exit.
Package mode is written in package file.
To modify package mode use console within the application.
**/

__rayapi signed char raydium_path_package_cache_clear(void);
/**
This function will clear the packages cache, and exit the application (for
some reasons, including laziness).
**There's no reason to call this function**, cache is managed by Raydium itself.
But you can use it to clear all old useless cache entries and get a bit more
free disk space, why not.
**/

__rayapi void raydium_path_package_register_all_cli(void);
/**
Internal.
**/

#endif
