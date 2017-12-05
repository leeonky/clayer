#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include <memory>
#include "stdexd/stdexd.h"
#include "lsdl2/lsdl2.h"
#include "lffmpeg/lffmpeg.h"
#include "media/media.h"
#include "mock_sdl2/mock_sdl2.h"
#include "mock_ffmpeg/mock_ffmpeg.h"

SUITE_START("frames_event_test");

mock_function_1(int, frames_event_action, frame_list*);

int frames_event_action_ref(frame_list &list) {
	return frames_event_action(&list);
}

static std::unique_ptr<iobus> iob;

BEFORE_EACH() {
	init_subject("");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;
	app_stderr = actxt.error_stream;
	iob.reset(new iobus(actxt.input_stream, actxt.output_stream, actxt.error_stream));
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUBJECT(int) {
	return frames_event(*iob, frames_event_action_ref);
}

static int assert_frame_list_0(frame_list *list) {
	CUE_ASSERT_EQ(list->count, 0);
	return 0;
}

SUITE_CASE("enmpty FRAMES") {
	init_subject("FRAMES");
	init_mock_function_with_function(frames_event_action, assert_frame_list_0);

	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(frames_event_action);
}

static int assert_frame_list_1(frame_list *list) {
	CUE_ASSERT_EQ(list->count, 1);
	CUE_ASSERT_EQ(list->frames[0].index, 1);
	CUE_ASSERT_EQ(list->frames[0].timestamp, 0);
	return 0;
}

SUITE_CASE("create sdl texture") {
	init_subject("FRAMES 1=>0");
	init_mock_function_with_function(frames_event_action, assert_frame_list_1);

	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(frames_event_action);
}

//SUITE_CASE("event not match") {
	//init_subject("AUDIO width:1920 height:1080 format:yuv420p");

	//CUE_ASSERT_SUBJECT_FAILED_WITH(-1);
//}

//SUITE_CASE("args not match") {
	//init_subject("VIDEO width:1920 format:yuv420p");

	//CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	//CUE_ASSERT_STDERR_EQ("Error[libiobus]: Invalid VIDEO arguments 'width:1920 format:yuv420p'\n");
//}

SUITE_END(frames_event_test);

