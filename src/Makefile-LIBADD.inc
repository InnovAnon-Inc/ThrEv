include $(top_srcdir)/ThIpe/src/Makefile-LIBADD.inc
LIBTHREV_LIBADD  = ../ThIpe/src/.libs/libthipe.la $(LIBTHIPE_LIBADD)
#LIBTHREV_LIBADD += -lev
LIBTHREV_LIBADD += ../libev/.libs/libev.la
#LIBTHREV_LIBADD = -lthrio -lthipe -lrestart -ltscpaq -lcpaq -lev -lpthread
