dnl $Id$
dnl config.m4 for extension udis86

PHP_ARG_WITH(udis86, for udis86 support,
 [  --with-udis86             Include udis86 support])

if test "$PHP_PHP_UDIS86" != "no"; then
  SEARCH_PATH="/usr/local /usr"
  SEARCH_FOR="/include/udis86.h"
  if test -r $PHP_PHP_UDIS86/$SEARCH_FOR; then
     PHP_UDIS86_DIR=$PHP_PHP_UDIS86
  else
    AC_MSG_CHECKING([for udis86 files in default path])
    for i in $SEARCH_PATH ; do
      if test -r $i/$SEARCH_FOR; then
        PHP_UDIS86_DIR=$i
        AC_MSG_RESULT(found in $i)
      fi
     done
  fi
  
  if test -z "$PHP_UDIS86_DIR"; then
    AC_MSG_RESULT([not found])
  fi

  PHP_ADD_INCLUDE($PHP_UDIS86_DIR/include)

  LIBNAME=udis86
  LIBSYMBOL=ud_init

  PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  [
    PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $PHP_UDIS86_DIR/lib, UDIS86_SHARED_LIBADD)
    AC_DEFINE(HAVE_PHP_LIBUDIS86,1,[ ])
  ],[
    AC_MSG_ERROR([wrong libudis86 version or lib not found])
  ],[
    -L$PHP_UDIS86_DIR/lib -lm
  ])
  
  PHP_SUBST(UDIS86_SHARED_LIBADD)

  PHP_NEW_EXTENSION(udis86, php_udis86.c, $ext_shared)
fi
