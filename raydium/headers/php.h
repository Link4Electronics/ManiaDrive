//!NOBINDINGS
#ifndef _PHP_H
#define _PHP_H

/*=
PHP scripting engine
3450
**/

// Introduction
/**
Most of this module is dedicated to RayPHP internal uses, where Raydium deals
with Zend engine.

The ##raydium_php_exec()## function is the interesting part of this module,
allowing you to call external PHP script in your application.

You may have a look the "Data Registration" chapter to know how "share"
data between your application and your PHP scripts.
**/

#include "../php_wrappers.h"
// use this macro when registering your functions
#define C2PHP ZEND_FN


// Dirty globals... (needed for WIN32 PHP support)
#ifdef ZTS
extern zend_compiler_globals *compiler_globals;
extern zend_executor_globals *executor_globals;
extern php_core_globals *core_globals;
extern sapi_globals_struct *sapi_globals;
extern void ***tsrm_ls;
#endif

__rayapi int raydium_php_exec (char *name);
/**
This function will call ##name## PHP script. All registered variables and
functions are exported to the script, and variable can be changed from the
script itself if needed.

This function will **not** use R3S to download a missing PHP script, for
obvious security reasons. But you may use PHP "allow_url_fopen" feature to
download things from your PHP scripts, if needed.

In PHP scripts, be sure to pre-alloc strings when calling Raydium functions
supposed to write to arguments. See this example:
%%(php)
$str=str_pad("",256); // "pre-alloc"
raydium_gui_read_name("main","track",$str);
echo "$str";
%%

**/

__rayapi void raydium_php_error (int type, const char *msg, ...);
__rayapi int raydium_php_uwrite (const char *str, uint str_length TSRMLS_DC);
__rayapi void raydium_php_init_request (char *filename);
__rayapi void raydium_php_close (void);
__rayapi void raydium_php_init (void);
#endif
