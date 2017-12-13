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

mock_function_1(int, swr_new_action, SwrContext *);

SUITE_START("swr_alloc_set_opts_and_init_test");

BEFORE_EACH() {
	init_subject("");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;
	app_stderr = actxt.error_stream;
	arg_in_layout = AV_CH_LAYOUT_2_1;
	arg_out_layout = AV_CH_LAYOUT_3POINT1;
	arg_in_format = AV_SAMPLE_FMT_S16;
	arg_out_format = AV_SAMPLE_FMT_FLTP;
	arg_in_rate = 44100;
	arg_out_rate = 96000;

	ret_swr_context = (SwrContext *)&ret_swr_context;

	init_mock_function_with_return(swr_alloc_set_opts, ret_swr_context);
	init_mock_function(swr_init);
	init_mock_function(swr_new_action);
	init_mock_function(swr_free);
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUBJECT(int) {
	return swr_alloc_set_opts_and_init(
			arg_in_layout, arg_in_format, arg_in_rate,
		       	arg_out_layout, arg_out_format, arg_out_rate, swr_new_action);
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

	CUE_EXPECT_CALLED_ONCE(swr_new_action);
	CUE_EXPECT_CALLED_WITH_PTR(swr_new_action, 1, ret_swr_context);

	CUE_EXPECT_CALLED_ONCE(swr_free);
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
