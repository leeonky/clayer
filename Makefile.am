SUBDIRS = 

if !ENV_TEST
SUBDIRS += stdexd
SUBDIRS += stdport
SUBDIRS += decoder
SUBDIRS += lffmpeg
endif

if !ENV_PRD
SUBDIRS += mock_ffmpeg
SUBDIRS += lffmpeg_test
SUBDIRS += stdport_test
endif

EXTRA_DIST = test.sh
