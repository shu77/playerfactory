bin_PROGRAMS = factory_test options_test
factory_test_SOURCES = factory-test.cpp
factory_test_CPPFLAGS = -I$(top_srcdir)/include -I$(top_builddir)/src/.libs
factory_test_CXXFLAGS = 	\
		$(GLIB_CFLAGS)	\
		$(GIO_CFLAGS)	\
		$(GLIBMM_CFLAGS)\
		$(GST_CFLAGS)
factory_test_LDADD =		\
		-L$(top_builddir)/src/.libs/ -lpf	\
		$(GLIB_LIBS)	\
		$(GIO_LIBS)	\
		$(GLIBMM_LIBS)	\
		$(GST_LIBS)

options_test_SOURCES = options-test.cpp
options_test_CPPFLAGS = -I$(top_srcdir)/include -I$(top_builddir)/src/.libs
options_test_CXXFLAGS =         \
                $(GLIB_CFLAGS)  \
                $(GIO_CFLAGS)   \
                $(GLIBMM_CFLAGS)\
                $(GST_CFLAGS)
options_test_LDADD =            \
                -L$(top_builddir)/src/.libs/ -lpf       \
                $(GLIB_LIBS)    \
                $(GIO_LIBS)     \
                $(GLIBMM_LIBS)  \
                $(GST_LIBS)

