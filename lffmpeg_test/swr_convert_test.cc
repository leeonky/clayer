#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include "stdexd/stdexd.h"
#include "mock_ffmpeg/mock_ffmpeg.h"
#include "lffmpeg/lffmpeg.h"


SUITE_START("swr_convert_test");

static resample_context arg_context;
static struct SwrContext *arg_swr_context;
static void *arg_in_put, *arg_out_put;
static int arg_sample_size;

static enum AVSampleFormat arg_in_format, arg_out_format;

BEFORE_EACH() {
	init_subject("");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;
	app_stderr = actxt.error_stream;

	arg_swr_context = (SwrContext *)&arg_swr_context;

	arg_context.in_channels = 2;
	arg_context.in_format = arg_in_format = AV_SAMPLE_FMT_S16;
	arg_context.in_rate = 48000;

	arg_context.out_format = arg_out_format = AV_SAMPLE_FMT_S32;
	arg_context.out_channels = 4;
	arg_context.out_rate = 96000;

	arg_context.swr_context = arg_swr_context; 

	arg_in_put = &arg_in_put;
	arg_out_put = &arg_out_put;
	arg_sample_size = 1024;

	init_mock_function(av_samples_fill_arrays);
	init_mock_function(swr_convert);
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUBJECT(int) {
	return swr_convert(arg_context, arg_in_put, arg_sample_size, arg_out_put);
}

static int assert_av_samples_fill_arrays_2(uint8_t **datas, int *lines, const uint8_t *buf, int channels, int samples, enum AVSampleFormat format, int align) {
	datas[0] = (uint8_t *)4321;
	CUE_ASSERT_PTR_EQ(lines, nullptr);
	CUE_ASSERT_EQ(channels, 2);
	CUE_ASSERT_EQ(samples, arg_sample_size);
	CUE_ASSERT_EQ(format, arg_in_format);
	CUE_ASSERT_EQ(align, 1);
	CUE_ASSERT_PTR_EQ(buf, arg_in_put);
	return 1;
}

static int assert_av_samples_fill_arrays_1(uint8_t **datas, int *lines, const uint8_t *buf, int channels, int samples, enum AVSampleFormat format, int align) {
	datas[0] = (uint8_t *)1234;
	CUE_ASSERT_PTR_EQ(lines, nullptr);
	CUE_ASSERT_EQ(channels, 4);
	CUE_ASSERT_EQ(samples, 2*arg_sample_size);
	CUE_ASSERT_EQ(format, arg_out_format);
	CUE_ASSERT_EQ(align, 1);
	CUE_ASSERT_PTR_EQ(buf, arg_out_put);

	init_mock_function_with_function(av_samples_fill_arrays, assert_av_samples_fill_arrays_2);
	return 1;
}

static int assert_swr_convert(struct SwrContext *, uint8_t **out_buf, int, const uint8_t **in_buf, int) {
	CUE_ASSERT_EQ(((long)out_buf[0]), 1234);
	CUE_ASSERT_EQ(((long)in_buf[0]), 4321);
	return 2*arg_sample_size;
}

SUITE_CASE("fill input and output data to uint8_t ** and convert") {
	init_mock_function_with_function(av_samples_fill_arrays, assert_av_samples_fill_arrays_1);
	init_mock_function_with_function(swr_convert, assert_swr_convert);

	CUE_ASSERT_EQ(CALL_SUBJECT(int), 2*arg_sample_size);

	CUE_EXPECT_CALLED_ONCE(swr_convert);
	CUE_EXPECT_CALLED_WITH_PTR(swr_convert, 1, arg_swr_context);
	CUE_EXPECT_CALLED_WITH_INT(swr_convert, 3, 2*arg_sample_size);
	CUE_EXPECT_CALLED_WITH_INT(swr_convert, 5, arg_sample_size);
}

SUITE_CASE("av_samples_fill_arrays out buffer error") {
	init_mock_function_with_return(av_samples_fill_arrays, -1);

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_EXPECT_NEVER_CALLED(swr_convert);

	CUE_ASSERT_STDERR_EQ("Error[liblffmpeg]: failed to fill output samples to arrays\n");
}

static int stub_av_samples_fill_arrays_for_regist_input_error(uint8_t **, int *, const uint8_t *, int, int, enum AVSampleFormat, int ) {
	init_mock_function_with_return(av_samples_fill_arrays, -1);
	return 0;
}

SUITE_CASE("av_samples_fill_arrays input buffer error") {
	init_mock_function_with_function(av_samples_fill_arrays, stub_av_samples_fill_arrays_for_regist_input_error);

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_EXPECT_NEVER_CALLED(swr_convert);

	CUE_ASSERT_STDERR_EQ("Error[liblffmpeg]: failed to fill input samples to arrays\n");
}

SUITE_CASE("av_samples_fill_arrays input buffer error") {
	init_mock_function_with_return(swr_convert, -1);

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_ASSERT_STDERR_EQ("Error[liblffmpeg]: swr_convert failed\n");
}

SUITE_CASE("give warning when convert size and guess size different") {
	init_mock_function_with_return(swr_convert, 100);

	CUE_ASSERT_EQ(CALL_SUBJECT(int), 100);

	CUE_ASSERT_STDERR_EQ("Warning[liblffmpeg]: swr_convert expect 2048 but got 100\n");
}

SUITE_END(swr_convert_test);
