AM_CPPFLAGS = -D__STDC_INT64__

if USE_SUBMODULES
SUBDIRS = ../glitter
AM_CPPFLAGS += -I $(top_srcdir)/glitter/src

SUBDIRS += ../TSCPAQ
AM_CPPFLAGS += -I $(top_srcdir)/TSCPAQ/src

SUBDIRS += ../ThIpe
AM_CPPFLAGS += -I $(top_srcdir)/ThIpe/src

SUBDIRS += ../libev
AM_CPPFLAGS += -I $(top_srcdir)/libev
endif

include Makefile-LIBADD.inc

lib_LTLIBRARIES = libthrev.la
include_HEADERS = threv.h
libthrev_la_LIBADD = $(LIBTHREV_LIBADD)

bin_PROGRAMS = threv-test
threv_test_LDADD = libthrev.la $(LIBTHREV_LIBADD)
