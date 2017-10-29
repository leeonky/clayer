#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include "stdexd/stdexd.h"
#include "mock_ffmpeg/mock_ffmpeg.h"
#include "lffmpeg/lffmpeg.h"

SUITE_START("av_new_frame_test");

static AVFrame ret_av_frame;

mock_function_1(int, av_new_frame_action, AVFrame *);

static int av_new_frame_action_ref(AVFrame &av_frame) {
	return av_new_frame_action(&av_frame);
}

BEFORE_EACH() {
	init_subject("");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;
	app_stderr = actxt.error_stream;

	init_mock_function_with_return(av_frame_alloc, &ret_av_frame);
	init_mock_function(av_frame_free);
	init_mock_function(av_new_frame_action);
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUBJECT(int) {
	return av_new_frame(av_new_frame_action_ref);
}

static int av_new_frame_action_assert(AVFrame *av_frame) {
	CUE_ASSERT_PTR_EQ(av_frame, &ret_av_frame);
	return 0;
}

SUITE_CASE("alloc AVFrame") {
	init_mock_function_with_function(av_new_frame_action, av_new_frame_action_assert);

	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(av_frame_alloc);

	CUE_EXPECT_CALLED_ONCE(av_new_frame_action);

	CUE_EXPECT_CALLED_ONCE(av_frame_free);
}

SUITE_CASE("failed to alloc AVFrame") {
	init_mock_function_with_return(av_frame_alloc, nullptr);

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_EXPECT_NEVER_CALLED(av_new_frame_action);

	CUE_EXPECT_NEVER_CALLED(av_frame_free);

	CUE_ASSERT_STDERR_EQ("Error[liblffmpeg]: failed to alloc AVFrame\n");
}

SUITE_END(av_new_frame_test);

