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

#ifdef COMPILE_DL_UDIS86
ZEND_GET_MODULE(udis86)
#endif

static void udis86_resource_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC) /* {{{ */
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
	REGISTER_LONG_CONSTANT("UDIS86_ATT",   PHP_UDIS86_ATT,   CONST_CS|CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("UDIS86_INTEL", PHP_UDIS86_INTEL, CONST_CS|CONST_PERSISTENT);
	
	le_udis86 = zend_register_list_destructors_ex(
		udis86_resource_dtor, NULL, "udis86", module_number);	
	
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
static PHP_FUNCTION(udis86_init)
{
	ud_t *ud_obj;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	
	ud_obj = emalloc(sizeof(ud_t));
	
	ud_init(ud_obj);
	ud_set_syntax(ud_obj, UD_SYN_ATT);
	ud_set_input_buffer(ud_obj, "", 0);
	
	ZEND_REGISTER_RESOURCE(return_value, ud_obj, le_udis86);
}
/* }}} */

/* {{{ proto void udis86_set_mode(int mode)
   Set the mode of disassembly */
static PHP_FUNCTION(udis86_set_mode) 
{
	ud_t *ud_obj;
	zval *ud;
	long mode;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl",
		&ud, &mode) == FAILURE) {
		return;
	}
	
	ZEND_FETCH_RESOURCE(ud_obj, ud_t*, &ud, -1, "udis86", le_udis86);
	
	switch (mode) {
		case 16:
		case 32:
		case 64:
			ud_set_mode(ud_obj, mode);
			break;
		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "invalid mode");
	}
}
/* }}} */

/* {{{ proto bool udis86_input_file(resource obj, string file)
   Set the file as internal buffer, return false if any error ocurred,
   otherwise true */
static PHP_FUNCTION(udis86_input_file) 
{
	ud_t *ud_obj;
	zval *ud;
	char *fname;
	int fname_len;
	FILE *fp;
	char resolved_path[MAXPATHLEN];
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs",
		&ud, &fname, &fname_len) == FAILURE) {
		return;
	}
	
	ZEND_FETCH_RESOURCE(ud_obj, ud_t*, &ud, -1, "udis86", le_udis86);
	
#if PHP_API_VERSION < 20100412
	if ((PG(safe_mode) && (!php_checkuid(fname, NULL, CHECKUID_CHECK_FILE_AND_DIR))) || php_check_open_basedir(fname TSRMLS_CC)) {
#else
	if (php_check_open_basedir(fname TSRMLS_CC)) {
#endif
		RETURN_FALSE;
	}
	if (!expand_filepath_with_mode(fname, resolved_path, NULL, 0, CWD_EXPAND TSRMLS_CC)) {
		RETURN_FALSE;
	}
		
	if ((fp = fopen(resolved_path, "rb")) == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "unable to open file");
		RETURN_FALSE;
	}
	
	ud_set_input_file(ud_obj, fp);
	
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int udis86_disassemble(void)
   Disassemble the internal buffer and return the number of bytes
   disassembled */
static PHP_FUNCTION(udis86_disassemble)
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
/* }}} */

/* {{{ proto string udis86_insn_asm(resource obj)
   Return the string representation of disassembled instruction */
static PHP_FUNCTION(udis86_insn_asm)
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
/* }}} */

/* {{{ proto int udis86_insn_len(resource obj)
   Return the number of bytes of the disassembled instruction */
static PHP_FUNCTION(udis86_insn_len)
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
/* }}} */

/* {{{ proto string udis86_insn_hex(resource obj)
   Return the hexadecimal representation of the disassembled instruction */
static PHP_FUNCTION(udis86_insn_hex)
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
/* }}} */

/* {{{ proto int udis86_insn_off(resource obj)
   Return the offset of the disassembled instruction relative to program
   counter value specified internally */
