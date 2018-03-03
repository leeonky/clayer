#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include <memory>
#include "stdexd/stdexd.h"
#include "media/media.h"
#include "mem/circular_shm.h"
#include "mock_sys/mock_sys.h"

SUITE_START("buffer_event_test");

mock_function_5(int, buffer_event_action, int, size_t, int, int, int);

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
	return buffer_event(iob, buffer_event_action);
}

SUITE_CASE("load circular_shm") {
	init_subject("BUFFER id:0 size:3112960 count:16 sem:7214 key:10");

	CUE_ASSERT_SUBJECT_SUCCEEDED();

	CUE_EXPECT_CALLED_ONCE(buffer_event_action);
	CUE_EXPECT_CALLED_WITH_INT(buffer_event_action, 1, 0);
	CUE_EXPECT_CALLED_WITH_INT(buffer_event_action, 2, 3112960);
	CUE_EXPECT_CALLED_WITH_INT(buffer_event_action, 3, 16);
	CUE_EXPECT_CALLED_WITH_INT(buffer_event_action, 4, 7214);
	CUE_EXPECT_CALLED_WITH_INT(buffer_event_action, 5, 10);
}

SUITE_CASE("event not match") {
	init_subject("AUDIO id:0 size:3112960 count:16 sem:7214 key:1");

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);
}

SUITE_CASE("args not match") {
	init_subject("BUFFER id:0 size:3112960 count:16");
	app_stdin = actxt.input_stream;
	app_stdout = actxt.output_stream;
	app_stderr = actxt.error_stream;

	CUE_ASSERT_SUBJECT_FAILED_WITH(-1);

	CUE_ASSERT_STDERR_EQ("Error[libiobus]: Invalid BUFFER arguments 'id:0 size:3112960 count:16'\n");
}

SUITE_END(buffer_event_test);

