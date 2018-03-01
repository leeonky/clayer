#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include <sys/shm.h>
#include "stdexd/stdexd.h"
#include "mock_sys/mock_sys.h"
#include "mock_std/mock_std.h"
#include "mem/pool_shm.h"

mock_function_1(int, pool_shm_load_action, pool_shm *);

static int pool_shm_load_action_ref(pool_shm &m) {
	return pool_shm_load_action(&m);
}

static int arg_shmid;
static char ret_buffer[4096*10];

SUITE_START("pool_shm_load_test");

BEFORE_EACH() {
	init_subject("");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;
	app_stderr = actxt.error_stream;

	arg_shmid = 1;

	init_mock_function(pool_shm_load_action);
	init_mock_function_with_return(shmat, ret_buffer);
	init_mock_function(shmdt);
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUBJECT(int) {
	return pool_shm::load(arg_shmid, pool_shm_load_action_ref);
}

static int assert_pool_shm(pool_shm *shm) {
	CUE_ASSERT_STRING_EQ(shm->serialize_to_string(), "POOL id:1");
	return 0;
}

SUITE_CASE("init with all resources") {
	init_mock_function_with_function(pool_shm_load_action, assert_pool_shm);

	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(shmat);
	CUE_EXPECT_CALLED_WITH_INT(shmat, 1, arg_shmid);
	CUE_EXPECT_CALLED_WITH_PTR(shmat, 2, nullptr);
	CUE_EXPECT_CALLED_WITH_INT(shmat, 3, 0);

	CUE_EXPECT_CALLED_ONCE(pool_shm_load_action);

	CUE_EXPECT_CALLED_ONCE(shmdt);
	CUE_EXPECT_CALLED_WITH_PTR(shmdt, 1, ret_buffer);
}

SUITE_END(pool_shm_load_test);