static PHP_FUNCTION(udis86_insn_off)
{
	ud_t *ud_obj;
	zval *ud;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r",
		&ud) == FAILURE) {
		return;
	}
	
	ZEND_FETCH_RESOURCE(ud_obj, ud_t*, &ud, -1, "udis86", le_udis86);
	
	RETURN_LONG(ud_insn_off(ud_obj));
}
/* }}} */

/* {{{ proto void udis86_input_skip(resource obj, int n)
   Skips N bytes in the input stream */
static PHP_FUNCTION(udis86_input_skip)
{
	ud_t *ud_obj;
	zval *ud;
	long n;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl",
		&ud, &n) == FAILURE) {
		return;
	}
	
	ZEND_FETCH_RESOURCE(ud_obj, ud_t*, &ud, -1, "udis86", le_udis86);
	
	ud_input_skip(ud_obj, n);
}
/* }}} */

/* {{{ proto void udis86_set_pc(resource obj, int pc)
   Set the program counter */
static PHP_FUNCTION(udis86_set_pc)
{
	ud_t *ud_obj;
	zval *ud;
	long pc;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl",
		&ud, &pc) == FAILURE) {
		return;
	}
	
	ZEND_FETCH_RESOURCE(ud_obj, ud_t*, &ud, -1, "udis86", le_udis86);
	
	ud_set_pc(ud_obj, pc);
}
/* }}} */

/* {{{ proto void udis86_set_syntax(resource obj, int syntax)
   Set the syntax to be used in the disassembly representation */
static PHP_FUNCTION(udis86_set_syntax)
{
	ud_t *ud_obj;
	zval *ud;
	long syntax;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl",
		&ud, &syntax) == FAILURE) {
		return;
	}
	
	ZEND_FETCH_RESOURCE(ud_obj, ud_t*, &ud, -1, "udis86", le_udis86);
	
	switch (syntax) {
		case PHP_UDIS86_ATT:
			ud_set_syntax(ud_obj, UD_SYN_ATT);
			break;
		case PHP_UDIS86_INTEL:
			ud_set_syntax(ud_obj, UD_SYN_INTEL);
			break;
		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "invalid syntax");
	}
}
/* }}} */

/* {{{ arginfo 
 */
ZEND_BEGIN_ARG_INFO_EX(arginfo_udis86_void, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_udis86_obj_only, 0, 0, 1)
	ZEND_ARG_INFO(0, obj)
ZEND_END_ARG_INFO()	

ZEND_BEGIN_ARG_INFO_EX(arginfo_udis86_input_file, 0, 0, 2)
	ZEND_ARG_INFO(0, obj)
	ZEND_ARG_INFO(0, file)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_udis86_input_skip, 0, 0, 2)
	ZEND_ARG_INFO(0, obj)
	ZEND_ARG_INFO(0, mode)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_udis86_set_mode, 0, 0, 2)
	ZEND_ARG_INFO(0, obj)
	ZEND_ARG_INFO(0, mode)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ udis86_functions[]
 */
const zend_function_entry udis86_functions[] = {
	PHP_FE(udis86_init,        arginfo_udis86_void)
	PHP_FE(udis86_input_file,  arginfo_udis86_input_file)
	PHP_FE(udis86_disassemble, arginfo_udis86_obj_only)
	PHP_FE(udis86_insn_asm,    arginfo_udis86_obj_only)
	PHP_FE(udis86_insn_len,    arginfo_udis86_obj_only)
	PHP_FE(udis86_insn_hex,    arginfo_udis86_obj_only)
	PHP_FE(udis86_insn_off,    arginfo_udis86_obj_only)
	PHP_FE(udis86_input_skip,  arginfo_udis86_input_skip)
	PHP_FE(udis86_set_mode,    arginfo_udis86_set_mode)
	PHP_FE(udis86_set_pc,      arginfo_udis86_set_mode)
	PHP_FE(udis86_set_syntax,  arginfo_udis86_set_mode)
	PHP_FE_END
};
/* }}} */

/* {{{ udis86_module_entry
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

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
