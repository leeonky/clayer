noinst_PROGRAMS = liblffmpeg.test
liblffmpeg_test_SOURCES = main.cc ../lffmpeg/lffmpeg.cc avformat_open_close_test.cc
liblffmpeg_test_LDADD = -lcunit -lcunitexd -lstdexd -lmock_ffmpeg
liblffmpeg_test_LDFLAGS = -L../mock_ffmpeg -L../stdexd
