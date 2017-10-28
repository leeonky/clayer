#include <stdio.h>
#include <stdlib.h>
#include <cunitexd.h>
#include <sys/shm.h>
#include "mock_std/mock_std.h"
#include "stdexd/stdexd.h"
#include "mock_sys/mock_sys.h"
#include "sysexd/sysexd.h"

mock_function_1(int, sem_new_action, sem_t *);

static int arg_id, arg_count;
static sem_t ret_sem;

SUITE_START("sem_new_with_id_test");

BEFORE_EACH() {
	init_subject("");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;
	app_stderr = actxt.error_stream;

	arg_id = 1;
	arg_count = 10;

	init_mock_function_with_return(sem_new_with_id, &ret_sem);
	init_mock_function(sem_close);
	init_mock_function(sem_unlink_with_id);
	init_mock_function(sem_new_action);
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUBJECT(int) {
	return sem_new_with_id(arg_id, arg_count, sem_new_action);
}

static int sem_new_action_assert(sem_t *s) {
	CUE_ASSERT_PTR_EQ(s, &ret_sem);
	return 1000;
}

SUITE_CASE("sem_new_with_id succeded") {
	init_mock_function_with_function(sem_new_action, sem_new_action_assert);

	CUE_ASSERT_SUBJECT_FAILED_WITH(1000);

	CUE_EXPECT_CALLED_ONCE(sem_new_with_id);
	CUE_EXPECT_CALLED_WITH_INT(sem_new_with_id, 1, arg_id);
	CUE_EXPECT_CALLED_WITH_INT(sem_new_with_id, 2, arg_count);

	CUE_EXPECT_CALLED_ONCE(sem_new_action);

	CUE_EXPECT_CALLED_ONCE(sem_close);
	CUE_EXPECT_CALLED_WITH_PTR(sem_close, 1, &ret_sem);

	CUE_EXPECT_CALLED_ONCE(sem_unlink_with_id);
	CUE_EXPECT_CALLED_WITH_INT(sem_unlink_with_id, 1, arg_id);
}

static sem_t *stub_sem_new_with_id_failed(int, int) {
	errno = 100;
	return SEM_FAILED;
}

SUITE_CASE("failed to init semaphore") {
	init_mock_function_with_function(sem_new_with_id, stub_sem_new_with_id_failed);

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_EXPECT_NEVER_CALLED(sem_new_action);

	CUE_EXPECT_NEVER_CALLED(sem_close);

	CUE_EXPECT_NEVER_CALLED(sem_unlink_with_id);

	CUE_ASSERT_STDERR_EQ("Error[libsysexd]: 100\n");
}

SUITE_END(sem_new_with_id_test);

