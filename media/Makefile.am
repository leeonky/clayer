lib_LTLIBRARIES = libmedia.la
libmedia_la_SOURCES = media.cc sub_srt.cc
include_HEADERS = media.h sub_srt.h
libmedia_la_LIBADD = -lavutil -llffmpeg -liobus -lstdport -lstdexd -llsdl2 -lportaudio -lmem
libmedia_la_LDFLAGS = -shared -fPIC -L../lffmpeg -L../iobus -L../stdport -L../stdexd -L../lsdl2 -L../mem
