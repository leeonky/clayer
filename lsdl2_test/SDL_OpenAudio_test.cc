#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include "stdexd/stdexd.h"
#include "lsdl2/lsdl2.h"
#include "mock_sdl2/mock_sdl2.h"

SUITE_START("SDL_OpenAudio_test");

static int arg_device_index, arg_freq, arg_channels;
static SDL_AudioFormat arg_format;
static SDL_AudioDeviceID ret_device_id;
static const char *ret_device_name;

mock_function_2(int, open_audio_action, SDL_AudioDeviceID, const SDL_AudioSpec *);

static int open_audio_action_ref(SDL_AudioDeviceID id, const SDL_AudioSpec &spec) {
	return open_audio_action(id, &spec);
}

BEFORE_EACH() {
	init_subject("");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;
	app_stderr = actxt.error_stream;

	arg_device_index = 10;
	arg_freq = 48000;
	arg_channels = 6;
	arg_format = AUDIO_F32;
	ret_device_id = 128;
	ret_device_name = "test";

	init_mock_function(SDL_InitSubSystem)
	init_mock_function_with_return(SDL_GetAudioDeviceName, ret_device_name);
	init_mock_function_with_return(SDL_OpenAudioDevice, ret_device_id);
	init_mock_function(open_audio_action);
	init_mock_function(SDL_CloseAudioDevice);
	init_mock_function(SDL_QuitSubSystem)
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUBJECT(int) {
	return SDL_OpenAudio(arg_device_index, arg_freq, arg_channels, arg_format, open_audio_action_ref);
}

static SDL_AudioDeviceID stub_SDL_OpenAudioDevice_assert(const char *, int, const SDL_AudioSpec *desired, SDL_AudioSpec *obtained, int) {
	CUE_ASSERT_EQ(desired->freq, arg_freq);
	CUE_ASSERT_EQ(desired->format, arg_format);
	CUE_ASSERT_EQ(desired->channels, arg_channels);
	void *p = (void *)desired->callback;
	CUE_ASSERT_PTR_EQ(p, NULL);
	CUE_ASSERT_PTR_EQ(desired->userdata, NULL);
	*obtained = *desired;
	return ret_device_id;
}

SUITE_CASE("get device name and open audio") {
	init_mock_function_with_function(SDL_OpenAudioDevice, stub_SDL_OpenAudioDevice_assert);

	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(SDL_InitSubSystem);
	CUE_EXPECT_CALLED_WITH_INT(SDL_InitSubSystem, 1, SDL_INIT_AUDIO);

	CUE_EXPECT_CALLED_ONCE(SDL_GetAudioDeviceName);
	CUE_EXPECT_CALLED_WITH_INT(SDL_GetAudioDeviceName, 1, arg_device_index);
	CUE_EXPECT_CALLED_WITH_INT(SDL_GetAudioDeviceName, 2, 0);

	CUE_EXPECT_CALLED_ONCE(SDL_OpenAudioDevice);
	CUE_EXPECT_CALLED_WITH_STRING(SDL_OpenAudioDevice, 1, ret_device_name);
	CUE_EXPECT_CALLED_WITH_INT(SDL_OpenAudioDevice, 2, 0);
	CUE_EXPECT_CALLED_WITH_INT(SDL_OpenAudioDevice, 5, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE);

	CUE_EXPECT_CALLED_ONCE(open_audio_action);
	CUE_EXPECT_CALLED_WITH_INT(open_audio_action, 1, ret_device_id);

	CUE_EXPECT_CALLED_ONCE(SDL_CloseAudioDevice);
	CUE_EXPECT_CALLED_WITH_INT(SDL_CloseAudioDevice, 1, ret_device_id);

	CUE_EXPECT_CALLED_ONCE(SDL_QuitSubSystem);
	CUE_EXPECT_CALLED_WITH_INT(SDL_QuitSubSystem, 1, SDL_INIT_AUDIO);
}

SUITE_CASE("failed init audio") {
	init_mock_function_with_return(SDL_InitSubSystem, -1);

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_EXPECT_NEVER_CALLED(SDL_GetAudioDeviceName);

	CUE_EXPECT_NEVER_CALLED(SDL_OpenAudioDevice);

	CUE_EXPECT_NEVER_CALLED(open_audio_action);

	CUE_EXPECT_NEVER_CALLED(SDL_CloseAudioDevice);

	CUE_EXPECT_NEVER_CALLED(SDL_QuitSubSystem);

	CUE_ASSERT_STDERR_EQ("Error[liblsdl2]: sdl error\n");
}

SUITE_CASE("failed to get device name") {
	init_mock_function_with_return(SDL_GetAudioDeviceName, NULL);

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_EXPECT_NEVER_CALLED(SDL_OpenAudioDevice);

	CUE_EXPECT_NEVER_CALLED(open_audio_action);

	CUE_EXPECT_NEVER_CALLED(SDL_CloseAudioDevice);

	CUE_EXPECT_CALLED_ONCE(SDL_QuitSubSystem);

	CUE_ASSERT_STDERR_EQ("Error[liblsdl2]: sdl error\n");
}

SUITE_CASE("failed to open device") {
	init_mock_function_with_return(SDL_OpenAudioDevice, 0);

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_EXPECT_NEVER_CALLED(SDL_CloseAudioDevice);

	CUE_EXPECT_CALLED_ONCE(SDL_QuitSubSystem);

	CUE_ASSERT_STDERR_EQ("Error[liblsdl2]: sdl error\n");
}

static SDL_AudioDeviceID stub_SDL_OpenAudioDevice_changed(const char *, int, const SDL_AudioSpec *desired, SDL_AudioSpec *obtained, int) {
	obtained->freq = desired->freq + 10;
	return ret_device_id;
}

SUITE_CASE("open succeded but arguments changed") {
	init_mock_function_with_function(SDL_OpenAudioDevice, stub_SDL_OpenAudioDevice_changed);

	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_ASSERT_STDERR_EQ("Warning[liblsdl2]: audio parameters changed\n");
}

SUITE_END(SDL_OpenAudio_test);

