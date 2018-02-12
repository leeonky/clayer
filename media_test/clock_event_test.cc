#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include <memory>
#include "stdexd/stdexd.h"
#include "media/media.h"

SUITE_START("clock_event_test");

mock_function_2(int, clock_event_action, int64_t, int64_t);

BEFORE_EACH() {
	init_subject("");
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUBJECT(int) {
	iobus iob{actxt.input_stream, actxt.output_stream, actxt.error_stream};
	return clock_event(iob, clock_event_action);
}

SUITE_CASE("create sdl texture") {
	init_subject("CLOCK base:1518407083772393 offset:33194667");

	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(clock_event_action);
	CUE_EXPECT_CALLED_WITH_INT(clock_event_action, 1, 1518407083772393LL);
	CUE_EXPECT_CALLED_WITH_INT(clock_event_action, 2, 33194667);

	CUE_ASSERT_STDERR_EQ("");
}

SUITE_END(clock_event_test);


