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

BEFORE_EACH() {
	init_subject("");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;
	app_stderr = actxt.error_stream;
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUITE_CASE("video format transform") {
	CUE_ASSERT_EQ(AVPixelFormat_to_SDL(AV_PIX_FMT_YUV420P), SDL_PIXELFORMAT_IYUV);
}

SUITE_CASE("unsupport format") {
	CUE_ASSERT_EQ(AVPixelFormat_to_SDL((enum AVPixelFormat)12345), SDL_PIXELFORMAT_UNKNOWN);

	CUE_ASSERT_STDERR_EQ("Error[libmedia]: Unknow support ffmpeg video format '12345'\n");
}

SUITE_CASE("audio format transform") {
	CUE_ASSERT_EQ(AVSampleFormat_to_SDL(AV_SAMPLE_FMT_U8), AUDIO_U8);
	CUE_ASSERT_EQ(AVSampleFormat_to_SDL(AV_SAMPLE_FMT_S16), AUDIO_S16);
	CUE_ASSERT_EQ(AVSampleFormat_to_SDL(AV_SAMPLE_FMT_S32), AUDIO_S32);
	CUE_ASSERT_EQ(AVSampleFormat_to_SDL(AV_SAMPLE_FMT_FLT), AUDIO_F32);
}

SUITE_CASE("unsupport audio format") {
	CUE_ASSERT_EQ(AVSampleFormat_to_SDL((enum AVSampleFormat)12345), 0);

	CUE_ASSERT_STDERR_EQ("Error[libmedia]: Unknow support ffmpeg audio format '12345'\n");
}

SUITE_END(ffmpeg_to_sdl_format);

