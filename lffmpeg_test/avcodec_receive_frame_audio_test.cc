#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include "stdexd/stdexd.h"
#include "mock_ffmpeg/mock_ffmpeg.h"
#include "lffmpeg/lffmpeg.h"

SUITE_START("avcodec_receive_frame_audio_test");

static stub_decoding_context arg_decoding_context;
static AVCodecContext arg_av_codec_context;
static AVFrame arg_working_av_frame, arg_decoded_av_frame;

mock_function_1(int, audio_decode_action, const AVFrame *);

static int audio_decode_action_ref(const AVFrame &av_frame) {
	return audio_decode_action(&av_frame);
}

BEFORE_EACH() {
	arg_av_codec_context.codec_type = AVMEDIA_TYPE_AUDIO;
	arg_av_codec_context.opaque = &arg_decoding_context;

	arg_decoding_context.working_av_frame = &arg_working_av_frame;
	arg_decoding_context.decoded_av_frame = &arg_decoded_av_frame;

	init_mock_function(avcodec_receive_frame);
	init_mock_function(audio_decode_action);
	init_mock_function(av_samples_copy);
	init_mock_function(av_frame_get_best_effort_timestamp);
	init_mock_function(av_frame_set_best_effort_timestamp);
	return 0;
}

SUBJECT(int) {
	return avcodec_receive_frame(arg_av_codec_context, audio_decode_action_ref);
}

static int64_t arg_pts;
static int arg_channels, arg_format, arg_samples_size, arg_working_av_frame_size, arg_decoded_av_frame_size;

SUITE_CASE("set decoded frame pts with working frame pts when first copy") {
	arg_pts = 1024;
	init_mock_function_with_return(av_frame_get_best_effort_timestamp, arg_pts);
	arg_working_av_frame.nb_samples = arg_working_av_frame_size = 50;
	arg_decoded_av_frame.nb_samples = arg_decoded_av_frame_size = 0;
	arg_decoding_context.samples_size = arg_working_av_frame_size + arg_decoded_av_frame_size + 1;

	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(av_frame_set_best_effort_timestamp);
	CUE_EXPECT_CALLED_WITH_PTR(av_frame_set_best_effort_timestamp, 1, &arg_decoded_av_frame);
	CUE_EXPECT_CALLED_WITH_INT(av_frame_set_best_effort_timestamp, 2, arg_pts);
}

SUITE_CASE("cache audio data from wframe to rframe") {
	arg_working_av_frame.nb_samples = arg_working_av_frame_size = 50;
	arg_decoded_av_frame.nb_samples = arg_decoded_av_frame_size = 10;
	arg_decoding_context.samples_size = arg_working_av_frame_size + arg_decoded_av_frame_size + 1;

	arg_working_av_frame.channels = arg_channels = 8;
	arg_working_av_frame.format = arg_format = 2;

	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(avcodec_receive_frame);
	CUE_EXPECT_CALLED_WITH_PTR(avcodec_receive_frame, 1, &arg_av_codec_context);
	CUE_EXPECT_CALLED_WITH_PTR(avcodec_receive_frame, 2, &arg_working_av_frame);

	CUE_EXPECT_NEVER_CALLED(audio_decode_action);

	CUE_EXPECT_CALLED_ONCE(av_samples_copy);
	CUE_EXPECT_CALLED_WITH_PTR(av_samples_copy, 1, arg_decoded_av_frame.data);
	CUE_EXPECT_CALLED_WITH_PTR(av_samples_copy, 2, arg_working_av_frame.data);
	CUE_EXPECT_CALLED_WITH_INT(av_samples_copy, 3, arg_decoded_av_frame_size);
	CUE_EXPECT_CALLED_WITH_INT(av_samples_copy, 4, 0);
	CUE_EXPECT_CALLED_WITH_INT(av_samples_copy, 5, arg_working_av_frame_size);
	CUE_EXPECT_CALLED_WITH_INT(av_samples_copy, 6, arg_channels);
	CUE_EXPECT_CALLED_WITH_INT(av_samples_copy, 7, arg_format);

	CUE_EXPECT_NEVER_CALLED(av_frame_set_best_effort_timestamp);

	CUE_ASSERT_EQ(arg_decoded_av_frame.nb_samples, arg_decoded_av_frame_size + arg_working_av_frame_size);
}

static int audio_decode_action_assert(const AVFrame *frame) {
	CUE_ASSERT_EQ(frame->nb_samples, arg_decoded_av_frame_size);
	return 0;
}

SUITE_CASE("drain rframe when rframe full") {
	arg_decoded_av_frame.pkt_duration = 31415926;

	arg_working_av_frame.nb_samples = arg_working_av_frame_size = 50;
	arg_decoded_av_frame.nb_samples = arg_decoded_av_frame_size = 60;
	arg_decoding_context.samples_size = arg_working_av_frame_size + arg_decoded_av_frame_size - 10;

	arg_working_av_frame.channels = arg_channels = 8;
	arg_working_av_frame.format = arg_format = 2;

	init_mock_function_with_function(audio_decode_action, audio_decode_action_assert);

	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(av_samples_copy);
	CUE_EXPECT_CALLED_WITH_PTR(av_samples_copy, 1, arg_decoded_av_frame.data);
	CUE_EXPECT_CALLED_WITH_PTR(av_samples_copy, 2, arg_working_av_frame.data);
	CUE_EXPECT_CALLED_WITH_INT(av_samples_copy, 3, 0);
	CUE_EXPECT_CALLED_WITH_INT(av_samples_copy, 4, 0);
	CUE_EXPECT_CALLED_WITH_INT(av_samples_copy, 5, arg_working_av_frame_size);
	CUE_EXPECT_CALLED_WITH_INT(av_samples_copy, 6, arg_channels);
	CUE_EXPECT_CALLED_WITH_INT(av_samples_copy, 7, arg_format);

	CUE_ASSERT_EQ(arg_decoded_av_frame.nb_samples, arg_working_av_frame_size);
	CUE_ASSERT_EQ(arg_decoded_av_frame.pkt_duration, 0);
}

SUITE_CASE("stream to the end after last decode") {
	arg_decoded_av_frame.pkt_duration = 31415926;
	arg_decoded_av_frame.nb_samples = arg_decoded_av_frame_size;
	arg_decoding_context.stream_ended = 1;
	init_mock_function_with_function(audio_decode_action, audio_decode_action_assert);
	init_mock_function_with_return(avcodec_receive_frame, -1);

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_EXPECT_CALLED_ONCE(audio_decode_action);

	CUE_ASSERT_EQ(arg_decoded_av_frame.nb_samples, 0);
	CUE_ASSERT_EQ(arg_decoded_av_frame.pkt_duration, 0);
}

SUITE_END(avcodec_receive_frame_audio_test);
