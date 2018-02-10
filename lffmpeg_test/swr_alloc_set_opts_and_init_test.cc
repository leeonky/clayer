#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include "stdexd/stdexd.h"
#include "mock_ffmpeg/mock_ffmpeg.h"
#include "lffmpeg/lffmpeg.h"

static int64_t arg_in_layout, arg_out_layout;
static enum AVSampleFormat arg_in_format, arg_out_format;
static int arg_in_rate, arg_out_rate;
static SwrContext *ret_swr_context;

mock_function_1(int, swr_new_action, resample_context *);

int swr_new_action_ref(resample_context &context) {
	return swr_new_action(&context);
}

static int stub_av_get_bytes_per_sample(enum AVSampleFormat format) {
	switch(format) {
		case AV_SAMPLE_FMT_S16:
			return 2;
		case AV_SAMPLE_FMT_S32:
			return 4;
		default:
			return 0;
	}
}

static int stub_av_get_channel_layout_nb_channels(uint64_t layout) {
	switch(layout) {
		case AV_CH_LAYOUT_STEREO:
			return 2;
		case AV_CH_LAYOUT_3POINT1:
			return 4;
	}
	return 0;
}

SUITE_START("swr_alloc_set_opts_and_init_test");

BEFORE_EACH() {
	init_subject("");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;
	app_stderr = actxt.error_stream;
	arg_in_layout = AV_CH_LAYOUT_STEREO;
	arg_out_layout = AV_CH_LAYOUT_3POINT1;
	arg_in_format = AV_SAMPLE_FMT_S16;
	arg_out_format = AV_SAMPLE_FMT_S32;
	arg_in_rate = 44100;
	arg_out_rate = 96000;

	ret_swr_context = (SwrContext *)&ret_swr_context;

	init_mock_function_with_return(swr_alloc_set_opts, ret_swr_context);
	init_mock_function(swr_init);
	init_mock_function(swr_new_action);
	init_mock_function(swr_free);
	init_mock_function_with_function(av_get_bytes_per_sample, stub_av_get_bytes_per_sample);
	init_mock_function_with_function(av_get_channel_layout_nb_channels, stub_av_get_channel_layout_nb_channels);
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUBJECT(int) {
	return swr_alloc_set_opts_and_init(
			arg_in_layout, arg_in_format, arg_in_rate,
		       	arg_out_layout, arg_out_format, arg_out_rate, swr_new_action_ref);
}

SUITE_CASE("ffmpeg_frame_copy for video") {
	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_WITH_PTR(swr_alloc_set_opts, 1, NULL);
	CUE_EXPECT_CALLED_WITH_INT(swr_alloc_set_opts, 2, arg_out_layout);
	CUE_EXPECT_CALLED_WITH_INT(swr_alloc_set_opts, 3, arg_out_format);
	CUE_EXPECT_CALLED_WITH_INT(swr_alloc_set_opts, 4, arg_out_rate);
	CUE_EXPECT_CALLED_WITH_INT(swr_alloc_set_opts, 5, arg_in_layout);
	CUE_EXPECT_CALLED_WITH_INT(swr_alloc_set_opts, 6, arg_in_format);
	CUE_EXPECT_CALLED_WITH_INT(swr_alloc_set_opts, 7, arg_in_rate);
	CUE_EXPECT_CALLED_WITH_INT(swr_alloc_set_opts, 8, 0);
	CUE_EXPECT_CALLED_WITH_PTR(swr_alloc_set_opts, 9, NULL);

	CUE_EXPECT_CALLED_ONCE(swr_init);
	CUE_EXPECT_CALLED_WITH_PTR(swr_init, 1, ret_swr_context);

	CUE_EXPECT_CALLED_ONCE(swr_free);
}

int swr_new_action_assert(resample_context *context) {
	CUE_ASSERT_EQ(context->in_layout, arg_in_layout);
	CUE_ASSERT_EQ(context->in_rate, arg_in_rate);
	CUE_ASSERT_EQ(context->in_format, arg_in_format);
	CUE_ASSERT_EQ(context->in_sample_bytes, 2);
	CUE_ASSERT_EQ(context->in_channels, 2);
	CUE_ASSERT_EQ(context->out_layout, arg_out_layout);
	CUE_ASSERT_EQ(context->out_rate, arg_out_rate);
	CUE_ASSERT_EQ(context->out_format, arg_out_format);
	CUE_ASSERT_EQ(context->out_sample_bytes, 4);
	CUE_ASSERT_EQ(context->out_channels, 4);
	CUE_ASSERT_PTR_EQ(context->swr_context, ret_swr_context);
	return 0;
}

SUITE_CASE("checking resample_context fields") {
	init_mock_function_with_function(swr_new_action, swr_new_action_assert);

	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(swr_new_action);
}

SUITE_CASE("swr_alloc_set_opts failed") {
	init_mock_function_with_return(swr_alloc_set_opts, NULL);

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_EXPECT_NEVER_CALLED(swr_init);

	CUE_EXPECT_NEVER_CALLED(swr_new_action);

	CUE_EXPECT_NEVER_CALLED(swr_free);
	CUE_ASSERT_STDERR_EQ("Error[liblffmpeg]: swr_alloc_set_opts failed\n");
}

SUITE_CASE("swr_init failed") {
	init_mock_function_with_return(swr_init, -2);

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_EXPECT_NEVER_CALLED(swr_new_action);

	CUE_EXPECT_CALLED_ONCE(swr_free);

	CUE_ASSERT_STDERR_EQ("Error[liblffmpeg]: -2\n");
}

SUITE_END(swr_alloc_set_opts_and_init_test);
