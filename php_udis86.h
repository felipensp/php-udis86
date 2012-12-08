/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2012 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Felipe Pena <felipe@php.net>                                 |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_UDIS86_H
#define PHP_UDIS86_H

#include <udis86.h>

#define PHP_UDIS86_VERSION "0.1.0"

extern zend_module_entry udis86_module_entry;
#define phpext_udis86_ptr &udis86_module_entry

#ifdef ZTS
#include "TSRM.h"
#endif

typedef struct _php_udis86_obj {
	ud_t ud;
	php_stream *stream;
} php_udis86_obj;

#define PHP_UDIS86_ATT   1
#define PHP_UDIS86_INTEL 2
#define PHP_UDIS86_AMD   3

#ifdef ZTS
#define PHP_UDIS86_G(v) TSRMG(udis86_globals_id, zend_udis86_globals *, v)
#else
#define PHP_UDIS86_G(v) (udis86_globals.v)
#endif

#endif	/* PHP_UDIS86_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
