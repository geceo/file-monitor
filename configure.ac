AC_PREREQ([2.68])
AC_INIT([fmond], [0.1], [fmond@yanluo.net])
AM_INIT_AUTOMAKE()

AC_CONFIG_SRCDIR([src/])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O

# Checks for libraries.
AC_CHECK_FUNCS([memset strdup strerror])
PKG_CHECK_MODULES([glib], [glib-2.0])
PKG_CHECK_MODULES([ssh], [libssh])

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h unistd.h ssh.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MALLOC


AC_CONFIG_HEADER([include/config.h])
AC_CONFIG_FILES([Makefile src/Makefile])

AC_OUTPUT()
