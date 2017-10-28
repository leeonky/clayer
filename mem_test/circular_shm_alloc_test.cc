#include <stdio.h>
#include <stdlib.h>
#include <cunitexd.h>
#include <sys/shm.h>
#include "stdexd/stdexd.h"
#include "mock_sys/mock_sys.h"
#include "mock_std/mock_std.h"
#include "mem/circular_shm.h"

mock_function_1(int, circular_shm_alloc_action, circular_shm *);

static int circular_shm_alloc_action_ref(circular_shm &m) {
	return circular_shm_alloc_action(&m);
}

static size_t arg_size;
static int arg_count, ret_shmid;
static char ret_buffer[4096*10];
static sem_t ret_sem;

static char *stub_strerror(int e) {
	static char buffer[256];
	sprintf(buffer, "%d", e);
	return buffer;
}

SUITE_START("circular_shm_alloc_test");

BEFORE_EACH() {
	init_subject("");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;
	app_stderr = actxt.error_stream;

	ret_shmid = -1;
	arg_size = 1;
	arg_count = 1;

	init_mock_function(circular_shm_alloc_action);
	init_mock_function_with_return(shmget, ret_shmid);
	init_mock_function_with_return(shmat, ret_buffer);
	init_mock_function(shmdt);
	init_mock_function(shmctl);
	init_mock_function_with_return(sem_new_with_ppid, &ret_sem);
	init_mock_function(sem_close);
	init_mock_function(sem_unlink_with_ppid);
	init_mock_function_with_function(strerror, stub_strerror);
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUBJECT(int) {
	return circular_shm::alloc(arg_size, arg_count, circular_shm_alloc_action_ref);
}

SUITE_CASE("size alignment: size < pagesize") {
	arg_size = getpagesize()-1;
	arg_count = 2;

	CUE_ASSERT_SUBJECT_FAILED();

	CUE_EXPECT_CALLED_ONCE(shmget);
	CUE_EXPECT_CALLED_WITH_INT(shmget, 1, IPC_PRIVATE);
	CUE_EXPECT_CALLED_WITH_INT(shmget, 2, arg_count*getpagesize());
	CUE_EXPECT_CALLED_WITH_INT(shmget, 3, 0666 | IPC_CREAT);
}

SUITE_CASE("size alignment: size = pagesize") {
	arg_size = getpagesize();
	arg_count = 2;

	CUE_ASSERT_SUBJECT_FAILED();

	CUE_EXPECT_CALLED_ONCE(shmget);
	CUE_EXPECT_CALLED_WITH_INT(shmget, 1, IPC_PRIVATE);
	CUE_EXPECT_CALLED_WITH_INT(shmget, 2, arg_count*getpagesize());
	CUE_EXPECT_CALLED_WITH_INT(shmget, 3, 0666 | IPC_CREAT);
}

SUITE_CASE("size alignment: size > pagesize") {
	arg_size = getpagesize()*2-1;
	arg_count = 2;

	CUE_ASSERT_SUBJECT_FAILED();

	CUE_EXPECT_CALLED_ONCE(shmget);
	CUE_EXPECT_CALLED_WITH_INT(shmget, 1, IPC_PRIVATE);
	CUE_EXPECT_CALLED_WITH_INT(shmget, 2, arg_count*(getpagesize()*2));
	CUE_EXPECT_CALLED_WITH_INT(shmget, 3, 0666 | IPC_CREAT);
}

SUITE_CASE("init with all resources") {
	init_mock_function_with_return(shmget, ret_shmid=100);
	init_mock_function_with_return(circular_shm_alloc_action, 1000);

	CUE_ASSERT_SUBJECT_FAILED_WITH(1000);

	CUE_EXPECT_CALLED_ONCE(shmget);

	CUE_EXPECT_CALLED_ONCE(shmat);
	CUE_EXPECT_CALLED_WITH_INT(shmat, 1, ret_shmid);
	CUE_EXPECT_CALLED_WITH_PTR(shmat, 2, nullptr);
	CUE_EXPECT_CALLED_WITH_INT(shmat, 3, 0);

	CUE_EXPECT_CALLED_ONCE(sem_new_with_ppid);
	CUE_EXPECT_CALLED_WITH_INT(sem_new_with_ppid, 1, getpid());
	CUE_EXPECT_CALLED_WITH_INT(sem_new_with_ppid, 2, arg_count);

	CUE_EXPECT_CALLED_ONCE(circular_shm_alloc_action);

	CUE_EXPECT_CALLED_ONCE(sem_close);
	CUE_EXPECT_CALLED_WITH_PTR(sem_close, 1, &ret_sem);

	CUE_EXPECT_CALLED_ONCE(sem_unlink_with_ppid);
	CUE_EXPECT_CALLED_WITH_INT(sem_unlink_with_ppid, 1, getpid());

	CUE_EXPECT_CALLED_ONCE(shmdt);
	CUE_EXPECT_CALLED_WITH_PTR(shmdt, 1, ret_buffer);

	CUE_EXPECT_CALLED_ONCE(shmctl);
	CUE_EXPECT_CALLED_WITH_INT(shmctl, 1, ret_shmid);
	CUE_EXPECT_CALLED_WITH_INT(shmctl, 2, IPC_RMID);
	CUE_EXPECT_CALLED_WITH_PTR(shmctl, 3, nullptr);
}

int stub_shmget_failed(key_t, size_t, int) {
	errno = 100;
	return -1;
}

SUITE_CASE("shmget failed") {
	init_mock_function_with_function(shmget, stub_shmget_failed);

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_EXPECT_NEVER_CALLED(sem_new_with_ppid);

	CUE_EXPECT_NEVER_CALLED(shmat);

	CUE_EXPECT_NEVER_CALLED(circular_shm_alloc_action);

	CUE_EXPECT_NEVER_CALLED(shmdt);

	CUE_EXPECT_NEVER_CALLED(sem_close);

	CUE_EXPECT_NEVER_CALLED(sem_unlink_with_ppid);

	CUE_EXPECT_NEVER_CALLED(shmctl);

	CUE_ASSERT_STDERR_EQ("Error[shm_cbuf]: 100\n");
}

//shmget failed checking
//save count

SUITE_END(circular_shm_alloc_test);
