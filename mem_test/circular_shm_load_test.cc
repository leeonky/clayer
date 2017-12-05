#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include <sys/shm.h>
#include "stdexd/stdexd.h"
#include "mock_sys/mock_sys.h"
#include "mock_std/mock_std.h"
#include "mem/circular_shm.h"

mock_function_1(int, circular_shm_load_action, circular_shm *);

static int circular_shm_load_action_ref(circular_shm &m) {
	return circular_shm_load_action(&m);
}

static size_t arg_size;
static int arg_count, arg_shmid, arg_sem_id;
static char ret_buffer[4096*10];
static sem_t ret_sem;
static int arg_index;

SUITE_START("circular_shm_load_test");

BEFORE_EACH() {
	init_subject("");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;
	app_stderr = actxt.error_stream;

	arg_shmid = 1;
	arg_size = 2;
	arg_count = 3;
	arg_sem_id = 100;

	init_mock_function(circular_shm_load_action);
	init_mock_function_with_return(shmat, ret_buffer);
	init_mock_function(shmdt);
	init_mock_function_with_return(sem_load_with_id, &ret_sem);
	init_mock_function(sem_close);
	init_mock_function(sem_post);
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUBJECT(int) {
	return circular_shm::load(arg_shmid, arg_size, arg_count, arg_sem_id, circular_shm_load_action_ref);
}

static int assert_circular_shm(circular_shm *shm) {
	CUE_ASSERT_STRING_EQ(shm->serialize_to_string(), "BUFFER id:1 size:2 count:3 sem:100");
	return 0;
}

SUITE_CASE("init with all resources") {
	init_mock_function_with_function(circular_shm_load_action, assert_circular_shm);

	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(shmat);
	CUE_EXPECT_CALLED_WITH_INT(shmat, 1, arg_shmid);
	CUE_EXPECT_CALLED_WITH_PTR(shmat, 2, nullptr);
	CUE_EXPECT_CALLED_WITH_INT(shmat, 3, 0);

	CUE_EXPECT_CALLED_ONCE(sem_load_with_id);
	CUE_EXPECT_CALLED_WITH_INT(sem_load_with_id, 1, arg_sem_id);

	CUE_EXPECT_CALLED_ONCE(circular_shm_load_action);

	CUE_EXPECT_CALLED_ONCE(sem_close);
	CUE_EXPECT_CALLED_WITH_PTR(sem_close, 1, &ret_sem);

	CUE_EXPECT_CALLED_ONCE(shmdt);
	CUE_EXPECT_CALLED_WITH_PTR(shmdt, 1, ret_buffer);
}

mock_function_1(int, post_action, void *);

static int assert_circular_shm_post(circular_shm *shm) {
	arg_index = 1;
	shm->free(arg_index, post_action);

	CUE_EXPECT_CALLED_ONCE(post_action);

	CUE_EXPECT_CALLED_WITH_PTR(post_action, 1, ret_buffer+arg_index*arg_size);

	CUE_EXPECT_CALLED_ONCE(sem_post);
	CUE_EXPECT_CALLED_WITH_PTR(sem_post, 1, &ret_sem);
	return 0;
}

SUITE_CASE("release buffer") {
	init_mock_function_with_function(circular_shm_load_action, assert_circular_shm_post);

	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(circular_shm_load_action);
}

SUITE_END(circular_shm_load_test);
