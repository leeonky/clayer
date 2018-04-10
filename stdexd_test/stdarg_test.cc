#include <cunitexd.h>
#include "stdexd/stdexd.h"

SUITE_START("stdarg_test");

mock_void_function_1(assert_arg_1, const char *);
mock_void_function_1(assert_arg_2, const char *);
mock_void_function_1(assert_arg_3, const char *);
mock_void_function_1(assert_arg_4, const char *);
mock_void_function_1(assert_arg_5, const char *);

BEFORE_EACH() {
	init_subject("");
	init_mock_function(assert_arg_1);
	init_mock_function(assert_arg_2);
	init_mock_function(assert_arg_3);
	init_mock_function(assert_arg_4);
	init_mock_function(assert_arg_5);
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUITE_CASE("parse with empty args") {
	init_subject("");

	CUE_ASSERT_PTR_EQ(command_argument().parse(actxt.argc, actxt.argv), NULL);
}

SUITE_CASE("parse with last arg") {
	init_subject("", "hello");

	CUE_ASSERT_STRING_EQ(command_argument().parse(actxt.argc, actxt.argv), "hello");
}

SUITE_CASE("parse paramters") {
	init_subject("", "--ver", "01", "-t", "ok", "--action=go", "-pfoo", "hello");

	CUE_ASSERT_STRING_EQ(command_argument()
			.require_full_argument("ver", 'v', [](const char *arg){
				assert_arg_1(arg);
				})
			.require_full_argument("test", 't', [](const char *arg){
				assert_arg_2(arg);
				})
			.require_full_argument("action", 'a', [](const char *arg){
				assert_arg_3(arg);
				})
			.require_full_argument("prev", 'p', [](const char *arg){
				assert_arg_4(arg);
				})
			.parse(actxt.argc, actxt.argv), "hello");

	CUE_EXPECT_CALLED_ONCE(assert_arg_1);
	CUE_EXPECT_CALLED_WITH_STRING(assert_arg_1, 1, "01");

	CUE_EXPECT_CALLED_ONCE(assert_arg_2);
	CUE_EXPECT_CALLED_WITH_STRING(assert_arg_2, 1, "ok");

	CUE_EXPECT_CALLED_ONCE(assert_arg_3);
	CUE_EXPECT_CALLED_WITH_STRING(assert_arg_3, 1, "go");

	CUE_EXPECT_CALLED_ONCE(assert_arg_4);
	CUE_EXPECT_CALLED_WITH_STRING(assert_arg_4, 1, "foo");
}

SUITE_CASE("option paramters") {
	init_subject("", "--pass");
	command_argument().require_option("pass", 'p', assert_arg_1).parse(actxt.argc, actxt.argv);

	CUE_EXPECT_CALLED_ONCE(assert_arg_1);
}

SUITE_END(stdarg_test);
