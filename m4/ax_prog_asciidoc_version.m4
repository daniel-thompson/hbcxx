# ===========================================================================
# Derived from:
#   http://www.gnu.org/software/autoconf-archive/ax_prog_perl_version.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_PROG_ASCIIDOC_VERSION([VERSION],[ACTION-IF-TRUE],[ACTION-IF-FALSE])
#
# DESCRIPTION
#
#   Makes sure that asciidoc supports the version indicated. If true the shell
#   commands in ACTION-IF-TRUE are executed. If not the shell commands in
#   ACTION-IF-FALSE are run. Note if $ASCIIDOC is not set (for example by
#   running AC_CHECK_PROG or AC_PATH_PROG) the macro will fail.
#
#   Example:
#
#     AC_PATH_PROG([ASCIIDOC],[asciidoc])
#     AX_PROG_ASCIIDOC_VERSION([8.6.0],[ ... ],[ ... ])
#
#   This will check to make sure that the asciidoc you have supports at least
#   version 8.6.0.
#
#   NOTE: This macro uses the $ASCIIDOC variable to perform the check.
#   The $ASCIIDOC_VERSION variable will be valorized with the detected
#   version.
#
# LICENSE
#
#   Copyright (c) 2014 Daniel Thompson <daniel@redfelineninja.org.uk>
#   Copyright (c) 2009 Francesco Salvestrini <salvestrini@users.sourceforge.net>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 12

AC_DEFUN([AX_PROG_ASCIIDOC_VERSION],[
    AC_REQUIRE([AC_PROG_SED])
    AC_REQUIRE([AC_PROG_GREP])

    AS_IF([test -n "$ASCIIDOC"],[
        ax_asciidoc_version="$1"

        AC_MSG_CHECKING([for asciidoc version])
        changequote(<<,>>)
        asciidoc_version=`$ASCIIDOC --version 2>&1 \
          | $SED -n -e '/asciidoc [0-9]/b inspect
b
: inspect
s/asciidoc \([0-9]*\.[0-9]*\.[0-9]*\)$/\1/;p'`
        changequote([,])
        AC_MSG_RESULT($asciidoc_version)

	AC_SUBST([ASCIIDOC_VERSION],[$asciidoc_version])

        AX_COMPARE_VERSION([$ax_asciidoc_version],[le],[$asciidoc_version],[
	    :
            $2
        ],[
	    :
            $3
        ])
    ],[
        AC_MSG_WARN([could not find the asciidoc compiler])
        $3
    ])
])
