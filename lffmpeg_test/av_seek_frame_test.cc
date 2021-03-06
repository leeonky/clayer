#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include "stdexd/stdexd.h"
#include "mock_ffmpeg/mock_ffmpeg.h"
#include "lffmpeg/lffmpeg.h"

SUITE_START("av_seek_frame_test");

static AVFormatContext arg_av_format_context;

static int64_t arg_time;

mock_function_0(int, av_seek_frame_action);

BEFORE_EACH() {
	init_subject("");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;
	app_stderr = actxt.error_stream;

	arg_time = 10000000;

	init_mock_function(avformat_seek_file);
	init_mock_function(av_seek_frame_action);
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUBJECT(int) {
	return av_seek_frame(arg_av_format_context, arg_time, av_seek_frame_action);
}

SUITE_CASE("seek frame") {
	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(avformat_seek_file);
	CUE_EXPECT_CALLED_WITH_PTR(avformat_seek_file, 1, &arg_av_format_context);
	CUE_EXPECT_CALLED_WITH_INT(avformat_seek_file, 2, -1);
	CUE_EXPECT_CALLED_WITH_INT(avformat_seek_file, 3, arg_time-100000);
	CUE_EXPECT_CALLED_WITH_INT(avformat_seek_file, 4, arg_time);
	CUE_EXPECT_CALLED_WITH_INT(avformat_seek_file, 5, arg_time+100000);
	CUE_EXPECT_CALLED_WITH_INT(avformat_seek_file, 6, AVSEEK_FLAG_BACKWARD);

	CUE_EXPECT_CALLED_ONCE(av_seek_frame_action);
}

SUITE_END(av_seek_frame_test);

