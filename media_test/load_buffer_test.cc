#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include <memory>
#include "stdexd/stdexd.h"
#include "media/media.h"
#include "mem/circular_shm.h"
#include "mock_sys/mock_sys.h"

SUITE_START("load_buffer_test");

static int ret_shmid;
static char ret_buffer[4096*10];
static sem_t ret_sem;

mock_function_1(int, buffer_action, circular_shm *);

static int buffer_action_ref(circular_shm &m) {
	return buffer_action(&m);
}

static std::unique_ptr<iobus> iob;

BEFORE_EACH() {
	init_subject("");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;
	app_stderr = actxt.error_stream;
	iob.reset(new iobus(actxt.input_stream, actxt.output_stream, actxt.error_stream));

	ret_shmid = 1;

	init_mock_function_with_return(shmat, ret_buffer);
	init_mock_function_with_return(sem_load_with_id, &ret_sem);
	init_mock_function(sem_close);
	init_mock_function(buffer_action);
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUBJECT(int) {
	return load_buffer(*iob, buffer_action_ref);
}

static int circular_shm_assert(circular_shm *shm) {
	CUE_ASSERT_STRING_EQ(shm->serialize_to_string(), "BUFFER id:0 size:3112960 count:16 sem:7214");
	return 0;
}

SUITE_CASE("load circular_shm") {
	init_subject("BUFFER id:0 size:3112960 count:16 sem:7214");
	init_mock_function_with_function(buffer_action, circular_shm_assert);

	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(shmat);
	CUE_EXPECT_CALLED_WITH_INT(shmat, 1, 0);

	CUE_EXPECT_CALLED_ONCE(sem_load_with_id);
	CUE_EXPECT_CALLED_WITH_INT(sem_load_with_id, 1, 7214);
}

SUITE_CASE("event not match") {
	init_subject("AUDIO id:0 size:3112960 count:16 sem:7214");

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);
}

SUITE_CASE("args not match") {
	init_subject("BUFFER id:0 size:3112960 count:16");

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_ASSERT_STDERR_EQ("Error[libiobus]: Invalid BUFFER arguments 'id:0 size:3112960 count:16'\n");
}

SUITE_END(load_buffer_test);

