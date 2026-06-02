/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

// Raydium configuration file
// For other options, see common.h

// Enable PHP support
#ifndef NO_PHP_SUPPORT
#define PHP_SUPPORT
#define PHP_INI_PATH "./"
#endif

// You may define this during compilation (-DRAYPHP_PATH=\"...\")
#ifndef RAYPHP_PATH
#define RAYPHP_PATH "rayphp"
#endif

#ifndef NO_ODE_SUPPORT
// Enable ODE physics support
#define ODE_SUPPORT
#define ODE_QUICKSTEP
#define ODE_PREDICTION
//#define ODE_NETWORK_GHOSTS
#endif

// RegApi Support (exports Raydium API to PHP)
#define REG_API

// enable profiling
#define DEBUG_PROFILE

// (link to OpenAL is always required !)
//#define NO_SOUND_DEBUG

// Allow Xinerama Support for X11
#define HAVE_XINERAMA

// Allow DPMS Support for X11
#define HAVE_DPMS

// will highlight triangle with tag != 0 at render time :
//#define RENDER_DEBUG_TAG

// disable clipping
//#define RENDER_DEBUG_NO_CLIP

// disable display list optims
//#define DEBUG_RENDER_DISABLE_DISPLAYLISTS

// draw shadow map (view from main light)
//#define DEBUG_SHADOW_MAP_VIEW

// draw HDR stencil (press F10)
//#define DEBUG_HDR_STENCIL

// debug network "tcp style" ACKs
//#define DEBUG_NETWORK

// debug ODE Network interface (verbose !)
//#define DEBUG_ODENET

// enable movie linear framerate (1/x sec)
// press F11 and see movie/ subdir [or create it]
//#define DEBUG_MOVIE 25

// allow vertex lighting for lightmaps textures
//#define RENDER_ALLOW_LIGHTING_FOR_LIGHTMAPS

// raydium_capture_frame_jpeg() JPEG quality percentage
#define DEBUG_JPEG_QUALITY      75

// axis used by Volumetric Fog when rendering meshes
// values: 0=x y=1 z=2
#define RENDER_VOLUMETRIC_FOG_AXIS      2

// reverse values from axis ?
#define RENDER_VOLUMETRIC_FOG_AXIS_REVERSE

// on iPhone 4G, disable retina screen (480x320 instead of native 960x640)
//#define IPHONEOS_NORETINA

// experimental Vertex Array renderer
//#define DEBUG_RENDER_VERTEXARRAY
//#define DEBUG_RENDER_VERTEXARRAY_GLBINDS_DISPLAY
