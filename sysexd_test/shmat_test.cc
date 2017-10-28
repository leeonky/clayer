#include <stdio.h>
#include <stdlib.h>
#include <cunitexd.h>
#include <sys/shm.h>
#include "mock_std/mock_std.h"
#include "stdexd/stdexd.h"
#include "mock_sys/mock_sys.h"
#include "sysexd/sysexd.h"

mock_function_1(int, shmat_action, void *);

static int arg_shmid;
static char ret_buffer[4096*10];

SUITE_START("shmat_test");

BEFORE_EACH() {
	init_subject("");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;
	app_stderr = actxt.error_stream;

	arg_shmid = 1;

	init_mock_function_with_return(shmat, ret_buffer);
	init_mock_function(shmdt);
	init_mock_function(shmat_action);
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUBJECT(int) {
	return shmat(arg_shmid, shmat_action);
}

static int shmat_action_assert(void *buffer) {
	CUE_ASSERT_PTR_EQ(buffer, ret_buffer);
	return 1000;
}

SUITE_CASE("shmat succeded") {
	init_mock_function_with_function(shmat_action, shmat_action_assert);

	CUE_ASSERT_SUBJECT_FAILED_WITH(1000);

	CUE_EXPECT_CALLED_ONCE(shmat);
	CUE_EXPECT_CALLED_WITH_INT(shmat, 1, arg_shmid);
	CUE_EXPECT_CALLED_WITH_PTR(shmat, 2, nullptr);
	CUE_EXPECT_CALLED_WITH_INT(shmat, 3, 0);

	CUE_EXPECT_CALLED_ONCE(shmdt);
	CUE_EXPECT_CALLED_WITH_PTR(shmdt, 1, ret_buffer);
}

void *stub_shmat_failed(int, const void *, int) {
	errno = 10;
	return (void *)-1;
}

SUITE_CASE("shmat failed") {
	init_mock_function_with_function(shmat, stub_shmat_failed);

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_EXPECT_NEVER_CALLED(shmdt);

	CUE_ASSERT_STDERR_EQ("Error[libsysexd]: 10\n");
}

SUITE_END(shmat_test);


