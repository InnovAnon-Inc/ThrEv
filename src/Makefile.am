lib_LTLIBRARIES = libthrev.la
libthrev_la_SOURCES = threv.c
include_HEADERS = threv.h
libthrev_la_LIBADD = -lthrio -lthipe -lrestart -ltscpaq -lcpaq -lev -lpthread

bin_PROGRAMS = threv
threv_SOURCES = main.c
threv_LDADD = libthrev.la -lthrio -lthipe -lrestart -ltscpaq -lcpaq -lev -lpthread

AM_CPPFLAGS = -D__STDC_INT64__
AM_CPPFLAGS += -I $(srcdir)/../glitter/src
AM_CPPFLAGS += -I $(srcdir)/../TSCPAQ/src
AM_CPPFLAGS += -I $(srcdir)/../ThIpe/src
