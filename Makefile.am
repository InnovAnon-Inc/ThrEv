# Install the pkg-config file.
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = src/libthrev.pc src/threv-test.pc

DISTCLEANFILES = $(pkgconfig_DATA)
EXTRA_DIST = src/libthrev.pc.in src/threv-test.pc.in

SUBDIRS = src
dist_doc_DATA = README.md
ACLOCAL_AMFLAGS = -I m4

