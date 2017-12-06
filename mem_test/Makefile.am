noinst_PROGRAMS = libmem.test
libmem_test_SOURCES = main.cc ../sysexd/sysexd.cc ../mem/circular_shm.cc circular_shm_create_test.cc circular_shm_load_test.cc
libmem_test_LDADD = -lcunit -lcunitexd -lmock_sys -lstdexd
libmem_test_LDFLAGS = -L../mock_sys -L../stdexd
