#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include <memory>
#include "stdexd/stdexd.h"
#include "media/media.h"
#include "mock_ffmpeg/mock_ffmpeg.h"

SUITE_START("rescaler_params_test");

static enum AVPixelFormat stub_av_get_pix_fmt(const char *name) {
	if(!strcmp(name, "yuv420p"))
		return AV_PIX_FMT_YUV420P;
	else if(!strcmp(name, "yuv422p"))
		return AV_PIX_FMT_YUV422P;
	else if(!strcmp(name, "yuv420p10le"))
		return AV_PIX_FMT_YUV420P10LE;
	else
		return AV_PIX_FMT_NONE;
}

BEFORE_EACH() {
	init_subject("");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;
	app_stderr = actxt.error_stream;

	init_mock_function_with_function(av_get_pix_fmt, stub_av_get_pix_fmt);
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUITE_CASE("nothing to do format") {
	CUE_ASSERT_EQ(analyze_pixel_format(AV_PIX_FMT_YUV420P, "", ""), AV_PIX_FMT_YUV420P);
}

SUITE_CASE("set format") {
	CUE_ASSERT_EQ(analyze_pixel_format(AV_PIX_FMT_YUV420P, "", "yuv420p10le"), AV_PIX_FMT_YUV420P10LE);
}

SUITE_CASE("nothing to do if not match") {
	CUE_ASSERT_EQ(analyze_pixel_format(AV_PIX_FMT_YUV422P, "yuv420p10le", "yuv420p"), AV_PIX_FMT_YUV422P);
}

SUITE_CASE("change format") {
	CUE_ASSERT_EQ(analyze_pixel_format(AV_PIX_FMT_YUV420P10LE, "yuv420p10le", "yuv420p"), AV_PIX_FMT_YUV420P);
}

SUITE_CASE("size not change") {
	int w = 1920, h = 1080;
	analyze_size_format(w, h, "");

	CUE_ASSERT_EQ(w, 1920);
	CUE_ASSERT_EQ(h, 1080);
}

SUITE_CASE("resize") {
	int w = 1920, h = 1080;
	analyze_size_format(w, h, "192x108");

	CUE_ASSERT_EQ(w, 192);
	CUE_ASSERT_EQ(h, 108);
}

SUITE_CASE("resize but should keep w/h") {
	int w = 400, h = 300;
	analyze_size_format(w, h, "1920x1080");

	CUE_ASSERT_EQ(w, 1440);
	CUE_ASSERT_EQ(h, 1080);

	w = 160, h = 90;
	analyze_size_format(w, h, "400x300");

	CUE_ASSERT_EQ(w, 400);
	CUE_ASSERT_EQ(h, 225);
}

SUITE_END(rescaler_params_test)


