noinst_PROGRAMS = liblport_audio.test
liblport_audio_test_SOURCES = main.cc ../lport_audio/lport_audio.cc Pa_Init_and_OpenStream_test.cc
liblport_audio_test_LDADD = -lcunit -lcunitexd -lstdexd -lmock_port_audio
liblport_audio_test_LDFLAGS = -L../mock_port_audio -L../stdexd
