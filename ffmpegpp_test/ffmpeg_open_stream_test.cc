#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include "stdexd/stdexd.h"
#include "mock_ffmpeg/mock_ffmpeg.h"
#include "ffmpegpp/ffmpegpp.h"

SUITE_START("ffmpeg_open_test");

static AVFormatContext ret_format_context;
static int stub_avformat_open_input(AVFormatContext **ps, const char *, AVInputFormat *, AVDictionary **) {
	*ps = &ret_format_context;
	return 0;
}

static const char *arg_input_file;
static enum AVMediaType arg_track_type;
static int arg_track_index;

mock_function_1(int, ffmpeg_open_action, ffmpeg_stream *);

static int ffmpeg_open_action_ref(ffmpeg_stream &f) {
	return ffmpeg_open_action(&f);
}

BEFORE_EACH() {
	init_subject("");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;
	app_stderr = actxt.error_stream;

	arg_input_file = "test.avi";

	init_mock_function(av_register_all);
	init_mock_function_with_function(avformat_open_input, stub_avformat_open_input);
	init_mock_function(avformat_find_stream_info);
	init_mock_function(avformat_close_input);

	init_mock_function(ffmpeg_open_action);
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUBJECT(int) {
	return ffmpeg::open_stream(arg_input_file, arg_track_type, arg_track_index, ffmpeg_open_action_ref);
}

SUITE_CASE("should make sure open and close stream file") {
	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(av_register_all);

	CUE_EXPECT_CALLED_ONCE(avformat_open_input);
	CUE_EXPECT_CALLED_WITH_STRING(avformat_open_input, 2, arg_input_file);

	CUE_EXPECT_CALLED_ONCE(avformat_find_stream_info);
	CUE_EXPECT_CALLED_WITH_PTR(avformat_find_stream_info, 1, &ret_format_context);

	CUE_EXPECT_CALLED_ONCE(ffmpeg_open_action);

	CUE_EXPECT_CALLED_ONCE(avformat_close_input);
}

SUITE_CASE("should output avformat_open_input error message and exit") {
	init_mock_function_with_return(avformat_open_input, -2);

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_EXPECT_NEVER_CALLED(avformat_find_stream_info);

	CUE_EXPECT_NEVER_CALLED(ffmpeg_open_action);

	CUE_EXPECT_NEVER_CALLED(avformat_close_input);

	CUE_ASSERT_STDERR_EQ("Error[libwrpffp]: -2\n");
}

SUITE_CASE("should output avformat_find_stream_info error message and exit") {
	init_mock_function_with_return(avformat_find_stream_info, -2);

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_EXPECT_NEVER_CALLED(ffmpeg_open_action);

	CUE_EXPECT_CALLED_ONCE(avformat_close_input);

	CUE_ASSERT_STDERR_EQ("Error[libwrpffp]: -2\n");
}

static int ffmpeg_open_action_assert(ffmpeg_stream *stream) {
	CUE_ASSERT_PTR_EQ(stream->format_context, &ret_format_context);
	return 100;
}

SUITE_CASE("call block and return the return of block") {
	init_mock_function_with_function(ffmpeg_open_action, ffmpeg_open_action_assert)

	CUE_ASSERT_SUBJECT_FAILED_WITH(100);
}

SUITE_END(ffmpeg_open_test);

