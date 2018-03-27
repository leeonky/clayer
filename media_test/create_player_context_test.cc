#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include <memory>
#include "stdexd/stdexd.h"
#include "mock_sys/mock_sys.h"
#include "media/media.h"

SUITE_START("create_player_context_test");

mock_function_1(int, player_context_action, player_context *);
static int player_context_action_ref(player_context &ctxt) {
	return player_context_action(&ctxt);
}
static int ret_msgid;

BEFORE_EACH() {
	init_subject("CONTROL id:1");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;
	app_stderr = actxt.error_stream;

	ret_msgid = 10;

	init_mock_function_with_return(msgget, ret_msgid);

	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUBJECT(int) {
	iobus iob{actxt.input_stream, actxt.output_stream, actxt.error_stream};
	return player_context::start(iob, player_context_action_ref);
}

static ssize_t stub_msgrcv(int, void *buffer, size_t, long, int) {
	strcpy((char *)buffer, "s 100");
	return 6;
}

static int assert_context(player_context *context) {
	init_mock_function_with_function(msgrcv, stub_msgrcv);
	init_mock_function(msgsnd);

	context->process_command();

	CUE_EXPECT_CALLED_ONCE(msgsnd);
	CUE_EXPECT_CALLED_WITH_INT(msgsnd, 1, 1);
	return 0;
}

SUITE_CASE("Init msg and receiver") {
	init_mock_function_with_function(player_context_action, assert_context);

	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(player_context_action);

	CUE_ASSERT_STDOUT_EQ("CONTROL id:10\n");
}

SUITE_END(create_player_context_test);


