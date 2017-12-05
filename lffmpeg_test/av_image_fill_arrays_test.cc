#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include "stdexd/stdexd.h"
#include "mock_ffmpeg/mock_ffmpeg.h"
#include "lffmpeg/lffmpeg.h"

SUITE_START("av_image_fill_arrays_test");

static char arg_buffer[100];
static int arg_w, arg_h;
static enum AVPixelFormat arg_format;
mock_function_2(int, fill_image_action, uint8_t **, int *);

BEFORE_EACH() {
	init_subject("");
	arg_w = 100;
	arg_h = 50;
	arg_format = AV_PIX_FMT_YUV420P;
	init_mock_function(av_image_fill_arrays);
	init_mock_function(fill_image_action);
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUBJECT(int) {
	return av_image_fill_arrays(arg_w, arg_h, arg_format, arg_buffer, fill_image_action);
}

SUITE_CASE("fill arrays") {
	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(av_image_fill_arrays);

	CUE_EXPECT_CALLED_WITH_PTR(av_image_fill_arrays, 3, arg_buffer);
	CUE_EXPECT_CALLED_WITH_INT(av_image_fill_arrays, 4, arg_format);
	CUE_EXPECT_CALLED_WITH_INT(av_image_fill_arrays, 5, arg_w);
	CUE_EXPECT_CALLED_WITH_INT(av_image_fill_arrays, 6, arg_h);
	CUE_EXPECT_CALLED_WITH_INT(av_image_fill_arrays, 7, 64);

	CUE_EXPECT_CALLED_ONCE(fill_image_action);
}

SUITE_CASE("failed to fill") {
	init_mock_function_with_return(av_image_fill_arrays, -10);

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_EXPECT_NEVER_CALLED(fill_image_action);
	CUE_ASSERT_STDERR_EQ("Error[liblffmpeg]: -10\n");
}

SUITE_END(av_image_fill_arrays_test);

