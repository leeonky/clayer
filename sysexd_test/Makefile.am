noinst_PROGRAMS = libsysexd.test
libsysexd_test_SOURCES = main.cc ../sysexd/sysexd.cc shmget_test.cc shmat_test.cc sem_new_with_id_test.cc
libsysexd_test_LDADD = -lcunit -lcunitexd -lstdexd -lmock_sys -lmock_std
libsysexd_test_LDFLAGS = -L../stdexd -L../mock_sys -L../mock_std
