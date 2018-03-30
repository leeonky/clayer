#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include "stdexd/stdexd.h"
#include "mock_ffmpeg/mock_ffmpeg.h"
#include "lffmpeg/lffmpeg.h"

SUITE_START("av_frame_pts2_test");

static stub_decoding_context arg_decoding_context;
static AVFrame arg_av_frame;
static int64_t arg_start_time, arg_frame_pts;
//static int64_t, ret_pts, arg_duration;
//static int64_t arg_previous_pts, arg_previous_duration, arg_pkt_duration, ret_duration;
//static int64_t arg_sample_rate, arg_nb_samples;

BEFORE_EACH() {
	static AVStream av_stream;
	av_stream.start_time = arg_start_time = 10;
	av_stream.time_base.num = 1;
	av_stream.time_base.den = 24;
	arg_decoding_context.av_stream = &av_stream;
	arg_av_frame.opaque = &arg_decoding_context;
	arg_frame_pts = 58;

	arg_av_frame.pts = AV_NOPTS_VALUE;

	//static AVCodecParameters codec_parameter;
	//codec_parameter.codec_type = AVMEDIA_TYPE_AUDIO;
	//av_stream.codecpar = &codec_parameter;
	//static AVCodecContext codec_context;
	//arg_decoding_context.av_codec_context = &codec_context;
	//arg_av_frame.pkt_duration = arg_duration = 0;
	//arg_av_frame.sample_rate = 44100;
	//ret_pts = 1000;
	//ret_duration = 3000;
	init_mock_function(av_frame_get_best_effort_timestamp);
	//init_mock_function(av_rescale_q);
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUITE_CASE("get by avframe pts") {
	arg_av_frame.pts = arg_frame_pts;

	CUE_ASSERT_EQ(av_frame_pts2(arg_av_frame), 2000000);
}

SUITE_CASE("get by best_effort_timestamp") {
	arg_av_frame.pts = AV_NOPTS_VALUE;
	init_mock_function_with_return(av_frame_get_best_effort_timestamp, arg_frame_pts);

	CUE_ASSERT_EQ(av_frame_pts2(arg_av_frame), 2000000);

	CUE_EXPECT_CALLED_ONCE(av_frame_get_best_effort_timestamp);
	CUE_EXPECT_CALLED_WITH_PTR(av_frame_get_best_effort_timestamp, 1, &arg_av_frame);
}

SUITE_END(av_frame_pts2_test);
