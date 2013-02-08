# ===========================================================================
#
# SYNOPSIS
#
#   FLAME_BOOST_PROGRAM_OPTIONS
#
# NOTE
#
#   This macro has adapted from AX_BOOST_PROGRAM_OPTIONS.
#   See: http://www.gnu.org/software/autoconf-archive/ax_boost_program_options.html
#
# DESCRIPTION
#
#   Test for Program Options library from the Boost C++ libraries. The macro requires
#   a preceding call to FLAME_BOOST_BASE and FLAME_BOOST_LIB_SUFFIX
#
#   This macro calls:
#
#     AC_SUBST(BOOST_PROGRAM_OPTIONS_LIB)
#
#   And sets:
#
#     HAVE_BOOST_PROGRAM_OPTIONS
#
# LICENSE
#
#   Copyright (c) 2008 Thomas Porschberg <thomas@randspringer.de>
#   Copyright (c) 2008 Michael Tindal
#   Copyright (c) 2008 Daniel Casimiro <dan.casimiro@gmail.com>
#   Copyright (c) 2012 Shawn Chin
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.
# 
# $Id: flame_boost_program_options.m4 1163 2013-01-26 15:47:42Z lsc $

AC_DEFUN([FLAME_BOOST_PROGRAM_OPTIONS],
[

  want_boost="yes"  # We definitely want boost.
  base_lib="boost_program_options"
  
  if test "x$BOOST_LIB_SUFFIX" = "x"; then
    ax_boost_user_program_options_lib=""
  else
    ax_boost_user_program_options_lib="${base_lib}-${BOOST_LIB_SUFFIX}"
  fi
  
#  AC_ARG_WITH([boost-program_options],
#    [AS_HELP_STRING(
#      [--with-boost-program_options=BOOST_PROGRAM_OPTIONS_LIB],
#      [Choose specific boost program_options library for the linker, e.g. --with-boost-program_options=boost_program_options-gcc-mt]
#    )],
#    [ax_boost_user_program_options_lib="$withval"],
#    [ax_boost_user_program_options_lib=""]
#  )

# AC_ARG_WITH([boost-program_options],
# AS_HELP_STRING([--with-boost-program_options@<:@=special-lib@:>@],
#                   [use the Program Options library from boost - it is possible to specify a certain library for the linker
#                        e.g. --with-boost-program_options=boost_program_options-gcc-mt ]),
#        [
#        if test "$withval" = "no"; then
#   want_boost="no"
#        elif test "$withval" = "yes"; then
#            want_boost="yes"
#            ax_boost_user_program_options_lib=""
#        else
#      want_boost="yes"
#  ax_boost_user_program_options_lib="$withval"
#  fi
#        ],
#        [want_boost="yes"]
# )

  if test "x$want_boost" = "xyes"; then
    AC_REQUIRE([AC_PROG_CC])
    AC_REQUIRE([AC_CANONICAL_BUILD])
    CPPFLAGS_SAVED="$CPPFLAGS"
    CPPFLAGS="$CPPFLAGS $BOOST_CPPFLAGS"
    export CPPFLAGS

    LDFLAGS_SAVED="$LDFLAGS"
    LDFLAGS="$LDFLAGS $BOOST_LDFLAGS"
    export LDFLAGS

    AC_CACHE_CHECK(whether the Boost::Program_Options library is available,
      ax_cv_boost_program_options,
          AC_LANG_PUSH([C++])
          CXXFLAGS_SAVE=$CXXFLAGS

          AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[@%:@include <boost/program_options.hpp>]],
                            [[boost::program_options]])],
                            ax_cv_boost_program_options=yes, ax_cv_boost_program_options=no)
          CXXFLAGS=$CXXFLAGS_SAVE
          AC_LANG_POP([C++])
    ])
    
    if test "x$ax_cv_boost_program_options" = "xyes"; then
      AC_SUBST(BOOST_CPPFLAGS)

      AC_DEFINE(HAVE_BOOST_PROGRAM_OPTIONS,,[define if the Boost::Program_Options library is available])
      BOOSTLIBDIR=`echo $BOOST_LDFLAGS | sed -e 's/@<:@^\/@:>@*//'`

      LDFLAGS_SAVE=$LDFLAGS
      if test "x$ax_boost_user_program_options_lib" = "x"; then
        for libextension in `ls -r $BOOSTLIBDIR/libboost_program_options* 2>/dev/null | sed 's,.*/lib,,' | sed 's,\..*,,'` ; do
          ax_lib=${libextension}
          AC_CHECK_LIB($ax_lib, exit,
                      [BOOST_PROGRAM_OPTIONS_LIB="-l$ax_lib"; AC_SUBST(BOOST_PROGRAM_OPTIONS_LIB) link_program_options="yes"; break],
                      [link_program_options="no"])
        done
        if test "x$link_program_options" != "xyes"; then
          for libextension in `ls -r $BOOSTLIBDIR/boost_program_options* 2>/dev/null | sed 's,.*/,,' | sed -e 's,\..*,,'` ; do
            ax_lib=${libextension}
            AC_CHECK_LIB($ax_lib, exit,
                        [BOOST_PROGRAM_OPTIONS_LIB="-l$ax_lib"; AC_SUBST(BOOST_PROGRAM_OPTIONS_LIB) link_program_options="yes"; break],
                        [link_program_options="no"])
          done
        fi

      else
        for ax_lib in $ax_boost_user_program_options_lib boost_program_options-$ax_boost_user_program_options_lib; do
          AC_CHECK_LIB($ax_lib, exit,
                      [BOOST_PROGRAM_OPTIONS_LIB="-l$ax_lib"; AC_SUBST(BOOST_PROGRAM_OPTIONS_LIB) link_program_options="yes"; break],
                      [link_program_options="no"])
        done

      fi
      if test "x$ax_lib" = "x"; then
        AC_MSG_ERROR(Could not find a version of the library!)
      fi
      if test "x$link_program_options" = "xno"; then
        AC_MSG_ERROR(Could not link against $ax_lib !)
      fi
    fi

    CPPFLAGS="$CPPFLAGS_SAVED"
    LDFLAGS="$LDFLAGS_SAVED"
  fi
])