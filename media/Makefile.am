lib_LTLIBRARIES = libmedia.la
libmedia_la_SOURCES = media.cc
include_HEADERS = media.h
libmedia_la_LIBADD = -lavutil -llffmpeg -liobus -lstdport -lstdexd -llsdl2 -lportaudio
libmedia_la_LDFLAGS = -shared -fPIC -L../lffmpeg -L../iobus -L../stdport -L../stdexd -L../lsdl2
