#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include <sys/shm.h>
#include "mock_std/mock_std.h"
#include "stdexd/stdexd.h"
#include "mock_sys/mock_sys.h"
#include "sysexd/sysexd.h"

mock_function_1(int, shmget_action, int);

static size_t arg_size;
static int ret_shmid;

SUITE_START("shmget_test");

BEFORE_EACH() {
	init_subject("");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;
	app_stderr = actxt.error_stream;

	arg_size = 1;
	ret_shmid = 100;

	init_mock_function(shmget_action);
	init_mock_function_with_return(shmget, ret_shmid);
	init_mock_function(shmctl);
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUBJECT(int) {
	return shmget(arg_size, shmget_action);
}

static int shmget_action_assert(int id) {
	CUE_ASSERT_EQ(id, ret_shmid);
	return 1000;
}

SUITE_CASE("shmget succeded") {
	init_mock_function_with_return(shmget, ret_shmid=100);
	init_mock_function_with_function(shmget_action, shmget_action_assert);

	CUE_ASSERT_SUBJECT_FAILED_WITH(1000);

	CUE_EXPECT_CALLED_ONCE(shmget);
	CUE_EXPECT_CALLED_WITH_INT(shmget, 1, IPC_PRIVATE);
	CUE_EXPECT_CALLED_WITH_INT(shmget, 2, arg_size);
	CUE_EXPECT_CALLED_WITH_INT(shmget, 3, 0666 | IPC_CREAT);

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

	CUE_EXPECT_NEVER_CALLED(shmget_action);

	CUE_EXPECT_NEVER_CALLED(shmctl);

	CUE_ASSERT_STDERR_EQ("Error[libsysexd]: 100\n");
}

SUITE_END(shmget_test);

