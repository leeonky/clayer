#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include "stdexd/stdexd.h"
#include "mock_ffmpeg/mock_ffmpeg.h"
#include "lffmpeg/lffmpeg.h"

SUITE_START("avstream_info_test");

static AVStream arg_av_stream;
static AVCodecContext arg_av_codec_context;
static stub_decoding_context arg_decoding_context;

BEFORE_EACH() {
	static AVCodecParameters codec_parameter;
	arg_av_stream.codecpar = &codec_parameter;
	arg_av_codec_context.opaque = &arg_decoding_context;
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

SUITE_CASE("output video codec info") {
	init_mock_function_with_return(av_get_pix_fmt_name, "PIX");
	arg_av_codec_context.width = 1080;
	arg_av_codec_context.height = 1920;
	arg_av_codec_context.pix_fmt = AV_PIX_FMT_RGB24;
	arg_av_codec_context.codec_type = AVMEDIA_TYPE_VIDEO;

	CUE_ASSERT_STRING_EQ(avstream_info(arg_av_codec_context), "VIDEO width:1080 height:1920 format:PIX");

}

SUITE_CASE("output audio codec info") {
	init_mock_function_with_return(av_get_sample_fmt_name, "s16");
	init_mock_function_with_function(av_get_channel_layout_string, stub_av_get_channel_layout_string_stereo);

	arg_av_codec_context.channel_layout = AV_CH_LAYOUT_2_1;
	arg_av_codec_context.channels = 3;
	arg_av_codec_context.sample_rate = 44100;
	arg_av_codec_context.sample_fmt = AV_SAMPLE_FMT_S16;
	arg_av_codec_context.codec_type = AVMEDIA_TYPE_AUDIO;

	CUE_ASSERT_STRING_EQ(avstream_info(arg_av_codec_context), "AUDIO sample_rate:44100 channels:3 layout:stereo format:s16 passthrough:0");
}

SUITE_CASE("output truehd audio codec with passthrough") {
	init_mock_function_with_return(av_get_sample_fmt_name, "s16");
	init_mock_function_with_function(av_get_channel_layout_string, stub_av_get_channel_layout_string_stereo);

	arg_av_codec_context.codec_id = AV_CODEC_ID_TRUEHD;
	arg_av_codec_context.channel_layout = AV_CH_LAYOUT_2_1;
	arg_av_codec_context.channels = 3;
	arg_av_codec_context.sample_rate = 44100;
	arg_av_codec_context.sample_fmt = AV_SAMPLE_FMT_S32;
	arg_av_codec_context.codec_type = AVMEDIA_TYPE_AUDIO;

	int res = passthrough_process(arg_av_codec_context);

	CUE_ASSERT_EQ(res, 1);
	CUE_ASSERT_STRING_EQ(avstream_info(arg_av_codec_context), "AUDIO sample_rate:192000 channels:8 layout:stereo format:s16 passthrough:1");
	CUE_EXPECT_CALLED_WITH_INT(av_get_sample_fmt_name, 1, AV_SAMPLE_FMT_S16);
	CUE_EXPECT_CALLED_WITH_INT(av_get_channel_layout_string, 3, 8);
	CUE_EXPECT_CALLED_WITH_INT(av_get_channel_layout_string, 4, AV_CH_LAYOUT_7POINT1);
}

SUITE_CASE("output eac3 audio codec with passthrough") {
	init_mock_function_with_return(av_get_sample_fmt_name, "s16");
	init_mock_function_with_function(av_get_channel_layout_string, stub_av_get_channel_layout_string_stereo);

	arg_av_codec_context.codec_id = AV_CODEC_ID_EAC3;
	arg_av_codec_context.channel_layout = AV_CH_LAYOUT_2_1;
	arg_av_codec_context.channels = 3;
	arg_av_codec_context.sample_rate = 44100;
	arg_av_codec_context.sample_fmt = AV_SAMPLE_FMT_S32;
	arg_av_codec_context.codec_type = AVMEDIA_TYPE_AUDIO;

	int res = passthrough_process(arg_av_codec_context);

	CUE_ASSERT_EQ(res, 1);
	CUE_ASSERT_STRING_EQ(avstream_info(arg_av_codec_context), "AUDIO sample_rate:44100 channels:8 layout:stereo format:s16 passthrough:1");
	CUE_EXPECT_CALLED_WITH_INT(av_get_sample_fmt_name, 1, AV_SAMPLE_FMT_S16);
	CUE_EXPECT_CALLED_WITH_INT(av_get_channel_layout_string, 3, 8);
	CUE_EXPECT_CALLED_WITH_INT(av_get_channel_layout_string, 4, AV_CH_LAYOUT_7POINT1);
}

SUITE_CASE("output ac3 audio codec with passthrough") {
	init_mock_function_with_return(av_get_sample_fmt_name, "s16");
	init_mock_function_with_function(av_get_channel_layout_string, stub_av_get_channel_layout_string_stereo);

	arg_av_codec_context.codec_id = AV_CODEC_ID_AC3;
	arg_av_codec_context.channel_layout = AV_CH_LAYOUT_2_1;
	arg_av_codec_context.channels = 3;
	arg_av_codec_context.sample_rate = 44100;
	arg_av_codec_context.sample_fmt = AV_SAMPLE_FMT_S32;
	arg_av_codec_context.codec_type = AVMEDIA_TYPE_AUDIO;

	int res = passthrough_process(arg_av_codec_context);

	CUE_ASSERT_EQ(res, 1);
	CUE_ASSERT_STRING_EQ(avstream_info(arg_av_codec_context), "AUDIO sample_rate:44100 channels:2 layout:stereo format:s16 passthrough:1");
	CUE_EXPECT_CALLED_WITH_INT(av_get_sample_fmt_name, 1, AV_SAMPLE_FMT_S16);
	CUE_EXPECT_CALLED_WITH_INT(av_get_channel_layout_string, 3, 2);
	CUE_EXPECT_CALLED_WITH_INT(av_get_channel_layout_string, 4, AV_CH_LAYOUT_STEREO);
}

SUITE_CASE("output dtshd ma audio codec with passthrough") {
	init_mock_function_with_return(av_get_sample_fmt_name, "s16");
	init_mock_function_with_function(av_get_channel_layout_string, stub_av_get_channel_layout_string_stereo);

	arg_av_codec_context.codec_id = AV_CODEC_ID_DTS;
	arg_av_codec_context.channel_layout = AV_CH_LAYOUT_2_1;
	arg_av_codec_context.channels = 3;
	arg_av_codec_context.sample_rate = 44100;
	arg_av_codec_context.sample_fmt = AV_SAMPLE_FMT_S32;
	arg_av_codec_context.codec_type = AVMEDIA_TYPE_AUDIO;
	arg_av_codec_context.profile = FF_PROFILE_DTS_HD_MA;

	int res = passthrough_process(arg_av_codec_context);

	CUE_ASSERT_EQ(res, 1);
	CUE_ASSERT_STRING_EQ(avstream_info(arg_av_codec_context), "AUDIO sample_rate:192000 channels:8 layout:stereo format:s16 passthrough:1");
	CUE_EXPECT_CALLED_WITH_INT(av_get_sample_fmt_name, 1, AV_SAMPLE_FMT_S16);
	CUE_EXPECT_CALLED_WITH_INT(av_get_channel_layout_string, 3, 8);
	CUE_EXPECT_CALLED_WITH_INT(av_get_channel_layout_string, 4, AV_CH_LAYOUT_7POINT1);
}

SUITE_CASE("output dtshd hra audio codec with passthrough") {
	init_mock_function_with_return(av_get_sample_fmt_name, "s16");
	init_mock_function_with_function(av_get_channel_layout_string, stub_av_get_channel_layout_string_stereo);

	arg_av_codec_context.codec_id = AV_CODEC_ID_DTS;
	arg_av_codec_context.channel_layout = AV_CH_LAYOUT_2_1;
	arg_av_codec_context.channels = 3;
	arg_av_codec_context.sample_rate = 44100;
	arg_av_codec_context.sample_fmt = AV_SAMPLE_FMT_S32;
	arg_av_codec_context.codec_type = AVMEDIA_TYPE_AUDIO;
	arg_av_codec_context.profile = FF_PROFILE_DTS_HD_HRA;

	int res = passthrough_process(arg_av_codec_context);

	CUE_ASSERT_EQ(res, 1);
	CUE_ASSERT_STRING_EQ(avstream_info(arg_av_codec_context), "AUDIO sample_rate:192000 channels:8 layout:stereo format:s16 passthrough:1");
	CUE_EXPECT_CALLED_WITH_INT(av_get_sample_fmt_name, 1, AV_SAMPLE_FMT_S16);
	CUE_EXPECT_CALLED_WITH_INT(av_get_channel_layout_string, 3, 8);
	CUE_EXPECT_CALLED_WITH_INT(av_get_channel_layout_string, 4, AV_CH_LAYOUT_7POINT1);
}

SUITE_CASE("output dts audio codec with passthrough") {
	init_mock_function_with_return(av_get_sample_fmt_name, "s16");
	init_mock_function_with_function(av_get_channel_layout_string, stub_av_get_channel_layout_string_stereo);

	arg_av_codec_context.codec_id = AV_CODEC_ID_DTS;
	arg_av_codec_context.channel_layout = AV_CH_LAYOUT_2_1;
	arg_av_codec_context.channels = 3;
	arg_av_codec_context.sample_rate = 44100;
	arg_av_codec_context.sample_fmt = AV_SAMPLE_FMT_S32;
	arg_av_codec_context.codec_type = AVMEDIA_TYPE_AUDIO;
	arg_av_codec_context.profile = FF_PROFILE_DTS;

	int res = passthrough_process(arg_av_codec_context);

	CUE_ASSERT_EQ(res, 1);
	CUE_ASSERT_STRING_EQ(avstream_info(arg_av_codec_context), "AUDIO sample_rate:48000 channels:2 layout:stereo format:s16 passthrough:1");
	CUE_EXPECT_CALLED_WITH_INT(av_get_sample_fmt_name, 1, AV_SAMPLE_FMT_S16);
	CUE_EXPECT_CALLED_WITH_INT(av_get_channel_layout_string, 3, 2);
	CUE_EXPECT_CALLED_WITH_INT(av_get_channel_layout_string, 4, AV_CH_LAYOUT_STEREO);
}

SUITE_END(avstream_info_test)
