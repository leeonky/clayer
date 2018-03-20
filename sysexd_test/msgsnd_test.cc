#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include <sys/msg.h>
#include "mock_std/mock_std.h"
#include "stdexd/stdexd.h"
#include "mock_sys/mock_sys.h"
#include "sysexd/sysexd.h"

mock_function_0(int, msgsnd_action);

static int arg_msgid;

struct snd_message {
	long type;
	int buffer[8];
} arg_message;

SUITE_START("msgsnd_test");

BEFORE_EACH() {
	init_subject("");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;
	app_stderr = actxt.error_stream;

	arg_msgid = 1;

	init_mock_function(msgsnd);
	init_mock_function(msgsnd_action);
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUBJECT(int) {
	return msgsnd(arg_msgid, arg_message, msgsnd_action);
}

SUITE_CASE("msgsnd succeded") {
	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(msgsnd);
	CUE_EXPECT_CALLED_WITH_INT(msgsnd, 1, arg_msgid);
	CUE_EXPECT_CALLED_WITH_PTR(msgsnd, 2, &arg_message);
	CUE_EXPECT_CALLED_WITH_INT(msgsnd, 3, (sizeof(snd_message)-sizeof(long)));
	CUE_EXPECT_CALLED_WITH_INT(msgsnd, 4, IPC_NOWAIT);

	CUE_EXPECT_CALLED_ONCE(msgsnd_action);
}

int stub_msgsnd_failed(int, const void *, size_t, int) {
	errno = 100;
	return -1;
}

SUITE_CASE("msgsnd failed") {
	init_mock_function_with_function(msgsnd, stub_msgsnd_failed);

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_EXPECT_NEVER_CALLED(msgsnd_action);

	CUE_ASSERT_STDERR_EQ("Error[libsysexd]: 100\n");
}

SUITE_END(msgsnd_test);

