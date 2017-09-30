LIBTHREV_LIBADD  = ../ThIpe/src/.libs/libthipe.la $(LIBTHIPE_LIBADD)
LIBTHREV_LIBADD += -lev
#LIBTHREV_LIBADD = -lthrio -lthipe -lrestart -ltscpaq -lcpaq -lev -lpthread

lib_LTLIBRARIES = libthrev.la
include_HEADERS = threv.h
libthrev_la_LIBADD = $(LIBTHREV_LIBADD)

bin_PROGRAMS = threv-test
threv_test_LDADD = libthrev.la $(LIBTHREV_LIBADD)

AM_CPPFLAGS = -D__STDC_INT64__

SUBDIRS = ../glitter
AM_CPPFLAGS += -I ../glitter/src

SUBDIRS += ../TSCPAQ
AM_CPPFLAGS += -I ../TSCPAQ/src

SUBDIRS += ../ThIpe
AM_CPPFLAGS += -I ../ThIpe/src
