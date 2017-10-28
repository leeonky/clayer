#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include <sys/shm.h>
#include "stdexd/stdexd.h"
#include "mock_sys/mock_sys.h"
#include "mock_std/mock_std.h"
#include "mem/circular_shm.h"

mock_function_1(int, circular_shm_create_action, circular_shm *);

static int circular_shm_create_action_ref(circular_shm &m) {
	return circular_shm_create_action(&m);
}

static size_t arg_size;
static int arg_count, ret_shmid;
static char ret_buffer[4096*10];
static sem_t ret_sem;

SUITE_START("circular_shm_create_test");

BEFORE_EACH() {
	init_subject("");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;
	app_stderr = actxt.error_stream;

	ret_shmid = 1;
	arg_size = 1;
	arg_count = 1;

	init_mock_function(circular_shm_create_action);
	init_mock_function_with_return(shmget, ret_shmid);
	init_mock_function_with_return(shmat, ret_buffer);
	init_mock_function(shmdt);
	init_mock_function(shmctl);
	init_mock_function_with_return(sem_new_with_id, &ret_sem);
	init_mock_function(sem_close);
	init_mock_function(sem_unlink_with_id);
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUBJECT(int) {
	return circular_shm::create(arg_size, arg_count, circular_shm_create_action_ref);
}

SUITE_CASE("size alignment: size < pagesize") {
	arg_size = getpagesize()-1;
	arg_count = 2;

	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(shmget);
	CUE_EXPECT_CALLED_WITH_INT(shmget, 1, IPC_PRIVATE);
	CUE_EXPECT_CALLED_WITH_INT(shmget, 2, arg_count*getpagesize());
	CUE_EXPECT_CALLED_WITH_INT(shmget, 3, 0666 | IPC_CREAT);
}

SUITE_CASE("size alignment: size = pagesize") {
	arg_size = getpagesize();
	arg_count = 2;

	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(shmget);
	CUE_EXPECT_CALLED_WITH_INT(shmget, 1, IPC_PRIVATE);
	CUE_EXPECT_CALLED_WITH_INT(shmget, 2, arg_count*getpagesize());
	CUE_EXPECT_CALLED_WITH_INT(shmget, 3, 0666 | IPC_CREAT);
}

SUITE_CASE("size alignment: size > pagesize") {
	arg_size = getpagesize()*2-1;
	arg_count = 2;

	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(shmget);
	CUE_EXPECT_CALLED_WITH_INT(shmget, 1, IPC_PRIVATE);
	CUE_EXPECT_CALLED_WITH_INT(shmget, 2, arg_count*(getpagesize()*2));
	CUE_EXPECT_CALLED_WITH_INT(shmget, 3, 0666 | IPC_CREAT);
}

SUITE_CASE("init with all resources") {
	init_mock_function_with_return(shmget, ret_shmid=100);
	init_mock_function_with_return(circular_shm_create_action, 1000);

	CUE_ASSERT_SUBJECT_FAILED_WITH(1000);

	CUE_EXPECT_CALLED_ONCE(shmget);

	CUE_EXPECT_CALLED_ONCE(shmat);
	CUE_EXPECT_CALLED_WITH_INT(shmat, 1, ret_shmid);
	CUE_EXPECT_CALLED_WITH_PTR(shmat, 2, nullptr);
	CUE_EXPECT_CALLED_WITH_INT(shmat, 3, 0);

	CUE_EXPECT_CALLED_ONCE(sem_new_with_id);
	CUE_EXPECT_CALLED_WITH_INT(sem_new_with_id, 1, getpid());
	CUE_EXPECT_CALLED_WITH_INT(sem_new_with_id, 2, arg_count);

	CUE_EXPECT_CALLED_ONCE(circular_shm_create_action);

	CUE_EXPECT_CALLED_ONCE(sem_close);
	CUE_EXPECT_CALLED_WITH_PTR(sem_close, 1, &ret_sem);

	CUE_EXPECT_CALLED_ONCE(sem_unlink_with_id);
	CUE_EXPECT_CALLED_WITH_INT(sem_unlink_with_id, 1, getpid());

	CUE_EXPECT_CALLED_ONCE(shmdt);
	CUE_EXPECT_CALLED_WITH_PTR(shmdt, 1, ret_buffer);

	CUE_EXPECT_CALLED_ONCE(shmctl);
	CUE_EXPECT_CALLED_WITH_INT(shmctl, 1, ret_shmid);
	CUE_EXPECT_CALLED_WITH_INT(shmctl, 2, IPC_RMID);
	CUE_EXPECT_CALLED_WITH_PTR(shmctl, 3, nullptr);
}

static int circular_shm_create_action_assert_serialize(circular_shm *cshm) {
	char buffer[1024];
	sprintf(buffer, "BUFFER id:%d size:%d count:%d sem:%d", ret_shmid, getpagesize(), arg_count, getpid());
	CUE_ASSERT_STRING_EQ(cshm->serialize_to_string(), buffer);
	return 0;
}

SUITE_CASE("circular_shm serialize to string") {
	arg_size = 1;
	arg_count = 1024;
	init_mock_function_with_return(shmget, ret_shmid=100);
	init_mock_function_with_function(circular_shm_create_action, circular_shm_create_action_assert_serialize);

	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(circular_shm_create_action);
}

static int circular_shm_create_action_assert_allocate(circular_shm *cshm) {
	CUE_ASSERT_PTR_EQ(cshm->allocate(), ret_buffer);

	CUE_EXPECT_CALLED_ONCE(sem_wait);
	CUE_EXPECT_CALLED_WITH_PTR(sem_wait, 1, &ret_sem);

	CUE_ASSERT_PTR_EQ(cshm->allocate(), ret_buffer+getpagesize());
	CUE_ASSERT_PTR_EQ(cshm->allocate(), ret_buffer);
	return 0;
}

SUITE_CASE("allocate buffer") {
	arg_size = 1;
	arg_count = 2;
	init_mock_function_with_function(circular_shm_create_action, circular_shm_create_action_assert_allocate);
	init_mock_function(sem_wait);

	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(circular_shm_create_action);
}
//save semaphore

SUITE_END(circular_shm_create_test);
