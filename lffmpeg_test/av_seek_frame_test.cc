#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include "stdexd/stdexd.h"
#include "mock_ffmpeg/mock_ffmpeg.h"
#include "lffmpeg/lffmpeg.h"

SUITE_START("av_seek_frame_test");

static stub_decoding_context arg_decoding_context;
static AVFormatContext arg_av_format_context;
static AVCodecContext arg_av_codec_context;
static AVStream arg_av_stream;

static int arg_index;
static int64_t arg_time;

mock_function_0(int, av_seek_frame_action);

BEFORE_EACH() {
	init_subject("");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;
	app_stderr = actxt.error_stream;

	arg_av_codec_context.opaque = &arg_decoding_context;
	arg_decoding_context.av_stream = &arg_av_stream;
	arg_av_stream.index = arg_index = 10;
	arg_av_stream.time_base.num = 1;
	arg_av_stream.time_base.den = 24;

	arg_time = 10000000;

	init_mock_function(av_seek_frame);
	init_mock_function(av_seek_frame_action);
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUBJECT(int) {
	return av_seek_frame(arg_av_format_context, arg_av_codec_context, arg_time, av_seek_frame_action);
}

SUITE_CASE("seek frame") {
	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(av_seek_frame);
	CUE_EXPECT_CALLED_WITH_PTR(av_seek_frame, 1, &arg_av_format_context);
	CUE_EXPECT_CALLED_WITH_INT(av_seek_frame, 2, arg_index);
	CUE_EXPECT_CALLED_WITH_INT(av_seek_frame, 3, arg_time*24/1000000);
	CUE_EXPECT_CALLED_WITH_INT(av_seek_frame, 4, AVSEEK_FLAG_BACKWARD);

	CUE_EXPECT_CALLED_ONCE(av_seek_frame_action);
}

SUITE_END(av_seek_frame_test);

