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
