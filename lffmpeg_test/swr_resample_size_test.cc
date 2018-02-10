#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include "stdexd/stdexd.h"
#include "mock_ffmpeg/mock_ffmpeg.h"
#include "lffmpeg/lffmpeg.h"

SUITE_START("swr_resample_size_test");

SUITE_CASE("work size by format layout and sample_rate") {
	int arg_src_size = 1000;
	resample_context arg_context;
	arg_context.in_sample_bytes = 2;
	arg_context.in_channels = 2;
	arg_context.in_rate = 48000;
	arg_context.out_sample_bytes = 4;
	arg_context.out_channels = 4;
	arg_context.out_rate = 96000;

	CUE_ASSERT_EQ(arg_context.resample_size(arg_src_size), 8000);
}

SUITE_END(swr_resample_size_test);
