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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_udis86.h"
#include <udis86.h>

/* True global resources - no need for thread safety here */
static int le_udis86;

/* {{{ php_udis86_functions[]
 *
 * Every user visible function must have an entry in php_udis86_functions[].
 */
const zend_function_entry udis86_functions[] = {
	PHP_FE(udis86_init,	NULL)
	PHP_FE(udis86_input_file, NULL)
	PHP_FE(udis86_disassemble, NULL)
	PHP_FE(udis86_insn_asm, NULL)
	PHP_FE(udis86_insn_len, NULL)
	PHP_FE(udis86_insn_hex, NULL)
	PHP_FE_END
};
/* }}} */

/* {{{ php_udis86_module_entry
 */
zend_module_entry udis86_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"udis86",
	udis86_functions,
	PHP_MINIT(udis86),
	PHP_MSHUTDOWN(udis86),
	NULL,
	NULL,
	PHP_MINFO(udis86),
#if ZEND_MODULE_API_NO >= 20010901
	PHP_UDIS86_VERSION,
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_UDIS86
ZEND_GET_MODULE(udis86)
#endif

static void udis86_resource_destructor(zend_rsrc_list_entry *rsrc TSRMLS_DC) /* {{{ */
{
	if (rsrc->ptr) {
		efree(rsrc->ptr);
		rsrc->ptr = NULL;
	}
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(udis86)
{	
	le_udis86 = zend_register_list_destructors_ex(
		udis86_resource_destructor, NULL, "udis86", module_number);	
	
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(udis86)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(udis86)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "udis86 support", "enabled");
	php_info_print_table_end();
}
/* }}} */

/* {{{ proto resource udis86_init(void)
   Return a resource of an initialized udis86 data */
PHP_FUNCTION(udis86_init)
{
	ud_t *ud_obj;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	
	ud_obj = emalloc(sizeof(ud_t));
	memset(ud_obj, 0, sizeof(ud_t));
	
	ud_init(ud_obj);
	ud_set_mode(ud_obj, 64);
	ud_set_syntax(ud_obj, UD_SYN_ATT);
	
	ZEND_REGISTER_RESOURCE(return_value, ud_obj, le_udis86);
}
/* }}} */

PHP_FUNCTION(udis86_input_file) 
{
	ud_t *ud_obj;
	zval *ud;
	char *fname;
	int fname_len;
	FILE *fp;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs",
		&ud, &fname, &fname_len) == FAILURE) {
		return;
	}
	
	ZEND_FETCH_RESOURCE(ud_obj, ud_t*, &ud, -1, "udis86", le_udis86);
	
	fp = fopen(fname, "rb");
	
	ud_set_input_file(ud_obj, fp);
}


PHP_FUNCTION(udis86_disassemble)
{
	ud_t *ud_obj;
	zval *ud;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r",
		&ud) == FAILURE) {
		return;
	}
	
	ZEND_FETCH_RESOURCE(ud_obj, ud_t*, &ud, -1, "udis86", le_udis86);
	
	RETURN_LONG(ud_disassemble(ud_obj));
}


PHP_FUNCTION(udis86_insn_asm)
{
	ud_t *ud_obj;
	zval *ud;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r",
		&ud) == FAILURE) {
		return;
	}
	
	ZEND_FETCH_RESOURCE(ud_obj, ud_t*, &ud, -1, "udis86", le_udis86);
	
	RETURN_STRING(ud_insn_asm(ud_obj), 1);
}

PHP_FUNCTION(udis86_insn_len)
{
	ud_t *ud_obj;
	zval *ud;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r",
		&ud) == FAILURE) {
		return;
	}
	
	ZEND_FETCH_RESOURCE(ud_obj, ud_t*, &ud, -1, "udis86", le_udis86);
	
	RETURN_LONG(ud_insn_len(ud_obj));
}

PHP_FUNCTION(udis86_insn_hex)
{
	ud_t *ud_obj;
	zval *ud;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r",
		&ud) == FAILURE) {
		return;
	}
	
	ZEND_FETCH_RESOURCE(ud_obj, ud_t*, &ud, -1, "udis86", le_udis86);
	
	RETURN_STRING(ud_insn_hex(ud_obj), 1);
}
	

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
