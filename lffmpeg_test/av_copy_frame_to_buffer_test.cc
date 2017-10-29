#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include "stdexd/stdexd.h"
#include "mock_ffmpeg/mock_ffmpeg.h"
#include "lffmpeg/lffmpeg.h"

SUITE_START("av_copy_frame_to_buffer_test");

static stub_decoding_context arg_decoding_context;
static AVStream arg_av_stream;

static AVFrame arg_av_frame;
static int arg_width, arg_height, arg_align, arg_channels, arg_nb_samples;
static int arg_format;

static char frame_buffer[100];

BEFORE_EACH() {
	init_subject("");

	static AVCodecParameters codec_parameter;
	arg_av_stream.codecpar = &codec_parameter;
	arg_decoding_context.av_stream = &arg_av_stream;
	arg_av_frame.opaque = &arg_decoding_context;

	init_mock_function(av_image_copy_to_buffer);
	init_mock_function(av_samples_fill_arrays);
	init_mock_function(av_samples_copy);
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUBJECT(int) {
	return av_copy_frame_to_buffer(arg_av_frame, frame_buffer, sizeof(frame_buffer));
}

SUITE_CASE("ffmpeg_frame_copy for video") {
	arg_decoding_context.align = arg_align = 64;
	arg_decoding_context.av_stream->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
	arg_av_frame.width = arg_width = 1920;
	arg_av_frame.height = arg_height = 1080;
	arg_av_frame.format = arg_format = AV_PIX_FMT_NV12;

	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(av_image_copy_to_buffer);
	CUE_EXPECT_CALLED_WITH_PTR(av_image_copy_to_buffer, 1, frame_buffer);
	CUE_EXPECT_CALLED_WITH_INT(av_image_copy_to_buffer, 2, sizeof(frame_buffer));
	CUE_EXPECT_CALLED_WITH_PTR(av_image_copy_to_buffer, 3, arg_av_frame.data);
	CUE_EXPECT_CALLED_WITH_PTR(av_image_copy_to_buffer, 4, arg_av_frame.linesize);
	CUE_EXPECT_CALLED_WITH_INT(av_image_copy_to_buffer, 5, arg_format);
	CUE_EXPECT_CALLED_WITH_INT(av_image_copy_to_buffer, 6, arg_width);
	CUE_EXPECT_CALLED_WITH_INT(av_image_copy_to_buffer, 7, arg_height);
	CUE_EXPECT_CALLED_WITH_INT(av_image_copy_to_buffer, 8, 64);
}

SUITE_CASE("failed to copy") {
	arg_decoding_context.av_stream->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
	init_mock_function_with_return(av_image_copy_to_buffer, -10);

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_ASSERT_STDERR_EQ("Error[liblffmpeg]: -10\n");
}

SUITE_CASE("copy for audio, depents on samples_size, channels, format") {
	arg_decoding_context.align = arg_align = 1;
	arg_decoding_context.av_stream->codecpar->codec_type = AVMEDIA_TYPE_AUDIO;
	arg_av_frame.channels = arg_channels = 6;
	arg_av_frame.nb_samples = arg_nb_samples = 128;
	arg_av_frame.format = arg_format = AV_SAMPLE_FMT_NB;

	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(av_samples_fill_arrays);
	CUE_EXPECT_CALLED_WITH_PTR(av_samples_fill_arrays, 2, nullptr);
	CUE_EXPECT_CALLED_WITH_PTR(av_samples_fill_arrays, 3, frame_buffer);
	CUE_EXPECT_CALLED_WITH_INT(av_samples_fill_arrays, 4, arg_channels);
	CUE_EXPECT_CALLED_WITH_INT(av_samples_fill_arrays, 5, arg_nb_samples);
	CUE_EXPECT_CALLED_WITH_INT(av_samples_fill_arrays, 6, arg_format);
	CUE_EXPECT_CALLED_WITH_INT(av_samples_fill_arrays, 7, arg_align);

	CUE_EXPECT_CALLED_ONCE(av_samples_copy);
	CUE_EXPECT_CALLED_WITH_PTR(av_samples_copy, 2, arg_av_frame.data);
	CUE_EXPECT_CALLED_WITH_INT(av_samples_copy, 3, 0);
	CUE_EXPECT_CALLED_WITH_INT(av_samples_copy, 4, 0);
	CUE_EXPECT_CALLED_WITH_INT(av_samples_copy, 5, arg_nb_samples);
	CUE_EXPECT_CALLED_WITH_INT(av_samples_copy, 6, arg_channels);
	CUE_EXPECT_CALLED_WITH_INT(av_samples_copy, 7, arg_format);

	CUE_ASSERT_PTR_EQ(params_of(av_samples_fill_arrays, 1), params_of(av_samples_copy, 1));
}

SUITE_END(av_copy_frame_to_buffer_test);
