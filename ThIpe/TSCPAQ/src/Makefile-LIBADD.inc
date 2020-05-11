#include $(top_srcdir)/CPAQ/src/Makefile-LIBADD.inc
include ../CPAQ/src/Makefile-LIBADD.inc

#if USE_SUBMODULES
#LIBTSCPAQ_LIBADD  = ../CPAQ/src/.libs/libcpaq.la $(LIBCPAQ_LIBADD)
#else
LIBTSCPAQ_LIBADD  = -lcpaq $(LIBCPAQ_LIBADD)
#endif
LIBTSCPAQ_LIBADD += -lpthread
