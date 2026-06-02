#ifndef _COMPAT_H
#define _COMPAT_H

/*=
Compatibility
9100
**/

// Introduction
/**
We try our best to keep the maximum compatibility between each version of
the engine. But sometimes, because of a bad design from the start, it's
a good idea to break things (a bit). To minimize maintenance in these cases,
we now provide compatibility flags.

Current flags are:

##RAYDIUM_COMPAT_ODE_OBJECT_MOVE_R1099##, to get old behavior for the
##raydium_ode_object_move()##, function where reference element was the last
one and not the first element of the object, as currently.
**/

__rayapi void raydium_compat_add(unsigned int flag);
/**
Add a new compatibility flag (or multiple ones, using the | operator) to
the application.
**/

__rayapi void raydium_compat_remove(unsigned int flag);
/**
Remove a compatibility flag.
**/

__rayapi signed char raydium_compat_isset(unsigned int flag);
/**
Test if a compatibility flag is currently set. Mostly used by the engine itself.
**/

#endif
