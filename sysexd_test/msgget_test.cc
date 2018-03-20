#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include <sys/msg.h>
#include "mock_std/mock_std.h"
#include "stdexd/stdexd.h"
#include "mock_sys/mock_sys.h"
#include "sysexd/sysexd.h"

mock_function_1(int, msgget_action, int);

static int ret_msgid;

SUITE_START("msgget_test");

BEFORE_EACH() {
	init_subject("");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;
	app_stderr = actxt.error_stream;

	ret_msgid = 1;

	init_mock_function_with_return(msgget, ret_msgid);
	init_mock_function(msgget_action);
	init_mock_function(msgctl);
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUBJECT(int) {
	return msgget(msgget_action);
}

SUITE_CASE("msgat succeded") {
	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(msgget);
	CUE_EXPECT_CALLED_WITH_INT(msgget, 1, IPC_PRIVATE);
	CUE_EXPECT_CALLED_WITH_INT(msgget, 2, 0666 | IPC_CREAT);

	CUE_EXPECT_CALLED_ONCE(msgget_action);
	CUE_EXPECT_CALLED_WITH_INT(msgget_action, 1, ret_msgid);

	CUE_EXPECT_CALLED_ONCE(msgctl);
	CUE_EXPECT_CALLED_WITH_INT(msgctl, 1, ret_msgid);
	CUE_EXPECT_CALLED_WITH_INT(msgctl, 2, IPC_RMID);
	CUE_EXPECT_CALLED_WITH_PTR(msgctl, 3, nullptr);
}

int stub_msgget_failed(key_t, int) {
	errno = 100;
	return -1;
}

SUITE_CASE("msgget failed") {
	init_mock_function_with_function(msgget, stub_msgget_failed);

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_EXPECT_NEVER_CALLED(msgget_action);

	CUE_EXPECT_NEVER_CALLED(msgctl);

	CUE_ASSERT_STDERR_EQ("Error[libsysexd]: 100\n");
}

SUITE_END(msgget_test);

