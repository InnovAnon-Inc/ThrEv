#lib_LTLIBRARIES = libthrev-client.la libthrev-server.la
#libthrev_client_la_SOURCES = threv-client.c threv.h
#libthrev_server_la_SOURCES = threv-server.c threv.h
#libthrev_client_la_LDFLAGS = -lezudp-client -lrestart
#libthrev_server_la_LDFLAGS = -lezudp-server -lrestart
#include_HEADERS	= threv-client.h threv-server.h

#libthrev_client_la_CFLAGS = -Wextra -pedantic -ansi
#libthrev_server_la_CFLAGS = -Wextra -pedantic -ansi

bin_PROGRAMS = threv-client threv-server
threv_client_SOURCES = threv-client.c threv.h
threv_server_SOURCES = threv-server.c threv.h
threv_client_LDFLAGS = -lev -lm -lezudp-client -lrestart -ltscpaq -lcpaq -lthpool -lpthread
threv_server_LDFLAGS = -lev -lm -lrestart -lthipe -ltscpaq -lcpaq -lpthread

#threv_client_CFLAGS = -Wextra -pedantic -ansi
#threv_server_CFLAGS = -Wextra -pedantic -ansi
threv_client_CFLAGS = -Wextra -pedantic
threv_server_CFLAGS = -Wextra -pedantic

AM_CPPFLAGS = -D__STDC_INT64__
