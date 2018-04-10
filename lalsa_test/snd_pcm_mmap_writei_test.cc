#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include "stdexd/stdexd.h"
#include "lalsa/lalsa.h"
#include "mock_alsa/mock_alsa.h"

SUITE_START("snd_pcm_mmap_writei_test");

static snd_pcm_t *arg_pcm;
static void *arg_buffer;
static int arg_frames;

BEFORE_EACH() {
	init_subject("");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;
	app_stderr = actxt.error_stream;

	arg_pcm = (snd_pcm_t *)&arg_pcm;
	arg_buffer = &arg_buffer;
	arg_frames = 100;

	init_mock_function_with_return(snd_pcm_mmap_writei, 10);
	init_mock_function(snd_pcm_prepare);
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUBJECT(int) {
	return snd_pcm_mmap_writei(arg_pcm, arg_buffer, arg_frames);
}

SUITE_CASE("writei OK") {
	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(snd_pcm_mmap_writei);
	CUE_EXPECT_CALLED_WITH_PTR(snd_pcm_mmap_writei, 1, arg_pcm);
	CUE_EXPECT_CALLED_WITH_PTR(snd_pcm_mmap_writei, 2, arg_buffer);
	CUE_EXPECT_CALLED_WITH_INT(snd_pcm_mmap_writei, 3, arg_frames);
}

static int stub_snd_pcm_prepare(snd_pcm_t *) {
	CUE_EXPECT_CALLED_ONCE(snd_pcm_mmap_writei);

	init_mock_function_with_return(snd_pcm_mmap_writei, 10);
	return 0;
}

SUITE_CASE("xrun; prepare and writei again") {
	init_mock_function_with_function(snd_pcm_prepare, stub_snd_pcm_prepare);
	init_mock_function_with_return(snd_pcm_mmap_writei, -EPIPE);

	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(snd_pcm_prepare);
	CUE_EXPECT_CALLED_WITH_PTR(snd_pcm_prepare, 1, arg_pcm);

	CUE_ASSERT_STDERR_EQ("Warning[liblalsa]: xrun\n");
}

SUITE_CASE("writei failed") {
	init_mock_function_with_return(snd_pcm_mmap_writei, -100);

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_EXPECT_NEVER_CALLED(snd_pcm_prepare);

	CUE_ASSERT_STDERR_EQ("Error[liblalsa]: -100\n");
}

SUITE_CASE("prepare failed") {
	init_mock_function_with_return(snd_pcm_prepare, -100);
	init_mock_function_with_return(snd_pcm_mmap_writei, -EPIPE);

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_ASSERT_STDERR_EQ("Warning[liblalsa]: xrun\nError[liblalsa]: -100\n");
}

SUITE_END(snd_pcm_mmap_writei_test);

