#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include <memory>
#include "stdexd/stdexd.h"
#include "media/media.h"
#include "mock_ffmpeg/mock_ffmpeg.h"

SUITE_START("resampler_params_test");

static int64_t ret_layout;
static enum AVSampleFormat ret_format;

static int stub_av_get_channel_layout_nb_channels(uint64_t layout) {
	switch(layout) {
		case AV_CH_LAYOUT_STEREO:
			return 2;
		case AV_CH_LAYOUT_5POINT1:
			return 6;
		case AV_CH_LAYOUT_7POINT1:
		case AV_CH_LAYOUT_7POINT1_WIDE:
		case AV_CH_LAYOUT_7POINT1_WIDE_BACK:
			return 8;
	}
	return 0;
}

static enum AVSampleFormat stub_av_get_sample_fmt(const char *name) {
	if(!strcmp(name, "fltp"))
		return AV_SAMPLE_FMT_FLTP;
	else if(!strcmp(name, "flt"))
		return AV_SAMPLE_FMT_FLT;
	else if(!strcmp(name, "s32"))
		return AV_SAMPLE_FMT_S32;
	else if(!strcmp(name, "s16"))
		return AV_SAMPLE_FMT_S16;
	else if(!strcmp(name, "s32p"))
		return AV_SAMPLE_FMT_S32P;
	else if(!strcmp(name, "s16p"))
		return AV_SAMPLE_FMT_S16P;
	else
		return AV_SAMPLE_FMT_S64;
}

static enum AVSampleFormat stub_av_get_packed_sample_fmt(enum AVSampleFormat format) {
	if(format == AV_SAMPLE_FMT_FLTP)
		return AV_SAMPLE_FMT_FLT;
	else if(format == AV_SAMPLE_FMT_S64P)
		return AV_SAMPLE_FMT_S64;
	else if(format == AV_SAMPLE_FMT_S32P)
		return AV_SAMPLE_FMT_S32;
	else if(format == AV_SAMPLE_FMT_S16P)
		return AV_SAMPLE_FMT_S16;
	else
		return format;
}

static enum AVSampleFormat stub_av_get_planar_sample_fmt(enum AVSampleFormat format) {
	if(format == AV_SAMPLE_FMT_FLT)
		return AV_SAMPLE_FMT_FLTP;
	else if(format == AV_SAMPLE_FMT_S32)
		return AV_SAMPLE_FMT_S32P;
	else if(format == AV_SAMPLE_FMT_S16)
		return AV_SAMPLE_FMT_S16P;
	else
		return format;
}

