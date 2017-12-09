lib_LTLIBRARIES = libmedia.la
libmedia_la_SOURCES = media.cc
include_HEADERS = media.h
libmedia_la_LIBADD = -llffmpeg -liobus -lstdport -lstdexd
libmedia_la_LDFLAGS = -shared -fPIC -L../lffmpeg -L../iobus -L../stdport -L../stdexd -L../mock_sys
