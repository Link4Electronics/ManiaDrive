/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

#ifndef DONT_INCLUDE_HEADERS
#include "index.h"
#else
#include "headers/profile.h"
#endif

// Simple "profiling" functions
// Only one profiler can be used a the same time

#ifdef DEBUG_PROFILE

void raydium_profile_start(void)
{
raydium_profile_timer=raydium_timecall_clock();
}


void raydium_profile_end(char *tag)
{
unsigned long del;
del=raydium_timecall_clock()-raydium_profile_timer;

printf("*** profiler: %40s | %.4f ms\n",tag,del/(double)raydium_timecall_clocks_per_sec*1000);
}


#else

#define raydium_profile_start()
#define raydium_profile_end(a)

#endif
