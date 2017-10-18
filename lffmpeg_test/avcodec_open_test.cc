#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include "stdexd/stdexd.h"
#include "mock_ffmpeg/mock_ffmpeg.h"
#include "lffmpeg/lffmpeg.h"

SUITE_START("avcodec_open_test");

static AVStream arg_av_stream;

static AVCodecID arg_codec_id;
static AVCodec ret_codec;
static AVCodecContext ret_codec_context;
static AVCodecParameters arg_codec_parameters;

static AVPacket *ret_av_packet;
static AVFrame ret_working_av_frame, ret_decoded_av_frame;

mock_function_1(int, avcodec_open_action, AVCodecContext *);

static int avcodec_open_action_ref(AVCodecContext &av_codec_context) {
	return avcodec_open_action(&av_codec_context);
}
static int av_frame_alloc_called_times;

BEFORE_EACH() {
	init_subject("");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;
	app_stderr = actxt.error_stream;

	av_frame_alloc_called_times = 0;

	init_mock_function_with_return(avcodec_find_decoder, &ret_codec);
	init_mock_function_with_return(avcodec_alloc_context3, &ret_codec_context);
	init_mock_function(avcodec_parameters_to_context);
	init_mock_function(avcodec_open2);
	init_mock_function(avcodec_close);
	init_mock_function(avcodec_free_context);
	init_mock_function(avcodec_open_action);
	init_mock_function(av_init_packet);
	init_mock_function(av_packet_unref);
	init_mock_function(av_frame_alloc);
	init_mock_function(av_frame_free);

	arg_av_stream.codecpar = &arg_codec_parameters;
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUBJECT(int) {
	return avcodec_open(arg_av_stream, avcodec_open_action_ref);
}

static int avcodec_open_action_assert(AVCodecContext *av_codec_context) {
	stub_decoding_context *context = static_cast<stub_decoding_context *>(av_codec_context->opaque);
	ret_av_packet = context->av_packet;
	CUE_ASSERT_PTR_EQ(context->working_av_frame, &ret_working_av_frame);
	CUE_ASSERT_PTR_EQ(context->decoded_av_frame, &ret_decoded_av_frame);
	return 0;
}

static AVFrame *stub_av_frame_alloc() {
	av_frame_alloc_called_times ++;
	if(1 == av_frame_alloc_called_times)
		return &ret_working_av_frame;
	else
		return &ret_decoded_av_frame;
}

SUITE_CASE("should open and close stream's decoder") {
	arg_codec_parameters.codec_id = arg_codec_id = (AVCodecID)100;
	init_mock_function_with_function(avcodec_open_action, avcodec_open_action_assert);
	av_frame_alloc_called_times = 0;
	init_mock_function_with_function(av_frame_alloc, stub_av_frame_alloc);

	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(avcodec_find_decoder);
	CUE_EXPECT_CALLED_WITH_INT(avcodec_find_decoder, 1, arg_codec_id);

	CUE_EXPECT_CALLED_ONCE(avcodec_alloc_context3);
	CUE_EXPECT_CALLED_WITH_PTR(avcodec_alloc_context3, 1, &ret_codec);

	CUE_EXPECT_CALLED_ONCE(avcodec_parameters_to_context);
	CUE_EXPECT_CALLED_WITH_PTR(avcodec_parameters_to_context, 1, &ret_codec_context);
	CUE_EXPECT_CALLED_WITH_PTR(avcodec_parameters_to_context, 2, &arg_codec_parameters);

	CUE_EXPECT_CALLED_ONCE(avcodec_open2);
	CUE_EXPECT_CALLED_WITH_PTR(avcodec_open2, 1, &ret_codec_context);
	CUE_EXPECT_CALLED_WITH_PTR(avcodec_open2, 2, &ret_codec);

	CUE_EXPECT_CALLED_ONCE(av_init_packet);
	CUE_EXPECT_CALLED_WITH_PTR(av_init_packet, 1, ret_av_packet);

	CUE_EXPECT_CALLED_TIMES(av_frame_alloc, 2);

	CUE_EXPECT_CALLED_ONCE(avcodec_open_action);
	CUE_EXPECT_CALLED_WITH_PTR(avcodec_open_action, 1, &ret_codec_context);

	CUE_EXPECT_CALLED_ONCE(avcodec_close);
	CUE_EXPECT_CALLED_WITH_PTR(avcodec_close, 1, &ret_codec_context);

	CUE_EXPECT_CALLED_ONCE(avcodec_free_context);
}

SUITE_CASE("failed to find decoder") {
	init_mock_function_with_return(avcodec_find_decoder, NULL);

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_EXPECT_NEVER_CALLED(avcodec_alloc_context3);
	CUE_EXPECT_NEVER_CALLED(avcodec_parameters_to_context);
	CUE_EXPECT_NEVER_CALLED(avcodec_open2);
	CUE_EXPECT_NEVER_CALLED(av_init_packet);
	CUE_EXPECT_NEVER_CALLED(av_frame_alloc);
	CUE_EXPECT_NEVER_CALLED(avcodec_open_action);
	CUE_EXPECT_NEVER_CALLED(avcodec_close);
	CUE_EXPECT_NEVER_CALLED(avcodec_free_context);

	CUE_ASSERT_STDERR_EQ("Error[liblffmpeg]: failed to find decoder\n");
}

SUITE_CASE("failed to alloc codec_context") {
	init_mock_function_with_return(avcodec_alloc_context3, NULL);

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_EXPECT_NEVER_CALLED(avcodec_parameters_to_context);
	CUE_EXPECT_NEVER_CALLED(avcodec_open2);
	CUE_EXPECT_NEVER_CALLED(avcodec_open_action);
	CUE_EXPECT_NEVER_CALLED(avcodec_close);
	CUE_EXPECT_NEVER_CALLED(avcodec_free_context);

	CUE_ASSERT_STDERR_EQ("Error[liblffmpeg]: failed to alloc AVCodecContext\n");
}

SUITE_CASE("failed to avcodec_parameters_to_context") {
	init_mock_function_with_return(avcodec_parameters_to_context, -100);

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_EXPECT_NEVER_CALLED(avcodec_open2);
	CUE_EXPECT_NEVER_CALLED(av_init_packet);
	CUE_EXPECT_NEVER_CALLED(av_frame_alloc);
	CUE_EXPECT_NEVER_CALLED(avcodec_open_action);
	CUE_EXPECT_NEVER_CALLED(avcodec_close);
	CUE_EXPECT_CALLED_ONCE(avcodec_free_context);

	CUE_ASSERT_STDERR_EQ("Error[liblffmpeg]: -100\n");
}

SUITE_CASE("open decoder failed") {
	init_mock_function_with_return(avcodec_open2, -200)

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_EXPECT_NEVER_CALLED(av_init_packet);
	CUE_EXPECT_NEVER_CALLED(av_frame_alloc);
	CUE_EXPECT_NEVER_CALLED(avcodec_open_action);
	CUE_EXPECT_NEVER_CALLED(avcodec_close);
	CUE_EXPECT_CALLED_ONCE(avcodec_free_context);

	CUE_ASSERT_STDERR_EQ("Error[liblffmpeg]: -200\n");
}

static int avcodec_open_action_assert_video(AVCodecContext *av_codec_context) {
	stub_decoding_context *context = static_cast<stub_decoding_context *>(av_codec_context->opaque);
	CUE_ASSERT_EQ(context->align, 64);
	return 0;
}

SUITE_CASE("stub_decoding_context args for video decoder") {
	arg_codec_parameters.codec_type = AVMEDIA_TYPE_VIDEO;
	init_mock_function_with_function(avcodec_open_action, avcodec_open_action_assert_video);
	av_frame_alloc_called_times = 0;
	init_mock_function_with_function(av_frame_alloc, stub_av_frame_alloc);

	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(avcodec_open_action);
}

static int avcodec_open_action_assert_audio(AVCodecContext *av_codec_context) {
	stub_decoding_context *context = static_cast<stub_decoding_context *>(av_codec_context->opaque);
	CUE_ASSERT_EQ(context->align, 1);
	return 0;
}

SUITE_CASE("decoding_context args for audio decoder") {
	arg_codec_parameters.codec_type = AVMEDIA_TYPE_AUDIO;
	init_mock_function_with_function(avcodec_open_action, avcodec_open_action_assert_audio);
	av_frame_alloc_called_times = 0;
	init_mock_function_with_function(av_frame_alloc, stub_av_frame_alloc);

	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(avcodec_open_action);
}

SUITE_END(avcodec_open_test);
