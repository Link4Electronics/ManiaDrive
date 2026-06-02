#include <../main/php_config.h>
#include <stdlib.h>
#include <string.h>
#if defined(APACHE) && defined(PHP_API_VERSION)
#undef HAVE_DLFCN_H
#endif
#ifndef ZEND_API
# define ZEND_API
#endif
#ifndef uint
typedef unsigned int uint;
#endif

/* Fallback: configure on some platforms (e.g. powerpc64 big-endian)
 * fails to reach the AC_DEFINE for HAVE_BUNDLED_PCRE even though
 * bundled PCRE is selected. Ensure it's always defined. */
#ifndef HAVE_BUNDLED_PCRE
# define HAVE_BUNDLED_PCRE 1
#endif
