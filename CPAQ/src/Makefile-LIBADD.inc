#include $(top_srcdir)/PArray/src/Makefile-LIBADD.inc
include ../PArray/src/Makefile-LIBADD.inc

LIBCPAQ_LIBADD = -lparray $(LIBPARRAY_LIBADD)
