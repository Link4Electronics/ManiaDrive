/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

#ifndef ATEXIT_H
#define ATEXIT_H

#define RAYDIUM_MAX_ATEXIT_FUNCTIONS    32

__global void *raydium_atexit_functions[RAYDIUM_MAX_ATEXIT_FUNCTIONS];
__global int raydium_atexit_index;

// proto
__rayapi void raydium_log (char *format, ...);

#endif
