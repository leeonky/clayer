SUBDIRS = 

if !ENV_TEST
SUBDIRS += sysport
SUBDIRS += stdport
SUBDIRS += sysexd
SUBDIRS += stdexd
SUBDIRS += mem
SUBDIRS += lffmpeg
SUBDIRS += lsdl2
SUBDIRS += lport_audio
SUBDIRS += iobus
SUBDIRS += media
SUBDIRS += decoder
SUBDIRS += screen
SUBDIRS += speaker
SUBDIRS += resampler
endif

if !ENV_PRD
SUBDIRS += mock_sys
SUBDIRS += mock_std
SUBDIRS += mock_sdl2
SUBDIRS += mock_port_audio
SUBDIRS += mock_ffmpeg
SUBDIRS += stdport_test
SUBDIRS += sysexd_test
SUBDIRS += stdexd_test
SUBDIRS += mem_test
SUBDIRS += lffmpeg_test
SUBDIRS += lsdl2_test
SUBDIRS += lport_audio_test
SUBDIRS += iobus_test
SUBDIRS += media_test
endif

EXTRA_DIST = test.sh
