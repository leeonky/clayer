#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include "stdexd/stdexd.h"
#include "mock_ffmpeg/mock_ffmpeg.h"
#include "lffmpeg/lffmpeg.h"

SUITE_START("av_get_buffer_size_test");

static stub_decoding_context arg_decoding_context;

static AVCodecContext arg_codec_context;
static int arg_align, arg_width, arg_height;
static AVPixelFormat arg_pix_format;
static AVSampleFormat arg_sample_format;

static int arg_channels, arg_samples_size;


BEFORE_EACH() {
	arg_codec_context.opaque = &arg_decoding_context;
	arg_decoding_context.passthrough = false;
	return 0;
}

SUITE_CASE("get frame buffer size for video") {
	init_mock_function_with_return(av_image_get_buffer_size, 100);
	arg_codec_context.codec_type = AVMEDIA_TYPE_VIDEO;
	arg_codec_context.width = arg_width = 1920;
	arg_codec_context.height = arg_height = 1080;
	arg_codec_context.pix_fmt = arg_pix_format = AV_PIX_FMT_YUVA420P10BE;
	arg_decoding_context.align = arg_align = 8;

	CUE_ASSERT_EQ(av_get_buffer_size(arg_codec_context), 100);

	CUE_EXPECT_CALLED_ONCE(av_image_get_buffer_size);
	CUE_EXPECT_CALLED_WITH_INT(av_image_get_buffer_size, 1, arg_pix_format);
	CUE_EXPECT_CALLED_WITH_INT(av_image_get_buffer_size, 2, arg_width);
	CUE_EXPECT_CALLED_WITH_INT(av_image_get_buffer_size, 3, arg_height);
	CUE_EXPECT_CALLED_WITH_INT(av_image_get_buffer_size, 4, arg_align);
}

SUITE_CASE("get frame buffer size for audio") {
	init_mock_function_with_return(av_samples_get_buffer_size, 1000);
	arg_codec_context.codec_type = AVMEDIA_TYPE_AUDIO;
	arg_codec_context.channels = arg_channels = 8;
	arg_codec_context.sample_fmt = arg_sample_format = AV_SAMPLE_FMT_S64;
	arg_decoding_context.samples_size = arg_samples_size = 128;
	arg_decoding_context.align = arg_align = 1;

	CUE_ASSERT_EQ(av_get_buffer_size(arg_codec_context), 1000);

	CUE_EXPECT_CALLED_ONCE(av_samples_get_buffer_size);
	CUE_EXPECT_CALLED_WITH_PTR(av_samples_get_buffer_size, 1, nullptr);
	CUE_EXPECT_CALLED_WITH_INT(av_samples_get_buffer_size, 2, arg_channels);
	CUE_EXPECT_CALLED_WITH_INT(av_samples_get_buffer_size, 3, arg_samples_size);
	CUE_EXPECT_CALLED_WITH_INT(av_samples_get_buffer_size, 4, arg_sample_format);
	CUE_EXPECT_CALLED_WITH_INT(av_samples_get_buffer_size, 5, arg_align);
}

SUITE_CASE("get frame buffer size for passthrough audio") {
	init_mock_function_with_return(av_samples_get_buffer_size, 1000);
	arg_codec_context.codec_type = AVMEDIA_TYPE_AUDIO;
	arg_decoding_context.passthrough = true;
	arg_decoding_context.passthrough_format = arg_sample_format = AV_SAMPLE_FMT_S16;
	arg_decoding_context.passthrough_channels = arg_channels = 8;
	arg_decoding_context.samples_size = arg_samples_size = 128;
	arg_decoding_context.align = arg_align = 1;

	CUE_ASSERT_EQ(av_get_buffer_size(arg_codec_context), 1000);

	CUE_EXPECT_CALLED_ONCE(av_samples_get_buffer_size);
	CUE_EXPECT_CALLED_WITH_PTR(av_samples_get_buffer_size, 1, nullptr);
	CUE_EXPECT_CALLED_WITH_INT(av_samples_get_buffer_size, 2, arg_channels);
	CUE_EXPECT_CALLED_WITH_INT(av_samples_get_buffer_size, 3, arg_samples_size);
	CUE_EXPECT_CALLED_WITH_INT(av_samples_get_buffer_size, 4, arg_sample_format);
	CUE_EXPECT_CALLED_WITH_INT(av_samples_get_buffer_size, 5, arg_align);
}

SUITE_END(av_get_buffer_size_test);
