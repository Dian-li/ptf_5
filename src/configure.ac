#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(ptf, 1.0.0, chiron_me@126.com)
AM_INIT_AUTOMAKE(ptf, 1.0.0)
AM_INIT_AUTOMAKE([subdir-objects])
AC_CONFIG_SRCDIR([main.C])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX

# Checks for libraries.
AC_CHECK_LIB([config], [config_init], [], [exit])
AC_CHECK_LIB([event], [event_base_new], [], [exit])
AC_CHECK_LIB([xml2], [xmlNewDoc], [], [exit])

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h])
AC_CHECK_HEADER_STDBOOL

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_OUTPUT(Makefile)
