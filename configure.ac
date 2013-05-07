dnl required version of autoconf
AC_PREREQ([2.62])

dnl TODO : fill in your package name and package version here
AC_INIT(player factory for webos, 1.0.0,
	https://github.com/justinjoy/playerfactory/issues,
	playerfactory)

AG_GST_INIT

AM_INIT_AUTOMAKE([-Wno-portability 1.11 no-dist-gzip dist-xz tar-ustar])

dnl required versions of gstreamer
GST_REQUIRED=1.0.0

#AC_CONFIG_SRCDIR([playerfactory/src/custompipeline.cpp])
AC_CONFIG_HEADERS([config.h])

dnl required version of automake
AM_INIT_AUTOMAKE([1.10])

dnl enable mainainer mode by default
AM_MAINTAINER_MODE([enable])

dnl check for tools (compiler etc.)
AC_PROG_CC
AC_PROG_CXX
AC_PROG_MAKE_SET
AC_PROG_RANLIB

dnl required version of libtool
LT_PREREQ([2.2.6])
LT_INIT

dnl give error and exit if we don't have pkconfig
AC_CHECK_PROG(HAVE_PKGCONGIF, pkg-config, [], [
	AC_MSG_ERROR([You need to have pkg-config installed!])
])

dnl check for glib module
PKG_CHECK_MODULES(GLIB, glib-2.0)
AC_SUBST(GLIB_CFLAGS)
AC_SUBST(GLIB_LIBS)

dnl check for gio module
PKG_CHECK_MODULES(GIO, gio-2.0 >= 2.16, HAVE_GIO=yes, HAVE_GIO=no)
AC_SUBST(GIO_CFLAGS)
AC_SUBST(GIO_LIBS)
AC_SUBST(GIO_LDFLAGS)

dnl check for glibmm module
PKG_CHECK_MODULES([GLIBMM], [glibmm-2.4 giomm-2.4])
AC_SUBST(GLIBMM_CFLAGS)
AC_SUBST(GLIBMM_LIBS)

dnl check for the required version of GStreamer core (and gst-plugins-base)
PKG_CHECK_MODULES(GST, [
	gstreamer-1.0 >= $GST_REQUIRED
], [
  AC_SUBST(GST_CFLAGS)
  AC_SUBST(GST_LIBS)
], [
  AC_MSG_ERROR([
      You need to install or upgrade the GStreamer development
      packages on your system. On debian-based systems these are
      libgstreamer1.0-dev and libgstreamer-plugins-base1.0-dev.
      on RPM-based systems gstreamer1.0-devel, libgstreamer1.0-devel
      or similar. The minimum version required is $GST_REQUIRED.
  ])
])

AC_CONFIG_FILES(
Makefile
src/Makefile
tests/Makefile
)
AC_OUTPUT