BEFORE_EACH() {
	init_subject("");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;
	app_stderr = actxt.error_stream;

	init_mock_function_with_return(av_get_channel_layout, ret_layout);
	init_mock_function_with_return(av_get_sample_fmt, ret_format);
	init_mock_function_with_function(av_get_channel_layout_nb_channels, stub_av_get_channel_layout_nb_channels);
	init_mock_function_with_function(av_get_sample_fmt, stub_av_get_sample_fmt);

	init_mock_function_with_function(av_get_packed_sample_fmt, stub_av_get_packed_sample_fmt)
	init_mock_function_with_function(av_get_planar_sample_fmt, stub_av_get_planar_sample_fmt)
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUITE_CASE("nothing to do for layout") {
	CUE_ASSERT_EQ(analyze_channel_layout(AV_CH_LAYOUT_3POINT1, ""), AV_CH_LAYOUT_3POINT1);
}

SUITE_CASE("set layout") {
	init_mock_function_with_return(av_get_channel_layout, AV_CH_LAYOUT_5POINT1);

	CUE_ASSERT_EQ(analyze_channel_layout(AV_CH_LAYOUT_3POINT1, "5.1(side)"), AV_CH_LAYOUT_5POINT1);

	CUE_EXPECT_CALLED_ONCE(av_get_channel_layout);
	CUE_EXPECT_CALLED_WITH_STRING(av_get_channel_layout, 1, "5.1(side)");
}

SUITE_CASE("set max channels max2.1") {
	CUE_ASSERT_EQ(analyze_channel_layout(AV_CH_LAYOUT_5POINT1, "max2.1"), AV_CH_LAYOUT_2POINT1);
	CUE_ASSERT_EQ(analyze_channel_layout(AV_CH_LAYOUT_STEREO, "max2.1"), AV_CH_LAYOUT_STEREO);
}

SUITE_CASE("set max channels max5.1") {
	CUE_ASSERT_EQ(analyze_channel_layout(AV_CH_LAYOUT_5POINT1, "max5.1"), AV_CH_LAYOUT_5POINT1);
	CUE_ASSERT_EQ(analyze_channel_layout(AV_CH_LAYOUT_7POINT1, "max5.1"), AV_CH_LAYOUT_5POINT1);
	CUE_ASSERT_EQ(analyze_channel_layout(AV_CH_LAYOUT_7POINT1_WIDE, "max5.1"), AV_CH_LAYOUT_5POINT1);
	CUE_ASSERT_EQ(analyze_channel_layout(AV_CH_LAYOUT_7POINT1_WIDE_BACK, "max5.1"), AV_CH_LAYOUT_5POINT1_BACK);
}

SUITE_CASE("nothing to do for rate") {
	CUE_ASSERT_EQ(analyze_sample_rate(44100, ""), 44100);
}

SUITE_CASE("set rate") {
	CUE_ASSERT_EQ(analyze_sample_rate(44100, "48000"), 48000);
}

SUITE_CASE("set max channels max96000") {
	CUE_ASSERT_EQ(analyze_sample_rate(44100, "max96000"), 44100);
	CUE_ASSERT_EQ(analyze_sample_rate(115200, "max96000"), 96000);
}

SUITE_CASE("nothing to do for format") {
	CUE_ASSERT_EQ(analyze_sample_format(AV_SAMPLE_FMT_S32, ""), AV_SAMPLE_FMT_S32);
}

SUITE_CASE("set format") {
	CUE_ASSERT_EQ(analyze_sample_format(AV_SAMPLE_FMT_S32, "s16"), AV_SAMPLE_FMT_S16);
}

SUITE_CASE("packed and planar") {
	CUE_ASSERT_EQ(analyze_sample_format(AV_SAMPLE_FMT_S32, "plan"), AV_SAMPLE_FMT_S32P);
	CUE_ASSERT_EQ(analyze_sample_format(AV_SAMPLE_FMT_S32P, "pack"), AV_SAMPLE_FMT_S32);
}

SUITE_CASE("flt and double to int") {
	CUE_ASSERT_EQ(analyze_sample_format(AV_SAMPLE_FMT_FLTP, "int"), AV_SAMPLE_FMT_S64P);
	CUE_ASSERT_EQ(analyze_sample_format(AV_SAMPLE_FMT_FLT, "int"), AV_SAMPLE_FMT_S64);
	CUE_ASSERT_EQ(analyze_sample_format(AV_SAMPLE_FMT_DBLP, "int"), AV_SAMPLE_FMT_S64P);
	CUE_ASSERT_EQ(analyze_sample_format(AV_SAMPLE_FMT_DBL, "int"), AV_SAMPLE_FMT_S64);
}

SUITE_CASE("flt32 and flt64") {
	CUE_ASSERT_EQ(analyze_sample_format(AV_SAMPLE_FMT_FLTP, "flt64"), AV_SAMPLE_FMT_DBLP);
	CUE_ASSERT_EQ(analyze_sample_format(AV_SAMPLE_FMT_FLT, "flt64"), AV_SAMPLE_FMT_DBL);
	CUE_ASSERT_EQ(analyze_sample_format(AV_SAMPLE_FMT_DBLP, "flt32"), AV_SAMPLE_FMT_FLTP);
	CUE_ASSERT_EQ(analyze_sample_format(AV_SAMPLE_FMT_DBL, "flt32"), AV_SAMPLE_FMT_FLT);
}

SUITE_CASE("maxbit") {
	CUE_ASSERT_EQ(analyze_sample_format(AV_SAMPLE_FMT_S64, "maxbit32"), AV_SAMPLE_FMT_S32);
	CUE_ASSERT_EQ(analyze_sample_format(AV_SAMPLE_FMT_S64P, "maxbit32"), AV_SAMPLE_FMT_S32P);
	CUE_ASSERT_EQ(analyze_sample_format(AV_SAMPLE_FMT_S64, "maxbit16"), AV_SAMPLE_FMT_S16);
	CUE_ASSERT_EQ(analyze_sample_format(AV_SAMPLE_FMT_S64P, "maxbit16"), AV_SAMPLE_FMT_S16P);
	CUE_ASSERT_EQ(analyze_sample_format(AV_SAMPLE_FMT_S32, "maxbit8"), AV_SAMPLE_FMT_U8);
	CUE_ASSERT_EQ(analyze_sample_format(AV_SAMPLE_FMT_S32P, "maxbit8"), AV_SAMPLE_FMT_U8P);
}

SUITE_CASE("pipe line") {
	CUE_ASSERT_EQ(analyze_sample_format(AV_SAMPLE_FMT_DBLP, "int:pack:maxbit16"), AV_SAMPLE_FMT_S16);
}

SUITE_END(resampler_params_test)

