#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include "stdexd/stdexd.h"
#include "lport_audio/lport_audio.h"
#include "mock_port_audio/mock_port_audio.h"

SUITE_START("Pa_GetStreamLast_test");

static long ret_buffer_available, arg_buffer_len;
static int arg_rate;
static PaStream *arg_stream;

BEFORE_EACH() {
	arg_buffer_len = 300;
	ret_buffer_available = 100;
	arg_rate = 44100;
	arg_stream = &arg_stream;

	init_mock_function_with_return(Pa_GetStreamWriteAvailable, ret_buffer_available)
	return 0;
}

SUITE_CASE("work time by rate and sample number") {
	CUE_ASSERT_EQ(Pa_GetStreamLast(arg_stream, arg_buffer_len, arg_rate), 200*1000000/44100);
}

SUITE_END(Pa_GetStreamLast_test);

