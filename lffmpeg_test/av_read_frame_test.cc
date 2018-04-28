#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include "stdexd/stdexd.h"
#include "mock_ffmpeg/mock_ffmpeg.h"
#include "lffmpeg/lffmpeg.h"

SUITE_START("av_read_frame_test");

static stub_decoding_context arg_decoding_context;
static AVFormatContext arg_av_format_context;
static AVCodecContext arg_av_codec_context;
static AVPacket arg_av_packet;
static AVStream arg_av_stream;
static int arg_stream_index;

static int stub_av_read_frame_always_got_matched_stream_packet(AVFormatContext *, AVPacket *av_packet) {
	av_packet->stream_index = arg_stream_index;
	return 0;
}

mock_void_function_1(av_packet_action, AVPacket *);

BEFORE_EACH() {
	init_subject("");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;
	app_stderr = actxt.error_stream;

	arg_stream_index = 0;
	arg_av_codec_context.opaque = &arg_decoding_context;
	arg_decoding_context.av_packet = &arg_av_packet;
	arg_decoding_context.av_stream = &arg_av_stream;

	init_mock_function_with_function(av_read_frame, stub_av_read_frame_always_got_matched_stream_packet);
	init_mock_function(av_packet_action);
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUBJECT(int) {
	return av_read_frame(arg_av_format_context, arg_av_codec_context, av_packet_action);
}

SUITE_CASE("should read from media and send to decoder") {
	arg_av_stream.index = arg_stream_index = 0;

	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(av_read_frame);
	CUE_EXPECT_CALLED_WITH_PTR(av_read_frame, 1, &arg_av_format_context);
	CUE_EXPECT_CALLED_WITH_PTR(av_read_frame, 2, &arg_av_packet);

	CUE_EXPECT_CALLED_ONCE(av_packet_action);
	CUE_EXPECT_CALLED_WITH_PTR(av_packet_action, 1, &arg_av_packet);
}

static int stub_av_read_frame_matched_stream_packet_in_2nd_time(AVFormatContext *, AVPacket *av_packet) {
	if(2 == called_times_of(av_read_frame))
		av_packet->stream_index = arg_stream_index;
	else
		av_packet->stream_index = 0;
	return 0;
}

SUITE_CASE("should read from media untill get right stream packet and send to decoder") {
	arg_av_stream.index = arg_stream_index = 2;
	init_mock_function_with_function(av_read_frame, stub_av_read_frame_matched_stream_packet_in_2nd_time);

	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_TIMES(av_read_frame, 2);

	CUE_EXPECT_CALLED_ONCE(av_packet_action);
	CUE_EXPECT_CALLED_WITH_PTR(av_packet_action, 1, &arg_av_packet);
}

SUITE_END(av_read_frame_test);
