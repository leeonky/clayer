noinst_PROGRAMS = libmedia.test
libmedia_test_SOURCES = main.cc ../media/media.cc ../iobus/iobus.cc ../mem/circular_shm.cc ../media/sub_srt.cc ../media/sub_ass.cc ffmpeg_to_sdl_format.cc video_event_test.cc buffer_event_test.cc frames_event_test.cc media_clock_test.cc audio_event_test.cc samples_event_test.cc resampler_params_test.cc clock_event_test.cc sub_srt_test.cc frame_event_test.cc layer_event_test.cc sample_event_test.cc no_layer_event_test.cc control_event_test.cc create_player_context_test.cc ../sysexd/sysexd.cc rescaler_params_test.cc sub_ass_test.cc
libmedia_test_LDADD = -lcunit -lcunitexd -lstdexd -lmock_sys -lmock_ffmpeg -lmock_ass
libmedia_test_LDFLAGS = -L../stdexd -L../mock_sys -L../mock_ffmpeg -L../mock_ass
