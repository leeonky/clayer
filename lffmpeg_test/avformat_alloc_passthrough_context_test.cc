#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include "stdexd/stdexd.h"
#include "mock_ffmpeg/mock_ffmpeg.h"
#include "lffmpeg/lffmpeg.h"

SUITE_START("avformat_alloc_passthrough_context_test");

static AVCodecContext arg_av_codec_context;
static stub_decoding_context arg_decoding_context;

static AVFormatContext ret_format_context;
static int stub_avformat_alloc_output_context2(AVFormatContext **ps, AVOutputFormat *, const char *, const char *) {
	*ps = &ret_format_context;
	return 0;
}

mock_function_1(int, avformat_alloc_action, AVFormatContext *);

static int avformat_alloc_action_ref(AVFormatContext &av_format_context) {
	return avformat_alloc_action(&av_format_context);
}

static AVIOContext ret_avio_context;
static void *ret_buffer;
static void *stub_av_malloc(size_t) {
	return ret_buffer;
}

mock_void_function_2(pull_data_handler, void *, int);

static int arg_buffer_size;
static void *ret_data_buffer;
static int ret_data_size;
static int ret_len_per_sample;
static enum AVSampleFormat arg_passthrough_format;
static AVStream *arg_av_stream;

BEFORE_EACH() {
	init_subject("");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;
	app_stderr = actxt.error_stream;

	arg_av_stream = (AVStream *)&arg_av_stream;
	arg_av_codec_context.opaque = &arg_decoding_context;
	arg_av_codec_context.codec_type = AVMEDIA_TYPE_AUDIO;
	arg_decoding_context.passthrough_dts_rate = 0;
	arg_decoding_context.passthrough_format = arg_passthrough_format = AV_SAMPLE_FMT_S32;
	arg_decoding_context.av_stream = arg_av_stream;

	ret_buffer = &ret_buffer;
	ret_data_buffer = &ret_data_buffer;
	ret_data_size = 10000;
	ret_len_per_sample = 30;


	init_mock_function_with_function(avformat_alloc_output_context2, stub_avformat_alloc_output_context2);
	init_mock_function_with_return(avio_alloc_context, &ret_avio_context);
	init_mock_function_with_function(av_malloc, stub_av_malloc);
	init_mock_function(av_opt_set_int);
	init_mock_function(av_free);
	init_mock_function(avio_context_free);
	init_mock_function(avformat_free_context);
	init_mock_function_with_return(av_samples_get_buffer_size, arg_buffer_size = 1000);

	init_mock_function_with_return(av_get_bytes_per_sample, ret_len_per_sample);

	init_mock_function(avformat_alloc_action);
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUBJECT(int) {
	return avformat_alloc_passthrough_context(arg_av_codec_context, avformat_alloc_action_ref, pull_data_handler);
}

static void assert_avio_context_free(AVIOContext **s) {
	CUE_ASSERT_PTR_EQ(*s, &ret_avio_context);
}

SUITE_CASE("create context and call action") {
	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(avformat_alloc_output_context2);
	CUE_EXPECT_CALLED_WITH_PTR(avformat_alloc_output_context2, 2, nullptr);
	CUE_EXPECT_CALLED_WITH_STRING(avformat_alloc_output_context2, 3, "spdif");
	CUE_EXPECT_CALLED_WITH_STRING(avformat_alloc_output_context2, 4, "");

	CUE_EXPECT_CALLED_ONCE(avformat_alloc_action);
	CUE_EXPECT_CALLED_WITH_PTR(avformat_alloc_action, 1, &ret_format_context);

	CUE_EXPECT_CALLED_ONCE(avformat_free_context);
	CUE_EXPECT_CALLED_WITH_PTR(avformat_free_context, 1, &ret_format_context);
}

static AVIOContext *assert_for_trigger_io_event(unsigned char *, int, int, void *opaque, avio_read_write_t, avio_read_write_t write_packet, avio_seek_t) {
	init_mock_function(pull_data_handler);

	int res = write_packet(opaque, (uint8_t *)ret_data_buffer, ret_data_size);

	CUE_EXPECT_CALLED_ONCE(pull_data_handler);

	CUE_EXPECT_CALLED_WITH_PTR(pull_data_handler, 1, ret_data_buffer);
	CUE_EXPECT_CALLED_WITH_INT(pull_data_handler, 2, ret_data_size/ret_len_per_sample);
	CUE_ASSERT_EQ(res, ret_data_size/ret_len_per_sample*ret_len_per_sample);

	CUE_EXPECT_CALLED_ONCE(av_get_bytes_per_sample);
	CUE_EXPECT_CALLED_WITH_INT(av_get_bytes_per_sample, 1, arg_passthrough_format);
	return &ret_avio_context;
}

SUITE_CASE("create avio context with right buffer and io event") {
	init_mock_function_with_function(avio_alloc_context, assert_for_trigger_io_event);

	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(av_malloc);
	CUE_EXPECT_CALLED_WITH_INT(av_malloc, 1, arg_buffer_size);

	CUE_EXPECT_CALLED_ONCE(avio_alloc_context);
	CUE_EXPECT_CALLED_WITH_PTR(avio_alloc_context, 1, ret_buffer);
	CUE_EXPECT_CALLED_WITH_INT(avio_alloc_context, 2, arg_buffer_size);
	CUE_EXPECT_CALLED_WITH_INT(avio_alloc_context, 3, 0);
	CUE_EXPECT_CALLED_WITH_PTR(avio_alloc_context, 5, nullptr);
	CUE_EXPECT_CALLED_WITH_PTR(avio_alloc_context, 7, nullptr);

	CUE_ASSERT_PTR_EQ(ret_format_context.pb, &ret_avio_context);

	CUE_EXPECT_CALLED_ONCE(av_free);
	CUE_EXPECT_CALLED_WITH_PTR(av_free, 1, ret_buffer);

	CUE_EXPECT_CALLED_ONCE(avio_context_free);
}

SUITE_CASE("should assign stream to avformat context") {
	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_ASSERT_PTR_EQ(ret_format_context.streams[0], arg_av_stream);
}

SUITE_CASE("should set dtsrate for dts hd") {
	arg_decoding_context.passthrough_dts_rate = 1000;

	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(av_opt_set_int);
	CUE_EXPECT_CALLED_WITH_PTR(av_opt_set_int, 1, ret_format_context.priv_data);
	CUE_EXPECT_CALLED_WITH_STRING(av_opt_set_int, 2, "dtshd_rate");
	CUE_EXPECT_CALLED_WITH_INT(av_opt_set_int, 3, 1000);
	CUE_EXPECT_CALLED_WITH_INT(av_opt_set_int, 4, 0);
}

SUITE_CASE("should not set dtsrate for non dts hd") {
	arg_decoding_context.passthrough_dts_rate = 0;

	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_NEVER_CALLED(av_opt_set_int);
}

SUITE_CASE("allocate format context failed") {
	init_mock_function_with_return(avformat_alloc_output_context2, -100);

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_EXPECT_NEVER_CALLED(av_malloc);
	CUE_EXPECT_NEVER_CALLED(avio_alloc_context);
	CUE_EXPECT_NEVER_CALLED(avformat_alloc_action);
	CUE_EXPECT_NEVER_CALLED(av_free);
	CUE_EXPECT_NEVER_CALLED(avio_context_free);
	CUE_EXPECT_NEVER_CALLED(avformat_free_context);

	CUE_ASSERT_STDERR_EQ("Error[liblffmpeg]: -100\n");
}

SUITE_CASE("allocate format context failed") {
	init_mock_function_with_return(av_malloc, NULL);

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_EXPECT_NEVER_CALLED(avio_alloc_context);
	CUE_EXPECT_NEVER_CALLED(avformat_alloc_action);
	CUE_EXPECT_NEVER_CALLED(av_free);
	CUE_EXPECT_NEVER_CALLED(avio_context_free);
	CUE_EXPECT_CALLED_ONCE(avformat_free_context);

	CUE_ASSERT_STDERR_EQ("Error[liblffmpeg]: alloc buffer failed\n");
}

SUITE_CASE("avio_alloc_context failed") {
	init_mock_function_with_return(avio_alloc_context, NULL);

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_EXPECT_NEVER_CALLED(avformat_alloc_action);
	CUE_EXPECT_NEVER_CALLED(avio_context_free);
	CUE_EXPECT_CALLED_ONCE(av_free);
	CUE_EXPECT_CALLED_ONCE(avformat_free_context);

	CUE_ASSERT_STDERR_EQ("Error[liblffmpeg]: avio_alloc_context failed\n");
}

SUITE_END(avformat_alloc_passthrough_context_test)

