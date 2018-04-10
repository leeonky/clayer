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

SUITE_START("audio_event_test");

static int64_t ret_layout;
static enum AVSampleFormat ret_format;

mock_function_5(int, audio_event_action, int, int, int64_t, enum AVSampleFormat, int);

BEFORE_EACH() {
	init_subject("");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;
	app_stderr = actxt.error_stream;

	ret_layout = AV_CH_LAYOUT_MONO;
	ret_format = AV_SAMPLE_FMT_S16;

	init_mock_function_with_return(av_get_channel_layout, ret_layout);
	init_mock_function_with_return(av_get_sample_fmt, ret_format);
	init_mock_function(audio_event_action);
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUBJECT(int) {
	iobus iob{actxt.input_stream, actxt.output_stream, actxt.error_stream};
	return audio_event(iob, audio_event_action);
}

SUITE_CASE("create sdl texture") {
	init_subject("AUDIO sample_rate:48000 channels:8 layout:7.1 format:s32 passthrough:1");

	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(av_get_channel_layout);

	CUE_EXPECT_CALLED_ONCE(av_get_sample_fmt);

	CUE_EXPECT_CALLED_ONCE(audio_event_action);
	CUE_EXPECT_CALLED_WITH_INT(audio_event_action, 1, 48000);
	CUE_EXPECT_CALLED_WITH_INT(audio_event_action, 2, 8);
	CUE_EXPECT_CALLED_WITH_INT(audio_event_action, 3, AV_CH_LAYOUT_MONO);
	CUE_EXPECT_CALLED_WITH_INT(audio_event_action, 4, AV_SAMPLE_FMT_S16);
	CUE_EXPECT_CALLED_WITH_INT(audio_event_action, 5, 1);
}

SUITE_CASE("unsupport format") {
	init_subject("AUDIO sample_rate:48000 channels:8 layout:7.1 format:s32 passthrough:0");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;
	app_stderr = actxt.error_stream;
	init_mock_function_with_return(av_get_sample_fmt, AV_SAMPLE_FMT_NONE);

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_ASSERT_STDERR_EQ("Error[libmedia]: Unsupport ffmpeg audio format 's32'\n");
}

SUITE_CASE("unsupport layout") {
	init_subject("AUDIO sample_rate:48000 channels:8 layout:7.1 format:s32 passthrough:0");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;
	app_stderr = actxt.error_stream;
	init_mock_function_with_return(av_get_channel_layout, 0);

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_ASSERT_STDERR_EQ("Error[libmedia]: Unsupport ffmpeg audio layout '7.1'\n");
}

SUITE_END(audio_event_test);


