#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include "stdexd/stdexd.h"
#include "mock_ffmpeg/mock_ffmpeg.h"
#include "lffmpeg/lffmpeg.h"

SUITE_START("avformat_open_close_test");

static AVFormatContext ret_format_context;
static int stub_avformat_open_input(AVFormatContext **ps, const char *, AVInputFormat *, AVDictionary **) {
	*ps = &ret_format_context;
	return 0;
}

static const char *arg_input_file;

mock_function_1(int, avformat_open_action, AVFormatContext *);

static int avformat_open_action_ref(AVFormatContext &av_format_context) {
	return avformat_open_action(&av_format_context);
}

BEFORE_EACH() {
	init_subject("");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;
	app_stderr = actxt.error_stream;

	arg_input_file = "test.avi";

	init_mock_function(av_register_all);
	init_mock_function_with_function(avformat_open_input, stub_avformat_open_input);
	init_mock_function(avformat_close_input);

	init_mock_function(avformat_open_action);
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUBJECT(int) {
	return avformat_open_input(arg_input_file, avformat_open_action_ref);
}

SUITE_CASE("should make sure open and close stream file") {
	init_mock_function_with_function(avformat_open_input, stub_avformat_open_input);

	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(av_register_all);

	CUE_EXPECT_CALLED_ONCE(avformat_open_input);
	CUE_EXPECT_CALLED_WITH_STRING(avformat_open_input, 2, arg_input_file);

	CUE_EXPECT_CALLED_ONCE(avformat_open_action);
	CUE_EXPECT_CALLED_WITH_PTR(avformat_open_action, 1, &ret_format_context);

	CUE_EXPECT_CALLED_ONCE(avformat_close_input);
}

SUITE_CASE("should output avformat_open_input error message and exit") {
	init_mock_function_with_return(avformat_open_input, -2);

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_EXPECT_NEVER_CALLED(avformat_open_action);

	CUE_EXPECT_NEVER_CALLED(avformat_close_input);

	CUE_ASSERT_STDERR_EQ("Error[liblffmpeg]: -2\n");
}

SUITE_END(avformat_open_close_test);

