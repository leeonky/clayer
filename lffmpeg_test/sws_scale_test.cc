#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include "stdexd/stdexd.h"
#include "mock_ffmpeg/mock_ffmpeg.h"
#include "lffmpeg/lffmpeg.h"


SUITE_START("sws_scale_test");

static scale_context arg_context;
static struct SwsContext *arg_sws_context;
static void *arg_in_put, *arg_out_put;
static int64_t arg_in_w, arg_in_h, arg_out_w, arg_out_h;
static enum AVPixelFormat arg_in_format, arg_out_format;

BEFORE_EACH() {
	init_subject("");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;
	app_stderr = actxt.error_stream;

	arg_sws_context = (SwsContext *)&arg_sws_context;

	arg_context.in_w = arg_in_w = 100;
	arg_context.in_h = arg_in_h = 200;
	arg_context.in_format = arg_in_format = AV_PIX_FMT_YUV420P;

	arg_context.out_w = arg_out_w = 1000;
	arg_context.out_h = arg_out_h = 2000;
	arg_context.out_format = arg_out_format = AV_PIX_FMT_YUV444P;

	arg_in_put = &arg_in_put;
	arg_out_put = &arg_out_put;

	arg_context.sws_context = arg_sws_context; 

	init_mock_function(av_image_fill_arrays);
	init_mock_function(sws_scale);
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUBJECT(int) {
	return sws_scale(arg_context, arg_in_put, arg_out_put);
}

static int assert_av_image_fill_arrays_2(uint8_t **datas, int *lines, const uint8_t *buf, enum AVPixelFormat format, int w, int h, int align) {
	datas[0] = (uint8_t *)4321;
	lines[0] = 200;
	CUE_ASSERT_EQ(format, arg_in_format);
	CUE_ASSERT_EQ(w, arg_in_w);
	CUE_ASSERT_EQ(h, arg_in_h);
	CUE_ASSERT_EQ(align, VIDEO_ALIGN);
	CUE_ASSERT_PTR_EQ(buf, arg_in_put);
	return 1;
}

static int assert_av_image_fill_arrays_1(uint8_t **datas, int *lines, const uint8_t *buf, enum AVPixelFormat format, int w, int h, int align) {
	datas[0] = (uint8_t *)1234;
	lines[0] = 100;
	CUE_ASSERT_EQ(format, arg_out_format);
	CUE_ASSERT_EQ(w, arg_out_w);
	CUE_ASSERT_EQ(h, arg_out_h);
	CUE_ASSERT_EQ(align, VIDEO_ALIGN);
	CUE_ASSERT_PTR_EQ(buf, arg_out_put);

	init_mock_function_with_function(av_image_fill_arrays, assert_av_image_fill_arrays_2);
	return 1;
}
static int assert_sws_convert(struct SwsContext *, const uint8_t * const *in_buf, const int *in_lines, int, int, uint8_t * const *out_buf, const int *out_lines) {
	CUE_ASSERT_EQ(((long)out_buf[0]), 1234);
	CUE_ASSERT_EQ(out_lines[0], 100);
	CUE_ASSERT_EQ(((long)in_buf[0]), 4321);
	CUE_ASSERT_EQ(in_lines[0], 200);
	return 100;
}

SUITE_CASE("fill input and output data to uint8_t ** and convert") {
	init_mock_function_with_function(av_image_fill_arrays, assert_av_image_fill_arrays_1);
	init_mock_function_with_function(sws_scale, assert_sws_convert);

	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(sws_scale);
	CUE_EXPECT_CALLED_WITH_PTR(sws_scale, 1, arg_sws_context);
	CUE_EXPECT_CALLED_WITH_INT(sws_scale, 4, 0);
	CUE_EXPECT_CALLED_WITH_INT(sws_scale, 5, arg_in_h);

	CUE_EXPECT_CALLED_ONCE(av_image_fill_arrays);
}

SUITE_END(sws_scale_test);

