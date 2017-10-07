noinst_PROGRAMS = liblffmpeg.test
liblffmpeg_test_SOURCES = main.cc ../lffmpeg/lffmpeg.cc avformat_open_close_test.cc avformat_find_stream_test.cc avstream_info_test.cc avcodec_open_test.cc
liblffmpeg_test_LDADD = -lcunit -lcunitexd -lstdexd -lmock_ffmpeg
liblffmpeg_test_LDFLAGS = -L../mock_ffmpeg -L../stdexd
