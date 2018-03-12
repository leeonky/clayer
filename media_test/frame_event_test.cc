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

SUITE_START("frame_event_test");

mock_function_3(int, frame_event_action, int, int, int64_t);

BEFORE_EACH() {
	init_subject("");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;
	app_stderr = actxt.error_stream;
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUBJECT(int) {
	iobus iob{actxt.input_stream, actxt.output_stream, actxt.error_stream};
	return frame_event(iob, frame_event_action);
}

SUITE_CASE("one frame") {
	init_subject("FRAME buffer:5 1=>0");

	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(frame_event_action);
	CUE_EXPECT_CALLED_WITH_INT(frame_event_action, 1, 5);
	CUE_EXPECT_CALLED_WITH_INT(frame_event_action, 2, 1);
	CUE_EXPECT_CALLED_WITH_INT(frame_event_action, 3, 0);
}

SUITE_CASE("event not match") {
	init_subject("AUDIO 1=>0");

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);
}

SUITE_END(frame_event_test);

