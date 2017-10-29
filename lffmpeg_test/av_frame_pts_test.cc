#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include "stdexd/stdexd.h"
#include "mock_ffmpeg/mock_ffmpeg.h"
#include "lffmpeg/lffmpeg.h"

SUITE_START("av_frame_pts_test");

static stub_decoding_context arg_decoding_context;
static AVFrame arg_av_frame;
static int64_t ret_best_effort_timestamp, ret_pts, arg_start_time, arg_duration;
static int64_t arg_previous_pts, arg_previous_duration, arg_pkt_duration, ret_duration;
static int64_t arg_sample_rate, arg_nb_samples;

BEFORE_EACH() {
	static AVCodecParameters codec_parameter;
	codec_parameter.codec_type = AVMEDIA_TYPE_AUDIO;
	static AVStream av_stream;
	av_stream.codecpar = &codec_parameter;
	av_stream.start_time = arg_start_time = 200000;
	arg_decoding_context.av_stream = &av_stream;
	static AVCodecContext codec_context;
	arg_decoding_context.av_codec_context = &codec_context;
	arg_av_frame.opaque = &arg_decoding_context;
	arg_av_frame.pkt_duration = arg_duration = 0;
	arg_av_frame.sample_rate = 44100;
	ret_best_effort_timestamp = 900000;
	ret_pts = 1000;
	ret_duration = 3000;
	init_mock_function(av_frame_get_best_effort_timestamp);
	init_mock_function(av_rescale_q);
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUITE_CASE("get by best_effort_timestamp") {
	init_mock_function_with_return(av_frame_get_best_effort_timestamp, ret_best_effort_timestamp);
	init_mock_function_with_return(av_rescale_q, ret_pts);

	CUE_ASSERT_EQ(av_frame_pts(arg_av_frame), ret_pts);

	CUE_EXPECT_CALLED_ONCE(av_frame_get_best_effort_timestamp);
	CUE_EXPECT_CALLED_WITH_PTR(av_frame_get_best_effort_timestamp, 1, &arg_av_frame);

	CUE_EXPECT_CALLED_ONCE(av_rescale_q);
	CUE_EXPECT_CALLED_WITH_INT(av_rescale_q, 1, ret_best_effort_timestamp-arg_start_time);
	CUE_ASSERT_EQ(arg_decoding_context.previous_pts, ret_pts);
}

SUITE_CASE("get by previous_pts + duration") {
	arg_decoding_context.previous_pts = arg_previous_pts = 1000;
	arg_decoding_context.previous_duration = arg_previous_duration = 2000;
	init_mock_function_with_return(av_frame_get_best_effort_timestamp, AV_NOPTS_VALUE);

	CUE_ASSERT_EQ(av_frame_pts(arg_av_frame), arg_previous_pts+arg_previous_duration);
	CUE_ASSERT_EQ(arg_decoding_context.previous_pts, arg_previous_pts+arg_previous_duration);
}

SUITE_CASE("update previous_duration by pkt_duration") {
	arg_av_frame.pkt_duration = arg_pkt_duration = 3000;
	arg_decoding_context.previous_duration = 0;
	init_mock_function_with_return(av_frame_get_best_effort_timestamp, AV_NOPTS_VALUE);
	init_mock_function_with_return(av_rescale_q, ret_duration = 3000);

	av_frame_pts(arg_av_frame);

	CUE_EXPECT_CALLED_ONCE(av_rescale_q);
	CUE_EXPECT_CALLED_WITH_INT(av_rescale_q, 1, arg_pkt_duration);

	CUE_ASSERT_EQ(arg_decoding_context.previous_duration, ret_duration);
}

SUITE_CASE("get audio duration by sample_rate") {
	arg_av_frame.pkt_duration = 0;
	arg_av_frame.nb_samples = arg_nb_samples = 44100;
	arg_av_frame.sample_rate = arg_sample_rate = 44100;
	arg_decoding_context.av_stream->codecpar->codec_type = AVMEDIA_TYPE_AUDIO;

	av_frame_pts(arg_av_frame);

	CUE_ASSERT_EQ(arg_decoding_context.previous_duration, 1000000);
}

SUITE_CASE("get video duration by frame rate") {
	arg_av_frame.pkt_duration = 0;
	arg_decoding_context.av_codec_context->framerate = {10, 1};
	arg_decoding_context.av_stream->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;

	av_frame_pts(arg_av_frame);

	CUE_ASSERT_EQ(arg_decoding_context.previous_duration, 100000);
}

SUITE_CASE("get video duration by default rate 23.976") {
	arg_av_frame.pkt_duration = 0;
	arg_decoding_context.av_codec_context->framerate = {0, 1};
	arg_decoding_context.av_stream->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;

	av_frame_pts(arg_av_frame);

	CUE_ASSERT_EQ(arg_decoding_context.previous_duration, 41708);
}

SUITE_END(av_frame_pts_test);
