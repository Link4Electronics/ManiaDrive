cmake_minimum_required(VERSION 3.16)
project(ManiaDrive VERSION 1.3.0 LANGUAGES C CXX)

# Suppress CMP0135: use the timestamp of extraction for downloaded archives
if(POLICY CMP0135)
  cmake_policy(SET CMP0135 NEW)
endif()

list(PREPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# ─── Options ──────────────────────────────────────────────
option(KIDS_MODE  "Enable kids mode (unlock all tracks, speed/accel sliders)" OFF)

# ─── C Standard ───────────────────────────────────────────
# The codebase uses GNU89 extensions and needs -fcommon for GCC 10+
add_compile_options(-fcommon)
set(CMAKE_C_STANDARD 90)
set(CMAKE_C_EXTENSIONS ON)

# ─── Dependencies ─────────────────────────────────────────
find_package(OpenGL REQUIRED)
if(NOT TARGET OpenGL::GLU AND OPENGL_glu_LIBRARY)
  add_library(OpenGL::GLU UNKNOWN IMPORTED)
  set_target_properties(OpenGL::GLU PROPERTIES
    IMPORTED_LOCATION "${OPENGL_glu_LIBRARY}"
    INTERFACE_LINK_LIBRARIES OpenGL::GL
    INTERFACE_INCLUDE_DIRECTORIES "${OPENGL_INCLUDE_DIR}"
  )
endif()
find_package(GLEW QUIET)
if(NOT GLEW_FOUND)
  if(PkgConfig_FOUND)
    pkg_check_modules(GLEW REQUIRED IMPORTED_TARGET glew)
  else()
    find_path(GLEW_INCLUDE_DIR GL/glew.h)
    find_library(GLEW_LIBRARY GLEW)
    if(NOT GLEW_LIBRARY)
      message(FATAL_ERROR "GLEW not found. Install libglew-dev or glew-devel.")
    endif()
    add_library(GLEW::GLEW UNKNOWN IMPORTED)
    set_target_properties(GLEW::GLEW PROPERTIES
      IMPORTED_LOCATION "${GLEW_LIBRARY}"
      INTERFACE_INCLUDE_DIRECTORIES "${GLEW_INCLUDE_DIR}"
    )
  endif()
endif()
find_package(OpenAL  REQUIRED)
if(NOT TARGET OpenAL::OpenAL AND OPENAL_LIBRARY)
  add_library(OpenAL::OpenAL UNKNOWN IMPORTED)
  set_target_properties(OpenAL::OpenAL PROPERTIES
    IMPORTED_LOCATION "${OPENAL_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${OPENAL_INCLUDE_DIR}"
  )
endif()
find_package(JPEG    REQUIRED)
find_package(PNG     REQUIRED)
find_package(ZLIB    REQUIRED)
find_package(CURL    REQUIRED)
find_package(LibXml2 REQUIRED)
find_package(X11     REQUIRED)

find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
  pkg_check_modules(ALUT    freealut IMPORTED_TARGET)
  pkg_check_modules(VORBIS  vorbis   IMPORTED_TARGET)
  pkg_check_modules(VORBF   vorbisfile IMPORTED_TARGET)
  pkg_check_modules(OGG     ogg      IMPORTED_TARGET)
  pkg_check_modules(V4LCONV libv4lconvert IMPORTED_TARGET)
endif()

# ALUT is handled below via FindALUT.cmake fallback
if(NOT TARGET PkgConfig::VORBIS)
  find_library(VORBIS_LIBRARY  vorbis)
endif()
if(NOT TARGET PkgConfig::VORBF)
  find_library(VORBF_LIBRARY   vorbisfile)
endif()
if(NOT TARGET PkgConfig::OGG)
  find_library(OGG_LIBRARY     ogg)
endif()
if(NOT TARGET PkgConfig::V4LCONV)
  find_library(V4LCONV_LIBRARY v4lconvert)
endif()

# FindX11 sets X11_Xinerama_LIB and X11_Xrandr_LIB (optional)
if(X11_Xrandr_FOUND OR X11_Xrandr_LIB)
  set(HAVE_XRANDR TRUE)
endif()

# ─── ODE (Open Dynamics Engine) ──────────────────────────
set(ODE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/ODE" CACHE PATH
  "Path to pre-extracted ODE 0.16+ source tree. Set to empty to search system or fetch.")

if(ODE_SOURCE_DIR)
  # Use the pre-extracted source directly via add_subdirectory
  if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.21")
    set(CMAKE_POLICY_VERSION_MINIMUM "3.5")
  endif()
  set(ODE_WITH_DEMOS               OFF CACHE BOOL "" FORCE)
  set(ODE_WITH_TESTS               OFF CACHE BOOL "" FORCE)
  set(ODE_WITH_TOOLS               OFF CACHE BOOL "" FORCE)
   set(ODE_DOUBLE_PRECISION          OFF CACHE BOOL "" FORCE)
   set(ODE_NO_BUILD_EXAMPLES         ON CACHE BOOL "" FORCE)
   add_subdirectory("${ODE_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/ode-build")
  if(NOT TARGET ODE::ODE AND TARGET ODE)
    add_library(ODE::ODE ALIAS ODE)
  endif()
else()
  # Try system-installed ODE first. If not found, download modern ODE.
  find_package(ODE QUIET)
  if(NOT ODE_FOUND)
    if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.21")
      set(CMAKE_POLICY_VERSION_MINIMUM "3.5")
    endif()
    include(FetchContent)
    FetchContent_Declare(
      ode
      GIT_REPOSITORY https://bitbucket.org/odedevs/ode.git
      GIT_TAG        0.16.5
    )
    set(ODE_WITH_DEMOS               OFF CACHE BOOL "" FORCE)
    set(ODE_WITH_TESTS               OFF CACHE BOOL "" FORCE)
    set(ODE_WITH_TOOLS               OFF CACHE BOOL "" FORCE)
    set(ODE_DOUBLE_PRECISION          ON CACHE BOOL "" FORCE)
   set(ODE_NO_BUILD_EXAMPLES         ON CACHE BOOL "" FORCE)
   set(ODE_NO_BUILTIN_THREADING_IMPL ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(ode)

    if(NOT TARGET ODE::ODE AND TARGET ODE)
      add_library(ODE::ODE ALIAS ODE)
    endif()
  endif()
endif()

# Ensure we have an ODE::ODE target
if(NOT TARGET ODE::ODE)
  message(FATAL_ERROR "Could not find or build ODE. "
                      "Install libode-dev, set ODE_SOURCE_DIR to a pre-extracted "
                      "ODE 0.16+ source tree, or ensure network access for FetchContent.")
endif()

# ─── PHP 5.3 Embedded ────────────────────────────────────
find_program(PHP_BISON bison)
if(NOT PHP_BISON)
  message(STATUS "bison not found — will use pre-generated PHP parser files")
else()
  message(STATUS "bison found: ${PHP_BISON}")
endif()
find_program(PHP_RE2C re2c)
if(NOT PHP_RE2C)
  message(STATUS "re2c not found — PHP parser regeneration disabled, using pre-generated files")
endif()

set(PHP_EXTRACTED_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/php-5.3.27" CACHE PATH
  "Path to pre-extracted PHP 5.3 source tree. Set to empty to use tarball.")

if(PHP_EXTRACTED_DIR)
  # Use pre-extracted source directory directly
  set(PHP_SOURCE_DIR "${PHP_EXTRACTED_DIR}")
  set(PHP_LIBRARY     "${PHP_EXTRACTED_DIR}/libs/libphp5.a")
  set(PHP_INCLUDE_DIR ${PHP_SOURCE_DIR})

  add_custom_target(libphp5
    COMMAND cd "${PHP_SOURCE_DIR}" &&
            CFLAGS="-std=gnu89" ./configure --enable-embed=static --with-pic
                        --without-pear --disable-cgi --disable-cli
                        --disable-phpdbg --disable-fpm
                        --disable-simplexml --disable-xmlreader
                        --disable-xmlwriter --disable-dom --with-zlib
                        --enable-ftp --with-curl --enable-zip
                        --enable-soap --disable-phar --disable-crypt
                        --without-gd --without-jpeg-dir
                        --without-png-dir
                        --with-libxml-dir=${LIBXML2_INCLUDE_DIR}/..
    COMMAND make -C "${PHP_SOURCE_DIR}" EXTRA_CFLAGS="-DHAVE_BUNDLED_PCRE=1"
    BYPRODUCTS "${PHP_LIBRARY}"
    WORKING_DIRECTORY "${PHP_EXTRACTED_DIR}"
    COMMENT "Configuring & building PHP 5.3 (pre-extracted source)"
  )

  add_library(php5 STATIC IMPORTED GLOBAL)
  set_target_properties(php5 PROPERTIES IMPORTED_LOCATION ${PHP_LIBRARY})
  add_dependencies(php5 libphp5)

  set(PHP_INCLUDE_DIRS
    ${PHP_SOURCE_DIR}
    ${PHP_SOURCE_DIR}/main
    ${PHP_SOURCE_DIR}/Zend
    ${PHP_SOURCE_DIR}/TSRM
    ${PHP_SOURCE_DIR}/include
  )
else()
  # Tarball-based extraction
  set(PHP_TARBALL "${CMAKE_CURRENT_SOURCE_DIR}/raydium/php-latest.tar.gz")
  if(NOT EXISTS ${PHP_TARBALL})
    message(FATAL_ERROR "PHP tarball not found at ${PHP_TARBALL}. "
                        "Place php-5.3.27.tar.gz at that path, "
                        "or set PHP_EXTRACTED_DIR to a pre-extracted PHP 5.3 source tree.")
  endif()

  include(ExternalProject)
  set(PHP_BINARY_DIR  ${CMAKE_CURRENT_BINARY_DIR}/php-build)
  set(PHP_SOURCE_DIR  ${PHP_BINARY_DIR}/src/libphp5)
  set(PHP_LIBRARY     ${PHP_BINARY_DIR}/libs/libphp5.a)
  set(PHP_INCLUDE_DIR ${PHP_SOURCE_DIR})

  ExternalProject_Add(libphp5
    URL                   file://${PHP_TARBALL}
    URL_STRIP_COMPONENTS  1
    SOURCE_DIR            ${PHP_SOURCE_DIR}
    CONFIGURE_COMMAND     cd ${PHP_SOURCE_DIR} && ./configure
                          --enable-embed=static
                          --with-pic
                          --without-pear
                          --disable-cgi
                          --disable-cli
                          --disable-phpdbg
                          --disable-fpm
                          --disable-simplexml
                          --disable-xmlreader
                          --disable-xmlwriter
                          --disable-dom
                          --with-zlib
                          --enable-ftp
                          --with-curl
                          --enable-zip
                          --enable-soap
                          --disable-phar
                          --disable-crypt
                          --without-gd
                          --without-jpeg-dir
                          --without-png-dir
                          --with-libxml-dir=${LIBXML2_INCLUDE_DIR}/..
    BUILD_COMMAND        make -C ${PHP_SOURCE_DIR} EXTRA_CFLAGS=-DHAVE_BUNDLED_PCRE=1
    BUILD_IN_SOURCE      TRUE
    INSTALL_COMMAND      ""
    LOG_CONFIGURE        1
    LOG_BUILD            1
    BUILD_BYPRODUCTS     ${PHP_LIBRARY}
  )
  ExternalProject_Get_property(libphp5 SOURCE_DIR)

  add_library(php5 STATIC IMPORTED GLOBAL)
  set_target_properties(php5 PROPERTIES IMPORTED_LOCATION ${PHP_LIBRARY})
  add_dependencies(php5 libphp5)

  set(PHP_INCLUDE_DIRS
    ${PHP_SOURCE_DIR}
    ${PHP_SOURCE_DIR}/main
    ${PHP_SOURCE_DIR}/Zend
    ${PHP_SOURCE_DIR}/TSRM
    ${PHP_SOURCE_DIR}/include
  )
endif()

# ─── Raydium Engine Library ──────────────────────────────
set(RAYDIUM_SOURCES
  raydium/atexit.c
  raydium/background.c
  raydium/callback.c
  raydium/camera.c
  raydium/capture.c
  raydium/clear.c
  raydium/cli.c
  raydium/compat.c
  raydium/console.c
  raydium/file.c
  raydium/file_tri.c
  raydium/fog.c
  raydium/gui.c
  raydium/hdr.c
  raydium/init.c
  raydium/internal.c
  raydium/joy.c
  raydium/key.c
  raydium/land.c
  raydium/lensflare.c
  raydium/light.c
  raydium/live.c
  raydium/log.c
  raydium/main.c
  raydium/math.c
  raydium/mouse.c
  raydium/myglut.c
  raydium/network.c
  raydium/normal.c
  raydium/object.c
  raydium/ode.c
  raydium/osd.c
  raydium/parser.c
  raydium/particle2.c
  raydium/path.c
  raydium/profile.c
  raydium/random.c
  raydium/reg_api.c
  raydium/register.c
  raydium/render.c
  raydium/shader.c
  raydium/shadow.c
  raydium/signal.c
  raydium/sky.c
  raydium/sound.c
  raydium/sprites.c
  raydium/texture.c
  raydium/timecall.c
  raydium/vertex.c
  raydium/video.c
  raydium/web.c
  raydium/window.c
)

list(APPEND RAYDIUM_SOURCES
  raydium/php.c
  raydium/rayphp.c
)

add_library(raydium SHARED ${RAYDIUM_SOURCES})

target_include_directories(raydium
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/raydium
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/raydium>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/raydium/headers>
)

target_compile_definitions(raydium
  PRIVATE
    LIBRAY
    FORCE_LIBRAYDIUM
    PHP_SUPPORT
    HAVE_BUNDLED_PCRE=1
    PHP_INI_PATH="./"
    ODE_SUPPORT
    ODE_QUICKSTEP
    ODE_PREDICTION
    REG_API
    DEBUG_PROFILE
    HAVE_XINERAMA
    HAVE_DPMS
    RAYPHP_PATH="${CMAKE_SOURCE_DIR}/rayphp"
)

# Link system libraries
target_link_libraries(raydium
  PRIVATE
    OpenGL::GL
    OpenGL::GLU
    GLEW::GLEW
    OpenAL::OpenAL
    ZLIB::ZLIB
    JPEG::JPEG
    PNG::PNG
    CURL::libcurl
    LibXml2::LibXml2
    X11::X11
)

if(TARGET PkgConfig::ALUT)
  target_link_libraries(raydium PRIVATE PkgConfig::ALUT)
else()
  find_package(ALUT REQUIRED)
  target_link_libraries(raydium PRIVATE ALUT::ALUT)
endif()

if(TARGET PkgConfig::VORBIS)
  target_link_libraries(raydium PRIVATE PkgConfig::VORBIS)
elseif(VORBIS_LIBRARY)
  target_link_libraries(raydium PRIVATE ${VORBIS_LIBRARY})
endif()

if(TARGET PkgConfig::VORBF)
  target_link_libraries(raydium PRIVATE PkgConfig::VORBF)
elseif(VORBF_LIBRARY)
  target_link_libraries(raydium PRIVATE ${VORBF_LIBRARY})
endif()

if(TARGET PkgConfig::OGG)
  target_link_libraries(raydium PRIVATE PkgConfig::OGG)
elseif(OGG_LIBRARY)
  target_link_libraries(raydium PRIVATE ${OGG_LIBRARY})
endif()

if(TARGET PkgConfig::V4LCONV)
  target_link_libraries(raydium PRIVATE PkgConfig::V4LCONV)
elseif(V4LCONV_LIBRARY)
  target_link_libraries(raydium PRIVATE ${V4LCONV_LIBRARY})
endif()

target_link_libraries(raydium
  PRIVATE
    m
    resolv
    crypt
    dl
    pthread
    stdc++
)

if(X11_Xrandr_LIB)
  target_link_libraries(raydium PRIVATE ${X11_Xrandr_LIB})
endif()
if(X11_Xinerama_LIB)
  target_link_libraries(raydium PRIVATE ${X11_Xinerama_LIB})
endif()

target_link_libraries(raydium PUBLIC ODE::ODE)

target_include_directories(raydium PRIVATE ${PHP_INCLUDE_DIRS})
target_link_libraries(raydium PRIVATE php5)

# Ensure raydium headers are findable by consuming targets
target_include_directories(raydium
  INTERFACE
    $<INSTALL_INTERFACE:include>
)

# ─── ManiaDrive executable ───────────────────────────────
add_executable(mania_drive mania_drive.c)
target_link_libraries(mania_drive PRIVATE raydium OpenGL::GL m)

target_include_directories(mania_drive PRIVATE ${PHP_INCLUDE_DIRS})

target_compile_definitions(mania_drive
  PRIVATE
    FORCE_LIBRAYDIUM
    PHP_SUPPORT
    ODE_SUPPORT
)

if(KIDS_MODE)
  target_compile_definitions(mania_drive PRIVATE KIDS_MODE)
  message(STATUS "Kids mode: ON (all tracks unlocked, speed/accel sliders available)")
else()
  message(STATUS "Kids mode: OFF")
endif()

set_target_properties(mania_drive PROPERTIES
  OUTPUT_NAME mania_drive
  RUNTIME_OUTPUT_NAME mania_drive
)

# ─── Level Editor executable ─────────────────────────────
add_executable(level_editor mania2.c)
target_link_libraries(level_editor PRIVATE raydium OpenGL::GL m)

target_include_directories(level_editor PRIVATE ${PHP_INCLUDE_DIRS})

target_compile_definitions(level_editor
  PRIVATE
    FORCE_LIBRAYDIUM
    PHP_SUPPORT
)

set_target_properties(level_editor PROPERTIES
  OUTPUT_NAME level_editor
  RUNTIME_OUTPUT_NAME level_editor
)

# ─── Install ──────────────────────────────────────────────
include(GNUInstallDirs)
install(TARGETS raydium
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(TARGETS mania_drive level_editor
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
install(DIRECTORY raydium/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/raydium
  FILES_MATCHING PATTERN "*.h"
)
install(DIRECTORY rayphp/ DESTINATION ${CMAKE_INSTALL_DATADIR}/raydium/rayphp)
install(FILES LICENSE DESTINATION ${CMAKE_INSTALL_DOCDIR})

# ─── CPack ────────────────────────────────────────────────
set(CPACK_PACKAGE_NAME "ManiaDrive")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Open source racing game")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_GENERATOR "TGZ;DEB")
include(CPack)
