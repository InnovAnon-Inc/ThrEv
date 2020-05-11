AM_CPPFLAGS = -D__STDC_INT64__

#if USE_SUBMODULES
#SUBDIRS = ../glitter
#AM_CPPFLAGS += -I $(top_srcdir)/glitter/src
#
#SUBDIRS += ../restart
#AM_CPPFLAGS += -I $(top_srcdir)/restart/src
#
#SUBDIRS += ../TSCPAQ
#AM_CPPFLAGS += -I $(top_srcdir)/TSCPAQ/src
#endif

lib_LTLIBRARIES =
include_HEADERS =
bin_PROGRAMS    =

include Makefile-LIBADD.inc
#LIBBUFFER_LIBADD =

lib_LTLIBRARIES += libbuffer.la
include_HEADERS += buffer.h
libbuffer_la_LIBADD = $(LIBBUFFER_LIBADD)

#LIBPIPE_LIBADD  = ../restart/src/.libs/librestart.la $(LIBRESTART_LIBADD)
#LIBPIPE_LIBADD += ../TSCPAQ/src/.libs/libtscpaq.la $(LIBTSCPAQ_LIBADD)
#LIBPIPE_LIBADD += libbuffer.la $(LIBBUFFER_LIBADD)

lib_LTLIBRARIES += libpipe.la
include_HEADERS += pipe.h
libpipe_la_LIBADD = $(LIBPIPE_LIBADD)

#LIBIO_LIBADD =
#LIBIO_LIBADD += ../TSCPAQ/src/.libs/libtscpaq.la $(LIBTSCPAQ_LIBADD)
#LIBIO_LIBADD += libbuffer.la $(LIBBUFFER_LIBADD)
#LIBIO_LIBADD += libpipe.la $(LIBPIPE_LIBADD)

lib_LTLIBRARIES += libio.la
include_HEADERS += io.h
libio_la_LIBADD = $(LIBIO_LIBADD)

#LIBTHIPE_LIBADD  = libio.la $(LIBIO_LIBADD)
##LIBTHIPE_LIBADD += libpipe.la $(LIBPIPE_LIBADD)
##LIBTHIPE_LIBADD += libbuffer.la $(LIBBUFFER_LIBADD)

lib_LTLIBRARIES += libthipe.la
libthipe_la_SOURCES =
libthipe_la_LIBADD = $(LIBTHIPE_LIBADD)

#bin_PROGRAMS += thipe-test
#thipe_test_LDADD = libthipe.la $(LIBTHIPE_LIBADD)
