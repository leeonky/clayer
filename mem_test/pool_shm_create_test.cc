#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include <sys/shm.h>
#include "stdexd/stdexd.h"
#include "mock_sys/mock_sys.h"
#include "mock_std/mock_std.h"
#include "mem/pool_shm.h"

mock_function_1(int, pool_shm_create_action, pool_shm *);

static int pool_shm_create_action_ref(pool_shm &m) {
	return pool_shm_create_action(&m);
}

static size_t arg_size;
static int ret_shmid;
static char ret_buffer[4096*10];

SUITE_START("pool_shm_create_test");

BEFORE_EACH() {
	init_subject("");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;
	app_stderr = actxt.error_stream;

	arg_size = 1024;
	ret_shmid = 1;

	init_mock_function(pool_shm_create_action);
	init_mock_function_with_return(shmget, ret_shmid);
	init_mock_function_with_return(shmat, ret_buffer);
	init_mock_function(shmdt);
	init_mock_function(shmctl);
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUBJECT(int) {
	return pool_shm::create(arg_size, pool_shm_create_action_ref);
}

SUITE_CASE("init with all resources") {
	init_mock_function_with_return(shmget, ret_shmid=100);
	init_mock_function_with_return(pool_shm_create_action, 1000);

	CUE_ASSERT_SUBJECT_FAILED_WITH(1000);

	CUE_EXPECT_CALLED_ONCE(shmget);
	CUE_EXPECT_CALLED_WITH_INT(shmget, 1, IPC_PRIVATE);
	CUE_EXPECT_CALLED_WITH_INT(shmget, 2, arg_size);
	CUE_EXPECT_CALLED_WITH_INT(shmget, 3, 0666 | IPC_CREAT);

	CUE_EXPECT_CALLED_ONCE(shmat);
	CUE_EXPECT_CALLED_WITH_INT(shmat, 1, ret_shmid);
	CUE_EXPECT_CALLED_WITH_PTR(shmat, 2, nullptr);
	CUE_EXPECT_CALLED_WITH_INT(shmat, 3, 0);

	CUE_EXPECT_CALLED_ONCE(pool_shm_create_action);

	CUE_EXPECT_CALLED_ONCE(shmdt);
	CUE_EXPECT_CALLED_WITH_PTR(shmdt, 1, ret_buffer);

	CUE_EXPECT_CALLED_ONCE(shmctl);
	CUE_EXPECT_CALLED_WITH_INT(shmctl, 1, ret_shmid);
	CUE_EXPECT_CALLED_WITH_INT(shmctl, 2, IPC_RMID);
	CUE_EXPECT_CALLED_WITH_PTR(shmctl, 3, nullptr);
}

static int pool_shm_create_action_assert_serialize(pool_shm *cshm) {
	char buffer[1024];
	sprintf(buffer, "POOL id:%d", ret_shmid);
	CUE_ASSERT_STRING_EQ(cshm->serialize_to_string(), buffer);
	return 0;
}

SUITE_CASE("pool_shm serialize to string") {
	arg_size = 1024;
	init_mock_function_with_return(shmget, ret_shmid=100);
	init_mock_function_with_function(pool_shm_create_action, pool_shm_create_action_assert_serialize);

	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(pool_shm_create_action);
}

static int pool_shm_create_action_assert_allocate(pool_shm *cshm) {
	CUE_ASSERT_PTR_EQ(cshm->allocate(100, 64), ret_buffer);

	CUE_ASSERT_PTR_EQ(cshm->allocate(100), ret_buffer+128);
	return 0;
}

SUITE_CASE("allocate buffer") {
	init_mock_function_with_function(pool_shm_create_action, pool_shm_create_action_assert_allocate);

	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(pool_shm_create_action);
}

SUITE_END(pool_shm_create_test);

