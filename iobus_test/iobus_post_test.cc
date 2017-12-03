#include <cstdio>
#include <cstdlib>
#include <cunitexd.h>
#include "stdexd/stdexd.h"
#include "iobus/iobus.h"

SUITE_START("iobus_post_test");

BEFORE_EACH() {
	init_subject("");
	app_stdout = actxt.output_stream;
	return 0;
}

AFTER_EACH() {
	return close_subject();
}

SUITE_CASE("post message") {
	iobus iob(app_stdin, app_stdout, app_stderr);

	iob.post("hello");

	CUE_ASSERT_STDOUT_EQ("hello\n");
}

SUITE_END(iobus_post_test);

