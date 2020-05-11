AM_CPPFLAGS = -D__STDC_INT64__

#if USE_SUBMODULES
#SUBDIRS = ../glitter
#AM_CPPFLAGS += -I $(top_srcdir)/glitter/src
#endif

include Makefile-LIBADD.inc

lib_LTLIBRARIES = libcpaq.la
include_HEADERS = cpaq.h
libcpaq_la_LIBADD = $(LIBCPAQ_LIBADD)

#include $(top_srcdir)/StD/src/Makefile-LIBADD.inc

include ../StD/src/Makefile-LIBADD.inc
bin_PROGRAMS = cpaq-test
cpaq_test_LDADD = libcpaq.la $(LIBCPAQ_LIBADD) -lsimon $(LIBSIMON_LIBADD)
