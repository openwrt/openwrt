AC_PREREQ(2.57)
AC_INIT(Judy, 1.0.5, dougbaskins@yahoo.com)

AM_MAINTAINER_MODE

dnl Turn on automake, and pass it the PACKAGE_NAME and PACKAGE_VERSION, too.
AM_INIT_AUTOMAKE($PACKAGE_NAME, $PACKAGE_VERSION)

dnl Tell autoconf we want to keep our preprocessor defines in a header named
dnl config.h.  This keeps automake from passing a zillion -D directives to
dnl the C compiler.
AM_CONFIG_HEADER([config.h])

dnl==========================================================================
dnl WARNING - WARNING - Shared Library Versioning - WARNING - WARNING
dnl==========================================================================
dnl This is the most dangerous part of this file--making a mistake here can
dnl cause massively painful chaos for libJudy developers, and potentially
dnl even end users.  So PLEASE pay attention, and read up on the theory of
dnl shared library versioning.  Tens of thousands of Linux users (and several
dnl QA departments) may thank you someday.
dnl
dnl There are two major concerns:
dnl
dnl   1) When changing the libJudy ABI (application binary interface),
dnl      VERSION_INFO *must* be updated according to libtool's rules.  Failure
dnl      to do this will make applications using libJudy dump core, typically
dnl      under obscure conditions on user systems.  I won't attempt to
dnl      explain these rules here; please see 'info libtool' for details.
dnl
dnl   2) When changing the libJudy ABI, it is also desirable to make libJudy
dnl      "parallel installable".  This means that it should be possible to
dnl      install development headers and libraries for more than one version
dnl      of libJudy at once.  Failure to do this may cause problems for
dnl      Linux distributions which include libJudy.  (For example, it's
dnl      impossible to switch between libpng2-dev and libpng3-dev on a
dnl      Debian system without uninstalling and reinstalling both the Gnome
dnl      and KDE SDKs.)  For more information, do a Google search for
dnl      "parallel installable".
dnl
dnl Right now, this package only provides the mechanisms to handle concern
dnl (1).  Concern (2) is slightly more complicated, and will require some
dnl careful thinking.  Fortunately, concern (2) doesn't become important
dnl until other SDKs rely on the libJudy SDK.
dnl
dnl Of course, it's safe to avoid changing the libJudy ABI. :-)
dnl 
dnl The version scheme used by Libtool tracks interfaces, where an interface is
dnl the set of exported entry points into the library. All Libtool libraries
dnl start with -version-info set to 0:0:0 - this will be the default version
dnl number if you don't explicitly set it on the Libtool link command line. The
dnl meaning of these numbers (from left to right) is as follows:
dnl 
dnl  current:
dnl     The number of the current interface exported by the library. A current
dnl     value of 0, means that you are calling the interface exported by this
dnl     library interface 0.
dnl
dnl  revision:
dnl     The implementation number of the most recent interface exported by this
dnl     library. In this case, a revision value of 0 means that this is the
dnl     first implementation of the interface.
dnl 
dnl     If the next release of this library exports the same interface, but has
dnl     different implementation (perhaps some bugs have been fixed), the
dnl     revision number will be higher, but current number will be the same. In
dnl     that case, when given a choice, the library with the highest revision
dnl     will always be used by the runtime loader.
dnl
dnl   age:
dnl     The number of previous additional interfaces supported by this library.
dnl     If age were 2, then this library can be linked into executables which
dnl     were built with a release of this library that exported the current
dnl     interface number, current, or any of the previous two interfaces.
dnl
dnl By definition age must be less than or equal to current. At the outset, only
dnl the first ever interface is implemented, so age can only be 0.
dnl 

VERSION_INFO="-version-info 1:3:0"
AC_SUBST(VERSION_INFO)

dnl==========================================================================
dnl Flavors
dnl==========================================================================
dnl Judy can be compiled in one of three flavors: "product" (the default),
dnl "debug", or "cov".  We allow the user to select flavors using
dnl --enable-debug and --enable-ccover arguments to automake, which is
dnl the typical way of doing things.
dnl 
dnl Note how we perform string comparison:
dnl
dnl   if test "x$enable_debug" = xyes; then
dnl
dnl We do several odd things here:
dnl
dnl   1) We use 'test' instead of '[ ]' for shell portability.
dnl   2) We prefix strings with 'x' when comparing them, to protect against
dnl      empty strings.
dnl   3) We ALWAYS quote user-supplied shell variables, to protect against
dnl      embedded spaces.
dnl
dnl The results of this test aren't used anywhere yet.

dnl Keep the user entertained.
AC_MSG_CHECKING(which flavor to build)

dnl Process our --enable-debug argument.
AC_ARG_ENABLE(debug,
              AC_HELP_STRING([--enable-debug],
                             [enable debugging features]),
              , enable_debug=no)
if test "x$enable_debug" != xyes -a "x$enable_debug" != xno; then
   AC_MSG_ERROR(You may not pass an argument to --enable-debug)
