noinst_PROGRAMS = liblmagic_wand.test
liblmagic_wand_test_SOURCES = main.cc ../lmagic_wand/lmagic_wand.cc create_magic_wand_test.cc create_drawing_wand_test.cc drawing_test.cc
liblmagic_wand_test_CPPFLAGS = $(MAGICKWAND_CFLAGS)
liblmagic_wand_test_LDADD = -lcunit -lcunitexd -lstdexd -lmock_magic_wand
liblmagic_wand_test_LDFLAGS = -L../mock_magic_wand -L../stdexd
