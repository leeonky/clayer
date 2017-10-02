#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include "stdexd/stdexd.h"
#include "mock_ffmpeg/mock_ffmpeg.h"
#include "lffmpeg/lffmpeg.h"

SUITE_START("avformat_find_stream_test");

static AVFormatContext arg_av_format_context;
static enum AVMediaType arg_track_type;
static int arg_track_index;
static AVStream arg_streams[2];
static const char *stub_av_get_media_type_string(enum AVMediaType type) {
	switch(type) {
		case AVMEDIA_TYPE_VIDEO:
			return "video";
		case AVMEDIA_TYPE_AUDIO:
			return "audio";
		default:
			return "unkown";
	}
}

mock_function_1(int, avformat_find_stream_action, AVStream *);

static int avformat_find_stream_action_ref(AVStream &av_stream) {
	return avformat_find_stream_action(&av_stream);
}

BEFORE_EACH() {
	init_subject("");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;
	app_stderr = actxt.error_stream;

	static AVStream* stream_refs[2] = {&arg_streams[0], &arg_streams[1]};
	static AVCodecParameters codec_parameters[2];

	arg_av_format_context.nb_streams = 2;
	arg_av_format_context.streams = stream_refs;

	arg_streams[0].codecpar = &codec_parameters[0];
	arg_streams[1].codecpar = &codec_parameters[1];

	codec_parameters[0].codec_type = AVMEDIA_TYPE_VIDEO;
	codec_parameters[1].codec_type = AVMEDIA_TYPE_VIDEO;

	arg_track_index = -1;
	arg_track_type = AVMEDIA_TYPE_VIDEO;

	init_mock_function(avformat_find_stream_info);
	init_mock_function(avformat_find_stream_action);
	init_mock_function_with_function(av_get_media_type_string, stub_av_get_media_type_string);
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUBJECT(int) {
	return avformat_find_stream(arg_av_format_context, arg_track_type, arg_track_index, avformat_find_stream_action_ref);
}

static int avformat_find_stream_action_assert_stream(AVStream *stream) {
	CUE_ASSERT_PTR_EQ(stream, &arg_streams[1]);
	return 0;
}

SUITE_CASE("should get stream info by first track of type") {
	init_mock_function_with_function(avformat_find_stream_action, avformat_find_stream_action_assert_stream);
	arg_streams[0].codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
	arg_streams[1].codecpar->codec_type = AVMEDIA_TYPE_AUDIO;
	arg_track_index = -1;
	arg_track_type = AVMEDIA_TYPE_AUDIO;

	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(avformat_find_stream_info);
	CUE_EXPECT_CALLED_WITH_PTR(avformat_find_stream_info, 1, &arg_av_format_context);

	CUE_EXPECT_CALLED_ONCE(avformat_find_stream_action);
}

SUITE_CASE("should output avformat_find_stream_info error message and exit") {
	init_mock_function_with_return(avformat_find_stream_info, -2);

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_EXPECT_NEVER_CALLED(avformat_find_stream_action);

	CUE_ASSERT_STDERR_EQ("Error[liblffmpeg]: -2\n");
}

static int avformat_find_stream_action_assert_stream2(AVStream *stream) {
	CUE_ASSERT_PTR_EQ(stream, &arg_streams[0]);
	return 0;
}

SUITE_CASE("should get stream by specific track") {
	arg_streams[0].codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
	arg_streams[1].codecpar->codec_type = AVMEDIA_TYPE_AUDIO;
	arg_track_index = 0;
	arg_track_type = AVMEDIA_TYPE_VIDEO;

	init_mock_function_with_function(avformat_find_stream_action, avformat_find_stream_action_assert_stream2);

	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(avformat_find_stream_action);
}

SUITE_CASE("no matched media stream") {
	arg_track_index = 3;
	arg_track_type = AVMEDIA_TYPE_VIDEO;

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_ASSERT_STDERR_EQ("Error[liblffmpeg]: video stream 3 doesn't exist\n");
}

SUITE_END(avformat_find_stream_test);

