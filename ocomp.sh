#!/bin/sh

# Small and ugly "static compiler" for Raydium apps, with ODE
# Use "./ocomp.sh test6.c" for example.

if [ -f configure.conf ]; then
    . ./configure.conf
fi

ulimit -c 0
rm test
gcc -g "$1" -Wall -O2 $EXTRA_GCC_FLAGS -o test -L/usr/X11R6/lib/ -lXinerama -lGL -lGLU -lm -lopenal -lalut -ljpeg -lv4lconvert \
-Iraydium/ode/include/ raydium/ode/ode/src/libode.a -lvorbis -lvorbisfile -logg \
-Iraydium/php/ -Iraydium/php/include -Iraydium/php/main/ -Iraydium/php/Zend -Iraydium/php/TSRM raydium/php/libs/libphp5.a \
-lresolv -lcrypt -lz -lcurl -lxml2 -lGLEW -lpng -lX11 -lXext -ldl -lstdc++ $OCOMP_FLAGS
sync
shift
./test "$@"
