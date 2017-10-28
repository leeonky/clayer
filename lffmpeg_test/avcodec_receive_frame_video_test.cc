#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include "stdexd/stdexd.h"
#include "mock_ffmpeg/mock_ffmpeg.h"
#include "lffmpeg/lffmpeg.h"

SUITE_START("avcodec_receive_frame_video_test");

static stub_decoding_context arg_decoding_context;
static AVCodecContext arg_av_codec_context;
static AVFrame arg_av_frame;

mock_function_1(int, video_decode_action, const AVFrame *);

static int video_decode_action_ref(const AVFrame &av_frame) {
	return video_decode_action(&av_frame);
}

BEFORE_EACH() {
	arg_av_codec_context.codec_type = AVMEDIA_TYPE_VIDEO;
	arg_av_codec_context.opaque = &arg_decoding_context;

	arg_decoding_context.working_av_frame = &arg_av_frame;

	init_mock_function(avcodec_receive_frame);
	init_mock_function(video_decode_action);
	return 0;
}

SUBJECT(int) {
	return avcodec_receive_frame(arg_av_codec_context, video_decode_action_ref);
}

static int video_decode_action_assert(const AVFrame *av_frame) {
	CUE_ASSERT_PTR_EQ(av_frame, arg_decoding_context.working_av_frame);
	return 0;
}

SUITE_CASE("decode video to frame and invoke action") {
	init_mock_function_with_function(video_decode_action, video_decode_action_assert);

	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(avcodec_receive_frame);
	CUE_EXPECT_CALLED_WITH_PTR(avcodec_receive_frame, 1, &arg_av_codec_context);
	CUE_EXPECT_CALLED_WITH_PTR(avcodec_receive_frame, 2, &arg_av_frame);

	CUE_EXPECT_CALLED_ONCE(video_decode_action);
}

SUITE_CASE("no frame to receive") {
	init_mock_function_with_return(avcodec_receive_frame, -100);

	CUE_ASSERT_SUBJECT_FAILED_WITH(-100);

	CUE_EXPECT_NEVER_CALLED(video_decode_action);
}

SUITE_END(avcodec_receive_frame_video_test);
