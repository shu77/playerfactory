lib_LTLIBRARIES = libpf.la

#INCLUDES = -I$(top_srcdir)/include

# sources used to compile this player factory
libpf_la_SOURCES = abstractplayer.cpp custompipeline.cpp customplayer.cpp genericpipeline.cpp genericplayer.cpp options.cpp pipeline.cpp playerfactory.cpp dlnapipeline.cpp dlnaplayer.cpp

# compiler and linker flags used to compile this plugin, set in configure.ac
libpf_la_CXXFLAGS =			\
		-I$(top_srcdir)/include	\
		$(GLIB_CFLAGS)		\
		$(GIO_CFLAGS)		\
		$(GLIBMM_CFLAGS)	\
		$(GST_CFLAGS)

#libpf_la_CPPFLAGS =

libpf_la_LIBADD =		\
		$(GLIB_LIBS)	\
		$(GIO_LIBS)	\
		$(GLIBMM_LIBS)	\
		$(GST_LIBS)

libpf_la_LDFLAGS = $(GIO_LDFLAGS)

libpf_la_LIBTOOLFLAGS = --tag=disable-static

# headers we need but don't want installed
#noinst_HEADERS =								\
#		$(top_srcdir)/include/pipeline/abstractplayer.hpp		\
#		$(top_srcdir)/include/pipeline/pf_export.hpp			\
#		$(top_srcdir)/include/pipeline/pipeline.hpp			\
#		$(top_srcdir)/include/pipeline/player.hpp			\
#		$(top_srcdir)/include/pipeline/playerfactory.hpp		\
#		$(top_srcdir)/include/pipeline/custom/custom_comm_types.hpp	\
#		$(top_srcdir)/include/pipeline/custom/custompipeline.hpp	\
#		$(top_srcdir)/include/pipeline/custom/customplayer.hpp		\
#		$(top_srcdir)/include/pipeline/event/playeventlistener.hpp	\
#		$(top_srcdir)/include/pipeline/generic/genericpipeline.hpp	\
#		$(top_srcdir)/include/pipeline/generic/genericplayer.hpp	\
#		$(top_srcdir)/include/pipeline/utils/options.hpp

