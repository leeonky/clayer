#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include "stdexd/stdexd.h"
#include "mock_ffmpeg/mock_ffmpeg.h"
#include "lffmpeg/lffmpeg.h"

SUITE_START("avformat_open_close_test");

static AVFormatContext ret_format_context;
static int stub_avformat_open_input(AVFormatContext **ps, const char *, AVInputFormat *, AVDictionary **) {
	*ps = &ret_format_context;
	return 0;
}

static const char *arg_input_file;

mock_function_1(int, avformat_open_action, AVFormatContext *);

static int avformat_open_action_ref(AVFormatContext &f) {
	return avformat_open_action(&f);
}

BEFORE_EACH() {
	init_subject("");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;
	app_stderr = actxt.error_stream;

	arg_input_file = "test.avi";

	init_mock_function(av_register_all);
	init_mock_function_with_function(avformat_open_input, stub_avformat_open_input);
	init_mock_function(avformat_find_stream_info);
	init_mock_function(avformat_close_input);

	init_mock_function(avformat_open_action);
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUBJECT(int) {
	return avformat_open_input(arg_input_file, avformat_open_action_ref);
}

SUITE_CASE("should make sure open and close stream file") {
	init_mock_function_with_function(avformat_open_input, stub_avformat_open_input);

	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(av_register_all);

	CUE_EXPECT_CALLED_ONCE(avformat_open_input);
	CUE_EXPECT_CALLED_WITH_STRING(avformat_open_input, 2, arg_input_file);

	CUE_EXPECT_CALLED_ONCE(avformat_find_stream_info);
	CUE_EXPECT_CALLED_WITH_PTR(avformat_find_stream_info, 1, &ret_format_context);

	CUE_EXPECT_CALLED_ONCE(avformat_open_action);
	CUE_EXPECT_CALLED_WITH_PTR(avformat_open_action, 1, &ret_format_context);

	CUE_EXPECT_CALLED_ONCE(avformat_close_input);
}

SUITE_CASE("should output avformat_open_input error message and exit") {
	init_mock_function_with_return(avformat_open_input, -2);

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_EXPECT_NEVER_CALLED(avformat_find_stream_info);

	CUE_EXPECT_NEVER_CALLED(avformat_open_action);

	CUE_EXPECT_NEVER_CALLED(avformat_close_input);

	CUE_ASSERT_STDERR_EQ("Error[liblffmpeg]: -2\n");
}

SUITE_CASE("should output avformat_find_stream_info error message and exit") {
	init_mock_function_with_return(avformat_find_stream_info, -2);

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_EXPECT_NEVER_CALLED(avformat_open_action);

	CUE_EXPECT_CALLED_ONCE(avformat_close_input);

	CUE_ASSERT_STDERR_EQ("Error[liblffmpeg]: -2\n");
}

SUITE_END(avformat_open_close_test);

/*static int ffmpeg_open_action_assert_format_context(ffmpeg_stream *stream) {*/
	/*CUE_ASSERT_PTR_EQ(stream->av_format_context, &ret_format_context);*/
	/*return 100;*/
/*}*/

/*SUITE_CASE("call block and return the return of block") {*/
	/*init_mock_function_with_function(ffmpeg_open_action, ffmpeg_open_action_assert_format_context)*/

	/*CUE_ASSERT_SUBJECT_FAILED_WITH(100);*/
/*}*/

/*static int ffmpeg_open_action_assert_stream(ffmpeg_stream *stream) {*/
	/*CUE_ASSERT_PTR_EQ(stream->av_stream, &arg_streams[1]);*/
	/*return 0;*/
/*}*/

/*SUITE_CASE("should get stream info by first track of type") {*/
	/*arg_streams[0].codecpar->codec_type = AVMEDIA_TYPE_VIDEO;*/
	/*arg_streams[1].codecpar->codec_type = AVMEDIA_TYPE_AUDIO;*/
	/*ret_format_context.nb_streams = 2;*/
	/*arg_track_index = -1;*/
	/*arg_track_type = AVMEDIA_TYPE_AUDIO;*/
	/*init_mock_function_with_function(ffmpeg_open_action, ffmpeg_open_action_assert_stream)*/

	/*CUE_ASSERT_SUBJECT_SUCCEEDED();*/

	/*CUE_EXPECT_CALLED_ONCE(av_init_packet);*/

	/*CUE_EXPECT_CALLED_ONCE(av_packet_unref);*/

	/*CUE_ASSERT_PTR_EQ(params_of(av_init_packet, 1), params_of(av_packet_unref, 1));*/
/*}*/

/*static int ffmpeg_open_action_assert_stream2(ffmpeg_stream *stream) {*/
	/*CUE_ASSERT_PTR_EQ(stream->av_stream, &arg_streams[0]);*/
	/*return 0;*/
/*}*/

/*SUITE_CASE("should get stream by specific track") {*/
	/*arg_streams[0].codecpar->codec_type = AVMEDIA_TYPE_VIDEO;*/
	/*arg_streams[1].codecpar->codec_type = AVMEDIA_TYPE_AUDIO;*/
	/*ret_format_context.nb_streams = 2;*/
	/*arg_track_index = 0;*/
	/*arg_track_type = AVMEDIA_TYPE_VIDEO;*/

	/*init_mock_function_with_function(ffmpeg_open_action, ffmpeg_open_action_assert_stream2);*/

	/*CUE_ASSERT_SUBJECT_SUCCEEDED();*/

	/*CUE_EXPECT_CALLED_ONCE(ffmpeg_open_action);*/
/*}*/

/*SUITE_CASE("no matched media stream") {*/
	/*arg_track_index = 3;*/
	/*arg_track_type = AVMEDIA_TYPE_VIDEO;*/

	/*CUE_ASSERT_SUBJECT_FAILED_WITH(-1);*/

	/*CUE_ASSERT_STDERR_EQ("Error[libwrpffp]: video stream 3 doesn't exist\n");*/
/*}*/


