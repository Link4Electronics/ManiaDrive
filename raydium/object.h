/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

#ifndef OBJECT_H
#define OBJECT_H

#define RAYDIUM_OBJECT_RENDER_CACHE_MAXPARTS 2048

typedef struct raydium_object_render_Part {
    int len;
    int start;
    GLuint texture[2]; // basic multitexturing (not "env")
} raydium_object_render_Part;

typedef struct raydium_object_render_Cache {
    int n_parts;
    raydium_object_render_Part *parts[RAYDIUM_OBJECT_RENDER_CACHE_MAXPARTS];
} raydium_object_render_Cache;

__global raydium_object_render_Cache raydium_object_cache[RAYDIUM_MAX_OBJECTS];

#endif
