AM_CPPFLAGS = -D__STDC_INT64__

SUBDIRS = ../glitter
AM_CPPFLAGS += -I $(top_srcdir)/glitter/src

SUBDIRS += ../TSCPAQ
AM_CPPFLAGS += -I $(top_srcdir)/TSCPAQ/src

SUBDIRS += ../ThIpe
AM_CPPFLAGS += -I $(top_srcdir)/ThIpe/src

LIBTHREV_LIBADD  = ../ThIpe/src/.libs/libthipe.la $(LIBTHIPE_LIBADD)
LIBTHREV_LIBADD += -lev
#LIBTHREV_LIBADD = -lthrio -lthipe -lrestart -ltscpaq -lcpaq -lev -lpthread

lib_LTLIBRARIES = libthrev.la
include_HEADERS = threv.h
libthrev_la_LIBADD = $(LIBTHREV_LIBADD)

bin_PROGRAMS = threv-test
threv_test_LDADD = libthrev.la $(LIBTHREV_LIBADD)
