dnl Process this file with autoconf to produce a configure script.

dnl There's little point in allowing support for older versions of autoconf.
dnl Autoconf 2.69 is what ships in Fedora 18 and Debian Wheezy (both of which
dnl are too old to have sufficient C++11 support to run hbcxx).
AC_PREREQ(2.69)
AC_INIT(hbcxx, 0.8)

AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE([1.12 silent-rules foreign subdir-objects -Wall])
AM_SILENT_RULES([yes])

dnl These are prerequisites for the AX_PROG_ASCIIDOC_VERSION (and makes the
dnl output of ./configure more readable)
AC_PROG_SED
AC_PROG_GREP

AC_PROG_CC_C99
AS_IF([test "x$ac_cv_prog_cc_c99" = "xno"], [AC_MSG_ERROR([compiler does not support C99])])
AM_PROG_CC_C_O
AC_PROG_CXX
AC_PROG_CXX_C_O
AX_CXX_COMPILE_STDCXX_11(, [mandatory])
AM_PROG_AR
AC_PROG_RANLIB

AX_CFLAGS_WARN_ALL
AX_APPEND_COMPILE_FLAGS([-fsanitize=address])
AX_APPEND_COMPILE_FLAGS([-fno-omit-frame-pointer])

AC_LANG([C++])
AX_CXXFLAGS_WARN_ALL
AX_APPEND_COMPILE_FLAGS([-Weffc++])
AX_APPEND_COMPILE_FLAGS([-fsanitize=address])
AX_APPEND_COMPILE_FLAGS([-fno-omit-frame-pointer])

AC_CHECK_HEADER([boost/filesystem.hpp], [], [
  AC_MSG_ERROR([unable to find the boost filesystem header])
])
AX_APPEND_LINK_FLAGS([-lboost_filesystem])
AC_CHECK_HEADER([boost/regex.hpp], [], [
  AC_MSG_ERROR([unable to find the boost regex header])
])
AX_APPEND_LINK_FLAGS([-lboost_regex])
dnl Needed by boost filesystem
AX_APPEND_LINK_FLAGS([-lboost_system])

dnl Keep this near the bottom - adding -Werror breaks various compiler 
dnl based tests
AX_APPEND_COMPILE_FLAGS([-Werror])

AC_PATH_PROG([ASCIIDOC],[asciidoc])
AX_PROG_ASCIIDOC_VERSION([8.6.0],[have_asciidoc=true],[AC_MSG_WARN([asciidoc is missing or too old to regenerate README.html. Do not worry too much about this since it only matters if you modify README.asciidoc.])])
AM_CONDITIONAL([HAVE_ASCIIDOC], test x"$have_asciidoc" = x"true")

AC_CONFIG_FILES(Makefile)
AC_OUTPUT
