#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include "stdexd/stdexd.h"
#include "lalsa/lalsa.h"
#include "mock_alsa/mock_alsa.h"

SUITE_START("snd_pcm_open_test");

static const char *arg_device;
static int arg_rate, arg_channels;
static snd_pcm_format_t arg_format;
static snd_pcm_hw_params_t *ret_hw_params;
static snd_pcm_t *ret_pcm;

mock_function_1(int, snd_open_action, snd_pcm_t *);

static int stub_snd_pcm_open(snd_pcm_t **pcm, const char *, snd_pcm_stream_t, int) {
	*pcm = ret_pcm;
	return 0;
}

static int stub_snd_pcm_hw_params_malloc(snd_pcm_hw_params_t **ptr) {
	*ptr = ret_hw_params;
	return 0;
}


BEFORE_EACH() {
	init_subject("");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;
	app_stderr = actxt.error_stream;

	arg_device = "default";
	arg_rate = 48000;
	arg_channels = 6;
	arg_format = SND_PCM_FORMAT_S16_LE;
	ret_pcm = (snd_pcm_t *)&ret_pcm;
	ret_hw_params = (snd_pcm_hw_params_t *)&ret_hw_params;

	init_mock_function_with_function(snd_pcm_open, stub_snd_pcm_open);
	init_mock_function_with_function(snd_pcm_hw_params_malloc, stub_snd_pcm_hw_params_malloc);
	init_mock_function(snd_pcm_hw_params_any);
	init_mock_function(snd_pcm_hw_params_set_access);
	init_mock_function(snd_pcm_hw_params_set_format);
	init_mock_function(snd_pcm_hw_params_set_rate);
	init_mock_function(snd_pcm_hw_params_set_channels);
	init_mock_function(snd_pcm_hw_params_set_buffer_size_near);
	init_mock_function(snd_pcm_hw_params_set_period_size_near);
	init_mock_function(snd_pcm_hw_params);
	init_mock_function(snd_pcm_hw_params_free);
	init_mock_function(snd_pcm_close);
	init_mock_function(snd_open_action);
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUBJECT(int) {
	return snd_pcm_open(arg_device, arg_rate, arg_channels, arg_format, snd_open_action);
}

static int assert_snd_pcm_hw_params_set_buffer_size_near(snd_pcm_t *, snd_pcm_hw_params_t *, snd_pcm_uframes_t *t) {
	CUE_ASSERT_EQ(*t, arg_rate/10);
	return 0;
}
static int assert_snd_pcm_hw_params_set_period_size_near(snd_pcm_t *, snd_pcm_hw_params_t *, snd_pcm_uframes_t *t, int *) {
	CUE_ASSERT_EQ(*t, 128);
	return 0;
}

SUITE_CASE("init and open audio") {
	init_mock_function_with_function(snd_pcm_hw_params_set_buffer_size_near, assert_snd_pcm_hw_params_set_buffer_size_near);
	init_mock_function_with_function(snd_pcm_hw_params_set_period_size_near, assert_snd_pcm_hw_params_set_period_size_near);

	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(snd_pcm_open);
	CUE_EXPECT_CALLED_WITH_STRING(snd_pcm_open, 2, arg_device);
	CUE_EXPECT_CALLED_WITH_INT(snd_pcm_open, 3, SND_PCM_STREAM_PLAYBACK);
	CUE_EXPECT_CALLED_WITH_INT(snd_pcm_open, 4, SND_PCM_ASYNC);

	CUE_EXPECT_CALLED_ONCE(snd_pcm_hw_params_malloc);

	CUE_EXPECT_CALLED_ONCE(snd_pcm_hw_params_any);
	CUE_EXPECT_CALLED_WITH_PTR(snd_pcm_hw_params_any, 1, ret_pcm);
	CUE_EXPECT_CALLED_WITH_PTR(snd_pcm_hw_params_any, 2, ret_hw_params);

	CUE_EXPECT_CALLED_ONCE(snd_pcm_hw_params_set_access);
	CUE_EXPECT_CALLED_WITH_PTR(snd_pcm_hw_params_set_access, 1, ret_pcm);
	CUE_EXPECT_CALLED_WITH_PTR(snd_pcm_hw_params_set_access, 2, ret_hw_params);
	CUE_EXPECT_CALLED_WITH_INT(snd_pcm_hw_params_set_access, 3, SND_PCM_ACCESS_MMAP_INTERLEAVED);

	CUE_EXPECT_CALLED_ONCE(snd_pcm_hw_params_set_rate);
	CUE_EXPECT_CALLED_WITH_PTR(snd_pcm_hw_params_set_rate, 1, ret_pcm);
	CUE_EXPECT_CALLED_WITH_PTR(snd_pcm_hw_params_set_rate, 2, ret_hw_params);
	CUE_EXPECT_CALLED_WITH_INT(snd_pcm_hw_params_set_rate, 3, arg_rate);

	CUE_EXPECT_CALLED_ONCE(snd_pcm_hw_params_set_format);
	CUE_EXPECT_CALLED_WITH_PTR(snd_pcm_hw_params_set_format, 1, ret_pcm);
	CUE_EXPECT_CALLED_WITH_PTR(snd_pcm_hw_params_set_format, 2, ret_hw_params);
	CUE_EXPECT_CALLED_WITH_INT(snd_pcm_hw_params_set_format, 3, arg_format);

	CUE_EXPECT_CALLED_ONCE(snd_pcm_hw_params_set_channels);
	CUE_EXPECT_CALLED_WITH_PTR(snd_pcm_hw_params_set_channels, 1, ret_pcm);
	CUE_EXPECT_CALLED_WITH_PTR(snd_pcm_hw_params_set_channels, 2, ret_hw_params);
	CUE_EXPECT_CALLED_WITH_INT(snd_pcm_hw_params_set_channels, 3, arg_channels);

	CUE_EXPECT_CALLED_ONCE(snd_pcm_hw_params_set_buffer_size_near);
	CUE_EXPECT_CALLED_WITH_PTR(snd_pcm_hw_params_set_buffer_size_near, 1, ret_pcm);
	CUE_EXPECT_CALLED_WITH_PTR(snd_pcm_hw_params_set_buffer_size_near, 2, ret_hw_params);

	CUE_EXPECT_CALLED_ONCE(snd_pcm_hw_params_set_period_size_near);
	CUE_EXPECT_CALLED_WITH_PTR(snd_pcm_hw_params_set_period_size_near, 1, ret_pcm);
	CUE_EXPECT_CALLED_WITH_PTR(snd_pcm_hw_params_set_period_size_near, 2, ret_hw_params);
	CUE_EXPECT_CALLED_WITH_PTR(snd_pcm_hw_params_set_period_size_near, 4, NULL);

	CUE_EXPECT_CALLED_ONCE(snd_pcm_hw_params);
	CUE_EXPECT_CALLED_WITH_PTR(snd_pcm_hw_params, 1, ret_pcm);
	CUE_EXPECT_CALLED_WITH_PTR(snd_pcm_hw_params, 2, ret_hw_params);

	CUE_EXPECT_CALLED_ONCE(snd_open_action);
	CUE_EXPECT_CALLED_WITH_PTR(snd_open_action, 1, ret_pcm);

	CUE_EXPECT_CALLED_ONCE(snd_pcm_hw_params_free);
	CUE_EXPECT_CALLED_WITH_PTR(snd_pcm_hw_params_free, 1, ret_hw_params);

	CUE_EXPECT_CALLED_ONCE(snd_pcm_close);
	CUE_EXPECT_CALLED_WITH_PTR(snd_pcm_close, 1, ret_pcm);
}

SUITE_CASE("snd_pcm_open failed") {
	init_mock_function_with_return(snd_pcm_open, -100);

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_EXPECT_NEVER_CALLED(snd_pcm_hw_params_malloc);
	CUE_EXPECT_NEVER_CALLED(snd_pcm_hw_params_any);
	CUE_EXPECT_NEVER_CALLED(snd_pcm_hw_params_set_access);
	CUE_EXPECT_NEVER_CALLED(snd_pcm_hw_params_set_rate);
	CUE_EXPECT_NEVER_CALLED(snd_pcm_hw_params_set_format);
	CUE_EXPECT_NEVER_CALLED(snd_pcm_hw_params_set_channels);
	CUE_EXPECT_NEVER_CALLED(snd_pcm_hw_params_set_buffer_size);
	CUE_EXPECT_NEVER_CALLED(snd_pcm_hw_params_set_period_size);
	CUE_EXPECT_NEVER_CALLED(snd_pcm_hw_params);
	CUE_EXPECT_NEVER_CALLED(snd_open_action);
	CUE_EXPECT_NEVER_CALLED(snd_pcm_hw_params_free);
	CUE_EXPECT_NEVER_CALLED(snd_pcm_close);

	CUE_ASSERT_STDERR_EQ("Error[liblalsa]: -100\n");
}

SUITE_CASE("snd_pcm_hw_params_malloc failed") {
	init_mock_function_with_return(snd_pcm_hw_params_malloc, -100);

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_EXPECT_NEVER_CALLED(snd_pcm_hw_params_any);
	CUE_EXPECT_NEVER_CALLED(snd_pcm_hw_params_set_access);
	CUE_EXPECT_NEVER_CALLED(snd_pcm_hw_params_set_rate);
	CUE_EXPECT_NEVER_CALLED(snd_pcm_hw_params_set_format);
	CUE_EXPECT_NEVER_CALLED(snd_pcm_hw_params_set_channels);
	CUE_EXPECT_NEVER_CALLED(snd_pcm_hw_params_set_buffer_size);
	CUE_EXPECT_NEVER_CALLED(snd_pcm_hw_params_set_period_size);
	CUE_EXPECT_NEVER_CALLED(snd_pcm_hw_params);
	CUE_EXPECT_NEVER_CALLED(snd_open_action);
	CUE_EXPECT_NEVER_CALLED(snd_pcm_hw_params_free);
	CUE_EXPECT_CALLED_ONCE(snd_pcm_close);

	CUE_ASSERT_STDERR_EQ("Error[liblalsa]: -100\n");
}

SUITE_CASE("snd_pcm_hw_params_set_params failed") {
	init_mock_function_with_return(snd_pcm_hw_params, -100);

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_EXPECT_NEVER_CALLED(snd_open_action);
	CUE_EXPECT_CALLED_ONCE(snd_pcm_hw_params_free);
	CUE_EXPECT_CALLED_ONCE(snd_pcm_close);

	CUE_ASSERT_STDERR_EQ("Error[liblalsa]: -100\n");
}

SUITE_END(snd_pcm_open_test);

