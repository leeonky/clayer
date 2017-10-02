#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include "stdexd/stdexd.h"
#include "mock_ffmpeg/mock_ffmpeg.h"
#include "lffmpeg/lffmpeg.h"

SUITE_START("avstream_info_test");

static AVStream arg_av_stream;

BEFORE_EACH() {
	static AVCodecParameters codec_parameter;
	arg_av_stream.codecpar = &codec_parameter;

	return 0;
}

SUITE_CASE("output video stream info") {
	init_mock_function_with_return(av_get_pix_fmt_name, "PIX");
	arg_av_stream.codecpar->width = 1080;
	arg_av_stream.codecpar->height = 1920;
	arg_av_stream.codecpar->format = 128;
	arg_av_stream.codecpar->codec_type = AVMEDIA_TYPE_VIDEO;

	CUE_ASSERT_STRING_EQ(avstream_info(arg_av_stream), "VIDEO width:1080 height:1920 format:PIX");
}

static void stub_av_get_channel_layout_string_stereo(char *buf, int, int, uint64_t) {
	sprintf(buf, "stereo");
}

SUITE_CASE("output audio stream info") {
	init_mock_function_with_return(av_get_sample_fmt_name, "s16");
	init_mock_function_with_function(av_get_channel_layout_string, stub_av_get_channel_layout_string_stereo);

	arg_av_stream.codecpar->channel_layout = AV_CH_LAYOUT_2_1;
	arg_av_stream.codecpar->channels = 3;
	arg_av_stream.codecpar->sample_rate = 44100;
	arg_av_stream.codecpar->format = AV_SAMPLE_FMT_S16;
	arg_av_stream.codecpar->codec_type = AVMEDIA_TYPE_AUDIO;

	CUE_ASSERT_STRING_EQ(avstream_info(arg_av_stream), "AUDIO sample_rate:44100 channels:3 layout:stereo format:s16");
}

SUITE_END(avstream_info_test)
