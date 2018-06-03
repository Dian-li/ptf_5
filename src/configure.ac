#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(ptf, 1.0.0, chiron_me@126.com)
AM_INIT_AUTOMAKE(ptf,1.0.0)
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
AC_CHECK_HEADERS([arpa/inet.h])
AC_CHECK_HEADERS([float.h])
AC_CHECK_HEADERS([netinet/in.h])
AC_CHECK_HEADERS([stddef.h])
AC_CHECK_HEADERS([sys/socket.h])
AC_CHECK_HEADERS([sys/time.h])
AC_CHECK_HEADER_STDBOOL

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_TYPES([ptrdiff_t])
AC_C_INLINE
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_TYPE_INT64_T
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T

# Checks for library functions.
AC_CHECK_FUNCS([gettimeofday])
AC_CHECK_FUNCS([inet_ntoa])
AC_CHECK_FUNCS([localeconv])
AC_CHECK_FUNCS([memset])
AC_CHECK_FUNCS([modf])
AC_CHECK_FUNCS([socket])
AC_CHECK_FUNCS([strcasecmp])
AC_CHECK_FUNCS([strchr])
AC_CHECK_FUNCS([strerror])

AC_OUTPUT(Makefile)
