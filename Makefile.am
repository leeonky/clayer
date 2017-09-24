SUBDIRS = 

if !ENV_TEST
SUBDIRS += stdexd
SUBDIRS += decoder
SUBDIRS += ffmpegpp
endif

if !ENV_PRD
SUBDIRS += mock_ffmpeg
SUBDIRS += ffmpegpp_test
endif

EXTRA_DIST = test.sh
