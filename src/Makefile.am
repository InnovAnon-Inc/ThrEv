AM_CPPFLAGS = -D__STDC_INT64__

#include Makefile-LIBADD.inc
dist_bin_SCRIPTS = libthrev_config threv_test_config

lib_LTLIBRARIES =
bin_PROGRAMS    =

lib_LTLIBRARIES += libthrev.la
include_HEADERS = threv.h
libthrev_la_LIBADD = @LIBTHREV_LIBADD@

bin_PROGRAMS += threv-test
#threv_test_LDADD = libthrev.la $(LIBTHREV_LIBADD)
threv_test_LDADD = @THREV_TEST_LIBADD@
EXTRA_threv_test_DEPENDENCIES = libthrev.la
