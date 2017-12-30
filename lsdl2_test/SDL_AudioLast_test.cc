#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include "stdexd/stdexd.h"
#include "lsdl2/lsdl2.h"
#include "mock_sdl2/mock_sdl2.h"

SUITE_START("SDL_AudioLast_test");

static SDL_AudioDeviceID arg_device_id;
static SDL_AudioSpec arg_audio_spec;

BEFORE_EACH() {
	init_subject("");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;
	app_stderr = actxt.error_stream;

	arg_device_id = 100;
	arg_audio_spec.channels = 8;
	arg_audio_spec.format = AUDIO_S32;
	arg_audio_spec.freq = 8000;

	init_mock_function(SDL_GetQueuedAudioSize);
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUBJECT(int) {
	return SDL_AudioLast(arg_device_id, arg_audio_spec);
}

SUITE_CASE("get left audio data in queue data") {
	init_mock_function_with_return(SDL_GetQueuedAudioSize, 8000*8*4);

	CUE_ASSERT_SUBJECT_FAILED_WITH(1000000);
}

SUITE_END(SDL_AudioLast_test);


