## Makefile for Raydium dynamic compiled apps.

SHARE="."
-include configure.conf

EXT_H = .h
ROOT = ./
RAYDIUM_DIR = raydium/
HEADERS_DIR = raydium/headers/
COMPILE_DIR = raydium/compile/
CC = gcc
AR = ar
RANLIB = ranlib
SYSTEM_LIBS =  -lGL -lGLU -lXinerama -lm -ljpeg -lopenal -lalut -lvorbis -lvorbisfile -logg -lresolv -lcrypt -lz -lcurl -lxml2 -lpng -lGLEW -lv4lconvert
OTHER_LIBS =  raydium/ode/ode/src/libode.a raydium/php/libs/libphp5.a
INCLUDE_PATH =  -Iraydium/ode/include/ -Iraydium/php/ -Iraydium/php/include -Iraydium/php/main/ -Iraydium/php/Zend -Iraydium/php/TSRM -I/usr/include/curl
LIBS_PATH =  -L/usr/X11R6/lib/
CFLAGS = -Wall -Wno-unused-result
COMPILE_OPTIONS = -g -DLIBRAY -O2 -DRAYPHP_PATH=\"$(SHARE)/rayphp\"
ARC := $(shell uname -m)
ifeq ($(ARC),x86_64)
	COMPILE_OPTIONS+= -fPIC
endif
## ... just because of Shell syntax in configure.conf
ifneq ($(EXTRA_GCC_FLAGS),"")
## (the echo thing is here to remove quotes)
	COMPILE_OPTIONS+= $(shell echo $(EXTRA_GCC_FLAGS))
endif
LDFLAGS=
LINKING_OPTIONS=-Wl,-soname,libraydium.so.0
AR_OPTIONS=
HEADERS=raydium/headers/background.h raydium/headers/callback.h raydium/headers/camera.h raydium/headers/capture.h raydium/headers/clear.h raydium/headers/console.h raydium/headers/file.h raydium/headers/file_tri.h raydium/headers/fog.h raydium/headers/init.h raydium/headers/cli.h raydium/headers/internal.h raydium/headers/joy.h raydium/headers/key.h raydium/headers/land.h raydium/headers/light.h raydium/headers/log.h raydium/headers/main.h raydium/headers/mouse.h raydium/headers/network.h raydium/headers/normal.h raydium/headers/object.h raydium/headers/ode.h raydium/headers/osd.h raydium/headers/parser.h raydium/headers/particle2.h raydium/headers/php.h raydium/headers/profile.h raydium/headers/random.h raydium/headers/rayphp.h raydium/headers/register.h raydium/headers/render.h raydium/headers/signal.h raydium/headers/sky.h raydium/headers/sound.h raydium/headers/texture.h raydium/headers/timecall.h raydium/headers/math.h raydium/headers/vertex.h raydium/headers/window.h raydium/headers/reg_api.h raydium/headers/gui.h raydium/headers/live.h raydium/headers/video.h raydium/headers/shadow.h raydium/headers/myglut.h raydium/headers/web.h raydium/headers/hdr.h raydium/headers/lensflare.h raydium/headers/shader.h raydium/headers/atexit.h raydium/headers/path.h raydium/headers/sprites.h raydium/headers/compat.h

OBJECTS      = $(HEADERS:raydium/headers/%.h=raydium/compile/%.o)

all: libraydium.a libraydium.so
	@echo "Files created: $+"

help:
	@echo "Help compiling raydium"
	@echo "	make help	this"
	@echo "	make all	create libraries"
	@echo "	make doc	generate raydium documentation in file doc.wiki"
	@echo "	make clean	remove compilation objects and libraries"
	@echo "	make install	install libraries, includes and RayPHP shared files"

doc: $(HEADERS)
	cd raydium/headers && ./raydoc.php > ../../doc.wiki && cd -

install: all
	@echo "Installing ..."
	@./install.sh

################################################################################
# Create libraries
################################################################################

libraydium.so.0.0: $(OBJECTS)
	@$(CC) $(LDFLAGS) $(LINKING_OPTIONS) -o $@ -shared $+ $(LIBS_PATH) $(OTHER_LIBS) $(SYSTEM_LIBS)
	@echo "File created: $@"

libraydium.so.0: libraydium.so.0.0
	@ln -sf $< $@ || true

libraydium.so: libraydium.so.0
	@ln -sf $< $@ || true

libraydium.a.0.0: $(OBJECTS)
	@$(AR) $(AR_OPTIONS) -crv $@ $+
	@$(RANLIB) $@ || true
	@echo "File created: $@"

libraydium.a.0: libraydium.a.0.0
	@ln -sf $< $@ || true

libraydium.a: libraydium.a.0
	@ln -sf $< $@ || true

################################################################################
# Cleaning rule
################################################################################

clean:
	rm -r $(COMPILE_DIR)	|| true
	rm libraydium.so*	|| true
	rm libraydium.a*	|| true

################################################################################
# All files
################################################################################

raydium/compile/%.o: raydium/%.c $(HEADERS)
	@mkdir -p $(COMPILE_DIR)
	@echo "Creating: $@"
	@$(CC) $(COMPILE_OPTIONS) $(CFLAGS) -o $@ -c $< $(INCLUDE_PATH)