fi

dnl Process our --enable-ccover argument.
AC_ARG_ENABLE(ccover,
              AC_HELP_STRING([--enable-ccover],
                             [enable use of ccover code coverage tools]),
              , enable_ccover=no)
if test "x$enable_ccover" != xyes -a "x$enable_ccover" != xno; then
   AC_MSG_ERROR(You may not pass an argument to --enable-ccover)
fi

dnl Determine our flavor.
if test "x$enable_debug" = xyes -a "x$enable_ccover" = xyes; then
   AC_MSG_ERROR(You may not use --enable-debug and --enable-ccover together)
elif test "x$enable_debug" = xyes; then
   FLAVOR=debug
elif test "x$enable_ccover" = xyes; then
   FLAVOR=cov
else
   FLAVOR=product
fi

dnl Define FLAVOR in our makefiles.
AC_SUBST(FLAVOR)

dnl Tell the user what flavor we've decided to build.
AC_MSG_RESULT($FLAVOR)


dnl==========================================================================
dnl Checks for Programs
dnl==========================================================================
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

dnl==========================================================================
dnl Checks for Header Files
dnl==========================================================================
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h inttypes.h limits.h malloc.h stddef.h stdint.h stdlib.h string.h strings.h sys/param.h sys/time.h unistd.h])

dnl==========================================================================
dnl Checks for Typedefs, Structures, and Compiler Characteristics
dnl==========================================================================

dnl Standard, boring stuff.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_STRUCT_TM
AC_C_VOLATILE
AC_CHECK_TYPES([ptrdiff_t])

dnl If we're compiling for a little-endian system, define JU_LITTLE_ENDIAN.
dnl If we can't tell what kind of system we're compling for, alert the
dnl user as described in 'info autoconf'.
AC_C_BIGENDIAN(, AC_DEFINE(JU_LITTLE_ENDIAN, 1,
			   [Define to 1 on little-endian systems.]))


b32="no"
b64="no"

AC_ARG_ENABLE(32-bit, [  --enable-32-bit          Generate code for a 32-bit environment],
	      b32="$enableval", b32="no")
if test "x$b32" != xno; then
    AC_MSG_RESULT(Configured to Build 32-bit)
    if test "x$GCC" = xyes; then
      CFLAGS="$CFLAGS -UJU_64BIT -m32"
    else
      CFLAGS="-UJU_64BIT"
    fi
fi

AC_ARG_ENABLE(64-bit, [  --enable-64-bit          Generate code for a 64-bit environment],
              b64="$enableval", b64="no")
if test "x$b64" != xno; then
    AC_MSG_RESULT(Configured to Building 64-bit)
    if test "x$GCC" = xyes; then
      CFLAGS="$CFLAGS -DJU_64BIT -m64"
    else
      CFLAGS="-DJU_64BIT"
    fi
fi

if test "x$b64" = xno -a "x$b32" = xno; then 
	    
	dnl  Figure out if we are 32-bit or 64-bit (LP64)
	AC_CHECK_SIZEOF(void *)
	if test "$ac_cv_sizeof_void_p" = 8; then 
	    AC_MSG_RESULT(Detected 64-bit Build Environment)
    	CFLAGS="$CFLAGS -DJU_64BIT"
	else 
    	AC_MSG_RESULT(Detected 32-bit Build Environment)
      	CFLAGS="$CFLAGS -UJU_64BIT"
	fi
fi

dnl==========================================================================
dnl Checks for Libraries
dnl==========================================================================

AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_FUNC_MEMCMP
AC_FUNC_MMAP
AC_FUNC_STAT
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([getpagesize gettimeofday memset munmap pow strchr strcspn strerror strstr strtoul uname])

dnl These must be called before AM_PROG_LIBTOOL, because it may want
dnl to call AC_CHECK_PROG.
AC_CHECK_TOOL(AR, ar)
AC_CHECK_TOOL(LD, ld)
AC_CHECK_TOOL(RANLIB, ranlib, :)

dnl Checks for libtool - this must be done after we set cflags (abi issues)
dnl
AM_PROG_LIBTOOL

WARN_CFLAGS=""
build_warnings="-W -Wall -Wstrict-prototypes -Wmissing-prototypes"
AC_ARG_ENABLE([build-warnings],
	      [  --enable-build-warnings    Enable build-time compiler warnings for gcc])
if test x"$build_warnings" = xyes; then
    if test x"$GCC" = xyes; then
	WARN_CFLAGS="${build_warnings}"
    fi
fi

AC_SUBST(WARN_CFLAGS)

AC_CONFIG_FILES([Makefile
                 src/Judy1/Makefile
                 src/JudyCommon/Makefile
                 src/JudyHS/Makefile
                 src/JudyL/Makefile
                 src/JudySL/Makefile
                 src/Makefile
                 src/obj/Makefile
		 tool/Makefile
		 doc/Makefile
		 test/Makefile])


AC_OUTPUT
