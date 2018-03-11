#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([blayer++], [1.0.0], [leeonky@gmail.com])
AC_CONFIG_SRCDIR([decoder])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([foreign])

AC_ARG_VAR(ENV, TEST; DEV)

# Checks for programs.
AC_PROG_CXX
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL

AC_CHECK_LIB([cunit], [CU_initialize_registry])

# Checks for header files.
AC_CHECK_HEADERS([cstdlib])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AM_CONDITIONAL([ENV_TEST], [test "x$ENV" = xTEST])
AM_CONDITIONAL([ENV_PRD], [test "x$ENV" != xDEV && test "x$ENV" != xTEST])

AC_SUBST(AM_CXXFLAGS, "-std=c++11 -fno-exceptions -fno-rtti -Wall -Wextra")
#AC_SUBST(AM_CXXFLAGS, "-std=c++11 -Wall -Wextra")

MAGICKWAND_CFLAGS=`pkg-config --cflags MagickWand`
MAGICKWAND_LIBS=`pkg-config --libs MagickWand`

AC_SUBST(MAGICKWAND_CFLAGS)
AC_SUBST(MAGICKWAND_LIBS)

AC_CONFIG_FILES([Makefile])

# Production Makefile
AM_COND_IF([ENV_TEST], [],
	  [AC_CONFIG_FILES([
	   sysport/Makefile
	   sysexd/Makefile
	   stdexd/Makefile
	   stdport/Makefile
	   lffmpeg/Makefile
	   decoder/Makefile
	   mem/Makefile
	   iobus/Makefile
	   lsdl2/Makefile
	   lport_audio/Makefile
	   media/Makefile
	   screen/Makefile
	   speaker/Makefile
	   resampler/Makefile
	   lmagic_wand/Makefile
	   subtitle/Makefile
	   ])])

# Testing Makefile
AM_COND_IF([ENV_PRD], [],
	  [AC_CONFIG_FILES([
	   mock_sys/Makefile
	   mock_std/Makefile
	   mock_sdl2/Makefile
	   mock_ffmpeg/Makefile
	   mock_port_audio/Makefile
	   mock_magic_wand/Makefile
	   stdport_test/Makefile
	   sysexd_test/Makefile
	   stdexd_test/Makefile
	   lffmpeg_test/Makefile
	   mem_test/Makefile
	   iobus_test/Makefile
	   lsdl2_test/Makefile
	   lport_audio_test/Makefile
	   media_test/Makefile
	   lmagic_wand_test/Makefile
	   ])])

AC_OUTPUT
