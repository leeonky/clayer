lib_LTLIBRARIES = libmedia.la
libmedia_la_SOURCES = media.cc sub_srt.cc sub_ass.cc
include_HEADERS = media.h sub_srt.h sub_ass.h
libmedia_la_LIBADD = -lavutil -llffmpeg -liobus -lstdport -lstdexd -llsdl2 -lportaudio -lmem -lass
libmedia_la_LDFLAGS = -shared -fPIC -L../lffmpeg -L../iobus -L../stdport -L../stdexd -L../lsdl2 -L../mem
