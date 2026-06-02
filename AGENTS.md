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
--with-zlib --with-curl --disable-phar
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

### Asset downloading
- PHP `--with-curl` is **required** so the game's PHP scripts (`rayphp/libfile.php`) can use `curl_init()`/`curl_exec()` to download assets from R3S repositories on first run
- The original R3S asset servers (`fastrepo.raydium.org`, `repository.raydium.org`) are still online as of 2026
- Without `--with-curl`, PHP's `curl_*` functions are undefined and asset downloads silently fail, resulting in a blank/black screen after the menu loads

### Why these flags
- `-fvisibility=default` — powers out hidden-symbol errors (`_emalloc`, `_efree`) on powerpc64 ELFv2 when DSO references static archive symbols
- `-DZEND_DLIMPORT=` — prevents angle-bracket `#include <../main/php_config.h>` from failing on powerpc64; ZEND_DLIMPORT needs an explicit empty definition
- `HAVE_BUNDLED_PCRE=1` — PHP configure says "bundled" but never writes the define to `php_config.h` on powerpc64; must be forced via EXTRA_CFLAGS + `target_compile_definitions`
- `PHP_EXT_DES_CRYPT=0`, `PHP_MD5_CRYPT=0`, `PHP_BLOWFISH_CRYPT=0` — configure doesn't define these on powerpc64; crypt functions are unused by the game so just disable
- `ac_cv_prog_RE2C=no` — cached autoconf var prevents configure from detecting re2c
- Pre-generated scanner `.c` files touched before `make` (via `cmake -E touch` in `CMakeLists.txt`) so Make's built-in `.l.c` pattern rule never fires. System `re2c` is too new for PHP 5.3's flex-style `.l` files and produces "unknown error processing section 1" / "bad character" failures on ppc64.

### Why shared library (not static) — both architectures
- `--enable-embed=shared` produces `libphp5.so` which resolves all internal Zend symbols within itself
- Static archive (`libphp5.a`) has circular dependencies between Zend objects that the GNU linker can't resolve even with `--whole-archive` on powerpc64 ELFv2
- On ELFv2, configure misdetects `finite()`/`isinf()`/`isnan()` etc., so function bodies for `zend_finite`, `zend_isinf`, `zend_isnan` are conditionally excluded — but callers still reference them, producing unresolved symbols
- `LDFLAGS=-Wl,--allow-shlib-undefined` is passed to PHP's `make` so `libphp5.so` links despite these; the symbols resolve at runtime from other objects within the same .so

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
- If `zend_isnan`/`zend_isinf`/`zend_finite`/`zend_sprintf` unresolved symbols appear: these functions are only defined on Windows/NetWare or conditionally guarded. On POSIX they must be provided externally. See `external/php-5.3.27/sapi/embed/php_embed.c` for the fix (added to the embedded SAPI source file since it's always compiled).

### Missing POSIX symbols in PHP 5.3.27
PHP 5.3.27 has several symbols that are only defined on Windows/NetWare (`zend_config.w32.h`, `zend_config.nw.h`) or guarded by configure-detected defines:
- `zend_isnan` — only `#define`d on Windows (`_isnan`) and NetWare. On POSIX, `isnan()` is a C99 macro, not a function. The symbol is referenced from `spprintf.c`, `snprintf.c`, `math.c`, `formatted_print.c`, `json.c`, etc.
- `zend_isinf` — same pattern, references from the same files.
- `zend_finite` — same pattern, also used in `zend_operators.c`, `logical_filters.c`.
- `zend_sprintf` — guarded by `#if ZEND_BROKEN_SPRINTF` in `zend_sprintf.c`. Configure always sets `ZEND_BROKEN_SPRINTF=0` on modern glibc, so the function body is excluded. Referenced from `zend_exceptions.c`, `zend_compile.c`, `mysqlnd_debug.c`.

**Fix**: `sapi/embed/php_embed.c` provides implementations for all four using standard C library functions (`isnan`, `isinf`, `finite`, `vsprintf`). This file is guaranteed compiled in the embedded SAPI build. Since `php_embed.c` is part of the PHP source tree, this could be ported to a patch file in future.

### `language_scanner_globals` missing on powerpc64

On powerpc64 ELFv2, `language_scanner_globals` (defined in `Zend/zend_language_scanner.c` under `#else` of `#ifdef ZTS`) may not be exported from `libphp5.so`. The root cause is unclear — possibly visibility quirks, or the object file being excluded from the link.

**Fix**: `sapi/embed/php_embed.c` provides a weak (`__attribute__((weak))`) definition of `zend_php_scanner_globals language_scanner_globals`. On platforms where `zend_language_scanner.c` defines it strongly (x86_64), the weak symbol is ignored. On powerpc64, the weak definition fills the gap. Added alongside the existing POSIX-symbol stubs.

### `compile_file` / `compile_string` / `compile_filename` missing on powerpc64

These symbols are also defined in `zend_language_scanner.c`. On ppc64 ELFv2 with GCC 14, `-fvisibility=default` (passed via `EXTRA_CFLAGS`) may not take effect, causing all symbols from this file to be compiled with `-fvisibility=hidden` (from `CFLAGS_CLEAN`).

**Fixes**:
1. `CMakeLists.txt` patches `Zend/zend_config.h` after configure to change `#define ZEND_API` to `#define ZEND_API __attribute__((visibility("default")))`. This attaches an explicit default-visibility attribute to every `ZEND_API`-marked symbol regardless of the active `-fvisibility` flag.
2. `Zend/zend_language_scanner.c` — the definitions of `compile_filename` and `compile_string` were missing the `ZEND_API` qualifier that their declarations in `zend_compile.h` carried. Added it so the visibility attribute from fix 1 also applies to them.

### `ini_scanner_globals` missing on powerpc64

Same pattern as `language_scanner_globals` — defined in `Zend/zend_ini_scanner.c` but not exported from `libphp5.so` on ppc64 ELFv2.

**Fix**: `sapi/embed/php_embed.c` provides a weak `__attribute__((weak))` definition of `zend_ini_scanner_globals ini_scanner_globals`, right after the existing `language_scanner_globals` fallback.

## ODE (Open Dynamics Engine)

### Precision
- The game requires **single precision** (`dReal == float`, 4 bytes). It checks at startup with `sizeof(dReal) != sizeof(float)` and exits if ODE was built with double precision.
- System ODE packages on ppc64 are often built with double precision (`dDOUBLE`).
- **Fix**: `find_package(ODE QUIET)` was removed — the bundled ODE in `external/ODE/` is always used. Single precision is forced via `set(ODE_DOUBLE_PRECISION OFF CACHE BOOL "" FORCE)`.

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
