# ManiaDrive Porting Notes

## PHP 5.3 on powerpc64 (big-endian ELFv2)

### Build setup
- PHP is built as a **shared library** (`--enable-embed=shared`) at `${CMAKE_CURRENT_BINARY_DIR}/php-build/libs/libphp5.so`
- Separate build dir (`php-build/`) to avoid source-tree contamination
- `add_custom_command(OUTPUT ...)` with `${PHP_BUILD_DIR}/config.status` stamp ensures configure only runs once
- `php5` is a `SHARED IMPORTED` CMake target

### Critical PHP configure flags
```
--enable-embed=shared --with-pic
--without-pear --disable-cgi --disable-cli
--disable-simplexml --disable-xmlreader --disable-xmlwriter --disable-dom
--with-zlib --disable-phar
--without-gd --without-jpeg-dir --without-png-dir
```

### Critical EXTRA_CFLAGS for make
```
EXTRA_CFLAGS="\
  -fvisibility=default \
  -DHAVE_BUNDLED_PCRE=1 \
  -DPHP_EXT_DES_CRYPT=0 \
  -DPHP_MD5_CRYPT=0 \
  -DPHP_BLOWFISH_CRYPT=0 \
  -DZEND_DLIMPORT="
```

### Why these flags
- `-fvisibility=default` — powers out hidden-symbol errors (`_emalloc`, `_efree`) on powerpc64 ELFv2 when DSO references static archive symbols
- `-DZEND_DLIMPORT=` — prevents angle-bracket `#include <../main/php_config.h>` from failing on powerpc64; ZEND_DLIMPORT needs an explicit empty definition
- `HAVE_BUNDLED_PCRE=1` — PHP configure says "bundled" but never writes the define to `php_config.h` on powerpc64; must be forced via EXTRA_CFLAGS + `target_compile_definitions`
- `PHP_EXT_DES_CRYPT=0`, `PHP_MD5_CRYPT=0`, `PHP_BLOWFISH_CRYPT=0` — configure doesn't define these on powerpc64; crypt functions are unused by the game so just disable
- `ac_cv_prog_RE2C=no` — cached autoconf var prevents configure from running re2c (system re2c too new, would clobber pre-generated scanner .c files). Using `RE2C=/bin/true` would silently produce empty scanner output.

### Why shared library (not static)
- `--enable-embed=shared` produces `libphp5.so` which resolves all internal Zend symbols within itself
- Static archive (`libphp5.a`) has circular dependencies between Zend objects that the GNU linker can't resolve without `--whole-archive` or `--start-group`/`--end-group`
- CMake consistently separates `-Wl,` flags from library paths, making `--whole-archive` wrapping unreliable
- Shared lib avoids both the circular-dep and hidden-symbol issues in one go

### Linking
- `target_link_libraries(raydium PRIVATE php5)` — libraydium.so has DT_NEEDED on libphp5.so
- Executables also link `php5` directly (`target_link_libraries(exec PRIVATE raydium ... php5 ...)`) because the game source code calls `php_sprintf` directly and the local linker (`--no-copy-dt-needed-entries`) doesn't follow transitive DT_NEEDED from raydium when building executables
- RPATH set to `"${CMAKE_CURRENT_BINARY_DIR};${PHP_BUILD_DIR}/libs"` on both executables

### RPATH note
- At runtime, the dynamic linker needs to find `libphp5.so` in `php-build/libs/` and `libraydium.so` in the build root
- `BUILD_RPATH` is set on both executables; for installed builds, adjust `INSTALL_RPATH`

### Build order / reconfigure avoidance
- PHP configure only reruns when `${PHP_BUILD_DIR}/config.status` is deleted
- PHP make only reruns when `${PHP_LIBRARY}` is missing or older than config.status
- After a clean first build, subsequent `cmake --build .` invocations skip PHP entirely

### Common pitfalls
- If `yyleng` multiple-definition error appears: two Flex scanners (`zend_language_scanner`, `zend_ini_scanner`) both define this global. Only happens with `--whole-archive`. With shared lib, doesn't occur.
- If `xmlStrncmp` / libxml2 error appears: `libphp5.a`'s `libxml.o` needs `-lxml2`. Not an issue with shared build.
- If `gzdopen` / zlib error appears: `zlib_fopen_wrapper.o` needs `-lz`. Not an issue with shared build.
- `config.h`'s `#ifndef NO_PHP_SUPPORT` block (in raydium headers) is dead code — PHP is always required, the option was removed.

## Building

```sh
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DKIDS_MODE=ON
cmake --build .
```

## Running

PHP scripts at repo root (`anim.php`, `mania_localtracks.php`, etc.) must be in `~/.mania_drive/data/`:

```sh
mkdir -p ~/.mania_drive/data
cp *.php ~/.mania_drive/data/
# or symlink: ln -s "$PWD" ~/.mania_drive/data/
```
