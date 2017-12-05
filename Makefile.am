SUBDIRS = 

if !ENV_TEST
SUBDIRS += sysport
SUBDIRS += sysexd
SUBDIRS += stdport
SUBDIRS += stdexd
SUBDIRS += lffmpeg
SUBDIRS += lsdl2
SUBDIRS += mem
SUBDIRS += iobus
SUBDIRS += media
SUBDIRS += decoder
SUBDIRS += screen
endif

if !ENV_PRD
SUBDIRS += mock_ffmpeg
SUBDIRS += mock_sdl2
SUBDIRS += mock_sys
SUBDIRS += mock_std
SUBDIRS += sysexd_test
SUBDIRS += stdexd_test
SUBDIRS += lffmpeg_test
SUBDIRS += mem_test
SUBDIRS += iobus_test
SUBDIRS += media_test
endif

EXTRA_DIST = test.sh
