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

mock_function_1(int, avcodec_open_action, AVCodecContext *);

static int avcodec_open_action_ref(AVCodecContext &av_codec_context) {
	return avcodec_open_action(&av_codec_context);
}

BEFORE_EACH() {
	init_subject("");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;
	app_stderr = actxt.error_stream;

	init_mock_function_with_return(avcodec_find_decoder, &ret_codec);
	init_mock_function_with_return(avcodec_alloc_context3, &ret_codec_context);
	init_mock_function(avcodec_parameters_to_context);
	init_mock_function(avcodec_open2);
	init_mock_function(avcodec_close);
	init_mock_function(avcodec_free_context);
	init_mock_function(avcodec_open_action);

	arg_av_stream.codecpar = &arg_codec_parameters;
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUBJECT(int) {
	return avcodec_open(arg_av_stream, avcodec_open_action_ref);
}

SUITE_CASE("should open and close stream's decoder") {
	arg_codec_parameters.codec_id = arg_codec_id = (AVCodecID)100;

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
	CUE_EXPECT_NEVER_CALLED(avcodec_open_action);
	CUE_EXPECT_NEVER_CALLED(avcodec_close);
	CUE_EXPECT_CALLED_ONCE(avcodec_free_context);

	CUE_ASSERT_STDERR_EQ("Error[liblffmpeg]: -100\n");
}

SUITE_CASE("open decoder failed") {
	init_mock_function_with_return(avcodec_open2, -200)

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_EXPECT_NEVER_CALLED(avcodec_open_action);
	CUE_EXPECT_NEVER_CALLED(avcodec_close);
	CUE_EXPECT_CALLED_ONCE(avcodec_free_context);

	CUE_ASSERT_STDERR_EQ("Error[liblffmpeg]: -200\n");
}

SUITE_END(avcodec_open_test);
