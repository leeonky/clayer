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

SUITE_START("video_event_test");

mock_function_3(int, video_event_action, int, int, enum AVPixelFormat);

static std::unique_ptr<iobus> iob;

BEFORE_EACH() {
	init_subject("");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;
	app_stderr = actxt.error_stream;
	iob.reset(new iobus(actxt.input_stream, actxt.output_stream, actxt.error_stream));

	init_mock_function_with_return(av_get_pix_fmt, AV_PIX_FMT_YUV420P);
	init_mock_function(video_event_action);
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUBJECT(int) {
	return video_event(*iob, video_event_action);
}

SUITE_CASE("create sdl texture") {
	init_subject("VIDEO width:1920 height:1080 format:yuv420p");

	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(av_get_pix_fmt);

	CUE_EXPECT_CALLED_ONCE(video_event_action);
	CUE_EXPECT_CALLED_WITH_INT(video_event_action, 1, 1920);
	CUE_EXPECT_CALLED_WITH_INT(video_event_action, 2, 1080);
	CUE_EXPECT_CALLED_WITH_INT(video_event_action, 3, AV_PIX_FMT_YUV420P);
}

SUITE_CASE("event not match") {
	init_subject("AUDIO width:1920 height:1080 format:yuv420p");

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);
}

SUITE_CASE("args not match") {
	init_subject("VIDEO width:1920 format:yuv420p");

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_ASSERT_STDERR_EQ("Error[libiobus]: Invalid VIDEO arguments 'width:1920 format:yuv420p'\n");
}

SUITE_CASE("unsupport format") {
	init_subject("VIDEO width:1920 height:1080 format:yuv420p");
	init_mock_function_with_return(av_get_pix_fmt, AV_PIX_FMT_NONE);

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_ASSERT_STDERR_EQ("Error[libmedia]: Unsupport ffmpeg video format 'yuv420p'\n");
}

SUITE_END(video_event_test);

