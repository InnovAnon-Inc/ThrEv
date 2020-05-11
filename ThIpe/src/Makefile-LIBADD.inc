#include $(top_srcdir)/restart/src/Makefile-LIBADD.inc
#include $(top_srcdir)/TSCPAQ/src/Makefile-LIBADD.inc
include ../restart/src/Makefile-LIBADD.inc
include ../TSCPAQ/src/Makefile-LIBADD.inc

LIBBUFFER_LIBADD =
LIBIO_LIBADD     =
LIBPIPE_LIBADD   =
LIBTHIPE_LIBADD  =

#if USE_SUBMODULES
#LIBPIPE_LIBADD += ../restart/src/.libs/librestart.la $(LIBRESTART_LIBADD)
#LIBPIPE_LIBADD += ../TSCPAQ/src/.libs/libtscpaq.la $(LIBTSCPAQ_LIBADD)
#
##LIBIO_LIBADD += ../TSCPAQ/src/.libs/libtscpaq.la $(LIBTSCPAQ_LIBADD)
#else
LIBPIPE_LIBADD += -lrestart $(LIBRESTART_LIBADD)
LIBPIPE_LIBADD += -ltscpaq $(LIBTSCPAQ_LIBADD)
#endif

# TODO
#LIBPIPE_LIBADD += libbuffer.la $(LIBBUFFER_LIBADD)
LIBPIPE_LIBADD += -lbuffer $(LIBBUFFER_LIBADD)

#LIBIO_LIBADD += libbuffer.la $(LIBBUFFER_LIBADD)
#LIBIO_LIBADD += libpipe.la $(LIBPIPE_LIBADD)
LIBIO_LIBADD += -lbuffer $(LIBBUFFER_LIBADD)
LIBIO_LIBADD += -lpipe $(LIBPIPE_LIBADD)

#LIBTHIPE_LIBADD += libio.la $(LIBIO_LIBADD)
##LIBTHIPE_LIBADD += libpipe.la $(LIBPIPE_LIBADD)
##LIBTHIPE_LIBADD += libbuffer.la $(LIBBUFFER_LIBADD)
LIBTHIPE_LIBADD += -lio $(LIBIO_LIBADD)
