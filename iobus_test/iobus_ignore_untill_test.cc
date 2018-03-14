#include <cstdio>
#include <cstdlib>
#include <memory>
#include <cunitexd.h>
#include "stdexd/stdexd.h"
#include "iobus/iobus.h"

SUITE_START("iobus_ignore_untill_test");

mock_function_1(int, mock_ignore_action, int);

BEFORE_EACH() {
	init_subject("");
	init_mock_function(mock_ignore_action);
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

static int do_event(iobus &iob, int(*action)(int)) {
	int a;
	return iob.get("DO", [&](){
			return action(a);
			}, 1, "arg:%d", &a);
}

SUITE_CASE("has matched event") {
	iobus iob{actxt.input_stream, actxt.output_stream, actxt.error_stream};

	init_subject("TEST a:1\nDO arg:2");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;

	CUE_ASSERT_EQ(ignore_untill(iob, do_event, mock_ignore_action), 0);

	CUE_EXPECT_CALLED_ONCE(mock_ignore_action);
	CUE_EXPECT_CALLED_WITH_INT(mock_ignore_action, 1, 2);
}

SUITE_CASE("no matched event") {
	iobus iob{actxt.input_stream, actxt.output_stream, actxt.error_stream};

	init_subject("TEST a:1");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;

	CUE_ASSERT_EQ(ignore_untill(iob, do_event, mock_ignore_action), -1);

	CUE_EXPECT_NEVER_CALLED(mock_ignore_action);
}

SUITE_END(iobus_ignore_untill_test);

