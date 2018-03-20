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

SUITE_START("control_event_test");

mock_function_1(int, control_event_action, int);

BEFORE_EACH() {
	init_subject("");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;
	app_stderr = actxt.error_stream;
	init_mock_function(control_event_action);
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUBJECT(int) {
	iobus iob{actxt.input_stream, actxt.output_stream, actxt.error_stream};
	return control_event(iob, control_event_action);
}

SUITE_CASE("get control list") {
	init_subject("CONTROL id:100");

	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(control_event_action);
	CUE_EXPECT_CALLED_WITH_INT(control_event_action, 1, 100);
}

SUITE_END(control_event_test);

