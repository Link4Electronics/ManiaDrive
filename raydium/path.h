/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

#ifndef PATH_H
#define PATH_H

#define RAYDIUM_MAX_PATHS       32
#define RAYDIUM_MAX_PACKAGES_FILES  16

#define RAYDIUM_PATH_MODE_READ  1
#define RAYDIUM_PATH_MODE_WRITE 2 // not used yet

#define RAYDIUM_PATH_PACKAGE_NOT_FOUND  -1

#define RAYDIUM_PATH_PACKAGE_READONLY       4
#define RAYDIUM_PATH_PACKAGE_READWRITE      8
#define RAYDIUM_PATH_PACKAGE_FREE           16


typedef struct raydium_path_Path
{
signed char state;
char path[RAYDIUM_MAX_DIR_LEN];
char ext[RAYDIUM_MAX_NAME_LEN];
int  priority;
signed char mode;
} raydium_path_Path;

__global raydium_path_Path raydium_path_paths[RAYDIUM_MAX_PATHS];
__global char raydium_path_write_current[RAYDIUM_MAX_DIR_LEN];
__global signed char raydium_path_write_local_dir_allowed;
__global char raydium_path_package_file[RAYDIUM_MAX_PACKAGES_FILES][RAYDIUM_MAX_DIR_LEN];
__global signed char raydium_path_package_mode_internal[RAYDIUM_MAX_PACKAGES_FILES];

#endif
