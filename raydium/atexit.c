/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

#ifndef DONT_INCLUDE_HEADERS
#include "index.h"
#else
#include "headers/atexit.h"
#endif

#include "atexit.h"

int raydium_atexit(void (*func)(void))
{
int i;
if(raydium_atexit_index==RAYDIUM_MAX_ATEXIT_FUNCTIONS)
    {
    raydium_log("atexit: no more free handlers (%i max)",RAYDIUM_MAX_ATEXIT_FUNCTIONS);
    return -1;
    }
for (i=0;i<RAYDIUM_MAX_ATEXIT_FUNCTIONS;i++)
    if (raydium_atexit_functions[i]==func)
        {
            raydium_log("atexit: attempt to register twice the same function.");
            return -1;
        }
raydium_atexit_functions[raydium_atexit_index++]=func;
return 0;
}

void raydium_atexit_call(void)
{
int i;
void (*f)(void);

for(i=raydium_atexit_index-1;i>=0;i--)
    {
    f=raydium_atexit_functions[i];
    f();
    }
}

void raydium_atexit_init(void)
{
raydium_atexit_index=0;

#ifndef RAYDLL
atexit(raydium_atexit_call);
#endif
}

