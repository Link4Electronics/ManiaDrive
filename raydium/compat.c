/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

#ifndef DONT_INCLUDE_HEADERS
#include "index.h"
#else
#include "headers/compat.h"
#endif

void raydium_compat_add(unsigned int flag)
{
raydium_compat_flags|=flag;
raydium_log("%i",raydium_compat_flags);
}

void raydium_compat_remove(unsigned int flag)
{
raydium_compat_flags &= ~flag;
}

signed char raydium_compat_isset(unsigned int flag)
{
return raydium_compat_flags&flag;
}
