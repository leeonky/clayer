lib_LTLIBRARIES = liblmagic_wand.la
liblmagic_wand_la_SOURCES = lmagic_wand.cc
include_HEADERS = lmagic_wand.h
liblmagic_wand_la_CPPFLAGS = $(MAGICKWAND_CFLAGS)
liblmagic_wand_la_LIBADD = -lstdexd
liblmagic_wand_la_LDFLAGS = -shared -fPIC -L../stdexd $(MAGICKWAND_LIBS)
