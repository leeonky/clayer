#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include <sys/msg.h>
#include "mock_std/mock_std.h"
#include "stdexd/stdexd.h"
#include "mock_sys/mock_sys.h"
#include "sysexd/sysexd.h"

mock_function_0(int, msgsndstr_action);

static int arg_msgid;
static const char *arg_msg;

SUITE_START("msgsndstr_test");

BEFORE_EACH() {
	init_subject("");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;
	app_stderr = actxt.error_stream;

	arg_msgid = 1;
	arg_msg = "hello";

	init_mock_function(msgsnd);
	init_mock_function(msgsndstr_action);
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUBJECT(int) {
	return msgsnd(arg_msgid, arg_msg, msgsndstr_action);
}

static int msgsnd_assert(int, const void *buffer, size_t, int) {
	long *t = (long *)buffer;
	char *p = (char *)buffer + sizeof(long);
	CUE_ASSERT_EQ(*t, 1);
	CUE_ASSERT_STRING_EQ(p, arg_msg);
	return 0;
}

SUITE_CASE("msgsnd succeded") {
	init_mock_function_with_function(msgsnd, msgsnd_assert);

	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(msgsnd);
	CUE_EXPECT_CALLED_WITH_INT(msgsnd, 1, arg_msgid);
	CUE_EXPECT_CALLED_WITH_INT(msgsnd, 3, (strlen(arg_msg)+1));
	CUE_EXPECT_CALLED_WITH_INT(msgsnd, 4, IPC_NOWAIT);

	CUE_EXPECT_CALLED_ONCE(msgsndstr_action);
}

static int stub_msgsndstr_failed(int, const void *, size_t, int) {
	errno = 100;
	return -1;
}

SUITE_CASE("msgsnd failed") {
	init_mock_function_with_function(msgsnd, stub_msgsndstr_failed);

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_EXPECT_NEVER_CALLED(msgsndstr_action);

	CUE_ASSERT_STDERR_EQ("Error[libsysexd]: 100\n");
}

SUITE_END(msgsndstr_test);

