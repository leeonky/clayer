#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include "stdexd/stdexd.h"
#include "mock_ffmpeg/mock_ffmpeg.h"
#include "lffmpeg/lffmpeg.h"

SUITE_START("swr_resample_size_test");

static int arg_src_size;
static int64_t arg_in_layout, arg_out_layout;
static enum AVSampleFormat arg_in_format, arg_out_format;
static int arg_in_rate, arg_out_rate;

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

BEFORE_EACH() {
	init_subject("");
	init_mock_function_with_function(av_get_bytes_per_sample, stub_av_get_bytes_per_sample);
	init_mock_function_with_function(av_get_channel_layout_nb_channels, stub_av_get_channel_layout_nb_channels);
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUBJECT(size_t) {
	return swr_resample_size(arg_src_size,
			arg_in_layout, arg_in_format, arg_in_rate,
		       	arg_out_layout, arg_out_format, arg_out_rate);
}

SUITE_CASE("work size by format layout and sample_rate") {
	arg_src_size = 1000;
	arg_in_layout = AV_CH_LAYOUT_STEREO;
	arg_out_layout = AV_CH_LAYOUT_3POINT1;
	arg_in_format = AV_SAMPLE_FMT_S16;
	arg_out_format = AV_SAMPLE_FMT_S32;
	arg_in_rate = 48000;
	arg_out_rate = 96000;

	CUE_ASSERT_EQ(CALL_SUBJECT(size_t), 8000);
}

SUITE_END(swr_resample_size_test);
