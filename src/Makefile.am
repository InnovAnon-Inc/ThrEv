AM_CPPFLAGS = -D__STDC_INT64__

lib_LTLIBRARIES =
bin_PROGRAMS    =

lib_LTLIBRARIES += libthrev.la
include_HEADERS = threv.h
libthrev_la_CPPFLAGS = $(GLITTER_CFLAGS) $(IO_CFLAGS)
#libthrev_la_LIBADD   = $(GLITTER_LIBS)   $(IO_LIBS) -lpthread
libthrev_la_LIBADD   = $(GLITTER_LIBS)   $(IO_LIBS)

bin_PROGRAMS += threv-test
threv_test_CPPFLAGS =         $(GLITTER_CFLAGS) $(IO_CFLAGS)
#threv_test_LDADD    = -lthrev $(GLITTER_LIBS)   $(IO_LIBS) -lpthread
threv_test_LDADD    = -lthrev $(GLITTER_LIBS)   $(IO_LIBS)
EXTRA_threv_test_DEPENDENCIES = libthrev.la

