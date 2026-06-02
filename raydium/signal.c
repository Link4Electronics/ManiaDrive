/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

#ifndef DONT_INCLUDE_HEADERS
#include "index.h"
#else
#include "headers/signal.h"
#endif

void raydium_signal_handler(int sig)
{
static signed char already=0;

if(!already)
    {
    raydium_log("Signal %i disabled. Call it again to confirm.",sig);
    already=1;
    }
else
    {
    // May be called multiple times (when an atexit fails, for instance)
    raydium_log("Accepting signal %i, exiting.");
    exit(1);
    }
}

void raydium_signal_install_trap(void)
{
signed int err=0;

if(signal(SIGINT,raydium_signal_handler)==SIG_ERR)
    err++;
#ifndef WIN32
if(signal(SIGPIPE,raydium_signal_handler)==SIG_ERR)
    err++;
#endif

if(err)
    raydium_log("Signal Handlers: FAILED !");
else
    raydium_log("Signal Handlers: OK");
}
