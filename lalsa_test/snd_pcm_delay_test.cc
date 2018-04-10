#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include "stdexd/stdexd.h"
#include "lalsa/lalsa.h"
#include "mock_alsa/mock_alsa.h"

SUITE_START("snd_pcm_delay_test");

static snd_pcm_t *arg_pcm;
static int arg_rate;
static snd_pcm_sframes_t ret_delay;

static int stub_snd_pcm_delay(snd_pcm_t *, snd_pcm_sframes_t *t) {
	*t = ret_delay;
	return 0;
}

BEFORE_EACH() {
	init_subject("");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;
	app_stderr = actxt.error_stream;

	arg_pcm = (snd_pcm_t *)&arg_pcm;
	arg_rate = 1000;
	ret_delay = 100;

	init_mock_function_with_function(snd_pcm_delay, stub_snd_pcm_delay);
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUBJECT(int) {
	return snd_pcm_delay(arg_pcm, arg_rate);
}

SUITE_CASE("get delay with rate in us") {
	CUE_ASSERT_EQ(CALL_SUBJECT(snd_pcm_sframes_t), 100000);
}

SUITE_CASE("get delay failed should return 0") {
	init_mock_function_with_return(snd_pcm_delay, -100);

	CUE_ASSERT_EQ(CALL_SUBJECT(snd_pcm_sframes_t), 0);

	CUE_ASSERT_STDERR_EQ("Error[liblalsa]: -100\n");
}

SUITE_END(snd_pcm_delay_test);

