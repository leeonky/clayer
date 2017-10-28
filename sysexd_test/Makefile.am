noinst_PROGRAMS = libsysexd.test
libsysexd_test_SOURCES = main.cc ../sysexd/sysexd.cc shmget_test.cc shmat_test.cc sem_new_with_id_test.cc
libsysexd_test_LDADD = -lcunit -lcunitexd -lmock_sys -lstdexd
libsysexd_test_LDFLAGS = -L../mock_sys -L../stdexd
