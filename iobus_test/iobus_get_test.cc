#include <cstdio>
#include <cstdlib>
#include <memory>
#include <cunitexd.h>
#include "stdexd/stdexd.h"
#include "iobus/iobus.h"

SUITE_START("iobus_get_test");

mock_function_2(int, get_message_action, const char *, const char *);
mock_function_2(int, get_message_action2, const char *, const char *);

BEFORE_EACH() {
	init_subject("");
	init_mock_function(get_message_action);
	init_mock_function(get_message_action2);
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUBJECT(int) {
	iobus iob{actxt.input_stream, actxt.output_stream, actxt.error_stream};
	return iob.get(get_message_action);
}

SUITE_CASE("get from stdin") {
	init_subject("TEST a:1");
	app_stdin = actxt.input_stream;

	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(get_message_action);
	CUE_EXPECT_CALLED_WITH_STRING(get_message_action, 1, "TEST");
	CUE_EXPECT_CALLED_WITH_STRING(get_message_action, 2, "a:1");
}

SUITE_CASE("should cache last result when action return non zero") {
	init_subject("TEST a:1");
	app_stdin = actxt.input_stream;
	init_mock_function_with_return(get_message_action, 100);
	iobus iob{actxt.input_stream, actxt.output_stream, actxt.error_stream};

	CUE_ASSERT_EQ(iob.get(get_message_action), 100);

	CUE_ASSERT_EQ(iob.get(get_message_action2), 0);

	CUE_EXPECT_CALLED_ONCE(get_message_action2);
	CUE_EXPECT_CALLED_WITH_STRING(get_message_action2, 1, "TEST");
	CUE_EXPECT_CALLED_WITH_STRING(get_message_action2, 2, "a:1");
}

SUITE_CASE("bad format: empty args") {
	init_subject("TEST\n");
	app_stdin = actxt.input_stream;

	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(get_message_action);
	CUE_EXPECT_CALLED_WITH_STRING(get_message_action, 1, "TEST");
	CUE_EXPECT_CALLED_WITH_STRING(get_message_action, 2, "");
}

SUITE_CASE("bad format: empty line") {
	init_subject("\n");
	app_stdin = actxt.input_stream;

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_EXPECT_NEVER_CALLED(get_message_action);
}

SUITE_CASE("un except event") {
	init_subject("DO\n");
	app_stdin = actxt.input_stream;
	iobus iob{actxt.input_stream, actxt.output_stream, actxt.error_stream};

	CUE_ASSERT_EQ(iob.except("TEST"), 0);
}

SUITE_CASE("except event") {
	init_subject("TEST\n");
	app_stdin = actxt.input_stream;
	iobus iob{actxt.input_stream, actxt.output_stream, actxt.error_stream};

	CUE_ASSERT_EQ(iob.except("TEST")!=0, 1);
}

SUITE_CASE("ignore_untill") {
	init_subject("TEST\nDO");
	app_stdin = actxt.input_stream;
	iobus iob{actxt.input_stream, actxt.output_stream, actxt.error_stream};

	iob.ignore_untill("DO");
	iob.get(get_message_action);

	CUE_EXPECT_CALLED_ONCE(get_message_action);
	CUE_EXPECT_CALLED_WITH_STRING(get_message_action, 1, "DO");
}

SUITE_END(iobus_get_test);

