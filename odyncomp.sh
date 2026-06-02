#!/bin/sh

# Small and ugly "dynamic compiler" for Raydium apps, with ODE
# Use "./odyncomp.sh test6.c" for example.

# When you need special dependencies (should work, but not supported !)
# you may define following env vars:

# AR_PATH
#       for AR Toolkit (http://artoolkit.sourceforge.net/)

# LIBWII_INCLUDES
#       for libcwiimote (http://libwiimote.sourceforge.net)

# ODYNCOMP_FLAGS
#       for anything else you may need ...

# Known bugs: AR_PATH and LIBWII_INCLUDES must not contain space character ...

ulimit -c 0
make

if [ "$?" != "0" ]; then
    echo "build failed"
    exit 1
fi

if [ -z "$CC" ]; then
    CC="gcc";
fi

if [ -n "$AR_PATH" ]; then
    if [ ! -d "$AR_PATH" ]; then
        echo "Unable to find ARToolKit (AR_PATH env)"
        exit 1
    fi
    AR_PATH_INCLUDE="-I $AR_PATH/include"
    AR_PATH_LIBS="$AR_PATH/lib/libAR.a"
fi

# Wiimote(s) support
# http://libwiimote.sourceforge.net/
if [ -n "$LIBWII_INCLUDES" ]; then
    if [ ! -f "$LIBWII_INCLUDES/wiimote.h" ]; then
        echo "Unable to find libwiimote includes (LIBWII_INCLUDES env)"
        exit 1
    fi
    LIBWII_PATH_INCLUDE="-I $LIBWII_INCLUDES"
    LIBWII_ADDS="-D_ENABLE_TILT -D_ENABLE_FORCE -lbluetooth -lcwiimote"
fi

if [ -f configure.conf ]; then
    . ./configure.conf
fi

if [ -f test ]; then
    rm test
fi


# Needed because of Fedora's explicit DSO linking
EXTRA_APPS_LIBS="-lGL -lGLEW -lm"

$CC "$1" -g -Wall -DFORCE_LIBRAYDIUM $EXTRA_GCC_FLAGS -O2 -o test libraydium.so $EXTRA_APPS_LIBS \
-Iraydium/php/ -Iraydium/php/main/ -Iraydium/php/Zend -Iraydium/php/TSRM \
-Iraydium/ode/include/ $AR_PATH_INCLUDE $AR_PATH_LIBS $LIBWII_PATH_INCLUDE $LIBWII_ADDS $ODYNCOMP_FLAGS

ret=$?
if [ $ret -ne 0 ]; then
    exit $ret
fi

if [ -z "$DISABLE_AUTORUN" ]; then
    export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH
    shift
    ./test "$@"
fi
