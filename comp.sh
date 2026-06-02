#!/bin/sh

# Small and ugly "static compiler" for Raydium apps, without ODE
# Use "./comp.sh raydium_modler.c" for example.

ulimit -c 0
rm test
gcc $1 -Wall -o test -L/usr/X11R6/lib/ -lXinerama -lGL -lGLU -lm -lopenal -lalut -lvorbis -logg -lvorbisfile -ljpeg \
-Iraydium/php/ -Iraydium/php/main/ -Iraydium/php/Zend -Iraydium/php/TSRM raydium/php/libs/libphp5.a \
-lresolv -lcrypt -lz -lcurl -lxml2 -lGLEW
sync
shift
./test "$@"
