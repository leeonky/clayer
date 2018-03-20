#include <cstdio>
#include <cstdlib>
#include <memory>
#include <cunitexd.h>
#include "stdexd/stdexd.h"
#include "iobus/iobus.h"

SUITE_START("iobus_forward_untill_test");

mock_function_1(int, mock_forward_action, int);

BEFORE_EACH() {
	init_subject("");
	init_mock_function(mock_forward_action);
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

static int do_event(iobus &iob, int(*action)(int)) {
	int a;
	return iob.get("DO", [&](){
			iob.post("HELLO");
			return action(a);
			}, 1, "arg:%d", &a);
}

SUITE_CASE("has matched event") {
	iobus iob{actxt.input_stream, actxt.output_stream, actxt.error_stream};

	init_subject("TEST a:1\nDO arg:2");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;

	CUE_ASSERT_EQ(forward_untill(iob, do_event, mock_forward_action), 0);

	CUE_EXPECT_CALLED_ONCE(mock_forward_action);
	CUE_EXPECT_CALLED_WITH_INT(mock_forward_action, 1, 2);

	CUE_ASSERT_STDOUT_EQ("TEST a:1\nHELLO\n");
}

SUITE_CASE("no matched event") {
	iobus iob{actxt.input_stream, actxt.output_stream, actxt.error_stream};

	init_subject("TEST a:1");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;

	CUE_ASSERT_EQ(forward_untill(iob, do_event, mock_forward_action), -1);

	CUE_EXPECT_NEVER_CALLED(mock_forward_action);
	CUE_ASSERT_STDOUT_EQ("TEST a:1\n");
}

SUITE_END(iobus_forward_untill_test);

