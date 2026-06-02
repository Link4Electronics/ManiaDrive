#!/bin/sh

# Small and ugly "full static compiler" for Raydium apps, with ODE
# Use "./ostatcomp.sh test6.c" for example.

# Warning, with GLEW 1.3.4, you must install libGLEW.a by hand, since
# the official Makefile is broken and strips this file during the make install

ulimit -c 0
rm test
gcc -g $1 -Wall -O2 -o test -static-libgcc \
-Wl,-dynamic-linker=/lib/ld-linux.so.2 -Wl,-Bstatic -L/usr/X11R6/lib/ \
-lm -lXi -ljpeg \
-Iraydium/ode/include/ raydium/ode/ode/src/libode.a \
-lGLEW -lXmu -lXinerama -lopenal -lalut -lvorbisfile -lvorbis -logg \
-Iraydium/php/ -Iraydium/php/main/ -Iraydium/php/Zend -Iraydium/php/TSRM raydium/php/libs/libphp5.a \
-lcrypt -lssl -lcrypto -lidn -lresolv -lz -lcurl -lxml2 -lssl -lcrypto -lidn -lstdc++ -lm -lX11 -lXext \
-Wl,-Bdynamic -lGL -lGLU -lasound -lpthread
# Strange thing above : -lssl -lcrypto must appear twice arround -lcurl ...
sync
shift
./test "$@"
