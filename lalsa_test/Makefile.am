noinst_PROGRAMS = liblalsa.test
liblalsa_test_SOURCES = main.cc ../lalsa/lalsa.cc snd_pcm_open_test.cc snd_pcm_mmap_writei_test.cc snd_pcm_delay_test.cc
liblalsa_test_LDADD = -lcunit -lcunitexd -lstdexd -lmock_alsa
liblalsa_test_LDFLAGS = -L../mock_alsa -L../stdexd
