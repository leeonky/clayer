SUBDIRS = 

if !ENV_TEST
SUBDIRS += sysport
SUBDIRS += stdexd
SUBDIRS += stdport
SUBDIRS += decoder
SUBDIRS += lffmpeg
SUBDIRS += mem
endif

if !ENV_PRD
SUBDIRS += mock_ffmpeg
SUBDIRS += mock_sys
SUBDIRS += mock_std
SUBDIRS += lffmpeg_test
SUBDIRS += mem_test
endif

EXTRA_DIST = test.sh
