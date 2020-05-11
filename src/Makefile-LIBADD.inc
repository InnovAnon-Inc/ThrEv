#include $(top_srcdir)/ThIpe/src/Makefile-LIBADD.inc
include ../ThIpe/src/Makefile-LIBADD.inc

#if USE_SUBMODULES
#LIBTHREV_LIBADD  = ../ThIpe/src/.libs/libthipe.la $(LIBTHIPE_LIBADD)
#LIBTHREV_LIBADD += ../libev/.libs/libev.la
#else
LIBTHREV_LIBADD  = -lthipe $(LIBTHIPE_LIBADD)
LIBTHREV_LIBADD += -lev
#endif
