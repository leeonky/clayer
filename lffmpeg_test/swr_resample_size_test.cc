#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include "stdexd/stdexd.h"
#include "mock_ffmpeg/mock_ffmpeg.h"
#include "lffmpeg/lffmpeg.h"

SUITE_START("swr_resample_size_test");

SUITE_CASE("work size by format layout and sample_rate") {
	resample_context arg_context;
	arg_context.out_channels = 8;
	arg_context.out_rate = 96000;
	arg_context.out_format = AV_SAMPLE_FMT_S64;
	init_mock_function_with_return(av_samples_get_buffer_size, 1000);

	CUE_ASSERT_EQ(arg_context.resample_size(), 1000);

	CUE_EXPECT_CALLED_ONCE(av_samples_get_buffer_size);
	CUE_EXPECT_CALLED_WITH_PTR(av_samples_get_buffer_size, 1, nullptr);
	CUE_EXPECT_CALLED_WITH_INT(av_samples_get_buffer_size, 2, 8);
	CUE_EXPECT_CALLED_WITH_INT(av_samples_get_buffer_size, 3, 9600);
	CUE_EXPECT_CALLED_WITH_INT(av_samples_get_buffer_size, 4, AV_SAMPLE_FMT_S64);
	CUE_EXPECT_CALLED_WITH_INT(av_samples_get_buffer_size, 5, 1);
}

SUITE_END(swr_resample_size_test);
