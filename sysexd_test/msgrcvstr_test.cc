#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include <sys/msg.h>
#include "mock_std/mock_std.h"
#include "stdexd/stdexd.h"
#include "mock_sys/mock_sys.h"
#include "sysexd/sysexd.h"

mock_function_1(int, msgrcvstr_action, const char *);

static int arg_msgid;

SUITE_START("msgrcvstr_test");

BEFORE_EACH() {
	init_subject("");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;
	app_stderr = actxt.error_stream;

	arg_msgid = 1;

	init_mock_function(msgrcv);
	init_mock_function(msgrcvstr_action);
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUBJECT(int) {
	return msgrcv(arg_msgid, msgrcvstr_action);
}

static ssize_t stub_msgrcvstr(int, void *buffer, size_t, long, int) {
	strcpy((char *)buffer+sizeof(long), "hello");
	return 0;
}

SUITE_CASE("msgrcv succeded") {
	init_mock_function_with_function(msgrcv, stub_msgrcvstr);

	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(msgrcv);
	CUE_EXPECT_CALLED_WITH_INT(msgrcv, 1, arg_msgid);
	CUE_EXPECT_CALLED_WITH_INT(msgrcv, 4, 1);
	CUE_EXPECT_CALLED_WITH_INT(msgrcv, 5, IPC_NOWAIT);

	CUE_EXPECT_CALLED_ONCE(msgrcvstr_action);
	CUE_EXPECT_CALLED_WITH_STRING(msgrcvstr_action, 1, "hello");
}

static ssize_t stub_msgrcvstr_failed(int, void *, size_t, long, int) {
	errno = 100;
	return -1;
}

SUITE_CASE("msgrcv failed") {
	init_mock_function_with_function(msgrcv, stub_msgrcvstr_failed);

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_EXPECT_NEVER_CALLED(msgrcvstr_action);

	CUE_ASSERT_STDERR_EQ("Error[libsysexd]: 100\n");
}

SUITE_END(msgrcvstr_test);

