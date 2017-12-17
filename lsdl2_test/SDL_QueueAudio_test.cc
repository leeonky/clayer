#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include "stdexd/stdexd.h"
#include "lsdl2/lsdl2.h"
#include "mock_sdl2/mock_sdl2.h"

SUITE_START("SDL_QueueAudio_test");

static char arg_buffer[10];
static SDL_AudioFormat arg_format;
static int arg_channels, arg_samples;
static SDL_AudioDeviceID arg_device_id;

BEFORE_EACH() {
	init_subject("");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;
	app_stderr = actxt.error_stream;

	arg_device_id = 100;

	init_mock_function(SDL_QueueAudio);
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUBJECT(int) {
	return SDL_QueueAudio(arg_device_id, arg_buffer, arg_channels, arg_format, arg_samples);
}

SUITE_CASE("play audio") {
	arg_format = AUDIO_S16;
	arg_channels = 6;
	arg_samples = 10;

	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(SDL_QueueAudio);
	CUE_EXPECT_CALLED_WITH_INT(SDL_QueueAudio, 1, arg_device_id);
	CUE_EXPECT_CALLED_WITH_PTR(SDL_QueueAudio, 2, arg_buffer);
	CUE_EXPECT_CALLED_WITH_INT(SDL_QueueAudio, 3, 10*6*2);
}

SUITE_END(SDL_QueueAudio_test);

