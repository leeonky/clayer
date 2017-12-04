#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include <memory>
#include "stdexd/stdexd.h"
#include "lsdl2/lsdl2.h"
#include "lffmpeg/lffmpeg.h"
#include "media/media.h"
#include "mock_sdl2/mock_sdl2.h"
#include "mock_ffmpeg/mock_ffmpeg.h"

SUITE_START("ffmpeg_to_sdl_format");

static const char *arg_format;

BEFORE_EACH() {
	init_subject("");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;
	app_stderr = actxt.error_stream;

	arg_format = "test";

	init_mock_function(av_get_pix_fmt);
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUITE_CASE("video format transform") {
	init_mock_function_with_return(av_get_pix_fmt, AV_PIX_FMT_YUV420P);

	CUE_ASSERT_EQ(AVPixelFormat_to_SDL(arg_format), SDL_PIXELFORMAT_IYUV);

	CUE_EXPECT_CALLED_ONCE(av_get_pix_fmt);
	CUE_EXPECT_CALLED_WITH_STRING(av_get_pix_fmt, 1, arg_format);
}

SUITE_CASE("parse ffmpeg format from name failed") {
	init_mock_function_with_return(av_get_pix_fmt, AV_PIX_FMT_NONE);

	CUE_ASSERT_EQ(AVPixelFormat_to_SDL(arg_format), SDL_PIXELFORMAT_UNKNOWN);

	CUE_ASSERT_STDERR_EQ("Error[libmedia]: Unknow ffmpeg format 'test'\n");
}

SUITE_CASE("unsupport format") {
	init_mock_function_with_return(av_get_pix_fmt, AV_PIX_FMT_Y400A);

	CUE_ASSERT_EQ(AVPixelFormat_to_SDL(arg_format), SDL_PIXELFORMAT_UNKNOWN);

	CUE_ASSERT_STDERR_EQ("Error[libmedia]: Unknow support format 'test'\n");
}

SUITE_END(ffmpeg_to_sdl_format);

