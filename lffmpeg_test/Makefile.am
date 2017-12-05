noinst_PROGRAMS = liblffmpeg.test
liblffmpeg_test_SOURCES = main.cc ../lffmpeg/lffmpeg.cc avformat_open_close_test.cc avformat_find_stream_test.cc avstream_info_test.cc avcodec_open_test.cc av_new_packet_test.cc av_new_frame_test.cc av_read_and_send_to_avcodec_test.cc avcodec_receive_frame_video_test.cc av_get_buffer_size_test.cc av_copy_frame_to_buffer_test.cc avcodec_receive_frame_audio_test.cc av_frame_pts_test.cc av_image_fill_arrays_test.cc
liblffmpeg_test_LDADD = -lcunit -lcunitexd -lstdexd -lmock_ffmpeg
liblffmpeg_test_LDFLAGS = -L../mock_ffmpeg -L../stdexd
