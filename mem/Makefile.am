lib_LTLIBRARIES = libmem.la
libmem_la_SOURCES = circular_shm.cc pool_shm.cc
include_HEADERS = circular_shm.h pool_shm.h
libmem_la_LIBADD = -lsysport -lsysexd -lstdexd
libmem_la_LDFLAGS = -shared -fPIC -L../sysport -L../sysexd -L../stdexd
